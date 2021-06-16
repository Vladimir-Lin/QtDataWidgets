#include <datawidgets.h>

N::TagsEditor:: TagsEditor    (QWidget * parent,Plan * p)
              : ItemEditor    (          parent,       p)
              , TagsManager   (                        p)
              , TagsCheckable (false                    )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::TagsEditor::~TagsEditor(void)
{
}

void N::TagsEditor::Configure(void)
{
  setAccessibleName            ( "N::TagsEditor"           ) ;
  setObjectName                ( "N::TagsEditor"           ) ;
  setWindowIcon                ( QIcon(":/images/tag.png") ) ;
  setDropFlag                  ( DropText        , true    ) ;
  setDropFlag                  ( DropUrls        , false   ) ;
  setDropFlag                  ( DropImage       , false   ) ;
  setDropFlag                  ( DropHtml        , false   ) ;
  setDropFlag                  ( DropColor       , true    ) ;
  setDropFlag                  ( DropTag         , true    ) ;
  setDropFlag                  ( DropPicture     , true    ) ;
  setDropFlag                  ( DropPeople      , true    ) ;
  setDropFlag                  ( DropVideo       , true    ) ;
  setDropFlag                  ( DropAlbum       , true    ) ;
  setDropFlag                  ( DropSetsAlgebra , true    ) ;
  setDropFlag                  ( DropGender      , false   ) ;
  setDragDropMode              ( DragDrop                  ) ;
  setRootIsDecorated           ( false                     ) ;
  setAlternatingRowColors      ( true                      ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded     ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded     ) ;
  ////////////////////////////////////////////////////////////
  addConnector  ( "Dispatching"                              ,
                  this                                       ,
                  SIGNAL ( DispatchPending ( ) )             ,
                  this                                       ,
                  SLOT   ( SendPending     ( ) )           ) ;
  onlyConnector ( "Dispatching"                            ) ;
  ////////////////////////////////////////////////////////////
  LocalMsgs [ TagsEditJoinTags  ] = tr("Join tag %1")        ;
  LocalMsgs [ TagsEditCounting  ] = tr("Counting tag members, please wait...") ;
  LocalMsgs [ TagsEditShowItems ] = tr("%1 items join tag %2") ;
  LocalMsgs [ TagsEditListItems ] = tr("%1 items join tag") ;
}

QTreeWidgetItem * N::TagsEditor::uuidAt(SUID uuid)
{
  for (int i=0;i<topLevelItemCount();i++)  {
    QTreeWidgetItem * it = topLevelItem(i) ;
    SUID tuid = nTreeUuid ( it , 0 )       ;
    if (uuid==tuid) return it              ;
  }                                        ;
  return NULL                              ;
}

SUID N::TagsEditor::atTag(QPoint pos)
{
  return atUuid ( pos , 0 ) ;
}

void N::TagsEditor::setCheckable(QTreeWidgetItem * item)
{
  nDropOut              ( ! TagsCheckable                          ) ;
  item -> setCheckState ( 0 ,Qt::Unchecked                         ) ;
  item -> setFlags      ( item->flags () | Qt::ItemIsUserCheckable ) ;
}

QMimeData * N::TagsEditor::dragMime (void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  if ( TagsCheckable )                           {
    for (int i=0;i<topLevelItemCount();i++)      {
      QTreeWidgetItem * it = topLevelItem(i)     ;
      if (it->checkState(0)==Qt::Checked)        {
        Uuids << nTreeUuid(it,0)                 ;
      }                                          ;
    }                                            ;
  } else                                         {
    for (int i=0;i<topLevelItemCount();i++)      {
      QTreeWidgetItem * it = topLevelItem(i)     ;
      if (it->isSelected())                      {
        Uuids << nTreeUuid(it,0)                 ;
      }                                          ;
    }                                            ;
  }                                              ;
  if (Uuids.count()<=0)                          {
    QTreeWidgetItem * it = currentItem()         ;
    if (NotNull(it))                             {
      uuid = nTreeUuid(it,0)                     ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  if (Uuids.count()<=0 && uuid == 0) return NULL ;
  if (Uuids.count()==1)                          {
    uuid  = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "tag/uuid"  , uuid  )       ;
  } else                                         {
    setMime ( mime , "tag/uuids" , Uuids )       ;
  }                                              ;
  return mime                                    ;
}

bool N::TagsEditor::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler(mime) ;
}

bool N::TagsEditor::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::TagsEditor::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  QTreeWidgetItem * it = itemAt ( pos )                                    ;
  if (IsNull(it)) return 0                                                 ;
  SUID tag = nTreeUuid ( it , 0 )                                          ;
  if ( tag <= 0 ) return false                                             ;
  plan->showMessage(QString(LocalMsgs[TagsEditJoinTags]).arg(it->text(0))) ;
  return true                                                              ;
}

UUIDs N::TagsEditor::LoadUuids(SqlConnection & Connection)
{
  return TagsManager :: Uuids ( Connection , ItemType ) ;
}

SUID N::TagsEditor::AppendUuid(SqlConnection & Connection)
{
  return TagsManager :: NewTag ( Connection , ItemType ) ;
}

bool N::TagsEditor::dropColor(QWidget * source,QPointF pos,const QColor & color)
{
  return false ;
}

bool N::TagsEditor::dropTags(QWidget * source,QPointF psf,const UUIDs & Uuids)
{ Q_UNUSED ( source )                                       ;
  QPoint pos = psf.toPoint()                                ;
  SUID tag = atUuid ( pos , 0 )                             ;
  if (tag<=0) return false                                  ;
  UUIDs Tuids = Uuids                                       ;
  if (Tuids.contains(tag)) Tuids.takeAt(Tuids.indexOf(tag)) ;
  return dropObjects(pos,Tuids,Types::Tag)                  ;
}

bool N::TagsEditor::dropPictures(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return false               ;
  QPoint pos = psf.toPoint()                   ;
  return dropObjects(pos,Uuids,Types::Picture) ;
}

bool N::TagsEditor::dropPeople(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return false              ;
  QPoint pos = psf.toPoint()                  ;
  return dropObjects(pos,Uuids,Types::People) ;
}

bool N::TagsEditor::dropVideos(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return false             ;
  QPoint pos = psf.toPoint()                 ;
  return dropObjects(pos,Uuids,Types::Video) ;
}

bool N::TagsEditor::dropAlbums(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return false             ;
  QPoint pos = psf.toPoint()                 ;
  return dropObjects(pos,Uuids,Types::Album) ;
}

bool N::TagsEditor::dropSetsAlgebras(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  if (source==this) return false                   ;
  QPoint pos = psf.toPoint()                       ;
  return dropObjects(pos,Uuids,Types::SetsAlgebra) ;
}

bool N::TagsEditor::dropObjects(QPoint pos,const UUIDs & Uuids,int ObjectType)
{
  SUID tag = atUuid ( pos , 0 ) ;
  if (tag<=0) return false      ;
  VarArgs args                  ;
  UUIDs   uu = Uuids            ;
  args << "JoinTag"             ;
  args << tag                   ;
  args << ObjectType            ;
  toVariants    ( uu , args )   ;
  addSequence   (      args )   ;
  StartCommando (           )   ;
  return true                   ;
}

void N::TagsEditor::DispatchCommands(void)
{
  while ( Sequences . count ( ) > 0 )          {
    VarArgs args = Sequences[0]                ;
    Sequences.takeAt(0)                        ;
    if (args[0].toString()=="JoinTag")         {
      SUID  tag        = args[1].toULongLong() ;
      int   ObjectType = args[2].toInt      () ;
      UUIDs Uuids                              ;
      for (int i=3;i<args.count();i++)         {
        Uuids << args[i].toULongLong()         ;
      }                                        ;
      Alert   ( Click                    )     ;
      JoinTag ( tag , ObjectType , Uuids )     ;
    }                                          ;
  }                                            ;
}

void N::TagsEditor::JoinTag(ThreadData * d)
{
  if ( ! IsContinue ( d )             ) return                   ;
  if ( d -> Arguments . count ( ) < 3 ) return                   ;
  ////////////////////////////////////////////////////////////////
  SUID    tag     = d -> Arguments [ 0 ] . toULongLong ( )       ;
  int     t       = d -> Arguments [ 1 ] . toInt       ( )       ;
  bool    success = false                                        ;
  QString m                                                      ;
  UUIDs   U                                                      ;
  for (int i = 2 ; i < d -> Arguments . count ( ) ; i++ )        {
    U << d -> Arguments [ i ] . toULongLong ( )                  ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                               ;
  GroupItems    GX ( plan        )                               ;
  GX . AutoMap    = true                                         ;
  GX . GroupTable = GX . LookTable                               (
                      Types::Tag                                 ,
                      t                                          ,
                      Groups::Subordination                    ) ;
  if ( SC . open ( FunctionString ) )                            {
    GX . Join ( SC                                               ,
                tag                                              ,
                Types::Tag                                       ,
                t                                                ,
                Groups::Subordination                            ,
                0                                                ,
                U                                                ,
                true                                           ) ;
    success = true                                               ;
    if ( columnCount ( ) > 1 )                                   {
      QTreeWidgetItem * it = uuidAt ( tag )                      ;
      if ( NotNull ( it ) )                                      {
        SUID origin = nTreeInt ( it , 1 )                        ;
        if ( origin > 10000 )                                    {
          plan -> showMessage ( LocalMsgs [ TagsEditCounting ] ) ;
        }                                                        ;
        int total = GX . Count                                   (
                      SC                                         ,
                      tag                                        ,
                      Types::Tag                                 ,
                      t                                          ,
                      Groups::Subordination                    ) ;
        it -> setText          ( 1 , QString::number(total) )    ;
        it -> setTextAlignment ( 1 , Qt::AlignRight         )    ;
        m = QString ( LocalMsgs [ TagsEditShowItems ]            )
            . arg   ( U   . count (   )                          )
            . arg   ( it -> text  ( 0 )                        ) ;
      }                                                          ;
    } else                                                       {
      m = QString   ( LocalMsgs [ TagsEditListItems ]            )
          .arg      ( U   . count (   )                        ) ;
    }                                                            ;
    SC . close  ( )                                              ;
  }                                                              ;
  SC   . remove ( )                                              ;
  ////////////////////////////////////////////////////////////////
  if ( success )                                                 {
    if ( m . length ( ) > 0 ) alert ( "Done" , m )               ;
                         else Alert ( Done       )               ;
  } else                                                         {
    Alert ( Error      )                                         ;
  }                                                              ;
}

bool N::TagsEditor::JoinTag(SUID tag,int ObjectType,const UUIDs & Uuids)
{
  VarArgs args                ;
  args << tag                 ;
  args << ObjectType          ;
  toVariants ( Uuids , args ) ;
  start      ( 10003 , args ) ;
  return true                 ;
}

bool N::TagsEditor::Menu(QPoint pos)
{
  MenuManager         mm (this)                                    ;
  QMenu           *   me                                           ;
  QMenu           *   ma                                           ;
  QMenu           *   ms                                           ;
  QMenu           *   mc                                           ;
  QAction         *   a                                            ;
  QList<QAction   * > Acts = actions (     )                       ;
  QTreeWidgetItem *   item = itemAt  ( pos )                       ;
  //////////////////////////////////////////////////////////////////
  addMenuItems       ( mm , pos                         )          ;
  mm   . add         ( 1000011 , tr("Statistics"     )  )          ;
  if                 ( columnCount ( ) > 1              )          {
    mm . add         ( 1000012 , tr("Hide statistics")  )          ;
  }                                                                ;
  if ( NotNull ( item ) )                                          {
    mm . add         ( 1000013 , tr("Tag names")        )          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  mm  . addSeparator (                                  )          ;
  me  = mm . addMenu ( tr("Edit"       )                )          ;
  ma  = mm . addMenu ( tr("Adjustments")                )          ;
  ms  = mm . addMenu ( tr("Sorting"    )                )          ;
  mc  = mm . addMenu ( tr("Selections" )                )          ;
  me -> addAction    ( plan -> Action ( Menus::Insert ) )          ;
  me -> addAction    ( plan -> Action ( Menus::Export ) )          ;
  me -> addAction    ( plan -> Action ( Menus::Paste  ) )          ;
  if ( Acts . count ( ) > 0 )                                      {
    me -> addSeparator ( )                                         ;
    for (int i=0;i<Acts.count();i++) me->addAction(Acts[i])        ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  AdjustMenu     ( mm , ma )                                       ;
  SortingMenu    ( mm , ms )                                       ;
  SelectionsMenu ( mm , mc )                                       ;
  DockingMenu    ( mm      )                                       ;
  mm . setFont   ( plan    )                                       ;
  if ( plan -> Booleans [ "Desktop" ] )                            {
    pos = QCursor::pos (     )                                     ;
  } else                                                           {
    pos = mapToGlobal  ( pos )                                     ;
  }                                                                ;
  a = mm.exec(pos)                                                 ;
  if (!plan->Booleans["Desktop"])                                  {
    allowGesture = true                                            ;
  }                                                                ;
  if (IsNull(a)) return true                                       ;
  //////////////////////////////////////////////////////////////////
  switch ( mm [ a ] )                                              {
    case 1000011                                                   :
      Statistics     (   )                                         ;
    break                                                          ;
    case 1000012                                                   :
      setColumnCount ( 1 )                                         ;
    break                                                          ;
    case 1000013                                                   :
      emit nameCollection ( nTreeUuid ( item , 0 ) )               ;
    break                                                          ;
    default                                                        :
      if ( RunAdjustment ( mm , a        ) ) return true           ;
      if ( RunSorting    ( mm , a        ) ) return true           ;
      if ( RunSelections ( mm , a        ) ) return true           ;
      if ( RunDocking    ( mm , a        ) ) return true           ;
    return RunMenuResult ( a  , mm , pos )                         ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  return true                                                      ;
}

bool N::TagsEditor::addMenuItems(MenuManager & menu,QPoint pos)
{
  return false ;
}

bool N::TagsEditor::RunMenuResult(QAction * action,MenuManager & menu,QPoint pos)
{
  return false ;
}

void N::TagsEditor::Statistics(void)
{
  if ( columnCount ( ) <= 1 )                               {
    QTreeWidgetItem * header = headerItem ()                ;
    QString           First                                 ;
    if (NotNull(header)) First = header -> text( 0 )        ;
    setColumnCount ( 2 )                                    ;
    nIfSafe ( header )                                      {
      NewTreeWidgetItem ( headx )                           ;
      QStringList headers                                   ;
      headers << First                                      ;
      headers << tr   ( "Statistics" )                      ;
      for (int i=0;i<headers.count();i++)                   {
        headx -> setText ( i , headers[i] )                 ;
      }                                                     ;
      setAllAlignments ( headx , Qt::AlignCenter )          ;
      setFont          ( headx , Fonts::ListView )          ;
      setHeaderItem    ( headx                   )          ;
    }                                                       ;
    if ( columnCount ( ) > 1 ) resizeColumnToContents ( 0 ) ;
    DoProcessEvents                                         ;
  }                                                         ;
  start ( 10011 )                                           ;
}

void N::TagsEditor::Statistics(ThreadData * d)
{
  emit assignEnabling ( false       )                                      ;
  emit OnBusy         (             )                                      ;
  SqlConnection SC    ( plan -> sql )                                      ;
  GroupItems    GX    ( plan        )                                      ;
  int           rt = ItemType                                              ;
  switch ( rt )                                                            {
    case Types::Video                                                      :
      rt = Types::Album                                                    ;
    break                                                                  ;
  }                                                                        ;
  GX . AutoMap    = true                                                   ;
  GX . GroupTable = GX . LookTable                                         (
                      Types::Tag                                           ,
                      rt                                                   ,
                      Groups::Subordination                              ) ;
  if ( SC . open ( FunctionString ) )                                      {
    for (int i = 0 ; IsContinue ( d ) && i < topLevelItemCount ( ) ; i++ ) {
      QTreeWidgetItem * it = topLevelItem ( i                            ) ;
      SUID uuid  = nTreeUuid              ( it , 0                       ) ;
      int  total = GX . Count                                              (
                     SC                                                    ,
                     uuid                                                  ,
                     Types::Tag                                            ,
                     rt                                                    ,
                     Groups::Subordination                               ) ;
      it -> setText                       ( 1  , QString::number(total)  ) ;
      it -> setTextAlignment              ( 1  , Qt::AlignRight          ) ;
    }                                                                      ;
    SC . close        (       )                                            ;
  }                                                                        ;
  SC   . remove       (       )                                            ;
  emit GoRelax        (       )                                            ;
  emit assignEnabling ( true  )                                            ;
  emit AutoFit        (       )                                            ;
  Alert               ( Done  )                                            ;
}

bool N::TagsEditor::GetFirstUuids (
       SqlConnection & SC         ,
       UUIDs         & U          ,
       SUID            u          ,
       int             t1         ,
       int             t2         ,
       int             relation   )
{
  GroupItems GI ( plan )                                    ;
  GI . AutoMap    = true                                    ;
  GI . GroupTable = GI . LookTable ( t1 , t2 , relation )   ;
  U = GI . GetOwners     ( SC                               ,
                           u                                ,
                           t1                               ,
                           t2                               ,
                           relation                         ,
                           SC . OrderByAsc ( "position" ) ) ;
  return ( U . count ( ) > 0 )                              ;
}

bool N::TagsEditor::GetSecondUuids (
       SqlConnection & SC          ,
       UUIDs         & U           ,
       SUID            u           ,
       int             t1          ,
       int             t2          ,
       int             relation    )
{
  GroupItems GI ( plan )                                    ;
  GI . AutoMap    = true                                    ;
  GI . GroupTable = GI . LookTable ( t1 , t2 , relation )   ;
  U = GI . Subordination ( SC                               ,
                           u                                ,
                           t1                               ,
                           t2                               ,
                           relation                         ,
                           SC . OrderByAsc ( "position" ) ) ;
  return ( U . count ( ) > 0 )                              ;
}

void N::TagsEditor::TagsUnion(ThreadData * data)
{
}

void N::TagsEditor::TagsIntersection(ThreadData * data)
{
}

void N::TagsEditor::SendPending(void)
{
}

void N::TagsEditor::run(int T,ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return ;
  switch               ( T       ) {
    case 10001                     :
      startLoading     (         ) ;
      List             (         ) ;
      stopLoading      (         ) ;
    break                          ;
    case 10003                     :
      startLoading     (         ) ;
      JoinTag          ( d       ) ;
      stopLoading      (         ) ;
    break                          ;
    case 10011                     :
      startLoading     (         ) ;
      Statistics       ( d       ) ;
      stopLoading      (         ) ;
    break                          ;
    case 10101                     :
      startLoading     (         ) ;
      TagsUnion        ( d       ) ;
      stopLoading      (         ) ;
    break                          ;
    case 10102                     :
      startLoading     (         ) ;
      TagsIntersection ( d       ) ;
      stopLoading      (         ) ;
    break                          ;
  }                                ;
}
