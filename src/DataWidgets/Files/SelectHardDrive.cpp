#include <nWork>

SelectHardDrive:: SelectHardDrive (QWidget * parent)
               :  nIconSelection  (          parent)
{
  /////////////////////////////////////////////////////////////////
  setViewMode(QListWidget::IconMode) ;
  setIconSize(QSize(128,128)       ) ;
  setGridSize(QSize(140,160)       ) ;
  setWordWrap(true                 ) ;
  /////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(Command(int)),this,SLOT(acceptCommand(int)));
  /////////////////////////////////////////////////////////////////
  load();
  /////////////////////////////////////////////////////////////////
  disconnect(this,SIGNAL(itemClicked(QListWidgetItem *)),0,0);
  connect(
    this , SIGNAL (itemDoubleClicked(QListWidgetItem*)),
    this , SLOT   (commandClicked   (QListWidgetItem*))
  );
}

SelectHardDrive::~SelectHardDrive(void)
{
}

void SelectHardDrive::load(void)
{
  clear();
  Drivers.clear();
  QFileInfoList d = QDir::drives();
  for (int ii=0;ii<d.count();ii++) {
    addCommand(QIcon(":/images/harddrive.png"),d[ii].absolutePath(),ii+1);
    Drivers[ii+1] = d[ii].absolutePath();
  };
}

void SelectHardDrive::acceptCommand(int Id)
{
  if (!Drivers.contains(Id)) return;
  emit Selected(Drivers[Id]);
}
