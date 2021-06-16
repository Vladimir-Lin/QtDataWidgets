#include "nFileBrowser.hpp"
#include "nWorkQuick.hpp"
#include "nUuid.hpp"
#include "nUuidSql.hpp"

QByteArray Neutrino::LoadFileFromSQL(nSqlSettings & SQL,unsigned long long uuid,QString & Format)
{
  QByteArray B;
  {
    QSqlDatabase db = SQL.db();
    if (SQL.Open(db)) {
      QSqlQuery q;
      q.prepare(SQL.SqlCommand);
      q.bindValue(":UUID",uuid);
      if (q.exec() && q.next()) {
        B      = q.value(0).toByteArray();
        Format = QString::fromUtf8(q.value(1).toByteArray());
      };
      SQL.Close(db);
    };
  };
  QSqlDatabase::removeDatabase(SQL.SqlDevice());
  return B;
};

/***********************************************
               File Scan Widget
 ***********************************************/
nFileScanWidget:: nFileScanWidget (QWidget * parent)
               :  nHideWidget     (          parent) ,
                  scanning        (false           )
{
};

nFileScanWidget::~nFileScanWidget (void)
{
};

void nFileScanWidget::connectToStatusBar(nStatusBar * statusbar)
{
  QObject::connect(
    this      , SIGNAL (showMessage(const QString &)),
    statusbar , SLOT   (showMessage(const QString &))
  );
  QObject::connect(
    this      , SIGNAL (setMode(int)),
    statusbar , SLOT   (setMode(int))
  );
  QObject::connect(
    this      , SIGNAL (setProgress(int,int,int)),
    statusbar , SLOT   (setProgress(int,int,int))
  );
};

void nFileScanWidget::connectToProgressBar(nProgressBar * progress)
{
  QObject::connect(
    this     , SIGNAL (setMessage(QString)),
    progress , SLOT   (setMessage(QString))
  );
  QObject::connect(
    this     , SIGNAL (setRange(int,int)),
    progress , SLOT   (setRange(int,int))
  );
  QObject::connect(
    this     , SIGNAL (setValue(int)),
    progress , SLOT   (setValue(int))
  );
};

void nFileScanWidget::Stop(void)
{
  scanning = false;
};

void nFileScanWidget::ScanChildrenDirectories(QDir & dir,QList<QDir> & dirs,bool recursive)
{
  QFileInfoList children;
  children = dir.entryInfoList(QDir::AllDirs);
  emit setRange(0,children.size());
  for (int i=0;scanning && i<children.size();i++)
  if (children[i].isDir   ()       &&
      children[i].fileName()!="."  &&
      children[i].fileName()!=".."   )
  {
    if (parentWidget()!=NULL && parentWidget()->isVisible()) {
      emit setValue(i+1);
      emit setProgress(0,children.size(),i+1);
      emit showMessage(children[i].absoluteFilePath());
      qApp->processEvents();
    };
    dirs << QDir(children[i].absoluteFilePath());
    if (recursive)
      ScanChildrenDirectories(QDir(children[i].absoluteFilePath()),dirs,recursive);
  };
};

void nFileScanWidget::ScanDirectories(QList<QDir> & dirs,bool recursive)
{
  if (scanning) return;
  QFileInfoList root;
  QFileInfoList children;
  scanning = true;
  dirs.clear();
  #ifdef WIN32
  root = QDir::drives();
  #endif
  #ifdef MACOSX
  #endif
  #ifdef LINUX
  #endif
  for (int j=0;j<root.size();j++) {
    if (root[j].isDir()          &&
        root[j].fileName()!="."  &&
        root[j].fileName()!=".."   )
      dirs << QDir(root[j].absoluteFilePath());
  };
  if (scanning && recursive && dirs.size()>0) {
    emit setMessage("%v / %m");
    emit setRange(0,root.size());
    for (int j=0;scanning && j<root.size();j++)
    if (root[j].isDir()          &&
        root[j].fileName()!="."  &&
        root[j].fileName()!=".."   )
    {
      emit setValue(j+1);
      emit setProgress(0,root.size(),j+1);
      emit setMode(0);
      emit showMessage(root[j].absolutePath());
      emit setMode(1);
      qApp->processEvents();
      children.clear();
      children = root[j].absoluteDir().entryInfoList(QDir::AllDirs);
      emit setRange(0,children.size());
      for (int i=0;scanning && i<children.size();i++)
      if (children[i].isDir   ()       &&
          children[i].fileName()!="."  &&
          children[i].fileName()!=".."   )
      {
        if (parentWidget()!=NULL && parentWidget()->isVisible()) {
          emit setValue(i+1);
          emit setProgress(0,children.size(),i+1);
          emit showMessage(children[i].absoluteFilePath());
          qApp->processEvents();
        };
        dirs << QDir(children[i].absoluteFilePath());
        if (recursive)
          ScanChildrenDirectories(QDir(children[i].absoluteFilePath()),dirs,recursive);
      };
    };
    emit setMode(0);
    QString M;
    M = tr("Total") + " " + QString::number(dirs.size()) + " " + tr("directories");
    emit showMessage(M);
  };
  scanning = false;
};

void nFileScanWidget::ScanFiles(QList<QDir> & dirs,QFileInfoList & fileinfo,QDir::Filters filters)
{
  if (dirs.size()<=0) return;
  if (scanning      ) return;
  scanning = true ;
  if (scanning) {
    QString       M        ;
    QFileInfoList children ;
    emit setMode    (1            );
    emit setMessage ("%v / %m"    );
    emit setRange   (0,dirs.size());
    for (int i=0;scanning && i<dirs.size();i++) {
      if (parentWidget()!=NULL && parentWidget()->isVisible()) {
        emit setValue(i+1);
        emit setProgress(0,dirs.size(),i+1);
        emit showMessage(dirs[i].absolutePath());
        qApp->processEvents();
      };
      children.clear();
      children = dirs[i].entryInfoList(filters);
      if (children.size()>0) fileinfo << children;
    };
    emit setMode(0);
    M = tr("Total") + " " + QString::number(fileinfo.size()) + " " + tr("files");
    emit showMessage (M);
    emit setMessage  (M);
  };
  scanning = false;
};

void nFileScanWidget::ScanFiles(
       QList<QDir>   & dirs        ,
       QStringList     nameFilters ,
       QFileInfoList & fileinfo    ,
       QDir::Filters   filters
     )
{
  if (dirs.size()<=0) return;
  if (scanning      ) return;
  scanning = true ;
  if (scanning) {
    QString       M        ;
    QFileInfoList children ;
    emit setMode    (1            );
    emit setMessage ("%v / %m"    );
    emit setRange   (0,dirs.size());
    for (int i=0;scanning && i<dirs.size();i++) {
      if (parentWidget()!=NULL && parentWidget()->isVisible()) {
        emit setValue(i+1);
        emit setProgress(0,dirs.size(),i+1);
        emit showMessage(dirs[i].absolutePath());
        qApp->processEvents();
      };
      children.clear();
      children = dirs[i].entryInfoList(nameFilters,filters);
      if (children.size()>0) fileinfo << children;
    };
    emit setMode(0);
    M = tr("Total") + " " + QString::number(fileinfo.size()) + " " + tr("files");
    emit showMessage (M);
    emit setMessage  (M);
  };
  scanning = false;
};

void nFileScanWidget::SaveDirectories(QList<QDir> & dirs,nSqlSettings & sql)
{
  if (dirs.size()<=0) return;
  if (scanning      ) return;
  scanning = true ;
  if (scanning) {
    QStringList        tables        ;
    QString            Q             ;
    QString            D             ;
    QString            Platform      ;
    unsigned long long uuid          ;
    #ifdef WIN32
    Platform = "win32";
    #endif
    #ifdef MACOSX
    Platform = "macosx";
    #endif
    #ifdef LINUX
    Platform = "linux";
    #endif
    QSqlDatabase db = sql.db() ;
    emit setMode    (1            );
    emit setMessage ("%v / %m"    );
    emit setRange   (0,dirs.size());
    if (sql.Open(db)) {
      Q = sql.CreateTable(sql.TableName)      +
          "("                                 +
          sql.PrimaryKey      ("id"   )       + "," +
          sql.Uuid            ("uuid" )       + "," +
          "enable integer default -1"         + "," +
          "existing integer default 1"        + "," +
          "permission integer default 0"      + "," +
          "platform varchar(32) default ''"   + "," +
          "owner varchar(32) default ''"      + "," +
          "files integer default -1"          + "," +
          "name blob not null"                + "," +
          sql.TimestampString ("ltime")       + "," +
          "unique (uuid)"                     + "," +
          "unique (name(720))"                + "," +
          "index (enable)"                    + "," +
          "index (existing)"                  + "," +
          "index (permission)"                + "," +
          "index (platform(32))"              + "," +
          "index (owner(32))"                 + "," +
          "index (files)"                     +
          ")"                                 + " " +
          sql.Engine("InnoDB","utf8")         + ";";
      sql.Execute(Q);
      tables = sql.tables();
      if (tables.contains(sql.Name(sql.TableName))) {
        QSqlQuery q;
        for (int i=0;i<dirs.size();i++) {
          if (parentWidget()!=NULL && parentWidget()->isVisible()) {
            emit setValue(i+1);
            emit setProgress(0,dirs.size(),i+1);
            emit showMessage(dirs[i].absolutePath());
            qApp->processEvents();
          };
          uuid = nUuid::createUuidShort();
          D    = dirs[i].absolutePath();
          Q    = sql.InsertInto(
                   "uuid,platform,name",
                   sql.Name("dirs"),
                   ":UUID,:PLATFORM,:NAME"
                 ) + ";";
          q.prepare   (Q                                );
          q.bindValue (":UUID"     , uuid               );
          q.bindValue (":PLATFORM" , Platform . toUtf8());
          q.bindValue (":NAME"     , D        . toUtf8());
          q.exec      (                                 );
        };
      };
      sql.Close(db);
    };
    emit setMode(0);
    Q = tr("Total") + " " + QString::number(dirs.size()) + " " + tr("directories");
    emit showMessage(Q);
    emit setMessage (Q);
  };
  QSqlDatabase::removeDatabase(sql.SqlDevice());
  scanning = false;
};

void nFileScanWidget::ImportFiles(QFileInfoList & fileinfo,nSqlSettings & sql)
{
  if (fileinfo.size()<=0) return;
  if (scanning          ) return;
  scanning = true ;
  if (scanning) {
    QStringList        tables        ;
    QString            Q             ;
    QString            D             ;
    QString            Suffix        ;
    QString            Platform      ;
    QString            Owner         ;
    QDateTime          lastModified  ;
    QFile              File          ;
    QByteArray         Data          ;
    unsigned long long uuid          ;
    long long          filesize      ;
    int                Permission    ;
    QList<unsigned long long> files  ;
    #ifdef WIN32
    Platform = "win32";
    #endif
    #ifdef MACOSX
    Platform = "macosx";
    #endif
    #ifdef LINUX
    Platform = "linux";
    #endif
    QSqlDatabase db = sql.db() ;
    emit setMode    (1            );
    emit setMessage ("%v / %m"    );
    emit setRange   (0,fileinfo.size());
    if (sql.Open(db)) {
      Q = sql.CreateTable("files")                        +
          "("                                             +
          sql.PrimaryKey      ("id"   )                   + "," +
          sql.Uuid            ("uuid" )                   + "," +
          "mimeid bigint unsigned default 0"              + "," +
          "suffix varchar(64) default ''"                 + "," +
          "filesize bigint default 0"                     + "," +
          sql.TimestampString ("ltime")                   + "," +
          "file longblob"                                 + "," +
          "unique (uuid)"                                 + "," +
          "index (filesize)"                              + "," +
          "index (suffix(64))"                            +
          ")"                                             + " " +
          sql.Engine("InnoDB","utf8")                     + ";";
      sql.Execute(Q);
      Q = sql.CreateTable("filename")                     +
          "("                                             +
          sql.PrimaryKey      ("id"   )                   + "," +
          sql.Uuid            ("uuid" )                   + "," +
          "enable integer default -1"                     + "," +
          "existing integer default 1"                    + "," +
          "permission integer default 0"                  + "," +
          "platform varchar(32) default ''"               + "," +
          "owner varchar(32) default ''"                  + "," +
          "lastModified datetime"                         + "," +
          "filesize bigint default 0"                     + "," +
          "name blob not null"                            + "," +
          sql.TimestampString ("ltime")                   + "," +
          sql.ForeignKey("uuid",sql.Name("files"),"uuid") + "," +
          "unique (name(720))"                            + "," +
          "index (enable)"                                + "," +
          "index (existing)"                              + "," +
          "index (permission)"                            + "," +
          "index (platform(32))"                          + "," +
          "index (owner(32))"                             + "," +
          "index (filesize)"                              +
          ")"                                             + " " +
          sql.Engine("InnoDB","utf8")                     + ";";
      sql.Execute(Q);
      tables = sql.tables();
      if (tables.contains(sql.Name("files"   )) &&
          tables.contains(sql.Name("filename"))   ) {
        QSqlQuery q;
        for (int i=0;i<fileinfo.size();i++)
        if (fileinfo[i].size()>0) {
          uuid         =      nUuid::createUuidShort       ();
          D            =      fileinfo[i].absoluteFilePath ();
          Suffix       =      fileinfo[i].completeSuffix   ();
          Permission   = (int)fileinfo[i].permissions      ();
          Owner        =      fileinfo[i].owner            ();
          filesize     =      fileinfo[i].size             ();
          lastModified =      fileinfo[i].lastModified     ();
          if (parentWidget()!=NULL && parentWidget()->isVisible()) {
            emit setValue       (i+1                  );
            emit setProgress    (0,fileinfo.size(),i+1);
            emit showMessage    (D                    );
            qApp->processEvents (                     );
          };
          Data . clear          (                     );
          File . setFileName    (D                    );
          File . open           (QIODevice::ReadOnly  );
          Data = File . readAll (                     );
          File . close          (                     );
          if (filesize>0 && filesize==Data.size()) {
            bool shouldAppend = true;
            files.clear();
            Q = sql.SelectFrom("uuid",sql.Name("files")) + " " +
                "where filesize=:FILESIZE;";
            q.prepare(Q);
            q.bindValue(":FILESIZE",filesize);
            if (q.exec()) {
              while (q.next())
                files << q.value(0).toULongLong();
            };
            if (files.size()>0) {
              for (int i=0;shouldAppend && i<files.size();i++) {
                Q = sql.SelectFrom("file",sql.Name("files")) + " " +
                    "where uuid=:UUID;";
                q.prepare   (Q               );
                q.bindValue (":UUID",files[i]);
                if (q.exec()) {
                  if (q.next()) {
                    QByteArray B = q.value(0).toByteArray();
                    if (Data==B) {
                      shouldAppend = false    ;
                      uuid         = files[i] ;
                    };
                    B.clear();
                  };
                };
              };
            };
            if (shouldAppend) {
              Q = sql.InsertInto(
                    "uuid,suffix,filesize,file",
                    sql.Name("files") ,
                    ":UUID,:SUFFIX,:FILESIZE,:FILE"
                  ) + ";";
              q.prepare   (Q                            );
              q.bindValue (":UUID"     , uuid           );
              q.bindValue (":SUFFIX"   , Suffix.toUtf8());
              q.bindValue (":FILESIZE" , filesize       );
              q.bindValue (":FILE"     , Data           );
              q.exec      (                             );
            };
            Q    = sql.InsertInto(
                      "uuid,permission,platform,owner,lastModified,filesize,name",
                      sql.Name("filename"),
                      ":UUID,:PERMISSION,:PLATFORM,:OWNER,:MODIFIED,:FILESIZE,:NAME"
                    ) + ";";
            q.prepare   (Q                                  );
            q.bindValue (":UUID"       , uuid               );
            q.bindValue (":PERMISSION" , Permission         );
            q.bindValue (":PLATFORM"   , Platform . toUtf8());
            q.bindValue (":OWNER"      , Owner    . toUtf8());
            q.bindValue (":MODIFIED"   , lastModified       );
            q.bindValue (":FILESIZE"   , filesize           );
            q.bindValue (":NAME"       , D        . toUtf8());
            q.exec      (                                   );
          };
        };
      };
      sql.Close(db);
    };
    emit setMode(0);
    Q = tr("Total") + " " + QString::number(fileinfo.size()) + " " + tr("files");
    emit showMessage(Q);
    emit setMessage (Q);
  };
  QSqlDatabase::removeDatabase(sql.SqlDevice());
  scanning = false;
};

/////////////////////////////////////////////////////////////////////////
nFileBrowser:: nFileBrowser (QWidget * parent)
            :  nListWidget  (          parent)
{
};

nFileBrowser::~nFileBrowser(void)
{
};

#ifdef QDESIGNER_EXPORT_WIDGETS

#include <QtPlugin>

/******************************************************
 *                     PlugIn
 ******************************************************/
nFileBrowserPlugin:: nFileBrowserPlugin (QObject * parent)
                  :  QObject            (          parent)
{
  initialized = false;
};

void nFileBrowserPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
  if (initialized) return;
  initialized = true;
};

bool nFileBrowserPlugin::isInitialized(void) const
{
  return initialized;
};

QWidget * nFileBrowserPlugin::createWidget(QWidget *parent)
{
  return new nFileBrowser(parent);
};

QString nFileBrowserPlugin::name(void) const
{
  return "nFileBrowser";
};

QString nFileBrowserPlugin::group(void) const
{
  return tr("Neutrino (Qt Tools) / File");
};

QIcon nFileBrowserPlugin::icon(void) const
{
  return QIcon(":/nTools/nFileBrowser.png");
};

QString nFileBrowserPlugin::toolTip(void) const
{
  return "";
};

QString nFileBrowserPlugin::whatsThis(void) const
{
  return "";
};

bool nFileBrowserPlugin::isContainer(void) const
{
  return false;
};

QString nFileBrowserPlugin::domXml(void) const
{
  return "<widget class=\"nFileBrowser\" name=\"filebrowser\">\n"
         " <property name=\"geometry\">\n"
         "  <rect>\n"
         "   <x>0</x>\n"
         "   <y>0</y>\n"
         "   <width>240</width>\n"
         "   <height>240</height>\n"
         "  </rect>\n"
         " </property>\n"
         "</widget>\n";
};

QString nFileBrowserPlugin::includeFile(void) const
{
  return "nFileBrowser.hpp";
};

#ifdef SPLIT_MODULE
Q_EXPORT_PLUGIN2(nFileBrowser,nFileBrowserPlugin)
#endif

#endif
