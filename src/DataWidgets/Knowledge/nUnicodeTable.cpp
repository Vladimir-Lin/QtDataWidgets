#include <datawidgets.h>

N::UnicodeTable:: UnicodeTable (QWidget * parent,Plan * p)
                : TableWidget  (          parent,       p)
                , Factor       (1.0                      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::UnicodeTable::~UnicodeTable (void)
{
}

QSize N::UnicodeTable::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize (  640 ,  480 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::UnicodeTable::Configure(void)
{
  QFont f = plan -> fonts [ Fonts::Editor ]              ;
  f . setPixelSize  ( 24                               ) ;
  setAccessibleName ( "UnicodeTable"                   ) ;
  setWindowTitle    ( tr    ( "Unicode table editor" ) ) ;
  setWindowIcon     ( QIcon ( ":/images/unicode.png" ) ) ;
  assignFont        ( f                                ) ;
  ////////////////////////////////////////////////////////
  nConnect          ( this , SIGNAL ( AutoFit  ( ) )     ,
                      this , SLOT   ( BestSize ( ) )   ) ;
}

bool N::UnicodeTable::FocusIn(void)
{
  AssignAction ( Label   , tr("Unicode table editor") ) ;
  LinkAction   ( Font    , selectFont ( )             ) ;
  LinkAction   ( Export  , Export     ( )             ) ;
  LinkAction   ( Refresh , startup    ( )             ) ;
  return true                                           ;
}

void N::UnicodeTable::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( Type ) {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

void N::UnicodeTable::load(Qt::ItemFlags flags)
{
  QTableWidgetItem * cell                ;
  ////////////////////////////////////////
  int total   = 0x110000                 ;
  int columns = 16                       ;
  int rows    = total / columns          ;
  CellFlags   = flags                    ;
  clear          (         )             ;
  setRowCount    ( rows    )             ;
  setColumnCount ( columns )             ;
  ////////////////////////////////////////
  for (int i=0;i<columns;i++)            {
    QString v = QString::number(i,16)    ;
    cell = new QTableWidgetItem()        ;
    cell->setText(v.toUpper())           ;
    setHorizontalHeaderItem(i,cell)      ;
  }                                      ;
  ////////////////////////////////////////
  for (int i=0;i<total;i+=columns)       {
    QString v = ""                       ;
    int     V = i                        ;
    for (int j=0;j<6;j++)                {
      v = QString::number(V%16,16) + v   ;
      V /= 16                            ;
    }                                    ;
    v = v.toUpper()                      ;
    cell = new QTableWidgetItem()        ;
    cell->setText(v)                     ;
    setVerticalHeaderItem(i/columns,cell);
  }                                      ;
  ////////////////////////////////////////
  start ( 10001 )                        ;
}

void N::UnicodeTable::startup(void)
{
  load ( ) ;
}

void N::UnicodeTable::List(void)
{
  QTableWidgetItem * cell                  ;
  int total   = 0x110000                   ;
  int columns = 16                         ;
  setEnabled ( false )                     ;
  //////////////////////////////////////////
  for (int i=1;i<total;i++)                {
    QString v = toUtf8(i)                  ;
    QString T = ""                         ;
    int     V = i                          ;
    for (int j=0;j<4;j++)                  {
      T = QString::number(V%16,16) + T     ;
      V /= 16                              ;
    }                                      ;
    ////////////////////////////////////////
    cell  = new QTableWidgetItem()         ;
    cell -> setText    ( v               ) ;
    cell -> setToolTip ( T.toUpper()     ) ;
    cell -> setData    ( Qt::UserRole,i  ) ;
    cell -> setFlags   ( CellFlags       ) ;
    emit putItem(i/columns,i%columns,cell) ;
  }                                        ;
  //////////////////////////////////////////
  setEnabled   ( true  )                   ;
  emit AutoFit (       )                   ;
}

void N::UnicodeTable::BestSize(void)
{
  resizeColumnsToContents ( ) ;
  resizeRowsToContents    ( ) ;
}

void N::UnicodeTable::assignFont(QFont font)
{
  Font   = font   ;
  Factor = 1.0    ;
  fontChanged ( ) ;
}

void N::UnicodeTable::selectFont(void)
{
  bool  okay = false                        ;
  QFont font = QFontDialog :: getFont       (
                 &okay                      ,
                 Font                       ,
                 this                       ,
                 tr("Unicode table font") ) ;
  if (!okay) return                         ;
  Font = font                               ;
  fontChanged ( )                           ;
}

qreal N::UnicodeTable::FactorLevel(qreal factor,bool enlarge)
{
  qreal f = factor;
  int   F = (int)(f * 100);
  bool  B = false;
  QList<int> Fs;
  Fs <<  10 ;
  Fs <<  20 ;
  Fs <<  30 ;
  Fs <<  40 ;
  Fs <<  50 ;
  Fs <<  60 ;
  Fs <<  70 ;
  Fs <<  80 ;
  Fs <<  90 ;
  Fs << 100 ;
  Fs << 110 ;
  Fs << 120 ;
  Fs << 130 ;
  Fs << 140 ;
  Fs << 150 ;
  Fs << 160 ;
  Fs << 170 ;
  Fs << 180 ;
  Fs << 190 ;
  Fs << 200 ;
  Fs << 220 ;
  Fs << 240 ;
  Fs << 260 ;
  Fs << 280 ;
  Fs << 300 ;
  Fs << 350 ;
  Fs << 400 ;
  Fs << 500 ;
  Fs << 600 ;
  Fs << 700 ;
  Fs << 800 ;
  for (int i=1;!B && i<Fs.count();i++) {
    if (Fs[i-1]==F) {
      if (enlarge) {
        F = Fs[i];
        B = true;
      } else {
        if (i>1) F = Fs[i-2]; else F = Fs[0];
        B = true;
      };
    } else
    if (Fs[i]==F) {
      if (enlarge) {
        F = Fs[i+1];
        B = true;
      } else {
        if (i>0) F = Fs[i-1]; else F = Fs[0];
        B = true;
      };
    } else
    if (Fs[i-1]<F && F<Fs[i]) {
      if (enlarge) {
        F = Fs[i+1];
        B = true;
      } else {
        if (i>0) F = Fs[i-1]; else F = Fs[0];
        B = true;
      };
    };
  };
  f = F; f /= 100;
  return f;
}

void N::UnicodeTable::ZoomIn(void)
{
  Factor = FactorLevel ( Factor , true  ) ;
  fontChanged          (                ) ;
}

void N::UnicodeTable::ZoomOut(void)
{
  Factor = FactorLevel ( Factor , false ) ;
  fontChanged          (                ) ;
}

void N::UnicodeTable::fontChanged(void)
{
  qreal p = Font . pointSizeF (   ) ;
  QFont F = Font                    ;
  p *= Factor                       ;
  F  .setPointSizeF           ( p ) ;
  QTableWidget::setFont       ( F ) ;
  resizeColumnsToContents     (   ) ;
  resizeRowsToContents        (   ) ;
}

void N::UnicodeTable::Export(void)
{
  QString filename = QFileDialog::getSaveFileName  (
                       this                        ,
                       tr("Export unicode table")  ,
                       plan->Temporary("")         ,
                       "*.txt"                   ) ;
  if (filename.length()<=0) return                 ;
  QFile F(filename)                                ;
  if (!F.open(QIODevice::WriteOnly)) return        ;
  for (int i=32;i<0x110000;i++)                    {
    QString v = toUtf8(i)                          ;
    QString T = ""                                 ;
    int     V = i                                  ;
    for (int j=0;j<6;j++)                          {
      T = QString::number(V%16,16) + T             ;
      V /= 16                                      ;
    }                                              ;
    T = T.toUpper()                                ;
    QString O = QString("%1 %2\r\n").arg(T).arg(v) ;
    F.write(O.toUtf8())                            ;
  }                                                ;
  F.close();
}

bool N::UnicodeTable::Menu(QPoint pos)
{
  return true ;
}

int N::UnicodeTable::toUtf8(uint32_t character,unsigned char * utf8)
{
  if ( NULL     == utf8         ) return 0 ;
  if ( character > 0x0010FFFFul ) return 0 ;
  //////////////////////////////////////////
  if ( character < 0x00000080ul )          {
    *utf8 = (unsigned char)character       ;
    return 1                               ;
  }                                        ;
  //////////////////////////////////////////
  if ( character < 0x00000800ul )          {
    int u0 = character                     ;
    int u1 = character                     ;
    u0 >>= 6                               ;
    u0  |= 0xC0                            ;
    u1  &= 0x3F                            ;
    u1  |= 0x80                            ;
    *utf8 = (unsigned char)u0              ;
    utf8++                                 ;
    *utf8 = (unsigned char)u1              ;
    return 2                               ;
  }                                        ;
  //////////////////////////////////////////
  if ( character > 0x0000FFFFul )          {
    int u0 = character                     ;
    int u1 = character                     ;
    int u2 = character                     ;
    int u3 = character                     ;
    u3  &= 0x3F                            ;
    u3  |= 0x80                            ;
    u2 >>= 6                               ;
    u2  &= 0x3F                            ;
    u2  |= 0x80                            ;
    u1 >>= 12                              ;
    u1  &= 0x3F                            ;
    u1  |= 0x80                            ;
    u0 >>= 18                              ;
    u0  &= 0x07                            ;
    u0  |= 0xF0                            ;
    *utf8 = (unsigned char)u0              ;
    utf8++                                 ;
    *utf8 = (unsigned char)u1              ;
    utf8++                                 ;
    *utf8 = (unsigned char)u2              ;
    utf8++                                 ;
    *utf8 = (unsigned char)u3              ;
    return 4                               ;
  }                                        ;
  //////////////////////////////////////////
  if ( character > 0x0000D7FF             &&
       character < 0x0000E000            ) {
    return 0                               ;
  }                                        ;
  //////////////////////////////////////////
  int u0 = character                       ;
  int u1 = character                       ;
  int u2 = character                       ;
  u2  &= 0x3F                              ;
  u2  |= 0x80                              ;
  u1 >>= 6                                 ;
  u1  &= 0x3F                              ;
  u1  |= 0x80                              ;
  u0 >>= 12                                ;
  u0  &= 0x0F                              ;
  u0  |= 0xE0                              ;
  *utf8 = (unsigned char)u0                ;
  utf8++                                   ;
  *utf8 = (unsigned char)u1                ;
  utf8++                                   ;
  *utf8 = (unsigned char)u2                ;
  //////////////////////////////////////////
  return 3                                 ;
}

QString N::UnicodeTable::toUtf8(uint32_t character)
{
  unsigned char utf8 [ 16 ]         ;
  int ulen                          ;
  QByteArray B                      ;
  ulen = toUtf8(character,utf8)     ;
  B.append((const char *)utf8,ulen) ;
  return QString::fromUtf8(B)       ;
}
