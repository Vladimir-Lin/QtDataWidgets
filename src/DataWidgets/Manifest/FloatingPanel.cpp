#include <datawidgets.h>

N::FloatingPanel:: FloatingPanel (QWidget * parent,Plan * p)
                 : Widget        (          parent,       p)
                 , uuid          (0                        )
                 , type          (0                        )
                 , Fading        (false                    )
{
  WidgetClass        ;
  Frameless ( this ) ;
}

N::FloatingPanel::~FloatingPanel (void)
{
}

void N::FloatingPanel::focusOutEvent(QFocusEvent * event)
{
  if (event->lostFocus())              {
    emit Finished          ( )         ;
    plan  -> processEvents ( )         ;
    event -> accept        ( )         ;
  } else QWidget::focusOutEvent(event) ;
}

void N::FloatingPanel::FadeOut(void)
{
  Fading = true                      ;
  AbstractGui::FadeOut ( this , 10 ) ;
  hide                 (           ) ;
  Fading = false                     ;
}

void N::FloatingPanel::clear(void)
{
  for (int i=0;i<buttons.count();i++) {
    buttons[i]->deleteLater()         ;
  }                                   ;
  for (int i=0;i<actions.count();i++) {
    actions[i]->deleteLater()         ;
  }                                   ;
  uuid       = 0                      ;
  type       = 0                      ;
  buttons    . clear ( )              ;
  actions    . clear ( )              ;
  CatalogMap . clear ( )              ;
}

void N::FloatingPanel::assign(SUID u,int t,QRect Rect,int user)
{
  int  total = 0                                  ;
  int  items = 0                                  ;
  clear ()                                        ;
  uuid       = u                                  ;
  type       = t                                  ;
  SqlConnection SC(plan->sql)                     ;
  if (SC.open("BranonFloating","assign"))         {
    UUIDs Uuids                                   ;
    SUID  U                                       ;
    QString Q                                     ;
    Q = plan->sql.SelectFrom                      (
          "count(*)"                              ,
          "branonpanel"                           ,
          QString("where branon = %1").arg(uuid)) ;
    if (SC.Fetch(Q))                              {
      total = SC.Value(0).toInt()                 ;
    }                                             ;
    Uuids . clear()                               ;
    Uuids = SC . Uuids                            (
              "branonpanel"                       ,
              "execution"                         ,
              QString("where branon = %1 order by position asc").arg(u)) ;
    foreach (U,Uuids)                             {
      Execution ne                                ;
      ne.clear()                                  ;
      ne.Load(plan,SC,U)                          ;
      add(U,ne.Name,ne.Icon)                      ;
    }                                             ;
    SC.close()                                    ;
  }                                               ;
  SC.remove()                                     ;
  items = total                                   ;
  if (type==2) items += user                      ;
  int y = Rect.top() - 4                          ;
  int h = (items/6)                               ;
  if ((items%6)>0) h++                            ;
  h *= 64                                         ;
  y -= h                                          ;
  QRect V                                         ;
  V.setLeft   (Rect.left ())                      ;
  V.setTop    (y           )                      ;
  V.setWidth  (Rect.width())                      ;
  V.setHeight (h           )                      ;
  setGeometry (V)                                 ;
}

void N::FloatingPanel::Arrangement(void)
{
  display()                    ;
  FadeIn(this,10)              ;
  setFocus(Qt::TabFocusReason) ;
}

void N::FloatingPanel::display(void)
{
  for (int i=0;i<buttons.count();i++) {
    int x = i % 6 ; x *= 64           ;
    int y = i / 6 ; y *= 64           ;
    QRect R(x,y,64,64)                ;
    buttons[i]->setGeometry(R)        ;
  }                                   ;
}

void N::FloatingPanel::add(SUID Id,QString text,QIcon icon)
{
  QToolButton * T = new QToolButton (this)  ;
  QAction     * A = new QAction     (T   )  ;
  T -> setIconSize      (QSize(64,64))      ;
  T -> setAutoRaise     (true        )      ;
  A -> setIcon          (icon        )      ;
  A -> setToolTip       (text        )      ;
  T -> setDefaultAction (A           )      ;
  buttons       << T                        ;
  actions       << A                        ;
  CatalogMap[A ] = Id                       ;
  connect(T   ,SIGNAL(triggered(QAction*))  ,
          this,SLOT  (activate (QAction*))) ;
}

void N::FloatingPanel::activate(QAction * action)
{
  emit Execute(CatalogMap[action],action->toolTip()) ;
}
