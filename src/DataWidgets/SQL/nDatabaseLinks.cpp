#include <datawidgets.h>

N::DatabaseLinks:: DatabaseLinks (QWidget * parent,Plan * p)
                 : TreeDock      (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DatabaseLinks::~DatabaseLinks (void)
{
}

void N::DatabaseLinks::Configure(void)
{
  Scope              = ""                                     ;
  dockingOrientation = Qt::Vertical                           ;
  dockingPlace       = Qt::RightDockWidgetArea                ;
  dockingPlaces      = Qt::AllDockWidgetAreas                 ;
  setWindowTitle               ( tr("Database connections") ) ;
  setDragDropMode              ( NoDragDrop                 ) ;
  setRootIsDecorated           ( false                      ) ;
  setAlternatingRowColors      ( true                       ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded      ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded      ) ;
  setAccessibleName            ( "N::DatabaseLinks"         ) ;
  setColumnCount               ( 1                          ) ;
  NewTreeWidgetItem            ( head                       ) ;
  head -> setText              ( 0 , tr("Connection")       ) ;
  assignHeaderItems            ( head                       ) ;
  MountClicked                 ( 2                          ) ;
  plan -> setFont              ( this                       ) ;
}

bool N::DatabaseLinks::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , New         ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  return true                                          ;
}

bool N::DatabaseLinks::Shutdown(void)
{
  return ( ! isStopped ( ) ) ;
}

void N::DatabaseLinks::run(int type,ThreadData * data)
{
  switch ( type )      {
    case 10001         :
      startLoading ( ) ;
      List         ( ) ;
      stopLoading  ( ) ;
    break              ;
  }                    ;
}

void N::DatabaseLinks::List(void)
{
  emit assignEnabling   ( false                   ) ;
  EnterSQL              ( SC , plan -> sql        ) ;
    QString Q                                       ;
    Q = SC . sql . SelectFrom                       (
        "uuid,name"                                 ,
        PlanTable(SqlConnections)                   ,
        SC . OrderByAsc ( "id" )                  ) ;
    SqlLoopNow ( SC , Q )                           ;
      SUID    uuid = SC . Uuid   ( 0 )              ;
      QString name = SC . String ( 1 )              ;
      NewTreeWidgetItem ( item                    ) ;
      item -> setData   ( 0 , Qt::UserRole , uuid ) ;
      item -> setText   ( 0 , name                ) ;
      addTopLevelItem   ( item                    ) ;
    SqlLoopErr          ( SC , Q                  ) ;
    SqlLoopEnd          ( SC , Q                  ) ;
  LeaveSQL              ( SC , plan -> sql        ) ;
  emit assignEnabling   ( true                    ) ;
  Alert                 ( Done                    ) ;
}

bool N::DatabaseLinks::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::DatabaseLinks::Apply(SUID uuid,Sql * sql)
{
  EnterSQL       ( SC , plan->sql                               ) ;
    QString Q                                                     ;
    if (uuid>0)                                                   {
      Q = SC.sql.Update                                           (
            PlanTable(SqlConnections)                             ,
            "where uuid = :UUID"                                  ,
            11                                                    ,
            "name"                                                ,
            "sqltype"                                             ,
            "hostname"                                            ,
            "username"                                            ,
            "password"                                            ,
            "dbname"                                              ,
            "prefix"                                              ,
            "options"                                             ,
            "extra"                                               ,
            "indexing"                                            ,
            "foreignkey"                                        ) ;
    } else                                                        {
      uuid = SC . Unique ( PlanTable(MajorUuid) , "uuid" , 9002 ) ;
      SC.assureUuid                                               (
        PlanTable(MajorUuid)                                      ,
        uuid                                                      ,
        Types::SqlConnection                                    ) ;
      Q = SC.sql.InsertInto                                       (
            PlanTable(SqlConnections)                             ,
            12                                                    ,
            "uuid"                                                ,
            "name"                                                ,
            "sqltype"                                             ,
            "hostname"                                            ,
            "username"                                            ,
            "password"                                            ,
            "dbname"                                              ,
            "prefix"                                              ,
            "options"                                             ,
            "extra"                                               ,
            "indexing"                                            ,
            "foreignkey"                                        ) ;
    }                                                             ;
    ///////////////////////////////////////////////////////////////
    SC . Prepare ( Q                                            ) ;
    SC . Bind    ( "uuid"       , uuid                          ) ;
    SC . Bind    ( "name"       , sql->Profile        .toUtf8() ) ;
    SC . Bind    ( "sqltype"    , sql->SqlMode        .toUtf8() ) ;
    SC . Bind    ( "hostname"   , sql->Hostname       .toUtf8() ) ;
    SC . Bind    ( "username"   , sql->Username       .toUtf8() ) ;
    SC . Bind    ( "password"   , sql->Password       .toUtf8() ) ;
    SC . Bind    ( "dbname"     , sql->dbName         .toUtf8() ) ;
    SC . Bind    ( "prefix"     , sql->PrefixName     .toUtf8() ) ;
    SC . Bind    ( "options"    , sql->Options        .toUtf8() ) ;
    SC . Bind    ( "extra"      , sql->ExtraAttributes.toUtf8() ) ;
    SC . Bind    ( "indexing"   , sql->Indexing       ? 1 : 0   ) ;
    SC . Bind    ( "foreignkey" , sql->isForeignKey   ? 1 : 0   ) ;
    SC . Exec    (                                              ) ;
  LeaveSQL       ( SC , plan->sql                               ) ;
  startup        (                                              ) ;
  Alert          ( Done                                         ) ;
}

void N::DatabaseLinks::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID uuid = nTreeUuid(item,0)      ;
  emit NewConnection ( this , uuid ) ;
}

void N::DatabaseLinks::New(void)
{
  emit NewConnection ( this , 0 ) ;
}

bool N::DatabaseLinks::Menu(QPoint pos)
{
  MenuManager       mm ( this )                ;
  QMenu           * ma                         ;
  QMenu           * ms                         ;
  QMenu           * mi                         ;
  QAction         * aa                         ;
  QTreeWidgetItem * it = itemAt ( pos )        ;
  //////////////////////////////////////////////
  mm . add ( 101 , tr("New")      )            ;
  mm . add ( 102 , tr("Refresh")  )            ;
  if ( NotNull ( it ) )                        {
    QString os                                 ;
    os = tr("Organize %1").arg(it->text(0))    ;
    mm . add ( 301 , os )                      ;
    os = tr("Monitor %1").arg(it->text(0))     ;
    mm . add ( 302 , os )                      ;
  }                                            ;
  mm . addSeparator (                   )      ;
  ma = mm . addMenu ( tr("Adjustments") )      ;
  ms = mm . addMenu ( tr("Sorting")     )      ;
  mi = mm . addMenu ( tr("Selections")  )      ;
  AdjustMenu     ( mm , ma )                   ;
  SortingMenu    ( mm , ms )                   ;
  SelectionsMenu ( mm , mi )                   ;
  DockingMenu    ( mm      )                   ;
  mm . setFont   ( plan    )                   ;
  aa = mm . exec (         )                   ;
  if ( IsNull ( aa ) ) return true             ;
  //////////////////////////////////////////////
  if ( RunAdjustment ( mm , aa ) ) return true ;
  if ( RunSorting    ( mm , aa ) ) return true ;
  if ( RunSelections ( mm , aa ) ) return true ;
  if ( RunDocking    ( mm , aa ) ) return true ;
  //////////////////////////////////////////////
  switch ( mm [ aa ] )                         {
    case 101                                   :
      New     ( )                              ;
    break                                      ;
    case 102                                   :
      startup ( )                              ;
    break                                      ;
    case 301                                   :
      emit Organize ( nTreeUuid ( it , 0 ) )   ;
    break                                      ;
    case 302                                   :
      emit Monitor  ( nTreeUuid ( it , 0 ) )   ;
    break                                      ;
  }                                            ;
  //////////////////////////////////////////////
  return true                                  ;
}
