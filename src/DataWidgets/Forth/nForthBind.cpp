#include <Exciton>

nForthBind:: nForthBind  ( QScrollArea * parent ,nPlan * plan )
           : nWidget     (               parent ,        plan )
           , nForth      (                                    )
           , SqlSync     ( true                               )
           , isToolTip   ( true                               )
           , Forthing    ( false                              )
           , DoingForth  ( false                              )
           , Verbose     ( 10                                 )
           , Mode        ( 1                                  )
{
  scrollArea = parent ;
  Configure ( )       ;
}

nForthBind::~nForthBind(void)
{
}

bool nForthBind::FocusIn(void)
{
  LinkAction ( Save , Save    () ) ;
  LinkAction ( Copy , Copy    () ) ;
  LinkAction ( Font , setFont () ) ;
  Alert      ( Click             ) ;
  return true                      ;
}

void nForthBind::Configure(void)
{
  Plan->setFont(this)                                                     ;
  /////////////////////////////////////////////////////////////////////////
  PrevRect      . setRect(0,0,0,0)                                        ;
  blink         = false                                                   ;
  overwriteMode = false                                                   ;
  charWidth     = fontMetrics() . width  ( QLatin1Char('9') )             ;
  charHeight    = fontMetrics() . height (                  )             ;
  cursorX       = fontMetrics() . width  ( "> "             )             ;
  cursorY       = 0                                                       ;
  /////////////////////////////////////////////////////////////////////////
  Symbols << " "  ; Symbols << "."  ; Symbols << "_"  ; Symbols << "="    ;
  Symbols << "+"  ; Symbols << "-"  ; Symbols << "*"  ; Symbols << "/"    ;
  Symbols << "&"  ; Symbols << "^"  ; Symbols << "%"  ; Symbols << "!"    ;
  Symbols << "|"  ; Symbols << ":"  ; Symbols << ";"  ; Symbols << ","    ;
  Symbols << "`"  ; Symbols << "["  ; Symbols << "]"  ; Symbols << "("    ;
  Symbols << ")"  ; Symbols << "{"  ; Symbols << "}"  ; Symbols << "<"    ;
  Symbols << ">"  ; Symbols << "\"" ; Symbols << "'"  ; Symbols << "#"    ;
  Symbols << "@"  ; Symbols << "$"  ; Symbols << ","  ; Symbols << "?"    ;
  /////////////////////////////////////////////////////////////////////////
  setFocusPolicy(Qt::StrongFocus)                                         ;
  connect(&cursorTimer,SIGNAL(timeout     ())                             ,
          this        ,SLOT  (updateCursor())                           ) ;
  /////////////////////////////////////////////////////////////////////////
  cursorTimer . setInterval ( 500 )                                       ;
  cursorTimer . start       (     )                                       ;
}

void nForthBind::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept(); else
  QWidget::contextMenuEvent(event);
}

void nForthBind::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QWidget::focusInEvent (event) ;
}

void nForthBind::focusOutEvent(QFocusEvent * event)
{
  if (!focusOut(event)) QWidget::focusOutEvent(event) ;
}

void nForthBind::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent ( event ) ;
}

void nForthBind::showEvent(QShowEvent * event)
{
  QWidget::showEvent ( event ) ;
}

void nForthBind::keyPressEvent(QKeyEvent * event)
{
  if (!Forthing)                                                            {
    QWidget::keyPressEvent(event)                                           ;
    return                                                                  ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  QString t  = event->text()                                                ;
  bool    kp = false                                                        ;
  int     key                                                               ;
  if (t.length()<=0) kp = true                                              ;
  ///////////////////////////////////////////////////////////////////////////
  if (event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return)          {
    Enter  ( )                                                              ;
    Adjust ( )                                                              ;
    return                                                                  ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if ((event->key      () == Qt::Key_Backspace)                            &&
      (event->modifiers() == Qt::NoModifier   )                           ) {
    int len = Commands.length()                                             ;
    len = len - 1                                                           ;
    if (len<0) len = 0                                                      ;
    Commands = Commands.left(len)                                           ;
    Adjust()                                                                ;
    return                                                                  ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if (!kp)                                                                  {
    key = t.at(0).toLatin1()                                                ;
    if (key>='a' && key<='z')                                               {
      Commands.append(key)                                                  ;
      Evaluate ( )                                                          ;
      Adjust   ( )                                                          ;
      return                                                                ;
    }                                                                       ;
    if (key>='A' && key<='Z')                                               {
      Commands.append(key)                                                  ;
      Evaluate ( )                                                          ;
      Adjust   ( )                                                          ;
      return                                                                ;
    }                                                                       ;
    if (key>='0' && key<='9')                                               {
      Commands.append(key)                                                  ;
      Evaluate ( )                                                          ;
      Adjust   ( )                                                          ;
      return                                                                ;
    }                                                                       ;
    if (Symbols.contains(QChar(key)))                                       {
      Commands.append(key)                                                  ;
      Evaluate ( )                                                          ;
      Adjust   ( )                                                          ;
      return                                                                ;
    }                                                                       ;
    kp = true                                                               ;
  }                                                                         ;
  if (kp) QWidget::keyPressEvent(event)                                     ;
  update ( )                                                                ;
}

void nForthBind::mouseMoveEvent(QMouseEvent * event)
{
  update ( ) ;
}

void nForthBind::mousePressEvent(QMouseEvent * event)
{
  if (IsMask(event->buttons(),Qt::LeftButton)) {
    update                   (              )  ;
  } else                                       {
    QWidget::mousePressEvent ( event        )  ;
  }                                            ;
}

void nForthBind::closeEvent(QCloseEvent * event)
{
  if (shutdown()) event->accept() ;
  else QWidget::closeEvent(event) ;
}

void nForthBind::paintCursor(QPainter & painter)
{
  nDropOut ( !hasFocus() )                    ;
  nDropOut ( !blink      )                    ;
  if (overwriteMode)                          {
    painter.fillRect                          (
      cursorX                                 ,
      cursorY + charHeight - 2                ,
      charWidth                               ,
      2                                       ,
      palette().color(QPalette::WindowText) ) ;
  } else                                      {
    painter.fillRect                          (
      cursorX                                 ,
      cursorY                                 ,
      2                                       ,
      charHeight                              ,
      palette().color(QPalette::WindowText) ) ;
  }                                           ;
}

void nForthBind::paintContents(QPainter & p)
{
  QString     H = QString::fromUtf8(History)         ;
  QStringList L = H.split('\n')                      ;
  if (History.size()<=0) L.clear()                   ;
  L << QString("> %1").arg(Commands)                 ;
  QString T = L.join("\n")                           ;
  QRect R(0,0,width(),height())                      ;
  p.setPen   (palette().color(QPalette::WindowText)) ;
  p.drawText (R,Qt::AlignLeft|Qt::AlignTop,T       ) ;
}

void nForthBind::paintEvent(QPaintEvent * event)
{
  QPainter painter ( this                                            ) ;
  painter.fillRect ( event->rect() , palette().color(QPalette::Base) ) ;
  paintContents    ( painter                                         ) ;
  paintCursor      ( painter                                         ) ;
}

void nForthBind::updateCursor(void)
{
  blink = blink ? false : true                 ;
  update(cursorX,cursorY,charWidth,charHeight) ;
}

int nForthBind::cursorAt(QString L)
{
  QRect T = fontMetrics().boundingRect(L)                  ;
  QString E                                                ;
  if (T.width()<width()) return T.width()                  ;
  do                                                       {
    int s,l,n                                              ;
    bool g = false                                         ;
    E = fontMetrics().elidedText(L,Qt::ElideRight,width()) ;
    s = E.length() - 3                                     ;
    l = s + 1                                              ;
    n = L.length()                                         ;
    for (int i=s;!g && i<n;i++,s++,l++)                    {
      QString M = L.left(l)                                ;
      T = fontMetrics().boundingRect(M)                    ;
      if (T.width()>width()) g = true                      ;
    }                                                      ;
    if (g) L = L.mid(l)                                    ;
    T = fontMetrics().boundingRect(L)                      ;
  } while (T.width()>width())                              ;
  return 0                                                 ;
}

void nForthBind::Adjust(void)
{
  QString     H = QString::fromUtf8(History) ;
  QStringList L = H.split('\n')              ;
  QRect       G = scrollArea -> geometry ()  ;
  QRect       M =               geometry ()  ;
  int         C = L           . count    ()  ;
  int         A = Qt::AlignLeft|Qt::AlignTop ;
  QString     T                              ;
  QString     I                              ;
  QRect       F                              ;
  I = QString("> %1").arg(Commands)          ;
  if (History.size()<=0) L . clear ( )       ;
  L << I                                     ;
  T  = L.join("\n")                          ;
  C  = 4 * ( C + 1 )                         ;
  C *= charHeight                            ;
  F  . setRect(0,0,M.width(),C)              ;
  F  = fontMetrics().boundingRect(F,A,T)     ;
  if (F.height()>G.height())                 {
    M.setRect(0,0,M.width(),F.height())      ;
  } else                                     {
    M.setRect(0,0,M.width(),G.height() - 6)  ;
  }                                          ;
  if (M!=PrevRect)                           {
    setMinimumHeight ( M.height() )          ;
    PrevRect = M                             ;
  }                                          ;
  cursorY = F.height() - charHeight          ;
  cursorX = cursorAt(I)                      ;
  update        ( )                          ;
  ensureVisible ( )                          ;
}

void nForthBind::ensureVisible(void)
{
  scrollArea->ensureVisible (
    cursorX                 ,
    cursorY + charHeight/2  ,
    3                       ,
    charHeight/2 + 2      ) ;
}

void nForthBind::Evaluate(void)
{
  QString     C = Commands               ;
  QString     c = C.toLower()            ;
  QStringList h = c.split(' ')           ;
  QStringList m                          ;
  nDropOut ( h.count() <= 0 )            ;
  int         l = h[0].length()          ;
  nDropOut ( l         <= 0 )            ;
  ////////////////////////////////////////
  nDropOut ( m.count() <= 0 )            ;
  nDropOut ( !isToolTip     )            ;
  ////////////////////////////////////////
//  QString T = m.join("\n")               ;
//  QPoint p(cursorX,cursorY+charHeight+8) ;
//  p = mapToGlobal(p)                     ;
//  QToolTip::showText(p,T)                ;
}

QStringList nForthBind::Split(QString c)
{
  QStringList h = c.split(' ') ;
  QStringList H                ;
  nFullLoop ( i , h.count() )  {
    QString s = h[i]           ;
    if (s.length()>0) H << s   ;
  }                            ;
  return H                     ;
}

void nForthBind::Enter(void)
{
  QString     C = Commands      ;
  QString     c = C.toLower ( ) ;
  QStringList h = Split     (c) ;
  QStringList H = Split     (C) ;
  if (C.length()<=0) return     ;
  Queues  << Commands           ;
  Commands = ""                 ;
}

bool nForthBind::Menu(QPoint pos)
{
  nMenuManager mm(this)             ;
  QMenu      * mp                   ;
  QMenu      * mt                   ;
  QAction    * aa                   ;
  mp = mm.addMenu(tr("Properties")) ;
  mt = mm.addMenu(tr("Trace"     )) ;
  mm.add(102,tr("Font"  ))          ;
  aa = mm.add(mp,501,tr("Quiet"  )) ; aa->setCheckable(true) ; aa->setChecked(VarQuiet==1) ;
  aa = mm.add(mp,502,tr("Echo"   )) ; aa->setCheckable(true) ; aa->setChecked(VarEcho ==1) ;
  aa = mm.add(mt,511,tr("Inner"  )) ; aa->setCheckable(true) ; aa->setChecked((VarTraceFlags & 0x2)!=0) ;
  aa = mm.add(mt,512,tr("Compile")) ; aa->setCheckable(true) ; aa->setChecked((VarTraceFlags & 0x4)!=0) ;
  aa = mm.add(mt,513,tr("Special")) ; aa->setCheckable(true) ; aa->setChecked((VarTraceFlags & 0x8)!=0) ;
  mm.setFont(Plan)                  ;
  aa = mm.exec()                    ;
  if (IsNull(aa)) return false      ;
  switch (mm[aa])                   {
    case 102                        :
      setFont ( )                   ;
    break                           ;
    case 501                        :
      VarQuiet = aa->isChecked() ? 1 : 0 ;
    break                           ;
    case 502                        :
      VarEcho  = aa->isChecked() ? 1 : 0 ;
    break                           ;
    case 511                        :
      if (aa->isChecked()) VarTraceFlags |= 0x2 ;
                      else VarTraceFlags ^= 0x2 ;
    break                           ;
    case 512                        :
      if (aa->isChecked()) VarTraceFlags |= 0x4 ;
                      else VarTraceFlags ^= 0x4 ;
    break                           ;
    case 513                        :
      if (aa->isChecked()) VarTraceFlags |= 0x8 ;
                      else VarTraceFlags ^= 0x8 ;
    break                           ;
  }                                 ;
  return true                       ;
}

void nForthBind::setFont(void)
{
  changeFont ( )                                           ;
  charWidth  = fontMetrics() . width  ( QLatin1Char('9') ) ;
  charHeight = fontMetrics() . height (                  ) ;
}

bool nForthBind::canDo(QString value)
{
  bool   okay = false                 ;
  double v    = value.toDouble(&okay) ;
  return okay                         ;
}

bool nForthBind::Load(void)
{
  nKickOut ( Uuid <= 0 , false )               ;
  //////////////////////////////////////////////
  setWindowTitle(Name)                         ;
  scrollArea->setWindowTitle(Name)             ;
  //////////////////////////////////////////////
  QWidget * pw                                 ;
  pw = scrollArea->parentWidget()              ;
  if (NotNull(pw)) pw->setWindowTitle(Name)    ;
  History.append(tr("Forth <%1>\n").arg(Name)) ;
  Commands = ""                                ;
  Adjust ( )                                   ;
  return true                                  ;
}

void nForthBind::Step(void)
{
  Adjust (      ) ;
  Alert  ( Done ) ;
}

void nForthBind::Go(void)
{
  DoingForth = true                  ;
  Do ( NULL , NULL , true )          ;
  DoingForth = false                 ;
  Forthing   = false                 ;
  Report(tr("Leave Embedded Forth")) ;
}

void nForthBind::Clear(void)
{
  History.clear ( ) ;
  Adjust        ( ) ;
}

bool nForthBind::GlueFunctions(void)
{
  return true ;
}

void nForthBind::Report(QString message)
{
  nDropOut ( message.length() <= 0 ) ;
  QByteArray U = message.toUtf8()    ;
  History.append(U)                  ;
  Adjust ( )                         ;
}

int nForthBind::Feed(char * buffer,int maxChars)
{
  while (Forthing && Queues.count()<=0) {
    nTime :: msleep       ( 30 )        ;
    Plan -> processEvents (    )        ;
  }                                     ;
  ///////////////////////////////////////
  QString m = Queues[0]                 ;
  Queues.takeAt(0)                      ;
  m = m.replace("\r","")                ;
  m = m.replace("\n","")                ;
  QByteArray d = m.toUtf8()             ;
  if (d.size()<=0) return 0             ;
  memcpy(buffer,d.data(),d.size())      ;
  if (VarEcho) History.append(m)        ;
  return d.size()                       ;
}

bool nForthBind::InLoop(void)
{
  return Forthing ;
}

bool nForthBind::shutdown(void)
{
  Forthing = false        ;
  while (DoingForth)      {
    Plan->processEvents() ;
    nTime::msleep(100)    ;
  }                       ;
  return true             ;
}

void nForthBind::run(void)
{
  Forthing = true  ;
  switch (Mode)    {
    case 0         :
      Step ( )     ;
    break          ;
    case 1         :
      Go   ( )     ;
    break          ;
  }                ;
  Forthing = false ;
}

void nForthBind::Copy(void)
{
  QString     H = QString::fromUtf8(History) ;
  QStringList L = H.split('\n')              ;
  L << QString("> %1").arg(Commands)         ;
  QString T = L.join("\n")                   ;
  nSetClipboard(T)                           ;
}

void nForthBind::Save(void)
{
  QString filename;
  filename = QFileDialog::getSaveFileName  (
               this                        ,
               tr("Export Forth into ...") ,
               Plan->Temporary("")         ,
               "*.fth\n"
               "*.*"                     ) ;
  if (filename.length()<=0) return         ;
}
