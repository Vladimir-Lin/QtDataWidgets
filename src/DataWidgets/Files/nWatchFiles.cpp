#include <datawidgets.h>

N::WatchFiles:: WatchFiles ( QObject * parent , Plan * p )
              : QObject    (           parent            )
              , Thread     (                             )
              , watcher    ( NULL                        )
              , plan       (                           p )
              , Scope      ( ""                          )
              , Watching   ( false                       )
              , Delay      ( 45                          )
              , Deletion   ( true                        )
{
  WidgetClass ;
}

N::WatchFiles::~WatchFiles (void)
{
}

bool N::WatchFiles::canDelete(void) const
{
  return Deletion ;
}

void N::WatchFiles::setDeletion(bool deletion)
{
  Deletion = deletion ;
}

void N::WatchFiles::directoryChanged(const QString & path)
{
  FileMutex   . lock   ( )    ;
  LastChanged = nTimeNow      ;
  if (!Pathes.contains(path)) {
    Pathes << path            ;
  }                           ;
  FileMutex   . unlock ( )    ;
}

void N::WatchFiles::fileChanged(const QString & path)
{
  FileMutex   . lock   ( )    ;
  LastChanged = nTimeNow      ;
  if (!Pathes.contains(path)) {
    Pathes << path            ;
  }                           ;
  FileMutex   . unlock ( )    ;
}

void N::WatchFiles::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( Type ) {
    case 10001    :
      Watch ( )   ;
    break         ;
  }               ;
}

void N::WatchFiles::sendStart(void)
{
}

void N::WatchFiles::startup(void)
{
  if (Scope.length()<=0) return                            ;
  QStringList L                                            ;
  L = plan->settings.getStringList(Scope)                  ;
  if (L.count()<=0) return                                 ;
  if (NotNull(watcher))                                    {
    Watching = false                                       ;
    watcher -> deleteLater ( )                             ;
    watcher  = NULL                                        ;
    Time :: skip ( 500 )                                   ;
  }                                                        ;
  watcher  = new QFileSystemWatcher ( this )               ;
  watcher -> addPaths               ( L    )               ;
  nConnect ( watcher , SIGNAL(directoryChanged(QString))   ,
             this    , SLOT  (directoryChanged(QString)) ) ;
  nConnect ( watcher , SIGNAL(fileChanged(QString))        ,
             this    , SLOT  (fileChanged(QString))      ) ;
  Watching = true                                          ;
  sendStart (       )                                      ;
  start     ( 10001 )                                      ;
}

void N::WatchFiles::Stop(void)
{
  if (NotNull(watcher))        {
    Watching = false           ;
    watcher -> deleteLater ( ) ;
    watcher  = NULL            ;
    Time :: skip ( 300 )       ;
  }                            ;
}

void N::WatchFiles::Deal(QStringList & files)
{
}

void N::WatchFiles::Watch(void)
{
  Watching = true                 ;
  while ( Watching )              {
    Time::msleep(300)             ;
    if (Pathes.count()>0)         {
      QDateTime NT = nTimeNow     ;
      QDateTime LM = LastChanged  ;
      LM = LM . addSecs ( Delay ) ;
      if (NT>LM)                  {
        QStringList L             ;
        FileMutex . lock   ( )    ;
        L = Pathes                ;
        Pathes    . clear  ( )    ;
        FileMutex . unlock ( )    ;
        if (L.count()>0)          {
          Deal ( L )              ;
        }                         ;
      }                           ;
    }                             ;
  }                               ;
}
