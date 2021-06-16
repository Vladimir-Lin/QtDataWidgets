#include <datawidgets.h>

N::ArchivePick:: ArchivePick(QWidget * parent,Plan * p)
               : TreeWidget (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ArchivePick::~ArchivePick(void)
{
}

QDir & N::ArchivePick::Path(void)
{
  return Root ;
}

QDir & N::ArchivePick::CurrentPath(void)
{
  return Current ;
}

void N::ArchivePick::setRoot(QString path)
{
  Root    = QDir ( path ) ;
  Current = QDir ( path ) ;
}

void N::ArchivePick::Configure (void)
{
  QStringList h                           ;
  h << tr            ( "Entry"          ) ;
  h << tr            ( "Size"           ) ;
  h <<                 ""                 ;
  setRootIsDecorated ( false            ) ;
  setDragDropMode    ( NoDragDrop       ) ;
  setColumnCount     ( 3                ) ;
  assignHeaderItems  ( h                ) ;
  setAlignment       ( 1                  ,
                       Qt::AlignRight     |
                       Qt::AlignVCenter ) ;
  plan -> setFont    ( this             ) ;
  MountClicked       ( 1                ) ;
}

void N::ArchivePick::singleClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( column )                                    ;
  int flag = item->data(0,Qt::UserRole).toInt()          ;
  flag    ^= 1                                           ;
  item    -> setData ( 0 , Qt::UserRole , flag )         ;
  ////////////////////////////////////////////////////////
  switch (flag)                                          {
    case 0                                               :
      item -> setIcon ( 0 , QIcon(":/icons/empty.png") ) ;
    break                                                ;
    case 1                                               :
      item -> setIcon ( 0 , QIcon(":/icons/join.png" ) ) ;
    break                                                ;
  }                                                      ;
  Alert ( Click )                                        ;
  emit selectionChanged ( )                              ;
}

void N::ArchivePick::List(void)
{
  setFocus ( Qt::TabFocusReason )                                      ;
  emit Listing          ( )                                            ;
  plan -> processEvents ( )                                            ;
  //////////////////////////////////////////////////////////////////////
  QFileInfoList dirs  = Current . entryInfoList ( QDir::Dirs           |
                                                  QDir::NoDotAndDotDot |
                                                  QDir::NoSymLinks   ) ;
  QFileInfoList files = Current . entryInfoList ( QDir::Files          |
                                                  QDir::NoDotAndDotDot |
                                                  QDir::NoSymLinks   ) ;
  QFileInfo     F                                                      ;
  QStringList   L                                                      ;
  clear ( )                                                            ;
  foreach ( F , dirs  )                                                {
    NewTreeWidgetItem ( it )                                           ;
    QString p = F.absoluteFilePath()                                   ;
    QString f = p                                                      ;
    p = Current.relativeFilePath(p)                                    ;
    f = Root   .relativeFilePath(f)                                    ;
    it -> setIcon   ( 0 , QIcon(":/icons/empty.png") )                 ;
    it -> setText   ( 0 , p                          )                 ;
    it -> setData   ( 0 , Qt::UserRole , 0           )                 ;
    it -> setText   ( 1 , tr("Directory")            )                 ;
    it -> setData   ( 1 , Qt::UserRole , 1           )                 ;
    setAlignments   ( it                             )                 ;
    addTopLevelItem ( it                             )                 ;
    plan -> processEvents ( )                                          ;
    L << f                                                             ;
  }                                                                    ;
  foreach ( F , files )                                                {
    NewTreeWidgetItem ( it )                                           ;
    QString p = F.absoluteFilePath()                                   ;
    p = Current.relativeFilePath(p)                                    ;
    it -> setIcon   ( 0 , QIcon(":/icons/empty.png") )                 ;
    it -> setText   ( 0 , p                          )                 ;
    it -> setData   ( 0 , Qt::UserRole , 0           )                 ;
    it -> setText   ( 1 , QString::number(F.size())  )                 ;
    it -> setData   ( 1 , Qt::UserRole , 0           )                 ;
    setAlignments   ( it                             )                 ;
    addTopLevelItem ( it                             )                 ;
    plan -> processEvents ( )                                          ;
  }                                                                    ;
  emit AutoFit ( )                                                     ;
  //////////////////////////////////////////////////////////////////////
  bool topmost                                                         ;
  topmost = (Current.absoluteFilePath("")==Root.absoluteFilePath(""))  ;
  emit Folders ( topmost , L )                                         ;
  emit Ready   (             )                                         ;
  plan -> processEvents (    )                                         ;
  Alert        ( Error       )                                         ;
}

int N::ArchivePick::Selections(void)
{
  int total = 0                                 ;
  for (int i=0;i<topLevelItemCount();i++)       {
    QTreeWidgetItem * it                        ;
    it = topLevelItem ( i )                     ;
    int flag = it->data(0,Qt::UserRole).toInt() ;
    if (flag>0) total++                         ;
  }                                             ;
  return total                                  ;
}

void N::ArchivePick::CdUp(void)
{
  Current . cdUp ( ) ;
  List           ( ) ;
}

void N::ArchivePick::Directory(QString path)
{
  Current = Root . absoluteFilePath ( path ) ;
  List                              (      ) ;
}

QStringList N::ArchivePick::PickFiles(void)
{
  QStringList L ;
  for (int i=0;i<topLevelItemCount();i++)          {
    QTreeWidgetItem * it                           ;
    it = topLevelItem ( i )                        ;
    int flag = it->data(0,Qt::UserRole).toInt()    ;
    if (flag>0)                                    {
      int type = it->data(1,Qt::UserRole).toInt()  ;
      if (type==1)                                 {
        L << Current.absoluteFilePath(it->text(0)) ;
      }                                            ;
    }                                              ;
  }                                                ;
  for (int i=0;i<topLevelItemCount();i++)          {
    QTreeWidgetItem * it                           ;
    it = topLevelItem ( i )                        ;
    int flag = it->data(0,Qt::UserRole).toInt()    ;
    if (flag>0)                                    {
      int type = it->data(1,Qt::UserRole).toInt()  ;
      if (type==0)                                 {
        L << Current.absoluteFilePath(it->text(0)) ;
      }                                            ;
    }                                              ;
  }                                                ;
  return L                                         ;
}
