#include <datawidgets.h>

N::SourcesView:: SourcesView ( QWidget * parent , Plan * p )
               : SnapView    (           parent ,        p )
               , dropAction  ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SourcesView::~SourcesView (void)
{
}

void N::SourcesView::Configure(void)
{
  setViewMode                  (IconMode                   ) ;
  setIconSize                  (QSize(128,128)             ) ;
  setGridSize                  (QSize(144,192)             ) ;
  setMovement                  (Snap                       ) ;
  setResizeMode                (Adjust                     ) ;
  setSelectionMode             (ExtendedSelection          ) ;
  setWordWrap                  (true                       ) ;
  setMouseTracking             (true                       ) ;
  setWrapping                  (true                       ) ;
  setTextElideMode             (Qt::ElideNone              ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded      ) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded      ) ;
  setMinimumSize               (QSize(144,200)             ) ;
  setDragDropMode              (DragDrop                   ) ;
  setAccessibleName            ("nSourcesView"             ) ;
  setWindowTitle               ( tr("Source code manager") ) ;
  setWindowIcon                ( QIcon(":/images/vcs.png") ) ;
  ////////////////////////////////////////////////////////////
  setDropFlag                  (DropImage    , true        ) ;
  setDropFlag                  (DropPicture  , true        ) ;
  setDropFlag                  (DropSource   , true        ) ;
  ////////////////////////////////////////////////////////////
  MountCommands                (                           ) ;
  plan -> setFont              (this                       ) ;
}

QMimeData * N::SourcesView::dragMime(void)
{
  if (Items.count()<=0) return NULL               ;
  QMimeData       * mime    = new QMimeData ( )   ;
  QListWidgetItem * current = currentItem   ( )   ;
  if (Items.count()==1)                           {
    SUID        type = nListUuid(Items[0])        ;
    QByteArray  data = CreateByteArray(type)      ;
    mime -> setData ("source/uuid",data)          ;
  } else                                          {
    UUIDs       Uuids = Selected        (       ) ;
    QByteArray  data  = CreateByteArray ( Uuids ) ;
    mime -> setData ("source/uuids",data)         ;
  }                                               ;
  if (NotNull(current))                           {
    QIcon  icon  = current->icon()                ;
    if (!icon.isNull())                           {
      QSize  s     = iconSize()                   ;
      QImage image = icon . pixmap(s) . toImage() ;
      if (!image.isNull())                        {
        mime -> setImageData(image)               ;
      }                                           ;
    }                                             ;
  }                                               ;
  QString tooltip                                 ;
  tooltip = tr("%1 source selected"               )
            .arg(Items.count()                  ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

QString N::SourcesView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType(
           mime,
           "picture/uuid;picture/uuids;"
           "source/uuid;source/uuids"
         );
}

UUIDs N::SourcesView::MimeUuids(const QMimeData * mime,QString mimetype)
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
  if (mimetype=="source/uuid")           {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="source/uuids")          {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::SourcesView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::SourcesView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                         ;
  QString    mtype                                     ;
  UUIDs      Uuids                                     ;
  Alert ( Click )                                      ;
  if (source==this)                                    {
    mtype = MimeType  (mime      )                     ;
    Uuids = MimeUuids (mime,mtype)                     ;
    if (mtype=="source/uuid"  || mtype=="source/uuids" )
    plan->showMessage                                  (
      tr("Start moving %1 sources"                     )
                .arg(Uuids.count()))                   ;
    plan->processEvents()                              ;
  } else                                               {
    mtype = MimeType  (mime      )                     ;
    Uuids = MimeUuids (mime,mtype)                     ;
    if (mtype=="picture/uuid" || mtype=="picture/uuids")
      plan->showMessage                                (
        tr("Copy %1 pictures from %2"                  )
        .arg(Uuids.count()                             )
        .arg(source->windowTitle())                  ) ;
    if (mtype=="source/uuid"  || mtype=="source/uuids" )
      plan->showMessage                                (
        tr("Copy %1 source from %2"                    )
        .arg(Uuids.count()                             )
        .arg(source->windowTitle())                  ) ;
  }                                                    ;
  return true                                          ;
}

bool N::SourcesView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false                                ;
  if (source==this)                                           {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::SourcesView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

bool N::SourcesView::startup(void)
{
  ScopedProgress Progress(plan,tr("%v/%m sources"),50)          ;
  clear      (       )                                          ;
  DoProcessEvents                                               ;
  setEnabled ( false )                                          ;
  PictureManager PM ( plan )                                    ;
  EnterSQL(SC,plan->sql)                                        ;
    QString Q                                                   ;
    UUIDs   Uuids                                               ;
    SUID    uuid                                                ;
    Uuids = SC.Uuids(PlanTable(Sources),"uuid","order by id desc") ;
    Progress . setRange ( 0 , Uuids.count() )                   ;
    Progress . show     (                   )                   ;
    Progress . Start    (                   )                   ;
    foreach (uuid,Uuids)                                        {
      ++Progress                                                ;
      QIcon             icon = QIcon(":/images/vcs.png")        ;
      QString           name = ItemName(SC,uuid)                ;
      SUID              puid = 0                                ;
      QListWidgetItem * item = NULL                             ;
      Q = SC.sql.SelectFrom                                     (
            "second",PlanTable(Groups)                             ,
            QString("where first = %1 "
                    "and t1 = %2 "
                    "and t2 = %3 "
                    "and relation = %4"                         )
                  .arg(uuid                                     )
                  .arg(Types::Source                  )
                  .arg(Types::Picture                 )
                  .arg(Groups::Icon               ) ) ;
      if (SC.Fetch(Q)) puid = SC . Uuid ( 0 )                   ;
      if (puid>0)                                               {
        QImage * image = PM . Thumb ( puid )                    ;
        if (NotNull(image))                                     {
          icon = PM.Icon(image,QSize(128,128))                  ;
          delete image                                          ;
        }                                                       ;
      }                                                         ;
      item = addItem ( uuid , name , icon )                     ;
    }                                                           ;
    Progress . Finish ( )                                       ;
  LeaveSQL(SC,plan->sql)                                        ;
  setEnabled ( true  )                                          ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded )         ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded )         ;
  setToolTip(tr("%1 sources").arg(count()))                     ;
  Alert ( Done )                                                ;
  return true                                                   ;
}

bool N::SourcesView::FocusIn(void)
{
  connectAction(Menus::Insert    ,this,SLOT(New            ())) ;
  connectAction(Menus::Refresh   ,this,SLOT(startup        ())) ;
//  connectAction(Menus::Rename    ,this,SLOT(Rename         ())) ;
  connectAction(Menus::Copy      ,this,SLOT(CopyToClipboard())) ;
  connectAction(Menus::SelectNone,this,SLOT(SelectNone     ())) ;
  connectAction(Menus::SelectAll ,this,SLOT(SelectAll      ())) ;
  connectAction(Menus::Delete    ,this,SLOT(Delete         ())) ;
  return true                                                             ;
}

void N::SourcesView::New(void)
{
  NewListWidgetItem (LWI)                               ;
  ///////////////////////////////////////////////////////
  LWI->setIcon(QIcon(":/images/vcs.png"))               ;
  LWI->setData(Qt::UserRole,0)                          ;
  insertItem     ( 0 , LWI )                            ;
  scrollToItem   ( LWI     )                            ;
  setCurrentItem ( LWI     )                            ;
  ItemEditing    = LWI                                  ;
  ///////////////////////////////////////////////////////
  QRect       R = visualItemRect(LWI)                   ;
  QLineEdit * l = new QLineEdit(this)                   ;
  QFont       f = font()                                ;
  QRect L (R.left(),R.bottom(),R.width(),f.pixelSize()) ;
  setItemWidget (ItemEditing,l)                         ;
  l->setGeometry(L)                                     ;
  ItemWidget = l                                        ;
  ///////////////////////////////////////////////////////
  connect(l   ,SIGNAL(editingFinished())                ,
          this,SLOT  (editingFinished())              ) ;
  ///////////////////////////////////////////////////////
  l->setFocus(Qt::TabFocusReason)                       ;
  Alert ( Action )                                      ;
}

void N::SourcesView::editingFinished(void)
{
  QLineEdit * l = Casting ( QLineEdit , ItemWidget )    ;
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
  ///////////////////////////////////////////////////////
  ItemEditing->setText(name)                            ;
  EnterSQL(SC,plan->sql)                                ;
    SUID uuid = SC.Unique                               (
              PlanTable(MajorUuid)                      ,
              "uuid"                                  ) ;
    QString Q                                           ;
    SC.assureUuid                                       (
              PlanTable(MajorUuid)                      ,
              uuid,Types::Source                      ) ;
    SC.insertUuid                                       (
              PlanTable(Sources)                        ,
              uuid,"uuid"                             ) ;
    Q = SC.sql.InsertInto                               (
              PlanTable(Names)                          ,
              3,"uuid","language","name"              ) ;
    SC.insertName(Q,uuid,vLanguageId,name)              ;
    ItemEditing->setData(Qt::UserRole,uuid)             ;
  LeaveSQL(SC,plan->sql)                                ;
  ///////////////////////////////////////////////////////
  setToolTip(tr("%1 sources").arg(count()))             ;
  Alert ( Done )                                        ;
}

void N::SourcesView::Refresh(QListWidgetItem * item)
{
  if (IsNull(item)) return                                    ;
  SUID uuid = nListUuid(item)                                 ;
  PictureManager PM ( plan )                                 ;
  EnterSQL ( SC , plan->sql )                                 ;
    QString Q                                                 ;
    QIcon             icon = QIcon(":/images/vcs.png")        ;
    QString           name = ItemName(SC,uuid)                ;
    SUID              puid = 0                                ;
    Q = SC.sql.SelectFrom                                     (
          "second",PlanTable(Groups)                             ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4"                         )
                .arg(uuid                                     )
                .arg(Types::Source                  )
                .arg(Types::Picture                 )
                .arg(Groups::Icon               ) ) ;
    if (SC.Fetch(Q)) puid = SC . Uuid ( 0 )                   ;
    if (puid>0)                                               {
      QImage * image = PM . Thumb ( puid )                    ;
      if (NotNull(image))                                     {
        icon = PM.Icon(image,QSize(128,128))                  ;
        delete image                                          ;
      }                                                       ;
    }                                                         ;
    item -> setIcon ( icon )                                  ;
    item -> setText ( name )                                  ;
  LeaveSQL ( SC , plan->sql )                                 ;
}

void N::SourcesView::Rename(QListWidgetItem * item)
{
  if (IsNull(item)) return                              ;
  ///////////////////////////////////////////////////////
  setCurrentItem(item)                                  ;
  ItemEditing = item                                    ;
  QRect       R = visualItemRect(item)                  ;
  QLineEdit * l = new QLineEdit(this)                   ;
  QFont       f = font()                                ;
  QRect L (R.left(),R.bottom(),R.width(),f.pixelSize()) ;
  setItemWidget(ItemEditing,l)                          ;
  l->setGeometry(L)                                     ;
  ItemWidget = l                                        ;
  l->setText(item->text())                              ;
  ///////////////////////////////////////////////////////
  connect(l   ,SIGNAL(editingFinished())                ,
          this,SLOT  (renameFinished ())              ) ;
  l->setFocus ( Qt::TabFocusReason )                    ;
  Alert ( Action )                                      ;
}

void N::SourcesView::renameFinished(void)
{
  QLineEdit * l = Casting(QLineEdit,ItemWidget)         ;
  if (IsNull(l)) return                                 ;
  ///////////////////////////////////////////////////////
  QString name = l->text()                              ;
  SUID    uuid = nListUuid(ItemEditing)                 ;
  removeItemWidget(ItemEditing)                         ;
  if (name.length()<=0)                                 {
    ItemEditing = NULL                                  ;
    ItemWidget  = NULL                                  ;
    return                                              ;
  }                                                     ;
  ItemEditing->setText(name)                            ;
  if (uuid<=0) return                                   ;
  ///////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                           ;
    QString Q                                           ;
    Q = SC . sql . Update ( PlanTable(Names)               ,
          "where uuid = :UUID and language = :LANGUAGE" ,
          1,"name"                                    ) ;
    SC.insertName(Q,uuid,vLanguageId,name)         ;
  LeaveSQL ( SC , plan->sql )                           ;
  ///////////////////////////////////////////////////////
  ItemEditing = NULL                                    ;
  ItemWidget  = NULL                                    ;
  Alert ( Done )                                        ;
}

void N::SourcesView::Remove(SUID uuid)
{
  UUIDs    Uuids ;
  Uuids << uuid  ;
  Remove(Uuids)  ;
}

void N::SourcesView::Delete(void)
{
  UUIDs Uuids = Selected ()    ;
  if (Uuids.count()<=0) return ;
  Remove ( Uuids )             ;
}

void N::SourcesView::Remove(UUIDs & Uuids)
{
  Alert ( Error ) ;
}

bool N::SourcesView::dropImage(QWidget * source,QPointF psf,const QImage & image)
{
  QPoint pos = psf.toPoint()                   ;
  QListWidgetItem * atItem = itemAt(pos)       ;
  if (IsNull(atItem)) return true              ;
  dropAction = true                            ;
  SUID uuid = nListUuid(atItem)                ;
  if (uuid>0)                                  {
    setGroupIcon(uuid,image)                   ;
    plan->Talk                                 (
      tr("Update %1 representation icon"       )
      .arg(atItem->text())                   ) ;
  }                                            ;
  Refresh          ( atItem                  ) ;
  dropAction = false                           ;
  Alert ( Done )                               ;
  return true                                  ;
}

bool N::SourcesView::setGroupIcon(SUID uuid,const QImage & image)
{
  PictureManager PM ( plan )                     ;
  SUID puid    = 0                                ;
  bool success = false                            ;
  QImage I = image                                ;
  Bustle  ( )                                     ;
  success = PM . Import ( I , puid )              ;
  Vacancy ( )                                     ;
  if (puid<=0 ) return true                       ;
  EnterSQL ( SC , plan->sql )                     ;
    QString Q                                     ;
    Q = SC.sql.DeleteFrom                         (
          PlanTable(Groups)                       ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4"             )
          .arg(uuid                               )
          .arg(Types::Source            )
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
    SC.Bind ("t1"      ,Types::Source ) ;
    SC.Bind ("t2"      ,Types::Picture) ;
    SC.Bind ("relation",Groups::Icon  ) ;
    SC.Exec (                                   ) ;
  LeaveSQL ( SC , plan->sql )                     ;
  return true                                     ;
}

bool N::SourcesView::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()                                 ;
  QListWidgetItem * atItem = itemAt(pos)                     ;
  if (IsNull(atItem)) return true                            ;
  dropAction = true                                          ;
  SUID uuid = nListUuid(atItem)                              ;
  EnterSQL(SC,plan->sql)                                     ;
    if (uuid>0 && Uuids.count()>0)                           {
      SUID    puid = Uuids [ 0 ]                             ;
      QString Q                                              ;
      Q = SC.sql.DeleteFrom(PlanTable(Groups)                   ,
            QString("where first = %1 and t1 = %2 "
                    "and t2 = %3 and relation = %4"          )
            .arg(uuid                                        )
            .arg(Types::Source                     )
            .arg(Types::Picture                    )
            .arg(Groups::Icon    ) )               ;
      SC . Query ( Q )                                       ;
      Q = SC.sql.InsertInto(PlanTable(Groups)                   ,
            5                                                ,
            "first"                                          ,
            "second"                                         ,
            "t1"                                             ,
            "t2"                                             ,
            "relation"                                     ) ;
      SC . Prepare ( Q )                                     ;
      SC . Bind    ( "first"    , uuid                     ) ;
      SC . Bind    ( "second"   , puid                     ) ;
      SC . Bind    ( "t1"       , Types::Source  ) ;
      SC . Bind    ( "t2"       , Types::Picture ) ;
      SC . Bind    ( "relation" , Groups::Icon   ) ;
      SC . Exec    (   )                                     ;
    }                                                        ;
  LeaveSQL(SC,plan->sql)                                     ;
  Refresh          ( atItem                  )               ;
  dropAction = false                                         ;
  if (uuid>0 && Uuids.count()>0)                             {
    plan->Talk                                               (
      tr("Update %1 representation icon"                     )
      .arg(atItem->text())                                 ) ;
  }                                                          ;
  Alert ( Done )                                             ;
  return true                                                ;
}

bool N::SourcesView::dropSources(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source!=this) return false               ;
  QPoint pos = psf.toPoint()                   ;
  QListWidgetItem * atItem = itemAt(pos)       ;
  if (IsNull(atItem)) return true              ;
  dropAction = true                            ;
  SUID            atUuid = nListUuid(atItem)   ;
  SUID            uuid                         ;
  UUIDs           ITEMs = ItemUuids ( )        ;
  ListWidgetItems mItems                       ;
  foreach (uuid,Uuids)                         {
    QListWidgetItem * it = uuidAt ( uuid )     ;
    mItems << it                               ;
    takeItem ( row ( it ) )                    ;
    ITEMs . takeAt ( ITEMs . indexOf (uuid) )  ;
  }                                            ;
  QListWidgetItem * rit   = uuidAt ( atUuid )  ;
  int               rid   = row    ( rit    )  ;
  for (int i=0;i<mItems.count();i++,rid++)     {
    insertItem ( rid , mItems [ i ] )          ;
  }                                            ;
  rid = ITEMs . indexOf ( atUuid )             ;
  if (rid<0) rid = 0                           ;
  foreach (uuid,Uuids)                         {
    ITEMs . insert ( rid , uuid )              ;
    rid++                                      ;
  }                                            ;
  RMAPs Position                               ;
  int   Id = ITEMs . count()                   ;
  foreach (uuid,ITEMs)                         {
    Position [ uuid ] = Id                     ;
    Id --                                      ;
  }                                            ;
  //////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                  ;
    QString Q                                  ;
    foreach (uuid,ITEMs)                       {
      int Id = Position [ uuid ]               ;
      Id    += ( ITEMs . count() * 2 )         ;
      Q = SC.sql.Update(PlanTable(Sources)        ,
            "where uuid = :UUID",1,"id")       ;
      SC . Prepare ( Q             )           ;
      SC . Bind    ( "id"   , Id   )           ;
      SC . Bind    ( "uuid" , uuid )           ;
      SC . Exec    (               )           ;
    }                                          ;
    foreach (uuid,ITEMs)                       {
      int Id = Position [ uuid ]               ;
      Q = SC.sql.Update(PlanTable(Sources)        ,
            "where uuid = :UUID",1,"id"      ) ;
      SC . Prepare ( Q             )           ;
      SC . Bind    ( "id"   , Id   )           ;
      SC . Bind    ( "uuid" , uuid )           ;
      SC . Exec    (               )           ;
    }                                          ;
  LeaveSQL ( SC , plan->sql )                  ;
  //////////////////////////////////////////////
  Alert ( Done )                               ;
  dropAction = false                           ;
  return true                                  ;
}

bool N::SourcesView::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                       ;
  QListWidgetItem * atItem = itemAt  ( pos                      ) ;
  QMdiSubWindow   * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget     * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa = NULL                                          ;
  mm.add(102,tr("Refresh"))                                       ;
  if (IsNull(atItem))                                             {
  } else                                                          {
    mm.add(111,tr("Rename"))                                      ;
  }                                                               ;
  if (NotNull(dock) || NotNull(mdi)) mm.addSeparator()            ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))    ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))    ;
  mm . setFont ( plan )                                           ;
  aa = mm.exec (      )                                           ;
  if (IsNull(aa)) return true                                     ;
  switch (mm[aa])                                                 {
    case 102                                                      :
      startup (        )                                          ;
    break                                                         ;
    case 111                                                      :
      Rename  ( atItem )                                          ;
    break                                                         ;
    case 1000001                                                  :
      emit attachMdi ( this , 0 )                                 ;
    break                                                         ;
    case 1000002                                                  :
      emit attachDock                                             (
        this                                                      ,
        windowTitle()                                             ,
        Qt::RightDockWidgetArea                                   ,
        Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea          |
        Qt::TopDockWidgetArea  | Qt::BottomDockWidgetArea       ) ;
    break                                                         ;
    default                                                       :
    break                                                         ;
  }                                                               ;
  return true                                                     ;
}

void N::SourcesView::run(void)
{
}
