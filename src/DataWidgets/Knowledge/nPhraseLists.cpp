#include <datawidgets.h>

N::PhraseLists :: PhraseLists     ( QWidget * parent , Plan * p )
                : TreeWidget      (           parent ,        p )
                , DocumentManager (                           p )
                , Object          ( 0 , Types::PlainText        )
                , Relation        ( 0 , Groups::Contains        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PhraseLists ::~PhraseLists(void)
{
}

void N::PhraseLists::Configure(void)
{
  setWindowTitle               ( tr("Phrase lists")    ) ;
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

bool N::PhraseLists::FocusIn(void)
{
  LinkAction ( Refresh , startup        () ) ;
  LinkAction ( Copy    , CopyToClipboard() ) ;
  return true                                ;
}

bool N::PhraseLists::startup(void)
{
  clear            (                         ) ;
  GroupItems    GI ( plan                    ) ;
  EnterSQL         ( SC , plan->sql          ) ;
    QString Q                                  ;
    UUIDs   Uuids                              ;
    SUID    uu                                 ;
    if (uuid<=0)                               {
      Uuids = SC.Uuids                         (
                PlanTable(Sentences)           ,
                "uuid","order by id asc"     ) ;
    } else                                     {
      Uuids = GI.Subordination                 (
                SC                             ,
                uuid                           ,
                type                           ,
                Types::Sentence                ,
                linkType                       ,
                "order by position asc"      ) ;
    }                                          ;
    int total = Uuids.count()                  ;
    foreach (uu,Uuids)                         {
      NewTreeWidgetItem(item)                  ;
      QString name                             ;
      Q = SC.sql.SelectFrom                    (
            "name"                             ,
            PlanTable(Sentences)               ,
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

bool N::PhraseLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                ;
  QTreeWidgetItem * item = itemAt  ( pos ) ;
  QAction         * aa                     ;
  nSafeExec(item) mm.add(501,tr("Deacon")) ;
  mm.setFont ( plan    )                   ;
  aa = mm.exec()                           ;
  nKickOut ( IsNull(aa) , true )           ;
  switch (mm[aa])                          {
    case 501                               :
      plan -> Talk ( item->text(0) )       ;
    break                                  ;
  }                                        ;
  return true                              ;
}
