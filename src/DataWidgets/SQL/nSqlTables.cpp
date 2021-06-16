#include <datawidgets.h>

N::SqlTables:: SqlTables  (QWidget * parent,Plan * p)
             : TreeDock   (          parent,       p)
             , Object     (0,Types::SqlPlan         )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlTables::~SqlTables (void)
{
}

QMimeData * N::SqlTables::dragMime (void)
{
  UUIDs Uuids                                    ;
  SUID  u = 0                                    ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->checkState(0)==Qt::Checked)          {
      Uuids << nTreeUuid(it,0)                   ;
    }                                            ;
  }                                              ;
  if (Uuids.count()<=0)                          {
    QTreeWidgetItem * it = currentItem()         ;
    if (NotNull(it))                             {
      u = nTreeUuid(it,0)                        ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  nKickOut(Uuids.count()<=0 && u == 0,NULL)      ;
  if (Uuids.count()==1)                          {
    u     = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "sql/uuid"  , u     )       ;
  } else                                         {
    setMime ( mime , "sql/uuids" , Uuids )       ;
  }                                              ;
  ////////////////////////////////////////////////
  QImage image = QImage(":/images/tables.png")   ;
  mime -> setImageData(image)                    ;
  return mime                                    ;
}

bool N::SqlTables::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::SqlTables::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::SqlTables::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::SqlTables::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::SqlTables::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::SqlTables::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::SqlTables::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::SqlTables::Configure(void)
{
  Scope = ""                                             ;
  NewTreeWidgetItem            ( head                  ) ;
  setWindowTitle               ( tr("Database tables") ) ;
  setDragDropMode              ( DragOnly              ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( ExtendedSelection     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setAccessibleName            ( "nSqlTables"          ) ;
  setColumnCount               ( 5                     ) ;
  head -> setText              ( 0, tr("Schema"      ) ) ;
  head -> setText              ( 1, tr("Database"    ) ) ;
  head -> setText              ( 2, tr("Table"       ) ) ;
  head -> setText              ( 3, tr("Enumeration" ) ) ;
  head -> setText              ( 4, tr("Number"      ) ) ;
  assignHeaderItems            ( head                  ) ;
  setColumnHidden              ( 1 , true              ) ;
  setColumnHidden              ( 2 , true              ) ;
  setColumnHidden              ( 3 , true              ) ;
  setColumnHidden              ( 4 , true              ) ;
  MountClicked                 ( 2                     ) ;
  setDropFlag                  ( DropSqlTable , true   ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::SqlTables::FocusIn(void)
{
  LinkAction ( Insert  , New             () ) ;
  LinkAction ( Refresh , startup         () ) ;
  LinkAction ( Rename  , Rename          () ) ;
  LinkAction ( Font    , setFont         () ) ;
  LinkAction ( Copy    , CopyToClipboard () ) ;
  LinkAction ( Export  , ExportCpp       () ) ;
  return true                                 ;
}

bool N::SqlTables::FocusOut(void)
{
  return true ;
}

UUIDs N::SqlTables::GetUuids(SqlConnection & SC)
{
  UUIDs Uuids                             ;
  if (uuid<=0)                            {
    Uuids = SC.Uuids(PlanTable(SqlTables) ,
                     "uuid"               ,
                     "order by id asc"  ) ;
  } else                                  {
    GroupItems GI(plan)                   ;
    Uuids = GI.Subordination              (
              SC                          ,
              uuid                        ,
              type                        ,
              Types::SqlTable             ,
              Groups::Subordination       ,
              "order by position asc"   ) ;
  }                                       ;
  return Uuids                            ;
}

QTreeWidgetItem * N::SqlTables::GetItem(SqlConnection & SC,SUID u)
{
  QString Q                                 ;
  QString name = SC.getName                 (
                   PlanTable(Names)         ,
                   "uuid"                   ,
                   plan->LanguageId         ,
                   u                      ) ;
  NewTreeWidgetItem ( item                ) ;
  item -> setData   ( 0,Qt::UserRole,u    ) ;
  item -> setText   ( 0,name              ) ;
  Q = SC.sql.SelectFrom                     (
        "value"                             ,
        PlanTable(Variables)                ,
        SC.sql.Where(2,"uuid","name")     ) ;
  SC . Prepare ( Q                        ) ;
  SC . Bind    ( "uuid" , u               ) ;
  SC . Bind    ( "name" , "Database"      ) ;
  if (SC.Exec() && SC.Next())               {
    QString t = SC.String(0)                ;
    item -> setText ( 1, t )                ;
  }                                         ;
  Q = SC.sql.SelectFrom                     (
        "value"                             ,
        PlanTable(Variables)                ,
        SC.sql.Where(2,"uuid","name")     ) ;
  SC . Prepare ( Q                        ) ;
  SC . Bind    ( "uuid" , u               ) ;
  SC . Bind    ( "name" , "Table"         ) ;
  if (SC.Exec() && SC.Next())               {
    QString t = SC.String(0)                ;
    item -> setText ( 2, t )                ;
  }                                         ;
  Q = SC.sql.SelectFrom                     (
        "value"                             ,
        PlanTable(Variables)                ,
        SC.sql.Where(2,"uuid","name")     ) ;
  SC . Prepare ( Q                        ) ;
  SC . Bind    ( "uuid" , u               ) ;
  SC . Bind    ( "name" , "Enumeration"   ) ;
  if (SC.Exec() && SC.Next())               {
    QString t = SC.String(0)                ;
    item -> setText ( 3, t )                ;
  }                                         ;
  Q = SC.sql.SelectFrom                     (
        "value"                             ,
        PlanTable(Variables)                ,
        SC.sql.Where(2,"uuid","name")     ) ;
  SC . Prepare ( Q                        ) ;
  SC . Bind    ( "uuid" , u               ) ;
  SC . Bind    ( "name" , "ID"            ) ;
  if (SC.Exec() && SC.Next())               {
    QString t = SC.String(0)                ;
    item -> setText ( 4, t )                ;
  }                                         ;
  return item                               ;
}

bool N::SqlTables::startup(void)
{
  if (uuid>0) setDragDropMode ( DragDrop ) ;
         else setDragDropMode ( DragOnly ) ;
  clear    (                )              ;
  EnterSQL ( SC , plan->sql )              ;
    UUIDs   Uuids = GetUuids ( SC )        ;
    SUID    u                              ;
    foreach (u,Uuids)                      {
      QTreeWidgetItem * item               ;
      item = GetItem(SC,u)                 ;
      addTopLevelItem   ( item           ) ;
    }                                      ;
  LeaveSQL ( SC , plan->sql )              ;
  Alert    ( Done )                        ;
  return true                              ;
}

void N::SqlTables::doubleClicked(QTreeWidgetItem * item,int column)
{
  SUID        u = nTreeUuid(item,0)        ;
  QLineEdit * line                         ;
  switch (column)                          {
    case 0                                 :
      emit Edit ( u )                      ;
    break                                  ;
    case 1                                 :
      removeOldItem()                      ;
      line = setLineEdit                   (
             item , 1                      ,
             SIGNAL(editingFinished())     ,
             SLOT  (nameFinished   ())   ) ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
    case 2                                 :
      removeOldItem()                      ;
      line = setLineEdit                   (
             item , 2                      ,
             SIGNAL(editingFinished())     ,
             SLOT  (nameFinished   ())   ) ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
    case 3                                 :
      removeOldItem()                      ;
      line = setLineEdit                   (
             item , 3                      ,
             SIGNAL(editingFinished())     ,
             SLOT  (nameFinished   ())   ) ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
    case 4                                 :
      removeOldItem()                      ;
      line = setLineEdit                   (
             item , 4                      ,
             SIGNAL(editingFinished())     ,
             SLOT  (nameFinished   ())   ) ;
      line->setFocus(Qt::TabFocusReason)   ;
    break                                  ;
  }                                        ;
}

void N::SqlTables::New(void)
{
  NewTreeWidgetItem ( IT                   ) ;
  IT->setData       ( 0 , Qt::UserRole , 0 ) ;
  addTopLevelItem   ( IT                   ) ;
  scrollToItem      ( IT                   ) ;
  Rename            ( IT                   ) ;
}

void N::SqlTables::Rename(void)
{
  QTreeWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item) )              ;
  Rename   ( item         )              ;
}

void N::SqlTables::Rename(QTreeWidgetItem * item)
{
  SUID        u = nTreeUuid(item,0)    ;
  QLineEdit * line                     ;
  removeOldItem()                      ;
  line = setLineEdit                   (
         item , 0                      ,
         SIGNAL(editingFinished())     ,
         SLOT  (nameFinished   ())   ) ;
  line->setFocus(Qt::TabFocusReason)   ;
}

void N::SqlTables::removeOldItem(void)
{
  nDropOut ( IsNull(ItemEditing) )               ;
  SUID    u    = nTreeUuid(ItemEditing,0)        ;
  QString name = ItemEditing->text(0)            ;
  removeItemWidget (ItemEditing,ItemColumn)      ;
  if (u<=0)                                      {
    int index = indexOfTopLevelItem(ItemEditing) ;
    takeTopLevelItem(index)                      ;
  }                                              ;
  ItemEditing = NULL                             ;
  ItemWidget  = NULL                             ;
  ItemColumn  = -1                               ;
}

void N::SqlTables::nameFinished(void)
{
  nDropOut ( IsNull(ItemEditing) )                    ;
  QLineEdit * line = Casting  (QLineEdit ,ItemWidget) ;
  nDropOut ( IsNull(line       ) )                    ;
  ItemEditing -> setText ( ItemColumn,line->text() )  ;
  /////////////////////////////////////////////////////
  SUID    u    = nTreeUuid(ItemEditing,0)             ;
  QString name = ItemEditing->text(ItemColumn)        ;
  nDropOut ( uuid<=0 && name.length() <= 0 )          ;
  /////////////////////////////////////////////////////
  CodeManager CM ( plan )                             ;
  GroupItems  GI ( plan )                             ;
  EnterSQL ( SC , plan->sql )                         ;
    QString Q                                         ;
    switch (ItemColumn)                               {
      case 0                                          :
        if (u<=0)                                     {
          QString database = ItemEditing->text(1)     ;
          QString table    = ItemEditing->text(2)     ;
          uuid = SC.Unique                            (
                   PlanTable(MajorUuid)               ,
                   "uuid"                             ,
                   9002                             ) ;
          SC.assureUuid                               (
                   PlanTable(MajorUuid)               ,
                   u                                  ,
                   Types::SqlTable                  ) ;
          Q = SC.sql.InsertInto                       (
                   PlanTable(SqlTables)               ,
                   1,"uuid"                         ) ;
          SC . Prepare ( Q          )                 ;
          SC . Bind    ( "uuid" , u )                 ;
          SC . Exec    (            )                 ;
          ItemEditing->setData(0,Qt::UserRole,u)      ;
          if (uuid>0)                                 {
            UUIDs Juids                               ;
            Juids << u                                ;
            GI.Join                                   (
              SC                                      ,
              uuid                                    ,
              type                                    ,
              Types::SqlTable                         ,
              Groups::Subordination                   ,
              0,Juids                               ) ;
          }                                           ;
          if (database . length() > 0 )               {
            assureDatabase ( SC , u , database )      ;
          }                                           ;
          if (table    . length() > 0 )               {
            assureTable    ( SC , u , table    )      ;
          }                                           ;
        }                                             ;
        CM.assureName                                 (
              SC                                      ,
              u                                       ,
              plan->LanguageId                        ,
              name                                  ) ;
      break                                           ;
      case 1                                          :
        assureDatabase ( SC , u , name )              ;
      break                                           ;
      case 2                                          :
        assureTable    ( SC , u , name )              ;
      break                                           ;
      case 3                                          :
        assureEnum     ( SC , u , name )              ;
      break                                           ;
      case 4                                          :
        assureID       ( SC , u , name )              ;
      break                                           ;
    }                                                 ;
  LeaveSQL ( SC , plan->sql )                         ;
  removeOldItem (      )                              ;
  Alert         ( Done )                              ;
}

bool N::SqlTables::assureDatabase(SqlConnection & SC,SUID u,QString name)
{
  QString Q                                ;
  QString key = "Database"                 ;
  Q = SC.sql.DeleteFrom                    (
        PlanTable(Variables)               ,
        SC.sql.Where(2,"uuid","name")    ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Exec    (                         ) ;
  Q = SC.sql.InsertInto                    (
        PlanTable(Variables)               ,
        3,"uuid","name","value"          ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Bind    ( "value" , name.toUtf8() ) ;
  SC . Exec    (                         ) ;
  return true                              ;
}

bool N::SqlTables::assureTable(SqlConnection & SC,SUID u,QString name)
{
  QString Q                                ;
  QString key = "Table"                    ;
  Q = SC.sql.DeleteFrom                    (
        PlanTable(Variables)               ,
        SC.sql.Where(2,"uuid","name")    ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Exec    (                         ) ;
  Q = SC.sql.InsertInto                    (
        PlanTable(Variables)               ,
        3,"uuid","name","value"          ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Bind    ( "value" , name.toUtf8() ) ;
  SC . Exec    (                         ) ;
  return true                              ;
}

bool N::SqlTables::assureEnum(SqlConnection & SC,SUID u,QString name)
{
  QString Q                                ;
  QString key = "Enumeration"              ;
  Q = SC.sql.DeleteFrom                    (
        PlanTable(Variables)               ,
        SC.sql.Where(2,"uuid","name")    ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Exec    (                         ) ;
  Q = SC.sql.InsertInto                    (
        PlanTable(Variables)               ,
        3,"uuid","name","value"          ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Bind    ( "value" , name.toUtf8() ) ;
  SC . Exec    (                         ) ;
  return true                              ;
}

bool N::SqlTables::assureID(SqlConnection & SC,SUID u,QString name)
{
  QString Q                                ;
  QString key = "ID"                       ;
  Q = SC.sql.DeleteFrom                    (
        PlanTable(Variables)               ,
        SC.sql.Where(2,"uuid","name")    ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Exec    (                         ) ;
  Q = SC.sql.InsertInto                    (
        PlanTable(Variables)               ,
        3,"uuid","name","value"          ) ;
  SC . Prepare ( Q                       ) ;
  SC . Bind    ( "uuid"  , u             ) ;
  SC . Bind    ( "name"  , key.toUtf8()  ) ;
  SC . Bind    ( "value" , name.toUtf8() ) ;
  SC . Exec    (                         ) ;
  return true                              ;
}

bool N::SqlTables::contains(QString database,QString table)
{
  nFullLoop (i,topLevelItemCount()) {
    QTreeWidgetItem * item          ;
    item = topLevelItem      ( i )  ;
    QString d = item -> text ( 1 )  ;
    QString t = item -> text ( 2 )  ;
    if (d==database && t==table)    {
      return true                   ;
    }                               ;
  }                                 ;
  return false                      ;
}

void N::SqlTables::addTable(QString database,QString table)
{
  nDropOut ( contains ( database , table ) ) ;
  NewTreeWidgetItem   ( item               ) ;
  item -> setData ( 0 , Qt::UserRole , 0   ) ;
  item -> setText ( 1 , database           ) ;
  item -> setText ( 2 , table              ) ;
  addTopLevelItem ( item                   ) ;
}

bool N::SqlTables::Menu(QPoint pos)
{
  QMdiSubWindow   * mdi  = Casting(QMdiSubWindow,parent())     ;
  QDockWidget     * dock = Casting(QDockWidget  ,parent())     ;
  QTreeWidgetItem * item = itemAt(pos)                         ;
  MenuManager       mm (this)                                  ;
  QAction         * aa                                         ;
  QMenu           * mi                                         ;
  if (NotNull(item)) mm.add(101,tr("Edit"  ))                  ;
  if (NotNull(item)) mm.add(102,tr("Rename"))                  ;
  mm.add(103,tr("Refresh"            ))                        ;
  mm.addSeparator()                                            ;
  mm.add(104,tr("Export C/C++ codes" ))                        ;
  if (uuid>0) mm.add(105,tr("Relocate table orders" ))         ;
  mm.addSeparator()                                            ;
  mi  = mm.addMenu(tr("Columns"))                              ;
  aa  = mm.add(mi,301,tr("Enable sorting"))                    ;
  aa -> setCheckable ( true               )                    ;
  aa -> setChecked   ( isSortingEnabled() )                    ;
  mm.addSeparator(mi)                                          ;
  aa  = mm.add(mi,501,tr("Database")      )                    ;
  aa -> setCheckable ( true               )                    ;
  aa -> setChecked   ( !isColumnHidden(1) )                    ;
  aa  = mm.add(mi,502,tr("Table")         )                    ;
  aa -> setCheckable ( true               )                    ;
  aa -> setChecked   ( !isColumnHidden(2) )                    ;
  aa  = mm.add(mi,503,tr("Enumeration")   )                    ;
  aa -> setCheckable ( true               )                    ;
  aa -> setChecked   ( !isColumnHidden(3) )                    ;
  aa  = mm.add(mi,504,tr("Number")        )                    ;
  aa -> setCheckable ( true               )                    ;
  aa -> setChecked   ( !isColumnHidden(4) )                    ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont ( plan )                                        ;
  aa = mm.exec (      )                                        ;
  nKickOut ( IsNull(aa) , true )                               ;
  switch (mm[aa])                                              {
    case 101                                                   :
      if (NotNull(item)) emit Edit(nTreeUuid(item,0))          ;
    break                                                      ;
    case 102                                                   :
      Rename    ( )                                            ;
    break                                                      ;
    case 103                                                   :
      startup   ( )                                            ;
    break                                                      ;
    case 104                                                   :
      ExportCpp ( )                                            ;
    break                                                      ;
    case 105                                                   :
      Ordering  ( )                                            ;
    break                                                      ;
    case 301                                                   :
      setSortingEnabled (      aa->isChecked() )               ;
    break                                                      ;
    case 501                                                   :
      setColumnHidden   ( 1 , !aa->isChecked() )               ;
      SuitableColumns   (                      )               ;
    break                                                      ;
    case 502                                                   :
      setColumnHidden   ( 2 , !aa->isChecked() )               ;
      SuitableColumns   (                      )               ;
    break                                                      ;
    case 503                                                   :
      setColumnHidden   ( 3 , !aa->isChecked() )               ;
      SuitableColumns   (                      )               ;
    break                                                      ;
    case 504                                                   :
      setColumnHidden   ( 4 , !aa->isChecked() )               ;
      SuitableColumns   (                      )               ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi (this,Qt::Vertical)                       ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::RightDockWidgetArea                                ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea       ) ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}

bool N::SqlTables::dropSqlTable(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                  ;
  Q_UNUSED ( pos    )                  ;
  nKickOut       ( uuid <= 0 , false ) ;
  GroupItems GI  ( plan              ) ;
  EnterSQL       ( SC , plan->sql    ) ;
    UUIDs Juids = Uuids                ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      type                             ,
      Types::SqlTable                  ,
      Groups::Subordination            ,
      0,Juids                        ) ;
  LeaveSQL       ( SC , plan->sql    ) ;
  startup        (                   ) ;
  return true                          ;
}

void N::SqlTables::Ordering(void)
{
  nDropOut       ( uuid <= 0          )            ;
  UUIDs Uuids                                      ;
  nFullLoop(i,topLevelItemCount())                 {
    QTreeWidgetItem * item                         ;
    item = topLevelItem(i)                         ;
    SUID uuid = nTreeUuid(item,0)                  ;
    if (uuid>0) Uuids << uuid                      ;
  }                                                ;
  nDropOut       ( Uuids.count() <= 0 )            ;
  EnterSQL       ( SC , plan->sql     )            ;
    QString Q                                      ;
    int t2       = Types::SqlTable                 ;
    int relation = Groups::Subordination           ;
    nFullLoop(i,Uuids.count())                     {
      SUID u = Uuids[i]                            ;
      Q = SC.sql.Update                            (
            PlanTable(Groups)                      ,
            SC.sql.Where                           (
              5                                    ,
              "first"                              ,
              "second"                             ,
              "t1"                                 ,
              "t2"                                 ,
              "relation"                         ) ,
            1,"position"                         ) ;
      SC . Prepare ( Q                           ) ;
      SC . Bind    ( "first"    , uuid           ) ;
      SC . Bind    ( "second"   , u              ) ;
      SC . Bind    ( "t1"       , type           ) ;
      SC . Bind    ( "t2"       , t2             ) ;
      SC . Bind    ( "relation" , relation       ) ;
      SC . Bind    ( "position" , i+1            ) ;
      SC . Exec    (                             ) ;
    }                                              ;
  LeaveSQL       ( SC , plan->sql )                ;
  Alert          ( Done           )                ;
}

void N::SqlTables::ExportCpp(void)
{
  QString filename                                                          ;
  filename = QFileDialog::getSaveFileName                                   (
               this                                                         ,
               tr("Export as Rishon C/C++ code")                            ,
               plan->Temporary("")                                          ,
               "*.cpp"                         )                            ;
  nDropOut ( filename.length() <= 0 )                                       ;
  int     Length = 0                                                        ;
  int     Count  = topLevelItemCount()                                      ;
  nFullLoop(i,Count)                                                        {
    QTreeWidgetItem * item = topLevelItem(i)                                ;
    QString L = item->text(3)                                               ;
    if (L.length()>Length) Length = L.length()                              ;
  }                                                                         ;
  QString C                                                                 ;
  QString S = "      "                                                      ;
  C += "#include <Exciton>" ; C += "\n\n"                                   ;
  C += "\n"                                                                 ;
  C += "  namespace Tables                    {\n\n"                        ;
  C += "    enum SystemTables                 {\n"                          ;
  nFullLoop(i,Count)                                                        {
    QTreeWidgetItem * item = topLevelItem(i)                                ;
    QString L = item->text(3)                                               ;
    QString N = item->text(4)                                               ;
    QString T = " "                                                         ;
    if (L.length()>0 && N.length()>0)                                       {
      T  = T.repeated(Length+2-L.length())                                  ;
      C += S                                                                ;
      C += L                                                                ;
      C += T                                                                ;
      C += "= "                                                             ;
      C += N                                                                ;
      if ((i+1)<Count) C += " ,"                                            ;
      C += "\n"                                                             ;
    }                                                                       ;
  }                                                                         ;
  C += "    }                                 ;\n"                          ;
  C += "\n"                                                                 ;
  C += "  }\n"                                                              ;
  C += "\n"                                                                 ;
  C += "\n"                                                                 ;
  C += "void Neutrino::InstallTables(nPlan & Plan)\n"                       ;
  C += "{\n"                                                                ;
  C += "  #define ADD(ENU,TAB) Plan.addTable(Neutrino::Tables::ENU,#TAB)\n" ;
  nFullLoop(i,Count)                                                        {
    QTreeWidgetItem * item = topLevelItem(i)                                ;
    QString T = item->text(2)                                               ;
    QString L = item->text(3)                                               ;
    if (T.length()>0 && L.length()>0)                                       {
      C += QString("  ADD(%1,%2);\n").arg(L,T)                              ;
    }                                                                       ;
  }                                                                         ;
  C += "  #undef  ADD\n"                                                    ;
  C += "}\n"                                                                ;
  C += "\n"                                                                 ;
  QFile F(filename)                                                         ;
  if (F.open(QIODevice::WriteOnly))                                         {
    F . write ( C.toUtf8() )                                                ;
    F . close (            )                                                ;
    Alert     ( Done       )                                                ;
  } else                                                                    {
    Alert     ( Error      )                                                ;
  }                                                                         ;
}
