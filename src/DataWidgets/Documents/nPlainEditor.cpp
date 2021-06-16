#include <datawidgets.h>

N::PlainEditor:: PlainEditor     ( QWidget * parent , Plan * p )
               : QPlainTextEdit  (           parent            )
               , VirtualGui      (           this   ,        p )
               , DocumentManager (                           p )
               , Thread          (  0 , false                  )
               , uuid            (  0                          )
               , document        (  0                          )
               , Editor          ( NULL                        )
               , Language        ( -1                          )
               , Encoding        ( ""                          )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PlainEditor::~PlainEditor(void)
{
  nSafeExec ( Editor ) Editor -> deleteLater ( ) ;
  Editor = NULL                                  ;
}

void N::PlainEditor::Configure(void)
{
  Editor            = NULL                                             ;
  plan             -> setFont (this)                                   ;
  Language          = vLanguageId                                      ;
  Data . Controller = & ( plan -> canContinue )                        ;
  QPlainTextEdit::setFont ( plan->fonts[Fonts::Editor]               ) ;
  setAccessibleName       ( "N::PlainEditor"                         ) ;
  setObjectName           ( "N::PlainEditor"                         ) ;
  addConnector  ( "Busy"  , SIGNAL(OnBusy    ()) , SLOT(StartBusy()) ) ;
  addConnector  ( "Relax" , SIGNAL(GoRelax   ()) , SLOT(StopBusy ()) ) ;
  addConnector  ( "Temp"  , SIGNAL(assignText()) , SLOT(tempText ()) ) ;
  addConnector  ( "Plain" , SIGNAL(assignPlainText(QString))           ,
                            SLOT  (setPlainText   (QString))         ) ;
  onlyConnector ( "Busy"                                             ) ;
  onlyConnector ( "Relax"                                            ) ;
  onlyConnector ( "Temp"                                             ) ;
  onlyConnector ( "Plain"                                            ) ;
}

void N::PlainEditor::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept() ; else
  QPlainTextEdit::contextMenuEvent(event);
}

void N::PlainEditor::focusInEvent(QFocusEvent * event)
{
  focusIn                       ( event ) ;
  QPlainTextEdit::focusInEvent  ( event ) ;
}

void N::PlainEditor::focusOutEvent(QFocusEvent * event)
{
  focusOut                      ( event ) ;
  QPlainTextEdit::focusOutEvent ( event ) ;
}

void N::PlainEditor::StartBusy(void)
{
  plan -> StartBusy ( ) ;
}

void N::PlainEditor::StopBusy(void)
{
  plan -> StopBusy  ( ) ;
}

bool N::PlainEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false             ) ;
  DisableAllActions (                                  ) ;
  AssignAction      ( Label        , windowTitle   ( ) ) ;
  LinkAction        ( Undo         , undo          ( ) ) ;
  LinkAction        ( Redo         , redo          ( ) ) ;
  LinkAction        ( Cut          , cut           ( ) ) ;
  LinkAction        ( Copy         , copy          ( ) ) ;
  LinkAction        ( Paste        , paste         ( ) ) ;
  LinkAction        ( ZoomIn       , ZoomIn        ( ) ) ;
  LinkAction        ( ZoomOut      , ZoomOut       ( ) ) ;
  LinkAction        ( Font         , setFont       ( ) ) ;
  LinkAction        ( Save         , Save          ( ) ) ;
  LinkAction        ( SaveAs       , SaveAs        ( ) ) ;
  LinkAction        ( Search       , Search        ( ) ) ;
  LinkAction        ( Replace      , Replace       ( ) ) ;
  LinkAction        ( Properties   , setProperties ( ) ) ;
  return true                                            ;
}

bool N::PlainEditor::FocusOut(void)
{
  return true ;
}

void N::PlainEditor::LoadScripts(SqlConnection & SC)
{
  QString Q                                               ;
  SUID    u                                               ;
  ScriptUuids . clear ( )                                 ;
  ScriptNames . clear ( )                                 ;
  Q = SC . sql . SelectFrom                               (
        "uuid"                                            ,
        PlanTable(Scripts)                                ,
        QString("where type = %1").arg(Scripts::Document) ,
        SC.OrderByAsc("id")                             ) ;
  SqlLoopNow ( SC , Q )                                   ;
    ScriptUuids << SC.Uuid(0)                             ;
  SqlLoopErr ( SC , Q )                                   ;
  SqlLoopEnd ( SC , Q )                                   ;
  foreach ( u , ScriptUuids )                             {
    QString n = SC . getName                              (
                  PlanTable(Names)                        ,
                  "uuid"                                  ,
                  vLanguageId                             ,
                  u                                     ) ;
    ScriptNames [ u ] = n                                 ;
  }                                                       ;
}

void N::PlainEditor::run(int Type,ThreadData * data)
{
  switch ( Type )                                                    {
    case 10001                                                       :
      startLoading  (      )                                         ;
      if ( 4 == data->Arguments.count() )                            {
        QString title    = data -> Arguments [ 0 ] . toString    ( ) ;
        QString version  = data -> Arguments [ 1 ] . toString    ( ) ;
        SUID    doc      = data -> Arguments [ 2 ] . toULongLong ( ) ;
        SUID    fileuuid = data -> Arguments [ 3 ] . toULongLong ( ) ;
        Load ( title , version , doc , fileuuid )                    ;
      }                                                              ;
      stopLoading   (      )                                         ;
    break                                                            ;
    case 10002                                                       :
      startLoading  (      )                                         ;
      addParagraphs ( data )                                         ;
      stopLoading   (      )                                         ;
    break                                                            ;
    case 10003                                                       :
      startLoading  (      )                                         ;
      Saving        ( data )                                         ;
      stopLoading   (      )                                         ;
    break                                                            ;
  }                                                                  ;
}

bool N::PlainEditor::startup(QString title,QString version,SUID doc,SUID fileuuid)
{
  VarArgs args           ;
  args << title          ;
  args << version        ;
  args << doc            ;
  args << fileuuid       ;
  start ( 10001 , args ) ;
  return true            ;
}

bool N::PlainEditor::Load(QString title,QString version,SUID doc,SUID fileuuid)
{
  setWindowTitle(tr("%1[%2]").arg(title).arg(version)) ;
  document = doc                                       ;
  uuid     = fileuuid                                  ;
  setEnabled   ( false )                               ;
  emit OnBusy  (       )                               ;
  EnterSQL(SC,plan->sql)                               ;
    QByteArray Body = PlainText(SC,fileuuid)           ;
    if (Body.size()>0)                                 {
      TempText = QString::fromUtf8(Body)               ;
      emit assignText ( )                              ;
    }                                                  ;
    LoadScripts ( SC )                                 ;
  LeaveSQL(SC,plan->sql)                               ;
  emit GoRelax (       )                               ;
  setEnabled   ( true  )                               ;
  Alert        ( Done  )                               ;
  return true                                          ;
}

void N::PlainEditor::tempText(void)
{
  setPlainText     ( TempText ) ;
  TempText . clear (          ) ;
}

bool N::PlainEditor::Menu(QPoint)
{
  nKickOut    ( uuid <= 0 , false )                         ;
  nScopedMenu ( mm        , this  )                         ;
  QAction   * aa        = NULL                              ;
  QTextCursor tc        = textCursor()                      ;
  QString     selection = tc.selectedText()                 ;
  CMAPs       UNIDs                                         ;
  ///////////////////////////////////////////////////////////
  if (selection.length()>0)                                 {
    plan->showMessage(selection)                            ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  QMenu * me = NULL                                         ;
  QMenu * md = NULL                                         ;
  QMenu * ms = NULL                                         ;
  mm . add ( 101 , tr("Search") )                           ;
  me = mm . addMenu ( tr("Edit"     )                     ) ;
  md = mm . addMenu ( tr("Database" )                     ) ;
  if ( ScriptUuids.count() > 0 )                            {
    SUID u                                                  ;
    int  uid = 3001                                         ;
    ms = mm . addMenu ( tr("Scripts" )                    ) ;
    foreach ( u , ScriptUuids )                             {
      mm . add ( ms , uid , ScriptNames[u] )                ;
      UNIDs [ uid ] = u                                     ;
      uid++                                                 ;
    }                                                       ;
  }                                                         ;
  mm . add ( md , 411 , tr("Add all paragraphs"         ) ) ;
  mm . addSeparator ( md )                                  ;
  mm . add ( md , 311 , tr("Related people"             ) ) ;
  mm . add ( md , 312 , tr("Related keywords"           ) ) ;
  mm . add ( md , 313 , tr("Related knowledge"          ) ) ;
  mm . add ( md , 314 , tr("Related phrases"            ) ) ;
  mm . add ( md , 315 , tr("Related paragraphs"         ) ) ;
  if (selection.length()>0)                                 {
    mm . add ( me , 201 , tr("Mark selection"           ) ) ;
    mm . add ( me , 202 , tr("Replace"                  ) ) ;
    mm . addSeparator ( md )                                ;
    mm . add ( md , 301 , tr("Add as people name"       ) ) ;
    mm . add ( md , 302 , tr("Add as keyword"           ) ) ;
    mm . add ( md , 303 , tr("Add as knowledge"         ) ) ;
    mm . add ( md , 306 , tr("Add as terminology"       ) ) ;
    mm . add ( md , 304 , tr("Add as phrase"            ) ) ;
    mm . add ( md , 305 , tr("Add as paragraph"         ) ) ;
    mm . addSeparator (                                   ) ;
  }                                                         ;
  mm . add ( me , 203 , tr("Simplify block")    )           ;
  mm . add ( me , 204 , tr("Merge blocks")      )           ;
  mm . add ( me , 205 , tr("Mark all keywords") )           ;
  mm . addSeparator ( me )                                  ;
  mm . add ( me , 401 , tr("Simplified to Traditional"  ) ) ;
  mm . add ( me , 402 , tr("Traditional to Simplified"  ) ) ;
  mm . add     ( 901 , tr("Select all") )                   ;
  mm . addSeparator (                   )                   ;
  mm . add     ( 902 , tr("Zoom in"   ) )                   ;
  mm . add     ( 903 , tr("Zoom out"  ) )                   ;
  mm . setFont ( plan                   )                   ;
  aa = mm.exec (                        )                   ;
  ///////////////////////////////////////////////////////////
  switch (mm[aa])                                           {
    case 101                                                :
      Search    ( )                                         ;
    break                                                   ;
    case 201                                                :
      Mark      ( selection )                               ;
    break                                                   ;
    case 202                                                :
      Replace   ( )                                         ;
    break                                                   ;
    case 203                                                :
      Simplify  ( )                                         ;
    break                                                   ;
    case 204                                                :
      Merge     ( )                                         ;
    break                                                   ;
    case 205                                                :
      MarkKeywords  (           )                           ;
    break                                                   ;
    case 301                                                :
      addPeople     ( selection )                           ;
    break                                                   ;
    case 302                                                :
      addKeyword    ( selection )                           ;
    break                                                   ;
    case 303                                                :
      addKnowledge  ( selection )                           ;
    break                                                   ;
    case 304                                                :
      addPhrase     ( selection )                           ;
    break                                                   ;
    case 305                                                :
      addParagraph  ( selection )                           ;
    break                                                   ;
    case 306                                                :
      addTerminology( selection )                           ;
    break                                                   ;
    case 311                                                :
      emit RelatedPeople     ( uuid , windowTitle() )       ;
    break                                                   ;
    case 312                                                :
      emit RelatedKeywords   ( uuid , windowTitle() )       ;
    break                                                   ;
    case 313                                                :
      emit RelatedKnowledge  ( uuid , windowTitle() )       ;
    break                                                   ;
    case 314                                                :
      emit RelatedPhrases    ( uuid , windowTitle() )       ;
    break                                                   ;
    case 315                                                :
      emit RelatedParagraphs ( uuid , windowTitle() )       ;
    break                                                   ;
    case 401                                                :
      toTraditional (           )                           ;
    break                                                   ;
    case 402                                                :
      toSimplified  (           )                           ;
    break                                                   ;
    case 411                                                :
      addParagraphs (           )                           ;
    break                                                   ;
    case 901                                                :
      selectAll ( )                                         ;
    break                                                   ;
    case 902                                                :
      ZoomIn    ( )                                         ;
    break                                                   ;
    case 903                                                :
      ZoomOut   ( )                                         ;
    break                                                   ;
    default                                                 :
      if ( UNIDs.count() > 0 && ScriptUuids.count() > 0 )   {
        int  uid = mm [ aa ]                                ;
        if (UNIDs.contains(uid))                            {
          SUID u = UNIDs [ uid ]                            ;
          RunScript ( u )                                   ;
        }                                                   ;
      }                                                     ;
    break                                                   ;
  }                                                         ;
  return true                                               ;
}

void N::PlainEditor::RunScript(SUID uuid)
{
  QString SCRIPT                                           ;
  QString Q                                                ;
  setEnabled ( false          )                            ;
  EnterSQL   ( SC , plan->sql )                            ;
    Q = SC . sql . SelectFrom                              (
          "script"                                         ,
          PlanTable(Scripts)                               ,
          SC.WhereUuid(uuid)                             ) ;
    if (SC.Fetch(Q)) SCRIPT = SC.String(0)                 ;
  LeaveSQL   ( SC , plan->sql )                            ;
  if (SCRIPT.length()>0)                                   {
    QString context = toPlainText()                        ;
    context = Scripts::DocumentMapper ( SCRIPT , context ) ;
    setPlainText(context)                                  ;
    setWindowModified(true)                                ;
  }                                                        ;
  setEnabled ( true           )                            ;
  Alert      ( Done           )                            ;
}

void N::PlainEditor::ZoomIn(void)
{
  QString text = toPlainText (  ) ;
  QFont   f    = font        (  ) ;
  f.setPixelSize(f.pixelSize()+1) ;
  QPlainTextEdit::setFont   ( f ) ;
  setPlainText   ( text )         ;
}

void N::PlainEditor::ZoomOut(void)
{
  QString text = toPlainText ()   ;
  QFont   f    = font        ()   ;
  nDropOut ( f.pixelSize() <= 6 ) ;
  f.setPixelSize(f.pixelSize()-1) ;
  QPlainTextEdit::setFont ( f )   ;
  setPlainText   ( text )         ;
}

void N::PlainEditor::setFont(void)
{
  bool  okay = false                              ;
  QFont f    = QFontDialog::getFont(&okay,font()) ;
  if (!okay) return                               ;
  QString text = toPlainText (      )             ;
  QPlainTextEdit::setFont    ( f    )             ;
  setPlainText               ( text )             ;
}

void N::PlainEditor::Save(void)
{
  start ( 10003 ) ;
}

void N::PlainEditor::Saving(ThreadData * d)
{
  nDropOut ( uuid <= 0 )                ;
  QString text                          ;
  text = toPlainText (                ) ;
  setEnabled         ( false          ) ;
  EnterSQL           ( SC , plan->sql ) ;
    savePlainText    ( SC , uuid,text ) ;
  ErrorSQL           ( SC , plan->sql ) ;
  LeaveSQL           ( SC , plan->sql ) ;
  setEnabled         ( true           ) ;
  setWindowModified  ( false          ) ;
  Alert              ( Done           ) ;
}

void N::PlainEditor::SaveAs(void)
{
  QString text = toPlainText()                    ;
  if (text.length()<=0) return                    ;
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Export")               ,
                       ""                         ,
                       "*.txt"                  ) ;
  if (filename.length()<=0) return                ;
  QFile F(filename)                               ;
  if (F.open(QIODevice::WriteOnly))               {
    F.write(text.toUtf8())                        ;
    F.close()                                     ;
    Alert ( Done  )                               ;
  } else                                          {
    Alert ( Error )                               ;
  }                                               ;
}

void N::PlainEditor::lineEditor(void)
{
  nSafeExec(Editor) Editor->deleteLater()           ;
  Editor = new QLineEdit()                          ;
  QFont          font = plan->status->font()        ;
  QSize          s    = plan->status->size()        ;
  QSize          m(s.width()/6,s.height())          ;
  if (font.pixelSize()>0)                           {
    font.setPixelSize (m.height()-4)                ;
    Editor ->setFont  (font        )                ;
  }                                                 ;
  Editor -> setMinimumSize ( m )                    ;
  Editor -> setMaximumSize ( m )                    ;
  plan   -> status -> addPermanentWidget ( Editor ) ;
  Editor -> show                         (        ) ;
}

void N::PlainEditor::Search(void)
{
  QTextCursor tc = textCursor()              ;
  QString selection = tc.selectedText()      ;
  lineEditor()                               ;
  Editor->setPlaceholderText(tr("Search"))   ;
  if (selection.length()>0)                  {
    Editor->setText(selection)               ;
  }                                          ;
  connect(Editor,SIGNAL(editingFinished())   ,
          this  ,SLOT  (editingFinished()) ) ;
  connect(Editor,SIGNAL(returnPressed  ())   ,
          this  ,SLOT  (searchPressed  ()) ) ;
  Editor -> setFocus ( Qt::TabFocusReason )  ;
}

void N::PlainEditor::Replace(void)
{
#ifdef UNDERLINE_EDITOR
  lineEditor()                                    ;
  Editor->setPlaceholderText(tr("Replace"))       ;
  if (selection.length()>0)                       {
    Editor->setText(selection)                    ;
  }                                               ;
  nConnect ( Editor , SIGNAL(editingFinished())   ,
             this   , SLOT  (editingFinished()) ) ;
  nConnect ( Editor , SIGNAL(returnPressed  ())   ,
             this   , SLOT  (replacePressed ()) ) ;
#else
  QTextCursor tc        = textCursor      ( )     ;
  QString     selection = tc.selectedText ( )     ;
  QString     replace   = ""                      ;
  TextReplacement * NTR                           ;
  NTR   = new TextReplacement ( this      )       ;
  NTR  -> setSource           ( selection )       ;
  plan -> setFont             ( NTR       )       ;
  if (NTR->exec()==QDialog::Accepted)             {
    selection = NTR -> sourceString  ( )          ;
    replace   = NTR -> replaceString ( )          ;
    NTR->deleteLater()                            ;
  } else                                          {
    NTR->deleteLater()                            ;
    Alert ( Error )                               ;
    return                                        ;
  }                                               ;
  Replace ( selection , replace )                 ;
#endif
}

void N::PlainEditor::editingFinished  (void)
{
  nDropOut ( IsNull(Editor) )             ;
  nSafeExec(Editor) Editor->deleteLater() ;
  Editor = NULL                           ;
  Alert ( Error )                         ;
}

void N::PlainEditor::searchPressed(void)
{
  nDropOut ( IsNull(Editor) )      ;
  QString keyword = Editor->text() ;
  Editor -> deleteLater ( )        ;
  Editor  = NULL                   ;
  Search(keyword)                  ;
}

void N::PlainEditor::replacePressed(void)
{
  nDropOut ( IsNull(Editor) )         ;
  QString replace = Editor->text()    ;
  Editor->deleteLater()               ;
  Editor = NULL                       ;
  QTextCursor tc = textCursor()       ;
  QString keyword = tc.selectedText() ;
  if (keyword.length()<=0)            {
    Alert ( Error )                   ;
    return                            ;
  }                                   ;
  Replace(keyword,replace)            ;
}

void N::PlainEditor::Search(QString keyword)
{
  QTextCursor     tc  = textCursor ()                    ;
  QTextDocument * doc = QPlainTextEdit::document()       ;
  QTextCursor     tb  = doc->find(keyword,tc.position()) ;
  if (tb.isNull())                                       {
    Alert ( Error )                                      ;
    return                                               ;
  }                                                      ;
  QTextCharFormat tcf                                    ;
  tcf = tb.blockCharFormat (                           ) ;
  tcf . setBackground      ( QColor(192,192,224)       ) ;
  tb  . setCharFormat      ( tcf                       ) ;
  tb  . selectionStart     (                           ) ;
  tb  . movePosition                                     (
        QTextCursor::WordRight                           ,
        QTextCursor::MoveAnchor                          ,
        keyword.length()                               ) ;
  tb  . selectionEnd       (                           ) ;
  setTextCursor            ( tb                        ) ;
  ensureCursorVisible      (                           ) ;
  Alert ( Action )                                       ;
}

void N::PlainEditor::Replace(QString keyword,QString replacement)
{
  QString context = toPlainText()                ;
  context = context.replace(keyword,replacement) ;
  setPlainText(context)                          ;
  setWindowModified(true)                        ;
  Alert ( Done )                                 ;
}

void N::PlainEditor::Merge(void)
{
  QString    Text = toPlainText()                  ;
  QByteArray Body = Text.toUtf8()                  ;
  QByteArray N                                     ;
  QBuffer    B ( &Body )                           ;
  N . clear    (       )                           ;
  if (B.open(QIODevice::ReadOnly))                 {
    QString    line                                ;
    QByteArray LastLine                            ;
    QByteArray CurrLine                            ;
    bool       stop = false                        ;
    do                                             {
      CurrLine.clear()                             ;
      if (B.canReadLine()) CurrLine = B.readLine() ;
      line = QString::fromUtf8(CurrLine)           ;
      line = line.replace("\r","")                 ;
      line = line.replace("\n","")                 ;
      if (line.length()>0)                         {
        if (LastLine.size()>0)                     {
          LastLine = LastLine.replace("\r","")     ;
          LastLine = LastLine.replace("\n","")     ;
          N       += LastLine                      ;
          LastLine . clear ( )                     ;
        }                                          ;
        LastLine   = CurrLine                      ;
      } else                                       {
        if (LastLine.size()>0) N += LastLine       ;
        if (CurrLine.isEmpty()) stop = true        ;
                           else N   += CurrLine    ;
        CurrLine.clear()                           ;
        LastLine.clear()                           ;
      }                                            ;
    } while (!stop)                                ;
    B.close()                                      ;
  }                                                ;
  QString context = QString::fromUtf8(N)           ;
  setPlainText(context)                            ;
  setWindowModified(true)                          ;
  Alert ( Done )                                   ;
}

void N::PlainEditor::Simplify(void)
{
  int             count = 0                                                   ;
  QTextDocument * doc   = QPlainTextEdit::document()                          ;
  for (QTextBlock tb = doc->begin();tb!=doc->end();tb=tb.next())              {
    QString txt = tb.text()                                                   ;
    QString sxt = txt                                                         ;
    txt = txt.replace(" ","")                                                 ;
    if (txt.length()!=sxt.length())                                           {
      QTextCursor     tc  = textCursor()                                      ;
      QTextCharFormat tcf = tc.blockCharFormat()                              ;
      tc      . setPosition(tb.position())                                    ;
      tc      . selectionStart()                                              ;
      tc      . movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor) ;
      tc      . selectionEnd()                                                ;
      setTextCursor(tc)                                                       ;
      ensureCursorVisible()                                                   ;
      DoProcessEvents                                                         ;
      tc      . removeSelectedText()                                          ;
      tcf     . setBackground(QColor(224,224,224))                            ;
      tc      . setCharFormat(tcf)                                            ;
      tc      . insertText(txt)                                               ;
      DoProcessEvents                                                         ;
      setTextCursor(tc)                                                       ;
      ensureCursorVisible()                                                   ;
      DoProcessEvents                                                         ;
      count++                                                                 ;
    }                                                                         ;
  }                                                                           ;
  Alert ( Done )                                                              ;
}

void N::PlainEditor::Mark(QString selection)
{
  int             from  = 0                            ;
  int             count = 0                            ;
  QTextDocument * doc   = QPlainTextEdit::document()   ;
  QTextCursor     tc                                   ;
  QTextCharFormat tcf                                  ;
  do                                                   {
    tc  = doc->find(selection,from)                    ;
    if (!tc.isNull())                                  {
      from    = tc.position() + selection.length() - 1 ;
      tcf     = tc.blockCharFormat()                   ;
      tcf     . setBackground(QColor(192,192,224))     ;
      tc      . setCharFormat(tcf)                     ;
      tc      . selectionStart()                       ;
      tc      . movePosition(QTextCursor::Right        ,
                             QTextCursor::MoveAnchor   ,
                             selection.length()      ) ;
      tc      . selectionEnd()                         ;
      setTextCursor(tc)                                ;
      ensureCursorVisible()                            ;
      count++                                          ;
    }                                                  ;
    DoProcessEvents                                    ;
  } while (!tc.isNull())                               ;
  Alert ( Done )                                       ;
}

void N::PlainEditor::MarkKeywords(void)
{
}

void N::PlainEditor::MarkKeywords(QStringList keywords)
{
  for (int ii=0;ii<keywords.count();ii++) {
    QString selection = keywords[ii];
    int from  = 0;
    int count = 0;
    QTextDocument * doc = QPlainTextEdit::document();
    QTextCursor tc;
    QTextCharFormat tcf;
    plan->showMessage(tr("Marking %1").arg(selection));
    plan->processEvents();
    do {
      tc  = doc->find(selection,from);
      if (!tc.isNull()) {
        from    = tc.position() + selection.length() - 1;
        tcf     = tc.blockCharFormat();
        tcf     . setBackground(QColor(192,192,224));
        tc      . setCharFormat(tcf);
        tc      . selectionStart();
        tc      . movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,selection.length());
        tc      . selectionEnd();
        setTextCursor(tc);
        ensureCursorVisible();
        count++;
      };
      DoProcessEvents     ;
    } while (!tc.isNull());
    if (count>0) {
      plan->showMessage(tr("%1 match %2 at %3 of %4").arg(count).arg(selection).arg(ii+1).arg(keywords.count()));
      plan->processEvents();
    };
  };
  plan->Talk(tr("Done with mark all %1 keywords").arg(keywords.count()));
  plan->processEvents();
}

void N::PlainEditor::addPeople(QString text)
{
  nDropOut ( uuid <= 0 )                    ;
  PeopleManager PM(plan)                    ;
  GroupItems    GI(plan)                    ;
  EnterSQL(SC,plan->sql)                    ;
    SUID puid                               ;
    puid = PM.FindPeople(SC,text)           ;
    if (puid<=0)                            {
      puid = PM.addPeople(SC,Language,text) ;
      if (puid<=0)                          {
        Alert ( Error )                     ;
      } else                                {
        if (uuid>0)                         {
          UUIDs Uuids                       ;
          Uuids << puid                     ;
          GI.Join                           (
            SC                              ,
            uuid                            ,
            Types::PlainText                ,
            Types::People                   ,
            Groups::Contains                ,
            0,Uuids                       ) ;
        }                                   ;
        Alert ( Done  )                     ;
      }                                     ;
    } else                                  {
      if (uuid>0)                           {
        UUIDs Uuids                         ;
        Uuids << puid                       ;
        GI.Join                             (
          SC                                ,
          uuid                              ,
          Types::PlainText                  ,
          Types::People                     ,
          Groups::Contains                  ,
          0,Uuids                         ) ;
        Alert ( Done  )                     ;
      } else                                {
        Alert ( Error )                     ;
      }                                     ;
    }                                       ;
  LeaveSQL(SC,plan->sql)                    ;
}

void N::PlainEditor::addKeyword(QString text)
{
  nDropOut ( uuid <= 0 )                         ;
  GroupItems GI(plan)                            ;
  EnterSQL(SC,plan->sql)                         ;
    SUID kuid = assureKeyword (SC,Language,text) ;
    UUIDs Uuids                                  ;
    Uuids << kuid                                ;
    GI.Join                                      (
      SC                                         ,
      uuid                                       ,
      Types::PlainText                           ,
      Types::Keyword                             ,
      Groups::Contains                           ,
      0,Uuids                                  ) ;
  LeaveSQL(SC,plan->sql)                         ;
  Alert ( Done )                                 ;
}

void N::PlainEditor::addKnowledge(QString text)
{
  nDropOut ( uuid <= 0 )                                    ;
  GroupItems GI(plan)                                       ;
  EnterSQL( SC , plan->sql )                                ;
    SUID  tuid = assureTerminology ( SC , Language , text ) ;
    SUID  kuid = assureKnowledge   ( SC , Language , text ) ;
    UUIDs Uuids                                             ;
    Uuids << tuid                                           ;
    GI.Join                                                 (
      SC                                                    ,
      kuid                                                  ,
      Types::Knowledge                                      ,
      Types::Terminology                                    ,
      Groups::Subordination                                 ,
      0,Uuids                                             ) ;
    Uuids  . clear ( )                                      ;
    Uuids << kuid                                           ;
    GI.Join                                                 (
      SC                                                    ,
      uuid                                                  ,
      Types::PlainText                                      ,
      Types::Terminology                                    ,
      Groups::Contains                                      ,
      0,Uuids                                             ) ;
    Uuids  . clear ( )                                      ;
    Uuids << kuid                                           ;
    GI.Join                                                 (
      SC                                                    ,
      uuid                                                  ,
      Types::PlainText                                      ,
      Types::Knowledge                                      ,
      Groups::Contains                                      ,
      0,Uuids                                             ) ;
  LeaveSQL( SC , plan->sql )                                ;
  Alert   ( Done           )                                ;
}

void N::PlainEditor::addTerminology(QString text)
{
  nDropOut ( uuid <= 0 )                                    ;
  GroupItems GI(plan)                                       ;
  EnterSQL( SC , plan->sql )                                ;
    SUID  tuid = assureTerminology ( SC , Language , text ) ;
    UUIDs Uuids                                             ;
    Uuids << tuid                                           ;
    GI.Join                                                 (
      SC                                                    ,
      uuid                                                  ,
      Types::PlainText                                      ,
      Types::Terminology                                    ,
      Groups::Contains                                      ,
      0,Uuids                                             ) ;
  LeaveSQL( SC , plan->sql )                                ;
  Alert   ( Done           )                                ;
}

void N::PlainEditor::addPhrase(QString text)
{
  nDropOut ( uuid <= 0 )                        ;
  GroupItems GI(plan)                           ;
  EnterSQL(SC,plan->sql)                        ;
    SUID uu = assureSentence (SC,Language,text) ;
    UUIDs Uuids                                 ;
    Uuids << uu                                 ;
    GI.Join                                     (
      SC                                        ,
      uuid                                      ,
      Types::PlainText                          ,
      Types::Sentence                           ,
      Groups::Contains                          ,
      0,Uuids                                 ) ;
  LeaveSQL(SC,plan->sql)                        ;
  Alert ( Done )                                ;
}

void N::PlainEditor::addParagraph(QString text)
{
  nDropOut ( uuid <= 0 )                        ;
  GroupItems GI(plan)                           ;
  EnterSQL(SC,plan->sql)                        ;
    SUID uu = assureParagraph(SC,Language,text) ;
    UUIDs Uuids                                 ;
    Uuids << uu                                 ;
    GI.Join                                     (
      SC                                        ,
      uuid                                      ,
      Types::PlainText                          ,
      Types::Paragraph                          ,
      Groups::Contains                          ,
      0,Uuids                                 ) ;
  LeaveSQL(SC,plan->sql)                        ;
  Alert ( Done )                                ;
}

void N::PlainEditor::addParagraphs(void)
{
  start ( 10002 ) ;
}

void N::PlainEditor::addParagraphs(ThreadData * d)
{
  nDropOut ( uuid <= 0 )                                                ;
  QString     srctext = toPlainText()                                   ;
  QStringList TL      = srctext.split("\n")                             ;
  nDropOut ( TL.count() <= 0 )                                          ;
  GroupItems GI(plan)                                                   ;
  EnterSQL(SC,plan->sql)                                                ;
    UUIDs Uuids                                                         ;
    /////////////////////////////////////////////////////////////////////
    for (int i = 0 ; IsContinue ( d ) && ( i < TL . count ( ) ) ; i++ ) {
      QString text = TL[i]                                              ;
      text = text.replace("\n","")                                      ;
      text = text.replace("\r","")                                      ;
      if (text.length()>0)                                              {
        SUID uu = assureParagraph(SC,Language,text)                     ;
        Uuids << uu                                                     ;
      }                                                                 ;
    }                                                                   ;
    /////////////////////////////////////////////////////////////////////
    if ( IsContinue ( d ) && ( Uuids . count ( ) > 0 ) )                {
      GI.Join                                                           (
        SC                                                              ,
        uuid                                                            ,
        Types::PlainText                                                ,
        Types::Paragraph                                                ,
        Groups::Contains                                                ,
        0,Uuids                                                       ) ;
    }                                                                   ;
    /////////////////////////////////////////////////////////////////////
  LeaveSQL(SC,plan->sql)                                                ;
  Alert ( Done )                                                        ;
}

void N::PlainEditor::toSimplified(void)
{
  ScriptStrings ss           ( this ) ;
  ss . setupTranslation      (      ) ;
  QString text = toPlainText (      ) ;
  text = ss . ToSimplified   ( text ) ;
  setPlainText               ( text ) ;
  setWindowModified          ( true ) ;
}

void N::PlainEditor::toTraditional(void)
{
  ScriptStrings ss           ( this ) ;
  ss . setupTranslation      (      ) ;
  QString text = toPlainText (      ) ;
  text = ss . ToTraditional  ( text ) ;
  setPlainText               ( text ) ;
  setWindowModified          ( true ) ;
}

void N::PlainEditor::setProperties(void)
{
  TextProperties * NTP = new TextProperties ( this , plan ) ;
  int e = 1                                                 ;
  NTP -> setLanguage ( Language )                           ;
  if ( Encoding . length ( ) > 0 )                          {
    e = NTP -> rEncode [ Encoding ]                         ;
    NTP -> setEncoding ( e )                                ;
  }                                                         ;
  if ( NTP -> exec ( ) != QDialog::Accepted )               {
    NTP -> deleteLater ( )                                  ;
    return                                                  ;
  }                                                         ;
  Language = NTP -> Language  (   )                         ;
  e        = NTP -> Encoding  (   )                         ;
  Encoding = NTP -> Encodings [ e ]                         ;
  NTP -> deleteLater ( )                                    ;
}
