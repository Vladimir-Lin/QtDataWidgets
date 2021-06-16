#include <datawidgets.h>

N::ParamentEditor:: ParamentEditor ( QWidget * parent , Plan * p )
                  : TreeWidget     (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ParamentEditor::~ParamentEditor (void)
{
}

void N::ParamentEditor::Configure(void)
{
  NewTreeWidgetItem            ( head                       ) ;
  setWindowTitle               ( tr("Parament editor")      ) ;
  setDragDropMode              ( NoDragDrop                 ) ;
  setRootIsDecorated           ( false                      ) ;
  setAlternatingRowColors      ( true                       ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded      ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded      ) ;
  setAccessibleName            ( "nParamentEditor"          ) ;
  setColumnCount               ( 9                          ) ;
  head -> setText              ( 0, tr("Name"      )        ) ;
  head -> setText              ( 1, tr("Type"      )        ) ;
  head -> setText              ( 2, tr("Scope"     )        ) ;
  head -> setText              ( 3, tr("Identifier")        ) ;
  head -> setText              ( 4, tr("Variety"   )        ) ;
  head -> setText              ( 5, tr("Value"     )        ) ;
  head -> setText              ( 6, tr("Floating"  )        ) ;
  head -> setText              ( 7, tr("Data"      )        ) ;
  head -> setText              ( 8, ""                      ) ;
  assignHeaderItems            ( head                       ) ;
  setAlignment                 ( 5 , Qt::AlignRight           |
                                     Qt::AlignVCenter       ) ;
  setAlignment                 ( 6 , Qt::AlignRight           |
                                     Qt::AlignVCenter       ) ;
  plan -> setFont              ( this                       ) ;
  MountClicked                 ( 2                          ) ;
  /////////////////////////////////////////////////////////////
  IntValidator = new QIntValidator(0,1000,this)               ;
}

bool N::ParamentEditor::FocusIn(void)
{
  LinkAction ( Refresh , startup () ) ;
  LinkAction ( Insert  , Insert  () ) ;
  LinkAction ( Rename  , Rename  () ) ;
  LinkAction ( Font    , setFont () ) ;
  return true                         ;
}

void N::ParamentEditor::run(int T,ThreadData * d)
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

bool N::ParamentEditor::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::ParamentEditor::startup(void)
{
  clear    (                )                          ;
  EnterSQL ( SC , plan->sql )                          ;
    QString Q                                          ;
    UUIDs   Uuids                                      ;
    SUID    uu                                         ;
    Q = SC.sql.SelectFrom                              (
          "id,uuid",PlanTable(ParamentTypes)           ,
          "order by id asc"                          ) ;
    SqlLoopNow(SC,Q)                                   ;
      int  id   = SC . Int  (0)                        ;
      uu        = SC . Uuid (1)                        ;
      MapParaments[id] = uu                            ;
      ParamentMaps[uu] = id                            ;
      ParamentUuids   << uu                            ;
    SqlLoopErr(SC,Q)                                   ;
    SqlLoopEnd(SC,Q)                                   ;
    foreach (uu,ParamentUuids)                         {
      ParamentNames[uu] = SC.getName                   (
        PlanTable(Names),"uuid"                        ,
        vLanguageId , uu                             ) ;
    }                                                  ;
    Uuids = SC.Uuids(PlanTable(Paraments)              ,
              "uuid" , "order by id asc"             ) ;
    foreach (uu,Uuids)                                 {
      QString name                                     ;
      QString identifier                               ;
      QString scope                                    ;
      QString data                                     ;
      int     t                                        ;
      int     variety                                  ;
      TUID    value                                    ;
      double  floating                                 ;
      SUID    puid                                     ;
      name = SC.getName                                (
        PlanTable(Names) , "uuid"                      ,
        vLanguageId , uu                             ) ;
      Q = SC.sql.SelectFrom                            (
            "type,variety,value,floating,"
            "scope,name,data"                          ,
          PlanTable(Paraments) , SC.WhereUuid(uu)    ) ;
      if (SC.Fetch(Q))                                 {
        NewTreeWidgetItem(item)                        ;
        t          = SC . Int   ( 0 )                  ;
        variety    = SC . Int   ( 1 )                  ;
        value      = SC . Tuid  ( 2 )                  ;
        floating   = SC . Value ( 3 ) . toDouble ()    ;
        scope      = SC . String( 4 )                  ;
        identifier = SC . String( 5 )                  ;
        data       = SC . String( 6 )                  ;
        puid       = MapParaments[t]                   ;
        item -> setData ( 0,Qt::UserRole,uu          ) ;
        item -> setData ( 1,Qt::UserRole,t           ) ;
        item -> setText ( 0,name                     ) ;
        item -> setText ( 1,ParamentNames[puid]      ) ;
        item -> setText ( 2,scope                    ) ;
        item -> setText ( 3,identifier               ) ;
        item -> setText ( 4,QString::number(variety) ) ;
        item -> setText ( 5,QString::number(value  ) ) ;
        item -> setText ( 6,QString::number(floating)) ;
        item -> setText ( 7,data                     ) ;
        setAlignments   ( item                       ) ;
        addTopLevelItem ( item                       ) ;
      }                                                ;
    }                                                  ;
  LeaveSQL ( SC , plan->sql )                          ;
  SuitableColumns (         )                          ;
  reportItems     (         )                          ;
  Alert           ( Done    )                          ;
  return true                                          ;
}

void N::ParamentEditor::Insert(void)
{
  NewTreeWidgetItem ( IT )                      ;
  IT->setData(0,Qt::UserRole,0)                 ;
  IT->setData(1,Qt::UserRole,0)                 ;
  IT->setText(1,ParamentNames[MapParaments[0]]) ;
  setAlignments   ( IT     )                    ;
  addTopLevelItem ( IT     )                    ;
  scrollToItem    ( IT     )                    ;
  doubleClicked   ( IT , 0 )                    ;
}

void N::ParamentEditor::Rename(void)
{
  QTreeWidgetItem * item = currentItem()   ;
  nDropOut ( IsNull(item) )                ;
  doubleClicked ( item , currentColumn() ) ;
}

void N::ParamentEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID        uu = nTreeUuid(item,0)                          ;
  int         t  = nTreeInt (item,1)                          ;
  QLineEdit * line                                            ;
  QComboBox * types                                           ;
  removeOldItem()                                             ;
  switch (column)                                             {
    case 1                                                  : {
      nDropOut ( uu <= 0 )                                    ;
      types = new QComboBox(this)                             ;
      N::AddItems(*types,ParamentUuids,ParamentNames)         ;
      (*types) <= MapParaments[t]                             ;
      setItemWidget ( item , column , types )                 ;
      ItemEditing = item                                      ;
      ItemColumn  = column                                    ;
      ItemWidget  = types                                     ;
      connect(types , SIGNAL (activated   (int))              ,
              this  , SLOT   (typeFinished(int))            ) ;
      types -> showPopup()                                    ;
    }                                                         ;
    break                                                     ;
    case 4                                                    :
      line = setLineEdit                                      (
             item                                             ,
             column                                           ,
             SIGNAL(editingFinished())                        ,
             SLOT  (nameFinished   ()) )                      ;
      line->setValidator(IntValidator) ;
      line->setFocus(Qt::TabFocusReason)                      ;
    break                                                     ;
    default                                                   :
      line = setLineEdit                                      (
             item                                             ,
             column                                           ,
             SIGNAL(editingFinished())                        ,
             SLOT  (nameFinished   ()) )                      ;
      line->setFocus(Qt::TabFocusReason)                      ;
    break                                                     ;
  }                                                           ;
}

void N::ParamentEditor::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * line    = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line       )) return                         ;
  ItemEditing -> setText    ( ItemColumn , line->text() ) ;
  /////////////////////////////////////////////////////////
  QIcon   icon   = QIcon(":/images/yes.png")              ;
  SUID    uu     = nTreeUuid(ItemEditing,0)               ;
  QString name   = line->text()                           ;
  int     column = ItemColumn                             ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    switch (column)                                       {
      case 0                                              :
        if (uu<=0 && name.length()>0)                     {
          uu = SC.Unique                                  (
                   PlanTable(MajorUuid)                   ,
                   "uuid"                                 ,
                   8816                                 ) ;
          SC.assureUuid                                   (
            PlanTable(MajorUuid)                          ,
            uu                                            ,
            Types::Parament                             ) ;
          SC.insertUuid(PlanTable(Paraments),uu,0)        ;
          ItemEditing->setData(0,Qt::UserRole,uu)         ;
        }                                                 ;
        if (uu>0)                                         {
          SC.assureName                                   (
            PlanTable(Names)                              ,
            uu                                            ,
            vLanguageId                                   ,
            name                                        ) ;
        }                                                 ;
        removeOldItem(true,0)                             ;
      break                                               ;
      case 2                                              :
        Q = SC.sql.Update(PlanTable(Paraments)            ,
              "where uuid = :UUID",1,"scope"            ) ;
        SC . Prepare  ( Q                               ) ;
        SC . Bind     ( "uuid"  , uu                    ) ;
        SC . Bind     ( "scope" , name.toUtf8()         ) ;
        SC . Exec     (                                 ) ;
        removeOldItem (                                 ) ;
      break                                               ;
      case 3                                              :
        Q = SC.sql.Update(PlanTable(Paraments)            ,
              "where uuid = :UUID",1,"name"             ) ;
        SC . Prepare  ( Q                               ) ;
        SC . Bind     ( "uuid" , uu                     ) ;
        SC . Bind     ( "name" , name.toUtf8()          ) ;
        SC . Exec     (                                 ) ;
        removeOldItem (                                 ) ;
      break                                               ;
      case 4                                              :
        Q = SC.sql.Update(PlanTable(Paraments)            ,
              "where uuid = :UUID",1,"variety"          ) ;
        SC . Prepare  ( Q                               ) ;
        SC . Bind     ( "uuid"    , uu                  ) ;
        SC . Bind     ( "variety" , name.toInt()        ) ;
        SC . Exec     (                                 ) ;
        removeOldItem (                                 ) ;
      break                                               ;
      case 5                                              :
        Q = SC.sql.Update(PlanTable(Paraments)            ,
              "where uuid = :UUID",1,"value"            ) ;
        SC . Prepare  ( Q                               ) ;
        SC . Bind     ( "uuid"  , uu                    ) ;
        SC . Bind     ( "value" , name.toInt()          ) ;
        SC . Exec     (                                 ) ;
        removeOldItem (                                 ) ;
      break                                               ;
      case 6                                              :
        Q = SC.sql.Update(PlanTable(Paraments)            ,
              "where uuid = :UUID",1,"floating"         ) ;
        SC . Prepare  ( Q                               ) ;
        SC . Bind     ( "uuid"     , uu                 ) ;
        SC . Bind     ( "floating" , name.toDouble()    ) ;
        SC . Exec     (                                 ) ;
        removeOldItem (                                 ) ;
      break                                               ;
      case 7                                              :
        Q = SC.sql.Update(PlanTable(Paraments)            ,
              "where uuid = :UUID",1,"data"             ) ;
        SC . Prepare  ( Q                               ) ;
        SC . Bind     ( "uuid" , uu                     ) ;
        SC . Bind     ( "data" , name.toUtf8()          ) ;
        SC . Exec     (                                 ) ;
        removeOldItem (                                 ) ;
      break                                               ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}

void N::ParamentEditor::typeFinished(int index)
{
  nDropOut ( IsNull(ItemEditing) )                        ;
  QComboBox * types = Casting(QComboBox ,ItemWidget)      ;
  if (IsNull(types)) return                               ;
  SUID T  = N::GetUuid(types)                             ;
  SUID uu = nTreeUuid(ItemEditing,0)                      ;
  int  t  = ParamentMaps[T]                               ;
  ItemEditing->setText(ItemColumn,types->itemText(index)) ;
  ItemEditing->setData(ItemColumn,Qt::UserRole,t)         ;
  EnterSQL       ( SC , plan->sql )                       ;
    QString Q                                             ;
    Q = SC.sql.Update(PlanTable(Paraments)                ,
             "where uuid = :UUID"                         ,
             1,"type"                                   ) ;
    SC . Prepare ( Q              )                       ;
    SC . Bind    ( "uuid" , uu    )                       ;
    SC . Bind    ( "type" , t     )                       ;
    SC . Exec    (                )                       ;
  LeaveSQL       ( SC , plan->sql )                       ;
  removeOldItem  (                )                       ;
  Alert          ( Done                   )               ;
}

bool N::ParamentEditor::Menu(QPoint pos)
{
  return false ;
}
