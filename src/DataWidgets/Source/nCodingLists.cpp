#include <datawidgets.h>

N::CodingLists:: CodingLists (QWidget * parent,Plan * p)
               : TreeWidget  (          parent,       p)
               , CodeManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CodingLists::~CodingLists (void)
{
}

void N::CodingLists::Configure(void)
{
  PassDragDrop = false                                           ;
  NewTreeWidgetItem            ( head                   )        ;
  head -> setText              ( 0 , tr("Language"      )    )   ;
  head -> setText              ( 1 , tr("Name"          )    )   ;
  head -> setText              ( 2 , tr("Identification")    )   ;
  head -> setText              ( 3 , tr("Rank"          )    )   ;
  head -> setText              ( 4 , tr("Comment"       )    )   ;
  setWindowTitle               ( tr("Programming languages") )   ;
  setDragDropMode              ( DropOnly                    )   ;
  setRootIsDecorated           ( false                       )   ;
  setAlternatingRowColors      ( true                        )   ;
  setSelectionMode             ( SingleSelection             )   ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded       )   ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded       )   ;
  setColumnCount               ( 5                           )   ;
  setHeaderItem                ( head                        )   ;
  plan -> setFont              ( this                        )   ;
  ////////////////////////////////////////////////////////////////
  setDropFlag                  ( DropUrls , true             )   ;
  setDropFlag                  ( DropURIs , true             )   ;
  ////////////////////////////////////////////////////////////////
  disconnect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
             NULL,NULL                                         ) ;
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::CodingLists::FocusIn(void)
{
  LinkAction ( Insert , New   () ) ;
  LinkAction ( Paste  , Paste () ) ;
  LinkAction ( Export , Export() ) ;
  LinkAction ( Copy   , Copy  () ) ;
  return true                      ;
}

bool N::CodingLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

QString N::CodingLists::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType ( mime      ,
           "uri/uuid;uri/uuids;"
           "bookmark/uuid;bookmark/uuids") ;
}

UUIDs N::CodingLists::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="uri/uuid"      )        {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="uri/uuids"     )        {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="bookmark/uuid" )        {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="bookmark/uuids")        {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::CodingLists::dropUrls(QWidget * source,QPointF psf,const QList<QUrl> & urls)
{
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  SUID uuid = nTreeUuid(item,0)        ;
  NetworkManager NM(plan)              ;
  GroupItems     GI(plan)              ;
  EnterSQL(SC,plan->sql)               ;
    UUIDs Uuids                        ;
    NM.LoadDomainIndex(SC)             ;
    for (int i=0;i<urls.count();i++)   {
      QUrl U = urls[i]                 ;
      SUID u = NM.assurePage(SC,U)     ;
      if (u>0) Uuids << u              ;
    }                                  ;
    GI.Join(SC,uuid                    ,
      Types::Coding                    ,
      Types::URL                       ,
      Groups::Subordination            ,
      0,Uuids                        ) ;
  LeaveSQL(SC,plan->sql)               ;
  Alert ( Done )                       ;
  return true                          ;
}

bool N::CodingLists::dropURIs(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  SUID uuid = nTreeUuid(item,0)        ;
  GroupItems GI(plan)                  ;
  EnterSQL(SC,plan->sql)               ;
    UUIDs U = Uuids                    ;
    GI.Join(SC,uuid                    ,
      Types::Coding                    ,
      Types::URL                       ,
      Groups::Subordination            ,
      0,U                            ) ;
  LeaveSQL(SC,plan->sql)               ;
  Alert ( Done )                       ;
  return true                          ;
}

bool N::CodingLists::List(void)
{
  clear ( )                                        ;
  EnterSQL ( SC , plan->sql )                      ;
    QString Q                                      ;
    Q = SC.sql.SelectFrom                          (
          "uuid,identification,name,comment,rank"  ,
          PlanTable(PDL)                           ,
          "order by id asc"                      ) ;
    SqlLoopNow ( SC , Q )                          ;
      NewTreeWidgetItem ( IT )                     ;
      SUID    uuid    = SC . Uuid   ( 0 )          ;
      int     ident   = SC . Int    ( 1 )          ;
      QString Name    = SC . String ( 2 )          ;
      QString comment = SC . String ( 3 )          ;
      int     rank    = SC . Int    ( 4 )          ;
      IT -> setData ( 0 , Qt::UserRole , uuid    ) ;
      IT -> setData ( 2 , Qt::UserRole , ident   ) ;
      IT -> setData ( 3 , Qt::UserRole , rank    ) ;
      IT -> setText ( 0 , Name                   ) ;
      IT -> setText ( 2 , QString::number(ident) ) ;
      IT -> setText ( 3 , QString::number(rank ) ) ;
      IT -> setText ( 4 , comment                ) ;
      addTopLevelItem   ( IT )                     ;
    SqlLoopErr ( SC , Q )                          ;
    SqlLoopEnd ( SC , Q )                          ;
    QString name                                   ;
    for (int i=0;i<topLevelItemCount();i++)        {
      QTreeWidgetItem * it = topLevelItem(i)       ;
      SUID uuid = nTreeUuid ( it , 0 )             ;
      if (uuid>0)                                  {
        name = Name(SC,uuid,vLanguageId)           ;
        it -> setText ( 1 , name )                 ;
      }                                            ;
    }                                              ;
  LeaveSQL ( SC , plan->sql )                      ;
  Alert ( Done )                                   ;
  return true                                      ;
}

void N::CodingLists::New(void)
{
  removeOldItem     (    )           ;
  NewTreeWidgetItem ( IT )           ;
  addTopLevelItem   ( IT )           ;
  scrollToItem      ( IT )           ;
  QLineEdit * name                   ;
  name = setLineEdit                 (
    IT,0                             ,
    SIGNAL(editingFinished())        ,
    SLOT  (nameFinished   ())      ) ;
  IT->setData(0,Qt::UserRole  , 0  ) ;
  IT->setText(3,"10000"            ) ;
  name->setFocus(Qt::TabFocusReason) ;
}

void N::CodingLists::Paste(void)
{
  QTreeWidgetItem * item = currentItem()    ;
  if (IsNull(item)) return                  ;
  if (currentColumn()<0) return             ;
  QString text = qApp->clipboard()->text()  ;
  if (text.length()<=0) return              ;
  item->setText(currentColumn(),text)       ;
  Ending ( item )                           ;
}

void N::CodingLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
    this , tr("Export programming languages" )    ,
    plan->User.absolutePath() , "*.txt"         ) ;
  if (filename.length()<=0) return                ;
  QFile F ( filename )                            ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * it = topLevelItem(i)        ;
    QString R                                     ;
    R = it->text(0) + "\t" + it->text(1)          +
                      "\t" + it->text(4)          +
                      "\t" + it->text(2)          +
                      "\t" + it->text(3) + "\r\n" ;
    F . write ( R . toUtf8 ( ) )                  ;
  }                                               ;
  F . close ( )                                   ;
}

void N::CodingLists::Copy(void)
{
  QTreeWidgetItem * item = currentItem()     ;
  if (IsNull(item)) return                   ;
  if (currentColumn()<0) return              ;
  QString text = item->text(currentColumn()) ;
  qApp->clipboard()->setText(text)           ;
}

bool N::CodingLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                             ;
  QPoint            global = mapToGlobal(pos)           ;
  QTreeWidgetItem * item   = itemAt(pos)                ;
  QAction         * a                                   ;
  SUID              uuid   = 0                          ;
  if (NotNull(item))                                    {
    uuid = nTreeUuid ( item , 0 )                       ;
    mm.add                                              (
      101                                               ,
      QIcon(":/images/bookmarks.png")                   ,
      tr("Bookmark")                                  ) ;
  }                                                     ;
  mm.add(102,tr("Refresh"))                             ;
  mm.addSeparator()                                     ;
  a = mm.add(201,tr("Enable sorting"))                  ;
  a -> setCheckable ( true )                            ;
  a -> setChecked(isSortingEnabled())                   ;
  mm.add(202,tr("Ascending sort"    ))                  ;
  mm.add(203,tr("Descending sort"   ))                  ;
  mm . setFont( plan )                                  ;
  a = mm.exec(global)                                   ;
  if (IsNull(a)) return true                            ;
  switch (mm[a])                                        {
    case 101                                            :
      emit Bookmark ( uuid , Types::Coding    ,
             item->text(0)                            ) ;
    break                                               ;
    case 102                                            :
      List ( )                                          ;
    break                                               ;
    case 201                                            :
      setSortingEnabled(a->isChecked())                 ;
    break                                               ;
    case 202                                            :
      sortItems(currentColumn()<0 ? 0 : currentColumn() ,
                Qt::AscendingOrder                    ) ;
    break                                               ;
    case 203                                            :
      sortItems(currentColumn()<0 ? 0 : currentColumn() ,
                Qt::DescendingOrder                   ) ;
    break                                               ;
    default                                             :
    break                                               ;
  }                                                     ;
  return true                                           ;
}

void N::CodingLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * name   = NULL               ;
  name = setLineEdit                      (
    item,column                           ,
    SIGNAL(editingFinished())             ,
    SLOT  (nameFinished   ())           ) ;
  if (IsNull(name)) return                ;
  name -> setFocus ( Qt::TabFocusReason ) ;
}

void N::CodingLists::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::CodingLists::nameFinished(void)
{
  QLineEdit * name = qobject_cast<QLineEdit *>(ItemWidget) ;
  if (NotNull(name))                                       {
    ItemEditing -> setText ( ItemColumn , name->text() )   ;
    if (Ending (ItemEditing)) removeOldItem()              ;
  }                                                        ;
}

bool N::CodingLists::Ending(QTreeWidgetItem * item)
{
  SUID    uuid     = nTreeUuid ( item , 0 )       ;
  QString language = item -> text ( 0 )           ;
  QString name     = item -> text ( 1 )           ;
  QString ident    = item -> text ( 2 )           ;
  QString rank     = item -> text ( 3 )           ;
  QString comment  = item -> text ( 4 )           ;
  bool    convert  = true                         ;
  int     Ident    = -1                           ;
  int     Rank     = -1                           ;
  if (language.length()<=0)                       {
    int index = indexOfTopLevelItem(item)         ;
    removeOldItem    (                          ) ;
    takeTopLevelItem ( index                    ) ;
    Alert ( Error )                               ;
    return false                                  ;
  }                                               ;
  Ident = ident . toInt(&convert)                 ;
  Rank  = rank  . toInt(&convert)                 ;
  if (!convert)                                   {
    removeOldItem    (                          ) ;
    Alert ( Error )                               ;
    return false                                  ;
  }                                               ;
  EnterSQL ( SC , plan->sql )                     ;
    QString Q                                     ;
    if (uuid<=0)                                  {
      uuid = SC.Unique                            (
        PlanTable(MajorUuid)                      ,
        "uuid"                                  ) ;
      SC.assureUuid                               (
        PlanTable(MajorUuid)                      ,
        uuid,Types::Coding                      ) ;
      Q = SC.sql.InsertInto                       (
            PlanTable(PDL)                        ,
            5                                     ,
            "uuid"                                ,
            "identification"                      ,
            "name"                                ,
            "comment"                             ,
            "rank"                              ) ;
    } else                                        {
      Q = SC.sql.Update                           (
            PlanTable(PDL)                        ,
            "where uuid = :UUID"                  ,
            4                                     ,
            "identification"                      ,
            "name"                                ,
            "comment"                             ,
            "rank"                              ) ;
    }                                             ;
    SC . Prepare ( Q )                            ;
    SC . Bind ("uuid"          ,uuid            ) ;
    SC . Bind ("identification",Ident           ) ;
    SC . Bind ("name"        ,language.toUtf8() ) ;
    SC . Bind ("comment"     ,comment .toUtf8() ) ;
    SC . Bind ("rank"        ,Rank              ) ;
    SC . Exec    (   )                            ;
    assureName(SC,uuid,vLanguageId,name)          ;
  LeaveSQL ( SC , plan->sql )                     ;
  Alert ( Done )                                  ;
  return true                                     ;
}
