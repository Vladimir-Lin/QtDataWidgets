#include <datawidgets.h>

N::KnowledgeBases:: KnowledgeBases   ( QWidget * parent , Plan * p )
                  : ListDock         (           parent ,        p )
                  , GroupItems       (                           p )
                  , KnowledgeManager (                           p )
                  , dropAction       ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::KnowledgeBases::~KnowledgeBases(void)
{
}

void N::KnowledgeBases::Configure(void)
{
  setWindowTitle               ( tr("Knowledge bases") ) ;
  setViewMode                  (IconMode               ) ;
  setIconSize                  (QSize(128,128)         ) ;
  setGridSize                  (QSize(144,160)         ) ;
  setMovement                  (Snap                   ) ;
  setResizeMode                (Adjust                 ) ;
  setSelectionMode             (SingleSelection        ) ;
  setWordWrap                  (true                   ) ;
  setMouseTracking             (true                   ) ;
  setWrapping                  (true                   ) ;
  setTextElideMode             (Qt::ElideNone          ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded  ) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded  ) ;
  setMinimumSize               (QSize(144,160)         ) ;
  setDragDropMode              (DragDrop               ) ;
  setDropFlag                  (DropTag      , true    ) ;
  setDropFlag                  (DropPicture  , true    ) ;
  setDropFlag                  (DropField    , true    ) ;
  plan -> setFont              (this                   ) ;
  MountCommands                (                       ) ;
}

bool N::KnowledgeBases::FocusIn(void)
{
  LinkAction ( Refresh , startup() ) ;
  LinkAction ( Copy    , Copy   () ) ;
  LinkAction ( Insert  , Insert () ) ;
  LinkAction ( Rename  , Rename () ) ;
  return true                        ;
}

QMimeData * N::KnowledgeBases::dragMime(void)
{
  QListWidgetItem * IT = currentItem()            ;
  if (IsNull(IT)) return NULL                     ;
  SUID  uuid  = nListUuid(IT)                     ;
  QMimeData * mime = new QMimeData()              ;
  setMime ( mime , "knowledgebase/uuid" , uuid )  ;
  if (NotNull(IT))                                {
    QIcon icon  = IT->icon()                      ;
    if (!icon.isNull())                           {
      QSize  s     = iconSize()                   ;
      QImage image = icon . pixmap(s) . toImage() ;
      if (!image.isNull())                        {
        mime -> setImageData(image)               ;
      }                                           ;
    }                                             ;
    mime -> setText ( IT->text() )                ;
  }                                               ;
  return mime                                     ;
}

bool N::KnowledgeBases::hasItem(void)
{
  QListWidgetItem * item = currentItem() ;
  return NotNull ( item )                ;
}

bool N::KnowledgeBases::startDrag(QMouseEvent * event)
{
  QListWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return false                                               ;
}

bool N::KnowledgeBases::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::KnowledgeBases::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::KnowledgeBases::finishDrag(QMouseEvent * event)
{
  return true   ;
}

bool N::KnowledgeBases::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

QString N::KnowledgeBases::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType         (
           mime                        ,
           "picture/uuid;picture/uuids;"
           "field/uuid;field/uuids;"
           "tag/uuid;tag/uuids"
         )                             ;
}

UUIDs N::KnowledgeBases::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="picture/uuid")          {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="picture/uuids")         {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="tag/uuid")              {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="tag/uuids")             {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::KnowledgeBases::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
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

bool N::KnowledgeBases::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::KnowledgeBases::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

bool N::KnowledgeBases::startup(void)
{
  clear ( )                                               ;
  PictureManager PM ( plan )                              ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    QString name                                          ;
    SUID    uu                                            ;
    UUIDs   Uuids = SC.Uuids                              (
                      PlanTable(KnowledgeBases)           ,
                      "uuid" , "order by id desc"       ) ;
    foreach (uu,Uuids)                                    {
      QIcon icon                                          ;
      name = Name(SC,uu,vLanguageId)                      ;
      SUID puid = FindSecond                              (
        SC                                                ,
        uu                                                ,
        Types  :: KnowledgeBase                           ,
        Types  :: Picture                                 ,
        Groups :: Icon                                    ,
        "order by position asc limit 0,1"               ) ;
      QImage * image   = PM.Thumb  (SC,puid)              ;
      if (NotNull(image))                                 {
        icon = PM.Icon(image,QSize(128,128))              ;
        delete image                                      ;
        image = NULL                                      ;
      } else                                              {
        icon = QIcon(":/images/knowledgebase.png")        ;
      }                                                   ;
      addItem ( uu , icon , name , name )                 ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert ( Done )                                          ;
  return true                                             ;
}

void N::KnowledgeBases::Copy(void)
{
  QMimeData * mime = dragMime    (      ) ;
  if (IsNull(mime)) return                ;
  qApp->clipboard()->setMimeData ( mime ) ;
}

void N::KnowledgeBases::Insert(void)
{
  QIcon Icon                                 ;
  Icon = QIcon(":/images/knowledgebase.png") ;
  NewListWidgetItem ( LWI              )     ;
  LWI->setIcon      ( Icon             )     ;
  LWI->setData      ( Qt::UserRole , 0 )     ;
  insertItem        ( 0 , LWI          )     ;
  scrollToItem      ( LWI              )     ;
  setCurrentItem    ( LWI              )     ;
  ItemEditing   = LWI                        ;
  QRect       R = visualItemRect(LWI)        ;
  QLineEdit * l = new QLineEdit(this)        ;
  QFont       f = font()                     ;
  QRect       L                              ;
  L . setTop    ( R.bottom    ()   )         ;
  L . setLeft   ( R.left      ()   )         ;
  L . setWidth  ( R.width     ()   )         ;
  L . setHeight ( f.pixelSize ()   )         ;
  setItemWidget ( ItemEditing , l )          ;
  l -> setGeometry ( L )                     ;
  ItemWidget = l                             ;
  connect(l   ,SIGNAL(editingFinished())     ,
          this,SLOT  (editingFinished()) )   ;
  l->setFocus(Qt::TabFocusReason)            ;
}

void N::KnowledgeBases::editingFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)   ;
  if (IsNull(l)) return                           ;
  /////////////////////////////////////////////////
  QString name = l->text()                        ;
  removeItemWidget(ItemEditing)                   ;
  if (name.length()<=0)                           {
    takeItem(row(ItemEditing))                    ;
    ItemEditing = NULL                            ;
    ItemWidget  = NULL                            ;
    return                                        ;
  }                                               ;
  ItemEditing->setText(name)                      ;
  /////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                     ;
    SUID uu = SC.Unique(PlanTable(MajorUuid)      ,
                          "uuid",6188           ) ;
    SC.assureUuid(PlanTable(MajorUuid),uu         ,
                  Types::KnowledgeBase          ) ;
    SC.insertUuid(PlanTable(KnowledgeBases)       ,
                  uu,"uuid"                     ) ;
    assureName(SC,uu,vLanguageId,name)            ;
    ItemEditing->setData ( Qt::UserRole , uu    ) ;
    ItemEditing->setText ( name                 ) ;
  LeaveSQL ( SC , plan->sql )                     ;
  /////////////////////////////////////////////////
  ItemEditing = NULL                              ;
  ItemWidget  = NULL                              ;
  Alert ( Done )                                  ;
}

void N::KnowledgeBases::Refresh(QListWidgetItem * item)
{
  PictureManager PM (plan)                        ;
  EnterSQL ( SC , plan->sql )                     ;
    SUID     uu   = nListUuid ( item  )           ;
    QString  name = ItemName  ( SC,uu )           ;
    SUID     puid = FindSecond                    (
             SC                                   ,
             uu                                   ,
             Types  :: KnowledgeBase              ,
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
      icon = QIcon(":/images/knowledgebase.png")  ;
      item->setIcon(icon)                         ;
    }                                             ;
    item -> setText    ( name    )                ;
  LeaveSQL ( SC , plan->sql )                     ;
}

void N::KnowledgeBases::Rename(void)
{
  QListWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  Rename ( item )                          ;
}

void N::KnowledgeBases::Rename(QListWidgetItem * item)
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
  connect(l    , SIGNAL(editingFinished())   ,
          this , SLOT  (renameFinished ()) ) ;
  l -> setFocus ( Qt::TabFocusReason )       ;
}

void N::KnowledgeBases::renameFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)         ;
  if (IsNull(l)) return                                 ;
  ///////////////////////////////////////////////////////
  QString name = l->text()                              ;
  SUID    uu   = nListUuid(ItemEditing)                 ;
  removeItemWidget(ItemEditing)                         ;
  if (name.length()<=0)                                 {
    ItemEditing = NULL                                  ;
    ItemWidget  = NULL                                  ;
    return                                              ;
  }                                                     ;
  ItemEditing->setText(name)                            ;
  if (uu<=0) return                                     ;
  ///////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                           ;
    assureName(SC,uu,vLanguageId,name)                  ;
  LeaveSQL ( SC , plan->sql )                           ;
  ///////////////////////////////////////////////////////
  ItemEditing = NULL                                    ;
  ItemWidget  = NULL                                    ;
  Alert ( Done )                                        ;
}

bool N::KnowledgeBases::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())   ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())   ;
  QListWidgetItem * item   = itemAt(pos)                       ;
  QPoint            global = mapToGlobal(pos)                  ;
  QAction       * a                                            ;
  nSafeExec(dock) mm.add(1000001,tr("Move to window area"))    ;
  nSafeExec(mdi ) mm.add(1000002,tr("Move to dock area"  ))    ;
  mm . setFont(plan)                                           ;
  a = mm.exec(global)                                          ;
  if (IsNull(a)) return true                                   ;
  switch (mm[a])                                               {
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

bool N::KnowledgeBases::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true          ;
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID kbase = nListUuid(atItem)         ;
  dropAction = true                      ;
  UUIDs Puids                            ;
  Puids << kbase                         ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    SUID uu                              ;
    foreach (uu,Uuids)                   {
      GI.Join                            (
        SC                               ,
        uu                               ,
        Types  :: Tag                    ,
        Types  :: KnowledgeBase          ,
        Groups :: Subordination          ,
        0                                ,
        Puids                          ) ;
    }                                    ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}

bool N::KnowledgeBases::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return true                          ;
  QPoint pos = psf.toPoint()                             ;
  QListWidgetItem * atItem = itemAt(pos)                 ;
  if (IsNull(atItem)) return true                        ;
  if (Uuids.count()<=0) return true                      ;
  SUID kbase = nListUuid(atItem)                         ;
  SUID puid  = Uuids[0]                                  ;
  dropAction = true                                      ;
  EnterSQL(SC,plan->sql)                                 ;
    QString Q                                            ;
    Q = SC.sql.DeleteFrom                                (
          PlanTable(Groups)                              ,
          QString("where first = %1 and t1 = %2 and t2 = %3 and relation = %4")
          .arg(kbase                                     )
          .arg(Types  :: KnowledgeBase                   )
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
    SC.Bind ("first"   ,kbase                          ) ;
    SC.Bind ("second"  ,puid                           ) ;
    SC.Bind ("t1"      ,Types  :: KnowledgeBase        ) ;
    SC.Bind ("t2"      ,Types  :: Picture              ) ;
    SC.Bind ("relation",Groups :: Icon                 ) ;
    SC.Exec (                                          ) ;
  LeaveSQL(SC,plan->sql)                                 ;
  Alert ( Done )                                         ;
  dropAction = false                                     ;
  startup()                                              ;
  return true                                            ;
}

bool N::KnowledgeBases::dropFields(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()             ;
  QListWidgetItem * atItem = itemAt(pos) ;
  if (IsNull(atItem)) return true        ;
  SUID kbid  = nListUuid(atItem)         ;
  dropAction = true                      ;
  UUIDs Puids                            ;
  Puids = Uuids                          ;
  GroupItems GI(plan)                    ;
  EnterSQL(SC,plan->sql)                 ;
    GI.Join                              (
      SC                                 ,
      kbid                               ,
      Types  :: KnowledgeBase            ,
      Types  :: Field                    ,
      Groups :: Subordination            ,
      0                                  ,
      Puids                            ) ;
  LeaveSQL(SC,plan->sql)                 ;
  Alert ( Done )                         ;
  dropAction = false                     ;
  return true                            ;
}
