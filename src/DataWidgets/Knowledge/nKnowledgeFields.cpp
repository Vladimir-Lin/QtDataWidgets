#include <datawidgets.h>

N::KnowledgeFields:: KnowledgeFields  ( QWidget * parent , Plan * p )
                   : ListDock         (           parent ,        p )
                   , GroupItems       (                           p )
                   , KnowledgeManager (                           p )
                   , Object           ( 0 , Types  :: KnowledgeBase )
                   , Relation         ( 0 , Groups :: Subordination )
                   , dropAction       ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::KnowledgeFields::~KnowledgeFields(void)
{
}

void N::KnowledgeFields::Configure(void)
{
  setWindowTitle               ( tr("Knowledge fields") ) ;
  setViewMode                  ( IconMode               ) ;
  setIconSize                  ( QSize(128,128)         ) ;
  setGridSize                  ( QSize(144,160)         ) ;
  setMovement                  ( Snap                   ) ;
  setResizeMode                ( Adjust                 ) ;
  setSelectionMode             ( ExtendedSelection      ) ;
  setWordWrap                  ( true                   ) ;
  setMouseTracking             ( true                   ) ;
  setWrapping                  ( true                   ) ;
  setTextElideMode             ( Qt::ElideNone          ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  ) ;
  setMinimumSize               ( QSize(144,160)         ) ;
  MountCommands                (                        ) ;
  setDragDropMode              ( DragDrop               ) ;
  setDropFlag                  ( DropTag       , true   ) ;
  setDropFlag                  ( DropPicture   , true   ) ;
  setDropFlag                  ( DropKnowledge , true   ) ;
  setDropFlag                  ( DropField     , true   ) ;
  plan -> setFont              ( this                   ) ;
}

bool N::KnowledgeFields::FocusIn(void)
{
  LinkAction   ( Refresh  , startup () ) ;
  LinkAction   ( Insert   , Insert  () ) ;
  LinkAction   ( Copy     , Copy    () ) ;
  LinkAction   ( Rename   , Rename  () ) ;
  if (uuid>0)                            {
    LinkAction ( Delete   , Delete  () ) ;
  }                                      ;
  return true                            ;
}

QMimeData * N::KnowledgeFields::dragMime(void)
{
  QList<QListWidgetItem *> items = selectedItems() ;
  QListWidgetItem * IT = currentItem()             ;
  nKickOut ( IsNull(IT)        , NULL )            ;
  nKickOut ( items.count() <=0 , NULL )            ;
  UUIDs Uuids                                      ;
  nFullLoop(i,items.count())                       {
    Uuids << nListUuid(items[i])                   ;
  }                                                ;
  QMimeData * mime = new QMimeData()               ;
  setMime ( mime , "field/uuids" , Uuids )         ;
  if (NotNull(IT))                                 {
    QIcon icon  = IT->icon()                       ;
    if (!icon.isNull())                            {
      QSize  s     = iconSize()                    ;
      QImage image = icon . pixmap(s) . toImage()  ;
      if (!image.isNull())                         {
        mime -> setImageData(image)                ;
      }                                            ;
    }                                              ;
    mime -> setText ( IT->text() )                 ;
  }                                                ;
  return mime                                      ;
}

bool N::KnowledgeFields::hasItem(void)
{
  QList<QListWidgetItem *> items = selectedItems() ;
  return ( items.count() > 0 )                     ;
}

bool N::KnowledgeFields::startDrag(QMouseEvent * event)
{
  QListWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return false                                               ;
}

bool N::KnowledgeFields::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::KnowledgeFields::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::KnowledgeFields::finishDrag(QMouseEvent * event)
{
  return true   ;
}

bool N::KnowledgeFields::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

QString N::KnowledgeFields::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType             (
           mime                            ,
           "picture/uuid;picture/uuids;"
           "knowledge/uuid;knowledge/uuids;"
           "field/uuid;field/uuids;"
           "tag/uuid;tag/uuids"
         )                                 ;
}

UUIDs N::KnowledgeFields::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="picture/uuid")          {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="picture/uuids")         {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="knowledge/uuid")        {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="knowledge/uuids")       {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="field/uuid")            {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="field/uuids")           {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="tag/uuid")              {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="tag/uuids")             {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::KnowledgeFields::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  QString    mtype                  ;
  UUIDs      Uuids                  ;
  if (source==this) return false    ;
  mtype = MimeType  (mime      )    ;
  Uuids = MimeUuids (mime,mtype)    ;
  if (mtype=="picture/uuid"        ||
      mtype=="picture/uuids"        )
    plan->showMessage               (
      tr("Assign %1 icon from %2"   )
      .arg(Uuids.count()            )
      .arg(source->windowTitle()) ) ;
  if (mtype=="tag/uuid"            ||
      mtype=="tag/uuids"            )
    plan->showMessage               (
      tr("Assign %1 tags to %2"     )
      .arg(Uuids.count()            )
      .arg(source->windowTitle()) ) ;
  return true                       ;
}

bool N::KnowledgeFields::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::KnowledgeFields::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

bool N::KnowledgeFields::startup(void)
{
  clear ( )                                               ;
  PictureManager PM ( plan )                              ;
  GroupItems     GI ( plan )                              ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    QString name                                          ;
    SUID    uu                                            ;
    UUIDs   Uuids                                         ;
    if (uuid>0)                                           {
      Uuids = GI.Subordination                            (
                SC                                        ,
                uuid                                      ,
                type                                      ,
                Types::Field                              ,
                linkType                                  ,
                "order by position asc"                 ) ;
      name = SC.getName                                   (
               PlanTable(Names)                           ,
               "uuid"                                     ,
               vLanguageId                                ,
               uuid                                     ) ;
      setWindowTitle(name)                                ;
    } else                                                {
      Uuids = SC.Uuids                                    (
                PlanTable(KnowledgeFields)                ,
                "uuid" , "order by id asc"              ) ;
    }                                                     ;
    foreach (uu,Uuids)                                    {
      QIcon icon                                          ;
      name = Name(SC,uu,vLanguageId)                      ;
      SUID puid = FindSecond                              (
        SC                                                ,
        uu                                                ,
        Types  :: Field                                   ,
        Types  :: Picture                                 ,
        Groups :: Icon                                    ,
        "order by position asc limit 0,1"               ) ;
      QImage * image   = PM.Thumb  (SC,puid)              ;
      if (NotNull(image))                                 {
        icon = PM.Icon(image,QSize(128,128))              ;
        delete image                                      ;
        image = NULL                                      ;
      } else                                              {
        icon = QIcon(":/images/openoffice.png")           ;
      }                                                   ;
      addItem ( uu , icon , name , name )                 ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert ( Done )                                          ;
  return true                                             ;
}

void N::KnowledgeFields::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

void N::KnowledgeFields::Delete(void)
{
  nDropOut ( uuid <= 0    )              ;
  QListWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item) )              ;
  SUID  kuid = nListUuid(item)           ;
  UUIDs Uuids                            ;
  Uuids << kuid                          ;
  GroupItems GI ( plan      )            ;
  EnterSQL ( SC , plan->sql )            ;
    GI.Detach                            (
      SC                                 ,
      uuid                               ,
      type                               ,
      Types::Field                       ,
      linkType                           ,
      Uuids                            ) ;
  LeaveSQL ( SC , plan->sql )            ;
  ////////////////////////////////////////
  int index = row(item)                  ;
  if (index<0)                           {
    Alert    ( Error )                   ;
  } else                                 {
    takeItem ( index )                   ;
    Alert    ( Done  )                   ;
  }                                      ;
}

void N::KnowledgeFields::Insert(void)
{
  QIcon Icon                               ;
  Icon = QIcon(":/images/openoffice.png")  ;
  NewListWidgetItem ( LWI               )  ;
  LWI->setIcon      ( Icon              )  ;
  LWI->setData      ( Qt::UserRole , 0  )  ;
  insertItem        ( 0 , LWI           )  ;
  scrollToItem      ( LWI               )  ;
  setCurrentItem    ( LWI               )  ;
  ItemEditing   = LWI                      ;
  QRect       R = visualItemRect(LWI)      ;
  QLineEdit * l = new QLineEdit(this)      ;
  QFont       f = font()                   ;
  QRect       L                            ;
  L . setTop    ( R.bottom    ()   )       ;
  L . setLeft   ( R.left      ()   )       ;
  L . setWidth  ( R.width     ()   )       ;
  L . setHeight ( f.pixelSize ()   )       ;
  setItemWidget ( ItemEditing , l )        ;
  l -> setGeometry ( L )                   ;
  ItemWidget = l                           ;
  nConnect(l   ,SIGNAL(editingFinished())  ,
           this,SLOT  (editingFinished())) ;
  l->setFocus(Qt::TabFocusReason)          ;
}

void N::KnowledgeFields::editingFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)  ;
  if (IsNull(l)) return                          ;
  ////////////////////////////////////////////////
  QString name = l->text()                       ;
  removeItemWidget(ItemEditing)                  ;
  if (name.length()<=0)                          {
    takeItem(row(ItemEditing))                   ;
    ItemEditing = NULL                           ;
    ItemWidget  = NULL                           ;
    return                                       ;
  }                                              ;
  ItemEditing->setText(name)                     ;
  ////////////////////////////////////////////////
  GroupItems GI ( plan )                         ;
  EnterSQL ( SC , plan->sql )                    ;
    SUID uu = SC.Unique(PlanTable(MajorUuid)     ,
                          "uuid",6168          ) ;
    SC.assureUuid(PlanTable(MajorUuid),uu        ,
                  Types::Field                 ) ;
    SC.insertUuid(PlanTable(KnowledgeFields)     ,
                  uu,"uuid"                    ) ;
    assureName(SC,uu,vLanguageId,name)           ;
    if (uuid>0)                                  {
      UUIDs Juids                                ;
      Juids << uu                                ;
      GI.Join                                    (
        SC                                       ,
        uuid                                     ,
        type                                     ,
        Types::Field                             ,
        linkType                                 ,
        0,Juids                                ) ;
    }                                            ;
    ItemEditing->setData ( Qt::UserRole , uu   ) ;
    ItemEditing->setText ( name                ) ;
  LeaveSQL ( SC , plan->sql )                    ;
  ////////////////////////////////////////////////
  ItemEditing = NULL                             ;
  ItemWidget  = NULL                             ;
  Alert ( Done )                                 ;
}

void N::KnowledgeFields::Refresh(QListWidgetItem * item)
{
  PictureManager PM (plan)                        ;
  EnterSQL ( SC , plan->sql )                     ;
    SUID     uu   = nListUuid ( item  )           ;
    QString  name = ItemName  ( SC,uu )           ;
    SUID     puid = FindSecond                    (
             SC                                   ,
             uu                                   ,
             Types  :: Field                      ,
             Types  :: Picture                    ,
             Groups :: Icon                       ,
             "order by position asc limit 0,1" )  ;
    QImage * image   = PM.Thumb      ( SC,puid )  ;
    if (NotNull(image))                           {
      QSize is(128,128)                           ;
      QIcon icon = PM . Icon (image,is)           ;
      delete image                                ;
      item->setIcon(icon)                         ;
    } else                                        {
      QIcon icon                                  ;
      icon = QIcon(":/images/openoffice.png")     ;
      item->setIcon(icon)                         ;
    }                                             ;
    item -> setText    ( name    )                ;
  LeaveSQL ( SC , plan->sql )                     ;
}

void N::KnowledgeFields::Rename(void)
{
  QListWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  Rename ( item )                          ;
}

void N::KnowledgeFields::Rename(QListWidgetItem * item)
{
  if (IsNull(item)) return                   ;
  setCurrentItem(item)                       ;
  ItemEditing = item                         ;
  QRect       R = visualItemRect(item)       ;
  QLineEdit * l = new QLineEdit(this)        ;
  QFont       f = font()                     ;
  QRect       L                              ;
  L . setLeft   ( R . left      ())          ;
  L . setTop    ( R . bottom    ())          ;
  L . setWidth  ( R . width     ())          ;
  L . setHeight ( f . pixelSize ())          ;
  setItemWidget ( ItemEditing , l )          ;
  l -> setGeometry ( L )                     ;
  ItemWidget = l                             ;
  l -> setText  ( item -> text ( )   )       ;
  nConnect(l    , SIGNAL(editingFinished())  ,
           this , SLOT  (renameFinished ())) ;
  l -> setFocus ( Qt::TabFocusReason )       ;
}

void N::KnowledgeFields::renameFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(l)) return                         ;
  ///////////////////////////////////////////////
  QString name = l->text()                      ;
  SUID    uu   = nListUuid(ItemEditing)         ;
  removeItemWidget(ItemEditing)                 ;
  if (name.length()<=0)                         {
    ItemEditing = NULL                          ;
    ItemWidget  = NULL                          ;
    return                                      ;
  }                                             ;
  ItemEditing->setText(name)                    ;
  if (uu<=0) return                             ;
  ///////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                   ;
    assureName(SC,uu,vLanguageId,name)          ;
  LeaveSQL ( SC , plan->sql )                   ;
  ///////////////////////////////////////////////
  ItemEditing = NULL                            ;
  ItemWidget  = NULL                            ;
  Alert ( Done )                                ;
}

bool N::KnowledgeFields::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                    ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())   ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())   ;
  QListWidgetItem * item   = itemAt(pos)                       ;
  QPoint            global = mapToGlobal(pos)                  ;
  QAction       * a                                            ;
  if (NotNull(item))                                           {
    mm.add(501,tr("Deacon"))                                   ;
    if (uuid>0) mm.add(502,tr("Detach"))                       ;
    mm.add(503,tr("Knowledge units"))                          ;
  }                                                            ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont(plan)                                           ;
  a = mm.exec(global)                                          ;
  if (IsNull(a)) return true                                   ;
  switch (mm[a])                                               {
    case 501                                                   :
      plan -> Talk ( item->text() )                            ;
    break                                                      ;
    case 502                                                   :
      Delete ( )                                               ;
    break                                                      ;
    case 503                                                   :
      emit Knowledge                                           (
             this                                              ,
             nListUuid(item)                                   ,
             Types::Field                            ,
             item->text()                                    ) ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi ( this , 0 )                              ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::RightDockWidgetArea                                ,
        Qt::TopDockWidgetArea                                  |
        Qt::BottomDockWidgetArea                               |
        Qt::LeftDockWidgetArea                                 |
        Qt::RightDockWidgetArea                              ) ;
    break                                                      ;
    default                                                    :
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}

bool N::KnowledgeFields::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true          ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID field = nListUuid(atItem)         ;
  dropAction = true                      ;
  UUIDs Puids                            ;
  Puids << field                         ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    SUID uuid                            ;
    foreach (uuid,Uuids)                 {
      GI.Join                            (
        SC                               ,
        uuid                             ,
        Types  :: Tag                    ,
        Types  :: Field                  ,
        Groups :: Subordination          ,
        0                                ,
        Puids                          ) ;
    }                                    ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}

bool N::KnowledgeFields::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true                          ;
  QPoint pos = psf.toPoint()                             ;
  QListWidgetItem * atItem = itemAt(pos)                 ;
  if (IsNull(atItem)) return true                        ;
  if (Uuids.count()<=0) return true                      ;
  SUID field = nListUuid(atItem)                         ;
  SUID puid  = Uuids[0]                                  ;
  dropAction = true                                      ;
  EnterSQL(SC,plan->sql)                                 ;
    QString Q                                            ;
    Q = SC.sql.DeleteFrom                                (
          PlanTable(Groups)                              ,
          QString("where first = %1 and t1 = %2 and t2 = %3 and relation = %4")
          .arg(field                                     )
          .arg(Types  :: Field                           )
          .arg(Types  :: Picture                         )
          .arg(Groups :: Icon                            )
        )                                                ;
    SC.Query(Q)                                          ;
    Q = SC.sql.InsertInto                                (
          PlanTable(Groups)                              ,
          5                                              ,
          "first"                                        ,
          "second"                                       ,
          "t1"                                           ,
          "t2"                                           ,
          "relation"                                   ) ;
    SC.Prepare ( Q                                     ) ;
    SC.Bind ("first"   ,field                          ) ;
    SC.Bind ("second"  ,puid                           ) ;
    SC.Bind ("t1"      ,Types  :: Field                ) ;
    SC.Bind ("t2"      ,Types  :: Picture              ) ;
    SC.Bind ("relation",Groups :: Icon                 ) ;
    SC.Exec (                                          ) ;
  LeaveSQL(SC,plan->sql)                                 ;
  Alert ( Done )                                         ;
  dropAction = false                                     ;
  startup()                                              ;
  return true                                            ;
}

bool N::KnowledgeFields::dropKnowledge(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true          ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID field = nListUuid(atItem)         ;
  dropAction = true                      ;
  UUIDs Puids = Uuids                    ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    GI.Join                              (
      SC                                 ,
      field                              ,
      Types  :: Field                    ,
      Types  :: Knowledge                ,
      Groups :: Subordination            ,
      0                                  ,
      Puids                            ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}

bool N::KnowledgeFields::dropFields(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID field = nListUuid(atItem)         ;
  dropAction = true                      ;
  UUIDs Puids                            ;
  Puids = Uuids                          ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    GI.Join                              (
      SC                                 ,
      field                              ,
      Types  :: Field                    ,
      Types  :: Field                    ,
      Groups :: Subordination            ,
      0                                  ,
      Puids                            ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}
