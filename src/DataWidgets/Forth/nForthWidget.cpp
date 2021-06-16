#include <Exciton>

nForthWidget:: nForthWidget ( QWidget * parent , nPlan * plan )
             : nScrollArea  (           parent ,         plan )
             , Forth        ( NULL                            )
{
  Configure ( ) ;
}

nForthWidget:: nForthWidget ( QWidget * parent , nPlan * plan , bool delay )
             : nScrollArea  (           parent ,         plan              )
             , Forth        ( NULL                                         )
{
  if (!delay) Configure ( ) ;
}

nForthWidget::~nForthWidget(void)
{
}

void nForthWidget::resizeEvent(QResizeEvent * event)
{
  QScrollArea::resizeEvent ( event )  ;
  if (NotNull(Forth)) Forth->Adjust() ;
}

void nForthWidget::showEvent(QShowEvent * event)
{
  QScrollArea::showEvent ( event )    ;
  if (NotNull(Forth)) Forth->Adjust() ;
}

void nForthWidget::Configure(void)
{
  setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn ) ;
  setWidgetResizable         ( true                  ) ;
  Forth = new nForthBind     ( this , Plan           ) ;
  setWidget                  ( Forth                 ) ;
  setWindowTitle             ( tr("Embedded Forth")  ) ;
  setFocusPolicy             ( Qt::NoFocus           ) ;
}

bool nForthWidget::setForth(nForthBind * forth)
{
  setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn ) ;
  setWidgetResizable         ( true                  ) ;
  Forth = forth                                        ;
  Forth-> setParent          ( this                  ) ;
  setWidget                  ( Forth                 ) ;
  setWindowTitle             ( tr("Embedded Forth")  ) ;
  setFocusPolicy             ( Qt::NoFocus           ) ;
  return true                                          ;
}

bool nForthWidget::setUuid(SUID uuid)
{
  Forth -> Uuid = uuid                  ;
  Forth -> Load()                       ;
  Forth -> setFocus(Qt::TabFocusReason) ;
  return true                           ;
}
