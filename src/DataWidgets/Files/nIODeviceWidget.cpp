#include <nWork>

#define BeginSql(Name,DB,SQL) { QSqlDatabase DB = (SQL).db(Name)

#define EndSql(Name) }; QSqlDatabase::removeDatabase(Name)

#define StartSql(Name,DB,SQL) \
{ QSqlDatabase DB = (SQL).db(Name); \
  if (SQL.Open(DB)) {

#define StopSql(Name,DB,SQL) \
      SQL.Close(DB); \
    }; \
  }; \
  QSqlDatabase::removeDatabase(Name);

/////////////////////////////////////////////////////////

bool Neutrino::LoadIODevicesFromSql(nSqlSettings & sql,QFileInfoList & fileinfo,QList<bool> & enables)
{
  fileinfo.clear();
  enables.clear();
  BeginSql(sql.Connection,db,sql);
  if (sql.Open(db)) {
    QFileInfo * fi   ;
    QString     Q    ;
    QSqlQuery   q(db);
    Q = sql.SelectFrom("name,enable",sql.Name(sql.TableName)) + " " +
        "order by id;";
    if (q.exec(Q)) {
      while (q.next()) {
        fi = new QFileInfo();
        fi->setFile(nStringSQL(q,0));
        fileinfo << (*fi);
        enables  << q.value(1).toBool();
      };
    };
    sql.Close(db);
  };
  EndSql(sql.Connection);
  return (fileinfo.size()>0);
}

bool Neutrino::SaveIODevicesFromSql(nSqlSettings & sql,QFileInfoList & fileinfo,QList<bool> & enables)
{
  if (fileinfo.size()<=0) return false;
  if (fileinfo.size()!=enables.size()) return false;
  bool queryrt = false;
  BeginSql(sql.Connection,db,sql);
  if (sql.Open(db)) {
    QString   Q    ;
    QSqlQuery q(db);
    Q = sql.CreateTable(sql.Name(sql.TableName)) + " " +
        "(" +
        sql.PrimaryKey ("id"   )     + "," +
        sql.Uuid       ("uuid" )     + "," +
        "enable integer default 0"   + "," +
        "flags integer default 0"    + "," +
        "name blob"                  + "," +
        sql.TimestampString("ltime") + "," +
        sql.Unique("uuid")           +
        ")" + " " ;
    if (sql.isForeignKey) Q += sql.Engine("InnoDB","utf8");
    Q += ";";
    q.exec(Q);
    Q = "delete from " + sql.Name(sql.TableName) + ";";
    q.exec(Q);
    for (int i=0;i<fileinfo.size();i++) {
      Q = sql.InsertInto("uuid,enable,name",sql.Name(sql.TableName),":UUID,:ENABLE,:NAME") + ";";
      q.prepare   (Q                                          );
      q.bindValue (":UUID",nUuid::createUuidShort()           );
      q.bindValue ("enable",enables[i]                        );
      q.bindValue (":NAME",fileinfo[i].absolutePath().toUtf8());
      q.exec      (                                           );
    };
    sql.Close(db);
  };
  EndSql(sql.Connection);
  return queryrt;
}

/////////////////////////////////////////////////////////
// nIODeviceWidget - Generate by Neutrino Code Generator
/////////////////////////////////////////////////////////

nIODeviceWidget :: nIODeviceWidget (QWidget * parent)
                :  nTreeWidget     (          parent)
{
  Initialize();
}

nIODeviceWidget ::~nIODeviceWidget(void)
{
}

void nIODeviceWidget::Initialize(void)
{
  setColumnCount     (1    );
  setHeaderHidden    (true );
  setRootIsDecorated (false);
  loadDevices        (     );
}

void nIODeviceWidget::Trigger(void)
{
  QFileInfoList devices;
  QList<bool> enables;
  if (getDevices(devices,enables))
    emit fetchDevices(devices,enables);
}

bool nIODeviceWidget::getDevices(QFileInfoList & devices,QList<bool> & enables)
{
  QFileInfo * fileinfo;
  QTreeWidgetItem * item;
  devices.clear();
  enables.clear();
  for (int i=0;i<topLevelItemCount();i++) {
    item = topLevelItem(i);
    fileinfo = new  QFileInfo();
    fileinfo->setFile(item->text(0));
    devices << (*fileinfo);
    enables << (item->checkState(0)==Qt::Checked);
  };
  return (devices.size()>0);
}

void nIODeviceWidget::loadDevices(void)
{
  QList<bool>   enables;
  QFileInfoList devices;
  clear();
  devices = QDir::drives();
  if (devices.size()<0) return;
  for (int i=0;i<devices.size();i++) enables << false;
  setDevices(devices,enables);
}

void nIODeviceWidget::setDevices(QFileInfoList & devices,QList<bool> enables)
{
  QTreeWidgetItem * item;
  clear();
  for (int i=0;i<devices.size();i++) {
    item  = new QTreeWidgetItem (                                          );
    item -> setCheckState       (0,enables[i] ? Qt::Checked : Qt::Unchecked);
    item -> setText             (0,devices[i].absolutePath()               );
    addTopLevelItem             (item                                      );
  };
}
