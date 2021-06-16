#include <datawidgets.h>

N::WatchPictures:: WatchPictures  ( QObject * parent , Plan * p )
                 : WatchFiles     (           parent ,        p )
                 , PictureManager (                           p )
{
  WidgetClass                                 ;
  nConnect ( this , SIGNAL ( showTray ( ) )   ,
             this , SLOT   ( sendTray ( ) ) ) ;
}

N::WatchPictures::~WatchPictures (void)
{
}

void N::WatchPictures::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( Type ) {
    case 10001    :
      Watch ( )   ;
    break         ;
  }               ;
}

void N::WatchPictures::setScope(QString scope)
{
  Scope = scope ;
}

void N::WatchPictures::Reload(QString scope)
{
  Scope = scope ;
  startup ( )   ;
}

void N::WatchPictures::sendTray(void)
{
  emit showMessage ( trayTitle  , trayMessage ) ;
}

void N::WatchPictures::sendStart(void)
{
  trayTitle   = tr("Picture monitor"           ) ;
  trayMessage = tr("Startup picture monitoring") ;
  emit showTray ( )                              ;
}

void N::WatchPictures::Deal(QStringList & files)
{
  QStringList filters                                      ;
  QStringList entries                                      ;
  QStringList images                                       ;
  QString     S                                            ;
  //////////////////////////////////////////////////////////
  filters = Images::ExtensionFilters()                     ;
  //////////////////////////////////////////////////////////
  foreach (S,files)                                        {
    QFileInfo FI ( S )                                     ;
    if (FI.isDir())                                        {
      QDir          D ( S )                                ;
      QFileInfoList FIL                                    ;
      FIL = D . entryInfoList ( filters , QDir::Files )    ;
      if (FIL.count()>0)                                   {
        for (int i=0;i<FIL.count();i++)                    {
          entries << FIL[i].absoluteFilePath()             ;
        }                                                  ;
      }                                                    ;
    } else                                                 {
      entries << S                                         ;
    }                                                      ;
  }                                                        ;
  //////////////////////////////////////////////////////////
  foreach (S,entries)                                      {
    if (!images.contains(S))                               {
      QImage I                                             ;
      if (I.load(S)) images << S                           ;
    }                                                      ;
  }                                                        ;
  entries . clear ( )                                      ;
  if (images.count()<=0) return                            ;
  //////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                         ;
  if (SC.open("WatchPictures","Deal"))                     {
    SUID  u                                                ;
    foreach (S,images) if (!entries.contains(S))           {
      u = 0                                                ;
      PictureManager :: Import ( SC , S , u )              ;
      if (u>0)                                             {
        entries << S                                       ;
        if (Deletion) QFile::remove(S)                     ;
      }                                                    ;
    }                                                      ;
    SC . close  ( )                                        ;
  }                                                        ;
  SC   . remove ( )                                        ;
  if (entries.count()<=0) return                           ;
  //////////////////////////////////////////////////////////
  trayTitle   = tr    ( "Picture monitor"                ) ;
  trayMessage = tr    ( "%1 pictures imported"             )
                . arg ( entries.count()                  ) ;
  emit showTray       (                                  ) ;
}
