#include <datawidgets.h>

N::SqlItems:: SqlItems   (QWidget * parent,Plan * p)
            : TreeDock   (          parent,       p)
            , Object     (0,Types::SqlPlan         )
            , dropAction (false                    )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlItems::~SqlItems (void)
{
}

QMimeData * N::SqlItems::dragMime (void)
{
  QMimeData * mime = standardMime("sqlcolumn")   ;
  nKickOut ( IsNull(mime) , NULL )               ;
  QImage image = QImage(":/images/sqlitems.png") ;
  mime -> setImageData(image)                    ;
  return mime                                    ;
}

bool N::SqlItems::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0) ;
  return ( Uuids.count() > 0 )   ;
}

bool N::SqlItems::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::SqlItems::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::SqlItems::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SqlItems::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::SqlItems::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::SqlItems::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::SqlItems::Configure(void)
{
  Scope = ""                                                  ;
  NewTreeWidgetItem            ( head                       ) ;
  setWindowTitle               ( tr("Database column items")) ;
  setDragDropMode              ( NoDragDrop                 ) ;
  setRootIsDecorated           ( false                      ) ;
  setAlternatingRowColors      ( true                       ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded      ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded      ) ;
  setAccessibleName            ( "nSqlItems"                ) ;
  setColumnCount               ( 9                          ) ;
  head -> setText              ( 0, tr("Description")       ) ;
  head -> setText              ( 1, tr("Name"       )       ) ;
  head -> setText              ( 2, tr("Type"       )       ) ;
  head -> setText              ( 3, tr("Sign"       )       ) ;
  head -> setText              ( 4, tr("Paraments"  )       ) ;
  head -> setText              ( 5, tr("Attributes" )       ) ;
  head -> setText              ( 6, tr("Extra"      )       ) ;
  head -> setText              ( 7, tr("Comment"    )       ) ;
  head -> setText              ( 8, ""                      ) ;
  assignHeaderItems            ( head                       ) ;
  setAlignment                 ( 4 , Qt::AlignRight           |
                                     Qt::AlignVCenter       ) ;
  MountClicked                 ( 2                          ) ;
  plan -> setFont              ( this                       ) ;
}

bool N::SqlItems::FocusIn(void)
{
  LinkAction ( Refresh , startup() ) ;
  LinkAction ( Insert  , New    () ) ;
  LinkAction ( Font    , setFont() ) ;
  if (uuid>0)                        {
    LinkAction ( Delete , Delete() ) ;
  }                                  ;
  return true                        ;
}

bool N::SqlItems::FocusOut(void)
{
  return true ;
}

QString N::SqlItems::DataType(SqlConnection & Connection,int Type)
{
  QString name = ""              ;
  QString Q                      ;
  Q = Connection.sql.SelectFrom  (
        "name"                   ,
        PlanTable(SqlDataTypes)  ,
        QString("where type = %1")
        .arg(Type)             ) ;
  if (Connection.Fetch(Q))       {
    name = Connection.String(0)  ;
  }                              ;
  return name                    ;
}

int N::SqlItems::DataType(SqlConnection & Connection,QString name)
{
  int T = 0                        ;
  QString Q                        ;
  Q = Connection.sql.SelectFrom    (
        "type"                     ,
        PlanTable(SqlDataTypes)    ,
        QString("where name = '%1'")
        .arg(name)               ) ;
  if (Connection.Fetch(Q))         {
    T = Connection.Int(0)          ;
  }                                ;
  return T                         ;
}

UUIDs N::SqlItems::GetUuids(SqlConnection & SC)
{
  UUIDs Uuids                                    ;
  if (uuid<=0)                                   {
    Uuids = SC.Uuids(PlanTable(SqlItems),"uuid"  ,
                     "order by id asc"         ) ;
  } else                                         {
    GroupItems GI ( plan )                       ;
    Uuids = GI.Subordination                     (
              SC                                 ,
              uuid                               ,
              type                               ,
              Types::SqlItem                     ,
              Groups::Subordination              ,
              "order by position asc"          ) ;
  }                                              ;
  return Uuids                                   ;
}

QTreeWidgetItem * N::SqlItems::GetItem(SqlConnection & SC,SUID u)
{
  QTreeWidgetItem * result = NULL              ;
  QIcon   icon = QIcon(":/images/yes.png")     ;
  QString Q                                    ;
  Q = SC.sql.SelectFrom                        (
    "name,type,sign,paraments,"
    "attributes,extra,comment"                 ,
    PlanTable(SqlItems)                        ,
    SC.WhereUuid(u)                          ) ;
  if (SC.Fetch(Q))                             {
    QString column                             ;
    QString name        = SC.String(0)         ;
    int     Type        = SC.Int   (1)         ;
    int     Sign        = SC.Int   (2)         ;
    QString paraments   = SC.String(3)         ;
    QString attributes  = SC.String(4)         ;
    QString extra       = SC.String(5)         ;
    QString comment     = SC.String(6)         ;
    QString description = SC.getName           (
                        PlanTable(Names)       ,
                        "uuid"                 ,
                        plan->LanguageId       ,
                        u                    ) ;
    QString TName                              ;
    QString SName                              ;
    SName = (Sign==0) ? "unsigned" : ""        ;
    TName = DataType  ( SC , Type            ) ;
    NewTreeWidgetItem ( item                 ) ;
    item -> setData   ( 0,Qt::UserRole,u     ) ;
    item -> setData   ( 2,Qt::UserRole,Type  ) ;
    item -> setData   ( 3,Qt::UserRole,Sign  ) ;
    item -> setIcon   ( 0,icon               ) ;
    item -> setText   ( 0,description        ) ;
    item -> setText   ( 1,name               ) ;
    item -> setText   ( 2,TName              ) ;
    item -> setText   ( 3,SName              ) ;
    item -> setText   ( 4,paraments          ) ;
    item -> setText   ( 5,attributes         ) ;
    item -> setText   ( 6,extra              ) ;
    item -> setText   ( 7,comment            ) ;
    if (paraments.length()>0)                  {
      column = QString("`%1` %2(%3)"           )
               .arg(name  . toLower ()         )
               .arg(TName . toLower ()         )
               .arg(paraments                ) ;
    } else                                     {
      column = QString("`%1` %2"               )
               .arg(name  . toLower ()         )
               .arg(TName . toLower ()       ) ;
    }                                          ;
    if (SName.length()>0)                      {
      column += " "                            ;
      column += SName                          ;
    }                                          ;
    if (attributes.length()>0)                 {
      column += " "                            ;
      column += attributes                     ;
    }                                          ;
    if (extra.length()>0)                      {
      column += " "                            ;
      column += extra                          ;
    }                                          ;
    if (!Items.contains(column))               {
      Items << column                          ;
      result = item                            ;
      setAlignments     ( item               ) ;
      addTopLevelItem   ( item               ) ;
    }                                          ;
  }                                            ;
  return result                                ;
}

SUID N::SqlItems::GetSqlItem(SqlConnection & SC,SqlParser & Parser,int i)
{
  SUID    u = 0                                   ;
  QString Q                                       ;
  QString column     = Parser.Columns  [ i      ] ;
  QString TName      = Parser.TypeMaps [ column ] ;
  QString SName      = ""                         ;
  QString paraments  = ""                         ;
  QString attributes = ""                         ;
  QString Extras     = ""                         ;
  int     sign       = 1                          ;
  int     T          = DataType(SC,TName)         ;
  if (Parser.SignMaps.contains(column))           {
    SName = Parser.SignMaps[column]               ;
    sign  = 0                                     ;
  }                                               ;
  if (Parser.Paraments.contains(column))          {
    paraments = Parser.Paraments[column]          ;
  }                                               ;
  if (Parser.Attributes.contains(column))         {
    attributes = Parser.Attributes[column]        ;
  }                                               ;
  if (Parser.Extras.contains(column))             {
    Extras = Parser.Extras[column]                ;
  }                                               ;
  Q = SC.sql.SelectFrom                           (
        "uuid"                                    ,
        PlanTable(SqlItems)                       ,
        SC.sql.Where                              (
          6                                       ,
          "name"                                  ,
          "type"                                  ,
          "sign"                                  ,
          "paraments"                             ,
          "attributes"                            ,
          "extra"                             ) ) ;
  SC . Prepare (Q                               ) ;
  SC . Bind    ("name",column.toUtf8()          ) ;
  SC . Bind    ("type",T                        ) ;
  SC . Bind    ("sign",sign                     ) ;
  SC . Bind    ("paraments",paraments.toUtf8()  ) ;
  SC . Bind    ("attributes",attributes.toUtf8()) ;
  SC . Bind    ("extra",Extras.toUtf8()         ) ;
  IfSqlExec(SC)                                   {
    u = SC.Uuid(0)                                ;
  }                                               ;
  return u                                        ;
}

void N::SqlItems::addParserItem(SqlConnection & SC,SqlParser & Parser,int i)
{
  QIcon   icon = QIcon(":/images/help.png")       ;
  QString column     = Parser.Columns  [ i      ] ;
  QString TName      = Parser.TypeMaps [ column ] ;
  QString SName      = ""                         ;
  QString paraments  = ""                         ;
  QString attributes = ""                         ;
  QString Extras     = ""                         ;
  int     sign       = 1                          ;
  int     T          = DataType(SC,TName)         ;
  if (Parser.SignMaps.contains(column))           {
    SName = Parser.SignMaps[column]               ;
    sign  = 0                                     ;
  }                                               ;
  if (Parser.Paraments.contains(column))          {
    paraments = Parser.Paraments[column]          ;
  }                                               ;
  if (Parser.Attributes.contains(column))         {
    attributes = Parser.Attributes[column]        ;
  }                                               ;
  if (Parser.Extras.contains(column))             {
    Extras = Parser.Extras[column]                ;
  }                                               ;
  NewTreeWidgetItem ( item                 )      ;
  item -> setData   ( 0,Qt::UserRole,0     )      ;
  item -> setData   ( 2,Qt::UserRole,T     )      ;
  item -> setData   ( 3,Qt::UserRole,sign  )      ;
  item -> setIcon   ( 0,icon               )      ;
  item -> setText   ( 0,""                 )      ;
  item -> setText   ( 1,column             )      ;
  item -> setText   ( 2,TName              )      ;
  item -> setText   ( 3,SName              )      ;
  item -> setText   ( 4,paraments          )      ;
  item -> setText   ( 5,attributes         )      ;
  item -> setText   ( 6,Extras             )      ;
  item -> setText   ( 7,""                 )      ;
  setAlignments     ( item                 )      ;
  addTopLevelItem   ( item                 )      ;
  Items << Parser.syntax(i)                       ;
}

bool N::SqlItems::startup(void)
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

void N::SqlItems::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID        u = nTreeUuid(item,0)                          ;
  QLineEdit * line                                           ;
  QComboBox * types                                          ;
  removeOldItem()                                            ;
  switch (column)                                            {
    case 2                                                 : {
      nDropOut ( u <= 0 )                                    ;
      types = new QComboBox(this)                            ;
      EnterSQL ( SC , plan->sql )                            ;
        int     ct = item->data(column,Qt::UserRole).toInt() ;
        CUIDs   IDs                                          ;
        QString Q                                            ;
        Q = SC.sql.SelectFrom                                (
              "type"                                         ,
              PlanTable(SqlDataTypes)                        ,
              "where type > 0 order by id asc"             ) ;
        SqlLoopNow ( SC , Q )                                ;
          IDs << SC . Int ( 0 )                              ;
        SqlLoopErr ( SC , Q )                                ;
        SqlLoopEnd ( SC , Q )                                ;
        int index = -1                                       ;
        for (int i=0;i<IDs.count();i++)                      {
          int     id = IDs [ i ]                             ;
          QString TN = DataType ( SC , id )                  ;
          if (id==ct) index = i                              ;
          types -> addItem ( TN , id )                       ;
        }                                                    ;
        if (index>=0) types->setCurrentIndex(index)          ;
      LeaveSQL ( SC , plan->sql )                            ;
      setItemWidget ( item , column , types )                ;
      ItemEditing = item                                     ;
      ItemColumn  = column                                   ;
      ItemWidget  = types                                    ;
      connect(types , SIGNAL (activated  (int))              ,
              this  , SLOT   (typeChanged(int))            ) ;
      types -> showPopup()                                   ;
    }                                                        ;
    break                                                    ;
    case 3                                                 : {
      nDropOut ( u <= 0 )                                    ;
      EnterSQL ( SC , plan->sql )                            ;
        QString Q                                            ;
        int Sign = item->data(column,Qt::UserRole).toInt()   ;
        if (Sign==0) Sign = 1 ; else Sign = 0                ;
        Q = SC.sql.Update                                    (
              PlanTable(SqlItems)                            ,
              "where uuid = :UUID"                           ,
              1                                              ,
              "sign"                                       ) ;
        SC . Prepare ( Q             )                       ;
        SC . Bind    ( "uuid" , u    )                       ;
        SC . Bind    ( "sign" , Sign )                       ;
        SC . Exec    (               )                       ;
        item -> setData(column,Qt::UserRole,Sign           ) ;
        item -> setText(column,(Sign==1) ? "" : "unsigned" ) ;
      LeaveSQL ( SC , plan->sql )                            ;
    }                                                        ;
    break                                                    ;
    case 1                                                   :
      line = setLineEdit                                     (
             item                                            ,
             column                                          ,
             SIGNAL(editingFinished())                       ,
             SLOT  (nameFinished   ()) )                     ;
      line->setFocus(Qt::TabFocusReason)                     ;
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

void N::SqlItems::New(void)
{
  NewTreeWidgetItem ( IT )      ;
  IT->setData(0,Qt::UserRole,0) ;
  IT->setData(2,Qt::UserRole,0) ;
  IT->setData(3,Qt::UserRole,1) ;
  setAlignments   ( IT     )    ;
  addTopLevelItem ( IT     )    ;
  scrollToItem    ( IT     )    ;
  doubleClicked   ( IT , 1 )    ;
}

void N::SqlItems::Delete(void)
{
  nDropOut ( uuid <= 0    )              ;
  QTreeWidgetItem * item = currentItem() ;
  if (IsNull(item))                      {
    Alert ( Error )                      ;
    return                               ;
  }                                      ;
  ////////////////////////////////////////
  SUID  u = nTreeUuid(item,0)            ;
  UUIDs Uuids                            ;
  Uuids << u                             ;
  GroupItems  GI ( plan )                ;
  EnterSQL ( SC , plan->sql )            ;
    GI.Detach                            (
      SC                                 ,
      uuid                               ,
      type                               ,
      Types::SqlItem                     ,
      Groups::Subordination              ,
      Uuids                             );
  LeaveSQL ( SC , plan->sql )            ;
  ////////////////////////////////////////
  takeItem ( item )                      ;
  Alert    ( Done )                      ;
}

void N::SqlItems::DeleteAll(void)
{
  UUIDs Uuids                            ;
  SUID  u                                ;
  QTreeWidgetItem * item                 ;
  nFullLoop(i,topLevelItemCount())       {
    item = topLevelItem(i)               ;
    u    = nTreeUuid(item,0)             ;
    Uuids << u                           ;
  }                                      ;
  GroupItems  GI ( plan )                ;
  EnterSQL ( SC , plan->sql )            ;
    GI.Detach                            (
      SC                                 ,
      uuid                               ,
      type                               ,
      Types::SqlItem                     ,
      Groups::Subordination              ,
      Uuids                            ) ;
  LeaveSQL ( SC , plan->sql )            ;
  ////////////////////////////////////////
  clear (      )                         ;
  Alert ( Done )                         ;
}

void N::SqlItems::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  SUID u = nTreeUuid(ItemEditing,0)         ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  if (u==0) takeItem(ItemEditing)           ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::SqlItems::typeChanged(int index)
{
  if (IsNull(ItemEditing)) return                         ;
  QComboBox * types = Casting(QComboBox ,ItemWidget)      ;
  if (IsNull(types)) return                               ;
  int  Type = types->itemData(index,Qt::UserRole).toInt() ;
  SUID u    = nTreeUuid(ItemEditing,0)                    ;
  ItemEditing->setText(ItemColumn,types->itemText(index)) ;
  ItemEditing->setData(ItemColumn,Qt::UserRole,Type)      ;
  EnterSQL       ( SC , plan->sql )                       ;
    QString Q                                             ;
    Q = SC.sql.Update(PlanTable(SqlItems)                 ,
             "where uuid = :UUID"                         ,
             1,"type"                                   ) ;
    SC . Prepare ( Q              )                       ;
    SC . Bind    ( "uuid" , u     )                       ;
    SC . Bind    ( "type" , Type  )                       ;
    SC . Exec    (                )                       ;
  LeaveSQL       ( SC , plan->sql )                       ;
  removeOldItem  (                )                       ;
  emit Changed   ( uuid,type,u,1  )                       ;
  Alert          ( Done           )                       ;
}

void N::SqlItems::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * line    = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line       )) return                         ;
  ItemEditing -> setText    ( ItemColumn , line->text() ) ;
  /////////////////////////////////////////////////////////
  QIcon   icon   = QIcon(":/images/yes.png")              ;
  SUID    u      = nTreeUuid(ItemEditing,0)               ;
  QString name   = line->text()                           ;
  int     column = ItemColumn                             ;
  bool    extras = false                                  ;
  CodeManager CM ( plan )                                 ;
  GroupItems  GI ( plan )                                 ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    switch (column)                                       {
      case 0                                              :
        removeOldItem()                                   ;
        CM . assureName                                   (
           SC                                             ,
           u                                              ,
           plan->LanguageId                               ,
           name                                         ) ;
        emit Changed ( uuid,type,u,1 )                    ;
      break                                               ;
      case 1                                              :
        if (u<=0 && name.length()>0)                      {
          UUIDs Juids                                     ;
          u = SC.Unique                                   (
                   PlanTable(MajorUuid)                   ,
                   "uuid"                                 ,
                   9009                                 ) ;
          SC.assureUuid                                   (
                   PlanTable(MajorUuid)                   ,
                   u                                      ,
                   Types::SqlItem                       ) ;
          Juids << u                                      ;
          GI.Join                                         (
            SC                                            ,
            uuid                                          ,
            type                                          ,
            Types::SqlItem                                ,
            Groups::Subordination                         ,
            0,Juids                                     ) ;
          ItemEditing->setData(0,Qt::UserRole,u)          ;
          ItemEditing->setIcon(0,icon)                    ;
          Q = SC.sql.InsertInto                           (
                   PlanTable(SqlItems)                    ,
                   2                                      ,
                   "uuid"                                 ,
                   "name"                               ) ;
          extras = true                                   ;
        } else if (name.length()>0)                       {
          Q = SC.sql.Update                               (
                   PlanTable(SqlItems)                    ,
                   "where uuid = :UUID"                   ,
                   1                                      ,
                   "name"                               ) ;
        }                                                 ;
        if (name.length()>0)                              {
          SC . Prepare ( Q                      )         ;
          SC . Bind    ( "uuid" , u             )         ;
          SC . Bind    ( "name" , name.toUtf8() )         ;
          SC . Exec    (                        )         ;
        }                                                 ;
        if (extras)                                       {
          QString n                                       ;
          int      itemType = nTreeInt(ItemEditing,2)     ;
          int      itemSign = nTreeInt(ItemEditing,3)     ;
          Q = SC.sql.Update(PlanTable(SqlItems)           ,
                   "where uuid = :UUID"                   ,
                   2,"type","sign"                      ) ;
          SC . Prepare ( Q                 )              ;
          SC . Bind    ( "uuid" , u        )              ;
          SC . Bind    ( "type" , itemType )              ;
          SC . Bind    ( "sign" , itemSign )              ;
          SC . Exec    (                   )              ;
          n = ItemEditing->text(0)                        ;
          if (n.length()>0)                               {
            CM . assureName                               (
              SC                                          ,
              u                                           ,
              plan->LanguageId                            ,
              n                                         ) ;
          }                                               ;
          n = ItemEditing->text(4)                        ;
          Q = SC.sql.Update                               (
                   PlanTable(SqlItems)                    ,
                   "where uuid = :UUID"                   ,
                   1 , "paraments"                )       ;
          SC . Prepare ( Q                        )       ;
          SC . Bind    ( "uuid"      , u          )       ;
          SC . Bind    ( "paraments" , n.toUtf8() )       ;
          SC . Exec    (                          )       ;
          n = ItemEditing->text(5)                        ;
          Q = SC.sql.Update                               (
                   PlanTable(SqlItems)                    ,
                   "where uuid = :UUID"                   ,
                   1                                      ,
                   "attributes"                    )      ;
          SC . Prepare ( Q                         )      ;
          SC . Bind    ( "uuid"       , u          )      ;
          SC . Bind    ( "attributes" , n.toUtf8() )      ;
          SC . Exec    (                           )      ;
          n = ItemEditing->text(6)                        ;
          Q = SC.sql.Update                               (
                   PlanTable(SqlItems)                    ,
                   "where uuid = :UUID"                   ,
                   1                                      ,
                   "extra"                    )           ;
          SC . Prepare ( Q                    )           ;
          SC . Bind    ( "uuid"  , u          )           ;
          SC . Bind    ( "extra" , n.toUtf8() )           ;
          SC . Exec    (                      )           ;
          n = ItemEditing->text(7)                        ;
          if (n.length()>0)                               {
            Q = SC.sql.Update                             (
                     PlanTable(SqlItems)                  ,
                     "where uuid = :UUID"                 ,
                     1                                    ,
                     "comment"                    )       ;
            SC . Prepare ( Q                      )       ;
            SC . Bind    ( "uuid"    , u          )       ;
            SC . Bind    ( "comment" , n.toUtf8() )       ;
            SC . Exec    (                        )       ;
          }                                               ;
          emit Changed   ( uuid,type,u,0          )       ;
        } else                                            {
          emit Changed   ( uuid,type,u,1          )       ;
        }                                                 ;
        removeOldItem()                                   ;
      break                                               ;
      case 4                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlItems)                      ,
                 "where uuid = :UUID"                     ,
                 1 , "paraments"                   )      ;
        SC . Prepare ( Q                           )      ;
        SC . Bind    ( "uuid"      , u             )      ;
        SC . Bind    ( "paraments" , name.toUtf8() )      ;
        SC . Exec    (                             )      ;
        emit Changed ( uuid,type,u,1               )      ;
      break                                               ;
      case 5                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlItems)                      ,
                 "where uuid = :UUID"                     ,
                 1                                        ,
                 "attributes"                           ) ;
        SC . Prepare ( Q                            )     ;
        SC . Bind    ( "uuid"       , u             )     ;
        SC . Bind    ( "attributes" , name.toUtf8() )     ;
        SC . Exec    (                              )     ;
        emit Changed ( uuid,type,u,1                )     ;
      break                                               ;
      case 6                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlItems)                      ,
                 "where uuid = :UUID"                     ,
                 1                                        ,
                 "extra"                                ) ;
        SC . Prepare ( Q                       )          ;
        SC . Bind    ( "uuid"  , u             )          ;
        SC . Bind    ( "extra" , name.toUtf8() )          ;
        SC . Exec    (                         )          ;
        emit Changed ( uuid,type,u,1           )          ;
      break                                               ;
      case 7                                              :
        removeOldItem()                                   ;
        Q = SC.sql.Update                                 (
                 PlanTable(SqlItems)                      ,
                 "where uuid = :UUID"                     ,
                 1                                        ,
                 "comment"                              ) ;
        SC . Prepare ( Q                         )        ;
        SC . Bind    ( "uuid"    , u             )        ;
        SC . Bind    ( "comment" , name.toUtf8() )        ;
        SC . Exec    (                           )        ;
        emit Changed ( uuid,type,u,1             )        ;
      break                                               ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}

void N::SqlItems::addTable(SqlConnection & SC,SqlParser & Parser)
{
  for (int i=0;i<Parser.count();i++)   {
    QString syntax = Parser.syntax(i)  ;
    if (!Items.contains(syntax))       {
      SUID u = GetSqlItem(SC,Parser,i) ;
      if (u>0)                         {
        GroupItems GI ( plan )         ;
        UUIDs       Juids              ;
        GetItem ( SC , u )             ;
        Juids << u                     ;
        GI.Join                        (
          SC                           ,
          uuid                         ,
          type                         ,
          Types::SqlItem               ,
          Groups::Subordination        ,
          0,Juids                    ) ;
      } else                           {
        addParserItem(SC,Parser,i)     ;
      }                                ;
    }                                  ;
  }                                    ;
}

bool N::SqlItems::Menu(QPoint pos)
{
  QMdiSubWindow * mdi    = Casting(QMdiSubWindow,parent())     ;
  QDockWidget   * dock   = Casting(QDockWidget  ,parent())     ;
  MenuManager     mm (this)                                    ;
  QAction       * a                                            ;
  if (uuid>0 && topLevelItemCount()>0)                         {
    mm.add(101,tr("Detach"))                                   ;
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

bool N::SqlItems::dropSqlColumn(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                  ;
  Q_UNUSED ( pos    )                  ;
  nKickOut       ( uuid <= 0 , false ) ;
  GroupItems GI  ( plan              ) ;
  EnterSQL       ( SC , plan->sql    ) ;
    UUIDs Juids = Uuids                ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      type                             ,
      Types::SqlItem                   ,
      Groups::Subordination            ,
      0,Juids                        ) ;
  LeaveSQL       ( SC , plan->sql    ) ;
  startup        (                   ) ;
  return true                          ;
}
