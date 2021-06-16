#include <datawidgets.h>
#include "ui_PadEditor.h"

N::PadEditor:: PadEditor ( QWidget      * parent , Plan * p )
             : Splitter  ( Qt::Vertical , parent ,        p )
             , ui        ( new Ui::PadEditor                )
             , LastId    ( 0                                )
{
  Configure ( ) ;
}

N::PadEditor::~PadEditor (void)
{
}

QSize N::PadEditor::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize ( 1280 ,  900 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::PadEditor::Configure(void)
{
  ui -> setupUi ( this )                                         ;
  ////////////////////////////////////////////////////////////////
  addAction (  0 , tr("New"       ) , QIcon(":/icons/text.png"            ) ) ;
  addAction (  1 , tr("Load"      ) , QIcon(":/images/folder.png"         ) ) ;
  addAction (  2 , tr("Save"      ) , QIcon(":/icons/save.png"            ) ) ;
  addAction (  3 , tr("Edit"      ) , QIcon(":/icons/edit.png"            ) ) ;
  addAction (  4 , tr("File"      ) , QIcon(":/icons/cdup.png"            ) ) ;
  addAction (  5 , tr("Cancel"    ) , QIcon(":/icons/remove.png"          ) ) ;
  addAction (  6 , tr("Cut"       ) , QIcon(":/images/cut.png"            ) ) ;
  addAction (  7 , tr("Copy"      ) , QIcon(":/images/copy.png"           ) ) ;
  addAction (  8 , tr("Paste"     ) , QIcon(":/images/paste.png"          ) ) ;
  addAction (  9 , tr("Undo"      ) , QIcon(":/images/projectup.png"      ) ) ;
  addAction ( 10 , tr("Redo"      ) , QIcon(":/images/exchange.png"       ) ) ;
  addAction ( 11 , tr("Find"      ) , QIcon(":/images/ScheduleZoomIn.png" ) ) ;
  addAction ( 12 , tr("Replace"   ) , QIcon(":/images/rename.png"         ) ) ;
  addAction ( 13 , tr("Select all") , QIcon(":/images/task.png"           ) ) ;
  addAction ( 14 , tr("Zoom in"   ) , QIcon(":/images/zoomin.png"         ) ) ;
  addAction ( 15 , tr("Zoom out"  ) , QIcon(":/images/zoomout.png"        ) ) ;
  addAction ( 16 , tr("Replace"   ) , QIcon(":/images/rename.png"         ) ) ;
  addAction ( 17 , tr("Search"    ) , QIcon(":/images/ScheduleZoomIn.png" ) ) ;
  ////////////////////////////////////////////////////////////////
  Displays [ 0 ] [  0 ] = true                                   ;
  Displays [ 0 ] [  1 ] = true                                   ;
  Displays [ 0 ] [  2 ] = true                                   ;
  Displays [ 0 ] [  3 ] = true                                   ;
  Displays [ 0 ] [  4 ] = false                                  ;
  Displays [ 0 ] [  5 ] = false                                  ;
  Displays [ 0 ] [  6 ] = false                                  ;
  Displays [ 0 ] [  7 ] = false                                  ;
  Displays [ 0 ] [  8 ] = false                                  ;
  Displays [ 0 ] [  9 ] = false                                  ;
  Displays [ 0 ] [ 10 ] = false                                  ;
  Displays [ 0 ] [ 11 ] = false                                  ;
  Displays [ 0 ] [ 12 ] = false                                  ;
  Displays [ 0 ] [ 13 ] = false                                  ;
  Displays [ 0 ] [ 14 ] = false                                  ;
  Displays [ 0 ] [ 15 ] = false                                  ;
  Displays [ 0 ] [ 16 ] = false                                  ;
  Displays [ 0 ] [ 17 ] = false                                  ;
  ////////////////////////////////////////////////////////////////
  Displays [ 1 ] [  0 ] = false                                  ;
  Displays [ 1 ] [  1 ] = false                                  ;
  Displays [ 1 ] [  2 ] = false                                  ;
  Displays [ 1 ] [  3 ] = false                                  ;
  Displays [ 1 ] [  4 ] = false                                  ;
  Displays [ 1 ] [  5 ] = true                                   ;
  Displays [ 1 ] [  6 ] = false                                  ;
  Displays [ 1 ] [  7 ] = false                                  ;
  Displays [ 1 ] [  8 ] = false                                  ;
  Displays [ 1 ] [  9 ] = false                                  ;
  Displays [ 1 ] [ 10 ] = false                                  ;
  Displays [ 1 ] [ 11 ] = false                                  ;
  Displays [ 1 ] [ 12 ] = false                                  ;
  Displays [ 1 ] [ 13 ] = false                                  ;
  Displays [ 1 ] [ 14 ] = false                                  ;
  Displays [ 1 ] [ 15 ] = false                                  ;
  Displays [ 1 ] [ 16 ] = false                                  ;
  Displays [ 1 ] [ 17 ] = false                                  ;
  ////////////////////////////////////////////////////////////////
  Displays [ 2 ] [  0 ] = false                                  ;
  Displays [ 2 ] [  1 ] = false                                  ;
  Displays [ 2 ] [  2 ] = false                                  ;
  Displays [ 2 ] [  3 ] = false                                  ;
  Displays [ 2 ] [  4 ] = true                                   ;
  Displays [ 2 ] [  5 ] = false                                  ;
  Displays [ 2 ] [  6 ] = true                                   ;
  Displays [ 2 ] [  7 ] = true                                   ;
  Displays [ 2 ] [  8 ] = true                                   ;
  Displays [ 2 ] [  9 ] = true                                   ;
  Displays [ 2 ] [ 10 ] = true                                   ;
  Displays [ 2 ] [ 11 ] = true                                   ;
  Displays [ 2 ] [ 12 ] = true                                   ;
  Displays [ 2 ] [ 13 ] = true                                   ;
  Displays [ 2 ] [ 14 ] = true                                   ;
  Displays [ 2 ] [ 15 ] = true                                   ;
  Displays [ 2 ] [ 16 ] = false                                  ;
  Displays [ 2 ] [ 17 ] = false                                  ;
  ////////////////////////////////////////////////////////////////
  Displays [ 3 ] [  0 ] = false                                  ;
  Displays [ 3 ] [  1 ] = false                                  ;
  Displays [ 3 ] [  2 ] = false                                  ;
  Displays [ 3 ] [  3 ] = true                                   ;
  Displays [ 3 ] [  4 ] = false                                  ;
  Displays [ 3 ] [  5 ] = false                                  ;
  Displays [ 3 ] [  6 ] = false                                  ;
  Displays [ 3 ] [  7 ] = false                                  ;
  Displays [ 3 ] [  8 ] = false                                  ;
  Displays [ 3 ] [  9 ] = false                                  ;
  Displays [ 3 ] [ 10 ] = false                                  ;
  Displays [ 3 ] [ 11 ] = false                                  ;
  Displays [ 3 ] [ 12 ] = false                                  ;
  Displays [ 3 ] [ 13 ] = false                                  ;
  Displays [ 3 ] [ 14 ] = false                                  ;
  Displays [ 3 ] [ 15 ] = false                                  ;
  Displays [ 3 ] [ 16 ] = false                                  ;
  Displays [ 3 ] [ 17 ] = true                                   ;
  ////////////////////////////////////////////////////////////////
  Displays [ 4 ] [  0 ] = false                                  ;
  Displays [ 4 ] [  1 ] = false                                  ;
  Displays [ 4 ] [  2 ] = false                                  ;
  Displays [ 4 ] [  3 ] = true                                   ;
  Displays [ 4 ] [  4 ] = false                                  ;
  Displays [ 4 ] [  5 ] = false                                  ;
  Displays [ 4 ] [  6 ] = false                                  ;
  Displays [ 4 ] [  7 ] = false                                  ;
  Displays [ 4 ] [  8 ] = false                                  ;
  Displays [ 4 ] [  9 ] = false                                  ;
  Displays [ 4 ] [ 10 ] = false                                  ;
  Displays [ 4 ] [ 11 ] = false                                  ;
  Displays [ 4 ] [ 12 ] = false                                  ;
  Displays [ 4 ] [ 13 ] = false                                  ;
  Displays [ 4 ] [ 14 ] = false                                  ;
  Displays [ 4 ] [ 15 ] = false                                  ;
  Displays [ 4 ] [ 16 ] = true                                   ;
  Displays [ 4 ] [ 17 ] = false                                  ;
  ////////////////////////////////////////////////////////////////
  nConnect ( ui->Back    , SIGNAL ( clicked       ( ) )          ,
             this        , SIGNAL ( Back          ( ) )        ) ;
  nConnect ( ui->Name    , SIGNAL ( returnPressed ( ) )          ,
             this        , SLOT   ( SaveTo        ( ) )        ) ;
  nConnect ( ui->Loading , SIGNAL ( returnPressed ( ) )          ,
             this        , SLOT   ( LoadFrom      ( ) )        ) ;
  nConnect ( ui->Find    , SIGNAL ( returnPressed ( ) )          ,
             this        , SLOT   ( Look          ( ) )        ) ;
  ////////////////////////////////////////////////////////////////
  nConnect ( Actions [  0 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( New        ( ) )        ) ;
  nConnect ( Actions [  1 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Load       ( ) )        ) ;
  nConnect ( Actions [  2 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Save       ( ) )        ) ;
  nConnect ( Actions [  3 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Edit       ( ) )        ) ;
  nConnect ( Actions [  4 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( File       ( ) )        ) ;
  nConnect ( Actions [  5 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Cancel     ( ) )        ) ;
  nConnect ( Actions [  6 ] , SIGNAL ( triggered  ( ) )          ,
             ui->Note       , SLOT   ( cut        ( ) )        ) ;
  nConnect ( Actions [  7 ] , SIGNAL ( triggered  ( ) )          ,
             ui->Note       , SLOT   ( copy       ( ) )        ) ;
  nConnect ( Actions [  8 ] , SIGNAL ( triggered  ( ) )          ,
             ui->Note       , SLOT   ( paste      ( ) )        ) ;
  nConnect ( Actions [  9 ] , SIGNAL ( triggered  ( ) )          ,
             ui->Note       , SLOT   ( undo       ( ) )        ) ;
  nConnect ( Actions [ 10 ] , SIGNAL ( triggered  ( ) )          ,
             ui->Note       , SLOT   ( redo       ( ) )        ) ;
  nConnect ( Actions [ 11 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Find       ( ) )        ) ;
  nConnect ( Actions [ 12 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Replace    ( ) )        ) ;
  nConnect ( Actions [ 13 ] , SIGNAL ( triggered  ( ) )          ,
             ui->Note       , SLOT   ( selectAll  ( ) )        ) ;
  nConnect ( Actions [ 14 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( ZoomIn     ( ) )        ) ;
  nConnect ( Actions [ 15 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( ZoomOut    ( ) )        ) ;
  nConnect ( Actions [ 16 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Replacement( ) )        ) ;
  nConnect ( Actions [ 17 ] , SIGNAL ( triggered  ( ) )          ,
             this           , SLOT   ( Look       ( ) )        ) ;
  ////////////////////////////////////////////////////////////////
  plan    -> setFont ( ui -> Note    )                           ;
  plan    -> setFont ( ui -> Replace )                           ;
  ////////////////////////////////////////////////////////////////
  setAccessibleName ( "N::PadEditor"                           ) ;
  setObjectName     ( "N::PadEditor"                           ) ;
  ////////////////////////////////////////////////////////////////
  Display           ( 0                                        ) ;
}

void N::PadEditor::addAction(int Id,QString text,QIcon icon)
{
  Actions [ Id ] = new QAction ( icon , text , ui->Tools ) ;
  ui -> Tools    -> addAction  ( Actions [ Id ]          ) ;
  Actions [ Id ] -> setFont    ( ui -> Tools -> font ( ) ) ;
}

void N::PadEditor::Display(int conf)
{
  BMAPs D   = Displays [ conf ]               ;
  CUIDs IDs = D . keys (      )               ;
  int   id                                    ;
  foreach (id,IDs)                            {
    Actions [ id ] -> setVisible ( D [ id ] ) ;
  }                                           ;
}

void N::PadEditor::setLoadable(bool enable)
{
  Displays [ 0 ] [  1 ] = enable ;
}

void N::PadEditor::setBack(QString text)
{
  ui -> Back -> setText ( text ) ;
}

void N::PadEditor::Load(QString filename)
{
  QByteArray Body                                       ;
  File :: toByteArray ( filename , Body )               ;
  ui -> Note -> blockSignals ( true                   ) ;
  ui -> Note -> append       ( QString::fromUtf8(Body)) ;
  ui -> Note -> verticalScrollBar ( ) -> setValue ( 0 ) ;
  ui -> Note -> blockSignals ( false                  ) ;
}

void N::PadEditor::Save(QString filename)
{
  QString    text = ui -> Note -> toPlainText ( ) ;
  QByteArray body = text . toUtf8 ( )             ;
  QFileInfo  fdir ( filename                )     ;
  Root . mkpath   ( fdir . absolutePath ( ) )     ;
  File :: toFile  ( filename , body         )     ;
}

void N::PadEditor::setFilename(QString filename)
{
  QString path = filename                 ;
  path = Root . relativeFilePath ( path ) ;
  ui -> Name    -> blockSignals ( true  ) ;
  ui -> Loading -> blockSignals ( true  ) ;
  ui -> Name    -> setText      ( path  ) ;
  ui -> Loading -> setText      ( path  ) ;
  ui -> Message -> setText      ( path  ) ;
  ui -> Loading -> blockSignals ( false ) ;
  ui -> Name    -> blockSignals ( false ) ;
}

void N::PadEditor::setBody(QByteArray & body)
{
  ui -> Note -> setPlainText ( QString::fromUtf8(body) ) ;
}

void N::PadEditor::New(void)
{
  ui -> Note -> blockSignals ( true  )     ;
  ui -> Note -> clear        (       )     ;
  ui -> Note -> blockSignals ( false )     ;
  //////////////////////////////////////////
  bool    e = false                        ;
  QString f = tr("Noname")                 ;
  QString t                                ;
  int     i = 1                            ;
  do                                       {
    t = QString("%1-%2.txt").arg(f).arg(i) ;
    i++                                    ;
    t = Current.absoluteFilePath(t)        ;
    QFileInfo F(t)                         ;
    if (!F.exists())                       {
      t = Root.relativeFilePath(t)         ;
      e = true                             ;
    }                                      ;
  } while (!e)                             ;
  //////////////////////////////////////////
  ui -> Name    -> blockSignals ( true  )  ;
  ui -> Loading -> blockSignals ( true  )  ;
  ui -> Name    -> setText      ( t     )  ;
  ui -> Loading -> setText      ( t     )  ;
  ui -> Message -> setText      ( t     )  ;
  ui -> Loading -> blockSignals ( false )  ;
  ui -> Name    -> blockSignals ( false )  ;
}

void N::PadEditor::Load(void)
{
  Display                           ( 1                  ) ;
  ui -> Label   -> setCurrentWidget ( ui->Loading        ) ;
  ui -> Editing -> setCurrentWidget ( ui->Note           ) ;
  ui -> Loading -> setFocus         ( Qt::TabFocusReason ) ;
}

void N::PadEditor::Save(void)
{
  Display                           ( 1                  ) ;
  ui -> Label   -> setCurrentWidget ( ui->Name           ) ;
  ui -> Editing -> setCurrentWidget ( ui->Note           ) ;
  ui -> Name    -> setFocus         ( Qt::TabFocusReason ) ;
}

void N::PadEditor::Edit(void)
{
  Display                           ( 2                  ) ;
  ui -> Label   -> setCurrentWidget ( ui->Head           ) ;
  ui -> Editing -> setCurrentWidget ( ui->Note           ) ;
  ui -> Name    -> setFocus         ( Qt::TabFocusReason ) ;
}

void N::PadEditor::File(void)
{
  Display                         ( 0        ) ;
  ui -> Label -> setCurrentWidget ( ui->Head ) ;
}

void N::PadEditor::Cancel(void)
{
  Display                         ( 0        ) ;
  ui -> Label -> setCurrentWidget ( ui->Head ) ;
}

void N::PadEditor::SaveTo(void)
{
  QString p = ui->Name->text()                         ;
  if (p.length()<=0) return                            ;
  QString filename = Current . absoluteFilePath ( p  ) ;
  QString hpath    = Root    . absoluteFilePath ( "" ) ;
  if (!filename.contains(hpath)) return                ;
  setFilename ( filename )                             ;
  Save        ( filename )                             ;
  ui -> Label -> setCurrentWidget ( ui->Head )         ;
  Alert       ( Error    )                             ;
}

void N::PadEditor::LoadFrom(void)
{
  QString p = ui -> Loading -> text ( )                ;
  if (p.length()<=0) return                            ;
  QString filename = Current . absoluteFilePath ( p  ) ;
  QString hpath    = Root    . absoluteFilePath ( "" ) ;
  if (!filename.contains(hpath)) return                ;
  QFileInfo F ( filename )                             ;
  if (!F.exists()) return                              ;
  setFilename ( filename )                             ;
  Load        ( filename )                             ;
  ui -> Label -> setCurrentWidget ( ui->Head )         ;
  Alert       ( Error    )                             ;
}

void N::PadEditor::ZoomIn(void)
{
  QString text = ui -> Note -> toPlainText (         ) ;
  QFont   f    = ui -> Note -> font        (         ) ;
  f           . setPixelSize ( f . pixelSize ( ) + 1 ) ;
  ui -> Note -> setFont      ( f                     ) ;
  ui -> Note -> setPlainText ( text                  ) ;
}

void N::PadEditor::ZoomOut(void)
{
  QString text = ui -> Note -> toPlainText (          ) ;
  QFont   f    = ui -> Note -> font        (          ) ;
  nDropOut                   ( f . pixelSize ( ) <= 6 ) ;
  f           . setPixelSize ( f . pixelSize ( )  - 1 ) ;
  ui -> Note -> setFont      ( f                      ) ;
  ui -> Note -> setPlainText ( text                   ) ;
}

void N::PadEditor::Find(void)
{
  Display                         ( 3                  ) ;
  ui -> Label -> setCurrentWidget ( ui->Find           ) ;
  ui -> Find  -> setFocus         ( Qt::TabFocusReason ) ;
}

void N::PadEditor::Look(void)
{
  QString keyword                                 ;
  keyword = ui -> Find -> text ( )                ;
  Edit ( )                                        ;
  plan -> processEvents ( )                       ;
  if (keyword.length()>0)                         {
    ui -> Note -> setFocus ( Qt::TabFocusReason ) ;
    Mark                   ( keyword            ) ;
  }                                               ;
}

void N::PadEditor::Replace(void)
{
  Display                           ( 4           ) ;
  ui -> Editing -> setCurrentWidget ( ui->Replace ) ;
}

void N::PadEditor::Replacement(void)
{
  Edit                  ( )                 ;
  plan -> processEvents ( )                 ;
  ///////////////////////////////////////////
  QString k = ui -> Keyword     -> text ( ) ;
  QString r = ui -> Replacement -> text ( ) ;
  if (k.length()<=0) return                 ;
  Replace ( k , r )                         ;
}

void N::PadEditor::Mark(QString selection,int total,int from)
{
  int             count = 0                            ;
  bool            done  = false                        ;
  QTextDocument * doc   = ui->Note->document()         ;
  QTextCursor     tc                                   ;
  QTextCharFormat tcf                                  ;
  if (from<0)                                          {
    tc   = ui -> Note -> textCursor ( )                ;
    from = tc          . position   ( )                ;
  }                                                    ;
  if (from<0) from = 0                                 ;
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
      ui -> Note -> setTextCursor       ( tc )         ;
      ui -> Note -> ensureCursorVisible (    )         ;
      count++                                          ;
    }                                                  ;
    DoProcessEvents                                    ;
    if (tc.isNull() ) done = true                      ;
    if (count>=total) done = true                      ;
  } while (!done)                                      ;
  Alert ( Error )                                      ;
}

void N::PadEditor::Replace(QString keyword,QString replacement)
{
  QString context = ui->Note->toPlainText()      ;
  context = context.replace(keyword,replacement) ;
  ui -> Note -> blockSignals ( true    )         ;
  ui -> Note -> setPlainText ( context )         ;
  ui -> Note -> blockSignals ( false   )         ;
  Alert                      ( Error   )         ;
}
