#include <datawidgets.h>

N::QtBuilder:: QtBuilder   (QWidget * parent,Plan * p)
             : TextBrowser (          parent,       p)
             , AttachDock  (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::QtBuilder::~QtBuilder(void)
{
}

void N::QtBuilder::Configure(void)
{
  plan   -> setFont   ( this )                                    ;
  Process . setParent ( this )                                    ;
  /////////////////////////////////////////////////////////////////
  ReportError = false                                             ;
  Ooops       = false                                             ;
  Codec       = QTextCodec::codecForLocale()                      ;
  /////////////////////////////////////////////////////////////////
  connect(&Process,SIGNAL(error(QProcess::ProcessError     ))     ,
          this    ,SLOT  (error(QProcess::ProcessError     ))   ) ;
  connect(&Process,SIGNAL(finished(int,QProcess::ExitStatus))     ,
          this    ,SLOT  (finished(int,QProcess::ExitStatus))   ) ;
  connect(&Process,SIGNAL(readyReadStandardError())               ,
          this    ,SLOT  (readyReadStandardError())             ) ;
  connect(&Process,SIGNAL(readyReadStandardOutput())              ,
          this    ,SLOT  (readyReadStandardOutput())            ) ;
  connect(&Process,SIGNAL(started())                              ,
          this    ,SLOT  (started())                            ) ;
  connect(&Process,SIGNAL(stateChanged(QProcess::ProcessState))   ,
          this    ,SLOT  (stateChanged(QProcess::ProcessState)) ) ;

}

void N::QtBuilder::Docking(QMainWindow * Main,QString title,Qt::DockWidgetArea area,Qt::DockWidgetAreas areas)
{
  AttachDock::Docking(Main,this,title,area,areas) ;
  QObject::connect(Dock,SIGNAL(visibilityChanged(bool)),this,SLOT(Visible(bool)));
}

void N::QtBuilder::Visible(bool visible)
{
  Visiblity(visible) ;
}

void N::QtBuilder::DockIn(bool shown)
{
  Show(shown);
}

void N::QtBuilder::closeEvent(QCloseEvent * event)
{
  if (Process.state()!=QProcess::NotRunning) {
    event->ignore()                          ;
    return                                   ;
  }                                          ;
  QTextBrowser :: closeEvent ( event )       ;
}

void N::QtBuilder::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept()    ;
  else QTextBrowser::contextMenuEvent(event) ;
}

void N::QtBuilder::error(QProcess::ProcessError err)
{
  setTextColor(QColor(255,0,0))     ;
  switch (err)                      {
    case QProcess::FailedToStart    :
      append(tr("Failed to start")) ;
    break                           ;
    case QProcess::Crashed          :
      append(tr("Crashed"        )) ;
    break                           ;
    case QProcess::Timedout         :
      append(tr("Timed out"      )) ;
    break                           ;
    case QProcess::WriteError       :
      append(tr("Write error"    )) ;
    break                           ;
    case QProcess::ReadError        :
      append(tr("Read error"     )) ;
    break                           ;
    case QProcess::UnknownError     :
      append(tr("Unknown error"  )) ;
    break                           ;
  }                                 ;
  setTextColor(QColor(  0,0,0))     ;
}

void N::QtBuilder::finished(int exitCode,QProcess::ExitStatus exitStatus)
{
  QString S = tr( "Compile finished" ) ;
  plan->Talk    ( S                  ) ;
  setTextColor  ( QColor(0,0,255)    ) ;
  append        ( S                  ) ;
  setTextColor  ( QColor(0,0,0  )    ) ;
  emit Finished (                    ) ;
}

void N::QtBuilder::readyReadStandardError(void)
{
  if (!ReportError) return                        ;
  /////////////////////////////////////////////////
  QByteArray SO = Process.readAllStandardError () ;
  if (SO.size()<=0) return                        ;
  QString T  = Codec->toUnicode(SO)               ;
  if (T.length()<=0) return                       ;
  QStringList TS = T.split("\n")                  ;
  setTextColor(QColor(255,0,0))                   ;
  for (int i=0;i<TS.count();i++)                  {
    QString S = TS[i]                             ;
    S = S.replace("\n","")                        ;
    S = S.replace("\r","")                        ;
    if (S.length()>0) append ( S )                ;
  }                                               ;
  setTextColor(QColor(0,0,0))                     ;
}

void N::QtBuilder::readyReadStandardOutput(void)
{
  QByteArray SO = Process.readAllStandardOutput () ;
  if (SO.size()<=0) return                         ;
  QString T  = Codec->toUnicode(SO)                ;
  if (T.length()<=0) return                        ;
  QStringList TS = T.split("\n")                   ;
  setTextColor(QColor(0,0,0))                      ;
  for (int i=0;i<TS.count();i++)                   {
    QString S = TS[i]                              ;
    S = S.replace("\n","")                         ;
    S = S.replace("\r","")                         ;
    if (S.length()>0)                              {
      QStringList ST = S.split(" ")                ;
      if (ST.count()>2 && ST[2]=="error")          {
        setTextColor(QColor(255,0,0))              ;
        append ( S )                               ;
        setTextColor(QColor(  0,0,0))              ;
        if (!Ooops)                                {
          plan->Talk(tr("An error occurred"))      ;
          Ooops = true                             ;
        }                                          ;
      } else append ( S )                          ;
    }                                              ;
  }                                                ;
  Parse ( T )                                      ;
}

void N::QtBuilder::started(void)
{
  plan->Talk(tr("Starting compile"));
  emit Started() ;
}

void N::QtBuilder::stateChanged(QProcess::ProcessState newState)
{
}

void N::QtBuilder::command(QString sh)
{
  QString    T  = sh + "\r\n"            ;
  QByteArray TC = Codec->fromUnicode(T)  ;
  Process . write ( TC )                 ;
  Process . waitForBytesWritten ( 1000 ) ;
}

void N::QtBuilder::run(QString program,QStringList arguments,QString directory)
{
  if (Process.state()!=QProcess::NotRunning) return ;
  OrganizePathes ( )                                ;
  Ooops  = false                                    ;
  Process.setWorkingDirectory(directory)            ;
  Process.start(program,arguments)                  ;
}

QString N::QtBuilder::Soliton(QString S)
{
  QStringList T = S.split(";")  ;
  QStringList E                 ;
  for (int i=0;i<T.count();i++) {
    QString F = T[i]            ;
    if (!E.contains(F))         {
      E << F                    ;
    }                           ;
  }                             ;
  return E.join(";")            ;
}

void N::QtBuilder::OrganizePathes(void)
{
  QStringList Environ = QProcess::systemEnvironment() ;
  QStringList E                                       ;
  for (int i=0;i<Environ.count();i++)                 {
    QString S = Environ[i]                            ;
    QString Path    = S.left(5)                       ;
    QString INCLUDE = S.left(8)                       ;
    QString LIB     = S.left(4)                       ;
    QString LIBPATH = S.left(8)                       ;
    if (Path=="Path=")                                {
      Path    = S.mid(5)                              ;
      S       = "Path="                               ;
      S      += Soliton(Path)                         ;
    } else
    if (INCLUDE=="INCLUDE=")                          {
      INCLUDE = S.mid(8)                              ;
      S       = "INCLUDE="                            ;
      S      += Soliton(INCLUDE)                      ;
    } else
    if (LIB=="LIB=")                                  {
      LIB     = S.mid(4)                              ;
      S       = "LIB="                                ;
      S      += Soliton(LIB)                          ;
    } else
    if (LIBPATH=="LIBPATH==")                         {
      LIBPATH = S.mid(8)                              ;
      S       = "LIBPATH="                            ;
      S      += Soliton(LIBPATH)                      ;
    }                                                 ;
    E << S                                            ;
  }                                                   ;
  Process . setEnvironment ( E )                      ;
}

void N::QtBuilder::Parse(QString stdIn)
{
  emit Result ( stdIn ) ;
}

bool N::QtBuilder::Menu(QPoint pos)
{
  QMdiSubWindow * mdi  = Casting(QMdiSubWindow,parent())       ;
  QDockWidget   * dock = Casting(QDockWidget  ,parent())       ;
  nScopedMenu(mm,this)                                         ;
  QAction   * aa                                               ;
  mm.add      ( 101 , tr("Clear"         ) )                   ;
  aa = mm.add ( 102 , tr("Report error"  ) )                   ;
  switch (Process.state())                                     {
    case QProcess::NotRunning                                  :
      mm.add  ( 901 , tr("System rebuild") )                   ;
    break                                                      ;
    case QProcess::Starting                                    :
    break                                                      ;
    case QProcess::Running                                     :
      mm.add  ( 902 , tr("Interrupt"     ) )                   ;
    break                                                      ;
  }                                                            ;
  if (NotNull(dock) || NotNull(mdi )) mm.addSeparator()        ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  aa -> setCheckable ( true        )                           ;
  aa -> setChecked   ( ReportError )                           ;
  mm  . setFont      ( plan        )                           ;
  aa  = mm.exec      (             )                           ;
  if (IsNull(aa)) return true                                  ;
  QStringList arguments                                        ;
  switch (mm[aa])                                              {
    case 101                                                   :
      clear()                                                  ;
    break                                                      ;
    case 102                                                   :
      ReportError = aa -> isChecked ( )                        ;
    break                                                      ;
    case 901                                                   :
      run ( Program , arguments , Directory )                  ;
    break                                                      ;
    case 902                                                   :
      Process . terminate (                          )         ;
      Process . kill      (                          )         ;
      Alert ( Error )                                          ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi (this,Qt::Horizontal)                     ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::BottomDockWidgetArea                               ,
        Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea)        ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
