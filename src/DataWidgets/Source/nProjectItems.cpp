#include <datawidgets.h>

#define TABLES(ID) plan->Tables[Tables::ID]

N::ProjectItems:: ProjectItems (QWidget * parent,Plan * p)
                : IconDock     (          parent,       p)
                , GroupItems   (                        p)
                , Group        (0                        )
                , Type         (Types::None              )
                , dropAction   (false                    )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ProjectItems::~ProjectItems (void)
{
}

QListWidgetItem * N::ProjectItems::uuidAt(SUID uuid)
{
  for (int i=0;i<count();i++)      {
    QListWidgetItem * it = item(i) ;
    SUID suid = nListUuid (it)     ;
    if (suid==uuid) return it      ;
  }                                ;
  return NULL                      ;
}

bool N::ProjectItems::hasItem(void)
{
  return ( Items.count()>0 ) ;
}

bool N::ProjectItems::startDrag(QMouseEvent * event)
{
  Items . clear ( )                                          ;
  QListWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  for (int i=0;i<count();i++)                                {
    if (item(i)->isSelected()) Items << item(i)              ;
  }                                                          ;
  return false                                               ;
}

bool N::ProjectItems::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

QMimeData * N::ProjectItems::dragMime(void)
{
  if (Items.count()<=0) return NULL               ;
  QMimeData       * mime    = new QMimeData ( )   ;
  if (Items.count()==1)                           {
    SUID        type = nListUuid(Items[0])        ;
    QByteArray  data = CreateByteArray(type)      ;
    mime -> setData ("division/uuid",data)        ;
  } else                                          {
    UUIDs       Uuids = Selected        (       ) ;
    QByteArray  data  = CreateByteArray ( Uuids ) ;
    mime -> setData ("division/uuids",data)       ;
  }                                               ;
  QString tooltip                                 ;
  tooltip = tr("%1 items selected"                )
            .arg(Items.count()                  ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

void N::ProjectItems::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::ProjectItems::finishDrag(QMouseEvent * event)
{
  if (Items.count()>0) {
    Items . clear ( )  ;
    return false       ;
  }                    ;
  Items . clear   ( )  ;
  return true          ;
}

QString N::ProjectItems::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType            (
           mime                           ,
           "album/uuid;album/uuids;"
           "video/uuid;video/uuids;"
           "picture/uuid;picture/uuids;"
           "people/uuid;people/uuids;"
           "division/uuid;division/uuids"
         )                                ;
}

UUIDs N::ProjectItems::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                                           ;
  QByteArray data = mime->data(mimetype)                     ;
  if (data.size()<=0) return Uuids                           ;
  if (mimetype=="album/uuid"    ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="album/uuids"   ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="video/uuid"    ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="video/uuids"   ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="picture/uuid"  ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="picture/uuids" ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="people/uuid"   ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="people/uuids"  ) Uuids  = GetUuids ( data ) ; else
  if (mimetype=="division/uuid" ) Uuids << GetUuid  ( data ) ; else
  if (mimetype=="division/uuids") Uuids  = GetUuids ( data ) ;
  return Uuids                                               ;
}

bool N::ProjectItems::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::ProjectItems::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                           ;
  QString    mtype                                       ;
  UUIDs      Uuids                                       ;
  Alert ( Click )                                        ;
  if (source==this)                                      {
    mtype = MimeType  (mime      )                       ;
    Uuids = MimeUuids (mime,mtype)                       ;
    plan->showMessage                                    (
      tr("Start moving %1 groups"                        )
                .arg(Uuids.count()))                     ;
    plan->processEvents()                                ;
  } else                                                 {
    if (mime->hasUrls())                                 {
    } else                                               {
      mtype = MimeType  (mime      )                     ;
      Uuids = MimeUuids (mime,mtype)                     ;
      if (mtype=="album/uuid"   || mtype=="album/uuids"  )
      plan->showMessage                                  (
        tr("Join %1 album from %2"                       )
        .arg(Uuids.count()                               )
        .arg(source->windowTitle())                    ) ;
      if (mtype=="picture/uuid" || mtype=="picture/uuids")
      plan->showMessage                                  (
        tr("Join %1 pictures from %2"                    )
        .arg(Uuids.count()                               )
        .arg(source->windowTitle())                    ) ;
      if (mtype=="people/uuid"  || mtype=="people/uuids" )
      plan->showMessage                                  (
        tr("Join %1 people from %2"                      )
        .arg(Uuids.count()                               )
        .arg(source->windowTitle())                    ) ;
      if (mtype=="video/uuid"   || mtype=="video/uuids"  )
      plan->showMessage                                  (
        tr("Join %1 video from %2"                       )
        .arg(Uuids.count()                               )
        .arg(source->windowTitle())                    ) ;
      plan->processEvents()                              ;
    }                                                    ;
  }                                                      ;
  return true                                            ;
}

bool N::ProjectItems::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::ProjectItems::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

bool N::ProjectItems::removeDrop(void)
{
  return true ;
}

void N::ProjectItems::Configure(void)
{
  setDragDropMode              (DragDrop             ) ;
  setViewMode                  (IconMode             ) ;
  setIconSize                  (QSize(128,128)       ) ;
  setGridSize                  (QSize(144,192)       ) ;
  setMovement                  (Snap                 ) ;
  setResizeMode                (Adjust               ) ;
  setSelectionMode             (ExtendedSelection    ) ;
  setWordWrap                  (true                 ) ;
  setMouseTracking             (true                 ) ;
  setWrapping                  (true                 ) ;
  setTextElideMode             (Qt::ElideNone        ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded) ;
  setMinimumSize               (QSize(144,192)       ) ;
  setDropFlag                  (DropText     , true  ) ;
  setDropFlag                  (DropUrls     , true  ) ;
  setDropFlag                  (DropImage    , true  ) ;
  setDropFlag                  (DropHtml     , true  ) ;
  setDropFlag                  (DropColor    , true  ) ;
  setDropFlag                  (DropTag      , true  ) ;
  setDropFlag                  (DropPicture  , true  ) ;
  setDropFlag                  (DropPeople   , true  ) ;
  setDropFlag                  (DropVideo    , true  ) ;
  setDropFlag                  (DropAlbum    , true  ) ;
  setDropFlag                  (DropGender   , true  ) ;
  setDropFlag                  (DropDivision , true  ) ;
  plan->setFont                (this                 ) ;
  MountCommands                (                     ) ;
}

bool N::ProjectItems::Shutdown(void)
{
  emit Leave(this) ;
  return true      ;
}

bool N::ProjectItems::Relocation(void)
{
  return false ;
}

UUIDs N::ProjectItems::GroupUuids(SqlConnection & Connection)
{
  return Connection.Uuids(TABLES(Groups),"second" ,
           QString("where first = %1 "
                   "and t1 = %2 "
                   "and t2 = %3 "
                   "and relation = %4 "
                   "order by position asc"        )
           .arg ( Group                           )
           .arg ( Types::Project        )
           .arg ( Type                            )
           .arg ( Groups::Subordination )
         )                                        ;
}

QIcon N::ProjectItems::GetIcon(SqlConnection & Connection,SUID uuid)
{
  PictureManager PM ( plan )            ;
  QIcon          icon                   ;
  QImage       * Thumb = NULL           ;
  Thumb = PM.Thumb( Connection , uuid ) ;
  if (NotNull(Thumb))                   {
    QSize is(128,128)                   ;
    icon = PM . Icon ( Thumb , is )     ;
    delete Thumb                        ;
  } else icon = Icon                    ;
  return icon                           ;
}

bool N::ProjectItems::startup(void)
{
  clear    (                )                  ;
  EnterSQL ( SC , plan->sql )                  ;
    UUIDs Uuids = GroupUuids ( SC )            ;
    SUID  uuid                                 ;
    foreach (uuid,Uuids)                       {
      QListWidgetItem * lwi                    ;
      QString name = SC.getName                (
        plan->Tables[Tables::Names]  ,
        "uuid",plan->LanguageId,uuid         ) ;
      SUID    puid = FindSecond                (
                  SC,uuid,Type                 ,
                  Types::Picture     ,
                  Groups::Icon       ,
           "order by position asc limit 0,1" ) ;
      QIcon   icon = GetIcon ( SC , puid     ) ;
      QString tooltip = name                   ;
      lwi = addItem ( uuid,icon,name,tooltip ) ;
      if (NotNull(lwi))                        {
        lwi->setData(Qt::UserRole+1,(int)Type) ;
      }                                        ;
    }                                          ;
  LeaveSQL ( SC , plan->sql )                  ;
  QString m = tr("%1 items").arg(count())      ;
  Alert ( Done )                               ;
  setToolTip ( m )                             ;
  plan->Talk ( m )                             ;
  return true                                  ;
}

bool N::ProjectItems::FocusIn(void)
{
  connectAction(Menus::Insert    ,this,SLOT(New            ())) ;
  connectAction(Menus::Rename    ,this,SLOT(Rename         ())) ;
  connectAction(Menus::Refresh   ,this,SLOT(startup        ())) ;
  connectAction(Menus::Copy      ,this,SLOT(CopyToClipboard())) ;
  connectAction(Menus::SelectNone,this,SLOT(SelectNone     ())) ;
  connectAction(Menus::SelectAll ,this,SLOT(SelectAll      ())) ;
  connectAction(Menus::Delete    ,this,SLOT(Delete         ())) ;
  return true                                                             ;
}

bool N::ProjectItems::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  bool            hasSomething = false                          ;
  if (Group>0)                                                  {
    mm.add(901,tr("Delete"))                                    ;
  }                                                             ;
  if (hasSomething)                                             {
    if (NotNull(dock) || NotNull(mdi )) mm.addSeparator (    )  ;
  }                                                             ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( SystemPlan    )                                  ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 901                                                    :
      Delete ( )                                                ;
    break                                                       ;
    case 1000001                                                :
      emit attachMdi ( this , 0 )                               ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this                                                    ,
        windowTitle()                                           ,
        Qt::BottomDockWidgetArea                                ,
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

void N::ProjectItems::New(void)
{
  NewListWidgetItem ( LWI              )   ;
  LWI->setIcon      ( Icon             )   ;
  LWI->setData      ( Qt::UserRole , 0 )   ;
  insertItem        ( 0 , LWI          )   ;
  scrollToItem      ( LWI              )   ;
  setCurrentItem    ( LWI              )   ;
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
  connect(l   ,SIGNAL(editingFinished())   ,
          this,SLOT  (editingFinished()) ) ;
  l->setFocus(Qt::TabFocusReason)          ;
}

void N::ProjectItems::editingFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)         ;
  if (IsNull(l)) return                                 ;
  ///////////////////////////////////////////////////////
  QString name = l->text()                              ;
  removeItemWidget(ItemEditing)                         ;
  if (name.length()<=0)                                 {
    takeItem(row(ItemEditing))                          ;
    ItemEditing = NULL                                  ;
    ItemWidget  = NULL                                  ;
    return                                              ;
  }                                                     ;
  ItemEditing->setText(name)                            ;
  ///////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                           ;
    SUID uuid = AppendGroup ( SC , name )               ;
    SC . close              (           )               ;
    ItemEditing->setData ( Qt::UserRole   ,      uuid ) ;
    ItemEditing->setData ( Qt::UserRole+1 , (int)Type ) ;
  LeaveSQL ( SC , plan->sql )                           ;
  ///////////////////////////////////////////////////////
  ItemEditing = NULL                                    ;
  ItemWidget  = NULL                                    ;
  Alert ( Done )                                        ;
}

void N::ProjectItems::Refresh(QListWidgetItem * item)
{
  PictureManager PM (plan)                       ;
  EnterSQL ( SC , plan->sql )                    ;
    SUID     uuid    = nListUuid(item)           ;
    QString  name    = ItemName      ( SC,uuid ) ;
    SUID     puid    = FindSecond                (
             SC                                  ,
             uuid                                ,
             Type                                ,
             Types::Picture            ,
             Groups::Icon              ,
             "order by position asc limit 0,1" ) ;
    int      total   = Counting      ( SC,uuid ) ;
    QImage * image   = PM.Thumb      ( SC,puid ) ;
    QString  tooltip = ToolTipTotoal ( total   ) ;
    if (NotNull(image))                          {
      QSize is(128,128)                          ;
      QIcon icon = PM . Icon (image,is)          ;
      delete image                               ;
      item->setIcon(icon)                        ;
    } else                                       {
      QIcon icon = Icon                          ;
      item->setIcon(icon)                        ;
    }                                            ;
    item -> setText    ( name    )               ;
    item -> setToolTip ( tooltip )               ;
  LeaveSQL ( SC , plan->sql )                    ;
}

void N::ProjectItems::Rename(void)
{
  QListWidgetItem * item = currentItem ( ) ;
  if (IsNull(item)) return                 ;
  Rename ( item )                          ;
}

void N::ProjectItems::Rename(QListWidgetItem * item)
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

void N::ProjectItems::renameFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)  ;
  if (IsNull(l)) return                          ;
  ////////////////////////////////////////////////
  QString name = l->text()                       ;
  SUID    uuid = nListUuid(ItemEditing)          ;
  removeItemWidget(ItemEditing)                  ;
  if (name.length()<=0)                          {
    ItemEditing = NULL                           ;
    ItemWidget  = NULL                           ;
    return                                       ;
  }                                              ;
  ItemEditing->setText(name)                     ;
  if (uuid<=0) return                            ;
  ////////////////////////////////////////////////
  DocumentManager DM ( plan )                    ;
  EnterSQL  ( SC , plan->sql )                   ;
    DM.assureName(SC,uuid,plan->LanguageId,name) ;
  LeaveSQL  ( SC , plan->sql )                   ;
  ////////////////////////////////////////////////
  ItemEditing = NULL                             ;
  ItemWidget  = NULL                             ;
  Alert ( Done )                                 ;
}

void N::ProjectItems::Remove(SUID uuid)
{
  nDropOut ( Group <= 0 ) ;
  UUIDs Uuids             ;
  Uuids << uuid           ;
  Remove(Uuids)           ;
}

void N::ProjectItems::Remove(UUIDs & Uuids)
{
  GroupItems GI ( plan )                           ;
  EnterSQL ( SC , plan->sql )                       ;
    SUID uuid                                       ;
    QListWidgetItem * it                            ;
    foreach (uuid,Uuids)                            {
      it = uuidAt(uuid)                             ;
      if (NotNull(it))                              {
        int type = it->data(Qt::UserRole+1).toInt() ;
        int index = row(it)                         ;
        UUIDs Duids                                 ;
        Duids << uuid                               ;
        GI.Detach                                   (
          SC                                        ,
          Group                                     ,
          Type                                      ,
          type                                      ,
          Groups::Subordination           ,
          Duids                                   ) ;
        if (index>=0) takeItem(index)               ;
      }                                             ;
    }                                               ;
  LeaveSQL ( SC , plan->sql )                       ;
  Alert    ( Done           )                       ;
}

void N::ProjectItems::Delete(void)
{
  nDropOut ( Group <= 0         ) ;
  UUIDs Uuids = Selected ()       ;
  nDropOut ( Uuids.count() <= 0 ) ;
  Remove ( Uuids )                ;
}

SUID N::ProjectItems::AppendGroup(SqlConnection & Connection,QString name)
{
  return 0 ;
}

QString N::ProjectItems::ToolTipTotoal(int total)
{
  return tr("%1 items in this group").arg(total) ;
}

int N::ProjectItems::Counting(SqlConnection & Connection,SUID uuid)
{
  return GroupItems::Count(Connection,uuid,Types::Project,Type,Groups::Subordination) ;
}

bool N::ProjectItems::setGroupIcon(SUID uuid,const QImage & image)
{
  PictureManager PM ( plan )                      ;
  SUID puid    = 0                                ;
  bool success = false                            ;
  QImage I = image                                ;
  Bustle  ( )                                     ;
  success = PM . Import ( I , puid )              ;
  Vacancy ( )                                     ;
  nKickOut ( puid <= 0 , true )                   ;
  EnterSQL  ( SC , plan->sql                    ) ;
    QString Q                                     ;
    Q = SC.sql.DeleteFrom                         (
          PlanTable(Groups)                       ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4"             )
          .arg(uuid                               )
          .arg(Type                               )
          .arg(Types::Picture           )
          .arg(Groups::Icon             )
        )                                         ;
    SC.Query(Q)                                   ;
    Q = SC.sql.InsertInto                         (
          PlanTable(Groups)                       ,
          5                                       ,
          "first"                                 ,
          "second"                                ,
          "t1"                                    ,
          "t2"                                    ,
          "relation"                            ) ;
    SC.Prepare ( Q                              ) ;
    SC.Bind ("first"   ,uuid                    ) ;
    SC.Bind ("second"  ,puid                    ) ;
    SC.Bind ("t1"      ,Type                    ) ;
    SC.Bind ("t2"      ,Types::Picture) ;
    SC.Bind ("relation",Groups::Icon  ) ;
    SC.Exec (                                   ) ;
  LeaveSQL  ( SC , plan->sql                    ) ;
  return true                                     ;
}

bool N::ProjectItems::setGroupIcon(SUID uuid,SUID picture)
{
  EnterSQL ( SC , plan->sql )                     ;
    QString Q                                     ;
    Q = SC.sql.DeleteFrom                         (
          PlanTable(Groups)                       ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4"             )
          .arg(uuid                               )
          .arg(Type                               )
          .arg(Types::Picture           )
          .arg(Groups::Icon             )
        )                                         ;
    SC.Query(Q)                                   ;
    Q = SC.sql.InsertInto                         (
          PlanTable(Groups)                       ,
          5                                       ,
          "first"                                 ,
          "second"                                ,
          "t1"                                    ,
          "t2"                                    ,
          "relation"                            ) ;
    SC.Prepare ( Q                              ) ;
    SC.Bind ("first"   ,uuid                    ) ;
    SC.Bind ("second"  ,picture                 ) ;
    SC.Bind ("t1"      ,Type                    ) ;
    SC.Bind ("t2"      ,Types::Picture) ;
    SC.Bind ("relation",Groups::Icon  ) ;
    SC.Exec (                                   ) ;
  LeaveSQL ( SC , plan->sql )                     ;
  return true                                     ;
}

bool N::ProjectItems::dropText(QWidget * source,QPointF psf,const QString & text)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinText(uuid,text)           ;
#endif
  return true ;
}

bool N::ProjectItems::dropUrls(QWidget * source,QPointF psf,const QList<QUrl> & urls)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinUrls(uuid,urls)           ;
#endif
  return true ;
}

bool N::ProjectItems::dropImage(QWidget * source,QPointF psf,const QImage & image)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinImage(uuid,image)         ;
#endif
  return true ;
}

bool N::ProjectItems::dropHtml(QWidget * source,QPointF psf,const QString & html)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinHtml(uuid,html)           ;
#endif
  return true ;
}

bool N::ProjectItems::dropColor(QWidget * source,QPointF psf,const QColor & color)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinColor(uuid,color)         ;
#endif
  return true ;
}

bool N::ProjectItems::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinTags(uuid,Uuids)          ;
#endif
  return true ;
}

bool N::ProjectItems::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinPictures(uuid,Uuids)      ;
#endif
  return true ;
}

bool N::ProjectItems::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinPeople(uuid,Uuids)        ;
#endif
  return true ;
}

bool N::ProjectItems::dropVideos(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinVideos(uuid,Uuids)        ;
#endif
  return true ;
}

bool N::ProjectItems::dropAlbums(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinAlbums(uuid,Uuids)        ;
#endif
  return true ;
}

bool N::ProjectItems::dropGender(QWidget * source,QPointF psf,const SUID gender)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  if (source==this) return false       ;
  QListWidgetItem * item = itemAt(pos) ;
  if (IsNull(item)) return  false      ;
  SUID uuid = nListUuid(item)          ;
  if (uuid<=0) return false            ;
  return JoinGender(uuid,gender)       ;
#endif
  return true ;
}

bool N::ProjectItems::dropDivisions(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  QListWidgetItem * item = itemAt(pos) ;
  SUID uuid = 0                        ;
  if (source==this)                    {
    if (IsNull(item)) return false     ;
    uuid = nListUuid(item)             ;
    if (uuid<=0) return false          ;
    return MoveGroups(uuid,Uuids)      ;
  }                                    ;
  return JoinGroups(uuid,Uuids)        ;
#endif
  return true ;
}

bool N::ProjectItems::JoinText(SUID uuid,const QString & text)
{
  return true ;
}

bool N::ProjectItems::JoinUrls(SUID uuid,const QList<QUrl> & urls)
{
  return true ;
}

bool N::ProjectItems::JoinImage(SUID uuid,const QImage & image)
{
  return true ;
}

bool N::ProjectItems::JoinHtml(SUID uuid,const QString & html)
{
  return true ;
}

bool N::ProjectItems::JoinColor(SUID uuid,const QColor & color)
{
  return true ;
}

bool N::ProjectItems::JoinTags(SUID uuid,const UUIDs & Uuids)
{
  return true ;
}

bool N::ProjectItems::JoinPictures(SUID uuid,const UUIDs & Uuids)
{
  return true ;
}

bool N::ProjectItems::JoinGallery(SUID uuid,const UUIDs & Uuids)
{
  QListWidgetItem * item  = uuidAt(uuid)               ;
  if (IsNull(item)) return false                       ;
  //////////////////////////////////////////////////////
  dropAction = true                                    ;
  //////////////////////////////////////////////////////
  QString           title = item->text()               ;
  plan -> showMessage(tr("%1 pictures join gallery %2" )
    .arg(Uuids.count()).arg(title)                   ) ;
  plan -> processEvents (                            ) ;
  //////////////////////////////////////////////////////
  Bustle  ( )                                          ;
  SqlConnection Connection ( plan->sql               )  ;
  if (Connection.open("nGroupView","JoinGallery")    ) {
    UUIDs Guids = Uuids                                ;
    GroupItems::Join                                   (
      Connection,uuid,Type                             ,
      Types::Picture                         ,
      Groups::Subordination,0,Guids        ) ;
    Connection . close ( )                             ;
  }                                                    ;
  Connection . remove ( )                              ;
  Vacancy ( )                                          ;
  //////////////////////////////////////////////////////
  plan->showMessage                                    (
    tr("%1 pictures join gallery %2 finished"          )
    .arg(Uuids.count()).arg(title)                   ) ;
  //////////////////////////////////////////////////////
  Alert ( Done )                                       ;
  dropAction = false                                   ;
  return true                                          ;
}

bool N::ProjectItems::JoinGallery(SUID uuid,const QImage & image)
{
  PictureManager PM ( plan )          ;
  SUID puid    = 0                    ;
  bool success = false                ;
  QImage I = image                    ;
  Bustle  ( )                         ;
  success = PM . Import ( I , puid )  ;
  Vacancy ( )                         ;
  if (puid<=0 ) return true           ;
  UUIDs Uuids                         ;
  Uuids << puid                       ;
  return JoinGallery ( uuid , Uuids ) ;
}

bool N::ProjectItems::JoinPeople(SUID uuid,const UUIDs & Uuids)
{
  QListWidgetItem * item  = uuidAt(uuid)           ;
  if (IsNull(item)) return false                   ;
  //////////////////////////////////////////////////
  dropAction = true                                ;
  //////////////////////////////////////////////////
  QString           title = item->text()           ;
  plan -> showMessage(tr("%1 people join group %2" )
    .arg(Uuids.count()).arg(title)               ) ;
  plan -> processEvents (                        ) ;
  //////////////////////////////////////////////////
  Bustle  ( )                                      ;
  SqlConnection Connection ( plan->sql           ) ;
  if (Connection.open("nGroupView","JoinPeople") ) {
    UUIDs Guids = Uuids                            ;
    GroupItems::Join                               (
      Connection,uuid,Type                         ,
      Types::People                      ,
      Groups::Subordination,0,Guids    ) ;
    Connection . close ( )                         ;
  }                                                ;
  Connection . remove ( )                          ;
  Vacancy ( )                                      ;
  //////////////////////////////////////////////////
  plan->showMessage                                (
    tr("%1 people join groups %2 finished"         )
    .arg(Uuids.count()).arg(title)               ) ;
  //////////////////////////////////////////////////
  Alert ( Done )                                   ;
  dropAction = false                               ;
  return true                                      ;
}

bool N::ProjectItems::JoinVideos(SUID uuid,const UUIDs & Uuids)
{
  QListWidgetItem * item  = uuidAt(uuid)           ;
  if (IsNull(item)) return false                   ;
  //////////////////////////////////////////////////
  dropAction = true                                ;
  //////////////////////////////////////////////////
  QString           title = item->text()           ;
  plan -> showMessage(tr("%1 videos join group %2" )
    .arg(Uuids.count()).arg(title)               ) ;
  plan -> processEvents (                        ) ;
  //////////////////////////////////////////////////
  Bustle  ( )                                      ;
  SqlConnection Connection ( plan->sql           ) ;
  if (Connection.open("nGroupView","JoinVideos") ) {
    UUIDs Guids = Uuids                            ;
    GroupItems::Join                               (
      Connection,uuid,Type                         ,
      Types::Video                       ,
      Groups::Subordination,0,Guids    ) ;
    Connection . close ( )                         ;
  }                                                ;
  Connection . remove ( )                          ;
  Vacancy ( )                                      ;
  //////////////////////////////////////////////////
  plan->showMessage                                (
    tr("%1 videos join groups %2 finished"         )
    .arg(Uuids.count()).arg(title)               ) ;
  //////////////////////////////////////////////////
  Alert ( Done )                                   ;
  dropAction = false                               ;
  return true                                      ;
}

bool N::ProjectItems::JoinAlbums(SUID uuid,const UUIDs & Uuids)
{
  QListWidgetItem * item  = uuidAt(uuid)           ;
  if (IsNull(item)) return false                   ;
  //////////////////////////////////////////////////
  dropAction = true                                ;
  //////////////////////////////////////////////////
  QString           title = item->text()           ;
  plan -> showMessage(tr("%1 albums join group %2" )
    .arg(Uuids.count()).arg(title)               ) ;
  plan -> processEvents (                        ) ;
  //////////////////////////////////////////////////
  Bustle  ( )                                      ;
  SqlConnection Connection ( plan->sql           ) ;
  if (Connection.open("nGroupView","JoinAlbums") ) {
    UUIDs Guids = Uuids                            ;
    GroupItems::Join                               (
      Connection,uuid                              ,
      Types::Division                    ,
      Types::Album                       ,
      Groups::Subordination,0,Guids    ) ;
    Connection . close ( )                         ;
  }                                                ;
  Connection . remove ( )                          ;
  Vacancy ( )                                      ;
  //////////////////////////////////////////////////
  plan->showMessage                                (
    tr("%1 albums join groups %2 finished"         )
    .arg(Uuids.count()).arg(title)               ) ;
  //////////////////////////////////////////////////
  Alert ( Done )                                   ;
  dropAction = false                               ;
  return true                                      ;
}

bool N::ProjectItems::JoinGender(SUID uuid,const SUID gender)
{
  return true ;
}

bool N::ProjectItems::JoinGroups(SUID uuid,const UUIDs & Uuids)
{
  return true ;
}

void N::ProjectItems::GroupPositions(RMAPs & Position,UUIDs & Uuids)
{
  int Id = 1               ;
  SUID uuid                ;
  foreach (uuid,Uuids)     {
    Position [ uuid ] = Id ;
    Id ++                  ;
  }                        ;
}

bool N::ProjectItems::MoveGroups(SUID atUuid,const UUIDs & Uuids)
{
  dropAction = true                                     ;
  SUID            uuid                                  ;
  UUIDs           ITEMs = ItemUuids ( )                 ;
  ListWidgetItems mItems                                ;
  foreach (uuid,Uuids)                                  {
    QListWidgetItem * it = uuidAt ( uuid )              ;
    mItems << it                                        ;
    takeItem ( row ( it ) )                             ;
    ITEMs . takeAt ( ITEMs . indexOf (uuid) )           ;
  }                                                     ;
  QListWidgetItem * rit   = uuidAt ( atUuid )           ;
  int               rid   = row    ( rit    )           ;
  for (int i=0;i<mItems.count();i++,rid++)              {
    insertItem ( rid , mItems [ i ] )                   ;
  }                                                     ;
  rid = ITEMs . indexOf ( atUuid )                      ;
  if (rid<0) rid = 0                                    ;
  foreach (uuid,Uuids)                                  {
    ITEMs . insert ( rid , uuid )                       ;
    rid++                                               ;
  }                                                     ;
  RMAPs Position                                        ;
  GroupPositions(Position,ITEMs)                        ;
  ///////////////////////////////////////////////////////
  Bustle ( )                                            ;
  SqlConnection SC(plan->sql)                           ;
  if (SC.open("nGroupView","MoveAlbums"))               {
    QString Q                                           ;
    foreach (uuid,ITEMs)                                {
      int Id = Position [ uuid ]                        ;
      Q = SC.sql.Update                                 (
            plan->Tables[Tables::Divisions]   ,
            QString("where uuid = %1 ").arg(uuid)       ,
            1,"position"                            )   ;
      SC . Prepare ( Q               )                  ;
      SC . Bind    ( "position" , Id )                  ;
      SC . Exec    (                 )                  ;
    }                                                   ;
    SC.close()                                          ;
  }                                                     ;
  SC.remove()                                           ;
  ///////////////////////////////////////////////////////
  Vacancy  ( )                                          ;
  Alert ( Done )                                        ;
  dropAction = false                                    ;
  return true                                           ;
}
