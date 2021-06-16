#include <datawidgets.h>

N::CheckLists :: CheckLists    ( QWidget * parent , Plan * p )
               : TreeDock      (           parent ,        p )
               , Object        ( 0 , Types::None             )
               , Sorting       ( Qt::DescendingOrder         )
               , backgrounding ( true                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CheckLists ::~CheckLists(void)
{
}

bool N::CheckLists::acceptTapHold(QTapAndHoldGesture * gesture)
{
  if ( IsNull(gesture) ) return false        ;
  QTreeWidgetItem * item = currentItem ( )   ;
  if (NotNull(item))                         {
    int column = currentColumn()             ;
    allowGesture = false                     ;
    emit itemDoubleClicked ( item , column ) ;
    return true                              ;
  }                                          ;
  return false                               ;
}

void N::CheckLists::Configure(void)
{
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("Check list" ) ) ;
  head -> setText              ( 1 , tr("Name"       ) ) ;
  head -> setText              ( 2 , tr("Items"      ) ) ;
  head -> setText              ( 3 , ""                ) ;
  setAccessibleName            ( "CheckLists"          ) ;
  setWindowTitle               ( tr("Check lists")     ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( SingleSelection       ) ;
  setColumnCount               ( 3                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setAllAlignments             ( head,Qt::AlignCenter  ) ;
  setHeaderItem                ( head                  ) ;
  setAlignment                 ( 2 , Qt::AlignRight      |
                                     Qt::AlignVCenter  ) ;
  plan -> setFont              ( this                  ) ;
  MountClicked                 ( 2                     ) ;
  grabGesture                  ( Qt::TapAndHoldGesture ) ;
  allowGesture = true                                    ;
}

bool N::CheckLists::FocusIn(void)
{
  LinkAction ( Refresh    , startup        () ) ;
  LinkAction ( Insert     , Insert         () ) ;
  LinkAction ( Copy       , CopyToClipboard() ) ;
  LinkAction ( SelectNone , SelectNone     () ) ;
  LinkAction ( SelectAll  , SelectAll      () ) ;
  return true                                   ;
}

void N::CheckLists::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch (Type)     {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

bool N::CheckLists::startup(void)
{
  if (backgrounding) start ( 10001 ) ;
                else List  (       ) ;
  return true                        ;
}

void N::CheckLists::List(void)
{
  clear    (                )                      ;
  GroupItems GI ( plan )                           ;
  EnterSQL ( SC , plan->sql )                      ;
    QString Q                                      ;
    QString name                                   ;
    QString Enum                                   ;
    SUID    u                                      ;
    UUIDs   Uuids                                  ;
    QString SortString                             ;
    if (uuid<=0)                                   {
      if (nEqual(Sorting,Qt::AscendingOrder))      {
        SortString = "order by id asc"             ;
      } else SortString = "order by id desc"       ;
      Uuids = SC.Uuids                             (
                        PlanTable(CheckLists)      ,
                        "uuid" , SortString      ) ;
    } else                                         {
      if (nEqual(Sorting,Qt::AscendingOrder))      {
        SortString = "order by position asc"       ;
      } else SortString = "order by position desc" ;
      Uuids = GI.Subordination                     (
                SC                                 ,
                uuid                               ,
                type                               ,
                Types::Enumeration                 ,
                Groups::Subordination              ,
                SortString                       ) ;
    }                                              ;
    foreach (u,Uuids)                              {
      NewTreeWidgetItem ( IT )                     ;
      int Total = 0                                ;
      name = SC.getName ( PlanTable(Names)         ,
               "uuid",vLanguageId,u              ) ;
      Q = SC.sql.SelectFrom                        (
            "name"                                 ,
            PlanTable(CheckLists)                  ,
            SC.WhereUuid(u)                      ) ;
      if (SC.Fetch(Q)) Enum = SC.String(0)         ;
      Q = SC.sql.SelectFrom                        (
            "count(*)"                             ,
            PlanTable(CheckItems)                  ,
            SC.WhereUuid(u)                      ) ;
      if (SC.Fetch(Q)) Total = SC.Int(0)           ;
      IT -> setData   ( 0,Qt::UserRole,u         ) ;
      IT -> setText   ( 0,Enum                   ) ;
      IT -> setText   ( 1,name                   ) ;
      IT -> setText   ( 2,QString::number(Total) ) ;
      setAlignments   ( IT                       ) ;
      addTopLevelItem ( IT                       ) ;
    }                                              ;
  LeaveSQL(SC,plan->sql)                           ;
  reportItems      (                             ) ;
  emit AutoFit     (                             ) ;
  Alert            ( Done                        ) ;
  allowGesture = true                              ;
}

bool N::CheckLists::Menu(QPoint pos)
{ Q_UNUSED ( pos )                                             ;
  allowGesture              = false                            ;
  QMdiSubWindow    * mdi    = Casting(QMdiSubWindow,parent())  ;
  QDockWidget      * dock   = Casting(QDockWidget  ,parent())  ;
  QTreeWidgetItem  * IT     = currentItem()                    ;
  MenuManager        mm (this)                                 ;
  QAction          * aa                                        ;
  if (NotNull(IT  )) mm.add(    101,tr("Edit"               )) ;
  mm . add ( 102 , tr("Refresh") )                             ;
  mm . add ( 201 , tr("Insert" ) )                             ;
  mm . addSeparator ( )                                        ;
  if (nEqual(Sorting,Qt::AscendingOrder))                      {
    mm .add(302,tr("Descending list"))                         ;
  } else                                                       {
    mm .add(301,tr("Ascending list" ))                         ;
  }                                                            ;
  aa  = mm.add(401,tr("Enable sorting"))                       ;
  aa -> setCheckable(true)                                     ;
  aa -> setChecked(isSortingEnabled())                         ;
  mm . addSeparator ( )                                        ;
  mm . add ( 501 , tr("Copy to clipboard") )                   ;
  mm . add ( 502 , tr("Clear selections" ) )                   ;
  mm . add ( 503 , tr("Select all"       ) )                   ;
  mm . addSeparator ( )                                        ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont(plan)                                           ;
  aa = mm.exec()                                               ;
  if ( IsNull(aa) )                                            {
    allowGesture = true                                        ;
    return true                                                ;
  }                                                            ;
  switch (mm[aa])                                              {
    case     101                                               :
      if (NotNull(IT))                                         {
        SUID u = nTreeUuid ( IT , 0 )                          ;
        emit Edit(this,IT->text(1),u)                          ;
      }                                                        ;
      allowGesture = true                                      ;
    break                                                      ;
    case 102                                                   :
      startup ( )                                              ;
    break                                                      ;
    case 201                                                   :
      Insert  ( )                                              ;
    break                                                      ;
    case 301                                                   :
      Sorting = Qt::AscendingOrder                             ;
      startup ( )                                              ;
      allowGesture = true                                      ;
    break                                                      ;
    case 302                                                   :
      Sorting = Qt::DescendingOrder                            ;
      startup ( )                                              ;
      allowGesture = true                                      ;
    break                                                      ;
    case 401                                                   :
      setSortingEnabled(aa->isChecked())                       ;
      allowGesture = true                                      ;
    break                                                      ;
    case 501                                                   :
      CopyToClipboard ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 502                                                   :
      SelectNone      ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 503                                                   :
      SelectAll       ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi (this,Qt::Vertical)                       ;
      allowGesture = true                                      ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::RightDockWidgetArea                                ,
        Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea       ) ;
      allowGesture = true                                      ;
    break                                                      ;
    default                                                    :
      allowGesture = true                                      ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}

void N::CheckLists::Insert(void)
{
  NewTreeWidgetItem(item)                 ;
  item->setData(0,Qt::UserRole,0)         ;
  item-> setTextAlignment                 (
           2,Qt::AlignRight               |
             Qt::AlignVCenter           ) ;
  if (nEqual(Sorting,Qt::AscendingOrder)) {
    addTopLevelItem(item)                 ;
  } else                                  {
    insertTopLevelItem(0,item)            ;
  }                                       ;
  doubleClicked(item,0)                   ;
}

void N::CheckLists::doubleClicked(QTreeWidgetItem *item,int column)
{
  QLineEdit * line                         ;
  allowGesture = false                     ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (enumFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 1                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    default                                :
      if (column<4)                        {
        Menu ( QCursor::pos() )            ;
      }                                    ;
    break                                  ;
  }                                        ;
}

void N::CheckLists::enumFinished(void)
{
  nDropOut ( IsNull(ItemEditing) )                 ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  nDropOut ( IsNull(line       ) )                 ;
  SUID    u    = nTreeUuid(ItemEditing,0)          ;
  QString Enum = line->text()                      ;
  if (Enum.length()>0)                             {
    GroupItems GI ( plan )                         ;
    EnterSQL ( SC , plan->sql )                    ;
      QString Q                                    ;
      QByteArray ENUM = Enum.toUtf8()              ;
      if (u<=0)                                    {
        u = SC.Unique ( PlanTable(MajorUuid)       ,
                           "uuid",9877           ) ;
        SC.assureUuid ( PlanTable(MajorUuid)       ,
                        u,Types::Enumeration     ) ;
        Q = SC.sql.InsertInto                      (
              PlanTable(CheckLists)                ,
              3,"uuid","length","name"           ) ;
        if (uuid>0)                                {
          UUIDs Guids                              ;
          Guids << u                               ;
          GI.Join                                  (
            SC                                     ,
            uuid                                   ,
            type                                   ,
            Types::Enumeration                     ,
            Groups::Subordination                  ,
            0,Guids                              ) ;
        }                                          ;
      } else                                       {
        Q = SC.sql.Update                          (
              PlanTable(CheckLists)                ,
              "where uuid = :UUID"                 ,
              2,"name","length"                  ) ;
      }                                            ;
      SC . Prepare ( Q                      )      ;
      SC . Bind    ( "uuid"   , u           )      ;
      SC . Bind    ( "length" , ENUM.size() )      ;
      SC . Bind    ( "name"   , ENUM        )      ;
      SC . Exec    (                        )      ;
      ItemEditing->setData( 0 , Qt::UserRole,u )   ;
      ItemEditing->setText( 0 , Enum           )   ;
    LeaveSQL ( SC , plan->sql )                    ;
  }                                                ;
  removeOldItem ( true , 0 )                       ;
  Alert         ( Done     )                       ;
  allowGesture = true                              ;
}

void N::CheckLists::nameFinished(void)
{
  nDropOut ( IsNull(ItemEditing) )                     ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)     ;
  nDropOut ( IsNull(line       ) )                     ;
  SUID    u    = nTreeUuid(ItemEditing,0)              ;
  QString name = line->text()                          ;
  EnterSQL ( SC , plan->sql )                          ;
    SC.assureName(PlanTable(Names),u,vLanguageId,name) ;
    ItemEditing->setText( 1 , name )                   ;
  LeaveSQL ( SC , plan->sql )                          ;
  removeOldItem ( true , 0 )                           ;
  Alert         ( Done     )                           ;
  allowGesture = true                                  ;
}
