#include <datawidgets.h>

N::InteractiveEditor:: InteractiveEditor ( QWidget * parent , Plan * p )
                     : Widget            (           parent ,        p )
                     , cursorTimer       ( NULL                        )
                     , scrollArea        ( NULL                        )
{
  WidgetClass                                           ;
  QScrollArea * sa = NULL                               ;
  sa = qobject_cast<QScrollArea *> ( parentWidget ( ) ) ;
  if ( NotNull ( sa ) ) Join ( sa )                     ;
  Configure ( )                                         ;
}

N::InteractiveEditor::~InteractiveEditor (void)
{
  shutdown ( ) ;
}

void N::InteractiveEditor::Join(QScrollArea * area)
{
  if (IsNull(area)) return                                           ;
  scrollArea  = area                                                 ;
  scrollArea -> setWidget                  ( this                  ) ;
  scrollArea -> setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn ) ;
  scrollArea -> setWidgetResizable         ( true                  ) ;
  scrollArea -> setFocusPolicy             ( Qt::NoFocus           ) ;
  DoProcessEvents                                                    ;
}

bool N::InteractiveEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  LinkAction        ( Paste        , Paste       ( ) ) ;
  LinkAction        ( Font         , setFont     ( ) ) ;
  Adjust            (                                ) ;
  Alert             ( Action                         ) ;
  return true                                          ;
}

void N::InteractiveEditor::Configure(void)
{
  setAttribute( Qt::WA_InputMethodEnabled )                            ;
  //////////////////////////////////////////////////////////////////////
  Prompt        = ">:"                                                 ;
  PrevRect      . setRect(0,0,0,0)                                     ;
  blink         = false                                                ;
  overwriteMode = false                                                ;
  charWidth     = fontMetrics() . width  ( QLatin1Char('9') )          ;
  charHeight    = fontMetrics() . height (                  )          ;
  cursorX       = fontMetrics() . width  ( Prompt           )          ;
  cursorY       = 0                                                    ;
  HistoryIndex  = -1                                                   ;
  //////////////////////////////////////////////////////////////////////
  Symbols << " "  ; Symbols << "."  ; Symbols << "_"  ; Symbols << "=" ;
  Symbols << "+"  ; Symbols << "-"  ; Symbols << "*"  ; Symbols << "/" ;
  Symbols << "&"  ; Symbols << "^"  ; Symbols << "%"  ; Symbols << "!" ;
  Symbols << "|"  ; Symbols << ":"  ; Symbols << ";"  ; Symbols << "," ;
  Symbols << "`"  ; Symbols << "["  ; Symbols << "]"  ; Symbols << "(" ;
  Symbols << ")"  ; Symbols << "{"  ; Symbols << "}"  ; Symbols << "<" ;
  Symbols << ">"  ; Symbols << "\"" ; Symbols << "'"  ; Symbols << "#" ;
  Symbols << "@"  ; Symbols << "$"  ; Symbols << ","  ; Symbols << "?" ;
  //////////////////////////////////////////////////////////////////////
  setFocusPolicy ( Qt::WheelFocus                                    ) ;
  addConnector   ( "Update"                                            ,
                   this        , SIGNAL ( EmitUpdate   ( ) )           ,
                   this        , SLOT   ( update       ( ) )         ) ;
  onlyConnector  ( "Update"                                          ) ;
  startCursor    (                                                   ) ;
}

void N::InteractiveEditor::contextMenuEvent(QContextMenuEvent * e)
{
  if ( Menu ( e -> pos ( ) ) ) e -> accept ( ) ; else
  QWidget::contextMenuEvent ( e )                   ;
}

void N::InteractiveEditor::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QWidget::focusInEvent (event) ;
}

void N::InteractiveEditor::focusOutEvent(QFocusEvent * event)
{
  if (!focusOut(event)) QWidget::focusOutEvent(event) ;
}

void N::InteractiveEditor::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent ( event ) ;
}

void N::InteractiveEditor::showEvent(QShowEvent * event)
{
  QWidget::showEvent ( event ) ;
}

void N::InteractiveEditor::keyPressEvent(QKeyEvent * event)
{
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
  if ( event -> key ( ) == Qt::Key_Up   )                                   {
    GoUp   ( )                                                              ;
    Adjust ( )                                                              ;
    return                                                                  ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if ( event -> key ( ) == Qt::Key_Down )                                   {
    GoDown ( )                                                              ;
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
  emit EmitUpdate ( )                                                       ;
}

void N::InteractiveEditor::inputMethodEvent(QInputMethodEvent * e)
{
  if ( InsertIM ( e -> commitString ( ) , e -> preeditString ( ) ) ) {
    e -> accept ( )                                                  ;
    return                                                           ;
  }                                                                  ;
  QWidget :: inputMethodEvent ( e )                                  ;
}

QVariant N::InteractiveEditor::inputMethodQuery(Qt::InputMethodQuery query) const
{
  switch ( query )                                                           {
    case Qt::ImCursorRectangle                                               :
    return QVariant ( QRect ( cursorX , cursorY , charWidth , charHeight ) ) ;
    case Qt::ImFont                                                          :
    return font ( )                                                          ;
    case Qt::ImCursorPosition                                                :
    return QVariant ( QPoint ( cursorX , cursorY ) )                         ;
    case Qt::ImSurroundingText                                               :
    return QVariant ( imString )                                             ;
    case Qt::ImCurrentSelection                                              :
    return QVariant ( peString )                                             ;
    case Qt::ImMaximumTextLength                                             :
    return QVariant ( 512 )                                                  ;
    case Qt::ImAnchorPosition                                                :
    case Qt::ImEnabled                                                       :
    case Qt::ImHints                                                         :
    case Qt::ImPreferredLanguage                                             :
    case Qt::ImPlatformData                                                  :
    case Qt::ImAbsolutePosition                                              :
    case Qt::ImTextBeforeCursor                                              :
    case Qt::ImTextAfterCursor                                               :
    default                                                                  :
    return QWidget::inputMethodQuery ( query )                               ;
  }                                                                          ;
}

bool N::InteractiveEditor::InsertIM(QString commit,QString preedit)
{
  if ( commit . length ( ) > 0 )      {
    Commands = imString + commit      ;
    imString = ""                     ;
    peString = ""                     ;
    Evaluate          (        )      ;
    Adjust            (        )      ;
    return true                       ;
  } else
  if ( preedit . length ( ) > 0 )     {
    if ( imString . length ( ) <= 0 ) {
      imString = Commands             ;
    }                                 ;
    peString = preedit                ;
    Commands = imString + preedit     ;
    Evaluate          (        )      ;
    Adjust            (        )      ;
    return false                      ;
  } else                              {
    imString = Commands               ;
  }                                   ;
  return true                         ;
}

void N::InteractiveEditor::mouseMoveEvent(QMouseEvent * event)
{
  emit EmitUpdate ( ) ;
}

void N::InteractiveEditor::mousePressEvent(QMouseEvent * event)
{
  if (IsMask(event->buttons(),Qt::LeftButton)) {
    emit EmitUpdate ( )                        ;
  } else                                       {
    QWidget::mousePressEvent ( event        )  ;
  }                                            ;
}

void N::InteractiveEditor::closeEvent(QCloseEvent * e)
{
  if ( shutdown ( ) ) e -> accept ( ) ; else
  QWidget::closeEvent ( e )                ;
}

void N::InteractiveEditor::paintCursor(QPainter & painter)
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

void N::InteractiveEditor::paintContents(QPainter & p)
{
  QString     H = QString::fromUtf8(History)         ;
  QStringList L = H.split('\n')                      ;
  if (History.size()<=0) L.clear()                   ;
  L << QString("%1%2").arg(Prompt).arg(Commands)     ;
  QString T = L.join("\n")                           ;
  QRect R(0,0,width(),height())                      ;
  p.setPen   (palette().color(QPalette::WindowText)) ;
  p.drawText (R,Qt::AlignLeft|Qt::AlignTop,T       ) ;
}

void N::InteractiveEditor::paintEvent(QPaintEvent * event)
{
  QPainter painter ( this                                            ) ;
  painter.fillRect ( event->rect() , palette().color(QPalette::Base) ) ;
  paintContents    ( painter                                         ) ;
  paintCursor      ( painter                                         ) ;
}

void N::InteractiveEditor::updateCursor(void)
{
  blink = blink ? false : true                          ;
  update ( cursorX , cursorY , charWidth , charHeight ) ;
}

int N::InteractiveEditor::cursorAt(QString L)
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

void N::InteractiveEditor::Adjust(void)
{
  QString     H = QString::fromUtf8(History)               ;
  QStringList L = H.split('\n')                            ;
  QRect       G                                            ;
  if (NotNull(scrollArea))                                 {
    G = scrollArea -> geometry ( )                         ;
  } else                                                   {
    G = geometry               ( )                         ;
  }                                                        ;
  QRect       M =               geometry ()                ;
  int         C = L           . count    ()                ;
  int         A = Qt::AlignLeft|Qt::AlignTop               ;
  QString     T                                            ;
  QString     I                                            ;
  QRect       F                                            ;
  I = QString("%1%2").arg(Prompt).arg(Commands)            ;
  if (History.size()<=0) L . clear ( )                     ;
  charWidth  = fontMetrics() . width  ( QLatin1Char('9') ) ;
  charHeight = fontMetrics() . height (                  ) ;
  L << I                                                   ;
  T  = L.join("\n")                                        ;
  C  = 4 * ( C + 1 )                                       ;
  C *= charHeight                                          ;
  F  . setRect(0,0,M.width(),C)                            ;
  F  = fontMetrics().boundingRect(F,A,T)                   ;
  if (F.height()>G.height())                               {
    M.setRect(0,0,M.width(),F.height())                    ;
  } else                                                   {
    M.setRect(0,0,M.width(),G.height() - 6)                ;
  }                                                        ;
  if (M!=PrevRect)                                         {
    setMinimumHeight ( M.height() )                        ;
    PrevRect = M                                           ;
  }                                                        ;
  cursorY = F.height() - charHeight                        ;
  cursorX = cursorAt(I)                                    ;
  emit EmitUpdate ( )                                      ;
  ensureVisible   ( )                                      ;
}

void N::InteractiveEditor::ensureVisible(void)
{
  if (IsNull(scrollArea)) return ;
  scrollArea->ensureVisible      (
    cursorX                      ,
    cursorY + charHeight/2       ,
    3                            ,
    charHeight/2 + 2           ) ;
}

void N::InteractiveEditor::Evaluate(void)
{
  QString     C = Commands             ;
  QString     c = C . toLower (     )  ;
  QStringList h = c . split   ( ' ' )  ;
  QStringList m                        ;
  nDropOut ( h . count ( ) <= 0 )      ;
  int         l = h [ 0 ] . length ( ) ;
  nDropOut ( l             <= 0 )      ;
  //////////////////////////////////////
  nDropOut ( m . count ( ) <= 0 )      ;
  nDropOut ( ! isToolTip        )      ;
}

QStringList N::InteractiveEditor::Split(QString c)
{
  QStringList h = c . split ( ' ' )  ;
  QStringList H                      ;
  QString     s                      ;
  foreach ( s , h )                  {
    if ( s . length ( ) > 0 ) H << s ;
  }                                  ;
  return H                           ;
}

bool N::InteractiveEditor::Menu(QPoint pos)
{
  if ( IsNull ( plan ) ) return true           ;
  //////////////////////////////////////////////
  nScopedMenu ( mm , this )                    ;
  QAction * aa                                 ;
  mm . add       ( 101 , tr("Clear history") ) ;
  mm . add       ( 102 , tr("Clear command") ) ;
  mm . add       ( 301 , tr("Append files")  ) ;
  //////////////////////////////////////////////
  mm . setFont   ( plan                      ) ;
  aa = mm . exec (                           ) ;
  //////////////////////////////////////////////
  if ( IsNull ( aa ) )                         {
    return true                                ;
  }                                            ;
  //////////////////////////////////////////////
  switch ( mm [ aa ] )                         {
    case 101                                   :
      Clear  ( )                               ;
    break                                      ;
    case 102                                   :
      Commands = ""                            ;
      Adjust ( )                               ;
    break                                      ;
    case 301                                   :
      BrowseFiles ( )                          ;
    break                                      ;
  }                                            ;
  //////////////////////////////////////////////
  return true                                  ;
}

void N::InteractiveEditor::setFont(void)
{
  changeFont ( )                                           ;
  charWidth  = fontMetrics() . width  ( QLatin1Char('9') ) ;
  charHeight = fontMetrics() . height (                  ) ;
}

void N::InteractiveEditor::Clear(void)
{
  History . clear ( ) ;
  Adjust          ( ) ;
}

QStringList & N::InteractiveEditor::CommandQueue(void)
{
  return Queues ;
}

void N::InteractiveEditor::Report(QString message)
{
  nDropOut ( message.length() <= 0 )  ;
  QByteArray U = message . toUtf8 ( ) ;
  History . append ( U )              ;
  Adjust           (   )              ;
}

bool N::InteractiveEditor::startCursor(void)
{
  if ( IsNull ( cursorTimer ) )                   {
    cursorTimer = new QTimer ( this )             ;
    addConnector  ( "Cursor"                      ,
                    cursorTimer                   ,
                    SIGNAL ( timeout      ( ) )   ,
                    this                          ,
                    SLOT   ( updateCursor ( ) ) ) ;
    onlyConnector ( "Cursor"                    ) ;
  }                                               ;
  cursorTimer -> start ( 500 )                    ;
  return true                                     ;
}

bool N::InteractiveEditor::shutdown(void)
{
  if ( NotNull(cursorTimer) ) {
    QTimer * E  = cursorTimer ;
    cursorTimer = NULL        ;
    E -> stop        ( )      ;
    E -> deleteLater ( )      ;
  }                           ;
  return true                 ;
}

void N::InteractiveEditor::Copy(void)
{
  QString     H = QString::fromUtf8(History)     ;
  QStringList L = H . split ( '\n' )             ;
  L << QString("%1%2").arg(Prompt).arg(Commands) ;
  QString     T = L . join  ( "\n" )             ;
  nSetClipboard             ( T    )             ;
}

void N::InteractiveEditor::Paste(void)
{
  QString T = nClipboardText        ;
  if ( T . length ( ) <= 0 ) return ;
  Commands . append ( T )           ;
  Evaluate          (   )           ;
  Adjust            (   )           ;
  emit EmitUpdate   (   )           ;
}

void N::InteractiveEditor::GoUp(void)
{
  HistoryIndex ++                                ;
  if ( HistoryIndex >= CommandList . count ( ) ) {
    HistoryIndex --                              ;
    return                                       ;
  }                                              ;
  Commands = CommandList [ HistoryIndex ]        ;
}

void N::InteractiveEditor::GoDown(void)
{
  if ( HistoryIndex <= 0 ) return                       ;
  HistoryIndex --                                       ;
  if ( HistoryIndex >= CommandList . count ( ) ) return ;
  Commands = CommandList [ HistoryIndex ]               ;
}

void N::InteractiveEditor::setPrompt(QString prompt)
{
  Prompt = prompt ;
}

void N::InteractiveEditor::Enter(void)
{
  if ( Commands . length ( ) <= 0 ) return ;
  Report ( Commands )                      ;
  Report ( "\n"     )                      ;
  Queues  << Commands                      ;
  Commands = ""                            ;
  emit LineInput ( )                       ;
}

void N::InteractiveEditor::Enter(QString command)
{
  if ( command . length ( ) <= 0 ) return ;
  Report         ( command )              ;
  Report         ( "\n"    )              ;
  Queues  << command                      ;
  emit LineInput (         )              ;
}

void N::InteractiveEditor::addDispatcher(SyntaxDispatcher * dispatcher)
{
  if ( NULL == dispatcher ) return           ;
  int t = dispatcher -> type ( )             ;
  if ( dispatchers . contains ( t ) ) return ;
  dispatchers [ t ] = dispatcher             ;
  dispatcher -> setHistory ( &History )      ;
}

bool N::InteractiveEditor::canDispatch(QString command,int & Type,int & Id)
{
  CUIDs ID = dispatchers . keys ( )                   ;
  int   id                                            ;
  foreach ( id , ID )                                 {
    Type = id                                         ;
    Id   = dispatchers [ id ] -> matching ( command ) ;
    if ( Id > 0 ) return true                         ;
  }                                                   ;
  return false                                        ;
}

bool N::InteractiveEditor::Dispatching(int Type,int Id)
{
  if ( ! dispatchers . contains ( Type ) ) return false ;
  return dispatchers [ Type ] -> execute ( Id )         ;
}

typedef struct     {
  N::Plan * plan   ;
  QWidget * widget ;
} ExtraPacket      ;

void N::InteractiveEditor::connectWidget(QWidget * w)
{
  ExtraPacket EP                            ;
  CUIDs       ID = dispatchers . keys ( )   ;
  int         id                            ;
  ///////////////////////////////////////////
  EP . plan   = plan                        ;
  EP . widget = w                           ;
  ///////////////////////////////////////////
  foreach ( id , ID )                       {
    dispatchers [ id ] -> setExtras ( &EP ) ;
  }                                         ;
}

void N::InteractiveEditor::BrowseFiles(void)
{
  QStringList s                     ;
  s = QFileDialog::getOpenFileNames (
        this                        ,
        tr("Browse files")          ,
        plan -> Temporary("")       ,
        tr("All files (*.*)")     ) ;
  if ( s . count ( ) <= 0 ) return  ;
  QStringList x                     ;
  QString     l                     ;
  foreach ( l , s )                 {
    l . prepend ( "\"" )            ;
    l . append  ( "\"" )            ;
    x << l                          ;
  }                                 ;
  l = x . join ( " " )              ;
  if ( Commands . length ( ) > 0 )  {
    l . prepend ( " " )             ;
  }                                 ;
  Commands . append ( l )           ;
  Adjust            (   )           ;
}

void N::InteractiveEditor::ClearCommands(void)
{
  CommandList . clear ( ) ;
}

bool N::InteractiveEditor::LoadCommands(QString filename)
{
  QFileInfo FXI                    ( filename                   ) ;
  nKickOut                         ( ! FXI . exists ( ) , false ) ;
  QStringList L = File::StringList ( filename           , "\n"  ) ;
  L = File::PurifyLines            ( L                          ) ;
  nKickOut                         ( L . count ( ) <= 0 , false ) ;
  CommandList = L                                                 ;
  return true                                                     ;
}

bool N::InteractiveEditor::SaveCommands(QString filename)
{
  if ( CommandList . count ( ) <= 0 ) return false   ;
  QString    S = CommandList . join ( "\r\n"       ) ;
  QByteArray B = S . toUtf8         (              ) ;
  return File::toFile               ( filename , B ) ;
}
