#include <datawidgets.h>

N::PolyglotLists :: PolyglotLists ( QWidget * parent , Plan * p )
                  : TreeDock      (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PolyglotLists ::~PolyglotLists(void)
{
}

void N::PolyglotLists::Configure(void)
{
  NewTreeWidgetItem            ( head                        )   ;
  head -> setText              ( 0 , tr("Item")              )   ;
  head -> setTextAlignment     ( 0 , Qt::AlignCenter         )   ;
  setWindowTitle               ( tr("Polyglot translations") )   ;
  setDragDropMode              ( NoDragDrop                  )   ;
  setRootIsDecorated           ( false                       )   ;
  setAlternatingRowColors      ( true                        )   ;
  setSelectionMode             ( SingleSelection             )   ;
  setColumnCount               ( 1                           )   ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded       )   ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded       )   ;
  setHeaderItem                ( head                        )   ;
  plan -> setFont              ( this                        )   ;
  ////////////////////////////////////////////////////////////////
  connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))   ,
          this,SLOT  (doubleClicked    (QTreeWidgetItem*,int)) ) ;
}

void N::PolyglotLists::addType(QString name,SUID type)
{
  NewTreeWidgetItem ( item                    ) ;
  item->setText     ( 0 , name                ) ;
  item->setData     ( 0 , Qt::UserRole , type ) ;
  addTopLevelItem   ( item                    ) ;
  Titles [ type ] = name                        ;
}

bool N::PolyglotLists::startup(void)
{
  addType ( tr("Object types"         ) ,  1 ) ;
  addType ( tr("Languages"            ) ,  2 ) ;
  addType ( tr("History types"        ) ,  3 ) ;
  addType ( tr("Nations"              ) ,  4 ) ;
  addType ( tr("Keyword relations"    ) ,  5 ) ;
  addType ( tr("Meaning types"        ) ,  6 ) ;
  addType ( tr("Paper size"           ) ,  7 ) ;
  addType ( tr("Group relations"      ) ,  8 ) ;
  addType ( tr("Action types"         ) ,  9 ) ;
  addType ( tr("Length units"         ) , 10 ) ;
  addType ( tr("Project types"        ) , 11 ) ;
  addType ( tr("Math object types"    ) , 12 ) ;
  addType ( tr("Matrices"             ) , 13 ) ;
  addType ( tr("Knowledge units"      ) , 14 ) ;
  addType ( tr("Knowledge fields"     ) , 15 ) ;
  addType ( tr("Knowledge bases"      ) , 16 ) ;
  addType ( tr("Calendars"            ) , 17 ) ;
  addType ( tr("Occupations"          ) , 18 ) ;
  addType ( tr("Types of organization") , 19 ) ;
  addType ( tr("Races"                ) , 20 ) ;
  addType ( tr("Organs"               ) , 21 ) ;
  addType ( tr("Parament types"       ) , 22 ) ;
  addType ( tr("CLF Types"            ) , 23 ) ;
  return true                                  ;
}

bool N::PolyglotLists::Menu(QPoint pos)
{ Q_UNUSED ( pos )                                              ;
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 1000001                                                :
      emit attachMdi ( this , Qt::Vertical )                    ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::RightDockWidgetArea                                 ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}


void N::PolyglotLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID id = nTreeUuid(item,0)                            ;
  nDropOut ( nEqual(id,0)   )                            ;
  UUIDs       Uuids                                      ;
  QStringList Names                                      ;
  switch (id)                                            {
    case    1                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "type"                                     ,
              PlanTable(Types)                           ,
              "where flags != 0 order by id asc ;" )     ;
        SqlLoopNow(SC,Q)                                 ;
          SUID uuid = SC.Uuid(0)                         ;
          Uuids << uuid                                  ;
          Names << QString::number(uuid)                 ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    2                                            :
      Uuids = plan->languages.LanguageUuids()            ;
      Names = plan->languages.LanguageNames()            ;
      emit Translations(Titles[id],Names,Uuids)          ;
    break                                                ;
    case    3                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(HistoryTypes)                    ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Value(0).toInt()                ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    4                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "country,uuid,native"                      ,
              PlanTable(Nations)                         ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          SUID    uuid    = SC.Uuid   (1)                ;
          QString native  = SC.String (2)                ;
          Uuids << uuid                                  ;
          Names << native                                ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    5                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(KeywordRelations)                ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int     id    = SC.Int  (0)                    ;
          SUID    uuid  = SC.Uuid (1)                    ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    6                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "type,uuid"                                ,
              PlanTable(MeaningTypes)                    ,
              "order by type asc ;"                    ) ;
        SqlLoopNow(SC,Q)                                 ;
          int     type    = SC.Int  (0)                  ;
          SUID    uuid    = SC.Uuid (1)                  ;
          Uuids << uuid                                  ;
          Names << QString::number(type)                 ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    7                                          : {
      QString Title = Titles[id]                         ;
      for (int i=0;i<plan->paper.papers.count();i++)     {
        Uuids << plan->paper.papers[i].uuid              ;
        Names << plan->paper.papers[i].name              ;
      }                                                  ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    8                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(Relations)                       ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int       id = SC.Int  (0)                     ;
          SUID    uuid = SC.Uuid (1)                     ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case    9                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "type,uuid"                                ,
              PlanTable(ActionTypes)                     ,
              "order by type asc ;"                    ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   10                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "unit,uuid"                                ,
              PlanTable(Units)                           ,
              "order by unit asc ;"                    ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   11                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(ProjectTypes)                    ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   12                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(MathTypes)                       ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   13                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid,name"                             ,
              PlanTable(Matrices)                        ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << SC.String(2)                          ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   14                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(Knowledge)                       ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   15                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(KnowledgeFields)                 ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   16                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(KnowledgeBases)                  ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   17                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "uuid,type"                                ,
              PlanTable(Calendars)                       ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          SUID uuid = SC.Uuid (0)                        ;
          int  type = SC.Int  (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(type)                 ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   18                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(Occupations)                     ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   19                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(Comity)                          ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   20                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(Races)                           ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   21                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(Organs)                          ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   22                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(ParamentTypes)                   ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
    case   23                                          : {
      QString Title = Titles[id]                         ;
      EnterSQL ( SC , plan->sql )                        ;
        QString Q                                        ;
        Q = SC.sql.SelectFrom                            (
              "id,uuid"                                  ,
              PlanTable(ClfTypes)                        ,
              "order by id asc ;"                      ) ;
        SqlLoopNow(SC,Q)                                 ;
          int  id   = SC.Int  (0)                        ;
          SUID uuid = SC.Uuid (1)                        ;
          Uuids << uuid                                  ;
          Names << QString::number(id)                   ;
        SqlLoopErr(SC,Q)                                 ;
        SqlLoopEnd(SC,Q)                                 ;
      LeaveSQL ( SC , plan->sql )                        ;
      emit Translations(Title,Names,Uuids)               ;
    }                                                    ;
    break                                                ;
  }                                                      ;
}
