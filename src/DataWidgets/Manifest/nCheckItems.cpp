#include <datawidgets.h>

N::CheckItems :: CheckItems    ( QWidget * parent , Plan * p )
               : TreeDock      (           parent ,        p )
               , Object        ( 0 , Types::None             )
               , Position      ( 0                           )
               , Sorting       ( Qt::AscendingOrder          )
               , backgrounding ( true                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CheckItems ::~CheckItems(void)
{
}

bool N::CheckItems::acceptTapHold(QTapAndHoldGesture * gesture)
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

void N::CheckItems::Configure(void)
{
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("Item"       ) ) ;
  head -> setText              ( 1 , tr("Value"      ) ) ;
  head -> setText              ( 2 , tr("Rating"     ) ) ;
  head -> setText              ( 3 , ""                ) ;
  setAccessibleName            ( "CheckItems"          ) ;
  setWindowTitle               ( tr("Manifest"       ) ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( SingleSelection       ) ;
  setColumnCount               ( 4                     ) ;
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
  nConnect ( this , SIGNAL ( AppendItem () )             ,
             this , SLOT   ( Insert     () )           ) ;
}

bool N::CheckItems::FocusIn(void)
{
  LinkAction ( Refresh    , startup        () ) ;
  LinkAction ( Export     , Export         () ) ;
  LinkAction ( Insert     , Insert         () ) ;
  LinkAction ( Delete     , Delete         () ) ;
  LinkAction ( Paste      , Paste          () ) ;
  LinkAction ( Copy       , CopyToClipboard() ) ;
  LinkAction ( SelectNone , SelectNone     () ) ;
  LinkAction ( SelectAll  , SelectAll      () ) ;
  return true                                   ;
}

void N::CheckItems::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch   ( Type ) {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

bool N::CheckItems::startup(void)
{
  if (backgrounding) start ( 10001 ) ;
                else List  (       ) ;
  return true                        ;
}

void N::CheckItems::List(void)
{
  Position = 0                                     ;
  clear    (                )                      ;
  EnterSQL ( SC , plan->sql )                      ;
    QString Q                                      ;
    QList<int> IDs                                 ;
    QString SortString                             ;
    if (nEqual(Sorting,Qt::AscendingOrder))        {
      SortString = "asc"                           ;
    } else SortString = "desc"                     ;
    Q = SC.sql.SelectFrom                          (
          "position,name,value,rating"             ,
          PlanTable(CheckItems)                    ,
          QString("where uuid = %1 "
                  " order by position %2"          )
          .arg(uuid).arg(SortString)             ) ;
    SqlLoopNow(SC,Q)                               ;
      NewTreeWidgetItem(item)                      ;
      int     pos   = SC.Int   (0)                 ;
      QString name  = SC.String(1)                 ;
      QString value = SC.String(2)                 ;
      int     rate  = SC.Int   (3)                 ;
      IDs << pos                                   ;
      Position = pos                               ;
      item->setData   ( 0,Qt::UserRole,pos       ) ;
      item->setText   ( 0,name                   ) ;
      item->setText   ( 1,value                  ) ;
      item->setText   ( 2,QString::number(rate)  ) ;
      setAlignments   ( item                     ) ;
      addTopLevelItem ( item                     ) ;
    SqlLoopErr(SC,Q)                               ;
    SqlLoopEnd(SC,Q)                               ;
  LeaveSQL(SC,plan->sql)                           ;
  reportItems      (                             ) ;
  emit AutoFit     (                             ) ;
  Alert            ( Done                        ) ;
  allowGesture = true                              ;
  if (topLevelItemCount()<=0)                      {
    emit AppendItem ( )                            ;
  }                                                ;
}

bool N::CheckItems::Menu(QPoint pos)
{ Q_UNUSED ( pos )                                             ;
  allowGesture = false                                         ;
  QMdiSubWindow   * mdi    = Casting(QMdiSubWindow,parent())   ;
  QDockWidget     * dock   = Casting(QDockWidget  ,parent())   ;
  MenuManager       mm (this)                                  ;
  QAction          * aa                                        ;
  mm . add(102,tr("Refresh"))                                  ;
  mm . add(201,tr("Export" ))                                  ;
  mm . addSeparator ( )                                        ;
  mm . add(501,tr("Insert"           ))                        ;
  mm . add(502,tr("Delete"           ))                        ;
  mm . add(503,tr("Paste"            ))                        ;
  mm . add(504,tr("Copy to clipboard"))                        ;
  mm . addSeparator ( )                                        ;
  mm . add(601,tr("Clear selections" ))                        ;
  mm . add(602,tr("Select all"       ))                        ;
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
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area")) ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  )) ;
  mm . setFont(plan)                                           ;
  aa = mm.exec()                                               ;
  if (IsNull(aa))                                              {
    allowGesture = true                                        ;
    return true                                                ;
  }                                                            ;
  switch (mm[aa])                                              {
    case 102                                                   :
      startup ( )                                              ;
      allowGesture = true                                      ;
    break                                                      ;
    case 201                                                   :
      Export  ( )                                              ;
      allowGesture = true                                      ;
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
      Insert          ( )                                      ;
    break                                                      ;
    case 502                                                   :
      Delete          ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 503                                                   :
      Paste           ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 504                                                   :
      CopyToClipboard ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 601                                                   :
      SelectNone      ( )                                      ;
      allowGesture = true                                      ;
    break                                                      ;
    case 602                                                   :
      SelectAll       ( )                                      ;
      allowGesture = true                                      ;
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
    default                                                    :
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}

void N::CheckItems::Insert(void)
{
  NewTreeWidgetItem(item)                 ;
  item->setData(0,Qt::UserRole,0)         ;
  item->setTextAlignment                  (
                    2,Qt::AlignRight      |
                      Qt::AlignVCenter  ) ;
  if (nEqual(Sorting,Qt::AscendingOrder)) {
    addTopLevelItem(item)                 ;
  } else                                  {
    insertTopLevelItem(0,item)            ;
  }                                       ;
  doubleClicked(item,0)                   ;
}

void N::CheckItems::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Save check items")     ,
                       plan->Temporary("")        ,
                       tr("Text file (*.txt)")  ) ;
  if (filename.length()<=0) return                ;
  QFile F(filename)                               ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * item  = topLevelItem(i)     ;
    QString full  = ""                            ;
    full  = item->text(0)                         ;
    full += " "                                   ;
    full += item->text(2)                         ;
    full += " "                                   ;
    full += item->text(1)                         ;
    if (full.length()>0)                          {
      full += "\r\n"                              ;
      F.write(full.toUtf8())                      ;
    }                                             ;
  }                                               ;
  F.close()                                       ;
  Alert ( Done )                                  ;
  allowGesture = true                             ;
}

void N::CheckItems::Delete(void)
{
  QTreeWidgetItem * item = currentItem() ;
  if (IsNull(item)) return               ;
  int id = nTreeInt(item,0)              ;
  EnterSQL ( SC , plan->sql )            ;
    QString Q                            ;
    Q = SC.sql.DeleteFrom                (
          PlanTable(CheckItems)          ,
          QString("where uuid = %1 and "
                  "position = %2 ;"      )
          .arg(uuid).arg(id)           ) ;
    SC . Query ( Q )                     ;
  LeaveSQL ( SC , plan->sql )            ;
  int index = indexOfTopLevelItem(item)  ;
  if (index>=0) takeTopLevelItem(index)  ;
  Alert       ( Done )                   ;
  reportItems (      )                   ;
  allowGesture = true                    ;
}

void N::CheckItems::Paste(void)
{
  allowGesture = false                         ;
  QString ts = qApp->clipboard()->text()       ;
  if (ts.length()<=0) return                   ;
  QStringList RS = ts.split('\n')              ;
  if (RS.count()<=0) return                    ;
  QStringList IS                               ;
  for (int i=0;i<topLevelItemCount();i++)      {
    QTreeWidgetItem * item = topLevelItem(i)   ;
    IS << item->text(0)                        ;
  }                                            ;
  QStringList AS                               ;
  for (int i=0;i<RS.count();i++)               {
    QString N = RS[i]                          ;
    N = N.replace("\r","")                     ;
    N = N.replace("\n","")                     ;
    if (N.length()>0)                          {
      if (!IS.contains(N)) AS << N             ;
    }                                          ;
  }                                            ;
  allowGesture = true                          ;
  if (AS.count()<=0) return                    ;
  allowGesture = false                         ;
  EnterSQL ( SC , plan->sql )                  ;
    QString Q                                  ;
    for (int i=0;i<AS.count();i++)             {
      QString N = AS[i]                        ;
      Q = SC.sql.InsertInto                    (
            PlanTable(CheckItems)              ,
            3,"uuid","position","name"       ) ;
      Position ++                              ;
      SC.Prepare ( Q                         ) ;
      SC.Bind    ( "uuid"     , uuid         ) ;
      SC.Bind    ( "position" , Position     ) ;
      SC.Bind    ( "name"     , N.toUtf8()   ) ;
      SC.Exec    (                           ) ;
      NewTreeWidgetItem(item)                  ;
      item->setData(0,Qt::UserRole,Position  ) ;
      item->setText(0,N                      ) ;
      addTopLevelItem(item)                    ;
    }                                          ;
  LeaveSQL ( SC , plan->sql )                  ;
  Alert ( Done )                               ;
  allowGesture = true                          ;
}

void N::CheckItems::doubleClicked(QTreeWidgetItem * item,int column)
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
               SLOT  (valueFinished  ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 2                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (rateFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    default                                :
      Menu ( QCursor::pos() )              ;
    break                                  ;
  }                                        ;
}

void N::CheckItems::enumFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    id   = nTreeInt(ItemEditing,0)           ;
  QString name = line->text()                      ;
  if (name.length()>0)                             {
    EnterSQL ( SC , plan->sql )                    ;
      QString Q                                    ;
      if (id==0)                                   {
        Q = SC.sql.InsertInto                      (
              PlanTable(CheckItems)                ,
              3,"uuid","position","name"         ) ;
        Position ++                                ;
        id = Position                              ;
      } else                                       {
        Q = SC.sql.Update                          (
              PlanTable(CheckItems)                ,
     "where uuid = :UUID and position = :POSITION" ,
              1,"name"                           ) ;
      }                                            ;
      SC.Prepare ( Q                             ) ;
      SC.Bind    ( "uuid"     , uuid             ) ;
      SC.Bind    ( "position" , id               ) ;
      SC.Bind    ( "name"     , name.toUtf8()    ) ;
      SC.Exec    (                               ) ;
      ItemEditing->setData(0,Qt::UserRole,id)      ;
      ItemEditing->setText(0,name           )      ;
    LeaveSQL ( SC , plan->sql )                    ;
  }                                                ;
  removeOldItem ( true , 0 )                       ;
  Alert ( Done )                                   ;
  allowGesture = true                              ;
}

void N::CheckItems::valueFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    id    = nTreeInt(ItemEditing,0)          ;
  QString Value = line->text()                     ;
  if (Value.length()>0)                            {
    EnterSQL ( SC , plan->sql )                    ;
      QString Q                                    ;
      Q = SC.sql.Update(PlanTable(CheckItems)      ,
     "where uuid = :UUID and position = :POSITION" ,
          1,"value"                              ) ;
      SC.Prepare ( Q                             ) ;
      SC.Bind    ( "uuid"     , uuid             ) ;
      SC.Bind    ( "position" , id               ) ;
      SC.Bind    ( "value"    , Value.toUtf8()   ) ;
      SC.Exec    (                               ) ;
      ItemEditing->setText(1,Value               ) ;
    LeaveSQL ( SC , plan->sql )                    ;
  }                                                ;
  removeOldItem ( true , 0 )                       ;
  Alert ( Done )                                   ;
  allowGesture = true                              ;
}

void N::CheckItems::rateFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    id   = nTreeInt(ItemEditing,0)           ;
  QString Rate = line->text()                      ;
  if (Rate.length()>0)                             {
    EnterSQL ( SC , plan->sql )                    ;
      QString Q                                    ;
      Q = SC.sql.Update(PlanTable(CheckItems)      ,
     "where uuid = :UUID and position = :POSITION" ,
          1,"rating"                             ) ;
      SC.Prepare ( Q                             ) ;
      SC.Bind    ( "uuid"     , uuid             ) ;
      SC.Bind    ( "position" , id               ) ;
      SC.Bind    ( "rating"   , Rate.toInt()     ) ;
      SC.Exec    (                               ) ;
      ItemEditing->setText(2,Rate                ) ;
    LeaveSQL ( SC , plan->sql )                    ;
  }                                                ;
  removeOldItem ( true , 0 )                       ;
  Alert ( Done )                                   ;
  allowGesture = true                              ;
}
