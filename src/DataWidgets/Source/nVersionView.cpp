#include <datawidgets.h>

N::VersionView:: VersionView (QWidget * parent,Plan * p)
               : TreeWidget  (          parent,       p)
               , Version     (0                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VersionView::~VersionView(void)
{
}

QSize N::VersionView::SuitableSize (void)
{
  return size ( ) ;
}

void N::VersionView::Configure(void)
{
  NewTreeWidgetItem            ( head                  )         ;
  head -> setText              ( 0 , tr("File"      )  )         ;
  head -> setText              ( 1 , tr("Format"    )  )         ;
  head -> setText              ( 2 , tr("Size"      )  )         ;
  head -> setText              ( 3 , tr("Subversion")  )         ;
  head -> setText              ( 4 , ""                )         ;
  setWindowTitle               ( tr("Version view")    )         ;
  setDragDropMode              ( NoDragDrop            )         ;
  setRootIsDecorated           ( false                 )         ;
  setAlternatingRowColors      ( true                  )         ;
  setSelectionMode             ( ExtendedSelection     )         ;
  setColumnCount               ( 5                     )         ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )         ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )         ;
  setHeaderItem                ( head                  )         ;
  plan -> setFont              ( this                  )         ;
  PassDragDrop = false                                           ;
  ////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::VersionView::startup(void)
{
  if (Version==0) return false                   ;
  clear             (                          ) ;
  setSortingEnabled ( false                    ) ;
  CodeManager   CM  ( plan                     ) ;
  EnterSQL          ( SC , plan->sql           ) ;
    QString                      Q               ;
    UUIDs                        Files           ;
    TreeWidgetItems              Items           ;
    QMap<QTreeWidgetItem *,SUID> FMaps           ;
    QMap<SUID,int>               FileSize        ;
    QMap<SUID,int>               Attributes      ;
    SUID                         uuid            ;
    Q = SC.sql.SelectFrom                        (
          "name"                                 ,
          PlanTable(SourceVersion)               ,
          SC.WhereUuid(Version)                ) ;
    if (SC.Fetch(Q))                             {
      setWindowTitle(SC.String(0))               ;
    }                                            ;
    Q = SC.sql.SelectFrom                        (
          "file,subversion,path"                 ,
          PlanTable(SourceMaps)                  ,
          SC.WhereUuid(Version)                ) ;
    SqlLoopNow ( SC , Q )                        ;
      SUID      file = SC . Uuid   ( 0 )         ;
      int       sv   = SC . Int    ( 1 )         ;
      QString   path = SC . String ( 2 )         ;
      QFileInfo FP ( path )                      ;
      QString   suffix = FP . suffix (   )       ;
      ////////////////////////////////////////////
      NewTreeWidgetItem ( item )                 ;
      item->setData( 0 , Qt::UserRole,file   )   ;
      item->setText( 0 , path                )   ;
      item->setText( 1 , suffix              )   ;
      item->setText( 3 , QString::number(sv) )   ;
      ////////////////////////////////////////////
      Items << item                              ;
      FMaps  [ item ] = file                     ;
      addTopLevelItem ( item )                   ;
      if (!Files.contains(file)) Files << file   ;
    SqlLoopErr ( SC , Q )                        ;
    SqlLoopEnd ( SC , Q )                        ;
    foreach (uuid,Files)                         {
      CM . GetAttributes                         (
        SC                                       ,
        uuid                                     ,
        Attributes[uuid]                         ,
        FileSize  [uuid]                       ) ;
    }                                            ;
    for (int i=0;i<Items.count();i++)            {
      QTreeWidgetItem * item = Items[i]          ;
      SUID file = FMaps[item]                    ;
      if (FileSize.contains(file))               {
        int  size  = FileSize   [file]           ;
        int  attr  = Attributes [file]           ;
        bool IsDir = CM.isDirectory(attr)        ;
        item->setData(1,Qt::UserRole,attr)       ;
        if (IsDir)                               {
          item->setText(1,tr("Directory"))       ;
        } else                                   {
          item->setTextAlignment                 (
                      2                          ,
                      Qt::AlignRight             |
                      Qt::AlignVCenter         ) ;
          item->setText(2,QString::number(size)) ;
        }                                        ;
      }                                          ;
    }                                            ;
  LeaveSQL          ( SC , plan->sql          )  ;
  ////////////////////////////////////////////////
  SuitableColumns   (                         )  ;
  setSortingEnabled ( true                    )  ;
  sortItems         ( 0 , Qt::AscendingOrder  )  ;
  setToolTip(tr("%1 files"                       )
             .arg(topLevelItemCount())        )  ;
  Alert ( Done )                                 ;
  return true                                    ;
}

void N::VersionView::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID uuid = nTreeUuid(item,0)                               ;
  emit FileUUID ( uuid , item -> text ( 0 ) , windowTitle() ) ;
}
