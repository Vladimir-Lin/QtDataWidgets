#include <datawidgets.h>

N::CloneFiles:: CloneFiles ( QWidget * parent , Plan * p )
              : TreeDock   (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CloneFiles::~CloneFiles (void)
{
}

void N::CloneFiles::Configure(void)
{
  setWindowIcon                ( QIcon(":/images/documentcopy.png") ) ;
  setWindowTitle               ( tr("Clone files")                  ) ;
  setSuggestion                ( QSize ( 320 , 600 )                ) ;
  setDragDropMode              ( NoDragDrop                         ) ;
  setRootIsDecorated           ( false                              ) ;
  setAlternatingRowColors      ( true                               ) ;
  setSelectionMode             ( ExtendedSelection                  ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded              ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded              ) ;
  setColumnCount               ( 4                                  ) ;
  /////////////////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                               ) ;
  head -> setText              ( 0 , tr("Source" )                  ) ;
  head -> setText              ( 1 , tr("Target" )                  ) ;
  head -> setText              ( 2 , tr("Status" )                  ) ;
  head -> setText              ( 3 , ""                             ) ;
  assignHeaderItems            ( head                               ) ;
  /////////////////////////////////////////////////////////////////////
  MountClicked                 ( 1                                  ) ;
  MountClicked                 ( 2                                  ) ;
  plan->setFont                ( this                               ) ;
  setFunction                  ( 107001 , false                     ) ;
}

bool N::CloneFiles::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Load         , Load        ( ) ) ;
  LinkAction        ( Save         , Save        ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Start        , Start       ( ) ) ;
  LinkAction        ( Stop         , Stop        ( ) ) ;
  return true                                          ;
}

void N::CloneFiles::singleClicked(QTreeWidgetItem *,int)
{
  Alert ( Click ) ;
}

void N::CloneFiles::doubleClicked(QTreeWidgetItem * item,int column)
{
  if ( NULL == item ) return         ;
  int status = nTreeInt ( item , 2 ) ;
  if ( 0 != status ) return          ;
  switch ( column )                  {
    case 0                           :
      BrowseSource ( )               ;
    break                            ;
    case 1                           :
      BrowseTarget ( )               ;
    break                            ;
    case 2                           :
      Start        ( )               ;
    break                            ;
  }
}

void N::CloneFiles::BrowseSource(void)
{
  if ( isFunction ( 107001 ) ) return          ;
  QTreeWidgetItem * it = currentItem ( )       ;
  if ( IsNull(it) ) return                     ;
  int status = nTreeInt ( it , 2 )             ;
  if ( 0 != status ) return                    ;
  //////////////////////////////////////////////
  QString filename = it -> text ( 0 )          ;
  if ( filename . length ( ) <= 0 )            {
    filename = GetPath ( )                     ;
  }                                            ;
  filename = QFileDialog::getExistingDirectory (
               this                            ,
               tr("Backup source directory")   ,
               filename                      ) ;
  if ( filename . length ( ) <= 0 ) return     ;
  KeepPath ( filename )                        ;
  //////////////////////////////////////////////
  QTreeWidgetItem * xt                         ;
  QString t = it -> text ( 1 )                 ;
  if ( t . length ( ) > 0 )                    {
    xt = findItem ( filename , t )             ;
    if ( NotNull(xt) )                         {
      Alert ( Error )                          ;
     return                                    ;
    }                                          ;
  }                                            ;
  it -> setText ( 0 , filename )               ;
}

void N::CloneFiles::BrowseTarget(void)
{
  if ( isFunction ( 107001 ) ) return          ;
  QTreeWidgetItem * it = currentItem ( )       ;
  if ( IsNull(it) ) return                     ;
  int status = nTreeInt ( it , 2 )             ;
  if ( 0 != status ) return                    ;
  //////////////////////////////////////////////
  QString filename = it -> text ( 1 )          ;
  if ( filename . length ( ) <= 0 )            {
    filename = GetPath ( )                     ;
  }                                            ;
  filename = QFileDialog::getExistingDirectory (
               this                            ,
               tr("Backup target directory")   ,
               filename                      ) ;
  if ( filename . length ( ) <= 0 ) return     ;
  KeepPath ( filename )                        ;
  //////////////////////////////////////////////
  QTreeWidgetItem * xt                         ;
  QString t = it -> text ( 0 )                 ;
  if ( t . length ( ) > 0 )                    {
    xt = findItem ( t , filename )             ;
    if ( NotNull(xt) )                         {
      Alert ( Error )                          ;
     return                                    ;
    }                                          ;
  }                                            ;
  it -> setText ( 1 , filename )               ;
}

void N::CloneFiles::Load(QString filename)
{
  if ( ! canStop ( ) ) return                                   ;
  ///////////////////////////////////////////////////////////////
  KeepPath ( filename )                                         ;
  plan -> Variables [ "ClonePlan" ] = filename                  ;
  clear ( )                                                     ;
  ///////////////////////////////////////////////////////////////
  N::Settings sf  ( filename , QSettings::IniFormat )           ;
  int total = 0                                                 ;
  ///////////////////////////////////////////////////////////////
  sf . beginGroup ( "Settings" )                                ;
  total = sf . value ( "Total" ) . toInt ( )                    ;
  sf . endGroup   (            )                                ;
  ///////////////////////////////////////////////////////////////
  for (int i=0;i<total;i++)                                     {
    QString n = QString::number(i+1)                            ;
    QString s                                                   ;
    QString t                                                   ;
    sf . beginGroup      ( n        )                           ;
    s = sf . valueString ( "Source" )                           ;
    t = sf . valueString ( "Target" )                           ;
    sf . endGroup        (          )                           ;
    NewTreeWidgetItem    ( IT       )                           ;
    IT -> setText        ( 0 , s    )                           ;
    IT -> setText        ( 1 , t    )                           ;
    addTopLevelItem      ( IT       )                           ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  Alert                  ( Done     )                           ;
  assignTitle            ( filename )                           ;
  emit AutoFit           (          )                           ;
}

void N::CloneFiles::Load(void)
{
  if ( ! canStop ( ) ) return                                   ;
  ///////////////////////////////////////////////////////////////
  QString filename                                              ;
  if ( plan  -> Variables . contains ( "ClonePlan" ) )          {
    filename = plan -> Variables [ "ClonePlan" ] . toString ( ) ;
  } else                                                        {
    filename = GetPath ( )                                      ;
  }                                                             ;
  filename = QFileDialog::getOpenFileName                       (
               this                                             ,
               tr("Load backup plan")                           ,
               filename                                         ,
               "Backup plan(*.ini)"                           ) ;
  if ( filename . length ( ) <= 0 ) return                      ;
  Load ( filename )                                             ;
}

void N::CloneFiles::Save(void)
{
  QString filename                                              ;
  if ( plan  -> Variables . contains ( "ClonePlan" ) )          {
    filename = plan -> Variables [ "ClonePlan" ] . toString ( ) ;
  } else                                                        {
    filename = GetPath ( )                                      ;
  }                                                             ;
  filename = QFileDialog::getSaveFileName                       (
               this                                             ,
               tr("Save backup plan")                           ,
               filename                                         ,
               "Backup plan(*.ini)"                           ) ;
  if ( filename . length ( ) <= 0 ) return                      ;
  KeepPath ( filename )                                         ;
  plan -> Variables [ "ClonePlan" ] = filename                  ;
  ///////////////////////////////////////////////////////////////
  N::Settings sf  ( filename , QSettings::IniFormat )           ;
  int total = topLevelItemCount ( )                             ;
  ///////////////////////////////////////////////////////////////
  QString app = "Neutrino Backup Plan"                          ;
  sf . beginGroup ( "Settings"            )                     ;
  sf . setValue   ( "Total"       , total )                     ;
  sf . setValue   ( "Application" , app   )                     ;
  sf . endGroup   (                       )                     ;
  ///////////////////////////////////////////////////////////////
  for (int i=0;i<total;i++)                                     {
    QTreeWidgetItem * it = topLevelItem ( i )                   ;
    QString n = QString::number(i+1)                            ;
    sf . beginGroup ( n                           )             ;
    sf . setValue   ( "Source" , it -> text ( 0 ) )             ;
    sf . setValue   ( "Target" , it -> text ( 1 ) )             ;
    sf . endGroup   (                             )             ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  Alert             ( Done                        )             ;
  assignTitle       ( filename                    )             ;
}

void N::CloneFiles::Insert(void)
{
  if ( isFunction ( 107001 ) ) return         ;
  NewTreeWidgetItem ( IT                    ) ;
  IT -> setData     ( 2  , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( IT                    ) ;
  scrollToItem      ( IT                    ) ;
  setCurrentItem    ( IT                    ) ;
  doubleClicked     ( IT , 0                ) ;
}

void N::CloneFiles::Delete(void)
{
  if ( isFunction ( 107001 ) ) return    ;
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( IsNull(it) ) return               ;
  int status = nTreeInt ( it , 2 )       ;
  if ( 0 != status ) return              ;
  takeItem ( it )                        ;
}

void N::CloneFiles::StartAll(void)
{
  if ( isFunction ( 107001 ) ) return ;
  start ( 10001 )                     ;
}

void N::CloneFiles::Start(void)
{
  if ( isFunction ( 107001 ) ) return    ;
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( IsNull(it) ) return               ;
  int status = nTreeInt(it,2)            ;
  if ( 0 != status ) return              ;
  if ( it->text(0).length()<=0) return   ;
  if ( it->text(1).length()<=0) return   ;
  it -> setData ( 2 , Qt::UserRole , 1 ) ;
  it -> setText ( 2 , tr("Start")      ) ;
  ////////////////////////////////////////
  VarArgs args                           ;
  args << it -> text ( 0 )               ;
  args << it -> text ( 1 )               ;
  start ( 10002 , args )                 ;
}

void N::CloneFiles::Stop(void)
{
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( IsNull(it) ) return               ;
  int status = nTreeInt(it,2)            ;
  if ( 0 == status ) return              ;
  it -> setData ( 2 , Qt::UserRole , 2 ) ;
  it -> setText ( 2 , tr("Stopping")   ) ;
}

void N::CloneFiles::KeepPath(QString path)
{
  QFileInfo F ( path )                   ;
  QString   p = F . absolutePath ( )     ;
  plan -> Variables [ "CloneFiles" ] = p ;
}

QString N::CloneFiles::GetPath(void)
{
  QString p                                               ;
  if ( plan  -> Variables . contains ( "CloneFiles" ) )   {
    p = plan -> Variables [ "CloneFiles" ] . toString ( ) ;
  } else                                                  {
    p = plan -> Temporary ( "" )                          ;
  }                                                       ;
  return p                                                ;
}

QTreeWidgetItem * N::CloneFiles::findItem(QString s,QString t)
{
  for (int i=0;i<topLevelItemCount();i++)                         {
    QTreeWidgetItem * it                                          ;
    it = topLevelItem ( i )                                       ;
    if ( ( s == it -> text ( 0 ) ) && ( t == it -> text ( 1 ) ) ) {
      return it                                                   ;
    }                                                             ;
  }                                                               ;
  return NULL                                                     ;
}

void N::CloneFiles::RunAll(void)
{
  int    i = 0                                                    ;
  int    pid                                                      ;
  qint64 value = 0                                                ;
  bool   go    = true                                             ;
  /////////////////////////////////////////////////////////////////
  setFunction ( 107001 , true  )                                  ;
  pid = plan -> Progress ( tr("Copy directories") , tr("%v/%m") ) ;
  plan -> Start          ( pid , &value , &go                   ) ;
  plan -> setRange       ( pid , 0      , topLevelItemCount ( ) ) ;
  while ( go && ( i < topLevelItemCount ( ) ) )                   {
    QTreeWidgetItem * it = topLevelItem ( i )                     ;
    if ( NULL != it )                                             {
      int status = nTreeInt(it,2)                                 ;
      value ++                                                    ;
      if ( 0 == status )                                          {
        QString s = it->text(0)                                   ;
        QString t = it->text(1)                                   ;
        if ( ( s.length() > 0 ) && ( t.length() > 0 ) )           {
          VarArgs args                                            ;
          it -> setData ( 2 , Qt::UserRole , 1 )                  ;
          it -> setText ( 2 , tr("Start")      )                  ;
          args << s                                               ;
          args << t                                               ;
          plan -> ProgressText ( pid , s )                        ;
          Copy ( args )                                           ;
        }                                                         ;
      }                                                           ;
    }                                                             ;
    i ++                                                          ;
  }                                                               ;
  /////////////////////////////////////////////////////////////////
  setFunction    ( 107001 , false )                               ;
  plan -> Finish ( pid            )                               ;
  Time::msleep   ( 1000           )                               ;
}

void N::CloneFiles::CreateDirs (
       QDir          & source  ,
       QDir          & target  ,
       QFileInfoList & infos   )
{
  QFileInfoList dirs                                                         ;
  for (int i=0;i<infos.count();i++)                                          {
    if ( infos [ i ] . isDir ( ) ) dirs << infos [ i ]                       ;
  }                                                                          ;
  if ( dirs . count ( ) <= 0 ) return                                        ;
  if ( ! target . exists ( ) ) return                                        ;
  ////////////////////////////////////////////////////////////////////////////
  qint64 value = 0                                                           ;
  bool   go    = true                                                        ;
  int    pid                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  pid = plan -> Progress  ( tr("Create directories") , tr("%v/%m") )         ;
  plan -> Start           ( pid , &value , &go                     )         ;
  plan -> setRange        ( pid , 0      , dirs . count ( )        )         ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0 ; go && ( i < dirs . count ( ) ) ; i++ )                    {
    QFileInfo fi = dirs [ i ]                                                ;
    QString   rp = source . relativeFilePath ( fi . absoluteFilePath ( ) )   ;
    QString   tp = target . absoluteFilePath ( rp                        )   ;
    if ( ( rp . length ( ) > 0 ) && ( tp . length ( ) > 0 ) )                {
      plan -> ProgressText ( pid , tp )                                      ;
      target . mkpath ( tp )                                                 ;
    }                                                                        ;
    value++                                                                  ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> Finish ( pid  )                                                    ;
}

void N::CloneFiles::Copy      (
       QDir          & source ,
       QDir          & target ,
       QFileInfoList & infos  )
{
  QFileInfoList files                                                        ;
  qint64        total = 0                                                    ;
  for (int i=0;i<infos.count();i++)                                          {
    if ( infos [ i ] . isFile ( ) )                                          {
      QFileInfo fi = infos [ i ]                                             ;
      QString   rp = source . relativeFilePath ( fi . absoluteFilePath ( ) ) ;
      QString   tp = target . absoluteFilePath ( rp                        ) ;
      QFileInfo ti ( tp )                                                    ;
      bool      same = true                                                  ;
      if ( ! ti . exists ( )                          ) same = false         ;
      if ( same && ( fi . size ( ) != ti . size ( ) ) ) same = false         ;
      if ( same && ( fi . lastModified ( ) != ti . lastModified ( ) ) )      {
        same = false                                                         ;
      }                                                                      ;
      if ( ! same )                                                          {
        files << infos [ i ]                                                 ;
        total += infos [ i ] . size ( )                                      ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  if ( files . count ( ) <= 0 ) return                                       ;
  if ( ! target . exists ( )  ) return                                       ;
  ////////////////////////////////////////////////////////////////////////////
  qint64 sum   = 0                                                           ;
  qint64 value = 0                                                           ;
  bool   go    = true                                                        ;
  int    pid                                                                 ;
  ////////////////////////////////////////////////////////////////////////////
  pid = plan -> Progress  ( tr("Copy files") , tr("%v/%m") )                 ;
  plan -> Start           ( pid , &value , &go             )                 ;
  plan -> setRange        ( pid , 0      , total / 1024    )                 ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0 ; go && ( i < files . count ( ) ) ; i++ )                   {
    QFileInfo fi = files [ i ]                                               ;
    QString   sp = fi     . absoluteFilePath (    )                          ;
    QString   rp = source . relativeFilePath ( sp )                          ;
    QString   tp = target . absoluteFilePath ( rp )                          ;
    if ( ( rp . length ( ) > 0 ) && ( tp . length ( ) > 0 ) )                {
      QFileInfo xi ( tp )                                                    ;
      plan -> ProgressText ( pid , tp )                                      ;
      if ( xi . exists ( ) ) QFile::remove ( tp )                            ;
      QFile::copy ( sp , tp )                                                ;
      sum  += fi . size ( )                                                  ;
      value = ( sum + 1023 ) / 1024                                          ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> Finish ( pid  )                                                    ;
}

void N::CloneFiles::Copy(VarArgs & args)
{
  if ( args . count ( ) < 2 ) return                                         ;
  ////////////////////////////////////////////////////////////////////////////
  QString           s    = args [ 0 ] . toString ( )                         ;
  QString           t    = args [ 1 ] . toString ( )                         ;
  qint64            step = 0                                                 ;
  bool              go   = true                                              ;
  int               pid                                                      ;
  QDir              source ( s )                                             ;
  QDir              target ( t )                                             ;
  QTreeWidgetItem * it                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  if ( ! source . exists ( ) ) return                                        ;
  target . mkpath ( target . absolutePath ( ) )                              ;
  it = findItem ( s , t )                                                    ;
  if ( IsNull(it) ) return                                                   ;
  ////////////////////////////////////////////////////////////////////////////
  QFileInfoList infos                                                        ;
  pid = plan -> Progress  ( s   , tr("%v/%m")            )                   ;
  plan -> Start           ( pid , &step  , &go           )                   ;
  plan -> setRange        ( pid , 0      , 3             )                   ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> ProgressText ( pid , tr("Scan source files"        ) )             ;
  step ++                                                                    ;
  if ( go )                                                                  {
    infos = File::FileInfos ( source                           )             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> ProgressText ( pid , tr("Create target directories") )             ;
  step ++                                                                    ;
  if ( go )                                                                  {
    CreateDirs              ( source , target , infos          )             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> ProgressText ( pid , tr("Copy files"               ) )             ;
  step ++                                                                    ;
  if ( go )                                                                  {
    Copy                    ( source , target , infos          )             ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  plan -> Finish  ( pid                  )                                   ;
  Alert           ( Done                 )                                   ;
  it   -> setData ( 2 , Qt::UserRole , 0 )                                   ;
  it   -> setText ( 2 , tr("Completed")  )                                   ;
}

void N::CloneFiles::run(int type,ThreadData * data)
{
  switch ( type )                       {
    case 10001                          :
      pushCanStop (                   ) ;
      RunAll      (                   ) ;
      popCanStop  (                   ) ;
    break                               ;
    case 10002                          :
      pushCanStop (                   ) ;
      Copy        ( data -> Arguments ) ;
      popCanStop  (                   ) ;
    break                               ;
  }                                     ;
}

bool N::CloneFiles::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                                  ;
  QMenu           * ma                                                       ;
  QMenu           * ms                                                       ;
  QMenu           * mc                                                       ;
  QAction         * aa                                                       ;
  QTreeWidgetItem * it  = itemAt        ( pos )                              ;
  QTreeWidgetItem * hr  = headerItem    (     )                              ;
  int               cc  = currentColumn (     )                              ;
  int               cx                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  mm   . add ( 101 , tr("Load backup plan") )                                ;
  mm   . add ( 102 , tr("Save backup plan") )                                ;
  if ( ! isFunction ( 107001 ) )                                             {
    mm   . addSeparator ( )                                                  ;
    mm   . add ( 201 , tr("Insert"        ) )                                ;
    if ( NotNull ( it ) )                                                    {
      int status = nTreeInt ( it , 2 )                                       ;
      if ( 0 == status )                                                     {
        mm . add ( 202 , tr("Delete"      ) )                                ;
      }                                                                      ;
      mm   . addSeparator ( )                                                ;
      if ( 0 == status )                                                     {
        mm . add ( 301 , tr("Start"       ) )                                ;
      } else                                                                 {
        mm . add ( 302 , tr("Stop"        ) )                                ;
      }                                                                      ;
    }                                                                        ;
  }                                                                          ;
  if ( ( ! isFunction ( 107001 ) ) && ( topLevelItemCount ( ) > 0 ) )        {
    mm . addSeparator ( )                                                    ;
    mm . add ( 401 , tr("Start all"       ) )                                ;
  }                                                                          ;
  mm . addSeparator ( )                                                      ;
  mc = mm . addMenu ( tr("Columns"    )                )                     ;
  ma = mm . addMenu ( tr("Adjustments")                )                     ;
  ms = mm . addMenu ( tr("Sorting"    )                )                     ;
  ////////////////////////////////////////////////////////////////////////////
  for (int i=0;i<columnCount();i++)                                          {
    QString hs = hr -> text ( i )                                            ;
    if ( hs . length ( ) <= 0 ) hs = tr("Pending")                           ;
    mm . add ( mc                                                            ,
               800 + i                                                       ,
               hs                                                            ,
               true                                                          ,
               ! isColumnHidden ( i )                                      ) ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  AdjustMenu  ( mm , ma   )                                                  ;
  SortingMenu ( mm , ms   )                                                  ;
  DockingMenu ( mm        )                                                  ;
  ////////////////////////////////////////////////////////////////////////////
  aa = mm . exec ( )                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( IsNull        ( aa      ) ) return true                               ;
  if ( RunAdjustment ( mm , aa ) ) return true                               ;
  if ( RunSorting    ( mm , aa ) ) return true                               ;
  if ( RunDocking    ( mm , aa ) ) return true                               ;
  ////////////////////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                                       {
    case 101                                                                 :
      Load     ( )                                                           ;
    break                                                                    ;
    case 102                                                                 :
      Save     ( )                                                           ;
    break                                                                    ;
    case 201                                                                 :
      Insert   ( )                                                           ;
    break                                                                    ;
    case 202                                                                 :
      Delete   ( )                                                           ;
    break                                                                    ;
    case 301                                                                 :
      Start    ( )                                                           ;
    break                                                                    ;
    case 302                                                                 :
      Stop     ( )                                                           ;
    break                                                                    ;
    case 401                                                                 :
      StartAll ( )                                                           ;
    break                                                                    ;
    case 800                                                                 :
    case 801                                                                 :
    case 802                                                                 :
    case 803                                                                 :
      setColumnHidden ( mm [ aa ] - 800 , ! aa -> isChecked ( ) )            ;
      emit AutoFit ( )                                                       ;
    break                                                                    ;
    default                                                                  :
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}
