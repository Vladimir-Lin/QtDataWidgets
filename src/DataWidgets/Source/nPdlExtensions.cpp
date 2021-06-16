#include <datawidgets.h>

N::PdlExtensions:: PdlExtensions ( QWidget * parent , Plan * p )
                 : TreeWidget    (           parent ,        p )
                 , CodeManager   (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PdlExtensions::~PdlExtensions(void)
{
}

bool N::PdlExtensions::FocusIn(void)
{
  LinkAction ( Insert  , New    () ) ;
  LinkAction ( Refresh , startup() ) ;
  return true                        ;
}

bool N::PdlExtensions::FocusOut(void)
{
  return true ;
}

void N::PdlExtensions::Configure(void)
{
  NewTreeWidgetItem       ( head                                       ) ;
  head -> setText         ( 0 , tr("Language" )                        ) ;
  head -> setText         ( 1 , tr("Extension")                        ) ;
  setWindowTitle          ( tr("Extensions for programming languages") ) ;
  setColumnCount          ( 2                                          ) ;
  setHeaderItem           ( head                                       ) ;
  setRootIsDecorated      ( false                                      ) ;
  setAlternatingRowColors ( true                                       ) ;
  plan -> setFont         ( this                                       ) ;
  ////////////////////////////////////////////////////////////////////////
  connect ( this , SIGNAL (itemDoubleClicked(QTreeWidgetItem*,int))      ,
            this , SLOT   (doubleClicked    (QTreeWidgetItem*,int))    ) ;
}

bool N::PdlExtensions::startup(void)
{
  clear                       (                         ) ;
  EnterSQL                    ( SC , plan->sql          ) ;
    SUID uuid                                             ;
    QString Q                                             ;
    PDLs = ListPDLs           ( SC                      ) ;
    PIDs = PdlIdentifications ( SC , PDLs               ) ;
    EXTs = ListExtensions     ( SC                      ) ;
    PdlNames . clear          (                         ) ;
    IdNames  . clear          (                         ) ;
    foreach (uuid,PDLs)                                   {
      int id = PIDs [ uuid ]                              ;
      PdlNames [ uuid ] = Name(SC,uuid,vLanguageId)       ;
      IdNames  [ id   ] = PdlNames [ uuid ]               ;
    }                                                     ;
    Q = SC.sql.SelectFrom                                 (
          "pdl,extension"                                 ,
          PlanTable(PdlExtensions)                        ,
          "order by id asc ;"                           ) ;
    SqlLoopNow           ( SC , Q                )        ;
      int pdl = SC . Int ( 0                     )        ;
      int ext = SC . Int ( 1                     )        ;
      NewTreeWidgetItem  ( item                  )        ;
      item->setData      ( 0  , Qt::UserRole,pdl )        ;
      item->setData      ( 1  , Qt::UserRole,ext )        ;
      item->setText      ( 0  , IdNames [ pdl ]  )        ;
      item->setText      ( 1  , EXTs    [ ext ]  )        ;
      addTopLevelItem    ( item                  )        ;
    SqlLoopErr           ( SC , Q                )        ;
    SqlLoopEnd           ( SC , Q                )        ;
  LeaveSQL                    ( SC , plan->sql          ) ;
  for (int i=0;i<columnCount();i++)                       {
    resizeColumnToContents    ( i )                       ;
  }                                                       ;
  Alert ( Done )                                          ;
  return true                                             ;
}

bool N::PdlExtensions::Menu(QPoint pos)
{
  return true ;
}

void N::PdlExtensions::languageChanged(int index)
{
  QComboBox * cb  = Casting(QComboBox,ItemWidget)             ;
  int         pdl = ItemEditing->data(0,Qt::UserRole).toInt() ;
  int         ext = ItemEditing->data(1,Qt::UserRole).toInt() ;
  int         nid = cb->itemData(index).toInt()               ;
  QString     nsn = cb->currentText()                         ;
  QTreeWidgetItem * it = ItemEditing                          ;
  ItemEditing->setData ( 0 , Qt::UserRole ,nid )              ;
  ItemEditing->setText ( 0 , nsn               )              ;
  removeOldItem        ( false , ItemColumn    )              ;
  EnterSQL             ( SC    , plan->sql     )              ;
    if (pdl>0 && ext>0) DeletePdlExtension ( SC , pdl , ext ) ;
    if (nid>0 && ext>0)                                       {
      if (!AddPdlExtension(SC,nid,ext))                       {
        int index = indexOfTopLevelItem(it)                   ;
        if (index>=0) takeTopLevelItem(index)                 ;
      }                                                       ;
    }                                                         ;
  LeaveSQL             ( SC    , plan->sql     )              ;
}

void N::PdlExtensions::extensionChanged(int index)
{
  QComboBox * cb = Casting(QComboBox,ItemWidget)              ;
  int         pdl = ItemEditing->data(0,Qt::UserRole).toInt() ;
  int         ext = ItemEditing->data(1,Qt::UserRole).toInt() ;
  int         nid = cb->itemData(index).toInt()               ;
  QString     nsn = cb->currentText()                         ;
  QTreeWidgetItem * it = ItemEditing                          ;
  ItemEditing->setData ( 1 , Qt::UserRole ,nid )              ;
  ItemEditing->setText ( 1 , nsn               )              ;
  removeOldItem        ( false , ItemColumn    )              ;
  EnterSQL             ( SC    , plan->sql     )              ;
    if (pdl>0 && ext>0) DeletePdlExtension ( SC , pdl , ext ) ;
    if (pdl>0 && nid>0)                                       {
      if (!AddPdlExtension(SC,pdl,nid))                       {
        int index = indexOfTopLevelItem(it)                   ;
        if (index>=0) takeTopLevelItem(index)                 ;
      }                                                       ;
    }                                                         ;
  LeaveSQL             ( SC    , plan->sql     )              ;
}

void N::PdlExtensions::doubleClicked(QTreeWidgetItem * item,int column)
{
  QComboBox * cb = NULL                              ;
  int         id = -1                                ;
  if (ItemColumn>=0) removeOldItem(false,ItemColumn) ;
  switch (column)                                    {
    case 0                                           :
      id = item->data(column,Qt::UserRole).toInt()   ;
      cb = setComboBox                               (
                  item                               ,
                  column                             ,
                  SIGNAL (activated      (int) )     ,
                  SLOT   (languageChanged(int) )   ) ;
      cb << IdNames                                  ;
      if (id>0) (*cb) <= id                          ;
      plan -> setFont     ( cb   )                   ;
      cb   -> setEditable ( true )                   ;
      cb   -> showPopup   (      )                   ;
    break                                            ;
    case 1                                           :
      id = item->data(column,Qt::UserRole).toInt()   ;
      cb = setComboBox                               (
                  item                               ,
                  column                             ,
                  SIGNAL (activated       (int) )    ,
                  SLOT   (extensionChanged(int) )  ) ;
      cb << EXTs                                     ;
      if (id>0) (*cb) <= id                          ;
      plan -> setFont     ( cb   )                   ;
      cb   -> setEditable ( true )                   ;
      cb   -> showPopup   (      )                   ;
    break                                            ;
  }                                                  ;
}

void N::PdlExtensions::New(void)
{
  NewTreeWidgetItem ( item                 ) ;
  item->setData     ( 0 , Qt::UserRole , 0 ) ;
  item->setData     ( 1 , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( item                 ) ;
  scrollToItem      ( item                 ) ;
  setCurrentItem    ( item                 ) ;
  doubleClicked     ( item , 0             ) ;
}
