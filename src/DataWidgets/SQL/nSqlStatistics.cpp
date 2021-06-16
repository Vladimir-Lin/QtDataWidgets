#include <datawidgets.h>
#include "ui_nSqlStatistics.h"

N::SqlStatistics:: SqlStatistics ( QWidget * parent , Plan * p )
                 : Widget        (           parent ,        p )
                 , ui            ( new Ui::nSqlStatistics      )
                 , DefaultScheme ( ""                          )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlStatistics::~SqlStatistics (void)
{
  delete ui ;
}

void N::SqlStatistics::Configure(void)
{
  NewTreeWidgetItem               ( head                 ) ;
  head -> setText                 ( 0 , tr("Table")      ) ;
  head -> setText                 ( 1 , tr("Statistics") ) ;
  head -> setText                 ( 2 , ""               ) ;
  ui   -> setupUi                 ( this                 ) ;
  ui   -> Tables -> setHeaderItem ( head                 ) ;
  setWindowTitle                  ( tr("SQL Statistics") ) ;
  plan -> setFont                 ( this                 ) ;
  FocusIn                         (                      ) ;
}

void N::SqlStatistics::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QWidget::focusInEvent (event) ;
}

void N::SqlStatistics::focusOutEvent(QFocusEvent * event)
{
  QWidget::focusOutEvent (event) ;
}

void N::SqlStatistics::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event) ;
  Relocation() ;
}

void N::SqlStatistics::showEvent(QShowEvent * event)
{
  QWidget::showEvent(event) ;
  Relocation() ;
}

void N::SqlStatistics::Relocation(void)
{
  ui -> Schemes -> resize ( width () , ui->Schemes->height()    ) ;
  ui -> Tables  -> resize ( width () , height()-ui->Tables->y() ) ;
}

bool N::SqlStatistics::FocusIn(void)
{
  LinkAction(Refresh,startup()) ;
  LinkAction(Copy   ,Copy   ()) ;
  return true                   ;
}

bool N::SqlStatistics::FocusOut(void)
{
  return true ;
}

bool N::SqlStatistics::startup(void)
{
  if (running==1) return true                    ;
  ui -> Schemes -> clear (  )                    ;
  ui -> Tables  -> clear (  )                    ;
  EnterSQL ( SC , plan->sql )                    ;
    int     index = -1                           ;
    QString Q                                    ;
    Q = "show databases ;"                       ;
    ui->Schemes->blockSignals ( true  )          ;
    SqlLoopNow ( SC , Q )                        ;
      QString S = SC.String(0)                   ;
      ui -> Schemes -> addItem ( S )             ;
    SqlLoopErr ( SC , Q )                        ;
    SqlLoopEnd ( SC , Q )                        ;
    if (DefaultScheme.length()<=0)               {
      DefaultScheme = SC.sql.dbName              ;
    }                                            ;
    index = ui->Schemes->findText(DefaultScheme) ;
    if (index>=0)                                {
      ui->Schemes->setCurrentIndex(index)        ;
    }                                            ;
    ui->Schemes->blockSignals ( false )          ;
    plan -> processEvents ( )                    ;
  LeaveSQL ( SC , plan->sql )                    ;
  return true                                    ;
}

void N::SqlStatistics::Copy(void)
{
  QStringList S                                          ;
  for (int i=0;i<ui->Tables->topLevelItemCount();i++)    {
    QTreeWidgetItem * item = ui->Tables->topLevelItem(i) ;
    if (item->isSelected()) S << item->text(0)           ;
  }                                                      ;
  if (S.count()<=0) return                               ;
  QString text = S.join("\n")                            ;
  qApp -> clipboard() -> setText (text)                  ;
  Alert ( Done )                                         ;
}

void N::SqlStatistics::schemeChanged(int index)
{
  if (running==1) return                           ;
  DefaultScheme = ui -> Schemes -> currentText ( ) ;
  EnterSQL ( SC , plan->sql )                      ;
    CountSql ( SC , DefaultScheme )                ;
  LeaveSQL ( SC , plan->sql )                      ;
  start ( )                                        ;
}

void N::SqlStatistics::CountSql(SqlConnection & SC,QString Scheme)
{
  QString     Q                                 ;
  DefaultScheme = Scheme                        ;
  ui -> Tables  -> clear (  )                   ;
  Q = QString("use %1 ;").arg(Scheme)           ;
  SC . Query (Q                     )           ;
  Q = "show tables ;"                           ;
  SqlLoopNow ( SC , Q )                         ;
    NewTreeWidgetItem(item)                     ;
    QString S = SC.String(0)                    ;
    item->setText(0,S)                          ;
    item->setTextAlignment(1,Qt::AlignRight     |
                             Qt::AlignVCenter ) ;
    ui->Tables->addTopLevelItem(item)           ;
  SqlLoopErr ( SC , Q )                         ;
  SqlLoopEnd ( SC , Q )                         ;
  plan->processEvents()                         ;
  for (int i=0;i<ui->Tables->columnCount();i++) {
    ui->Tables->resizeColumnToContents(i)       ;
  }                                             ;
  ui->Tables->setToolTip                        (
    tr("%1 tables"                              )
    .arg(ui->Tables->topLevelItemCount())     ) ;
  plan->processEvents()                         ;
}

void N::SqlStatistics::run(void)
{
  QString Q                                             ;
  SqlConnection SC(plan->sql)                           ;
  if (SC.open("nSqlStatistics","run"))                  {
    QDateTime Now = QDateTime::currentDateTime()        ;
    Q = QString("use %1 ;").arg(DefaultScheme)          ;
    SC.Query(Q)                                         ;
    for (int i=0;i<ui->Tables->topLevelItemCount();i++) {
      QTreeWidgetItem * item                            ;
      item = ui->Tables->topLevelItem(i)                ;
      QString S    = item->text(0)                      ;
      int    total = 0                                  ;
      Now = QDateTime::currentDateTime()                ;
      ui->Tables->setCurrentItem(item)                  ;
      Q = QString("select count(*) from %1 ;").arg(S)   ;
      if (SC.Fetch(Q)) total = SC.Int(0)                ;
      item->setText(1,QString::number(total))           ;
      if (Now.msecsTo(QDateTime::currentDateTime())<25) {
        Time::msleep(15)                                ;
      }                                                 ;
    }                                                   ;
    SC.close()                                          ;
  }                                                     ;
  SC.remove()                                           ;
  Alert ( Done )                                        ;
}
