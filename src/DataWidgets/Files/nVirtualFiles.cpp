#include <datawidgets.h>

N::VirtualFiles:: VirtualFiles ( QWidget * parent , Plan * p )
                : TreeDock     (           parent ,        p )
                , VirtualIO    (                             )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VirtualFiles::~VirtualFiles(void)
{
}

QMimeData * N::VirtualFiles::dragMime (void)
{
  return NULL ;
}

bool N::VirtualFiles::hasItem(void)
{
  return false ;
}

bool N::VirtualFiles::startDrag(QMouseEvent * event)
{
  return false ;
}

bool N::VirtualFiles::fetchDrag(QMouseEvent * event)
{
  return false ;
}

void N::VirtualFiles::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::VirtualFiles::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::VirtualFiles::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return false ;
}

bool N::VirtualFiles::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return false ;
}

void N::VirtualFiles::Configure(void)
{
  UnpackMode = 0                                            ;
  ProgressID = -1                                           ;
  Unpacking  = false                                        ;
  ///////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                     ) ;
  head -> setText              ( 0 , tr("Filename"     )  ) ;
  head -> setText              ( 1 , tr("Suffix"       )  ) ;
  head -> setText              ( 2 , tr("Type"         )  ) ;
  head -> setText              ( 3 , tr("Owner"        )  ) ;
  head -> setText              ( 4 , tr("Group"        )  ) ;
  head -> setText              ( 5 , tr("Mode"         )  ) ;
  head -> setText              ( 6 , tr("Size"         )  ) ;
  head -> setText              ( 7 , tr("Last modified")  ) ;
  head -> setText              ( 8 , tr("Checksum"     )  ) ;
  head -> setText              ( 9 , ""                   ) ;
  for (int i=0;i<8;i++)                                     {
    head -> setTextAlignment   ( i , Qt::AlignCenter      ) ;
  }                                                         ;
  setWindowTitle               ( tr("Virtual filesystem") ) ;
  setDragDropMode              ( NoDragDrop               ) ;
  setRootIsDecorated           ( true                     ) ;
  setAlternatingRowColors      ( true                     ) ;
  setSelectionMode             ( ExtendedSelection        ) ;
  setColumnCount               ( 10                       ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded    ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded    ) ;
  setHeaderItem                ( head                     ) ;
  plan -> setFont              ( this                     ) ;
  ///////////////////////////////////////////////////////////
  setAlignment ( 5 , Qt::AlignRight | Qt::AlignVCenter    ) ;
  setAlignment ( 6 , Qt::AlignRight | Qt::AlignVCenter    ) ;
  setAlignment ( 7 , Qt::AlignRight | Qt::AlignVCenter    ) ;
  setAlignment ( 8 , Qt::AlignRight | Qt::AlignVCenter    ) ;
  ///////////////////////////////////////////////////////////
  ImageFilters    = Images::ExtensionFilters()              ;
  CppFilters     << "c"                                     ;
  CppFilters     << "cc"                                    ;
  CppFilters     << "cpp"                                   ;
  CppFilters     << "c++"                                   ;
  CppFilters     << "h"                                     ;
  CppFilters     << "hh"                                    ;
  CppFilters     << "hpp"                                   ;
  CppFilters     << "h++"                                   ;
  ///////////////////////////////////////////////////////////
  connect(this,SIGNAL(Arrange    ())                        ,
          this,SLOT  (Arrangement())                      ) ;
  connect(this,SIGNAL(Unmessage    (QString))               ,
          this,SLOT  (statusMessage(QString))             ) ;
}

bool N::VirtualFiles::FocusIn(void)
{
  LinkAction ( Save , Save() ) ;
  return true                  ;
}

void N::VirtualFiles::Arrangement(void)
{
  SuitableColumns ( ) ;
}

void N::VirtualFiles::statusMessage(QString msg)
{
  plan->showMessage(msg);
}

bool N::VirtualFiles::startup(QFileInfo & F)
{
  SourceFI = F                                   ;
  nKickOut         ( Unpacking         , false ) ;
  nKickOut         ( !acceptable ( F ) , false ) ;
  setFileInfo      (  F                        ) ;
  nKickOut         ( !Detect     (   ) , false ) ;
  UnpackMode = 1                                 ;
  Unpacking  = true                              ;
  setEnabled       ( false                     ) ;
  clear            (                           ) ;
  DirItems . clear (                           ) ;
  start            (                           ) ;
  return true                                    ;
}

void N::VirtualFiles::run(void)
{
  switch (UnpackMode)             {
    case 1                        :
      List         (            ) ;
      Alert        ( Done       ) ;
      if (NotNull(file))          {
        delete file               ;
        file = NULL               ;
      }                           ;
      emit Arrange ( )            ;
    break                         ;
    case 2                        :
      Unpack       (            ) ;
      Alert        ( Done       ) ;
      CurrentSize = TotalSize     ;
      plan->Finish ( ProgressID ) ;
      ProgressID = -1             ;
      if (NotNull(file))          {
        delete file               ;
        file = NULL               ;
      }                           ;
    break                         ;
  }                               ;
  setEnabled ( true )             ;
  UnpackMode = 0                  ;
  Unpacking  = false              ;
}

void N::VirtualFiles::resetSize(void)
{
  switch (UnpackMode)  {
    case 1             :
      TotalSize   = 0  ;
      TotalFiles  = 0  ;
    break              ;
    case 2             :
      CurrentSize = 0  ;
    break              ;
  }                    ;
}

void N::VirtualFiles::appendSize(int size)
{
  switch (UnpackMode)     {
    case 1                :
      TotalSize   += size ;
      TotalFiles  ++      ;
    break                 ;
    case 2                :
      CurrentSize += size ;
    break                 ;
  }                       ;
}

void N::VirtualFiles::appendFile(FileInfo & F)
{
  switch (UnpackMode)  {
    case 1             :
      files <<     F   ;
      ListFile   ( F ) ;
    break              ;
    case 2             :
      files <<     F   ;
      UnpackItem ( F ) ;
    break              ;
  }                    ;
}

QTreeWidgetItem * N::VirtualFiles::setDirectory(FileInfo & F)
{
  QString fp = F.FullPath()                      ;
  ////////////////////////////////////////////////
  QStringList R = fp . split ('/')               ;
  QStringList X                                  ;
  nFullLoop ( i , R . count() )                  {
    QString r = R[i]                             ;
    if (r.length()>0) X << r                     ;
  }                                              ;
  if (X.count()>0) fp = X.join("/")              ;
              else fp = ""                       ;
  if (fp.length()<=0) return NULL                ;
  ////////////////////////////////////////////////
  if (DirItems.contains(fp))                     {
    return DirItems[fp]                          ;
  }                                              ;
  QTreeWidgetItem * ptem = getDirectory(F)       ;
  QString path = fp                              ;
  QString ft                                     ;
  QString fm                                     ;
  NewTreeWidgetItem  ( item )                    ;
  fp = X.last()                                  ;
  ft = F.Lastest.toString("yyyy/MM/dd hh:mm:ss") ;
  fm = QString::number(F.mode,8)                 ;
  item -> setText   ( 0 , fp              )      ;
  item -> setText   ( 2 , tr("Directory") )      ;
  item -> setText   ( 3 , F.uname         )      ;
  item -> setText   ( 4 , F.gname         )      ;
  item -> setText   ( 5 , fm              )      ;
  item -> setText   ( 7 , ft              )      ;
  ////////////////////////////////////////////////
  item->setIcon(0,QIcon(":/images/folder.png"))  ;
  ////////////////////////////////////////////////
  DirItems[path] = item                          ;
  setAlignments       ( item )                   ;
  if (IsNull(ptem))                              {
    addTopLevelItem   ( item )                   ;
  } else                                         {
    ptem->addChild    ( item )                   ;
    ptem->setExpanded ( true )                   ;
  }                                              ;
  return item                                    ;
}

QTreeWidgetItem * N::VirtualFiles::getDirectory(FileInfo & F)
{
  QString fp = F.FullPath()                      ;
  ////////////////////////////////////////////////
  QStringList R = fp . split ('/')               ;
  QStringList X                                  ;
  nFullLoop ( i , R . count() )                  {
    QString r = R[i]                             ;
    if (r.length()>0) X << r                     ;
  }                                              ;
  R = X                                          ;
  if (X.count()>0) X . takeLast ( )              ;
  if (X.count()>0) fp = X.join("/")              ;
              else fp = ""                       ;
  if (fp.length()<=0) return NULL                ;
  ////////////////////////////////////////////////
  QString path = fp                              ;
  if (path.length()<=0) return NULL              ;
  if (DirItems.contains(path))                   {
    return DirItems[path]                        ;
  }                                              ;
  FileInfo FX                                    ;
  QString ft                                     ;
  FX = F                                         ;
  FX . Filename = path                           ;
  FX . Prefix   = ""                             ;
  QTreeWidgetItem * ptem = getDirectory(FX)      ;
  NewTreeWidgetItem  ( item )                    ;
  ft = F.Lastest.toString("yyyy/MM/dd hh:mm:ss") ;
  item -> setText   ( 0 , X.last()        )      ;
  item -> setText   ( 2 , tr("Directory") )      ;
  item -> setText   ( 3 , F.uname         )      ;
  item -> setText   ( 4 , F.gname         )      ;
  item -> setText   ( 7 , ft              )      ;
  ////////////////////////////////////////////////
  item->setIcon(0,QIcon(":/images/folder.png"))  ;
  ////////////////////////////////////////////////
  DirItems[path] = item                          ;
  setAlignments       ( item )                   ;
  if (IsNull(ptem))                              {
    addTopLevelItem   ( item )                   ;
  } else                                         {
    ptem->addChild    ( item )                   ;
    ptem->setExpanded ( true )                   ;
  }                                              ;
  return item                                    ;
}

QTreeWidgetItem * N::VirtualFiles::getEntry(FileInfo & F)
{
  QString m                                      ;
  QString sf                                     ;
  QString sl                                     ;
  QString tn                                     ;
  QString fm                                     ;
  QString fs                                     ;
  QString ft                                     ;
  QString fc                                     ;
  int     checksum = 0                           ;
  bool    okay     = true                        ;
  NewTreeWidgetItem  ( item )                    ;
  m  = F.FullPath()                              ;
  QFileInfo SF(m)                                ;
  sf = SF.suffix()                               ;
  fm = QString::number(F.mode,8)                 ;
  fs = QString::number(F.size  )                 ;
  ft = F.Lastest.toString("yyyy/MM/dd hh:mm:ss") ;
  checksum = F.CheckSum.toInt(&okay,8)           ;
  if (okay)                                      {
    fc = QString::number(checksum)               ;
  }                                              ;
  ////////////////////////////////////////////////
  QString il                                     ;
  sl = sf.toLower()                              ;
  il = QString("*.%1").arg(sl)                   ;
  if (ImageFilters.contains(il))                 {
    tn = tr("Image file")                        ;
    item->setIcon                                (
      0                                          ,
      QIcon(":/images/pictures.png")           ) ;
  } else
  if (CppFilters.contains(sl))                   {
    tn = tr("C/C++")                             ;
    item->setIcon                                (
      0                                          ,
      QIcon(":/images/sourcecpp.png")          ) ;
  } else
  if (sl=="m4")                                  {
    tn = tr("M4 Macro language")                 ;
  } else
  if (sl=="y")                                   {
    tn = tr("YACC")                              ;
  } else
  if (sl=="f")                                   {
    tn = tr("Fortran")                           ;
  } else
  if (sl=="po")                                  {
    tn = tr("Unix translation file")             ;
  } else
  if (sl=="ts")                                  {
    tn = tr("Qt translation source")             ;
  } else
  if (sl=="qm")                                  {
    tn = tr("Qt translation file")               ;
  } else
  if (sl=="txt")                                 {
    tn = tr("Plain text file")                   ;
    item->setIcon                                (
      0                                          ,
      QIcon(":/images/paper.png")              ) ;
  } else
  if (sl=="sql")                                 {
    tn = tr("Database file")                     ;
    item->setIcon                                (
      0                                          ,
      QIcon(":/images/odbc.png")               ) ;
  }                                              ;
  ////////////////////////////////////////////////
  item -> setText ( 0 , SF.fileName() )          ;
  item -> setText ( 1 , sf            )          ;
  item -> setText ( 2 , tn            )          ;
  item -> setText ( 3 , F.uname       )          ;
  item -> setText ( 4 , F.gname       )          ;
  item -> setText ( 5 , fm            )          ;
  item -> setText ( 6 , fs            )          ;
  item -> setText ( 7 , ft            )          ;
  item -> setText ( 8 , fc            )          ;
  ////////////////////////////////////////////////
  setAlignments   ( item              )          ;
  return item                                    ;
}

void N::VirtualFiles::ListFile(FileInfo & F)
{
  if (F.Type==FileInfo::Directory)              {
    setDirectory(F)                             ;
  } else                                        {
    QTreeWidgetItem * item = getEntry     ( F ) ;
    QTreeWidgetItem * ptem = getDirectory ( F ) ;
    if (IsNull(ptem))                           {
      addTopLevelItem   ( item )                ;
    } else                                      {
      ptem->addChild    ( item )                ;
      ptem->setExpanded ( true )                ;
    }                                           ;
  }                                             ;
  DoProcessEvents                               ;
}

void N::VirtualFiles::UnpackItem(FileInfo & F)
{
  QString m = F.FullPath() ;
  emit Unmessage(m)        ;
}

void N::VirtualFiles::Save(void)
{
  nDropOut ( Unpacking )                                   ;
  QString path                                             ;
  path = QFileDialog::getExistingDirectory                 (
           this                                            ,
           tr("Unpack to directory")                       ,
           plan->Temporary("")                           ) ;
  if (path.length()>0)                                     {
    directory.setPath(path)                                ;
    nDropOut    ( !acceptable ( SourceFI )               ) ;
    setFileInfo (  SourceFI                              ) ;
    nDropOut    ( !Detect     (          )               ) ;
    UnpackMode = 2                                         ;
    Unpacking  = true                                      ;
    ProgressID = plan->Progress(windowTitle(),tr("%v/%m")) ;
    CurrentSize = 0                                        ;
    plan->setRange ( ProgressID,           0, TotalSize  ) ;
    plan->Start    ( ProgressID,&CurrentSize,&Unpacking  ) ;
    plan->setRange ( ProgressID,           0, TotalSize  ) ;
    setEnabled  ( false )                                  ;
    DoProcessEvents                                        ;
    start       (       )                                  ;
  } else                                                   {
    Alert ( Error )                                        ;
  }                                                        ;
}

bool N::VirtualFiles::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow   * mdi  = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget     * dock = Casting ( QDockWidget   , parent() ) ;
  QTreeWidgetItem * item = itemAt  ( pos                      ) ;
  QAction    * aa                                               ;
  if (!Unpacking)                                               {
    mm . add ( 101 , tr("Save") )                               ;
  }                                                             ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 101                                                    :
      Save ( )                                                  ;
    break                                                       ;
    case 1000001                                                :
      emit attachMdi ( this , 0 )                               ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::BottomDockWidgetArea                                ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}
