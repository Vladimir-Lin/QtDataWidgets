#include <datawidgets.h>

N::FileLaunchers:: FileLaunchers ( QWidget * parent , Plan * p )
                 : TreeWidget    (           parent ,        p )
                 , MimeManager   (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::FileLaunchers::~FileLaunchers(void)
{
}

void N::FileLaunchers::Configure(void)
{
  NewTreeWidgetItem            ( head                   )        ;
  head -> setText              ( 0 , tr("Extension"   ) )        ;
  head -> setText              ( 1 , tr("Application" ) )        ;
  head -> setText              ( 2 , tr("Paraments"   ) )        ;
  for (int i=0;i<3;i++)                                          {
    head -> setTextAlignment   ( i , Qt::AlignCenter    )        ;
  }                                                              ;
  setWindowTitle               ( tr("File launchers"  ) )        ;
  setDragDropMode              ( NoDragDrop             )        ;
  setRootIsDecorated           ( false                  )        ;
  setAlternatingRowColors      ( true                   )        ;
  setSelectionMode             ( SingleSelection        )        ;
  setColumnCount               ( 3                      )        ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded  )        ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded  )        ;
  setHeaderItem                ( head                   )        ;
  plan -> setFont              ( this                   )        ;
  ////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::FileLaunchers::FocusIn(void)
{
  LinkAction ( Refresh , startup        () ) ;
  LinkAction ( Insert  , Insert         () ) ;
  LinkAction ( Delete  , Delete         () ) ;
  LinkAction ( Copy    , CopyToClipboard() ) ;
  return true                                ;
}

bool N::FileLaunchers::startup(void)
{
  clear            (                )   ;
  Extensions . clear ( )                ;
  Suffixes   . clear ( )                ;
  EnterSQL         ( SC , plan->sql )   ;
    QString Q                           ;
    load ( SC )                         ;
    Q = SC.sql.SelectFrom               (
          "extension,name,paraments"    ,
          PlanTable(Applications)       ,
          "order by id asc "          ) ;
    SqlLoopNow ( SC , Q )               ;
      NewTreeWidgetItem ( item )        ;
      int     eid  = SC.Int   (0)       ;
      QString name = SC.String(1)       ;
      QString para = SC.String(2)       ;
      item->setData(0,Qt::UserRole,eid) ;
      item->setText(0,Suffixes[eid])    ;
      item->setText(1,name         )    ;
      item->setText(2,para         )    ;
      addTopLevelItem(item)             ;
    SqlLoopErr ( SC , Q )               ;
    SqlLoopEnd ( SC , Q )               ;
  LeaveSQL         ( SC , plan->sql )   ;
  SuitableColumns  (                )   ;
  Alert            ( Done           )   ;
  return true                           ;
}

void N::FileLaunchers::Insert(void)
{
  NewTreeWidgetItem(item)         ;
  item->setData(0,Qt::UserRole,0) ;
  addTopLevelItem(item)           ;
  doubleClicked(item,0)           ;
}

void N::FileLaunchers::Delete(void)
{
  QTreeWidgetItem * item = currentItem() ;
  if (IsNull(item)) return               ;
  int ext   = nTreeUuid(item,0)          ;
  int index = indexOfTopLevelItem(item)  ;
  EnterSQL  ( SC , plan->sql           ) ;
    QString Q                            ;
    Q = SC.sql.DeleteFrom                (
          PlanTable(Applications)        ,
          QString("where extension = %1" )
          .arg(ext)                    ) ;
    SC.Query(Q)                          ;
    takeTopLevelItem(index)              ;
  LeaveSQL  ( SC , plan->sql           ) ;
  Alert     ( Done                     ) ;
}

bool N::FileLaunchers::Menu(QPoint pos)
{
  return true ;
}

void N::FileLaunchers::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                           ;
  QComboBox * cc                             ;
  int id = nTreeUuid(item,0)                 ;
  switch (column)                            {
    case 0                                   :
      cc   = setComboBox                     (
               item,column                   ,
               SIGNAL(activated(int))        ,
               SLOT(extensionFinished(int))) ;
      N::AddItems                            (
        *cc                                  ,
        Extensions                           ,
        Suffixes                           ) ;
      if (id>0) (*cc) <= id                  ;
      cc->setEditable(true)                  ;
      cc->showPopup()                        ;
    break                                    ;
    case 1                                   :
      fetchApplication ( item )              ;
    break                                    ;
    case 2                                   :
      line = setLineEdit                     (
               item,column                   ,
               SIGNAL(editingFinished  ())   ,
               SLOT  (paramentsFinished()) ) ;
      line->setFocus( Qt::TabFocusReason )   ;
    break                                    ;
  }                                          ;
}

void N::FileLaunchers::extensionFinished(int)
{
  if (IsNull(ItemEditing)) return                ;
  QComboBox * cc = Casting(QComboBox,ItemWidget) ;
  if (IsNull(cc)) return                         ;
  int oid = nTreeUuid(ItemEditing,0)             ;
  int eid = N::GetUuid(cc)                       ;
  EnterSQL         ( SC , plan->sql          )   ;
    QString Q                                    ;
    if (oid<=0)                                  {
      Q = SC.sql.InsertInto                      (
            PlanTable(Applications)              ,
            1,"extension"                      ) ;
      SC . Prepare ( Q                 )         ;
      SC . Bind    ( "extension" , eid )         ;
      SC . Exec    (                   )         ;
    } else                                       {
      Q = SC.sql.Update(PlanTable(Applications)  ,
            "where extension = :EXT"             ,
            1,"extension"                      ) ;
      SC . Prepare ( Q                 )         ;
      SC . Bind    ( "ext"       , oid )         ;
      SC . Bind    ( "extension" , eid )         ;
      SC . Exec    (                   )         ;
    }                                            ;
    ItemEditing->setData(0,Qt::UserRole,eid  )   ;
    ItemEditing->setText(0,cc->currentText() )   ;
  LeaveSQL         ( SC , plan->sql          )   ;
  removeOldItem    ( true , 0                )   ;
  Alert            ( Done                    )   ;
}

void N::FileLaunchers::paramentsFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    id       = nTreeUuid(ItemEditing,0)      ;
  QString parament = line->text()                  ;
  EnterSQL       ( SC , plan->sql                ) ;
    QString Q                                      ;
    Q = SC.sql.Update                              (
          PlanTable(Applications)                  ,
          "where extension = :EXTENSION"           ,
          1,"paraments"                          ) ;
    SC . Prepare ( Q                             ) ;
    SC . Bind    ( "extension",id                ) ;
    SC . Bind    ( "paraments",parament.toUtf8() ) ;
    SC . Exec    (                               ) ;
    ItemEditing -> setText ( 2 , parament        ) ;
  LeaveSQL       ( SC , plan->sql                ) ;
  removeOldItem  ( true , 0                      ) ;
}

void N::FileLaunchers::fetchApplication(QTreeWidgetItem * item)
{
  QString filename = QFileDialog::getOpenFileName  (
                       this                        ,
                       tr("Application")           ,
                       plan->Dirs[Directory::Binary].absoluteFilePath("") ,
                       "*.*"                     ) ;
  if (filename.length()<=0) return                 ;
  SUID    id       = nTreeUuid(item,0)             ;
  EnterSQL       ( SC , plan->sql                ) ;
    QString Q                                      ;
    Q = SC.sql.Update                              (
          PlanTable(Applications)                  ,
          "where extension = :EXTENSION"           ,
          1,"name"                               ) ;
    SC . Prepare ( Q                             ) ;
    SC . Bind    ( "extension",id                ) ;
    SC . Bind    ( "name"     ,filename.toUtf8() ) ;
    SC . Exec    (                               ) ;
    item -> setText ( 1 , filename )               ;
  LeaveSQL       ( SC , plan->sql                ) ;
}
