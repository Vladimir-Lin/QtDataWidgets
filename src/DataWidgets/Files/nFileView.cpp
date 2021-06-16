#include <datawidgets.h>

N::FileView:: FileView (QWidget * parent,Plan * p)
            : SnapView (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::FileView::~FileView (void)
{
}

QMimeData * N::FileView::dragMime(void)
{
  if (Items.count()<=0) return NULL                           ;
  QMimeData * mime = new QMimeData()                          ;
  QList<QUrl> Urls                                            ;
  for (int i=0;i<Items.count();i++)                           {
    QByteArray A = Items[i]->data(Qt::UserRole).toByteArray() ;
    QUrl u(QString::fromUtf8(A))                              ;
    Urls << u                                                 ;
  }                                                           ;
  if (Urls.count()>0) mime->setUrls(Urls)                     ;
  return mime                                                 ;
}

QString N::FileView::MimeType(const QMimeData * mime)
{
  return "" ;
}

UUIDs N::FileView::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs  Uuids ;
  return Uuids ;
}

bool N::FileView::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return false ;
}

bool N::FileView::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return false ;
}

bool N::FileView::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return false ;
}

void N::FileView::singleClicked(QListWidgetItem * item)
{
}

void N::FileView::doubleClicked(QListWidgetItem * item)
{
  QByteArray PATH     = item->data(Qt::UserRole).toByteArray() ;
  QString    filename = QString::fromUtf8(PATH)                ;
  QFileInfo info ( filename )                                  ;
  emit Clicked ( info )                                        ;
}

void N::FileView::Configure(void)
{
  setViewMode                  (IconMode                     ) ;
  setIconSize                  (QSize(128,128)               ) ;
  setGridSize                  (QSize(144,192)               ) ;
  setMovement                  (Snap                         ) ;
  setResizeMode                (Adjust                       ) ;
  setSelectionMode             (ExtendedSelection            ) ;
  setWordWrap                  (true                         ) ;
  setMouseTracking             (true                         ) ;
  setWrapping                  (true                         ) ;
  setTextElideMode             (Qt::ElideNone                ) ;
  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff       ) ;
  setVerticalScrollBarPolicy   (Qt::ScrollBarAlwaysOff       ) ;
  setMinimumSize               (QSize(144,200)               ) ;
  setDragDropMode              (DragDrop                     ) ;
  plan->setFont                (this                         ) ;
}

void N::FileView::startup(QDir directory)
{
  setWindowTitle(directory.absolutePath())                  ;
  QList<QFileInfo> fil = directory.entryInfoList(Filters()) ;
  QListWidget::blockSignals (true)                          ;
  for (int i=0;i<fil.count();i++)                           {
    QListWidgetItem * LWI = Load (fil[i])                   ;
    if (NotNull(LWI)) QListWidget::addItem(LWI)             ;
  }                                                         ;
  QListWidget::blockSignals (false)                         ;
  Alert ( Done )                                            ;
  setToolTip(tr("%1 files").arg(fil.count()))               ;
}
