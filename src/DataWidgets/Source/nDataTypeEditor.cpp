#include <datawidgets.h>

#define TABLES(ID) CodePlan->Tables[Tables::ID]

N::DataTypeEditor:: DataTypeEditor ( QWidget * parent , Plan * p )
                  : TreeWidget     (           parent ,        p )
                  , CodeManager    (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DataTypeEditor::~DataTypeEditor(void)
{
}

void N::DataTypeEditor::Configure(void)
{
  setWindowTitle          ( tr("Data types") ) ;
  setHeaderLabel          ( tr("Type"      ) ) ;
  setRootIsDecorated      ( false            ) ;
  setAlternatingRowColors ( true             ) ;
  plan->setFont           ( this             ) ;
  //////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

bool N::DataTypeEditor::FocusIn(void)
{
  LinkAction(Insert,New()) ;
  return true              ;
}

bool N::DataTypeEditor::FocusOut(void)
{
  return true ;
}

bool N::DataTypeEditor::startup(void)
{
  clear            (                          ) ;
  EnterSQL         ( SC , plan->sql           ) ;
    QString Q                                   ;
    UUIDs   Uuids = DataTypes ( SC            ) ;
    NAMEs   Names = TypeNames ( SC , Uuids    ) ;
    SUID    uuid                                ;
    foreach (uuid,Uuids)                        {
      QString name = Names[uuid]                ;
      NewTreeWidgetItem ( item )                ;
      item->setData   ( 0,Qt::UserRole,uuid   ) ;
      item->setText   ( 0,name                ) ;
      addTopLevelItem ( item                  ) ;
    }                                           ;
  LeaveSQL         ( SC , plan->sql           ) ;
  Alert            ( Done                     ) ;
  return true                                   ;
}

void N::DataTypeEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID        uuid = nTreeUuid(item,0) ;
  QLineEdit * line                     ;
  removeOldItem()                      ;
  line = setLineEdit                   (
         item                          ,
         column                        ,
         SIGNAL(editingFinished())     ,
         SLOT  (nameFinished   ())   ) ;
  line->setFocus(Qt::TabFocusReason)   ;
}

void N::DataTypeEditor::New(void)
{
  NewTreeWidgetItem(IT)         ;
  IT->setData(0,Qt::UserRole,0) ;
  addTopLevelItem (IT  )        ;
  scrollToItem    (IT  )        ;
  doubleClicked   (IT,0)        ;
}

void N::DataTypeEditor::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return                ;
  SUID    uuid    = nTreeUuid(ItemEditing,0)     ;
  removeItemWidget (ItemEditing,ItemColumn)      ;
  if (uuid<=0)                                   {
    int index = indexOfTopLevelItem(ItemEditing) ;
    takeTopLevelItem(index)                      ;
  }                                              ;
  ItemEditing = NULL                             ;
  ItemWidget  = NULL                             ;
  ItemColumn  = -1                               ;
}

void N::DataTypeEditor::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * line    = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line       )) return                         ;
  ItemEditing -> setText    ( ItemColumn , line->text() ) ;
  /////////////////////////////////////////////////////////
  SUID    uuid        = nTreeUuid(ItemEditing,0)          ;
  QString name        = ItemEditing->text(0)              ;
  EnterSQL ( SC , plan->sql )                             ;
    QString Q                                             ;
    if (uuid>0)                                           {
      removeOldItem ( )                                   ;
      Q = SC.sql.Update(TABLES(DataTypes)                 ,
                        "where uuid = :UUID",1,"name"   ) ;
      SC . Prepare ( Q                      )             ;
      SC . Bind    ( "uuid" , uuid          )             ;
      SC . Bind    ( "name" , name.toUtf8() )             ;
      SC . Exec    (                        )             ;
    } else                                                {
      uuid = SC.Unique(TABLES(MajorUuid),"uuid")          ;
      SC.assureUuid                                       (
        TABLES(MajorUuid)                                 ,
        uuid                                              ,
        Types::DataTypes                      ) ;
      /////////////////////////////////////////////////////
      Q = SC.sql.InsertInto                               (
            TABLES(Codes)                                 ,
            3                                             ,
            "uuid"                                        ,
            "type"                                        ,
            "pdl"                                       ) ;
      /////////////////////////////////////////////////////
      SC.Prepare ( Q                                    ) ;
      SC.Bind    ( "uuid" , uuid                        ) ;
      SC.Bind    ( "type" , PDL::DataType     ) ;
      SC.Bind    ( "pdl"  , 0                           ) ;
      SC.Exec    (                                      ) ;
      /////////////////////////////////////////////////////
      Q = SC.sql.InsertInto                               (
            TABLES(DataModels)                            ,
            4                                             ,
            "uuid"                                        ,
            "type"                                        ,
            "pdl"                                         ,
            "flags"                                     ) ;
      SC.Prepare ( Q                                    ) ;
      SC.Bind    ( "uuid"  , uuid                       ) ;
      SC.Bind    ( "type"  , PDL::StandardType) ;
      SC.Bind    ( "pdl"   , 0                          ) ;
      SC.Bind    ( "flags" , 1                          ) ;
      SC.Exec    (                                      ) ;
      /////////////////////////////////////////////////////
      Q = SC.sql.InsertInto                               (
            TABLES(DataTypes)                             ,
            2                                             ,
            "uuid"                                        ,
            "name"                                      ) ;
      SC.Prepare ( Q                   )                  ;
      SC.Bind ( "uuid" , uuid          )                  ;
      SC.Bind ( "name" , name.toUtf8() )                  ;
      SC.Exec (                        )                  ;
      /////////////////////////////////////////////////////
      ItemEditing->setData(0,Qt::UserRole,uuid)           ;
      removeOldItem ( )                                   ;
    }                                                     ;
  LeaveSQL ( SC , plan->sql )                             ;
  Alert    ( Done           )                             ;
}

bool N::DataTypeEditor::Menu(QPoint pos)
{
  return true ;
}
