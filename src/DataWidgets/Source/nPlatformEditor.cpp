#include <datawidgets.h>

N::PlatformEditor :: PlatformEditor ( QWidget * parent , Plan * p )
                   : TreeWidget     (           parent ,        p )
                   , CodeManager    (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PlatformEditor ::~PlatformEditor(void)
{
}

void N::PlatformEditor::Configure(void)
{
  NewTreeWidgetItem       ( head                                       ) ;
  head -> setText         ( 0 , tr( "Operating system"           )     ) ;
  head -> setText         ( 1 , tr( "Architecture"               )     ) ;
  head -> setText         ( 2 , tr( "Version"                    )     ) ;
  head -> setText         ( 3 , tr( "Endian"                     )     ) ;
  setWindowTitle          (     tr( "Operating system platforms" )     ) ;
  setColumnCount          ( 4                                          ) ;
  setHeaderItem           ( head                                       ) ;
  setRootIsDecorated      ( false                                      ) ;
  setAlternatingRowColors ( true                                       ) ;
  plan -> setFont         ( this                                       ) ;
  ////////////////////////////////////////////////////////////////////////
  connect ( this , SIGNAL (itemDoubleClicked(QTreeWidgetItem*,int))      ,
            this , SLOT   (doubleClicked    (QTreeWidgetItem*,int))    ) ;
}

bool N::PlatformEditor::FocusIn(void)
{
  LinkAction ( Insert  , New    () ) ;
  LinkAction ( Refresh , startup() ) ;
  return true                        ;
}

bool N::PlatformEditor::FocusOut(void)
{
  return true ;
}

bool N::PlatformEditor::startup(void)
{
  clear            (                         ) ;
  EnterSQL         ( SC , plan->sql          ) ;
    UUIDs   Uuids = Platforms ( SC )           ;
    SUID    uuid                               ;
    QString Q                                  ;
    foreach (uuid,Uuids)                       {
      NewTreeWidgetItem ( item )               ;
      Q = SC.sql.SelectFrom                    (
            "os,arch,version,endian"           ,
            PlanTable(Platforms)               ,
            SC.WhereUuid(uuid)               ) ;
      if (SC.Fetch(Q))                         {
        QString os      = SC . String ( 0 )    ;
        QString arch    = SC . String ( 1 )    ;
        QString version = SC . String ( 2 )    ;
        QString endian  = SC . String ( 3 )    ;
        item -> setData (0,Qt::UserRole,uuid)  ;
        item -> setText (0,os               )  ;
        item -> setText (1,arch             )  ;
        item -> setText (2,version          )  ;
        item -> setText (3,endian           )  ;
        addTopLevelItem (item               )  ;
      }                                        ;
    }                                          ;
  LeaveSQL         ( SC , plan->sql          ) ;
  SuitableColumns  (                         ) ;
  Alert ( Done )                               ;
  return true                                  ;
}

void N::PlatformEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
}

bool N::PlatformEditor::Menu(QPoint pos)
{
  return true ;
}

void N::PlatformEditor::New(void)
{
  NewTreeWidgetItem ( item                 ) ;
  item->setData     ( 0 , Qt::UserRole , 0 ) ;
  item->setData     ( 1 , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( item                 ) ;
  scrollToItem      ( item                 ) ;
  setCurrentItem    ( item                 ) ;
  doubleClicked     ( item , 0             ) ;
}
