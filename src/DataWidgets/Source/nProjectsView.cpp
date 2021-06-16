#include <datawidgets.h>

#pragma message("ProjectsView need to migrate all functions into thread")

N::ProjectsView:: ProjectsView ( QWidget * parent , Plan * p )
                : SnapView     (           parent ,        p )
                , Object       ( 0 , Project::MAJORPROJECT   )
                , dropAction   ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ProjectsView::~ProjectsView (void)
{
}

void N::ProjectsView::Configure(void)
{
  setViewMode                  (IconMode                    ) ;
  setIconSize                  (QSize(128,128)              ) ;
  setGridSize                  (QSize(144,192)              ) ;
  setMovement                  (Snap                        ) ;
  setResizeMode                (Adjust                      ) ;
  setSelectionMode             (ExtendedSelection           ) ;
  setWordWrap                  (true                        ) ;
  setMouseTracking             (true                        ) ;
  setWrapping                  (true                        ) ;
  setTextElideMode             (Qt::ElideNone               ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAsNeeded       ) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAsNeeded       ) ;
  setMinimumSize               (QSize(144,200)              ) ;
  setDragDropMode              (DragDrop                    ) ;
  setAccessibleName            ("nProjectsView"             ) ;
  setWindowTitle               ( tr("Projects")             ) ;
  setWindowIcon                ( QIcon(":/images/plan.png") ) ;
  /////////////////////////////////////////////////////////////
  setDropFlag                  (DropImage    , true         ) ;
  setDropFlag                  (DropPicture  , true         ) ;
  /////////////////////////////////////////////////////////////
  #define PICON(PROJECT,ICON)                                 \
    LocalIcons [ QString::number( Project::PROJECT ) ] = QIcon(ICON)
  PICON ( MAJORPROJECT , ":/images/project.png"       )       ;
  PICON ( SUBPROJECT   , ":/images/plan.png"          )       ;
  PICON ( DOCUMENTS    , ":/images/documents.png"     )       ;
  PICON ( PLANNER      , ":/images/design.png"        )       ;
  PICON ( SCHEDULER    , ":/images/ExactHour.png"     )       ;
  PICON ( TASK         , ":/images/task.png"          )       ;
  PICON ( FINANCE      , ":/images/finance.png"       )       ;
  PICON ( TEST         , ":/images/test.png"          )       ;
  PICON ( NETWORK      , ":/images/network.png"       )       ;
  PICON ( SQL          , ":/images/SQL.png"           )       ;
  PICON ( FLOW         , ":/images/flow.png"          )       ;
  PICON ( DATAMODEL    , ":/images/datamodel.png"     )       ;
  PICON ( CLASSMODEL   , ":/images/classmodel.png"    )       ;
  PICON ( QTPROJECT    , ":/images/QT.png"            )       ;
  PICON ( INSTALLER    , ":/images/installer.png"     )       ;
  PICON ( SOURCES      , ":/images/sources.png"       )       ;
  PICON ( PICTURES     , ":/images/pictures.png"      )       ;
  PICON ( GUI          , ":/images/GUI.png"           )       ;
  PICON ( KNOWLEDGE    , ":/images/knowledgebase.png" )       ;
  PICON ( WEBSITE      , ":/images/web.png"           )       ;
  PICON ( BOOKMARKS    , ":/images/bookmarks.png"     )       ;
  PICON ( SKETCH       , ":/images/sketch.png"        )       ;
  PICON ( DATASHEET    , ":/images/checklist.png"     )       ;
  #undef  PICON
  /////////////////////////////////////////////////////////////
  MountCommands                (                      )       ;
  plan -> setFont              ( this                 )       ;
}

QMimeData * N::ProjectsView::dragMime(void)
{
  if (Items.count()<=0) return NULL               ;
  QMimeData       * mime    = new QMimeData ( )   ;
  QListWidgetItem * current = currentItem   ( )   ;
  if (Items.count()==1)                           {
    SUID        type = nListUuid(Items[0])        ;
    QByteArray  data = CreateByteArray(type)      ;
    mime -> setData ("project/uuid",data)         ;
  } else                                          {
    UUIDs       Uuids = Selected        (       ) ;
    QByteArray  data  = CreateByteArray ( Uuids ) ;
    mime -> setData ("project/uuids",data)        ;
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
  tooltip = tr("%1 project selected"              )
            .arg(Items.count()                  ) ;
  QToolTip :: showText ( QCursor::pos(),tooltip ) ;
  return mime                                     ;
}

QString N::ProjectsView::MimeType(const QMimeData * mime)
{
  return AbstractGui::MimeType (
           mime,
           "album/uuid;album/uuids;"
           "video/uuid;video/uuids;"
           "picture/uuid;picture/uuids;"
           "people/uuid;people/uuids"
         );
}

UUIDs N::ProjectsView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="album/uuid")            {
    Uuids << GetUuid  ( data )           ;
  } else
  if (mimetype=="album/uuids")           {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="video/uuid")            {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="video/uuids")           {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="picture/uuid")          {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="picture/uuids")         {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="people/uuid")           {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="people/uuids")          {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

bool N::ProjectsView::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::ProjectsView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false               ;
  QString    mtype                           ;
  UUIDs      Uuids                           ;
  Alert ( Click )                            ;
  if (source==this)                          {
    mtype = MimeType  (mime      )           ;
    Uuids = MimeUuids (mime,mtype)           ;
    plan->showMessage                        (
      tr("Start moving %1 album"             )
                .arg(Uuids.count()))         ;
    DoProcessEvents                          ;
  } else                                     {
    if (mime->hasUrls())                     {
    } else                                   {
      mtype = MimeType  (mime      )         ;
      Uuids = MimeUuids (mime,mtype)         ;
      if (mtype=="album/uuid"   || mtype=="album/uuids")
      plan->showMessage                      (
        tr("Copy %1 album from %2"        )
        .arg(Uuids.count()                   )
        .arg(source->windowTitle())        ) ;
      if (mtype=="picture/uuid" || mtype=="picture/uuids")
      plan->showMessage                      (
        tr("Copy %1 pictures from %2"        )
        .arg(Uuids.count()                   )
        .arg(source->windowTitle())        ) ;
      if (mtype=="people/uuid"  || mtype=="people/uuids")
      plan->showMessage                      (
        tr("Copy %1 people from %2"        )
        .arg(Uuids.count()                   )
        .arg(source->windowTitle())        ) ;
      if (mtype=="video/uuid"   || mtype=="video/uuids")
      plan->showMessage                      (
        tr("Copy %1 video from %2"        )
        .arg(Uuids.count()                   )
        .arg(source->windowTitle())        ) ;
      plan->processEvents()                  ;
    }                                        ;
  }                                          ;
  return true                                ;
}

bool N::ProjectsView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if ( dropAction     ) return false                          ;
  if ( source == this )                                       {
    QListWidgetItem * atItem = itemAt(pos)                    ;
    if (IsNull (atItem)) return false                         ;
    if (NotNull(atItem) && atItem->isSelected()) return false ;
  }                                                           ;
  return true                                                 ;
}

bool N::ProjectsView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if (dropAction) return false             ;
  return dropItems ( source , mime , pos ) ;
}

void N::ProjectsView::run(int T,ThreadData * d)
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

bool N::ProjectsView::Load(ThreadData * d)
{
  nKickOut            ( ! IsContinue ( d ) , false                         ) ;
  WaitClear           (                                                    ) ;
  emit assignEnabling ( false                                              ) ;
  PictureManager PM   ( plan                                               ) ;
  EnterSQL            ( SC , plan -> sql                                   ) ;
    QString Q                                                                ;
    UUIDs   U                                                                ;
    SUID    u                                                                ;
    //////////////////////////////////////////////////////////////////////////
    if ( isStandby ( ) )                                                     {
      U = SC . Uuids                                                         (
            PlanTable(Projects)                                              ,
            "uuid"                                                           ,
            QString("where type = 0 %1").arg ( SC . OrderByAsc ( "id" ) )  ) ;
      emit assignTitle ( tr("Major projects") )                              ;
    } else                                                                   {
      QString name                                                           ;
      U = SC . Uuids                                                         (
            PlanTable(Groups)                                                ,
            "second"                                                         ,
            QString ( "where first = %1 and t1 = %2 "
                      "and t2 = %3 and relation = %4 "
                      "order by position asc"                                )
        . arg ( ObjectUuid ( )                                               )
        . arg ( Types::Project                                               )
        . arg ( Types::Project                                               )
        . arg ( Groups::Subordination                                    ) ) ;
      name = ItemName  ( SC , ObjectUuid ( ) )                               ;
      emit assignTitle ( name                )                               ;
      Q = SC . sql . SelectFrom                                              (
            "type"                                                           ,
            PlanTable(Projects)                                              ,
            SC . WhereUuid ( ObjectUuid ( ) )                              ) ;
      if ( SC . Fetch ( Q ) ) setObjectType (  SC . Int ( 0 )              ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    foreach ( u , U )                                                        {
      int type = -1                                                          ;
      Q = SC . sql . SelectFrom                                              (
            "type"                                                           ,
            PlanTable(Projects)                                              ,
            SC . WhereUuid ( u )                                           ) ;
      if ( SC . Fetch ( Q ) ) type = SC . Int ( 0 )                          ;
      if ( type >= 0 )                                                       {
        QString           name                                               ;
        QIcon             icon = LocalIcons [ QString::number(type) ]        ;
        SUID              puid = 0                                           ;
        QListWidgetItem * item = NULL                                        ;
        name = ItemName ( SC , u )                                           ;
        Q = SC . sql . SelectFrom                                            (
              "second"                                                       ,
              PlanTable(Groups)                                              ,
              QString("where first = %1 "
                      "and t1 = %2 "
                      "and t2 = %3 "
                      "and relation = %4"                                    )
                    . arg ( u                                                )
                    . arg ( Types::Project                                   )
                    . arg ( Types::Picture                                   )
                    . arg ( Groups::Icon                                 ) ) ;
        if ( SC . Fetch ( Q ) ) puid = SC . Uuid ( 0 )                       ;
        if ( puid > 0 )                                                      {
          QImage * image = PM . Thumb ( puid )                               ;
          if ( NotNull ( image ) )                                           {
            icon = PM . Icon ( image , QSize ( 128 , 128 ) )                 ;
            delete image                                                     ;
          }                                                                  ;
        }                                                                    ;
        item  = addItem ( uuid , name , icon      )                          ;
        item -> setData ( Qt::UserRole + 1 , type )                          ;
      }                                                                      ;
    }                                                                        ;
  LeaveSQL(SC,plan->sql)                                                     ;
  ////////////////////////////////////////////////////////////////////////////
  QString m = tr("%1 projects") . arg ( count ( ) )                          ;
  emit assignEnabling ( true  )                                              ;
  emit assignToolTip  ( m     )                                              ;
  Alert               ( Done  )                                              ;
  return true                                                                ;
}

bool N::ProjectsView::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

bool N::ProjectsView::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , New             ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Rename       , Rename          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  return true                                              ;
}

void N::ProjectsView::New(void)
{
  int pt = Project::MAJORPROJECT                                        ;
  if   ( ObjectUuid ( ) > 0                      )                      {
    if ( ObjectType ( ) == Project::MAJORPROJECT )                      {
      pt = Project::SUBPROJECT                                          ;
    } else
    if ( ObjectType ( ) == Project::SUBPROJECT   )                      {
      pt = Project::SUBPROJECT                                          ;
    } else                                                              {
      return                                                            ;
    }                                                                   ;
  }                                                                     ;
  ///////////////////////////////////////////////////////////////////////
  QListWidgetItem * LWI                                                 ;
  LWI  = addItem ( 0 , "" , LocalIcons [ QString::number(pt) ] )        ;
  LWI -> setData ( Qt::UserRole   , 0  )                                ;
  LWI -> setData ( Qt::UserRole+1 , pt )                                ;
  scrollToItem   ( LWI )                                                ;
  setCurrentItem ( LWI )                                                ;
  ItemEditing =    LWI                                                  ;
  QRect       R = visualItemRect ( LWI  )                               ;
  QLineEdit * l = new QLineEdit  ( this )                               ;
  QFont       f = font ( )                                              ;
  QRect L ( R.left ( ) , R.bottom ( ) , R.width ( ) , f.pixelSize ( ) ) ;
  setItemWidget ( ItemEditing , l )                                     ;
  l->setGeometry(L)                                                     ;
  ItemWidget = l                                                        ;
  nConnect ( l    , SIGNAL ( editingFinished ( ) )                      ,
             this , SLOT   ( editingFinished ( ) )                    ) ;
  l -> setFocus ( Qt::TabFocusReason )                                  ;
}

void N::ProjectsView::Delete(void)
{
  UUIDs U = Selected ( )          ;
  nDropOut ( U . count ( ) <= 0 ) ;
  Remove ( U )                    ;
}

void N::ProjectsView::Rename(void)
{
  QListWidgetItem * it = currentItem ( ) ;
  if ( IsNull ( it ) ) return            ;
  Rename ( it )                          ;
}

void N::ProjectsView::editingFinished(void)
{
  QLineEdit * l = Casting ( QLineEdit , ItemWidget )       ;
  if (IsNull(l)) return                                    ;
  //////////////////////////////////////////////////////////
  QString name = l->text()                                 ;
  int     type = ItemEditing->data(Qt::UserRole+1).toInt() ;
  removeItemWidget(ItemEditing)                            ;
  if (name.length()<=0)                                    {
    takeItem(row(ItemEditing))                             ;
    ItemEditing = NULL                                     ;
    ItemWidget  = NULL                                     ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  ItemEditing->setText(name)                               ;
  EnterSQL(SC,plan->sql)                                   ;
    QString Q                                              ;
    SUID uuid = SC.Unique                                  (
              PlanTable(MajorUuid)                         ,
              "uuid"                                     ) ;
    SC.assureUuid                                          (
              PlanTable(MajorUuid)                         ,
              uuid,Types::Project                        ) ;
    SC.assureUuid                                          (
              PlanTable(Projects )                         ,
              uuid,type                                  ) ;
    Q = SC.sql.InsertInto                                  (
          PlanTable(Names)                                 ,
          3                                                ,
          "uuid"                                           ,
          "language"                                       ,
          "name"                                         ) ;
    SC . insertName ( Q,uuid,plan->LanguageId,name )       ;
    if ( ObjectUuid ( ) > 0 )                              {
      UUIDs Juids                                          ;
      Juids << uuid                                        ;
      Join                                                 (
        SC                                                 ,
        ObjectUuid ( )                                     ,
        Types ::Project                                    ,
        Types ::Project                                    ,
        Groups::Subordination                              ,
        0                                                  ,
        Juids                                            ) ;
    }                                                      ;
    ItemEditing->setData(Qt::UserRole,uuid)                ;
  LeaveSQL(SC,plan->sql)                                   ;
  //////////////////////////////////////////////////////////
  setToolTip(tr("%1 projects").arg(count()))               ;
  Alert ( Done )                                           ;
}

void N::ProjectsView::Refresh(QListWidgetItem * item)
{
  if (IsNull(item)) return                                        ;
  SUID uuid = nListUuid(item)                                     ;
  PictureManager PM ( plan )                                      ;
  EnterSQL ( SC , plan->sql )                                     ;
    QString Q                                                     ;
    int     type = -1                                             ;
    Q = SC.sql.SelectFrom("type",PlanTable(Projects)              ,
          SC.WhereUuid(uuid)                                    ) ;
    if (SC.Fetch(Q)) type = SC.Int(0)                             ;
    QIcon             icon = LocalIcons [ QString::number(type) ] ;
    QString           name = ItemName(SC,uuid)                    ;
    SUID              puid = 0                                    ;
    Q = SC.sql.SelectFrom                                         (
          "second",PlanTable(Groups)                              ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4"                             )
                .arg(uuid                                         )
                .arg(Types::Project                               )
                .arg(Types::Picture                               )
                .arg(Groups::Icon                             ) ) ;
    if (SC.Fetch(Q)) puid = SC . Uuid ( 0 )                       ;
    if (puid>0)                                                   {
      QImage * image = PM . Thumb ( puid )                        ;
      if (NotNull(image))                                         {
        icon = PM.Icon(image,QSize(128,128))                      ;
        delete image                                              ;
      }                                                           ;
    }                                                             ;
    item -> setIcon ( icon )                                      ;
    item -> setText ( name )                                      ;
  LeaveSQL ( SC , plan->sql )                                     ;
}

void N::ProjectsView::Rename(QListWidgetItem * item)
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

void N::ProjectsView::renameFinished(void)
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
  DocumentManager DM ( plan )                           ;
  EnterSQL ( SC , plan->sql )                           ;
    DM.assureName(SC,uuid,plan->LanguageId,name)        ;
  LeaveSQL ( SC , plan->sql )                           ;
  ///////////////////////////////////////////////////////
  ItemEditing = NULL                                    ;
  ItemWidget  = NULL                                    ;
  Alert ( Done )                                        ;
}

void N::ProjectsView::Remove(SUID uuid)
{
  UUIDs    Uuids ;
  Uuids << uuid  ;
  Remove(Uuids)  ;
}

void N::ProjectsView::Remove(UUIDs & Uuids)
{
  Alert ( Done ) ;
}

void N::ProjectsView::New(int Type)
{
  assignType = Type                                                      ;
  QListWidgetItem * LWI                                                  ;
  LWI  = addItem ( 0 , "" , LocalIcons [ QString::number(assignType) ] ) ;
  LWI -> setData ( Qt::UserRole   , 0          )                         ;
  LWI -> setData ( Qt::UserRole+1 , assignType )                         ;
  scrollToItem   ( LWI )                                                 ;
  setCurrentItem ( LWI )                                                 ;
  ItemEditing =    LWI                                                   ;
  QRect       R = visualItemRect(LWI)                                    ;
  QLineEdit * l = new QLineEdit(this)                                    ;
  QFont       f = font()                                                 ;
  QRect L (R.left(),R.bottom(),R.width(),f.pixelSize())                  ;
  setItemWidget(ItemEditing,l)                                           ;
  l->setGeometry(L)                                                      ;
  ItemWidget = l                                                         ;
  connect(l   ,SIGNAL(editingFinished())                                 ,
          this,SLOT  (itemFinished   ())                               ) ;
  l->setFocus(Qt::TabFocusReason)                                        ;
}

void N::ProjectsView::itemFinished(void)
{
  QLineEdit * l = Casting ( QLineEdit , ItemWidget )       ;
  nDropOut ( IsNull(l) )                                   ;
  //////////////////////////////////////////////////////////
  QString name = l->text()                                 ;
  int     type = ItemEditing->data(Qt::UserRole+1).toInt() ;
  removeItemWidget(ItemEditing)                            ;
  if (name.length()<=0)                                    {
    takeItem(row(ItemEditing))                             ;
    ItemEditing = NULL                                     ;
    ItemWidget  = NULL                                     ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  ItemEditing->setText(name)                               ;
  EnterSQL(SC,plan->sql)                                   ;
    QString Q                                              ;
    SUID uuid = SC.Unique                                  (
              PlanTable(MajorUuid)                         ,
              "uuid"                                     ) ;
    SC.assureUuid                                          (
              PlanTable(MajorUuid)                         ,
              uuid,objectType                            ) ;
    SC.assureUuid                                          (
              PlanTable(Projects )                         ,
              uuid,type                                  ) ;
    Q = SC.sql.InsertInto                                  (
              PlanTable(Names    )                         ,
              3,"uuid","language","name"                 ) ;
    SC.insertName(Q,uuid,plan->LanguageId,name)            ;
    if (ObjectUuid ( )>0)                                  {
      UUIDs Juids                                          ;
      Juids << uuid                                        ;
      Join                                                 (
        SC                                                 ,
        ObjectUuid ( )                                     ,
        Types ::Project                                    ,
        objectType                                         ,
        Groups::Subordination                              ,
        0                                                  ,
        Juids                                            ) ;
    }                                                      ;
    ItemEditing->setData(Qt::UserRole,uuid)                ;
  LeaveSQL(SC,plan->sql)                                   ;
  //////////////////////////////////////////////////////////
  setToolTip(tr("%1 projects").arg(count()))               ;
  Alert ( Done )                                           ;
}

void N::ProjectsView::SqlPlan(void)
{
  int pt = Project::SQL                                       ;
  QListWidgetItem * LWI                                       ;
  LWI  = addItem ( 0 , "" , LocalIcons[QString::number(pt)] ) ;
  LWI -> setData ( Qt::UserRole   , 0  )                      ;
  LWI -> setData ( Qt::UserRole+1 , pt )                      ;
  scrollToItem   ( LWI )                                      ;
  setCurrentItem ( LWI )                                      ;
  ItemEditing =    LWI                                        ;
  QRect       R = visualItemRect(LWI)                         ;
  QLineEdit * l = new QLineEdit(this)                         ;
  QFont       f = font()                                      ;
  QRect L (R.left(),R.bottom(),R.width(),f.pixelSize())       ;
  setItemWidget(ItemEditing,l)                                ;
  l->setGeometry(L)                                           ;
  ItemWidget = l                                              ;
  connect(l   ,SIGNAL(editingFinished())                      ,
          this,SLOT  (sqlFinished())                        ) ;
  l->setFocus(Qt::TabFocusReason)                             ;
}

void N::ProjectsView::sqlFinished(void)
{
  QLineEdit * l = Casting ( QLineEdit , ItemWidget )       ;
  nDropOut ( IsNull(l) )                                   ;
  //////////////////////////////////////////////////////////
  QString name = l->text()                                 ;
  int     type = ItemEditing->data(Qt::UserRole+1).toInt() ;
  removeItemWidget(ItemEditing)                            ;
  if (name.length()<=0)                                    {
    takeItem(row(ItemEditing))                             ;
    ItemEditing = NULL                                     ;
    ItemWidget  = NULL                                     ;
    return                                                 ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  ItemEditing->setText(name)                               ;
  EnterSQL(SC,plan->sql)                                   ;
    QString Q                                              ;
    SUID uuid = SC.Unique                                  (
              PlanTable(MajorUuid)                         ,
              "uuid"                                     ) ;
    SC.assureUuid                                          (
              PlanTable(MajorUuid)                         ,
              uuid,Types::SqlPlan                        ) ;
    SC.insertUuid(PlanTable(SqlPlans),uuid,"uuid")         ;
    SC.assureUuid                                          (
              PlanTable(Projects )                         ,
              uuid,type                                  ) ;
    Q = SC.sql.InsertInto                                  (
              PlanTable(Names    )                         ,
              3,"uuid","language","name"                 ) ;
    SC.insertName(Q,uuid,plan->LanguageId,name)            ;
    if (ObjectUuid ( )>0)                                  {
      UUIDs Juids                                          ;
      Juids << uuid                                        ;
      Join                                                 (
        SC                                                 ,
        ObjectUuid ( )                                     ,
        Types ::Project                                    ,
        Types ::SqlPlan                                    ,
        Groups::Subordination                              ,
        0                                                  ,
        Juids                                            ) ;
    }                                                      ;
    ItemEditing->setData(Qt::UserRole,uuid)                ;
  LeaveSQL(SC,plan->sql)                                   ;
  //////////////////////////////////////////////////////////
  setToolTip(tr("%1 projects").arg(count()))               ;
  Alert ( Done )                                           ;
}

bool N::ProjectsView::dropImage(QWidget * source,QPointF psf,const QImage & image)
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

bool N::ProjectsView::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
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
      Q = SC . sql . DeleteFrom                              (
            PlanTable(Groups)                                ,
            QString("where first = %1 and t1 = %2 "
                    "and t2 = %3 and relation = %4"          )
            . arg ( uuid                                     )
            . arg ( Types ::Project                          )
            . arg ( Types ::Picture                          )
            . arg ( Groups::Icon                         ) ) ;
      SC . Query ( Q )                                       ;
      Q = SC.sql.InsertInto(PlanTable(Groups)                ,
            5                                                ,
            "first"                                          ,
            "second"                                         ,
            "t1"                                             ,
            "t2"                                             ,
            "relation"                                     ) ;
      SC . Prepare ( Q )                                     ;
      SC . Bind    ( "first"    , uuid                     ) ;
      SC . Bind    ( "second"   , puid                     ) ;
      SC . Bind    ( "t1"       , Types ::Project          ) ;
      SC . Bind    ( "t2"       , Types ::Picture          ) ;
      SC . Bind    ( "relation" , Groups::Icon             ) ;
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

bool N::ProjectsView::setGroupIcon(SUID u,const QImage & image)
{
  PictureManager PM ( plan )                      ;
  SUID puid    = 0                                ;
  bool success = false                            ;
  QImage I = image                                ;
  Bustle  ( )                                     ;
  success = PM . Import ( I , puid )              ;
  Vacancy ( )                                     ;
  if ( puid <= 0 ) return true                    ;
  EnterSQL       ( SC , plan -> sql             ) ;
    QString Q                                     ;
    Q = SC . sql . DeleteFrom                     (
          PlanTable(Groups)                       ,
          QString ( "where first = %1 "
                    "and t1 = %2 "
                    "and t2 = %3 "
                    "and relation = %4"           )
          . arg   ( u                             )
          . arg   ( Types::Project                )
          . arg   ( Types::Picture                )
          . arg   ( Groups::Icon                  )
        )                                         ;
    SC . Query ( Q )                              ;
    Q = SC . sql . InsertInto                     (
          PlanTable(Groups)                       ,
          5                                       ,
          "first"                                 ,
          "second"                                ,
          "t1"                                    ,
          "t2"                                    ,
          "relation"                            ) ;
    SC . Prepare ( Q                            ) ;
    SC . Bind    ( "first"    , u               ) ;
    SC . Bind    ( "second"   , puid            ) ;
    SC . Bind    ( "t1"       , Types::Project  ) ;
    SC . Bind    ( "t2"       , Types::Picture  ) ;
    SC . Bind    ( "relation" , Groups::Icon    ) ;
    SC . Exec    (                              ) ;
  LeaveSQL       ( SC , plan -> sql             ) ;
  return true                                     ;
}

void N::ProjectsView::singleClicked(QListWidgetItem * item)
{
  SUID u = item -> data ( Qt::UserRole ) . toULongLong ( ) ;
  emit Selected ( u     )                                  ;
  Alert         ( Click )                                  ;
}

void N::ProjectsView::doubleClicked(QListWidgetItem * item)
{
  SUID u = item -> data ( Qt::UserRole   ) . toULongLong ( ) ;
  int  t = item -> data ( Qt::UserRole+1 ) . toInt       ( ) ;
  switch ( t )                                               {
    case Project::MAJORPROJECT                               :
      emit Command ( u                                     ) ;
      emit Command ( this , item -> text ( ) , u           ) ;
    break                                                    ;
    case Project::SUBPROJECT                                 :
      emit Command ( u                                     ) ;
      emit Command ( this , item -> text ( ) , u           ) ;
    break                                                    ;
    case Project::DOCUMENTS                                  :
    case Project::PLANNER                                    :
    case Project::SCHEDULER                                  :
    case Project::TASK                                       :
    case Project::FINANCE                                    :
    case Project::TEST                                       :
    case Project::NETWORK                                    :
    case Project::SQL                                        :
    case Project::FLOW                                       :
    case Project::DATAMODEL                                  :
    case Project::CLASSMODEL                                 :
    case Project::QTPROJECT                                  :
    case Project::INSTALLER                                  :
    case Project::SOURCES                                    :
    case Project::PICTURES                                   :
    case Project::GUI                                        :
    case Project::KNOWLEDGE                                  :
    case Project::WEBSITE                                    :
    case Project::BOOKMARKS                                  :
    case Project::SKETCH                                     :
    case Project::DATASHEET                                  :
      emit Project ( this , item -> text ( ) , u , t       ) ;
    break                                                    ;
  }                                                          ;
}

bool N::ProjectsView::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                       ;
  QListWidgetItem * atItem = itemAt  ( pos                      ) ;
  QMenu           * ma     = NULL                                 ;
  QAction         * aa     = NULL                                 ;
  mm . add ( 102 , tr("Refresh") )                                ;
  if (IsNull(atItem))                                             {
    if ( ObjectUuid ( ) > 0 )                                     {
      if (nEqual ( ObjectType ( ) , Project::MAJORPROJECT )      ||
          nEqual ( ObjectType ( ) , Project::SUBPROJECT   )     ) {
        ma = mm . addMenu ( tr("New") )                           ;
        mm      . add ( ma , 501 , tr("Subproject" ) )            ;
        mm      . add ( ma , 502 , tr("Documents"  ) )            ;
        mm      . add ( ma , 503 , tr("Planner"    ) )            ;
        mm      . add ( ma , 504 , tr("Scheduler"  ) )            ;
        mm      . add ( ma , 505 , tr("Task"       ) )            ;
        mm      . add ( ma , 506 , tr("Finance"    ) )            ;
        mm      . add ( ma , 507 , tr("Test"       ) )            ;
        mm      . add ( ma , 508 , tr("Network"    ) )            ;
        mm      . add ( ma , 509 , tr("Database"   ) )            ;
        mm      . add ( ma , 510 , tr("Flow"       ) )            ;
        mm      . add ( ma , 511 , tr("Data model" ) )            ;
        mm      . add ( ma , 512 , tr("Class model") )            ;
        mm      . add ( ma , 513 , tr("Qt project" ) )            ;
        mm      . add ( ma , 514 , tr("Installer"  ) )            ;
        mm      . add ( ma , 515 , tr("Sources"    ) )            ;
        mm      . add ( ma , 516 , tr("Pictures"   ) )            ;
        mm      . add ( ma , 517 , tr("GUI"        ) )            ;
        mm      . add ( ma , 518 , tr("Knowledge"  ) )            ;
        mm      . add ( ma , 519 , tr("Website"    ) )            ;
        mm      . add ( ma , 520 , tr("Bookmarks"  ) )            ;
        mm      . add ( ma , 521 , tr("Sketch"     ) )            ;
        mm      . add ( ma , 522 , tr("Check list" ) )            ;
      }                                                           ;
    }                                                             ;
  } else                                                          {
    int type = atItem->data(Qt::UserRole+1).toInt()               ;
    mm . add ( 111 , tr("Rename") )                               ;
    switch (type)                                                 {
      case Project::MAJORPROJECT                                  :
      break                                                       ;
      case Project::SUBPROJECT                                    :
      break                                                       ;
      case Project::DOCUMENTS                                     :
      break                                                       ;
      case Project::PLANNER                                       :
      break                                                       ;
      case Project::SCHEDULER                                     :
      break                                                       ;
      case Project::TASK                                          :
      break                                                       ;
      case Project::FINANCE                                       :
      break                                                       ;
      case Project::TEST                                          :
      break                                                       ;
      case Project::NETWORK                                       :
      break                                                       ;
      case Project::SQL                                           :
      break                                                       ;
      case Project::FLOW                                          :
      break                                                       ;
      case Project::DATAMODEL                                     :
      break                                                       ;
      case Project::CLASSMODEL                                    :
      break                                                       ;
      case Project::QTPROJECT                                     :
      break                                                       ;
      case Project::INSTALLER                                     :
      break                                                       ;
      case Project::SOURCES                                       :
      break                                                       ;
      case Project::PICTURES                                      :
      break                                                       ;
      case Project::GUI                                           :
      break                                                       ;
      case Project::KNOWLEDGE                                     :
      break                                                       ;
      case Project::WEBSITE                                       :
      break                                                       ;
      case Project::BOOKMARKS                                     :
      break                                                       ;
      case Project::SKETCH                                        :
      break                                                       ;
      case Project::DATASHEET                                     :
      break                                                       ;
    }                                                             ;
  }                                                               ;
  DockingMenu    ( mm   )                                         ;
  mm . setFont   ( plan )                                         ;
  aa = mm . exec (      )                                         ;
  if ( IsNull     (      aa ) ) return true                       ;
  if ( RunDocking ( mm , aa ) ) return true                       ;
  switch ( mm [ aa ] )                                            {
    case 102                                                      :
      startup (        )                                          ;
    break                                                         ;
    case 111                                                      :
      Rename  ( atItem )                                          ;
    break                                                         ;
    case 501                                                      :
      objectType = Types::Project                                 ;
      New(Project::SUBPROJECT)                                    ;
    break                                                         ;
    case 502                                                      :
      objectType = Types::Project                                 ;
      New(Project::DOCUMENTS)                                     ;
    break                                                         ;
    case 509                                                      :
      SqlPlan ( )                                                 ;
    break                                                         ;
    case 513                                                      :
      objectType = Types::Project                                 ;
      New(Project::QTPROJECT)                                     ;
    break                                                         ;
    case 515                                                      :
      objectType = Types::Project                                 ;
      New(Project::SOURCES)                                       ;
    break                                                         ;
    case 522                                                      :
      objectType = Types::Project                                 ;
      New(Project::DATASHEET)                                     ;
    break                                                         ;
    default                                                       :
    break                                                         ;
  }                                                               ;
  return true                                                     ;
}
