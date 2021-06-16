#include <nWork>

nDepotFiles::nDepotFiles (QWidget * parent)
           : QWidget     (          parent)
{
  RUNSQL     = NULL;
  SQLCOMMAND = NULL;
  VIEW       = NULL;
  PROGRESS   = NULL;
  setGeometry(0,0,640,480);
}

nDepotFiles::~nDepotFiles(void)
{
}

void nDepotFiles::resizeEvent(QResizeEvent * event)
{
  QSize SIZE = event->size();
  if (SIZE.width()==0 || SIZE.height()==0) return;
  if (RUNSQL!=NULL) {
    RUNSQL     -> move (0,0);
    RUNSQL     -> resize(28,28);
  };
  if (SQLCOMMAND!=NULL) {
    SQLCOMMAND -> move(28,0);
    SQLCOMMAND -> resize(width()-28,28);
  };
  if (VIEW!=NULL) {
    VIEW       -> move(0,28);
    VIEW       -> resize(width(),height()-56);
  };
  if (PROGRESS!=NULL) {
    PROGRESS   -> move(0,height()-28);
    PROGRESS   -> resize(width(),28);
  };
}

void nDepotFiles::Initialize(void)
{
  RUNSQL     = new QToolButton  (this);
  SQLCOMMAND = new QLineEdit    (this);
  VIEW       = new QTableView   (this);
  PROGRESS   = new QProgressBar (this);
  RUNSQL    -> setText(">");
  QObject::connect(RUNSQL    ,SIGNAL(clicked(bool)    ),this,SLOT(RunSQL(bool)));
  QObject::connect(SQLCOMMAND,SIGNAL(editingFinished()),this,SLOT(RunSQL(    )));
  RUNSQL     -> show();
  SQLCOMMAND -> show();
  VIEW       -> show();
  PROGRESS   -> show();
  repaint ();
  update  ();
}

void nDepotFiles::RunSQL(bool)
{
  RunSQL();
}

void nDepotFiles::RunSQL(void)
{
  QString S = SQLCOMMAND->text();
  if (S.length()<=0) return;
  setCursor(Qt::WaitCursor);
  DEPOT->SqlView(S,VIEW);
  setCursor(Qt::ArrowCursor);
}
