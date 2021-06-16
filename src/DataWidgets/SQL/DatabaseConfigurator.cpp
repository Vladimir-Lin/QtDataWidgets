#include <Exciton>
#include "ui_DatabaseConfigurator.h"

DatabaseConfigurator:: DatabaseConfigurator (QWidget * parent,nPlan * plan)
                     : nStackedWidget       (          parent,        plan)
                     , ui                   (new Ui::DatabaseConfigurator )
                     , Uuid                 (0                            )
{
  ui->setupUi(this);
}

DatabaseConfigurator::~DatabaseConfigurator(void)
{
  delete ui;
}

void DatabaseConfigurator::Selector(void)
{
  setCurrentWidget (ui->Selector                     ) ;
  Plan->Talk       (tr("Select your SQL server type")) ;
}

void DatabaseConfigurator::MySQL(void)
{
  setCurrentWidget (ui->MySQL                           ) ;
  Plan->Talk       (tr("Setup MySQL configuration")     ) ;
}

void DatabaseConfigurator::PostgreSQL(void)
{
  setCurrentWidget (ui->PostgreSQL                      ) ;
  Plan->Talk       (tr("Setup PostgreSQL configuration")) ;
}

void DatabaseConfigurator::Firebird(void)
{
  setCurrentWidget (ui->Firebird                        ) ;
  Plan->Talk       (tr("Setup Firebird configuration")  ) ;
}

void DatabaseConfigurator::ODBC(void)
{
  setCurrentWidget (ui->ODBC                            ) ;
  Plan->Talk       (tr("Setup ODBC configuration")      ) ;
}

void DatabaseConfigurator::SQLite(void)
{
  setCurrentWidget (ui->SQLite                          ) ;
  Plan->Talk       (tr("Setup SQLite configuration")    ) ;
}

void DatabaseConfigurator::TestMySQL(void)
{
  bool valid = false                                               ;
  nSql Sql                                                         ;
  Sql . SqlMode      = "MYSQL"                                     ;
  Sql . Profile      = ui->myProfileEdit->text()                   ;
  Sql . Hostname     = ui->myHostEdit   ->text()                   ;
  Sql . Username     = ui->myUserEdit   ->text()                   ;
  Sql . Password     = ui->myPassEdit   ->text()                   ;
  Sql . dbName       = ui->mySchemeEdit ->text()                   ;
  Sql . Options      = ui->myOptionEdit ->text()                   ;
  Sql . Indexing     = ui->myIndexBox   ->isChecked()              ;
  Sql . isForeignKey = ui->myForeignBox ->isChecked()              ;
  if (Sql.Profile.length()>0) valid = true                         ;
  if (valid) valid = Sql.isValid()                                 ;
  if (!valid) return                                               ;
  QString Q                                                        ;
  Q = tr("Test MySQL %1 connection").arg(Sql.Profile)              ;
  Plan->Talk        (Q)                                            ;
  Plan->showMessage (Q)                                            ;
  Plan->processEvents()                                            ;
  nSqlConnection SC(Sql)                                           ;
  bool successful = false                                          ;
  if (SC.open("DatabaseConfigurator","TestMySQL"))                 {
    bool connectivity = false                                      ;
    Q = tr("MySQL %1 is connected").arg(Sql.Profile)               ;
    Plan->Talk        (Q)                                          ;
    Plan->showMessage (Q)                                          ;
    Plan->processEvents()                                          ;
    Q = tr("Test creating tables and paraments")                   ;
    Plan->Talk        (Q)                                          ;
    Plan->showMessage (Q)                                          ;
    Plan->processEvents()                                          ;
    Q = "create table DatabaseConfiguratorTestMySQL ("
        " id integer primary key auto_increment,"
        " ltime timestamp default CURRENT_TIMESTAMP"
        " on update CURRENT_TIMESTAMP"
        ");"                                                       ;
    if (SC.Query(Q))                                               {
      connectivity = true                                          ;
    } else Notify(SC.q.lastError().text())                         ;
    if (connectivity)                                              {
      Q = "drop table DatabaseConfiguratorTestMySQL ;"             ;
      if (SC.Query(Q))                                             {
        successful = true                                          ;
      } else connectivity = false                                  ;
    }                                                              ;
    if (successful)                                                {
      Q = "create table TestMySQLIndex ("
          " id integer primary key auto_increment,"
          " uuid bigint unsigned not null,"
          " ltime timestamp default CURRENT_TIMESTAMP"
          " on update CURRENT_TIMESTAMP,"
          " index `uuidindex` (uuid)"
          ");"                                                     ;
      if (SC.Query(Q))                                             {
        connectivity = true                                        ;
      } else connectivity = false                                  ;
      if (connectivity) {
        Q = "create table TestMySQLForeignKey ("
            " id integer primary key auto_increment,"
            " uuid bigint unsigned not null,"
            " ltime timestamp default CURRENT_TIMESTAMP"
            " on update CURRENT_TIMESTAMP,"
            " foreign key (uuid) references TestMySQLIndex(uuid)"
            ");"                                                   ;
        if (SC.Query(Q))                                           {
          connectivity = true                                      ;
        } else connectivity = false                                ;
      }                                                            ;
      if (!SC.Query("drop table TestMySQLForeignKey ;"))           {
        successful = false                                         ;
      }                                                            ;
      if (!SC.Query("drop table TestMySQLIndex ;"))                {
        successful = false                                         ;
      }                                                            ;
    }                                                              ;
    if (!successful)                                               {
      Q = tr("MySQL %1 operation is not correct").arg(Sql.Profile) ;
      Plan->Talk        (Q)                                        ;
      Plan->showMessage (Q)                                        ;
      Plan->processEvents()                                        ;
    }                                                              ;
    SC.close()                                                     ;
  } else                                                           {
    Q = tr("MySQL %1 can not connect").arg(Sql.Profile)            ;
    Plan->Talk        (Q)                                          ;
    Plan->showMessage (Q)                                          ;
    Plan->processEvents()                                          ;
  }                                                                ;
  SC.remove()                                                      ;
  if (successful)                                                  {
    Q = tr("MySQL %1 is successful").arg(Sql.Profile)              ;
    Plan->Talk        (Q)                                          ;
    Plan->showMessage (Q)                                          ;
    Plan->processEvents()                                          ;
    ui->myApplyButton->setEnabled(true)                            ;
  } else                                                           {
    ui->myApplyButton->setEnabled(false)                           ;
  }                                                                ;
}

void DatabaseConfigurator::TestPostgreSQL(void)
{
  bool valid = false                                                    ;
  nSql Sql                                                              ;
  Sql . SqlMode      = "PQSQL"                                          ;
  Sql . Profile      = ui->pqProfileEdit->text()                        ;
  Sql . Hostname     = ui->pqHostEdit   ->text()                        ;
  Sql . Username     = ui->pqUserEdit   ->text()                        ;
  Sql . Password     = ui->pqPassEdit   ->text()                        ;
  Sql . dbName       = ui->pqSchemeEdit ->text()                        ;
  Sql . Options      = ui->pqOptionEdit ->text()                        ;
  Sql . Indexing     = ui->pqIndexBox   ->isChecked()                   ;
  Sql . isForeignKey = ui->pqForeignBox ->isChecked()                   ;
  if (Sql.Profile.length()>0) valid = true                              ;
  if (valid) valid = Sql.isValid()                                      ;
  if (!valid) return                                                    ;
  QString Q = tr("Test PostgreSQL %1 connection").arg(Sql.Profile)      ;
  Plan->Talk        (Q)                                                 ;
  Plan->showMessage (Q)                                                 ;
  Plan->processEvents()                                                 ;
  nSqlConnection SC(Sql)                                                ;
  bool successful = false                                               ;
  if (SC.open("DatabaseConfigurator","TestPostgreSQL"))                 {
    bool connectivity = false                                           ;
    Q = tr("PostgreSQL %1 is connected").arg(Sql.Profile)               ;
    Plan->Talk        (Q)                                               ;
    Plan->showMessage (Q)                                               ;
    Plan->processEvents()                                               ;
    Q = tr("Test creating tables and paraments")                        ;
    Plan->Talk        (Q)                                               ;
    Plan->showMessage (Q)                                               ;
    Plan->processEvents()                                               ;
    Q = "create table DatabaseConfiguratorTestPostgreSQL ("
        " id serial,"
        " ltime timestamp default CURRENT_TIMESTAMP"
        ");"                                                            ;
    if (SC.Query(Q))                                                    {
      connectivity = true                                               ;
    } else Notify(SC.q.lastError().text())                              ;
    if (connectivity)                                                   {
      Q = "drop table DatabaseConfiguratorTestPostgreSQL ;"             ;
      if (SC.Query(Q))                                                  {
        successful = true                                               ;
      } else connectivity = false                                       ;
    }                                                                   ;
    if (successful)                                                     {
      Q = "create table TestPostgreSQLIndex ("
          " id serial,"
          " uuid bigint not null,"
          " ltime timestamp default CURRENT_TIMESTAMP,"
          " unique (uuid)"
          ");"                                                          ;
      if (SC.Query(Q))                                                  {
        connectivity = true                                             ;
      } else                                                            {
        Notify(SC.q.lastError().text())                                 ;
        connectivity = false                                            ;
      }                                                                 ;
      if (connectivity) {
        Q = "create table TestPostgreSQLForeignKey ("
            " id serial,"
            " uuid bigint not null,"
            " ltime timestamp default CURRENT_TIMESTAMP,"
            " foreign key (uuid) references TestPostgreSQLIndex(uuid)"
            ");"                                                        ;
        if (SC.Query(Q))                                                {
          connectivity = true                                           ;
        } else                                                          {
          Notify(SC.q.lastError().text())                               ;
          connectivity = false                                          ;
        }                                                               ;
      }                                                                 ;
      if (!SC.Query("drop table TestPostgreSQLForeignKey ;"))           {
        successful = false                                              ;
      }                                                                 ;
      if (!SC.Query("drop table TestPostgreSQLIndex ;"))                {
        successful = false                                              ;
      }                                                                 ;
    }                                                                   ;
    if (!successful)                                                    {
      Q = tr("PostgreSQL %1 operation is not correct").arg(Sql.Profile) ;
      Plan->Talk        (Q)                                             ;
      Plan->showMessage (Q)                                             ;
      Plan->processEvents()                                             ;
    }                                                                   ;
    SC.close()                                                          ;
  } else                                                                {
    Q = tr("PostgreSQL %1 can not connect").arg(Sql.Profile)            ;
    Plan->Talk        (Q)                                               ;
    Plan->showMessage (Q)                                               ;
    Plan->processEvents()                                               ;
  }                                                                     ;
  SC.remove()                                                           ;
  if (successful)                                                       {
    Q = tr("PostgreSQL %1 is successful").arg(Sql.Profile)              ;
    Plan->Talk        (Q)                                               ;
    Plan->showMessage (Q)                                               ;
    Plan->processEvents()                                               ;
    ui->pqApplyButton->setEnabled(true)                                 ;
  } else                                                                {
    ui->pqApplyButton->setEnabled(false)                                ;
  }                                                                     ;
}

void DatabaseConfigurator::TestFirebird(void)
{
  bool valid = false                                                  ;
  nSql Sql                                                            ;
  Sql . SqlMode      = "FireBird"                                     ;
  Sql . Profile      = ui->fbProfileEdit->text()                      ;
  Sql . Hostname     = ""                                             ;
  Sql . Username     = ui->fbUserEdit   ->text()                      ;
  Sql . Password     = ui->fbPassEdit   ->text()                      ;
  Sql . dbName       = ui->fbFileEdit   ->text()                      ;
  Sql . Options      = ui->fbOptionEdit ->text()                      ;
  Sql . Indexing     = ui->fbIndexBox   ->isChecked()                 ;
  Sql . isForeignKey = ui->fbForeignBox ->isChecked()                 ;
  if (Sql.Profile.length()>0) valid = true                            ;
  if (valid) valid = Sql.isValid()                                    ;
  if (!valid) return                                                  ;
  QString Q = tr("Test Firebird %1 connection").arg(Sql.Profile)      ;
  Plan->Talk        (Q)                                               ;
  Plan->showMessage (Q)                                               ;
  Plan->processEvents()                                               ;
  nSqlConnection SC(Sql)                                              ;
  bool successful = false                                             ;
  if (SC.open("DatabaseConfigurator","TestFireBird"))                 {
    bool connectivity = false                                         ;
    Q = tr("Firebird %1 is connected").arg(Sql.Profile)               ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    Q = tr("Test creating tables and paraments")                      ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    Q = "create table dbConfTestFirebird ("
        " id integer primary key,"
        " ltime timestamp"
        ");"                                                          ;
    if (SC.Query(Q))                                                  {
      connectivity = true                                             ;
    } else Notify(SC.q.lastError().text())                            ;
    if (connectivity)                                                 {
      Q = "drop table dbConfTestFirebird ;"                           ;
      if (SC.Query(Q))                                                {
        successful = true                                             ;
      } else connectivity = false                                     ;
    }                                                                 ;
    if (successful)                                                   {
      Q = "create table TestFirebirdIndex ("
          " id integer primary key,"
          " uuid bigint not null,"
          " ltime timestamp default CURRENT_TIMESTAMP,"
          " unique (uuid)"
          ");"                                                        ;
      if (SC.Query(Q))                                                {
        connectivity = true                                           ;
      } else                                                          {
        Notify(SC.q.lastError().text())                               ;
        connectivity = false                                          ;
      }                                                               ;
      if (connectivity) {
        Q = "create table TestFirebirdFKey ("
            " id integer primary key,"
            " uuid bigint not null,"
            " ltime timestamp default CURRENT_TIMESTAMP,"
            " foreign key (uuid) references TestFirebirdIndex(uuid)"
            ");"                                                      ;
        if (SC.Query(Q))                                              {
          connectivity = true                                         ;
        } else                                                        {
          Notify(SC.q.lastError().text())                             ;
          connectivity = false                                        ;
        }                                                             ;
      }                                                               ;
      if (!SC.Query("drop table TestFirebirdFKey ;"))                 {
        successful = false                                            ;
      }                                                               ;
      if (!SC.Query("drop table TestFirebirdIndex ;"))                {
        successful = false                                            ;
      }                                                               ;
    }                                                                 ;
    if (!successful)                                                  {
      Q = tr("Firebird %1 operation is not correct").arg(Sql.Profile) ;
      Plan->Talk        (Q)                                           ;
      Plan->showMessage (Q)                                           ;
      Plan->processEvents()                                           ;
    }                                                                 ;
    SC.close()                                                        ;
  } else                                                              {
    Q = tr("Firebird %1 can not connect").arg(Sql.Profile)            ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
  }                                                                   ;
  SC.remove()                                                         ;
  if (successful)                                                     {
    Q = tr("Firebird %1 is successful").arg(Sql.Profile)              ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    ui->fbApplyButton->setEnabled(true)                               ;
  } else                                                              {
    ui->fbApplyButton->setEnabled(false)                              ;
  }                                                                   ;
}

void DatabaseConfigurator::TestODBC(void)
{
  bool valid = false                                                  ;
  nSql Sql                                                            ;
  Sql . SqlMode      = "ODBC"                                         ;
  Sql . Profile      = ui->ocProfileEdit->text()                      ;
  Sql . Hostname     = ui->ocHostEdit   ->text()                      ;
  Sql . Username     = ui->ocUserEdit   ->text()                      ;
  Sql . Password     = ui->ocPassEdit   ->text()                      ;
  Sql . dbName       = ui->ocSchemeEdit ->text()                      ;
  Sql . Options      = ui->ocOptionEdit ->text()                      ;
  Sql . Indexing     = ui->ocIndexBox   ->isChecked()                 ;
  Sql . isForeignKey = ui->ocForeignBox ->isChecked()                 ;
  if (Sql.Profile.length()>0) valid = true                            ;
  if (valid) valid = Sql.isValid()                                    ;
  if (!valid) return                                                  ;
  QString Q = tr("Test ODBC %1 connection").arg(Sql.Profile)          ;
  Plan->Talk        (Q)                                               ;
  Plan->showMessage (Q)                                               ;
  Plan->processEvents()                                               ;
  nSqlConnection SC(Sql)                                              ;
  bool successful = false                                             ;
  if (SC.open("DatabaseConfigurator","TestODBC"))                     {
    bool connectivity = false                                         ;
    Q = tr("ODBC %1 is connected").arg(Sql.Profile)                   ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    Q = tr("Test creating tables and paraments")                      ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    Q = "create table DatabaseConfiguratorTestODBC ("
        " id integer primary key,"
        " ltime timestamp"
        ");"                                                          ;
    if (SC.Query(Q))                                                  {
      connectivity = true                                             ;
    } else Notify(SC.q.lastError().text())                            ;
    if (connectivity)                                                 {
      Q = "drop table DatabaseConfiguratorTestODBC ;"                 ;
      if (SC.Query(Q))                                                {
        successful = true                                             ;
      } else connectivity = false                                     ;
    }                                                                 ;
    if (successful)                                                   {
      Q = "create table TestOdbcIndex ("
          " id integer primary key,"
          " uuid bigint not null,"
          " ltime timestamp default CURRENT_TIMESTAMP,"
          " unique (uuid)"
          ");"                                                        ;
      if (SC.Query(Q))                                                {
        connectivity = true                                           ;
      } else                                                          {
        Notify(SC.q.lastError().text())                               ;
        connectivity = false                                          ;
      }                                                               ;
      if (connectivity) {
        Q = "create table TestOdbcForeignKey ("
            " id integer primary key,"
            " uuid bigint not null,"
            " ltime timestamp default CURRENT_TIMESTAMP,"
            " foreign key (uuid) references TestOdbcIndex(uuid)"
            ");"                                                      ;
        if (SC.Query(Q))                                              {
          connectivity = true                                         ;
        } else                                                        {
          Notify(SC.q.lastError().text())                             ;
          connectivity = false                                        ;
        }                                                             ;
      }                                                               ;
      if (!SC.Query("drop table TestOdbcForeignKey ;"))               {
        successful = false                                            ;
      }                                                               ;
      if (!SC.Query("drop table TestOdbcIndex ;"))                    {
        successful = false                                            ;
      }                                                               ;
    }                                                                 ;
    if (!successful)                                                  {
      Q = tr("ODBC %1 operation is not correct").arg(Sql.Profile)     ;
      Plan->Talk        (Q)                                           ;
      Plan->showMessage (Q)                                           ;
      Plan->processEvents()                                           ;
    }                                                                 ;
    SC.close()                                                        ;
  } else                                                              {
    Q = tr("ODBC %1 can not connect").arg(Sql.Profile)                ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
  }                                                                   ;
  SC.remove()                                                         ;
  if (successful)                                                     {
    Q = tr("ODBC %1 is successful").arg(Sql.Profile)                  ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    ui->ocApplyButton->setEnabled(true)                               ;
  } else                                                              {
    ui->ocApplyButton->setEnabled(false)                              ;
  }                                                                   ;
}

void DatabaseConfigurator::TestSQLite(void)
{
  bool valid = false                                                  ;
  nSql Sql                                                            ;
  Sql . SqlMode      = "SQLITE"                                       ;
  Sql . Profile      = ui->sqProfileEdit->text()                      ;
  Sql . Hostname     = ""                                             ;
  Sql . Username     = ""                                             ;
  Sql . Password     = ""                                             ;
  Sql . dbName       = ui->sqFileEdit   ->text()                      ;
  Sql . Options      = ui->sqOptionEdit ->text()                      ;
  Sql . Indexing     = ui->sqIndexBox   ->isChecked()                 ;
  Sql . isForeignKey = ui->sqForeignBox ->isChecked()                 ;
  if (Sql.Profile.length()>0) valid = true                            ;
  if (valid) valid = Sql.isValid()                                    ;
  if (!valid) return                                                  ;
  QString Q = tr("Test SQLite %1 connection").arg(Sql.Profile)        ;
  Plan->Talk        (Q)                                               ;
  Plan->showMessage (Q)                                               ;
  Plan->processEvents()                                               ;
  nSqlConnection SC(Sql)                                              ;
  bool successful = false                                             ;
  if (SC.open("DatabaseConfigurator","TestSQLite"))                   {
    bool connectivity = false                                         ;
    Q = tr("SQLite %1 is connected").arg(Sql.Profile)                 ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    Q = tr("Test creating tables and paraments")                      ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    Q = "create table DatabaseConfiguratorTestSQLITE ("
        " id integer primary key,"
        " ltime timestamp"
        ");"                                                          ;
    if (SC.Query(Q))                                                  {
      connectivity = true                                             ;
    } else Notify(SC.q.lastError().text())                            ;
    if (connectivity)                                                 {
      Q = "drop table DatabaseConfiguratorTestSQLITE ;"               ;
      if (SC.Query(Q))                                                {
        successful = true                                             ;
      } else connectivity = false                                     ;
    }                                                                 ;
    if (successful)                                                   {
      Q = "create table TestSQLiteIndex ("
          " id integer primary key autoincrement,"
          " uuid bigint not null,"
          " ltime timestamp,"
          " unique (uuid)"
          ");"                                                        ;
      if (SC.Query(Q))                                                {
        connectivity = true                                           ;
      } else                                                          {
        Notify(SC.q.lastError().text())                               ;
        connectivity = false                                          ;
      }                                                               ;
      if (connectivity) {
        Q = "create table TestSQLiteForeignKey ("
            " id integer primary key autoincrement,"
            " uuid bigint not null,"
            " ltime timestamp,"
            " foreign key (uuid) references TestSQLiteIndex(uuid)"
            ");"                                                      ;
        if (SC.Query(Q))                                              {
          connectivity = true                                         ;
        } else {
          Notify(SC.q.lastError().text())                             ;
          connectivity = false                                        ;
        }                                                             ;
      }                                                               ;
      if (!SC.Query("drop table TestSQLiteForeignKey ;"))             {
        successful = false                                            ;
      }                                                               ;
      if (!SC.Query("drop table TestSQLiteIndex ;"))                  {
        successful = false                                            ;
      }                                                               ;
    }                                                                 ;
    if (!successful)                                                  {
      Q = tr("SQLite %1 operation is not correct").arg(Sql.Profile)   ;
      Plan->Talk        (Q)                                           ;
      Plan->showMessage (Q)                                           ;
      Plan->processEvents()                                           ;
    }                                                                 ;
    SC.close()                                                        ;
  } else                                                              {
    Q = tr("SQLite %1 can not connect").arg(Sql.Profile)              ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
  }                                                                   ;
  SC.remove()                                                         ;
  if (successful)                                                     {
    Q = tr("SQLite %1 is successful").arg(Sql.Profile)                ;
    Plan->Talk        (Q)                                             ;
    Plan->showMessage (Q)                                             ;
    Plan->processEvents()                                             ;
    ui->sqApplyButton->setEnabled(true)                               ;
  } else                                                              {
    ui->sqApplyButton->setEnabled(false)                              ;
  }                                                                   ;
}

void DatabaseConfigurator::ApplyMySQL(void)
{
  bool valid = false                                  ;
  nSql Sql                                            ;
  Sql . SqlMode      = "MYSQL"                        ;
  Sql . Profile      = ui->myProfileEdit->text()      ;
  Sql . Hostname     = ui->myHostEdit   ->text()      ;
  Sql . Username     = ui->myUserEdit   ->text()      ;
  Sql . Password     = ui->myPassEdit   ->text()      ;
  Sql . dbName       = ui->mySchemeEdit ->text()      ;
  Sql . Options      = ui->myOptionEdit ->text()      ;
  Sql . Indexing     = ui->myIndexBox   ->isChecked() ;
  Sql . isForeignKey = ui->myForeignBox ->isChecked() ;
  if (Sql.Profile.length()>0) valid = true            ;
  if (valid) valid = Sql.isValid()                    ;
  if (!valid) return                                  ;
  emit apply (this,Sql)                               ;
  emit apply (Uuid,Sql)                               ;
}

void DatabaseConfigurator::ApplyPostgreSQL(void)
{
  bool valid = false                                  ;
  nSql Sql                                            ;
  Sql . SqlMode      = "PQSQL"                        ;
  Sql . Profile      = ui->pqProfileEdit->text()      ;
  Sql . Hostname     = ui->pqHostEdit   ->text()      ;
  Sql . Username     = ui->pqUserEdit   ->text()      ;
  Sql . Password     = ui->pqPassEdit   ->text()      ;
  Sql . dbName       = ui->pqSchemeEdit ->text()      ;
  Sql . Options      = ui->pqOptionEdit ->text()      ;
  Sql . Indexing     = ui->pqIndexBox   ->isChecked() ;
  Sql . isForeignKey = ui->pqForeignBox ->isChecked() ;
  if (Sql.Profile.length()>0) valid = true            ;
  if (valid) valid = Sql.isValid()                    ;
  if (!valid) return                                  ;
  emit apply (this,Sql)                               ;
  emit apply (Uuid,Sql)                               ;
}

void DatabaseConfigurator::ApplyFirebird(void)
{
  bool valid = false                                  ;
  nSql Sql                                            ;
  Sql . SqlMode      = "FireBird"                     ;
  Sql . Profile      = ui->fbProfileEdit->text()      ;
  Sql . Hostname     = ""                             ;
  Sql . Username     = ui->fbUserEdit   ->text()      ;
  Sql . Password     = ui->fbPassEdit   ->text()      ;
  Sql . dbName       = ui->fbFileEdit   ->text()      ;
  Sql . Options      = ui->fbOptionEdit ->text()      ;
  Sql . Indexing     = ui->fbIndexBox   ->isChecked() ;
  Sql . isForeignKey = ui->fbForeignBox ->isChecked() ;
  if (Sql.Profile.length()>0) valid = true            ;
  if (valid) valid = Sql.isValid()                    ;
  if (!valid) return                                  ;
  emit apply (this,Sql)                               ;
  emit apply (Uuid,Sql)                               ;
}

void DatabaseConfigurator::ApplyODBC(void)
{
  bool valid = false                                  ;
  nSql Sql                                            ;
  Sql . SqlMode      = "ODBC"                         ;
  Sql . Profile      = ui->ocProfileEdit->text()      ;
  Sql . Hostname     = ui->ocHostEdit   ->text()      ;
  Sql . Username     = ui->ocUserEdit   ->text()      ;
  Sql . Password     = ui->ocPassEdit   ->text()      ;
  Sql . dbName       = ui->ocSchemeEdit ->text()      ;
  Sql . Options      = ui->ocOptionEdit ->text()      ;
  Sql . Indexing     = ui->ocIndexBox   ->isChecked() ;
  Sql . isForeignKey = ui->ocForeignBox ->isChecked() ;
  if (Sql.Profile.length()>0) valid = true            ;
  if (valid) valid = Sql.isValid()                    ;
  if (!valid) return                                  ;
  emit apply (this,Sql)                               ;
  emit apply (Uuid,Sql)                               ;
}

void DatabaseConfigurator::ApplySQLite(void)
{
  bool valid = false                                  ;
  nSql Sql                                            ;
  Sql . SqlMode      = "SQLITE"                       ;
  Sql . Profile      = ui->sqProfileEdit->text()      ;
  Sql . Hostname     = ""                             ;
  Sql . Username     = ""                             ;
  Sql . Password     = ""                             ;
  Sql . dbName       = ui->sqFileEdit   ->text()      ;
  Sql . Options      = ui->sqOptionEdit ->text()      ;
  Sql . Indexing     = ui->sqIndexBox   ->isChecked() ;
  Sql . isForeignKey = ui->sqForeignBox ->isChecked() ;
  if (Sql.Profile.length()>0) valid = true            ;
  if (valid) valid = Sql.isValid()                    ;
  if (!valid) return                                  ;
  emit apply (this,Sql)                               ;
  emit apply (Uuid,Sql)                               ;
}

void DatabaseConfigurator::StringMySQL(QString text)
{
  bool valid = false                       ;
  nSql Sql                                 ;
  Sql . SqlMode = "MYSQL"                  ;
  Sql.Profile  = ui->myProfileEdit->text() ;
  Sql.Hostname = ui->myHostEdit   ->text() ;
  Sql.Username = ui->myUserEdit   ->text() ;
  Sql.Password = ui->myPassEdit   ->text() ;
  Sql.dbName   = ui->mySchemeEdit ->text() ;
  Sql.Options  = ui->myOptionEdit ->text() ;
  if (Sql.Profile.length()>0) valid = true ;
  if (valid) valid = Sql.isValid()         ;
  ui->myTestButton ->setEnabled(valid)     ;
  ui->myApplyButton->setEnabled(false)     ;
}

void DatabaseConfigurator::StringPostgreSQL(QString text)
{
  bool valid = false                       ;
  nSql Sql                                 ;
  Sql . SqlMode = "PQSQL"                  ;
  Sql.Profile  = ui->pqProfileEdit->text() ;
  Sql.Hostname = ui->pqHostEdit   ->text() ;
  Sql.Username = ui->pqUserEdit   ->text() ;
  Sql.Password = ui->pqPassEdit   ->text() ;
  Sql.dbName   = ui->pqSchemeEdit ->text() ;
  Sql.Options  = ui->pqOptionEdit ->text() ;
  if (Sql.Profile.length()>0) valid = true ;
  if (valid) valid = Sql.isValid()         ;
  ui->pqTestButton ->setEnabled(valid)     ;
  ui->pqApplyButton->setEnabled(false)     ;
}

void DatabaseConfigurator::StringFirebird(QString text)
{
  bool valid = false                       ;
  nSql Sql                                 ;
  Sql . SqlMode = "FireBird"               ;
  Sql.Profile  = ui->fbProfileEdit->text() ;
  Sql.Hostname = ""                        ;
  Sql.Username = ui->fbUserEdit   ->text() ;
  Sql.Password = ui->fbPassEdit   ->text() ;
  Sql.dbName   = ui->fbFileEdit   ->text() ;
  Sql.Options  = ui->fbOptionEdit ->text() ;
  if (Sql.Profile.length()>0) valid = true ;
  if (valid) valid = Sql.isValid()         ;
  ui->fbTestButton ->setEnabled(valid)     ;
  ui->fbApplyButton->setEnabled(false)     ;
}

void DatabaseConfigurator::StringODBC(QString text)
{
  bool valid = false                       ;
  nSql Sql                                 ;
  Sql . SqlMode = "ODBC"                   ;
  Sql.Profile  = ui->ocProfileEdit->text() ;
  Sql.Hostname = ui->ocHostEdit   ->text() ;
  Sql.Username = ui->ocUserEdit   ->text() ;
  Sql.Password = ui->ocPassEdit   ->text() ;
  Sql.dbName   = ui->ocSchemeEdit ->text() ;
  Sql.Options  = ui->ocOptionEdit ->text() ;
  if (Sql.Profile.length()>0) valid = true ;
  if (valid) valid = Sql.isValid()         ;
  ui->ocTestButton ->setEnabled(valid)     ;
  ui->ocApplyButton->setEnabled(false)     ;
}

void DatabaseConfigurator::StringSQLite(QString text)
{
  bool valid = false                       ;
  nSql Sql                                 ;
  Sql . SqlMode = "SQLITE"                 ;
  Sql.Profile  = ui->sqProfileEdit->text() ;
  Sql.Hostname = ""                        ;
  Sql.Username = ""                        ;
  Sql.Password = ""                        ;
  Sql.dbName   = ui->sqFileEdit   ->text() ;
  Sql.Options  = ui->sqOptionEdit ->text() ;
  if (Sql.Profile.length()>0) valid = true ;
  if (valid) valid = Sql.isValid()         ;
  ui->sqTestButton ->setEnabled(valid)     ;
  ui->sqApplyButton->setEnabled(false)     ;
}

void DatabaseConfigurator::BrowseFirebird(void)
{
  QString filename = QFileDialog::getSaveFileName(this,tr("Firebird database file"),"","*.*");
  if (filename.length()<=0) return ;
  ui->fbFileEdit->setText(filename);
}

void DatabaseConfigurator::BrowseSQLite(void)
{
  QString filename = QFileDialog::getSaveFileName(this,tr("SQLite database file"),"","*.*");
  if (filename.length()<=0) return ;
  ui->sqFileEdit->setText(filename);
}

void DatabaseConfigurator::set(nSql & Sql)
{
  if (Sql.isMySQL   ()) {
    ui->myProfileEdit->setText    (Sql . Profile     ) ;
    ui->myHostEdit   ->setText    (Sql . Hostname    ) ;
    ui->myUserEdit   ->setText    (Sql . Username    ) ;
    ui->myPassEdit   ->setText    (Sql . Password    ) ;
    ui->mySchemeEdit ->setText    (Sql . dbName      ) ;
    ui->myOptionEdit ->setText    (Sql . Options     ) ;
    ui->myIndexBox   ->setChecked (Sql . Indexing    ) ;
    ui->myForeignBox ->setChecked (Sql . isForeignKey) ;
    MySQL      ()                                      ;
  } else
  if (Sql.isPQSQL   ()) {
    ui->pqProfileEdit->setText    (Sql . Profile     ) ;
    ui->pqHostEdit   ->setText    (Sql . Hostname    ) ;
    ui->pqUserEdit   ->setText    (Sql . Username    ) ;
    ui->pqPassEdit   ->setText    (Sql . Password    ) ;
    ui->pqSchemeEdit ->setText    (Sql . dbName      ) ;
    ui->pqOptionEdit ->setText    (Sql . Options     ) ;
    ui->pqIndexBox   ->setChecked (Sql . Indexing    ) ;
    ui->pqForeignBox ->setChecked (Sql . isForeignKey) ;
    PostgreSQL ()                                      ;
  } else
  if (Sql.isFireBird()) {
    ui->fbProfileEdit->setText    (Sql . Profile     ) ;
    ui->fbUserEdit   ->setText    (Sql . Username    ) ;
    ui->fbPassEdit   ->setText    (Sql . Password    ) ;
    ui->fbFileEdit   ->setText    (Sql . dbName      ) ;
    ui->fbOptionEdit ->setText    (Sql . Options     ) ;
    ui->fbIndexBox   ->setChecked (Sql . Indexing    ) ;
    ui->fbForeignBox ->setChecked (Sql . isForeignKey) ;
    Firebird   ()       ;
  } else
  if (Sql.isODBC    ()) {
    ui->ocProfileEdit->setText    (Sql . Profile     ) ;
    ui->ocHostEdit   ->setText    (Sql . Hostname    ) ;
    ui->ocUserEdit   ->setText    (Sql . Username    ) ;
    ui->ocPassEdit   ->setText    (Sql . Password    ) ;
    ui->ocSchemeEdit ->setText    (Sql . dbName      ) ;
    ui->ocOptionEdit ->setText    (Sql . Options     ) ;
    ui->ocIndexBox   ->setChecked (Sql . Indexing    ) ;
    ui->ocForeignBox ->setChecked (Sql . isForeignKey) ;
    ODBC       ()                                      ;
  } else
  if (Sql.isSQLITE  ()) {
    ui->sqProfileEdit->setText    (Sql . Profile     ) ;
    ui->sqFileEdit   ->setText    (Sql . dbName      ) ;
    ui->sqOptionEdit ->setText    (Sql . Options     ) ;
    ui->sqIndexBox   ->setChecked (Sql . Indexing    ) ;
    ui->sqForeignBox ->setChecked (Sql . isForeignKey) ;
    SQLite     ()                                      ;
  }                                                    ;
}
