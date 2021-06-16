#include <datawidgets.h>

N::SqlMonitor:: SqlMonitor (QWidget * parent,Plan * p)
              : TreeDock   (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlMonitor::~SqlMonitor(void)
{
}

void N::SqlMonitor::Configure(void)
{
  StackSize          = 0                                            ;
  Reservation        = false                                        ;
  Monitoring         = false                                        ;
  autoFit            = true                                         ;
  Interval           = 3000                                         ;
  Scope              = ""                                           ;
  dockingOrientation = Qt::Horizontal                               ;
  dockingPlace       = Qt::BottomDockWidgetArea                     ;
  dockingPlaces      = Qt::AllDockWidgetAreas                       ;
  SQLs              << plan -> sql                                  ;
  ///////////////////////////////////////////////////////////////////
  LocalIcons [ "Start" ] = QIcon ( ":/icons/next.png"             ) ;
  LocalIcons [ "Stop"  ] = QIcon ( ":/icons/stop.png"             ) ;
  ///////////////////////////////////////////////////////////////////
  setWindowTitle                 ( tr("MySQL connection monitor") ) ;
  ///////////////////////////////////////////////////////////////////
  setColumnCount                 ( 0                              ) ;
  setDragDropMode                ( NoDragDrop                     ) ;
  setRootIsDecorated             ( false                          ) ;
  setAlternatingRowColors        ( true                           ) ;
  setSelectionMode               ( SingleSelection                ) ;
  setHorizontalScrollBarPolicy   ( Qt::ScrollBarAsNeeded          ) ;
  setVerticalScrollBarPolicy     ( Qt::ScrollBarAsNeeded          ) ;
  plan -> setFont                ( this                           ) ;
}

bool N::SqlMonitor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Start        , startup     ( ) ) ;
  LinkAction        ( Stop         , Shutdown    ( ) ) ;
  return true                                          ;
}

void N::SqlMonitor::run(int T,ThreadData * d)
{
  switch           ( T                                ) {
    case 10001                                          :
      Dispatch     (                                  ) ;
    break                                               ;
    case 10002                                          :
      startLoading (                                  ) ;
      Monitor      ( d -> Arguments [ 0 ] . toInt ( ) ) ;
      stopLoading  (                                  ) ;
    break                                               ;
  }                                                     ;
}

bool N::SqlMonitor::Shutdown(void)
{
  if ( Monitoring )     {
    Monitoring = false  ;
    stopForcely (     ) ;
    finalize    ( 100 ) ;
  }                     ;
  return true           ;
}

void N::SqlMonitor::TimeChanged(int ms)
{
  Interval = ms ;
}

void N::SqlMonitor::Dispatch(void)
{
  if ( isLoading ( ) ) return                   ;
  VarArgs args                                  ;
  for (int i = 0 ; i < SQLs . count ( ) ; i++ ) {
    args  . clear ( )                           ;
    args << i                                   ;
    start ( 10002 , args )                      ;
  }                                             ;
}

void N::SqlMonitor::Monitor(int id)
{
  Sql sql = SQLs [ id ]                                     ;
  Monitoring = true                                         ;
  SqlConnection SC ( sql )                                  ;
  if ( SC . open ( FunctionString ) )                       {
    QMap<int,QTreeWidgetItem *> Items                       ;
    while ( Monitoring && ( ! isStopped ( ) ) )             {
      QString Q = "show processlist ;"                      ;
      if ( SC . Query ( Q ) )                               {
        QSqlRecord r                                        ;
        r = SC . q . record ( )                             ;
        if ( ( r . count ( ) + 1 ) != columnCount ( ) )     {
          setColumnCount ( r . count ( ) + 1 )              ;
          QTreeWidgetItem * head                            ;
          head  = new QTreeWidgetItem ( )                   ;
          head -> setText ( 0 , tr("Address") )             ;
          for (int i = 0 ; i < r . count ( ) ; i++ )        {
            head -> setText ( i + 1 , r . fieldName ( i ) ) ;
          }                                                 ;
          setHeaderItem(head)                               ;
          if ( autoFit ) emit AutoFit ( )                   ;
        }                                                   ;
        CUIDs IDs                                           ;
        CUIDs Last = Items . keys ( )                       ;
        while ( SC . Next ( ) )                             {
          int Id = SC . Int ( 0 )                           ;
          QTreeWidgetItem * it = NULL                       ;
          IDs << Id                                         ;
          if ( Items . contains ( Id ) )                    {
            it = Items [ Id ]                               ;
          } else                                            {
            it = new QTreeWidgetItem ( )                    ;
            Items [ Id ] = it                               ;
            addTopLevelItem ( it )                          ;
          }                                                 ;
          it -> setText ( 0 , sql . Hostname )              ;
          for (int i = 0 ; i < r . count ( ) ; i++ )        {
            it -> setText ( i + 1 , SC . String ( i ) )     ;
          }                                                 ;
        }                                                   ;
        if ( Last . count ( ) > 0 )                         {
          int id                                            ;
          foreach ( id , Last )                             {
            if ( ! IDs . contains ( id ) )                  {
              int index                                     ;
              QTreeWidgetItem * it                          ;
              it = Items [ id ]                             ;
              Items . remove ( id )                         ;
              index = indexOfTopLevelItem ( it )            ;
              if ( index >= 0 )                             {
                takeTopLevelItem ( index )                  ;
              }                                             ;
            }                                               ;
          }                                                 ;
        }                                                   ;
      }                                                     ;
      if ( autoFit ) emit AutoFit ( )                       ;
      Time::msleep ( Interval )                             ;
    }                                                       ;
    SC . close  ( )                                         ;
  }                                                         ;
  SC   . remove ( )                                         ;
}

void N::SqlMonitor::Append(SUID uuid)
{
  EnterSQL ( SC , plan -> sql )                                      ;
    Sql sql                                                          ;
    sql = SC . GetConnection ( PlanTable ( SqlConnections ) , uuid ) ;
    if ( sql . isValid ( ) )                                         {
      int     id = SQLs . count ( )                                  ;
      VarArgs args                                                   ;
      SQLs << sql                                                    ;
      args << id                                                     ;
      start ( 10002 , args )                                         ;
    }                                                                ;
  LeaveSQL ( SC , plan -> sql )                                      ;
}

bool N::SqlMonitor::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

bool N::SqlMonitor::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QAction  * aa                                                 ;
  QSpinBox * spin = new QSpinBox(this)                          ;
  ///////////////////////////////////////////////////////////////
  spin -> setRange      ( 200 , 120 * 1000                    ) ;
  spin -> setSingleStep ( 1000                                ) ;
  spin -> setValue      ( Interval                            ) ;
  spin -> setPrefix     ( tr("Interval : ")                   ) ;
  spin -> setSuffix     ( tr(" ms")                           ) ;
  spin -> setAlignment  ( Qt::AlignVCenter | Qt::AlignRight   ) ;
  nConnect ( spin , SIGNAL(valueChanged(int) )                  ,
             this , SLOT  (TimeChanged (int) )                ) ;
  ///////////////////////////////////////////////////////////////
  if ( Monitoring )                                             {
    mm . add ( 102                                              ,
               LocalIcons [ "Stop"  ]                           ,
               tr("Stop monitoring" )                         ) ;
  } else                                                        {
    mm . add ( 101                                              ,
               LocalIcons [ "Start" ]                           ,
               tr("Start monitoring")                         ) ;
  }                                                             ;
  mm . add ( 901 , spin                                       ) ;
  mm . add ( 103 , tr("Auto resize columns") , true , autoFit ) ;
  mm . add ( 201 , tr("Sorting") , true , isSortingEnabled()  ) ;
  ///////////////////////////////////////////////////////////////
  DockingMenu    ( mm   )                                       ;
  ///////////////////////////////////////////////////////////////
  mm . setFont   ( plan )                                       ;
  aa = mm . exec (      )                                       ;
  ///////////////////////////////////////////////////////////////
  if ( IsNull     (      aa ) ) return true                     ;
  if ( RunDocking ( mm , aa ) ) return true                     ;
  ///////////////////////////////////////////////////////////////
  switch ( mm [ aa ] )                                          {
    case 101                                                    :
      startup      (      )                                     ;
    break                                                       ;
    case 102                                                    :
      Shutdown     (      )                                     ;
      Alert        ( Done )                                     ;
      SQLs . clear (      )                                     ;
      SQLs << plan -> sql                                       ;
    break                                                       ;
    case 103                                                    :
      autoFit = aa -> isChecked ( )                             ;
    break                                                       ;
    case 201                                                    :
      setSortingEnabled ( aa -> isChecked ( ) )                 ;
    break                                                       ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  return true                                                   ;
}
