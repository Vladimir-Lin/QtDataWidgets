#include <datawidgets.h>

N::DatabaseDock:: DatabaseDock (QWidget * parent,Plan * p)
                : TreeDock     (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DatabaseDock::~DatabaseDock (void)
{
}

void N::DatabaseDock::Configure(void)
{
  Scope              = ""                                   ;
  dockingOrientation = Qt::Vertical                         ;
  dockingPlace       = Qt::RightDockWidgetArea              ;
  dockingPlaces      = Qt::AllDockWidgetAreas               ;
  sql                = plan -> sql                          ;
  setDragDropMode              ( DropOnly                 ) ;
  setSelectionMode             ( ExtendedSelection        ) ;
  setRootIsDecorated           ( true                     ) ;
  setAlternatingRowColors      ( true                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded    ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded    ) ;
  setAccessibleName            ( "nDatabaseDock"          ) ;
  setColumnCount               ( 1                        ) ;
  plan->setFont                ( this                     ) ;
  Icons [ Database ] = QIcon ( ":/images/database.png"    ) ;
  Icons [ Table    ] = QIcon ( ":/images/sqltable.png"    ) ;
  Icons [ View     ] = QIcon ( ":/images/importtable.png" ) ;
  Icons [ Unknown  ] = QIcon ( ":/images/help.png"        ) ;
  Icons [ Error    ] = QIcon ( ":/images/removefiles.png" ) ;
}

void N::DatabaseDock::run(int type,ThreadData * data)
{
  switch ( type )                                             {
    case 10001                                                :
      pushCanStop  (                                        ) ;
      startLoading (                                        ) ;
      List         (                                        ) ;
      stopLoading  (                                        ) ;
      popCanStop   (                                        ) ;
    break                                                     ;
    case 10002                                                :
      startLoading (                                        ) ;
      Optimize     ( data -> Arguments [ 0 ] . toString ( )   ,
                     data -> Arguments [ 1 ] . toString ( ) ) ;
      stopLoading  (                                        ) ;
    break                                                     ;
    case 10003                                                :
      startLoading (                                        ) ;
      Optimize     ( data -> Arguments [ 0 ] . toString ( ) ) ;
      stopLoading  (                                        ) ;
    break                                                     ;
  }                                                           ;
}

void N::DatabaseDock::List(void)
{
  nDropOut     ( IsNull ( plan ) )                                           ;
  setEnabled   ( false           )                                           ;
  blockSignals ( true            )                                           ;
  SqlConnection DC ( sql )                                                   ;
  if ( DC . open ( QtUUID::createUuidString() , QString(__FUNCTION__) ) )    {
    QString                         Q                                        ;
    QString                         T                                        ;
    QStringList                     DBs                                      ;
    QStringList                     TBs                                      ;
    QMap<QString,QTreeWidgetItem *> Items                                    ;
    int                             pid                                      ;
    int                             tid                                      ;
    qint64                          value  = 0                               ;
    qint64                          tvalue = 0                               ;
    bool                            go     = true                            ;
    bool                            tgo    = true                            ;
    //////////////////////////////////////////////////////////////////////////
    Q = "show databases ;"                                                   ;
    SqlLoopNow(DC,Q)                                                         ;
      DBs << DC.String(0)                                                    ;
    SqlLoopErr(q,Q)                                                          ;
    SqlLoopEnd(q,Q)                                                          ;
    //////////////////////////////////////////////////////////////////////////
    pid = plan -> Progress ( tr("List schemas") , tr("%v/%m schemas") )      ;
    tid = plan -> Progress ( tr("List tables" ) , tr("%v/%m tables" ) )      ;
    plan -> Start          ( pid , &value  , &go                      )      ;
    plan -> setRange       ( pid , 0       , DBs . count ( )          )      ;
    plan -> Start          ( tid , &tvalue , &tgo                     )      ;
    for (int i = 0 ; go && ( i < DBs . count ( ) ) ; i++ , value++ )         {
      T = DBs[i]                                                             ;
      ////////////////////////////////////////////////////////////////////////
      plan -> ProgressText ( pid , T )                                       ;
      NewTreeWidgetItem(db)                                                  ;
      db->setText(0,T)                                                       ;
      db->setIcon(0,Icons[Database])                                         ;
      addTopLevelItem(db)                                                    ;
      ////////////////////////////////////////////////////////////////////////
      DC . Query ( QString("use %1 ;").arg(T) )                              ;
      Q = "show tables ;"                                                    ;
      TBs.clear()                                                            ;
      SqlLoopNow(DC,Q)                                                       ;
        T = DC.String(0)                                                     ;
        NewTreeWidgetItem(tb)                                                ;
        db->addChild(tb)                                                     ;
        tb->setText(0,T)                                                     ;
        Items[T] = tb                                                        ;
        TBs << T                                                             ;
      SqlLoopErr(DC,Q)                                                       ;
      SqlLoopEnd(DC,Q)                                                       ;
      db->setToolTip(0,tr("%1 tables").arg(TBs.count()))                     ;
      ////////////////////////////////////////////////////////////////////////
      plan  -> setRange ( tid , 0 , TBs . count ( ) )                        ;
      tvalue = 0                                                             ;
      for (int j = 0 ; tgo && ( j < TBs . count ( ) ) ; j++ , tvalue++ )     {
        T = TBs[j]                                                           ;
        plan -> ProgressText ( tid , T )                                     ;
        Q = QString("show create table %1 ;").arg(T)                         ;
        IfSqlQuery(DC,Q)                                                     {
          QString    C = DC.String(1)                                        ;
          QSqlRecord r = DC.q.record()                                       ;
          Items[T]->setToolTip(0,C)                                          ;
          if (r.field(0).name()=="Table")                                    {
            Items[T]->setIcon(0,Icons[Table])                                ;
          } else
          if (r.field(0).name()=="View")                                     {
            Items[T]->setIcon(0,Icons[View])                                 ;
          } else                                                             {
            Items[T]->setIcon(0,Icons[Unknown])                              ;
          }                                                                  ;
        } else                                                               {
          Items[T]->setIcon(0,Icons[Error])                                  ;
        }                                                                    ;
        if ( isStopped ( ) ) tgo = false                                     ;
      }                                                                      ;
      ////////////////////////////////////////////////////////////////////////
      if ( isStopped ( ) ) go = false                                        ;
    }                                                                        ;
    plan -> Finish ( tid )                                                   ;
    plan -> Finish ( pid )                                                   ;
    DC . close ( )                                                           ;
  }                                                                          ;
  DC . remove  ( )                                                           ;
  ////////////////////////////////////////////////////////////////////////////
  blockSignals ( false )                                                     ;
  setEnabled   ( true  )                                                     ;
  Alert        ( Done  )                                                     ;
}

bool N::DatabaseDock::Shutdown(void)
{
  return ( ! isStopped ( ) ) ;
}

bool N::DatabaseDock::startup(void)
{
  nKickOut       ( IsNull(plan) , false ) ;
  setWindowTitle ( sql . Profile        ) ;
  setHeaderLabel ( sql . Hostname       ) ;
  clear          (                      ) ;
  start          ( 10001                ) ;
  return true                             ;
}

bool N::DatabaseDock::Menu(QPoint pos)
{
  QTreeWidgetItem * item = itemAt(pos)                           ;
  if (IsNull(item)) return false                                 ;
  nScopedMenu ( mm , this )                                      ;
  QAction * aa                                                   ;
  VarArgs   args                                                 ;
  ////////////////////////////////////////////////////////////////
  if (item->parent()==NULL)                                      {
    aa  = mm . add   ( 0 , item->text(0) )                       ;
    aa -> setEnabled ( false             )                       ;
        mm . addSeparator ( )                                    ;
    mm . add ( 1 , tr("MySQL Dump")            )                 ;
    mm . add ( 2 , tr("MySQL import sequence") )                 ;
    mm . add ( 3 , tr("Optimize")              )                 ;
    DockingMenu    ( mm   )                                      ;
    mm . setFont   ( plan )                                      ;
    aa = mm . exec (      )                                      ;
    if ( IsNull(aa)             ) return true                    ;
    if ( RunDocking ( mm , aa ) ) return true                    ;
    switch ( mm [ aa ] )                                         {
      case 1                                                     :
        Export         ( item )                                  ;
      break                                                      ;
      case 2                                                     :
        ImportSequence ( item )                                  ;
      break                                                      ;
      case 3                                                     :
        args << item->text(0)                                    ;
        start ( 10003 , args )                                   ;
      break                                                      ;
      default                                                    :
      break                                                      ;
    }                                                            ;
  } else                                                         {
    QTreeWidgetItem * ptem = item->parent()                      ;
    aa  = mm . add   ( 101 , item->text(0) )                     ;
    aa -> setEnabled ( false               )                     ;
          mm . addSeparator ( )                                  ;
    aa = mm . add    ( 201 , tr("Modify"  ) )                    ;
    aa = mm . add    ( 202 , tr("Items"   ) )                    ;
    aa = mm . add    ( 203 , tr("Optimize") )                    ;
    aa = mm . add    ( 204 , tr("Parse"   ) )                    ;
    aa = mm . add    ( 205 , tr("Organize") )                    ;
    DockingMenu ( mm )                                           ;
    mm . setFont   ( plan )                                      ;
    aa = mm . exec (      )                                      ;
    if ( IsNull(aa)             ) return true                    ;
    if ( RunDocking ( mm , aa ) ) return true                    ;
    switch ( mm [ aa ] )                                         {
      case 201                                                   :
        emit ModifyTable (ptem->text(0),item->text(0))           ;
      break                                                      ;
      case 202                                                   :
        emit TableItems  (ptem->text(0),item->text(0))           ;
      break                                                      ;
      case 203                                                   :
        args << ptem -> text ( 0            )                    ;
        args << item -> text ( 0            )                    ;
        start                ( 10002 , args )                    ;
      break                                                      ;
      case 204                                                   :
        emit ParseTable  ( ptem -> text    ( 0 )                 ,
                           item -> text    ( 0 )                 ,
                           item -> toolTip ( 0 )               ) ;
      break                                                      ;
      case 205                                                   :
        if (NotNull(ptem))                                       {
          VarArgs     Command                                    ;
          VarArgLists Commands                                   ;
          Command  << QString ( "Ordering"      )                ;
          Command  << ptem -> text ( 0 )                         ;
          Command  << item -> text ( 0 )                         ;
          Command  << 1000                                       ;
          Command  << QString ( "AutoIncrement" )                ;
          Commands << Command                                    ;
          emit RunSets ( Commands )                              ;
        }                                                        ;
      break                                                      ;
      default                                                    :
      break                                                      ;
    }                                                            ;
  }                                                              ;
  return true                                                    ;
}

void N::DatabaseDock::Export(QTreeWidgetItem * item)
{
  QString bat = QFileDialog::getSaveFileName      (
                  this                            ,
                  tr("Save mysqldump batch file") ,
                  plan->Path("sql")               ,
                  "*.bat"                       ) ;
  nDropOut ( bat.length() <= 0 )                  ;
  /////////////////////////////////////////////////
  QFile F ( bat )                                 ;
  if (!F.open(QIODevice::WriteOnly))              {
    Alert ( Error )                               ;
    return                                        ;
  }                                               ;
  /////////////////////////////////////////////////
  QTreeWidgetItem * Child                         ;
  QString           Dump                          ;
  for (int i=0;i<item->childCount();i++)          {
    Child = item->child(i)                        ;
    Dump  = QString                               (
              "mysqldump "
              "--net_buffer_length=16M "
              "--max-allowed-packet=768M "
              "-u %4 --password=\"%5\" "
              "%1 %2 > %3.sql\r\n"                )
            . arg ( item  -> text(0)              )
            . arg ( Child -> text(0)              )
            . arg ( Child -> text(0)              )
            . arg ( sql . Username                )
            . arg ( sql . Password              ) ;
    F . write ( Dump.toUtf8() )                   ;
  }                                               ;
  F . close (      )                              ;
  Alert     ( Done )                              ;
}

void N::DatabaseDock::ImportSequence(QTreeWidgetItem * item)
{
  QString bat = QFileDialog::getSaveFileName         (
                  this                               ,
                  tr("Save mysql import batch file") ,
                  plan->Path("Sql")                  ,
                  "*.bat"                          ) ;
  nDropOut ( bat.length() <= 0 )                     ;
  ////////////////////////////////////////////////////
  QFile F ( bat )                                    ;
  if (!F.open(QIODevice::WriteOnly))                 {
    Alert ( Error )                                  ;
    return                                           ;
  }                                                  ;
  ////////////////////////////////////////////////////
  QTreeWidgetItem * Child                            ;
  QString           Dump                             ;
  for (int i=0;i<item->childCount();i++)             {
    Child = item->child(i)                           ;
    Dump  = QString                                  (
              "mysql "
              "--net_buffer_length=16M "
              "--max-allowed-packet=768M "
              "-u %3 "
              "--password=\"%4\" "
              "%2 < %1.sql\r\n"                      )
            . arg ( Child -> text(0)                 )
            . arg ( "%1%"                            )
            . arg ( sql . Username                   )
            . arg ( sql . Password                 ) ;
    F . write ( Dump.toUtf8() )                      ;
  }                                                  ;
  F . close (      )                                 ;
  Alert     ( Done )                                 ;
}

bool N::DatabaseDock::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Refresh      , startup     ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  return true                                          ;
}

void N::DatabaseDock::Copy(void)
{
  QTreeWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item)           )    ;
  nDropOut ( IsNull(item->parent()) )    ;
  QString tt = item -> toolTip   (  0 )  ;
  nDropOut ( tt.length() <= 0       )    ;
  nSetClipboard ( tt )                   ;
}

void N::DatabaseDock::Optimize(QString database)
{
  QString     Q                                                              ;
  QStringList tables                                                         ;
  int         pid                                                            ;
  qint64      value = 0                                                      ;
  bool        go    = true                                                   ;
  EnterSQL ( SC , sql )                                                      ;
    Q = QString("use %1 ;").arg(database)                                    ;
    SC.Query(Q)                                                              ;
    Q = "show tables ;"                                                      ;
    SqlLoopNow(SC,Q)                                                         ;
      tables << SC.String(0)                                                 ;
    SqlLoopErr(SC,Q)                                                         ;
    SqlLoopEnd(SC,Q)                                                         ;
    if ( tables . count ( ) > 0 )                                            {
      pid = plan -> Progress ( tr("Optimize tables") , tr("%v/%m tables")  ) ;
      plan -> Start          ( pid , &value , &go                          ) ;
      plan -> setRange       ( pid , 0      , tables . count ( )           ) ;
      for (int i = 0 ; go && ( i < tables . count ( ) ) ; i++ )              {
        Q = QString("optimize table %1 ;" ).arg(tables[i])                   ;
        plan -> ProgressText ( pid , tables [ i ] )                          ;
        SC . Query ( Q )                                                     ;
        value++                                                              ;
        if ( isStopped ( ) ) go = false                                      ;
      }                                                                      ;
      plan -> Finish ( pid )                                                 ;
    }                                                                        ;
  LeaveSQL ( SC , sql )                                                      ;
  Alert    ( Done     )                                                      ;
}

void N::DatabaseDock::Optimize (
       QString database        ,
       QString table           )
{
  EnterSQL ( SC , sql )                            ;
    QString Q                                      ;
    Q = QString("use %1 ;").arg(database)          ;
    SC . Query ( Q )                               ;
    Q  = QString("optimize table %1 ;").arg(table) ;
    SC . Query ( Q )                               ;
  LeaveSQL ( SC , sql )                            ;
  Alert    ( Done     )                            ;
}

void N::DatabaseDock::Organize(SUID uuid)
{
  EnterSQL ( SC , plan -> sql )                                      ;
    sql = SC . GetConnection ( PlanTable ( SqlConnections ) , uuid ) ;
  LeaveSQL ( SC , plan -> sql )                                      ;
  Alert    ( Done             )                                      ;
  startup  (                  )                                      ;
}
