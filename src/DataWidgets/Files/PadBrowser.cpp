#include <datawidgets.h>
#include "ui_PadBrowser.h"

N::PadBrowser:: PadBrowser ( QWidget      * parent , Plan * p )
              : Splitter   ( Qt::Vertical , parent ,        p )
              , ui         ( new Ui::PadBrowser               )
              , Join       ( QIcon(":/icons/join.png" )       )
              , Flags      ( 0xFF                             )
{
  Configure ( ) ;
}

N::PadBrowser::~PadBrowser(void)
{
  delete ui ;
}

QSize N::PadBrowser::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize ( 1280 ,  900 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::PadBrowser::Configure(void)
{
  NewTreeWidgetItem          ( head                )                          ;
  QFont f = plan -> fonts    [ Fonts :: ListView   ]                          ;
  ui -> setupUi              ( this                )                          ;
  head -> setText            ( 0 , tr("Name")      )                          ;
  head -> setText            ( 1 , tr("Type")      )                          ;
  head -> setText            ( 2 , tr("Size")      )                          ;
  head -> setText            ( 3 , ""              )                          ;
  for (int i=0;i<4;i++)                                                       {
    head -> setTextAlignment ( i , Qt::AlignCenter )                          ;
    head -> setFont          ( i , f               )                          ;
  }                                                                           ;
  ui->Files->setHeaderItem   ( head                )                          ;
  /////////////////////////////////////////////////////////////////////////////
  addAction (  0 , tr( "Refresh" ) , QIcon(":/images/refreshtopleft.png") )   ;
  addAction (  1 , tr( "Okay"    ) , QIcon(":/icons/pick.png"           ) )   ;
  addAction (  2 , tr( "Up"      ) , QIcon(":/icons/cdup.png"           ) )   ;
  addAction (  3 , tr( "cd"      ) , QIcon(":/icons/next.png"           ) )   ;
  addAction (  4 , tr( "New"     ) , QIcon(":/icons/new.png"            ) )   ;
  addAction (  5 , tr( "Delete"  ) , QIcon(":/icons/remove.png"         ) )   ;
  addAction (  6 , tr( "Rename"  ) , QIcon(":/icons/rename.png"         ) )   ;
  addAction (  7 , tr( "Edit"    ) , QIcon(":/images/edit.png"          ) )   ;
  addAction (  8 , tr( "New"     ) , QIcon(":/images/new.png"           ) )   ;
  addAction (  9 , tr( "Rename"  ) , QIcon(":/images/rename.png"        ) )   ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 0 ] [  0 ] = true                                                ;
  Displays [ 0 ] [  1 ] = true                                                ;
  Displays [ 0 ] [  2 ] = true                                                ;
  Displays [ 0 ] [  3 ] = true                                                ;
  Displays [ 0 ] [  4 ] = true                                                ;
  Displays [ 0 ] [  5 ] = true                                                ;
  Displays [ 0 ] [  6 ] = true                                                ;
  Displays [ 0 ] [  7 ] = false                                               ;
  Displays [ 0 ] [  8 ] = false                                               ;
  Displays [ 0 ] [  9 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 1 ] [  0 ] = false                                               ;
  Displays [ 1 ] [  1 ] = false                                               ;
  Displays [ 1 ] [  2 ] = false                                               ;
  Displays [ 1 ] [  3 ] = false                                               ;
  Displays [ 1 ] [  4 ] = false                                               ;
  Displays [ 1 ] [  5 ] = false                                               ;
  Displays [ 1 ] [  6 ] = false                                               ;
  Displays [ 1 ] [  7 ] = true                                                ;
  Displays [ 1 ] [  8 ] = true                                                ;
  Displays [ 1 ] [  9 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 2 ] [  0 ] = false                                               ;
  Displays [ 2 ] [  1 ] = false                                               ;
  Displays [ 2 ] [  2 ] = false                                               ;
  Displays [ 2 ] [  3 ] = false                                               ;
  Displays [ 2 ] [  4 ] = false                                               ;
  Displays [ 2 ] [  5 ] = false                                               ;
  Displays [ 2 ] [  6 ] = false                                               ;
  Displays [ 2 ] [  7 ] = true                                                ;
  Displays [ 2 ] [  8 ] = false                                               ;
  Displays [ 2 ] [  9 ] = true                                                ;
  /////////////////////////////////////////////////////////////////////////////
  nConnect ( ui->Back  , SIGNAL ( clicked     (                      ) )      ,
             this      , SIGNAL ( Back        (                      ) )    ) ;
  nConnect ( ui->Line  , SIGNAL ( returnPressed (                    ) )      ,
             this      , SLOT   ( NewDirectory  (                    ) )    ) ;
  nConnect ( ui->Files , SIGNAL ( itemClicked ( QTreeWidgetItem*,int ) )      ,
             this      , SLOT   ( itemClicked ( QTreeWidgetItem*,int ) )    ) ;
  /////////////////////////////////////////////////////////////////////////////
  nConnect ( Actions [  0 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( List          ( ) )                  ) ;
  nConnect ( Actions [  1 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( Okay          ( ) )                  ) ;
  nConnect ( Actions [  2 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( Up            ( ) )                  ) ;
  nConnect ( Actions [  3 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( Cd            ( ) )                  ) ;
  nConnect ( Actions [  4 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( New           ( ) )                  ) ;
  nConnect ( Actions [  5 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( Delete        ( ) )                  ) ;
  nConnect ( Actions [  6 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( Rename        ( ) )                  ) ;
  nConnect ( Actions [  7 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( Edit          ( ) )                  ) ;
  nConnect ( Actions [  8 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( NewDirectory  ( ) )                  ) ;
  nConnect ( Actions [  9 ] , SIGNAL ( triggered     ( ) )                    ,
             this           , SLOT   ( RenameFinished( ) )                  ) ;
  /////////////////////////////////////////////////////////////////////////////
  plan    -> setFont ( ui -> Message )                                        ;
  plan    -> setFont ( ui -> Line    )                                        ;
  plan    -> setFont ( ui -> Files   )                                        ;
  Display            ( 0             )                                        ;
}

bool N::PadBrowser::doFile(void)
{
  return ( ( Flags & 0x01 ) > 0 ) ;
}

bool N::PadBrowser::doDirectory(void)
{
  return ( ( Flags & 0x02 ) > 0 ) ;
}

bool N::PadBrowser::doPick(void)
{
  return ( ( Flags & 0x04 ) > 0 ) ;
}

void N::PadBrowser::setAlignments(QTreeWidgetItem * item)
{
  item -> setTextAlignment ( 2 , Qt::AlignRight | Qt::AlignVCenter ) ;
}

void N::PadBrowser::addAction(int Id,QString text,QIcon icon)
{
  Actions [ Id ] = new QAction ( icon , text , ui->Tools ) ;
  ui -> Tools    -> addAction  ( Actions [ Id ]          ) ;
  Actions [ Id ] -> setFont    ( ui -> Tools -> font ( ) ) ;
}

void N::PadBrowser::Display(int conf)
{
  BMAPs D   = Displays [ conf ]               ;
  CUIDs IDs = D . keys (      )               ;
  int   id                                    ;
  foreach (id,IDs)                            {
    Actions [ id ] -> setVisible ( D [ id ] ) ;
  }                                           ;
}

void N::PadBrowser::setFlags(int flags)
{
  Flags = flags ;
}

void N::PadBrowser::setBack(QString text)
{
  ui -> Back -> setText ( text ) ;
}

void N::PadBrowser::setAnswer(bool enable)
{
  Displays [ 0 ] [ 1 ] = enable ;
}

void N::PadBrowser::itemClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( column )                                              ;
  if (!doPick()) return                                            ;
  int flag = item->data(0,Qt::UserRole).toInt()                    ;
  int type = item->data(1,Qt::UserRole).toInt()                    ;
  if (doPick())                                                    {
    flag    ^= 1                                                   ;
    item    -> setData ( 0 , Qt::UserRole , flag )                 ;
    ////////////////////////////////////////////////////////////////
    switch (flag)                                                  {
      case 0                                                       :
        item -> setIcon ( 0 , QIcon(":/icons/empty.png") )         ;
      break                                                        ;
      case 1                                                       :
        item -> setIcon ( 0 , Join                       )         ;
      break                                                        ;
    }                                                              ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  switch (type)                                                    {
    case 0                                                         :
      Actions  [ 2 ] -> setEnabled   ( false )                     ;
      Actions  [ 3 ] -> setEnabled   ( false )                     ;
    break                                                          ;
    case 1                                                         :
      Actions  [ 3 ] -> setEnabled   ( true  )                     ;
      if (Root.absoluteFilePath("")==Current.absoluteFilePath("")) {
        Actions  [ 2 ] -> setEnabled ( false )                     ;
      } else                                                       {
        Actions  [ 2 ] -> setEnabled ( true  )                     ;
      }                                                            ;
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  Alert ( Click )                                                  ;
  emit selectionChanged ( )                                        ;
}

void N::PadBrowser::List(void)
{
  QStringList L                                                            ;
  bool        topmost                                                      ;
  QString     rp = Root    . absoluteFilePath ( "" )                       ;
  QString     cp = Current . absoluteFilePath ( "" )                       ;
  topmost = ( rp == cp )                                                   ;
  //////////////////////////////////////////////////////////////////////////
  setFocus ( Qt::TabFocusReason )                                          ;
  emit Listing          ( )                                                ;
  plan -> processEvents ( )                                                ;
  //////////////////////////////////////////////////////////////////////////
  ui -> Files -> blockSignals ( true  )                                    ;
  ui -> Files -> clear        (       )                                    ;
  if ( topmost && ( TopLevels . count ( ) > 0 ) )                          {
    QString s                                                              ;
    foreach ( s , TopLevels )                                              {
      QString   p = Root . absoluteFilePath ( s )                          ;
      QFileInfo F ( p )                                                    ;
      if (F.exists())                                                      {
        NewTreeWidgetItem ( it )                                           ;
        it -> setIcon   ( 0 , QIcon(":/icons/empty.png") )                 ;
        it -> setText   ( 0 , s                          )                 ;
        it -> setData   ( 0 , Qt::UserRole , 0           )                 ;
        it -> setText   ( 1 , tr("Directory")            )                 ;
        it -> setData   ( 1 , Qt::UserRole , 1           )                 ;
        it -> setText   ( 2 , ""                         )                 ;
        setAlignments   ( it                             )                 ;
        ui   -> Files -> addTopLevelItem ( it            )                 ;
        plan -> processEvents ( )                                          ;
        L << F . absoluteFilePath ( )                                      ;
      }                                                                    ;
    }                                                                      ;
  } else                                                                   {
    if (doDirectory())                                                     {
      QFileInfo     F                                                      ;
      QFileInfoList dirs  = Current . entryInfoList ( QDir::Dirs           |
                                                      QDir::NoDotAndDotDot |
                                                      QDir::NoSymLinks   ) ;
      foreach ( F , dirs  )                                                {
        NewTreeWidgetItem ( it )                                           ;
        QString p = F.absoluteFilePath()                                   ;
        QString f = p                                                      ;
        p = Current.relativeFilePath(p)                                    ;
        f = Root   .relativeFilePath(f)                                    ;
        it -> setIcon   ( 0 , QIcon(":/icons/empty.png") )                 ;
        it -> setText   ( 0 , p                          )                 ;
        it -> setData   ( 0 , Qt::UserRole , 0           )                 ;
        it -> setText   ( 1 , tr("Directory")            )                 ;
        it -> setData   ( 1 , Qt::UserRole , 1           )                 ;
        it -> setText   ( 2 , ""                         )                 ;
        setAlignments   ( it                             )                 ;
        ui   -> Files -> addTopLevelItem ( it            )                 ;
        plan -> processEvents ( )                                          ;
        L << F . absoluteFilePath ( )                                      ;
      }                                                                    ;
    }                                                                      ;
    if (doFile     ())                                                     {
      QFileInfo     F                                                      ;
      QFileInfoList files = Current . entryInfoList ( QDir::Files          |
                                                      QDir::NoDotAndDotDot |
                                                      QDir::NoSymLinks   ) ;
      foreach ( F , files )                                                {
        NewTreeWidgetItem ( it )                                           ;
        QString p = F.absoluteFilePath()                                   ;
        p = Current.relativeFilePath(p)                                    ;
        it -> setIcon   ( 0 , QIcon(":/icons/empty.png") )                 ;
        it -> setText   ( 0 , p                          )                 ;
        it -> setData   ( 0 , Qt::UserRole , 0           )                 ;
        it -> setText   ( 1 , tr("File")                 )                 ;
        it -> setData   ( 1 , Qt::UserRole , 0           )                 ;
        it -> setText   ( 2 , QString::number(F.size())  )                 ;
        setAlignments   ( it                             )                 ;
        ui   -> Files -> addTopLevelItem ( it            )                 ;
        plan -> processEvents ( )                                          ;
        L << F . absoluteFilePath ( )                                      ;
      }                                                                    ;
    }                                                                      ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  for (int i=0;i<4;i++)                                                    {
    ui -> Files -> resizeColumnToContents ( i )                            ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  emit Folders ( topmost , L )                                             ;
  emit Ready   (             )                                             ;
  plan -> processEvents (    )                                             ;
  ui -> Files -> blockSignals ( false )                                    ;
  //////////////////////////////////////////////////////////////////////////
  if (topmost)                                                             {
    ui -> Message -> setText ( tr("Topmost directory") )                   ;
    Actions [ 2 ] -> setEnabled ( false )                                  ;
    Actions [ 3 ] -> setEnabled ( false )                                  ;
  } else                                                                   {
    Actions [ 2 ] -> setEnabled ( true  )                                  ;
    Actions [ 3 ] -> setEnabled ( false )                                  ;
    rp = Root . relativeFilePath ( cp )                                    ;
    ui -> Message -> setText ( rp                      )                   ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  Alert        ( Error       )                                             ;
}

void N::PadBrowser::DeletePath(QDir & root,QStringList files)
{
  QString S                                        ;
  foreach (S,files)                                {
    QFileInfo F ( S )                              ;
    if (F.isDir())                                 {
      QDir          D ( S )                        ;
      QFileInfoList L                              ;
      QStringList   M                              ;
      QString       p = S                          ;
      p = root.relativeFilePath(p)                 ;
      L = D.entryInfoList                          (
            QDir::Dirs                             |
            QDir::Files                            |
            QDir::NoDotAndDotDot                   |
            QDir::NoSymLinks                     ) ;
      for (int i=0;i<L.count();i++)                {
        M << L[i].absoluteFilePath()               ;
      }                                            ;
      DeletePath    ( root , M )                   ;
      ui->Message->setText(tr("Delete %1").arg(p)) ;
      root . rmpath ( S        )                   ;
    } else
    if (F.isFile())                                {
      QString p = S                                ;
      p = root.relativeFilePath(p)                 ;
      ui->Message->setText(tr("Delete %1").arg(p)) ;
      QFile::remove(S)                             ;
    }                                              ;
    plan -> processEvents ( )                      ;
  }                                                ;
}

void N::PadBrowser::Okay(void)
{
  QStringList L                                      ;
  for (int i=0;i<ui->Files->topLevelItemCount();i++) {
    QTreeWidgetItem * it                             ;
    it = ui->Files->topLevelItem(i)                  ;
    if (it->data(0,Qt::UserRole).toInt()==1)         {
      QString n = it->text(0)                        ;
      QString p = Current.absoluteFilePath(n)        ;
      L << p                                         ;
    }                                                ;
  }                                                  ;
  emit SelectFiles      ( L )                        ;
  emit Back             (   )                        ;
  plan -> processEvents (   )                        ;
}

void N::PadBrowser::Up(void)
{
  QString rp = Root    . absoluteFilePath ( "" ) ;
  QString cp = Current . absoluteFilePath ( "" ) ;
  if ( rp == cp ) return                         ;
  Current . cdUp ( )                             ;
  List           ( )                             ;
}

void N::PadBrowser::Cd(void)
{
  QTreeWidgetItem * item = ui->Files->currentItem() ;
  if (IsNull(item)) return                          ;
  QString path = item->text(0)                      ;
  if (path.length()<=0) return                      ;
  Current . cd ( path )                             ;
  List         (      )                             ;
}

void N::PadBrowser::New(void)
{
  ui -> Line  -> blockSignals     ( true               ) ;
  ui -> Line  -> clear            (                    ) ;
  ui -> Line  -> blockSignals     ( false              ) ;
  ui -> Label -> setCurrentWidget ( ui -> Line         ) ;
  ui -> Edit  -> setCurrentWidget ( ui -> Files        ) ;
  Display                         ( 1                  ) ;
  ui -> Line  -> setFocus         ( Qt::TabFocusReason ) ;
}

void N::PadBrowser::NewDirectory(void)
{
  QString n = ui->Line->text()                     ;
  if (n.length()>0)                                {
    QString ph = Current . absoluteFilePath ( n  ) ;
    QString rp = Root    . absoluteFilePath ( "" ) ;
    if (ph.contains(rp))                           {
      Current . mkdir ( n )                        ;
    }                                              ;
  }                                                ;
  Edit ( )                                         ;
  List ( )                                         ;
}

void N::PadBrowser::Delete(void)
{
  QTreeWidgetItem * item = ui->Files->currentItem() ;
  if (IsNull(item)) return                          ;
  QString path = item->text(0)                      ;
  if (path.length()<=0) return                      ;
  path = Current . absoluteFilePath ( path )        ;
  QStringList L                                     ;
  L << path                                         ;
  DeletePath ( Current , L )                        ;
  List       (             )                        ;
}

void N::PadBrowser::Rename(void)
{
  QTreeWidgetItem * item = ui->Files->currentItem() ;
  if (IsNull(item)) return                          ;
  QString path = item->text(0)                      ;
  if (path.length()<=0) return                      ;
  path = Current . absoluteFilePath ( path )        ;
  ///////////////////////////////////////////////////
  ui -> Label -> setCurrentWidget ( ui -> Head   )  ;
  ui -> Edit  -> setCurrentWidget ( ui -> Rename )  ;
  Display                         ( 2            )  ;
  ///////////////////////////////////////////////////
  QString n = Root . relativeFilePath ( path )      ;
  ui -> OldName -> blockSignals ( true  )           ;
  ui -> NewName -> blockSignals ( true  )           ;
  ui -> OldName -> setText      ( n     )           ;
  ui -> NewName -> setText      ( ""    )           ;
  ui -> NewName -> blockSignals ( false )           ;
  ui -> OldName -> blockSignals ( false )           ;
}

void N::PadBrowser::RenameFinished(void)
{
  QString rp = Root . absoluteFilePath ( "" ) ;
  QString on = ui  -> OldName -> text  (    ) ;
  QString nn = ui  -> NewName -> text  (    ) ;
  bool    cc = true                           ;
  if (on.length()<=0  ) cc = false            ;
  if (nn.length()<=0  ) cc = false            ;
  on         = Root . absoluteFilePath ( on ) ;
  nn         = Root . absoluteFilePath ( nn ) ;
  if (!on.contains(rp)) cc = false            ;
  if (!nn.contains(rp)) cc = false            ;
  if ( on == nn       ) cc = false            ;
  if (cc) QFile::rename ( on , nn )           ;
  Edit                                 (    ) ;
  List                                 (    ) ;
}

void N::PadBrowser::Edit(void)
{
  ui -> Label -> setCurrentWidget ( ui -> Head  ) ;
  ui -> Edit  -> setCurrentWidget ( ui -> Files ) ;
  Display                         ( 0           ) ;
}
