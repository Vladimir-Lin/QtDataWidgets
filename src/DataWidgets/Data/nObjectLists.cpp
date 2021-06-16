#include <datawidgets.h>

N::ObjectLists :: ObjectLists ( QWidget * parent , Plan * p )
                : TreeDock    (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ObjectLists ::~ObjectLists(void)
{
}

void N::ObjectLists::Configure(void)
{
  NewTreeWidgetItem            ( head                   )        ;
  head -> setText              ( 0 , tr("Name"        ) )        ;
  head -> setText              ( 1 , tr("Type"        ) )        ;
  head -> setText              ( 2 , tr("UUID"        ) )        ;
  head -> setText              ( 3 , ""                 )        ;
  for (int i=0;i<3;i++)                                          {
    head -> setTextAlignment   ( i , Qt::AlignCenter    )        ;
  }                                                              ;
  setWindowTitle               ( tr("Object lists"    ) )        ;
  setDragDropMode              ( NoDragDrop             )        ;
  setRootIsDecorated           ( false                  )        ;
  setAlternatingRowColors      ( true                   )        ;
  setSelectionMode             ( SingleSelection        )        ;
  setColumnCount               ( 4                      )        ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  )        ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  )        ;
  setHeaderItem                ( head                   )        ;
  plan -> setFont              ( this                   )        ;
  ////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::ObjectLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  return true                                              ;
}

void N::ObjectLists::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::ObjectLists::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::ObjectLists::startup(UUIDs & Uuids,RMAPs & Types,NAMEs & Names)
{
  NAMEs TN                                     ;
  SUID  uuid                                   ;
  clear            (                         ) ;
  EnterSQL         ( SC , plan->sql          ) ;
    UUIDs Xuids = SC.Uuids                     (
           PlanTable(Types),"type"             ,
           "where flags != 0 order by id asc") ;
    foreach (uuid,Xuids)                       {
      TN [ uuid ] = SC.getName                 (
                      PlanTable(Names)         ,
                      "uuid"                   ,
                      vLanguageId         ,
                      uuid                   ) ;
    }                                          ;
  LeaveSQL         ( SC , plan->sql          ) ;
  foreach (uuid,Uuids)                         {
    int     type = Types[uuid]                 ;
    QString name                               ;
    if (Names.contains(uuid))                  {
      name = Names[uuid]                       ;
    }                                          ;
    NewTreeWidgetItem(item)                    ;
    item->setData(0,Qt::UserRole,uuid    )     ;
    item->setData(1,Qt::UserRole,type    )     ;
    item->setText(0,name                 )     ;
    item->setText(1,TN[type]             )     ;
    item->setText(2,QString::number(uuid))     ;
    addTopLevelItem(item)                      ;
  }                                            ;
  reportItems      (                         ) ;
  emit AutoFit     (                         ) ;
  Alert            ( Done                    ) ;
  return true                                  ;
}

void N::ObjectLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID uuid = nTreeUuid(item,0) ;
  int  type = nTreeInt (item,1) ;
}

bool N::ObjectLists::Menu(QPoint pos)
{ Q_UNUSED        ( pos       )             ;
  nScopedMenu     ( mm , this )             ;
  QAction    * aa                           ;
  DockingMenu     ( mm        )             ;
  mm . setFont    ( plan      )             ;
  aa = mm . exec  (           )             ;
  if ( IsNull     (      aa ) ) return true ;
  if ( RunDocking ( mm , aa ) ) return true ;
  return true                               ;
}
