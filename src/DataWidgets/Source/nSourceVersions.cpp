#include <datawidgets.h>

N::SourceVersions:: SourceVersions (QWidget * parent,Plan * p)
                  : TreeWidget     (          parent,       p)
                  , Source         (0                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SourceVersions::~SourceVersions (void)
{
}

QSize N::SourceVersions::SuitableSize (void)
{
  return size ( ) ;
}

void N::SourceVersions::Configure(void)
{
  NewTreeWidgetItem            ( head                  )         ;
  head -> setText              ( 0 , tr("Version")     )         ;
  setWindowTitle               ( tr("Source versions") )         ;
  setDragDropMode              ( NoDragDrop            )         ;
  setRootIsDecorated           ( false                 )         ;
  setAlternatingRowColors      ( true                  )         ;
  setSelectionMode             ( SingleSelection       )         ;
  setColumnCount               ( 1                     )         ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )         ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )         ;
  setHeaderItem                ( head                  )         ;
  plan -> setFont              ( this                  )         ;
  PassDragDrop = false                                           ;
  ////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::SourceVersions::startup(void)
{
  clear      (                )                   ;
  setEnabled ( false          )                   ;
  EnterSQL   ( SC , plan->sql )                   ;
    QString Q                                     ;
    QString n = SC.getName( PlanTable(Names)         ,
                  "uuid",vLanguageId,Source)      ;
    setWindowTitle ( n )                          ;
    Q = SC.sql.SelectFrom                         (
        "uuid,name" , PlanTable(SourceVersion)       ,
     QString("where source = %1 order by id asc"  )
        .arg(Source)                            ) ;
    SqlLoopNow ( SC , Q )                         ;
      NewTreeWidgetItem(item)                     ;
      SUID    uuid = SC . Uuid   ( 0 )            ;
      QString name = SC . String ( 1 )            ;
      item -> setData ( 0 , Qt::UserRole , uuid ) ;
      item -> setText ( 0 , name                ) ;
      addTopLevelItem ( item                    ) ;
    SqlLoopErr ( SC , Q )                         ;
    SqlLoopEnd ( SC , Q )                         ;
  LeaveSQL   ( SC , plan->sql )                   ;
  setEnabled ( true           )                   ;
  if (topLevelItemCount()<=0)                     {
    Alert ( Error )                               ;
    return true                                   ;
  }                                               ;
  QString m = tr("%1 has %2 versions"             )
              .arg ( windowTitle      ()          )
              .arg ( topLevelItemCount()        ) ;
  setToolTip ( m )                                ;
  plan->Talk ( m )                                ;
  return true                                     ;
}

void N::SourceVersions::doubleClicked(QTreeWidgetItem * item,int column)
{
  QString wv                    ;
  SUID uuid = nTreeUuid(item,0) ;
  wv  = windowTitle()+ " "      ;
  wv += item->text(0)           ;
  emit Version (uuid)           ;
  emit Version (uuid,wv)        ;
}
