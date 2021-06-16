#include <datawidgets.h>

N::PictureBrowser:: PictureBrowser (QWidget * parent,Plan * p)
                  : FileView       (          parent,       p)
{
  WidgetClass                                     ;
  addShortcut(Qt::Key_Insert,this,SLOT(Import())) ;
}

N::PictureBrowser::~PictureBrowser (void)
{
}

QStringList N::PictureBrowser::selectedFiles(void)
{
  QStringList Files                                           ;
  QList<QListWidgetItem *> items = selectedItems()            ;
  for (int i=0;i<items.count();i++)                           {
    QByteArray A = items[i]->data(Qt::UserRole).toByteArray() ;
    Files << QString::fromUtf8(A)                             ;
  }                                                           ;
  return Files                                                ;
}

QStringList N::PictureBrowser::Filters(void)
{
  return Images :: ExtensionFilters () ;
}

QListWidgetItem * N::PictureBrowser::Load (QFileInfo & info)
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
  LWI->setText(info.fileName())          ;
  LWI->setData(Qt::UserRole,Fn.toUtf8()) ;
  LWI->setToolTip                        (
    tr("%1 x %2"                         )
    .arg(image->width ()                 )
    .arg(image->height()                 )
  )                                      ;
  delete image                           ;
  return LWI                             ;
}

void N::PictureBrowser::Import(void)
{
  QStringList files = selectedFiles()                           ;
  plan->Talk(tr("Total %1 files").arg(files.count()))           ;
  Alert ( Done )                                                ;
  DoProcessEvents                                               ;
  PictureManager PM ( plan )                                    ;
  QProgressBar * P  = plan->Progress (tr("%v/%m pictures"))     ;
  QToolButton  * tb = new QToolButton(plan->status)             ;
  tb->setToolTip(tr("Stop importing"))                          ;
  P -> setRange (0,files.count())                               ;
  int total = 0                                                 ;
  for (int i=0;tb->isChecked() && i<files.count();i++)          {
    P->setValue(i+1)                                            ;
    plan->showMessage(files[i])                                 ;
    DoProcessEvents                                             ;
    if (PM.Import(files[i])) total++                            ;
  }                                                             ;
  P -> deleteLater()                                            ;
  tb-> deleteLater()                                            ;
  QString m = tr("Append %1 pictures into database").arg(total) ;
  plan->showMessage(m)                                          ;
  plan->Talk(m)                                                 ;
  Alert ( Done )                                                ;
}
