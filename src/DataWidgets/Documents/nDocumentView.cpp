#include <datawidgets.h>

N::DocumentView:: DocumentView    ( QWidget * parent , Plan * p )
                : GroupView       (           parent ,        p )
                , DocumentManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DocumentView::~DocumentView(void)
{
}

void N::DocumentView::Configure(void)
{
  type = Types::Document                                     ;
  icon = plan->Icon( type,1,0,QIcon(":/images/folder.png") ) ;
  setWindowTitle               ( tr("Document categories") ) ;
  setDragDropMode              ( DragDrop                  ) ;
  setViewMode                  ( IconMode                  ) ;
  setIconSize                  ( QSize(128,128)            ) ;
  setGridSize                  ( QSize(144,192)            ) ;
  setMovement                  ( Snap                      ) ;
  setResizeMode                ( Adjust                    ) ;
  setSelectionMode             ( ExtendedSelection         ) ;
  setWordWrap                  ( true                      ) ;
  setMouseTracking             ( true                      ) ;
  setWrapping                  ( true                      ) ;
  setTextElideMode             ( Qt::ElideNone             ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded     ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded     ) ;
  setMinimumSize               ( QSize(172,192)            ) ;
  setDropFlag                  ( DropText     , false      ) ;
  setDropFlag                  ( DropUrls     , false      ) ;
  setDropFlag                  ( DropImage    , true       ) ;
  setDropFlag                  ( DropHtml     , false      ) ;
  setDropFlag                  ( DropColor    , false      ) ;
  setDropFlag                  ( DropTag      , false      ) ;
  setDropFlag                  ( DropPicture  , true       ) ;
  setDropFlag                  ( DropPeople   , false      ) ;
  setDropFlag                  ( DropVideo    , false      ) ;
  setDropFlag                  ( DropAlbum    , false      ) ;
  setDropFlag                  ( DropGender   , false      ) ;
  setDropFlag                  ( DropDivision , true       ) ;
  setDropFlag                  ( DropDocument , true       ) ;
  plan->setFont                ( this                      ) ;
}

bool N::DocumentView::dropDocuments(QWidget * source,QPointF psf,const UUIDs & Uuids)
{ Q_UNUSED ( source )                ;
  QPoint pos = psf.toPoint()         ;
  QListWidgetItem * it = itemAt(pos) ;
  if (IsNull(it)) return false       ;
  SUID u = nListUuid(it)             ;
  if (u<=0) return false             ;
  GroupItems GI ( plan )             ;
  EnterSQL ( SC , plan->sql )        ;
    UUIDs U = Uuids                  ;
    GI.Join                          (
      SC                             ,
      u                              ,
      Types::Division                ,
      Types::Document                ,
      Groups::Subordination          ,
      0,U                          ) ;
  LeaveSQL ( SC , plan->sql )        ;
  Alert ( Done )                     ;
  return true                        ;
}

QString N::DocumentView::ToolTipTotoal(int total)
{
  return tr("%1 documents in this folder").arg(total) ;
}

int N::DocumentView::Counting(SqlConnection & Connection,SUID u)
{
  return GroupItems :: Count           (
           Connection                  ,
           u                           ,
           Types    :: Division        ,
           type                        ,
           Groups   :: Subordination ) ;
}

void N::DocumentView::reportItems(void)
{
  QString m                                  ;
  m = tr("%1 document folders").arg(count()) ;
  setToolTip ( m )                           ;
}

bool N::DocumentView::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                                     ;
  QMenu           * ms = NULL                                   ;
  QMenu           * mx = NULL                                   ;
  QMenu           * me = NULL                                   ;
  QAction         * aa = NULL                                   ;
  QListWidgetItem * it = itemAt(pos)                            ;
  UUIDs             Tuids                                       ;
  mm.add(101,tr("Open"))                                        ;
  mm.add(102,tr("New" ))                                        ;
  if (NotNull(it)) mm.add(103,tr("Rename"))                     ;
  mm.addSeparator()                                             ;
  mm.add(201,tr("Reflush"          ))                           ;
  if (NotNull(it)) mm.add(202,tr("Reflush item"))               ;
  mm.addSeparator()                                             ;
  mm.add(301,tr("Copy to clipboard"))                           ;
  mm.add(302,tr("Clear selection"  ))                           ;
  mm.add(303,tr("Select all"       ))                           ;
  me = mm . addMenu ( tr("Edit"       ) )                       ;
  ms = mm . addMenu ( tr("Adjustments") )                       ;
  mx = mm . addMenu ( tr("Selections" ) )                       ;
  mm.add(me,211,tr("Multilingual translations"))                ;
  ///////////////////////////////////////////////////////////////
  AdjustMenu     ( mm , ms )                                    ;
  SelectionsMenu ( mm , mx )                                    ;
  DockingMenu    ( mm      )                                    ;
  ///////////////////////////////////////////////////////////////
  mm.setFont(plan)                                              ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  ///////////////////////////////////////////////////////////////
  if ( RunDocking     ( mm , aa ) ) return true                 ;
  if ( RunAdjustment  ( mm , aa ) ) return true                 ;
  if ( RunSelections  ( mm , aa ) ) return true                 ;
  ///////////////////////////////////////////////////////////////
  switch (mm[aa])                                               {
    case 101                                                    :
      nSafeExec(it) doubleClicked(it)                           ;
    break                                                       ;
    case 102                                                    :
      New     (    )                                            ;
    break                                                       ;
    case 103                                                    :
      Rename  (    )                                            ;
    break                                                       ;
    case 201                                                    :
      startup (    )                                            ;
    break                                                       ;
    case 202                                                    :
      Refresh ( it )                                            ;
    break                                                       ;
    case 211                                                    :
      Tuids = ItemUuids()                                       ;
      emit Translations(windowTitle(),Tuids)                    ;
    break                                                       ;
    case 301                                                    :
      CopyToClipboard ( )                                       ;
    break                                                       ;
    case 302                                                    :
      SelectNone      ( )                                       ;
    break                                                       ;
    case 303                                                    :
      SelectAll       ( )                                       ;
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

bool N::DocumentView::JoinImage(SUID u,const QImage & image)
{
  bool result = setGroupIcon(u,image) ;
  QListWidgetItem * item = uuidAt(u)  ;
  Refresh ( item )                    ;
  return result                       ;
}

bool N::DocumentView::JoinPictures(SUID u,const UUIDs & Uuids)
{
  if (Uuids.count()<=0) return true         ;
  bool result = setGroupIcon( u , Uuids[0]) ;
  QListWidgetItem * item = uuidAt(u)        ;
  Refresh ( item )                          ;
  return result                             ;
}
