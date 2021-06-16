#include <datawidgets.h>

N::ParagraphEditor:: ParagraphEditor (QWidget * parent,Plan * p)
                   : TreeWidget      (          parent,       p)
                   , Paragraphs      ( 0                       )
                   , StartId         ( 0                       )
                   , Items           (-1                       )
                   , valueAt         (NULL                     )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ParagraphEditor::~ParagraphEditor (void)
{
}

void N::ParagraphEditor::Organize(void)
{
  for (int i=0;i<columnCount();i++) {
    resizeColumnToContents(i)       ;
  }                                 ;
}

void N::ParagraphEditor::Configure(void)
{
  NewTreeWidgetItem       (head             )                    ;
  head   -> setText       (0,tr("Id"      ) )                    ;
  head   -> setText       (1,tr("UUID")     )                    ;
  head   -> setText       (2,tr("Language") )                    ;
  head   -> setText       (3,tr("Paragraph"))                    ;
  setFocusPolicy          (Qt::WheelFocus   )                    ;
  setDragDropMode         (NoDragDrop       )                    ;
  setAlternatingRowColors (true             )                    ;
  setRootIsDecorated      (false            )                    ;
  setSelectionMode        (ExtendedSelection)                    ;
  setColumnCount          (4                )                    ;
  setHeaderItem           (head             )                    ;
  setWindowTitle          (tr("Paragraphs") )                    ;
  plan -> setFont         (this             )                    ;
  QHeaderView * hv = header()                                    ;
  connect(hv  ,SIGNAL(sectionPressed      (int))                 ,
          this,SLOT  (sectionPressed      (int)))                ;
  connect(hv  ,SIGNAL(sectionDoubleClicked(int))                 ,
          this,SLOT  (sectionDoubleClicked(int)))                ;
  connect(this,SIGNAL(itemClicked      (QTreeWidgetItem*,int))   ,
          this,SLOT  (singleClicked    (QTreeWidgetItem*,int)) ) ;
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
  addShortcut(Qt::Key_Home    ,this,SLOT(Home    ()))            ;
  addShortcut(Qt::Key_End     ,this,SLOT(End     ()))            ;
  addShortcut(Qt::Key_PageUp  ,this,SLOT(PageUp  ()))            ;
  addShortcut(Qt::Key_PageDown,this,SLOT(PageDown()))            ;
}

bool N::ParagraphEditor::startup(int startId,int items)
{
  StartId = startId                                ;
  Items   = items                                  ;
  if (Items<0) Items = Page()                      ;
  Alert ( Click )                                  ;
  AbstractGui::Mutex.lock  ()                      ;
  clear       ()                                   ;
  setEnabled (false)                               ;
  Sql sql = plan->sql                              ;
  SqlConnection SC(sql)                            ;
  if (SC.open("nParagraphEditor","startup"))       {
    QString Q                                      ;
    Paragraphs = Total(SC)                         ;
    Q = sql.SelectFrom                             (
          "id,uuid,language,name"                  ,
          PlanTable(Names)                         ,
          QString("order by id asc limit %1,%2"    )
          .arg(StartId).arg(Items)            )    ;
    SqlLoopNow(SC,Q)                               ;
      int     id       = SC.Value (0).toInt()      ;
      SUID    uuid     = SC.Uuid  (1)              ;
      int     language = SC.Value (2).toInt()      ;
      QString name     = SC.String(3)              ;
      NewTreeWidgetItem (IT)                       ;
      IT->setData(0,Qt::UserRole,id      )         ;
      IT->setData(1,Qt::UserRole,uuid    )         ;
      IT->setData(2,Qt::UserRole,language)         ;
      IT->setText(0,QString::number(id)  )         ;
      IT->setText(1,QString::number(uuid))         ;
      IT->setText(2,plan->languages[language])     ;
      IT->setText(3,name                     )     ;
      addTopLevelItem   (IT)                       ;
    SqlLoopErr(SC,Q)                               ;
    SqlLoopEnd(SC,Q)                               ;
    SC.close()                                     ;
  }                                                ;
  SC.remove()                                      ;
  setEnabled (true)                                ;
  AbstractGui::Mutex.unlock()                      ;
  setToolTip(tr("Total %1 items").arg(Paragraphs)) ;
  Organize ()                                      ;
  Alert ( Done )                                   ;
  return true                                      ;
}

int N::ParagraphEditor::Total(SqlConnection & Connection)
{
  int total = 0                                                ;
  QString Q = Connection.sql.SelectFrom                        (
                "count(*)"                                     ,
                PlanTable(Names)                               ,
                "")                                            ;
  if (Connection.Fetch(Q)) total = Connection.Value(0).toInt() ;
  return total                                                 ;
}

int N::ParagraphEditor::Page(void)
{
  QSize s    = size()            ;
  QFont f    = font()            ;
  int   page = -1                ;
  page       = s.height()        ;
  page      /= (f.pixelSize()+3) ;
  return page                    ;
}

void N::ParagraphEditor::sectionPressed(int index)
{
  if (NotNull(valueAt))      {
    valueAt -> deleteLater() ;
    valueAt  = NULL          ;
  }                          ;
}

void N::ParagraphEditor::sectionDoubleClicked(int index)
{
  QHeaderView * hv = header()                                 ;
  if (IsNull(hv)) return                                      ;
  if (NotNull(valueAt))                                       {
    valueAt -> deleteLater()                                  ;
    valueAt  = NULL                                           ;
  }                                                           ;
  QSize s = hv->size()                                        ;
  QRect r                                                     ;
  switch (index)                                              {
    case 0                                                    :
      r.setLeft(0)                                            ;
      r.setTop(0)                                             ;
      r.setWidth(hv->sectionSize(0))                          ;
      r.setHeight(s.height())                                 ;
      valueAt = new SpinBox(hv,plan)                          ;
      valueAt->setRange(0,Paragraphs)                         ;
      valueAt->setValue(StartId)                              ;
      valueAt->setGeometry(r)                                 ;
      valueAt->setToolTip(tr("Start"))                        ;
      valueAt->show()                                         ;
      valueAt->setFocus(Qt::TabFocusReason)                   ;
      connect(valueAt,SIGNAL(editingFinished()),this,SLOT(startFinished()));
    break                                                     ;
    case 1                                                    :
      r.setLeft(hv->sectionSize(0))                           ;
      r.setTop(0)                                             ;
      r.setWidth(hv->sectionSize(1))                          ;
      r.setHeight(s.height())                                 ;
      valueAt = new SpinBox(hv,plan)                          ;
      valueAt->setRange(0,Paragraphs)                         ;
      valueAt->setValue(Items)                                ;
      valueAt->setGeometry(r)                                 ;
      valueAt->setToolTip(tr("Max %1 items").arg(Paragraphs)) ;
      valueAt->show()                                         ;
      valueAt->setFocus(Qt::TabFocusReason)                   ;
      connect(valueAt,SIGNAL(editingFinished()),this,SLOT(numberFinished()));
    break                                                     ;
  }                                                           ;
}

void N::ParagraphEditor::startFinished(void)
{
  if (IsNull(valueAt)) return  ;
  int value = valueAt->value() ;
  valueAt -> deleteLater()     ;
  valueAt  = NULL              ;
  if (value<0) return          ;
  startup(value,Items)         ;
}

void N::ParagraphEditor::numberFinished(void)
{
  if (IsNull(valueAt)) return  ;
  int value = valueAt->value() ;
  valueAt -> deleteLater()     ;
  valueAt  = NULL              ;
  if (value<=0) return         ;
  startup(StartId,value)       ;
}

void N::ParagraphEditor::Home(void)
{
  startup(0,Items) ;
}

void N::ParagraphEditor::End(void)
{
  int end = Paragraphs - Items ;
  if (end<0) end = 0 ;
  startup(end,Items) ;
}

void N::ParagraphEditor::PageUp(void)
{
  int page = StartId - Items ;
  if (page<0) page = 0       ;
  startup(page,Items)        ;
}

void N::ParagraphEditor::PageDown(void)
{
  int page = StartId    + Items ;
  int end  = Paragraphs - Items ;
  if (end<0) end = 0            ;
  if (page>end) page = end      ;
  startup(page,Items)           ;
}

void N::ParagraphEditor::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return          ;
  removeItemWidget(ItemEditing,ItemColumn) ;
  ItemEditing = NULL                       ;
  ItemWidget  = NULL                       ;
  ItemColumn  = -1                         ;
}

void N::ParagraphEditor::singleClicked(QTreeWidgetItem * item,int column)
{
  if (ItemEditing==item) return ;
  removeOldItem()               ;
}

void N::ParagraphEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  removeOldItem()                                      ;
  QLineEdit * Line                                     ;
  QComboBox * Combo                                    ;
  switch (column)                                      {
    case 0                                             :
    break                                              ;
    case 1                                             :
    break                                              ;
    case 2                                             :
      Combo = new QComboBox (this)                     ;
      Combo << plan->languages                         ;
      (*Combo) <= item->data(2,Qt::UserRole).toInt()   ;
      setItemWidget(item,column,Combo)                 ;
      ItemEditing = item                               ;
      ItemColumn  = column                             ;
      ItemWidget  = Combo                              ;
      connect(Combo,SIGNAL(activated(int)),this,SLOT(languageIndex(int)));
      Combo->showPopup()                               ;
    break                                              ;
    case 3                                             :
      Line = new QLineEdit(this)                       ;
      Line->setText(item->text(3))                     ;
      setItemWidget(item,column,Line)                  ;
      ItemEditing = item                               ;
      ItemColumn  = column                             ;
      ItemWidget  = Line                               ;
      connect(Line,SIGNAL(editingFinished()),this,SLOT(editingFinished()));
      Line->setFocus(Qt::TabFocusReason)               ;
    break                                              ;
  }                                                    ;
}

void N::ParagraphEditor::languageIndex(int index)
{
  int id        = ItemEditing->data(0,Qt::UserRole).toInt() ;
  int language  = -1                                        ;
  QComboBox * combo = qobject_cast<QComboBox *>(ItemWidget) ;
  if (NotNull(combo)) language = combo->itemData(combo->currentIndex(),Qt::UserRole).toInt() ;
  if (id<0) return                                          ;
  if (language<0) return                                    ;
  Sql sql = plan->sql                                       ;
  SqlConnection SC(sql)                                     ;
  if (SC.open("nParagraphEditor","languageIndex"))          {
    QString Q                                               ;
    Q = sql.Update(PlanTable(Names),"where id = :ID",1,"language") ;
    SC.Prepare (Q                  )                        ;
    SC.Bind    ("id",id            )                        ;
    SC.Bind    ("language",language)                        ;
    SC.Exec    (                   )                        ;
    SC.close   (                   )                        ;
  }                                                         ;
  SC.remove()                                               ;
  if (language>0)                                           {
    ItemEditing->setData(2,Qt::UserRole,language)           ;
    ItemEditing->setText(2,plan->languages[language])       ;
  }                                                         ;
  removeOldItem ()                                          ;
}

void N::ParagraphEditor::editingFinished(void)
{
  int     id        = ItemEditing->data(0,Qt::UserRole).toInt() ;
  QString paragraph = ""                                        ;
  QLineEdit * name  = qobject_cast<QLineEdit *>(ItemWidget)     ;
  if (NotNull(name)) paragraph = name->text()                   ;
  if (id<0) return                                              ;
  if (IsNull(name)) return                                      ;
  Sql sql = plan->sql                                           ;
  SqlConnection SC(sql)                                         ;
  if (SC.open("nParagraphEditor","editingFinished"))            {
    QString Q                                                   ;
    Q = sql.Update(PlanTable(Names),"where id = :ID",1,"name")  ;
    SC.Prepare (Q                        )                      ;
    SC.Bind    ("id"  ,id                )                      ;
    SC.Bind    ("name",paragraph.toUtf8())                      ;
    SC.Exec    (                         )                      ;
    SC.close()                                                  ;
  }                                                             ;
  SC.remove()                                                   ;
  if (id>=0) ItemEditing->setText(3,paragraph)                  ;
  removeOldItem ()                                              ;
}
