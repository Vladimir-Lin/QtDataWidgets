#include <datawidgets.h>

N::SqlRelations:: SqlRelations ( QWidget * parent , Plan * p )
                : TreeDock     (           parent ,        p )
                , Object       ( 0 , Types::SqlPlan          )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlRelations::~SqlRelations(void)
{
}

void N::SqlRelations::Configure(void)
{
  Scope = ""                                                         ;
  NewTreeWidgetItem            ( head                              ) ;
  setWindowTitle               ( tr("Database column constraints") ) ;
  setDragDropMode              ( NoDragDrop                        ) ;
  setRootIsDecorated           ( false                             ) ;
  setAlternatingRowColors      ( true                              ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded             ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded             ) ;
  setAccessibleName            ( "nSqlConstraints"                 ) ;
  setColumnCount               ( 6                                 ) ;
  head -> setText              ( 0, tr("Name"       )              ) ;
  head -> setText              ( 1, tr("Constraint" )              ) ;
  head -> setText              ( 2, tr("Items"      )              ) ;
  head -> setText              ( 3, tr("Reference"  )              ) ;
  head -> setText              ( 4, tr("Maps"       )              ) ;
  head -> setText              ( 5, ""                             ) ;
  assignHeaderItems            ( head                              ) ;
  MountClicked                 ( 2                                 ) ;
  plan -> setFont              ( this                              ) ;
}

bool N::SqlRelations::FocusIn(void)
{
  AssignAction ( Label   , windowTitle ( ) ) ;
  LinkAction   ( Refresh , startup     ( ) ) ;
  LinkAction   ( Insert  , New         ( ) ) ;
  LinkAction   ( Font    , setFont     ( ) ) ;
  if (uuid>0)                                {
    LinkAction ( Delete  , Delete      ( ) ) ;
  }                                          ;
  return true                                ;
}

bool N::SqlRelations::FocusOut(void)
{
  return true ;
}

QString N::SqlRelations::addReverseQuote(QString item)
{
  QString N                                                  ;
  if (item.contains('('))                                    {
    int     index     = item.indexOf('(')                    ;
    QString key       = item.left(index)                     ;
    QString paraments = item.mid (index,item.length()-index) ;
    N = QString("`%1`%2").arg(key).arg(paraments)            ;
  } else                                                     {
    N = QString("`%1`").arg(item)                            ;
  }                                                          ;
  return  N                                                  ;
}

QString N::SqlRelations::toReverseQuotes(QString item)
{
  QStringList L = item.split(' ') ;
  QStringList R                   ;
  for (int i=0;i<L.count();i++)   {
    R << addReverseQuote(L[i])    ;
  }                               ;
  return R.join(",")              ;
}

UUIDs N::SqlRelations::GetUuids(SqlConnection & SC)
{
  UUIDs Uuids                                         ;
  if (uuid<=0)                                        {
    Uuids = SC.Uuids(PlanTable(SqlConstraints),"uuid" ,
                     "order by id asc"              ) ;
  } else                                              {
    GroupItems GI ( plan )                            ;
    Uuids = GI.Subordination                          (
              SC                                      ,
              uuid                                    ,
              type                                    ,
              Types::SqlConstraint                    ,
              Groups::Subordination                   ,
              "order by position asc"               ) ;
  }                                                   ;
  return Uuids                                        ;
}

QTreeWidgetItem * N::SqlRelations::GetItem(SqlConnection & SC,SUID u)
{
  QTreeWidgetItem * result = NULL              ;
  QIcon   icon = QIcon(":/images/yes.png")     ;
  QString Q                                    ;
  Q = SC.sql.SelectFrom                        (
    "name,symbol,items,reference,maps"         ,
    PlanTable(SqlConstraints)                  ,
    SC.WhereUuid(u)                          ) ;
  if (SC.Fetch(Q))                             {
    QString boundary  = ""                     ;
    QString name      = SC.String(0)           ;
    QString symbol    = SC.String(1)           ;
    QString items     = SC.String(2)           ;
    QString reference = SC.String(3)           ;
    QString maps      = SC.String(4)           ;
    NewTreeWidgetItem ( item                 ) ;
    item -> setData   ( 0,Qt::UserRole,u     ) ;
    item -> setIcon   ( 0,icon               ) ;
    item -> setText   ( 0,name               ) ;
    if (symbol.toLower()=="unique")            {
      item -> setData ( 1,Qt::UserRole,1     ) ;
      item -> setText ( 1,tr("Unique")       ) ;
    } else
    if (symbol.toLower()=="key")               {
      item -> setData ( 1,Qt::UserRole,2     ) ;
      item -> setText ( 1,tr("Key"   )       ) ;
    } else
    if (symbol.toLower()=="foreign")           {
      item -> setData ( 1,Qt::UserRole,3     ) ;
      item -> setText ( 1,tr("Foreign key")  ) ;
    }                                          ;
    item -> setText   ( 2,items              ) ;
    item -> setText   ( 3,reference          ) ;
    item -> setText   ( 4,maps               ) ;
    result = item                              ;
    setAlignments     ( item               )   ;
    addTopLevelItem   ( item               )   ;
    if (symbol.toLower()=="unique")            {
      QString idx = toReverseQuotes(items)     ;
      boundary = QString("unique key `%1` (%2)")
                 .arg(name).arg(idx)           ;
    } else
    if (symbol.toLower()=="key")               {
      QString idx = toReverseQuotes(items)     ;
      boundary = QString("key `%1` (%2)"       )
                 .arg(name).arg(idx)           ;
    } else
    if (symbol.toLower()=="foreign")           {
      QString idx = toReverseQuotes(items)     ;
      QString map = toReverseQuotes(maps )     ;
      boundary = QString("constraint `%1` foreign key (%2) references `%3` (%4)")
                 .arg(name                     )
                 .arg(idx                      )
                 .arg(reference                )
                 .arg(map)                     ;
    }                                          ;
    Items << boundary                          ;
  }                                            ;
  return result                                ;
}

SUID N::SqlRelations::GetSqlItem(SqlConnection & SC,SqlParser & Parser,int i)
{
  QString Q                                         ;
  SUID    u          = 0                            ;
  QString name       = Parser.Conditions [ i    ]   ;
  QString symbol     = Parser.Symbols    [ name ]   ;
  QString Remains    = Parser.Remains    [ name ]   ;
  if (Parser.isForeign(i))                          {
    QStringList R = Parser.References(i)            ;
    QStringList members                             ;
    QString     L = R[1]                            ;
    QString     items                               ;
    QString     maps                                ;
    members = Parser.Members(R[0])                  ;
    items   = members.join(" ")                     ;
    L       = Parser.Undress(L,'`')                 ;
    members = Parser.Members(R[2])                  ;
    maps    = members.join(" ")                     ;
    symbol  = "foreign"                             ;
    Q = SC.sql.SelectFrom                           (
          "uuid"                                    ,
          PlanTable(SqlConstraints)                 ,
          SC.sql.Where                              (
            5                                       ,
            "name"                                  ,
            "symbol"                                ,
            "items"                                 ,
            "reference"                             ,
            "maps"                              ) ) ;
    SC . Prepare (Q                               ) ;
    SC . Bind    ("name"     ,name   . toUtf8()   ) ;
    SC . Bind    ("symbol"   ,symbol . toUtf8()   ) ;
    SC . Bind    ("items"    ,items  . toUtf8()   ) ;
    SC . Bind    ("reference",L      . toUtf8()   ) ;
    SC . Bind    ("maps"     ,maps   . toUtf8()   ) ;
    if (SC.Exec() && SC.Next())                     {
      u = SC.Uuid(0)                                ;
    }                                               ;
  } else                                            {
    QStringList members                             ;
    QString     items                               ;
    members = Parser.Members(Remains)               ;
    items   = members.join(" ")                     ;
    Q = SC.sql.SelectFrom                           (
          "uuid"                                    ,
          PlanTable(SqlConstraints)                 ,
          SC.sql.Where                              (
            3                                       ,
            "name"                                  ,
            "symbol"                                ,
            "items"                             ) ) ;
    SC . Prepare (Q                               ) ;
    SC . Bind    ("name"  ,name   . toUtf8()      ) ;
    SC . Bind    ("symbol",symbol . toUtf8()      ) ;
    SC . Bind    ("items" ,items  . toUtf8()      ) ;
    if (SC.Exec() && SC.Next())                     {
      u = SC.Uuid(0)                                ;
    }                                               ;
  }                                                 ;
  return u                                          ;
}

void N::SqlRelations::addParserItem(SqlConnection & SC,SqlParser & Parser,int i)
{
  NewTreeWidgetItem ( item )                     ;
  QIcon   icon    = QIcon(":/images/help.png")   ;
  QString name    = Parser . Conditions [ i    ] ;
  QString symbol  = Parser . Symbols    [ name ] ;
  QString remains = Parser . Remains    [ name ] ;
  item -> setData ( 0 , Qt::UserRole , 0 )       ;
  item -> setIcon ( 0 , icon             )       ;
  item -> setText ( 0 , name             )       ;
  if (Parser.isForeign(i))                       {
    QStringList R = Parser.References(i)         ;
    item -> setData ( 1 , Qt::UserRole , 3 )     ;
    item -> setText ( 1 , tr("Foreign key"))     ;
    if (R.count()>=3)                            {
      QStringList members                        ;
      QString     L = R[1]                       ;
      members = Parser.Members(R[0])             ;
      item -> setText ( 2 , members.join(" ")  ) ;
      L = Parser.Undress(L,'`')                  ;
      item -> setText ( 3 , L                  ) ;
      members = Parser.Members(R[2])             ;
      item -> setText ( 4 , members.join(" ")  ) ;
    }                                            ;
  } else                                         {
    QStringList members                          ;
    if (symbol=="unique")                        {
      item -> setData ( 1 , Qt::UserRole , 1 )   ;
      item -> setText ( 1 , tr("Unique"    ) )   ;
      members = Parser.Members(remains)          ;
    } else
    if (symbol=="key")                           {
      item -> setData ( 1 , Qt::UserRole , 2 )   ;
      item -> setText ( 1 , tr("Key"       ) )   ;
      members = Parser.Members(remains)          ;
    }                                            ;
    if (members.count()>0)                       {
      QString L = members.join(" ")              ;
      item -> setText ( 2 , L )                  ;
    }                                            ;
  }                                              ;
  addTopLevelItem ( item )                       ;
  Items << Parser . boundary(i)                  ;
}

bool N::SqlRelations::startup(void)
{
  clear         (           ) ;
  Items . clear (           ) ;
  EnterSQL ( SC , plan->sql ) ;
    UUIDs   Uuids             ;
    SUID    u                 ;
    Uuids = GetUuids ( SC )   ;
    foreach (u,Uuids)         {
      GetItem ( SC , u )      ;
    }                         ;
  LeaveSQL ( SC , plan->sql ) ;
  SuitableColumns (         ) ;
  reportItems     (         ) ;
  Alert           ( Done    ) ;
  return true                 ;
}

void N::SqlRelations::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID        u = nTreeUuid(item,0)                          ;
  int         t = nTreeInt (item,1)                          ;
  QLineEdit * line                                           ;
  QComboBox * types                                          ;
  removeOldItem()                                            ;
  switch (column)                                            {
    case 0                                                   :
      line = setLineEdit                                     (
             item                                            ,
             column                                          ,
             SIGNAL(editingFinished())                       ,
             SLOT  (nameFinished   ()) )                     ;
      line->setFocus(Qt::TabFocusReason)                     ;
    break                                                    ;
    case 1                                                 : {
      nDropOut ( u <= 0 )                                    ;
      types = new QComboBox(this)                            ;
      types->addItem(tr("Unique"     ),1)                    ;
      types->addItem(tr("Key"        ),2)                    ;
      types->addItem(tr("Foreign key"),3)                    ;
      (*types) <= t                                          ;
      setItemWidget ( item , column , types )                ;
      ItemEditing = item                                     ;
      ItemColumn  = column                                   ;
      ItemWidget  = types                                    ;
      connect(types , SIGNAL (activated  (int))              ,
              this  , SLOT   (typeChanged(int))            ) ;
      types -> showPopup()                                   ;
    }                                                        ;
    break                                                    ;
    default                                                  :
      nDropOut ( u <= 0 )                                    ;
      line = setLineEdit                                     (
             item                                            ,
             column                                          ,
             SIGNAL(editingFinished())                       ,
             SLOT  (nameFinished   ()) )                     ;
      line->setFocus(Qt::TabFocusReason)                     ;
    break                                                    ;
  }                                                          ;
}

void N::SqlRelations::New(void)
{
  NewTreeWidgetItem ( IT )      ;
  IT->setData(0,Qt::UserRole,0) ;
  IT->setData(1,Qt::UserRole,2) ;
  IT->setText(1,tr("Key")     ) ;
  IT->setText(2,"-"           ) ;
  setAlignments   ( IT        ) ;
  addTopLevelItem ( IT        ) ;
  scrollToItem    ( IT        ) ;
  doubleClicked   ( IT , 0    ) ;
}

void N::SqlRelations::Delete(void)
{
  nDropOut ( uuid <= 0    )              ;
  QTreeWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item) )              ;
  SUID  u = nTreeUuid(item,0)            ;
  UUIDs Uuids                            ;
  Uuids << u                             ;
  GroupItems  GI ( plan )                ;
  EnterSQL ( SC , plan->sql )            ;
    GI.Detach                            (
      SC                                 ,
      uuid                               ,
      type                               ,
      Types::SqlConstraint               ,
      Groups::Subordination              ,
      Uuids                            ) ;
  LeaveSQL ( SC , plan->sql )            ;
  ////////////////////////////////////////
  int index = indexOfTopLevelItem(item)  ;
  if (index<0)                           {
    takeTopLevelItem ( index )           ;
    Alert            ( Error )           ;
  } else                                 {
    Alert ( Done  )                      ;
  }                                      ;
}

void N::SqlRelations::DeleteAll(void)
{
  UUIDs Uuids                            ;
  SUID  u                                ;
  QTreeWidgetItem * item                 ;
  nFullLoop(i,topLevelItemCount())       {
    item = topLevelItem(i)               ;
    u    = nTreeUuid(item,0)             ;
    Uuids << u                           ;
  }                                      ;
  GroupItems GI ( plan )                 ;
  EnterSQL ( SC , plan->sql )            ;
    GI.Detach                            (
      SC                                 ,
      uuid                               ,
      type                               ,
      Types::SqlConstraint               ,
      Groups::Subordination              ,
      Uuids                            ) ;
  LeaveSQL ( SC , plan->sql )            ;
  ////////////////////////////////////////
  clear (      )                         ;
  Alert ( Done )                         ;
}

void N::SqlRelations::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  SUID u = nTreeUuid(ItemEditing,0)         ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  if (u==0) takeItem(ItemEditing)           ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::SqlRelations::typeChanged(int index)
{
  if (IsNull(ItemEditing)) return                         ;
  QComboBox * types = Casting(QComboBox ,ItemWidget)      ;
  if (IsNull(types)) return                               ;
  int  Type = types->itemData(index,Qt::UserRole).toInt() ;
  SUID u    = nTreeUuid(ItemEditing,0)                    ;
  ItemEditing->setText(ItemColumn,types->itemText(index)) ;
  ItemEditing->setData(ItemColumn,Qt::UserRole,Type)      ;
  EnterSQL       ( SC , plan->sql             )           ;
    QString Q                                             ;
    QString symbol = ""                                   ;
    switch (Type)                                         {
      case 0: symbol = "-"       ; break                  ;
      case 1: symbol = "unique"  ; break                  ;
      case 2: symbol = "key"     ; break                  ;
      case 3: symbol = "foreign" ; break                  ;
    }                                                     ;
    Q = SC.sql.Update(PlanTable(SqlConstraints)           ,
             "where uuid = :UUID"                         ,
             1,"symbol"                                 ) ;
    SC . Prepare ( Q                          )           ;
    SC . Bind    ( "uuid"   , u               )           ;
    SC . Bind    ( "symbol" , symbol.toUtf8() )           ;
    SC . Exec    (                            )           ;
  LeaveSQL       ( SC , plan->sql             )           ;
  removeOldItem  (                            )           ;
  emit Changed   ( uuid,type,u,1              )           ;
  Alert          ( Done                       )           ;
}

void N::SqlRelations::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * line    = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line       )) return                         ;
  ItemEditing -> setText    ( ItemColumn , line->text() ) ;
  /////////////////////////////////////////////////////////
  QIcon   icon   = QIcon(":/images/yes.png")              ;
  SUID    u      = nTreeUuid(ItemEditing,0)               ;
  int     t      = nTreeInt (ItemEditing,1)               ;
  QString name   = line->text()                           ;
  int     column = ItemColumn                             ;
  GroupItems  GI ( plan )                                 ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    switch (column)                                       {
      case 0                                              :
        if (u<=0 && name.length()>0)                      {
          UUIDs   Juids                                   ;
          QString symbol                                  ;
          QString items                                   ;
          QString reference                               ;
          QString maps                                    ;
          switch (t)                                      {
            case 1: symbol = "unique"  ; break            ;
            case 2: symbol = "key"     ; break            ;
            case 3: symbol = "foreign" ; break            ;
          }                                               ;
          items     = ItemEditing->text(2)                ;
          reference = ItemEditing->text(3)                ;
          maps      = ItemEditing->text(4)                ;
          u = SC.Unique                                   (
                PlanTable(MajorUuid)                      ,
                "uuid"                                    ,
                9009                                    ) ;
          SC.assureUuid                                   (
                PlanTable(MajorUuid)                      ,
                u                                         ,
                Types::SqlConstraint                    ) ;
          Juids << u                                      ;
          GI.Join                                         (
            SC                                            ,
            uuid                                          ,
            type                                          ,
            Types::SqlConstraint                          ,
            Groups::Subordination                         ,
            0,Juids                                     ) ;
          ItemEditing->setData(0,Qt::UserRole,u)          ;
          ItemEditing->setIcon(0,icon)                    ;
          Q = SC.sql.InsertInto                           (
                   PlanTable(SqlConstraints)              ,
                   6                                      ,
                   "uuid"                                 ,
                   "name"                                 ,
                   "symbol"                               ,
                   "items"                                ,
                   "reference"                            ,
                   "maps"                               ) ;
          SC . Prepare ( Q                              ) ;
          SC . Bind    ( "uuid"     , u                 ) ;
          SC . Bind    ( "name"     , name     .toUtf8()) ;
          SC . Bind    ( "symbol"   , symbol   .toUtf8()) ;
          SC . Bind    ( "items"    , items    .toUtf8()) ;
          SC . Bind    ( "reference", reference.toUtf8()) ;
          SC . Bind    ( "maps"     , maps     .toUtf8()) ;
          SC . Exec    (                                ) ;
          emit Changed ( uuid,t,u,0                     ) ;
        } else if (name.length()>0)                       {
          Q = SC.sql.Update                               (
                   PlanTable(SqlConstraints)              ,
                   SC.sql.Where(1,"uuid")                 ,
                   1                                      ,
                   "name"                               ) ;
          SC . Prepare ( Q                      )         ;
          SC . Bind    ( "uuid" , u             )         ;
          SC . Bind    ( "name" , name.toUtf8() )         ;
          SC . Exec    (                        )         ;
          emit Changed ( uuid,type,u,1          )       ;
        }                                                 ;
        removeOldItem()                                   ;
      break                                               ;
      case 2                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlConstraints)                ,
                 SC.sql.Where(1,"uuid")                   ,
                 1 , "items"                       )      ;
        SC . Prepare ( Q                           )      ;
        SC . Bind    ( "uuid"   , u                )      ;
        SC . Bind    ( "items"  , name .toUtf8()   )      ;
        SC . Exec    (                             )      ;
        emit Changed ( uuid,type,u,1               )      ;
      break                                               ;
      case 3                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlConstraints)                ,
                 SC.sql.Where(1,"uuid")                   ,
                 1                                        ,
                 "reference"                        )     ;
        SC . Prepare ( Q                            )     ;
        SC . Bind    ( "uuid"      , u              )     ;
        SC . Bind    ( "reference" , name.toUtf8()  )     ;
        SC . Exec    (                              )     ;
        emit Changed ( uuid,type,u,1                )     ;
      break                                               ;
      case 4                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlConstraints)                ,
                 SC.sql.Where(1,"uuid")                   ,
                 1                                        ,
                 "maps"                                 ) ;
        SC . Prepare ( Q                       )          ;
        SC . Bind    ( "uuid" , u              )          ;
        SC . Bind    ( "maps" , name.toUtf8()  )          ;
        SC . Exec    (                         )          ;
        emit Changed ( uuid,type,u,1           )          ;
      break                                               ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}

void N::SqlRelations::addTable(SqlConnection & SC,SqlParser & Parser)
{
  for (int i=0;i<Parser.limits();i++)      {
    QString boundary = Parser.boundary(i)  ;
    if (!Items.contains(boundary))         {
      SUID u = GetSqlItem(SC,Parser,i)     ;
      if (u>0)                             {
        GroupItems GI ( plan )             ;
        UUIDs       Juids                  ;
        GetItem ( SC , u )                 ;
        Juids << u                         ;
        GI.Join                            (
          SC                               ,
          uuid                             ,
          type                             ,
          Types::SqlConstraint             ,
          Groups::Subordination            ,
          0,Juids                        ) ;
      } else                               {
        addParserItem(SC,Parser,i)         ;
      }                                    ;
    }                                      ;
  }                                        ;
}

bool N::SqlRelations::Menu(QPoint pos)
{
  QMdiSubWindow * mdi    = Casting(QMdiSubWindow,parent())     ;
  QDockWidget   * dock   = Casting(QDockWidget  ,parent())     ;
  MenuManager     mm (this)                                    ;
  QAction       * a                                            ;
  if (uuid>0 && topLevelItemCount()>0)                         {
    mm.add(101,tr("Detach"    ))                               ;
    mm.add(102,tr("Detach all"))                               ;
  }                                                            ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont(plan)                                           ;
  a = mm.exec()                                                ;
  if (IsNull(a)) return true                                   ;
  switch (mm[a])                                               {
    case 101                                                   :
      Delete    ( )                                            ;
    break                                                      ;
    case 102                                                   :
      DeleteAll ( )                                            ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi ( this , 0 )                              ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::BottomDockWidgetArea                               ,
        Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea  | Qt::BottomDockWidgetArea    ) ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
