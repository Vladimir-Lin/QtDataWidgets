#include <datawidgets.h>

N::SqlServers:: SqlServers (QWidget * parent,Plan * p)
              : TreeDock   (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlServers::~SqlServers (void)
{
}

void N::SqlServers::Configure(void)
{
  Scope = ""                                             ;
  NewTreeWidgetItem            ( head                  ) ;
  setWindowTitle               ( tr("SQL Servers")     ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setAccessibleName            ( "N::SqlServers"       ) ;
  setColumnCount               ( 3                     ) ;
  head -> setText              ( 0, tr("Name"       )  ) ;
  head -> setText              ( 1, tr("Version"    )  ) ;
  head -> setText              ( 2, tr("Description")  ) ;
  assignHeaderItems            ( head                  ) ;
  MountClicked                 ( 2                     ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::SqlServers::FocusIn(void)
{
  LinkAction ( Insert , New    () ) ;
  LinkAction ( Font   , setFont() ) ;
  return true                       ;
}

bool N::SqlServers::FocusOut(void)
{
  return true ;
}

bool N::SqlServers::startup(void)
{
  clear    (                )                      ;
  EnterSQL ( SC , plan->sql )                      ;
    UUIDs   Uuids                                  ;
    SUID    uuid                                   ;
    QString Q                                      ;
    Uuids = SC.Uuids(PlanTable(SqlServers),"uuid"  ,
                     "order by id asc"           ) ;
    foreach (uuid,Uuids)                           {
      Q = SC.sql.SelectFrom                        (
        "name,version"                             ,
        PlanTable(SqlServers)                      ,
        SC.WhereUuid(uuid)                       ) ;
      if (SC.Fetch(Q))                             {
        QString name    = SC.String(0)             ;
        QString version = SC.String(1)             ;
        QString descrip = SC.getName               (
                            PlanTable(Names)       ,
                            "uuid"                 ,
                            plan->LanguageId       ,
                            uuid                 ) ;
        NewTreeWidgetItem ( item                 ) ;
        item -> setData   ( 0,Qt::UserRole,uuid  ) ;
        item -> setText   ( 0,name               ) ;
        item -> setText   ( 1,version            ) ;
        item -> setText   ( 2,descrip            ) ;
        addTopLevelItem   ( item                 ) ;
      }                                            ;
    }                                              ;
  LeaveSQL ( SC , plan->sql )                      ;
  Alert    ( Done           )                      ;
  return true                                      ;
}

void N::SqlServers::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID        uuid = nTreeUuid(item,0) ;
  QLineEdit * line                     ;
  removeOldItem()                      ;
  line = setLineEdit                   (
         item                          ,
         column                        ,
         SIGNAL(editingFinished())     ,
         SLOT  (nameFinished   ())   ) ;
  line->setFocus(Qt::TabFocusReason)   ;
}

void N::SqlServers::New(void)
{
  NewTreeWidgetItem(IT)         ;
  IT->setData(0,Qt::UserRole,0) ;
  addTopLevelItem (IT  )        ;
  scrollToItem    (IT  )        ;
  doubleClicked   (IT,0)        ;
}

void N::SqlServers::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return                         ;
  SUID    uuid    = nTreeUuid(ItemEditing,0)              ;
  QString name    = ItemEditing->text(0)                  ;
  QString version = ItemEditing->text(1)                  ;
  removeItemWidget (ItemEditing,ItemColumn)               ;
  if (uuid<=0 && name.length()<=0 && version.length()<=0) {
    int index = indexOfTopLevelItem(ItemEditing)          ;
    takeTopLevelItem(index)                               ;
  }                                                       ;
  ItemEditing = NULL                                      ;
  ItemWidget  = NULL                                      ;
  ItemColumn  = -1                                        ;
}

void N::SqlServers::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * line    = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line       )) return                         ;
  ItemEditing -> setText    ( ItemColumn , line->text() ) ;
  /////////////////////////////////////////////////////////
  SUID    uuid        = nTreeUuid(ItemEditing,0)          ;
  QString name        = ItemEditing->text(0)              ;
  QString version     = ItemEditing->text(1)              ;
  QString description = ItemEditing->text(2)              ;
  int     column      = ItemColumn                        ;
  int mode = 0                                            ;
  // set description
  if (uuid>0 && column==2 && description.length()>0)      {
    mode = 4                                              ;
  } else
  // add name and version
  if (uuid==0 && column<2                                &&
      name.length()>0 && version.length()>0)              {
    mode = 2                                              ;
  } else
  if (uuid> 0 && column<2                                &&
      name.length()>0 && version.length()>0)              {
    mode = 3                                              ;
  } else
  // ignore
  if (uuid==0 && (name.length()>0 || version.length()>0)) {
    mode = 1                                              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  switch (mode)                                           {
    case 0                                                :
      removeOldItem()                                     ;
    return                                                ;
    case 1                                                :
      removeOldItem()                                     ;
    return                                                ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  CodeManager CM ( plan )                                 ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    switch (mode)                                         {
      case 2                                              :
        uuid = SC.Unique                                  (
                 PlanTable(MajorUuid)                     ,
                 "uuid"                                   ,
                 9002                                   ) ;
        SC.assureUuid(PlanTable(MajorUuid),uuid           ,
                      Types::SqlServer                  ) ;
        ItemEditing->setData(0,Qt::UserRole,uuid)         ;
        removeOldItem()                                   ;
        Q = SC.sql.InsertInto(PlanTable(SqlServers)       ,
                        3,"uuid","name","version" )       ;
        SC . Prepare ( Q                          )       ;
        SC . Bind    ( "uuid"   ,uuid             )       ;
        SC . Bind    ( "name"   ,name   .toUtf8() )       ;
        SC . Bind    ( "version",version.toUtf8() )       ;
        SC . Exec    (                            )       ;
      break                                               ;
      case 3                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update(PlanTable(SqlServers)           ,
                          "where uuid = :UUID"            ,
                          2,"name","version"      )       ;
        SC . Prepare ( Q                          )       ;
        SC . Bind    ( "uuid"   ,uuid             )       ;
        SC . Bind    ( "name"   ,name   .toUtf8() )       ;
        SC . Bind    ( "version",version.toUtf8() )       ;
        SC . Exec    (                            )       ;
      break                                               ;
      case 4                                              :
        removeOldItem()                                   ;
        CM.assureName                                     (
          SC                                              ,
          uuid                                            ,
          plan->LanguageId                                ,
          description                                   ) ;
      break                                               ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}

bool N::SqlServers::Menu(QPoint pos)
{ Q_UNUSED ( pos )                                             ;
  QMdiSubWindow * mdi    = Casting(QMdiSubWindow,parent())     ;
  QDockWidget   * dock   = Casting(QDockWidget  ,parent())     ;
  MenuManager     mm (this)                                    ;
  QAction       * a                                            ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont(plan)                                           ;
  a = mm.exec()                                                ;
  if (IsNull(a)) return true                                   ;
  switch (mm[a])                                               {
    case 1000001                                               :
      emit attachMdi (this,Qt::Vertical)                       ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::RightDockWidgetArea                                ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea       ) ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
