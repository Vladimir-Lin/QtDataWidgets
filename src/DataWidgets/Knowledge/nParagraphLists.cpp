#include <datawidgets.h>

N::ParagraphLists:: ParagraphLists  ( QWidget * parent , Plan * p )
                  : TreeWidget      (           parent ,        p )
                  , DocumentManager (                           p )
                  , Object          ( 0 , Types  :: PlainText     )
                  , Relation        ( 0 , Groups :: Contains      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ParagraphLists::~ParagraphLists(void)
{
}

void N::ParagraphLists::Configure(void)
{
  setWindowTitle               ( tr("Paragraph lists") ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( SingleSelection       ) ;
  setColumnCount               ( 1                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setHeaderHidden              ( true                  ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::ParagraphLists::FocusIn(void)
{
  LinkAction ( Refresh , startup        () ) ;
  LinkAction ( Copy    , CopyToClipboard() ) ;
  return true                                ;
}

bool N::ParagraphLists::startup(void)
{
  clear            (                         ) ;
  GroupItems    GI ( plan                    ) ;
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    UUIDs   Uuids                              ;
    SUID    uu                                 ;
    if (uuid<=0)                               {
      Uuids = SC.Uuids                         (
                PlanTable(Paragraphs)          ,
                "uuid","order by id asc"     ) ;
    } else                                     {
      Uuids = GI.Subordination                 (
                SC                             ,
                uuid                           ,
                type                           ,
                Types::Paragraph               ,
                linkType                       ,
                "order by position asc"      ) ;
    }                                          ;
    int total = Uuids.count()                  ;
    foreach (uu,Uuids)                         {
      NewTreeWidgetItem(item)                  ;
      QString name                             ;
      Q = SC.sql.SelectFrom                    (
            "name"                             ,
            PlanTable(Paragraphs)              ,
            SC.WhereUuid(uu)                 ) ;
      if (SC.Fetch(Q)) name = SC.String(0)     ;
      if (name.length()>0)                     {
        item -> setData (0,Qt::UserRole,uu   ) ;
        item -> setText (0,name              ) ;
        addTopLevelItem (item                ) ;
      }                                        ;
    }                                          ;
    setToolTip ( tr("%1 items") . arg(total) ) ;
  LeaveSQL         ( SC , plan->sql          ) ;
  Alert ( Done )                               ;
  return true                                  ;
}

bool N::ParagraphLists::Menu(QPoint pos)
{ Q_UNUSED ( pos ) ;
  return true      ;
}
