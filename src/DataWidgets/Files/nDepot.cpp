#include <nWork>

nDepot::nDepot  (QObject * parent)
      : QObject (          parent)
{
}

nDepot::~nDepot(void)
{
}

bool nDepot::CreateTables(void)
{
  bool queryrt = false;
  if (SQL->isSQLITE()) {
    {
      QStringList  tables;
      QSqlDatabase db = QSqlDatabase::addDatabase(SQL->SqlDevice());
      db . setDatabaseName(SQL->dbName);
      if (db.isValid()) queryrt = true;
      if (queryrt && db.open()) {
        QString Q;
        QSqlQuery query;
        Q = "create table " + SQL->Name("depot") + " ("                                            +
            "depotid integer primary key,"                                                         +
            "filesize integer not null,"                                                           +
            "mimeid integer not null,"                                                             +
            "createtime datetime not null,"                                                        +
            "ltime timestamp default CURRENT_TIMESTAMP,"                                           +
            "suffix varchar(256) default '',"                                                      +
            "validate integer default 1,"                                                          +
            "document longblob,"                                                                   +
            "index (filesize,mimeid,createtime,validate,suffix(256)),"                             +
            "foreign key (mimeid) references " + SQL->Name("mimetypes") + "(mimeid)"               +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_mark") + " ("                                       +
            "markid integer primary key autoincrement,"                                            +
            "marking integer"                                                                      +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_map") + " ("                                        +
            "mapid integer primary key autoincrement,"                                             +
            "ctime datetime not null,"                                                             +
            "ltime timestamp default CURRENT_TIMESTAMP,"                                           +
            "depotid integer not null,"                                                            +
            "fullname blob,"                                                                       +
            "name blob,"                                                                           +
            "index (depotid),"                                                                     +
            "foreign key (depotid) references " + SQL->Name("depot") + "(depotid)"                 +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_comment") + " ("                                    +
            "commentid integer primary key autoincrement,"                                         +
            "depotid integer not null,"                                                            +
            "comment blob,"                                                                        +
            "ltime timestamp default CURRENT_TIMESTAMP,"                                           +
            "index (depotid),"                                                                     +
            "foreign key (depotid) references " + SQL->Name("depot") + "(depotid)"                 +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_unix") + " ("                                       +
            "unixid integer primary key autoincrement,"                                            +
            "depotid integer not null,"                                                            +
            "filemode integer not null,"                                                           +
            "ltime timestamp default CURRENT_TIMESTAMP,"                                           +
            "index (depotid,filemode),"                                                            +
            "foreign key (depotid) references " + SQL->Name("depot") + "(depotid)"                 +
            ")";
        query.exec(Q);
        db.close();
      } else queryrt = false;
      if (queryrt) {
        if (queryrt && !tables.contains(SQL->Name("depot"        ))) queryrt = false;
        if (queryrt && !tables.contains(SQL->Name("depot_map"    ))) queryrt = false;
      };
    };
    QSqlDatabase::removeDatabase(SQL->SqlDevice());
  } else
  if (SQL->isMySQL ()) {
    {
      QStringList  tables;
      QSqlDatabase db;
      db = QSqlDatabase::addDatabase(SQL->SqlDevice());
      if (db.isValid()) queryrt = true;
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
      if (queryrt && db.open()) {
        QString Q;
        QSqlQuery query;
        Q = "create table " + SQL->Name("depot") + " ("                                            +
            "depotid int primary key,"                                                             +
            "filesize int not null,"                                                               +
            "mimeid int not null,"                                                                 +
            "createtime datetime not null,"                                                        +
            "ltime timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,"               +
            "suffix varchar(256) default '',"                                                      +
            "validate int default 1,"                                                              +
            "document longblob,"                                                                   +
            "index (filesize,mimeid,createtime,validate,suffix(256)),"                             +
            "foreign key (mimeid) references " + SQL->Name("mimetypes") + "(mimeid)"               +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_mark") + " ("                                       +
            "markid int primary key auto_increment,"                                               +
            "marking int"                                                                          +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_map") + " ("                                        +
            "mapid int primary key auto_increment,"                                                +
            "ctime datetime not null,"                                                             +
            "ltime timestamp default CURRENT_TIMESTAMP,"                                           +
            "depotid int not null,"                                                                +
            "fullname blob,"                                                                       +
            "name blob,"                                                                           +
            "index (depotid),"                                                                     +
            "foreign key (depotid) references " + SQL->Name("depot") + "(depotid)"                 +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_comment") + " ("                                    +
            "commentid int primary key auto_increment,"                                            +
            "depotid int not null,"                                                                +
            "comment blob,"                                                                        +
            "ltime timestamp default CURRENT_TIMESTAMP,"                                           +
            "index (depotid),"                                                                     +
            "foreign key (depotid) references " + SQL->Name("depot") + "(depotid)"                 +
            ")";
        query.exec(Q);
        Q = "create table " + SQL->Name("depot_unix") + " ("                                       +
            "unixid int primary key auto_increment,"                                               +
            "depotid int not null,"                                                                +
            "filemode int not null,"                                                               +
            "ltime timestamp default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,"               +
            "index (depotid,filemode),"                                                            +
            "foreign key (depotid) references " + SQL->Name("depot") + "(depotid)"                 +
            ")";
        query.exec(Q);
        db.close();
      } else queryrt = false;
      if (queryrt) {
        if (queryrt && !tables.contains(SQL->Name("depot"        ))) queryrt = false;
        if (queryrt && !tables.contains(SQL->Name("depot_map"    ))) queryrt = false;
      };
    };
    QSqlDatabase::removeDatabase(SQL->SqlDevice());
  };
  return queryrt;
}

int nDepot::addFile(QFileInfo & file,int MimeId)
{
  QFile f(file.absoluteFilePath());
  if (!f.exists()) return -1;
  
  int  depotid   = -1;
  bool queryrt   = false;
  bool addition  = true;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QString Q;
      QSqlQuery query;
      /* find old data */
      Q = "select depotid from " + SQL->Name("depot") + " where filesize=:FILESIZE and mimeid=:MIMEID and suffix=:SUFFIX;";
      query.prepare(Q);
      query.bindValue(":FILESIZE",f.size());
      query.bindValue(":MIMEID"  ,MimeId  );
      query.bindValue(":SUFFIX"  ,file.suffix().toUtf8());
      queryrt = query.exec();
      if (queryrt && query.size()>0) {
        QList<int> DIDs;
        f.open(QIODevice::ReadOnly);
        QByteArray A = f.readAll();
        QByteArray B;
        f.close();
        while (query.next()) DIDs << query.value(0).toInt();
        for (int i=0;addition && depotid<0 && i<DIDs.size();i++) {
          Q = "select document from " + SQL->Name("depot") + " where depotid=:DEPOTID;";
          query.prepare   (Q);
          query.bindValue (":DEPOTID",DIDs[i]);
          queryrt = query.exec();
          B.clear();
          if (queryrt && query.next()) B = query.value(0).toByteArray();
          if (A==B) {
            depotid  = DIDs[i];
            addition = false;
          };
        };
        A.clear();
      };
      if (addition) {
        Q = "insert into " + SQL->Name("depot_mark") + " (marking) values (1);";
        queryrt = query.exec(Q);
        if (queryrt) {
          Q = "select markid from " + SQL->Name("depot_mark") + " limit 0,1";
          queryrt = query.exec(Q);
          if (queryrt && query.next()) depotid = query.value(0).toInt();
          Q = "delete from " + SQL->Name("depot_mark");
          query.exec(Q);
        };
        if (depotid>=0) {
          Q = "insert into " + SQL->Name("depot") + " (depotid,filesize,mimeid,createtime,suffix) values (:DEPOTID,:FILESIZE,:MIMEID,:CREATETIME,:SUFFIX);";
          query.prepare(Q);
          query.bindValue(":DEPOTID"   ,depotid                                                     );
          query.bindValue(":FILESIZE"  ,file.size()                                                 );
          query.bindValue(":MIMEID"    ,MimeId                                                      );
          query.bindValue(":CREATETIME",file.lastModified().toString("yyyy/MM/dd hh:mm:ss").toUtf8());
          query.bindValue(":SUFFIX"    ,file.suffix().toUtf8()                                      );
          queryrt = query.exec();
          if (queryrt) {
            f.open(QIODevice::ReadOnly);
            QByteArray C = f.readAll();
            f.close();
            Q = "update " + SQL->Name("depot") + " set document=:DOCUMENT where depotid=:DEPOTID;";
            query.prepare(Q);
            query.bindValue(":DEPOTID" ,depotid);
            query.bindValue(":DOCUMENT",C);
            queryrt = query.exec();
            if (queryrt) { ; } else depotid = -1;
            C.clear();
          } else depotid = -1;
        };
      };
      if (depotid>=0) {
        Q = "insert into " + SQL->Name("depot_map") + " (ctime,depotid,fullname,name) values (:CTIME,:DEPOTID,:FULLNAME,:NAME);";
        query.prepare   (Q);
        query.bindValue (":CTIME"    , file.lastModified().toString("yyyy/MM/dd hh:mm:ss").toUtf8());
        query.bindValue (":DEPOTID"  , depotid                                                     );
        query.bindValue (":FULLNAME" , file.absoluteFilePath().toUtf8()                            );
        query.bindValue (":NAME"     , file.fileName().toUtf8()                                    );
        queryrt = query.exec();
      };
      db.close();
    };
  };
  
  QSqlDatabase::removeDatabase(SQL->SqlDevice());

  return depotid;
}

void nDepot::SqlView(QString sqlCommand,QTableView * VIEW)
{
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QSqlQueryModel * model = new QSqlQueryModel;
      model->setQuery(sqlCommand);
      VIEW->setModel(model);
      VIEW->show();
      db.close();
    };
  };
  
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
}

QString nDepot::getSuffix(int depotid)
{
  QString Suffix = "";
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QString   Q;
      QSqlQuery query;
      Q = "select suffix from " + SQL->Name("depot") + " where depotid=:DEPOTID;";
      query.prepare(Q);
      query.bindValue(":DEPOTID",depotid);
      queryrt = query.exec();
      if (queryrt && query.size()>0 && query.next()) {
        Suffix = QString::fromUtf8(query.value(0).toByteArray());
      };
      db.close();
    };
  };
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
  return Suffix;
}

QList<int> nDepot::getSuffix(QString suffix)
{
  QList<int> DepotId;
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QString   Q;
      QSqlQuery query;
      Q = "select depotid from " + SQL->Name("depot") + " where suffix=:SUFFIX;";
      query.prepare(Q);
      query.bindValue(":SUFFIX",suffix.toUtf8());
      queryrt = query.exec();
      if (queryrt) {
        while (query.next()) DepotId << query.value(0).toInt();
      };
      db.close();
    };
  };
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
  return DepotId;
}

QList<int> nDepot::getSuffixs(QStringList suffix)
{
  QList<int> DepotId;
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      int       i;
      QString   Q;
      QSqlQuery query;
      Q = "select depotid from " + SQL->Name("depot") + " where";
      for (i=1;i<suffix.size();i++) Q += " suffix='" + suffix[i-1] + "' or";
      Q += " suffix='" + suffix[suffix.size()-1] + "'";
      Q += ";";
      queryrt = query.exec(Q);
      if (queryrt) {
        while (query.next()) DepotId << query.value(0).toInt();
      };
      db.close();
    };
  };
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
  return DepotId;
}

QByteArray nDepot::getByteArray(int depotid)
{
  QByteArray B;
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QString   Q;
      QSqlQuery query;
      Q = "select document from " + SQL->Name("depot") + " where depotid=:DEPOTID;";
      query.prepare(Q);
      query.bindValue(":DEPOTID",depotid);
      queryrt = query.exec();
      if (queryrt && query.next()) B = query.value(0).toByteArray();
      db.close();
    };
  };
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
  return B;
}

QList<QTreeWidgetItem *> nDepot::getTrees(QList<int> DepotId)
{
  QList<QTreeWidgetItem *> F;
  QTreeWidgetItem       *  I;
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QString   Q;
      QSqlQuery query;
      for (int i=0;i<DepotId.size();i++) {
        Q = "select depotid,filesize,mimeid,createtime,suffix from " + SQL->Name("depot") + " where depotid=:DEPOTID;";
        query.prepare(Q);
        query.bindValue(":DEPOTID",DepotId[i]);
        queryrt = query.exec();
        if (queryrt && query.next()) {
          I  = new QTreeWidgetItem();
          I -> setText(0,query.value(0).toString());
          I -> setText(1,query.value(1).toString());
          I -> setText(2,query.value(2).toString());
          I -> setText(3,query.value(3).toDateTime().toString("yyyy/MM/dd hh:mm:ss"));
          I -> setText(4,QString::fromUtf8(query.value(4).toByteArray()));
          F << I;
        };
        qApp->processEvents();
      };
      db.close();
    };
  };
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
  return F;
}

QList<QTreeWidgetItem *> nDepot::getTrees(QList<int> DepotId,QProgressBar * Progress)
{
  QList<QTreeWidgetItem *> F;
  QTreeWidgetItem       *  I;
  bool queryrt   = false;
  {
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase(SQL->SqlDevice());
    if (db.isValid()) queryrt = true;
      if (SQL->isSQLITE()) {
      db . setDatabaseName(SQL->dbName);
    } else
    if (SQL->isMySQL()) {
      db . setDatabaseName (SQL->dbName  );
      db . setHostName     (SQL->Hostname);
      db . setUserName     (SQL->Username);
      db . setPassword     (SQL->Password);
    };
    if (queryrt && db.open()) {
      QString   Q;
      QSqlQuery query;
      Progress->setRange(0,DepotId.size());
      Progress->setValue(0);
      for (int i=0;i<DepotId.size();i++) {
        Q = "select depotid,filesize,mimeid,createtime,suffix from " + SQL->Name("depot") + " where depotid=:DEPOTID;";
        query.prepare(Q);
        query.bindValue(":DEPOTID",DepotId[i]);
        queryrt = query.exec();
        if (queryrt && query.next()) {
          I  = new QTreeWidgetItem();
          I -> setText(0,query.value(0).toString());
          I -> setText(1,query.value(1).toString());
          I -> setText(2,query.value(2).toString());
          I -> setText(3,query.value(3).toDateTime().toString("yyyy/MM/dd hh:mm:ss"));
          I -> setText(4,QString::fromUtf8(query.value(4).toByteArray()));
          F << I;
        };
        qApp->processEvents();
        Progress->setValue(i+1);
      };
      db.close();
    };
  };
  QSqlDatabase::removeDatabase(SQL->SqlDevice());
  return F;
}
