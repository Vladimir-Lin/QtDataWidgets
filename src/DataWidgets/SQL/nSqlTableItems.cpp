#include <datawidgets.h>

N::SqlTableItems:: SqlTableItems ( QWidget * parent , Plan * p )
                 : TreeWidget    (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlTableItems::~SqlTableItems (void)
{
}

QSize N::SqlTableItems::SuitableSize(void)
{
  QSize  s = size()             ;
  CUIDs  w = columnWidths()     ;
  int    t = 0                  ;
  if (w.count()<=0) return s    ;
  for (int i=0;i<w.count();i++) {
    t += w[i]                   ;
  }                             ;
  s . setWidth ( t )            ;
  ///////////////////////////////
  int idx = topLevelItemCount() ;
  idx = idx - 1                 ;
  if (idx<0) return s           ;
  QTreeWidgetItem * item        ;
  item = topLevelItem(idx)      ;
  QRect R                       ;
  R = visualItemRect (item)     ;
  if (s.height()>R.bottom())    {
    s.setHeight(R.bottom())     ;
  }                             ;
  return s                      ;
}

void N::SqlTableItems::Configure(void)
{
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("Field"  )     ) ;
  head -> setText              ( 1 , tr("Type"   )     ) ;
  head -> setText              ( 2 , tr("Null"   )     ) ;
  head -> setText              ( 3 , tr("Key"    )     ) ;
  head -> setText              ( 4 , tr("Default")     ) ;
  head -> setText              ( 5 , tr("Extra"  )     ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setColumnCount               ( 6                     ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  assignHeaderItems            ( head                  ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::SqlTableItems::load(QString database,QString table)
{
  setWindowTitle (tr("Table %1.%2").arg(database).arg(table)) ;
  clear          (                                          ) ;
  Sql sql    = plan->sql                                      ;
  sql.dbName = database                                       ;
  EnterSQL ( SC , sql )                                       ;
    QString Q = QString("describe %1 ;").arg(table)           ;
    SqlLoopNow(SC,Q)                                          ;
      NewTreeWidgetItem (IT            )                      ;
      IT -> setText     (0,SC.String(0))                      ;
      IT -> setText     (1,SC.String(1))                      ;
      IT -> setText     (2,SC.String(2))                      ;
      IT -> setText     (3,SC.String(3))                      ;
      IT -> setText     (4,SC.String(4))                      ;
      IT -> setText     (5,SC.String(5))                      ;
      addTopLevelItem   (IT            )                      ;
    SqlLoopErr(SC,Q)                                          ;
    SqlLoopEnd(SC,Q)                                          ;
  LeaveSQL ( SC , sql )                                       ;
  for (int i=0;i<5;i++) resizeColumnToContents ( i )          ;
  update ( )                                                  ;
  plan -> processEvents()                                     ;
  QSize ss = SuitableSize()                                   ;
  emit Adjustment ( this , ss )                               ;
  return true                                                 ;
}
