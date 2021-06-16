#include <datawidgets.h>

N::PdlSites:: PdlSites    ( QWidget * parent , Plan * p )
            : TreeWidget  (           parent ,        p )
            , CodeManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PdlSites::~PdlSites(void)
{
}

bool N::PdlSites::FocusIn(void)
{
  LinkAction ( Insert  , New            () ) ;
  LinkAction ( Refresh , startup        () ) ;
  LinkAction ( Copy    , CopyToClipboard() ) ;
  return true                                ;
}

bool N::PdlSites::FocusOut(void)
{
  return true ;
}

void N::PdlSites::Configure(void)
{
  NewTreeWidgetItem       ( head                                    ) ;
  head -> setText         ( 0 , tr("Language" )                     ) ;
  head -> setText         ( 1 , tr("Address"  )                     ) ;
  setWindowTitle          ( tr("Programming language websites")     ) ;
  setColumnCount          ( 2                                       ) ;
  setHeaderItem           ( head                                    ) ;
  setRootIsDecorated      ( false                                   ) ;
  setAlternatingRowColors ( true                                    ) ;
  plan -> setFont         ( this                                    ) ;
  /////////////////////////////////////////////////////////////////////
  connect ( this , SIGNAL (itemDoubleClicked(QTreeWidgetItem*,int))   ,
            this , SLOT   (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::PdlSites::startup(void)
{
  clear                       (                         ) ;
  setEnabled                  ( false                   ) ;
  NetworkManager NM           ( plan                    ) ;
  GroupItems     GI           ( plan                    ) ;
  EnterSQL                    ( SC , plan->sql          ) ;
    SUID uuid                                             ;
    QString Q                                             ;
    NM       . LoadDomainIndex( SC                      ) ;
    PDLs     = ListPDLs       ( SC                      ) ;
    PdlNames . clear          (                         ) ;
    foreach (uuid,PDLs)                                   {
      PdlNames [ uuid ] = Name(SC,uuid,vLanguageId)       ;
    }                                                     ;
    foreach (uuid,PDLs)                                   {
      UUIDs URLs                                          ;
      SUID  iuid                                          ;
      URLs = GI.Subordination                             (
               SC                                         ,
               uuid                                       ,
               Types::Coding                    ,
               Types::URL                       ,
               Groups::Subordination            ,
               "order by position,id asc"               ) ;
      foreach (iuid,URLs)                                 {
        QUrl X = NM.PageUrl ( SC , iuid               )   ;
        NewTreeWidgetItem   ( item                    )   ;
        item->setData       ( 0 , Qt::UserRole , uuid )   ;
        item->setData       ( 1 , Qt::UserRole , iuid )   ;
        item->setText       ( 0 , PdlNames      [uuid])   ;
        item->setText       ( 1 , X . toString ( )    )   ;
        addTopLevelItem     ( item                    )   ;
        DoProcessEvents                                   ;
      }                                                   ;
      DoProcessEvents                                     ;
    }                                                     ;
  LeaveSQL                    ( SC , plan->sql          ) ;
  for (int i=0;i<columnCount();i++)                       {
    resizeColumnToContents    ( i )                       ;
  }                                                       ;
  setEnabled                  ( true                    ) ;
  Alert ( Done )                                          ;
  return true                                             ;
}

bool N::PdlSites::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                  ;
  QTreeWidgetItem * item = itemAt (pos)      ;
  QAction         * aa                       ;
  if (IsNull(item)) return true              ;
  mm . add     ( 101 , tr("Go to website") ) ;
  mm . setFont ( plan )                      ;
  aa = mm.exec (      )                      ;
  if (IsNull(aa)) return true                ;
  switch (mm[aa])                            {
    case 101                                 :
      if (NotNull(item))                     {
        QUrl U(item->text(1))                ;
        emit Go ( U , true )                 ;
      }                                      ;
    break                                    ;
  }                                          ;
  return true                                ;
}

bool N::PdlSites::Attach(SqlConnection & SC,SUID pdl,SUID url)
{
  GroupItems GI(plan)              ;
  UUIDs Uuids                       ;
  Uuids << url                      ;
  GI.Join                           (
    SC                              ,
    pdl                             ,
    Types::Coding         ,
    Types::URL            ,
    Groups::Subordination ,
    0,Uuids                       ) ;
  return true                       ;
}

bool N::PdlSites::Detach(SqlConnection & SC,SUID pdl,SUID url)
{
  GroupItems GI(plan)     ;
  UUIDs Uuids             ;
  Uuids << url            ;
  GI.Detach               (
    SC                    ,
    pdl                   ,
    Types::Coding         ,
    Types::URL            ,
    Groups::Subordination ,
    Uuids               ) ;
  return true             ;
}

void N::PdlSites::nameChanged(void)
{
  QLineEdit       * le  = Casting    ( QLineEdit   , ItemWidget ) ;
  SUID              pdl = nTreeUuid  ( ItemEditing , 0          ) ;
  SUID              idx = nTreeUuid  ( ItemEditing , 1          ) ;
  QString           ul  = le -> text (                          ) ;
  QTreeWidgetItem * it  = ItemEditing                             ;
  ItemEditing->setText  ( 1 , ul             )                    ;
  removeOldItem         ( false , ItemColumn )                    ;
  NetworkManager NM     ( plan               )                    ;
  EnterSQL              ( SC    , plan->sql  )                    ;
    QUrl U              ( ul                 )                    ;
    SUID ulx                                                      ;
    NM.LoadDomainIndex  ( SC                 )                    ;
    ulx = NM.assurePage ( SC , U             )                    ;
    if (pdl>0 && idx>0) Detach(SC,pdl,idx)                        ;
    if (pdl>0 && ulx>0) Attach(SC,pdl,ulx)                        ;
  LeaveSQL              ( SC    , plan->sql  )                    ;
  Alert ( Done )                                                  ;
}

void N::PdlSites::languageChanged(int index)
{
  QComboBox * cb  = Casting   ( QComboBox   , ItemWidget ) ;
  SUID        pdl = nTreeUuid ( ItemEditing , 0          ) ;
  SUID        idx = nTreeUuid ( ItemEditing , 1          ) ;
  SUID        nid = cb->itemData(index).toULongLong (    ) ;
  QString     nsn = cb->currentText()                      ;
  QTreeWidgetItem * it = ItemEditing                       ;
  ItemEditing->setData ( 0 , Qt::UserRole ,nid )           ;
  ItemEditing->setText ( 0 , nsn               )           ;
  removeOldItem        ( false , ItemColumn    )           ;
  EnterSQL             ( SC    , plan->sql     )           ;
    if (pdl>0 && idx>0) Detach(SC,pdl,idx)                 ;
    if (nid>0 && idx>0) Attach(SC,nid,idx)                 ;
  LeaveSQL             ( SC    , plan->sql     )           ;
}

void N::PdlSites::doubleClicked(QTreeWidgetItem * item,int column)
{
  QComboBox * cb = NULL                              ;
  QLineEdit * le = NULL                              ;
  SUID        id = 0                                 ;
  if (ItemColumn>=0) removeOldItem(false,ItemColumn) ;
  switch (column)                                    {
    case 0                                           :
      id = nTreeUuid(item,column)                    ;
      cb = setComboBox                               (
                  item                               ,
                  column                             ,
                  SIGNAL (activated      (int) )     ,
                  SLOT   (languageChanged(int) )   ) ;
      N::AddItems(*cb,PDLs,PdlNames)                 ;
      if (id>0) (*cb) <= id                          ;
      plan -> setFont     ( cb   )                   ;
      cb   -> setEditable ( true )                   ;
      cb   -> showPopup   (      )                   ;
    break                                            ;
    case 1                                           :
      id = nTreeUuid(item,column)                    ;
      le = setLineEdit                               (
                  item                               ,
                  column                             ,
                  SIGNAL (editingFinished () )       ,
                  SLOT   (nameChanged     () )     ) ;
      plan -> setFont    ( le )                      ;
      le   -> setFocus   ( Qt::TabFocusReason )      ;
    break                                            ;
  }                                                  ;
}

void N::PdlSites::New(void)
{
  NewTreeWidgetItem ( item                 ) ;
  item->setData     ( 0 , Qt::UserRole , 0 ) ;
  item->setData     ( 1 , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( item                 ) ;
  scrollToItem      ( item                 ) ;
  setCurrentItem    ( item                 ) ;
  doubleClicked     ( item , 0             ) ;
}
