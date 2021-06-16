#include <datawidgets.h>

N::SqlQuery:: SqlQuery    ( QWidget * parent , Plan * p)
            : TableWidget (           parent ,        p)
            , Limit       ( 10000                      )
            , isQuery     ( false                      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlQuery::~SqlQuery (void)
{
}

bool N::SqlQuery::FocusIn(void)
{
  LinkAction ( Refresh , Refresh() ) ;
  return true                        ;
}

void N::SqlQuery::Configure(void)
{
  setWordWrap     ( true ) ;
  plan -> setFont ( this ) ;
}

void N::SqlQuery::showColumns(QSqlRecord & r)
{
  QStringList H                ;
  nFullLoop ( n , r.count() )  {
    QSqlField f = r.field(n)   ;
    H << f.name ()             ;
  }                            ;
  setHorizontalHeaderLabels(H) ;
}

void N::SqlQuery::showValues(int index,QSqlRecord & r,QSqlQuery & q)
{
  for (int x = 0 ; x < r.count(); x ++)                                {
    QString  n = r.field(x).name()                                     ;
    QVariant V = q.value(x)                                            ;
    NewTableWidgetItem(cell)                                           ;
    switch (V.type())                                                  {
      case QVariant::Bool                                              :
        cell->setText(V.toBool() ? tr("true") : tr("false") )          ;
      break                                                            ;
      case QVariant::Char                                              :
        cell->setText(V.toChar())                                      ;
      break                                                            ;
      case QVariant::Int                                               :
        cell->setText(QString::number(V.toInt()))                      ;
      break                                                            ;
      case QVariant::UInt                                              :
        cell->setText(QString::number(V.toUInt()))                     ;
      break                                                            ;
      case QVariant::LongLong                                          :
        cell->setText(QString::number(V.toLongLong()))                 ;
      break                                                            ;
      case QVariant::ULongLong                                         :
        cell->setText(QString::number(V.toULongLong()))                ;
      break                                                            ;
      case QVariant::Double                                            :
        cell->setText(QString::number(V.toDouble()))                   ;
      break                                                            ;
      case QVariant::String                                            :
        cell->setText(V.toString())                                    ;
      break                                                            ;
      case QVariant::Date                                              :
        cell->setText(V.toDate().toString("yyyy/MM/dd"))               ;
      break                                                            ;
      case QVariant::Time                                              :
        cell->setText(V.toTime().toString("hh:mm:ss"))                 ;
      break                                                            ;
      case QVariant::DateTime                                          :
        cell->setText(V.toDateTime().toString("yyyy/MM/dd hh:mm:ss"))  ;
      break                                                            ;
      case QVariant::ByteArray                                         :
        cell->setText(QString::fromUtf8(V.toByteArray()))              ;
      break                                                            ;
      default                                                          :
        cell->setText(tr("Type = %1").arg(V.type()))                   ;
        cell->setBackground(QBrush(QColor(224,224,224)))               ;
      break                                                            ;
    }                                                                  ;
    setItem(index,x,cell)                                              ;
  }                                                                    ;
}

void N::SqlQuery::Refresh(void)
{
  nDropOut ( isQuery ) ;
  isQuery = true       ;
  start    (         ) ;
}

bool N::SqlQuery::startup(QString query)
{
  nKickOut       ( isQuery , false )                         ;
  Query = query                                              ;
  setWindowTitle ( tr("[%1] please wait ...") . arg(Query) ) ;
  isQuery = true                                             ;
  start          (                 )                         ;
  return true                                                ;
}

void N::SqlQuery::run(void)
{
  setColumnCount ( 0     )                            ;
  setRowCount    ( 0     )                            ;
  setEnabled     ( false )                            ;
  EnterSQL ( SC , plan->sql )                         ;
    if (SC.q.exec(Query))                             {
      QDateTime S = QDateTime::currentDateTime()      ;
      QSqlRecord r                                    ;
      int        total                                ;
      r     = SC.q.record ( )                         ;
      total = SC.q.size   ( )                         ;
      if (r.count()>0 && total>0)                     {
        int i = 0                                     ;
        if (total>Limit)                              {
          total = Limit                               ;
        }                                             ;
        setColumnCount ( r.count() )                  ;
        setRowCount    ( total     )                  ;
        showColumns    ( r         )                  ;
        while (i < total && SC.q.next() )             {
          showValues ( i , r , SC.q )                 ;
          i++                                         ;
        }                                             ;
      }                                               ;
      int ds = S.secsTo(QDateTime::currentDateTime()) ;
      plan->Talk(tr("%1 items").arg(total))           ;
      if (ds>10)                                      {
        QString t                                     ;
        t = tr("Query takes %1 seconds").arg(ds)      ;
        plan->Talk(t)                                 ;
      }                                               ;
      Alert ( Done  )                                 ;
    } else                                            {
      Alert ( Error )                                 ;
    }                                                 ;
  LeaveSQL ( SC , plan->sql )                         ;
  setEnabled ( true )                                 ;
  setWindowTitle ( Query )                            ;
  isQuery = false                                     ;
}

bool N::SqlQuery::Menu(QPoint pos)
{
  QTableWidgetItem * cell                ;
  MenuManager mm(this)                   ;
  QMenu      * mo                        ;
  QMenu      * me                        ;
  QAction    * aa                        ;
  cell = itemAt(pos)                     ;
  me = mm.addMenu(tr("Edit"           )) ;
  mo = mm.addMenu(tr("Organize"       )) ;
  mm.add(me,101,tr("Query again"      )) ;
  mm.add(me,102,tr("set maximum items")) ;
  mm.add(mo,901,tr("Resize columns"   )) ;
  mm.add(mo,902,tr("Resize rows"      )) ;
  mm.setFont(plan)                       ;
  aa = mm.exec()                         ;
  if (IsNull(aa)) return true            ;
  switch (mm[aa])                        {
    case 101                             :
      Refresh ( )                        ;
    break                                ;
    case 102                           : {
      bool okay = false                  ;
      int  r    = 0                      ;
      r = QInputDialog::getInt           (
            this                         ,
            tr("Maximum items")          ,
            tr("Limit"        )          ,
            Limit                        ,
            1                            ,
            1000000                      ,
            1                            ,
            &okay                      ) ;
      if (okay) Limit = r                ;
    }                                    ;
    break                                ;
    case 901                             :
      resizeColumnsToContents ()         ;
    break                                ;
    case 902                             :
      resizeRowsToContents    ()         ;
    break                                ;
  }                                      ;
  return true                            ;
}
