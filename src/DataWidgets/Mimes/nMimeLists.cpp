#include <datawidgets.h>

N::MimeLists:: MimeLists   (QWidget * parent,Plan * p)
             : ComboBox    (          parent,       p)
             , MimeManager (                        p)
{
  WidgetClass                  ;
  setInsertPolicy ( NoInsert ) ;
}

N::MimeLists::~MimeLists (void)
{
}

void N::MimeLists::list(QString Default)
{
  blockSignals        ( true       ) ;
  clear               (            ) ;
  QComboBox::addItems ( supports() ) ;
  int index = findText( Default    ) ;
  if (index>=0)                      {
    setCurrentIndex   ( index      ) ;
  }                                  ;
  blockSignals        ( false      ) ;
}

void N::MimeLists::setEditable (bool editable)
{
  QComboBox::setEditable(editable)      ;
  if (!editable) return                 ;
  LineEdit * nle = new LineEdit(this)   ;
  setLineEdit(nle)                      ;
  connect(nle ,SIGNAL(returnPressed())  ,
          this,SLOT  (mimeFinished ())) ;
}

void N::MimeLists::mimeFinished (void)
{
  QLineEdit * le = lineEdit()        ;
  if (IsNull(le)) return             ;
  QString     mime = le->text()      ;
  QStringList ms   = mime.split("/") ;
  if (ms.count()!=2) return          ;
  int index = findText(mime)         ;
  if (index>=0) return               ;
  N::MimeType M                      ;
  ConnectSQL    ( SC,MimePlan->sql ) ;
  M = append    ( SC,mime          ) ;
  DisconnectSQL ( SC               ) ;
  if (M.mime.length()<=0) return     ;
  blockSignals ( true   )            ;
  addItem      ( M.mime )            ;
  index = findText(mime )            ;
  setCurrentIndex(index )            ;
  blockSignals ( false  )            ;
}
