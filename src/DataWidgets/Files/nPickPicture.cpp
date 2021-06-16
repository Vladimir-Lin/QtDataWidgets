#include <datawidgets.h>

N::PickPicture:: PickPicture    ( QWidget * parent , Plan * p )
               : PictureBrowser (           parent ,        p )
               , watcher        ( NULL                        )
               , Index          ( 0                           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PickPicture::~PickPicture (void)
{
}

QSize N::PickPicture::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize ( 1280 ,  900 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::PickPicture::resizeEvent(QResizeEvent * event)
{
  QListWidget :: resizeEvent ( event ) ;
  relocation                 (       ) ;
}

void N::PickPicture::showEvent(QShowEvent * event)
{
  QListWidget :: showEvent ( event ) ;
  relocation               (       ) ;
}

void N::PickPicture::Configure (void)
{
  setViewMode                  ( IconMode               ) ;
  setIconSize                  ( QSize(128,128)         ) ;
  setGridSize                  ( QSize(132,152)         ) ;
  setMovement                  ( Snap                   ) ;
  setResizeMode                ( Adjust                 ) ;
  setSelectionMode             ( SingleSelection        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAlwaysOff ) ;
  setDragDropMode              ( NoDragDrop             ) ;
  MountCommands                (                        ) ;
}

void N::PickPicture::relocation(void)
{
}

void N::PickPicture::setRoot(QString path)
{
  Root = QDir ( path )                                         ;
  //////////////////////////////////////////////////////////////
  watcher = new QFileSystemWatcher  ( this                   ) ;
  nConnect ( watcher , SIGNAL ( directoryChanged (QString) )   ,
             this    , SLOT   ( imageChanged     (QString) ) ) ;
  nConnect ( watcher , SIGNAL ( fileChanged      (QString) )   ,
             this    , SLOT   ( imageChanged     (QString) ) ) ;
  //////////////////////////////////////////////////////////////
}

void N::PickPicture::startup(QDir directory)
{
  setWindowTitle(directory.absolutePath())                  ;
  QList<QFileInfo> fil = directory.entryInfoList(Filters()) ;
  QListWidget::blockSignals (true)                          ;
  int total = (width() / 132 ) * ( height() / 152 )         ;
  int id    = 0                                             ;
  clear ( )                                                 ;
  bool in = true                                            ;
  for (int i=0;in && i<fil.count();i++)                     {
    QListWidgetItem * LWI = Load (fil[i])                   ;
    if (NotNull(LWI))                                       {
      if (i>=Index && i<(Index+total))                      {
        QListWidget :: addItem ( LWI )                      ;
      } else                                                {
        delete LWI                                          ;
        if (i>=(Index+total)) in = false                    ;
      }                                                     ;
      id++                                                  ;
    }                                                       ;
    plan -> processEvents ( )                               ;
  }                                                         ;
  QListWidget::blockSignals (false)                         ;
  Alert ( Done )                                            ;
  setToolTip(tr("%1 files").arg(fil.count()))               ;
  emit Full ( Index , ( total == count() ) )                ;
  if (count()<=0 || fil.count()<=0) emit Empty ( )          ;
}

QListWidgetItem * N::PickPicture::Load (QFileInfo & info)
{
  if (!info.exists()) return NULL        ;
  QString Fn = info.absoluteFilePath()   ;
  QImage * image = new QImage()          ;
  if (!image->load(Fn))                  {
    delete image                         ;
    return NULL                          ;
  }                                      ;
  PictureManager PM(plan)                ;
  QSize s(128,128)                       ;
  QIcon icon = PM.Icon(image,s)          ;
  NewListWidgetItem(LWI)                 ;
  LWI->setIcon(icon)                     ;
  LWI->setData(Qt::UserRole,Fn.toUtf8()) ;
  LWI->setText                           (
    tr("%1 x %2"                         )
    .arg(image->width ()                 )
    .arg(image->height()                 )
  )                                      ;
  delete image                           ;
  return LWI                             ;
}

void N::PickPicture::imageChanged(const QString & path)
{ Q_UNUSED             ( path                              ) ;
  QTimer :: singleShot ( 1000 , this , SLOT( Refresh ( ) ) ) ;
}

void N::PickPicture::Refresh(void)
{
  startup ( Root ) ;
}

bool N::PickPicture::CurrentPath(QString & path)
{
  QListWidgetItem * LWI = currentItem()                ;
  if (IsNull(LWI)) return false                        ;
  QByteArray P = LWI->data(Qt::UserRole).toByteArray() ;
  QString    S = QString::fromUtf8(P)                  ;
  path = Root.relativeFilePath(S)                      ;
  return ( path.length() > 0 )                         ;
}

void N::PickPicture::singleClicked(QListWidgetItem * item)
{ Q_UNUSED ( item )              ;
  QString path                   ;
  if (!CurrentPath(path)) return ;
  FileSelected ( path )          ;
}

void N::PickPicture::Previous(void)
{
  int total = (width() / 132 ) * ( height() / 152 ) ;
  Index    -= total                                 ;
  if (Index < 0) Index = 0                          ;
  Refresh ( )                                       ;
}

void N::PickPicture::Next(void)
{
  int total = (width() / 132 ) * ( height() / 152 ) ;
  Index    += total                                 ;
  Refresh ( )                                       ;
}
