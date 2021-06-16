#include <datawidgets.h>

#pragma message("MimeSuffix requires additional works on editing")

N::MimeSuffix:: MimeSuffix  (QWidget * parent,Plan * p)
              : TreeWidget  (          parent,       p)
              , MimeManager (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::MimeSuffix::~MimeSuffix (void)
{
}

bool N::MimeSuffix::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true            ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  LinkAction        ( Copy         , Copy        ( ) ) ;
  alert             ( "Action"     , ""              ) ;
  return true                                          ;
}

void N::MimeSuffix::Configure(void)
{
  setWindowTitle               ( tr("MIME Suffix and Types") ) ;
  NewTreeWidgetItem            ( head                        ) ;
  head -> setText              ( 0,tr("Suffix"   )           ) ;
  head -> setText              ( 1,tr("MIME Type")           ) ;
  setDragDropMode              ( NoDragDrop                  ) ;
  setRootIsDecorated           ( false                       ) ;
  setAlternatingRowColors      ( true                        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded       ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded       ) ;
  setColumnCount               ( 2                           ) ;
  setAllAlignments             ( head,Qt::AlignCenter        ) ;
  setFont                      ( head,Fonts::ListView        ) ;
  setHeaderItem                ( head                        ) ;
  plan -> setFont              ( this                        ) ;
  MountClicked                 ( 2                           ) ;
}

void N::MimeSuffix::run(int T,ThreadData * d)
{
  ItemRunner ( T , d ) ;
}

bool N::MimeSuffix::ItemRunner(int T,ThreadData * d)
{
  switch           ( T ) {
    case 10001           :
      startLoading (   ) ;
      Listing      ( d ) ;
      stopLoading  (   ) ;
    return true          ;
  }                      ;
  return false           ;
}

bool N::MimeSuffix::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::MimeSuffix::Listing(ThreadData * d)
{
  emit clearItems  (                           ) ;
  EnterSQL         ( SC , plan->sql            ) ;
    QString Q                                    ;
    load           ( SC                        ) ;
    Q = SC.sql.SelectFrom                        (
      "extensionid,mimeid,name"                  ,
      PlanTable(Extensions)                      ,
      "order by extensionid asc"               ) ;
    SqlLoopNow ( SC , Q )                        ;
      int     EID    = SC . Int    ( 0 )         ;
      int     MimeId = SC . Int    ( 1 )         ;
      QString Suffix = SC . String ( 2 )         ;
      if (contains(MimeId))                      {
        QString m = ME[MimeId].mime              ;
        NewTreeWidgetItem ( IT )                 ;
        IT->setData       ( 0,Qt::UserRole,EID ) ;
        IT->setText       ( 0 , Suffix )         ;
        IT->setText       ( 1 , m      )         ;
        addTopLevelItem   ( IT )                 ;
      }                                          ;
    SqlLoopErr ( SC , Q )                        ;
    SqlLoopEnd ( SC , Q )                        ;
  LeaveSQL         ( SC , plan->sql            ) ;
  alert            ( "Done" , ""               ) ;
}

void N::MimeSuffix::List(void)
{
  startup ( ) ;
}

void N::MimeSuffix::doubleClicked(QTreeWidgetItem * item,int column)
{
  MimeLists * ML = NULL                                 ;
  removeOldItem ( )                                     ;
  switch (column)                                       {
    case 0                                              :
      setLineEdit                                       (
        item                                            ,
        column                                          ,
        SIGNAL(editingFinished())                       ,
        SLOT  (extensionEnd   ()) )                     ;
    break                                               ;
    case 1                                              :
      ML = new MimeLists(this,plan)                     ;
      ML->MimeTypes = MimeTypes                         ;
      ML->list        ( item->text(1) )                 ;
      ML->setEditable ( true )                          ;
      setItemWidget ( item , column , ML )              ;
      ItemWidget  = ML                                  ;
      ItemEditing = item                                ;
      ItemColumn  = column                              ;
      connect(ML,SIGNAL(currentIndexChanged(QString))   ,
              this,SLOT(mimeChanged        (QString)) ) ;
    break                                               ;
  }                                                     ;
}

void N::MimeSuffix::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return           ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
}

void N::MimeSuffix::mimeChanged(QString mime)
{
  if (IsNull(ItemEditing)) return ;
  ItemEditing->setText(1,mime)    ;
  Final         (ItemEditing)     ;
  removeOldItem ( )               ;
}

void N::MimeSuffix::extensionEnd(void)
{
  if (IsNull(ItemEditing)) return                       ;
  QLineEdit * l = qobject_cast<QLineEdit *>(ItemWidget) ;
  if (IsNull(l)) return                                 ;
  ItemEditing->setText(0,l->text())                     ;
  Final         (ItemEditing)                           ;
  removeOldItem ( )                                     ;
}

void N::MimeSuffix::Final(QTreeWidgetItem * item)
{
  ConnectSQL    ( SC , plan->sql )                    ;
    QString Q                                         ;
    int     EID  = item->data(0,Qt::UserRole).toInt() ;
    QString name = item->text(0)                      ;
    QString mime = item->text(1)                      ;
    if (EID<0)                                        {
      if (name.length()>0 && mime.length()>0)         {
        if (contains(mime))                           {
          int MID = ME[mime].id                       ;
          Q = SC.sql.SelectFrom                       (
           "extensionid"                              ,
           PlanTable(Extensions)                      ,
           "order by extensionid desc limit 0,1"    ) ;
          if (SC.Fetch(Q)) EID = SC.Int(0)            ;
          if (EID>=0)                                 {
            EID++                                     ;
            Q = SC.sql.InsertInto                     (
            PlanTable(Extensions)                     ,
            3,"extensionid","mimeid","name"         ) ;
            SC.Prepare(Q                            ) ;
            SC.Bind   ("extensionid",EID            ) ;
            SC.Bind   ("mimeid"     ,MID            ) ;
            SC.Bind   ("name"       ,name.toUtf8()  ) ;
            SC.Exec   (                             ) ;
            item->setData ( 0,Qt::UserRole,EID      ) ;
          }                                           ;
        }                                             ;
      }                                               ;
    } else                                            {
      if (name.length()>0 && mime.length()>0)         {
        if (contains(mime))                           {
          int MID = ME[mime].id                       ;
          Q = SC.sql.Update                           (
           PlanTable(Extensions)                      ,
           "where extensionid = :EXTENSIONID"         ,
           2,"mimeid","name"                        ) ;
          SC.Prepare(Q                              ) ;
          SC.Bind   ("extensionid",EID              ) ;
          SC.Bind   ("mimeid"     ,MID              ) ;
          SC.Bind   ("name"       ,name.toUtf8()    ) ;
          SC.Exec   (                               ) ;
        }                                             ;
      }                                               ;
    }                                                 ;
  DisconnectSQL ( SC        )                         ;
}

void N::MimeSuffix::Insert(void)
{
  NewTreeWidgetItem(IT)          ;
  IT->setData(0,Qt::UserRole,-1) ;
  addTopLevelItem  (IT)          ;
  doubleClicked    (IT,1       ) ;
}

void N::MimeSuffix::Delete(void)
{
  QTreeWidgetItem * item = currentItem()        ;
  if (IsNull(item)) return                      ;
  int EID  = nTreeInt(item,0)                   ;
  if (EID<0) return                             ;
  ConnectSQL    ( SC , plan->sql )              ;
    QString Q                                   ;
    Q = SC.sql.DeleteFrom                       (
    PlanTable(Extensions)                       ,
    QString("where extensionid = %1").arg(EID)) ;
    SC.Query(Q)                                 ;
    int index = indexOfTopLevelItem(item)       ;
    if (index>=0) takeTopLevelItem(index)       ;
  DisconnectSQL ( SC        )                   ;
}

void N::MimeSuffix::Copy(void)
{
  QTreeWidgetItem * item = currentItem()     ;
  if (IsNull(item)) return                   ;
  QString name = item->text(currentColumn()) ;
  qApp->clipboard()->setText(name)           ;
}
