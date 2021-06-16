#include <datawidgets.h>

N::ArchiveList:: ArchiveList(QWidget * parent,Plan * p)
               : TreeWidget (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ArchiveList::~ArchiveList(void)
{
}

QDir & N::ArchiveList::Path(void)
{
  return Root ;
}

void N::ArchiveList::setRoot(QString path)
{
  Root = QDir ( path ) ;
}

void N::ArchiveList::Configure (void)
{
  QStringList h                           ;
  h << tr            ( "File"           ) ;
  h << tr            ( "Type"           ) ;
  h << tr            ( "Size"           ) ;
  h <<                 ""                 ;
  setRootIsDecorated ( false            ) ;
  setDragDropMode    ( NoDragDrop       ) ;
  setColumnCount     ( 4                ) ;
  assignHeaderItems  ( h                ) ;
  setAlignment       ( 2                  ,
                       Qt::AlignRight     |
                       Qt::AlignVCenter ) ;
  plan -> setFont    ( this             ) ;
  MountClicked       ( 1                ) ;
}

void N::ArchiveList::singleClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( column ) ;
  int flag = item->data(0,Qt::UserRole).toInt()        ;
  flag    ^= 1                                         ;
  item    -> setData ( 0 , Qt::UserRole , flag )       ;
  //////////////////////////////////////////////////////
  switch (flag)                                        {
    case 0                                             :
      item -> setIcon ( 0,QIcon(":/icons/empty.png" )) ;
    break                                              ;
    case 1                                             :
      item -> setIcon ( 0,QIcon(":/icons/remove.png")) ;
    break                                              ;
  }                                                    ;
  Alert ( Click )                                      ;
  emit selectionChanged ( )                            ;
}

int N::ArchiveList::Selections(void)
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

QStringList N::ArchiveList::PickFiles(void)
{
  QStringList L                                  ;
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it                         ;
    it = topLevelItem ( i )                      ;
    int type = it->data(1,Qt::UserRole).toInt()  ;
    if (type==1) L << it->text(0)                ;
  }                                              ;
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it                         ;
    it = topLevelItem ( i )                      ;
    int type = it->data(1,Qt::UserRole).toInt()  ;
    if (type==0) L << it->text(0)                ;
  }                                              ;
  return L                                       ;
}

void N::ArchiveList::Delete(void)
{
  TreeWidgetItems items                         ;
  ///////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)       {
    QTreeWidgetItem * it                        ;
    it = topLevelItem ( i )                     ;
    int flag = it->data(0,Qt::UserRole).toInt() ;
    if (flag>0) items << it                     ;
  }                                             ;
  ///////////////////////////////////////////////
  if (items.count()<=0) return                  ;
  ///////////////////////////////////////////////
  for (int i=0;i<items.count();i++)             {
    QTreeWidgetItem * it = items[i]             ;
    int index = indexOfTopLevelItem(it)         ;
    if (index>=0) takeTopLevelItem(index)       ;
  }                                             ;
  ///////////////////////////////////////////////
  Alert ( Menu )                                ;
}

void N::ArchiveList::Append(QStringList files,bool bypass)
{
  QStringList L                                          ;
  QString     S                                          ;
  ////////////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)                {
    QTreeWidgetItem * it                                 ;
    it = topLevelItem ( i )                              ;
    QString P = Root.absoluteFilePath(it->text(0))       ;
    if (!L.contains(P)) L << P                           ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  foreach (S,files)                                      {
    if (!L.contains(S))                                  {
      NewTreeWidgetItem ( it )                           ;
      QFileInfo F(S)                                     ;
      QString   p = F.absoluteFilePath()                 ;
      p = Root.relativeFilePath(p)                       ;
      it -> setIcon   ( 0 , QIcon(":/icons/empty.png") ) ;
      it -> setText   ( 0 , p                          ) ;
      it -> setData   ( 0 , Qt::UserRole , 0           ) ;
      if (F.isDir())                                     {
        it -> setText ( 1 , tr("Directory")            ) ;
        it -> setData ( 1 , Qt::UserRole , 1           ) ;
        setAlignments   ( it                           ) ;
        addTopLevelItem ( it                           ) ;
        //////////////////////////////////////////////////
        QDir          D ( S )                            ;
        QFileInfoList L                                  ;
        QStringList   M                                  ;
        L = D.entryInfoList                              (
              QDir::Dirs                                 |
              QDir::Files                                |
              QDir::NoDotAndDotDot                       |
              QDir::NoSymLinks                         ) ;
        for (int i=0;i<L.count();i++)                    {
          M << L[i].absoluteFilePath()                   ;
        }                                                ;
        if (M.count()>0) Append ( M , true )             ;
      } else
      if (F.isFile())                                    {
        it -> setText ( 1 , tr("File")                 ) ;
        it -> setData ( 1 , Qt::UserRole , 0           ) ;
        it -> setText ( 2 , QString::number(F.size())  ) ;
        setAlignments   ( it                           ) ;
        addTopLevelItem ( it                           ) ;
      }                                                  ;
      plan -> processEvents ( )                          ;
    }                                                    ;
  }                                                      ;
  if (!bypass)                                           {
    emit AutoFit (       )                               ;
    Alert        ( Click )                               ;
  }                                                      ;
}
