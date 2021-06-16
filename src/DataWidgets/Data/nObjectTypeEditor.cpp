#include <datawidgets.h>

N::ObjectTypeEditor:: ObjectTypeEditor ( QWidget * parent , Plan * p )
                    : TreeWidget       (           parent ,        p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ObjectTypeEditor::~ObjectTypeEditor (void)
{
}

QSize N::ObjectTypeEditor::SuitableSize(void)
{
  QSize  s = size()             ;
  CUIDs  w = columnWidths()     ;
  int    t = 0                  ;
  if (w.count()<=0) return s    ;
  for (int i=0;i<w.count();i++) {
    t += w[i]                   ;
  }                             ;
  s . setWidth ( t )            ;
  return s                      ;
}

void N::ObjectTypeEditor::Configure(void)
{
  NewTreeWidgetItem            ( head                      ) ;
  head -> setText              ( 0 , tr("Type")            ) ;
  head -> setText              ( 1 , tr("Name")            ) ;
  head -> setText              ( 2 , tr("Enumeration")     ) ;
  head -> setText              ( 3 , tr("Statistics")      ) ;
  head -> setText              ( 4 , ""                    ) ;
  ////////////////////////////////////////////////////////////
  setWindowTitle               ( tr("Object types")        ) ;
  setDragDropMode              ( NoDragDrop                ) ;
  setRootIsDecorated           ( false                     ) ;
  setAlternatingRowColors      ( true                      ) ;
  setSelectionMode             ( SingleSelection           ) ;
  setColumnCount               ( 5                         ) ;
  setColumnHidden              ( 3 , true                  ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded     ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded     ) ;
  assignHeaderItems            ( head                      ) ;
  plan -> setFont              ( this                      ) ;
  MountClicked                 ( 2                         ) ;
  MountClicked                 ( 9                         ) ;
  ////////////////////////////////////////////////////////////
  if (!plan->Booleans["Desktop"])                            {
    allowGesture = true                                      ;
    grabGesture ( Qt::TapAndHoldGesture )                    ;
    grabGesture ( Qt::PanGesture        )                    ;
  }                                                          ;
}

void N::ObjectTypeEditor::List(void)
{
  start ( 1001 ) ;
}

void N::ObjectTypeEditor::AllTypes(void)
{
  blockSignals ( true  )                                                    ;
  clear        (       )                                                    ;
  emit OnBusy  (       )                                                    ;
  EnterSQL(SC,plan->sql)                                                    ;
    UUIDs   Uuids = SC.Uuids(PlanTable(Types),"type","order by id asc")     ;
    SUID    uuid                                                            ;
    QString Q                                                               ;
    foreach (uuid,Uuids)                                                    {
      QString name = GetName(SC,uuid)                                       ;
      QString Enum                                                          ;
      int     flags                                                         ;
      Q = SC.sql.SelectFrom("flags",PlanTable(Types)                        ,
                            QString("where type = %1").arg(uuid)          ) ;
      if (SC.Fetch(Q)) flags = SC . Int ( 0 )                               ;
      Q = SC.sql.SelectFrom("value",PlanTable(Variables)                    ,
                            "where uuid = :UUID and name = :NAME"         ) ;
      SC.Prepare ( Q                      )                                 ;
      SC.Bind    ( "uuid" , uuid          )                                 ;
      SC.Bind    ( "name" , "Enumeration" )                                 ;
      if (SC.Exec() && SC.Next()) Enum = SC.String(0)                       ;
      NewTreeWidgetItem  ( item )                                           ;
      item->setCheckState(0,IsMask(flags,1) ? Qt::Checked : Qt::Unchecked ) ;
      item->setData          (0,Qt::UserRole   ,uuid           )            ;
      item->setTextAlignment (0,Qt::AlignRight|Qt::AlignVCenter)            ;
      item->setText          (0,QString::number(uuid)          )            ;
      item->setText          (1,name                           )            ;
      item->setText          (2,Enum                           )            ;
      addTopLevelItem    ( item )                                           ;
    }                                                                       ;
  LeaveSQL(SC,plan->sql)                                                    ;
  blockSignals ( false )                                                    ;
  emit AutoFit ( )                                                          ;
  emit GoRelax ( )                                                          ;
  Alert ( Done )                                                            ;
}

void N::ObjectTypeEditor::doubleClicked (QTreeWidgetItem * item,int column)
{
  switch (column)                   {
    case 1                          :
      setLineEdit                   (
        item,column                 ,
        SIGNAL(editingFinished())   ,
        SLOT  (nameChanged    ()) ) ;
    break                           ;
    case 2                          :
      setLineEdit                   (
        item,column                 ,
        SIGNAL(editingFinished())   ,
        SLOT  (enumChanged    ()) ) ;
    break                           ;
  }                                 ;
}

void N::ObjectTypeEditor::stateChanged (QTreeWidgetItem * item,int column)
{
  if (column!=0) return                             ;
  bool checked = (item->checkState(0)==Qt::Checked) ;
  SUID uuid    = nTreeUuid ( item , 0 )             ;
  int  flags   =  0                                 ;
  if (checked) flags = 1                            ;
  EnterSQL ( SC , plan->sql )                       ;
    QString Q                                       ;
    Q = SC.sql.Update(PlanTable(Types)              ,
           QString("where type = :TYPE"),1,"flags") ;
    SC . Prepare ( Q               )                ;
    SC . Bind    ( "type"  , uuid  )                ;
    SC . Bind    ( "flags" , flags )                ;
    SC . Exec    (                 )                ;
  LeaveSQL ( SC , plan->sql )                       ;
  Alert ( Done )                                    ;
  if (checked)                                      {
    plan->Talk(15,tr("Enable object type" ))        ;
  } else                                            {
    plan->Talk(15,tr("Disable object type"))        ;
  }                                                 ;
}

void N::ObjectTypeEditor::removeOldItem(void)
{
  nDropOut ( IsNull(ItemEditing) )          ;
  removeItemWidget (ItemEditing,ItemColumn) ;
  ItemEditing = NULL                        ;
  ItemWidget  = NULL                        ;
  ItemColumn  = -1                          ;
  if (!plan->Booleans["Desktop"])           {
    allowGesture = true                     ;
  }                                         ;
}

void N::ObjectTypeEditor::nameChanged(void)
{
  nDropOut ( IsNull(ItemEditing) )                     ;
  nDropOut ( IsNull(ItemWidget ) )                     ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)     ;
  SUID        uuid = nTreeUuid(ItemEditing,0)          ;
  if (NotNull(line))                                   {
    QString name = line->text()                        ;
    EnterSQL ( SC , plan->sql )                        ;
      QString Q                                        ;
      Q = SC.sql.SelectFrom("id",PlanTable(Names)      ,
            QString("where uuid = %1 and language = %2")
            .arg(uuid                                  )
            .arg(vLanguageId)                        ) ;
      if (SC.Fetch(Q))                                 {
        Q = SC.NameUpdate(PlanTable(Names),"name")     ;
      } else                                           {
        Q = SC.NameSyntax(PlanTable(Names))            ;
      }                                                ;
      if (SC.insertName(Q,uuid,vLanguageId,name))      {
        ItemEditing -> setText ( 1 , name )            ;
        Alert ( Done  )                                ;
      } else                                           {
        Alert ( Error )                                ;
      }                                                ;
    LeaveSQL ( SC , plan->sql )                        ;
  }                                                    ;
  removeOldItem ( )                                    ;
}

void N::ObjectTypeEditor::enumChanged(void)
{
  nDropOut ( IsNull(ItemEditing) )                    ;
  nDropOut ( IsNull(ItemWidget ) )                    ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)    ;
  SUID        uuid = nTreeUuid(ItemEditing,0)         ;
  if (NotNull(line))                                  {
    QString name = line->text()                       ;
    EnterSQL ( SC , plan->sql )                       ;
      QString Q                                       ;
      Q = SC.sql.DeleteFrom(PlanTable(Variables)      ,
            QString("where uuid = %1 "
                    "and name = `Enumeration`"        )
            .arg(uuid)                              ) ;
      SC . Query ( Q )                                ;
      Q = SC.sql.InsertInto(PlanTable(Variables)      ,
                             2,"uuid","name")         ;
      SC . Prepare ( Q                       )        ;
      SC . Bind    ( "uuid"  , uuid          )        ;
      SC . Bind    ( "name"  , "Enumeration" )        ;
      SC . Exec    (                         )        ;
      Q = SC.sql.Update(PlanTable(Variables)          ,
                "where uuid = :UUID and name = :NAME" ,
                1 , "value"                         ) ;
      SC . Prepare ( Q                       )        ;
      SC . Bind    ( "uuid"  , uuid          )        ;
      SC . Bind    ( "name"  , "Enumeration" )        ;
      SC . Bind    ( "value" , name          )        ;
      SC . Exec    (                         )        ;
      ItemEditing -> setText ( ItemColumn , name )    ;
    LeaveSQL ( SC , plan->sql )                       ;
    Alert ( Done )                                    ;
  }                                                   ;
  removeOldItem ( )                                   ;
}

bool N::ObjectTypeEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  AssignAction      ( Refresh      , tr("Refresh types") ) ;
  LinkAction        ( Refresh      , List            ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  return true                                              ;
}

bool N::ObjectTypeEditor::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                 ;
  QAction    * aa                           ;
  QMenu      * ms                           ;
  QMenu      * ma                           ;
  QMenu      * me                           ;
  mm.add(101,tr("Refresh"                )) ;
  mm.add(102,tr("Statistics"             )) ;
  mm.addSeparator()                         ;
  mm.add(103,tr("Export C++ enumeration" )) ;
  mm.add(104,tr("Display C++ enumeration")) ;
  mm.addSeparator()                         ;
  mm.add(105,tr("Export names to file"   )) ;
  mm.addSeparator()                         ;
  ms = mm . addMenu ( tr ("Sorting"     ) ) ;
  ma = mm . addMenu ( tr ("Adjustments" ) ) ;
  me = mm . addMenu ( tr ("Selection"   ) ) ;
  SortingMenu    ( mm , ms )                ;
  AdjustMenu     ( mm , ma )                ;
  SelectionsMenu ( mm , me )                ;
  mm.setFont(plan)                          ;
  if (plan->Booleans["Desktop"])            {
    pos = QCursor::pos()                    ;
  } else                                    {
    pos = mapToGlobal ( pos )               ;
  }                                         ;
  aa = mm.exec(pos)                         ;
  if (!plan->Booleans["Desktop"])           {
    allowGesture = true                     ;
  }                                         ;
  if (IsNull       (   aa)) return true     ;
  if (RunSorting   (mm,aa)) return true     ;
  if (RunAdjustment(mm,aa)) return true     ;
  if (RunSelections(mm,aa)) return true     ;
  QSize s                                   ;
  switch (mm[aa])                           {
    case 101                                :
      List ( )                              ;
    break                                   ;
    case 102                                :
      setColumnHidden(3,false)              ;
      nFullLoop(i,columnCount())            {
        resizeColumnToContents ( i )        ;
      }                                     ;
      nFullLoop(i,topLevelItemCount())      {
        QTreeWidgetItem * item              ;
        item  = topLevelItem ( i      )     ;
        item -> setText      ( 3 , "" )     ;
      }                                     ;
      DoProcessEvents                       ;
      update  (   )                         ;
      s = SuitableSize()                    ;
      resize  ( s )                         ;
      DoProcessEvents                       ;
      repaint (   )                         ;
      emit Adjustment ( this , s )          ;
      DoProcessEvents                       ;
      start   ( 1002 )                      ;
    break                                   ;
    case 103                                :
      ExportCpp   ( )                       ;
    break                                   ;
    case 104                                :
      DisplayCpp  ( )                       ;
    break                                   ;
    case 105                                :
      ExportNames ( )                       ;
    break                                   ;
  }                                         ;
  return true                               ;
}

void N::ObjectTypeEditor::AllItems(void)
{
  plan->Talk(tr("Counting object items, please wait for a moment.")) ;
  emit OnBusy ( )                                                    ;
  QMap<SUID,QTreeWidgetItem *> ITEMs                                 ;
  QTreeWidgetItem * EndItem = NULL                                   ;
  for (int i=0;i<topLevelItemCount();i++)                            {
    QTreeWidgetItem * item  = topLevelItem ( i        )              ;
    SUID              uuid  = nTreeUuid    ( item , 0 )              ;
    ITEMs            [uuid] = item                                   ;
    if (IsNull(EndItem))                                             {
      if (item->checkState(0)==Qt::Unchecked) EndItem = item         ;
    }                                                                ;
  }                                                                  ;
  int Total = 0                                                      ;
  SqlConnection SC(plan->sql)                                        ;
  if (SC.open("nObjectTypeEditor","run"))                            {
    QString Q                                                        ;
    Q = SC.sql.SelectFrom("type,count(*)",PlanTable(MajorUuid)       ,
                          "group by type order by type asc"        ) ;
    SqlLoopNow ( SC , Q )                                            ;
      SUID type  = SC . Uuid ( 0 )                                   ;
      int  count = SC . Int  ( 1 )                                   ;
      Total += count                                                 ;
      if (ITEMs.contains(type))                                      {
        ITEMs[type]->setTextAlignment(3,Qt::AlignRight               |
                                        Qt::AlignVCenter           ) ;
        ITEMs[type]->setText(3,QString::number(count))               ;
      }                                                              ;
    SqlLoopErr ( SC , Q )                                            ;
    SqlLoopEnd ( SC , Q )                                            ;
    SC.close()                                                       ;
  }                                                                  ;
  SC.remove()                                                        ;
  if (NotNull(EndItem))                                              {
    EndItem->setText(3,QString::number(Total))                       ;
  }                                                                  ;
  emit GoRelax ( )                                                   ;
  Alert ( Done )                                                     ;
}

void N::ObjectTypeEditor::DisplayCpp(void)
{
  QByteArray Data                    ;
  ExportCpp ( Data , "ObjectTypes" ) ;
  if (Data.size()<=0) return         ;
  emit CppEnumeration ( Data )       ;
}

void N::ObjectTypeEditor::ExportCpp(void)
{
  QString filename = QFileDialog::getSaveFileName (
     this                                         ,
     tr("Export C++ enumeration")                 ,
     plan->Temporary("")                          ,
     tr("C++ (*.h *.c *.hpp *.cpp *.cc *.c++)") ) ;
  if (filename.length()<=0) return                ;
  ExportCpp(filename,"ObjectTypes")               ;
}

void N::ObjectTypeEditor::ExportCpp(QString filename,QString EnumName)
{
  QByteArray Data                  ;
  ExportCpp ( Data , EnumName )    ;
  if (Data.size()<=0) return       ;
  File::toFile ( filename , Data ) ;
}

void N::ObjectTypeEditor::ExportCpp(QByteArray & Data,QString EnumName)
{
  QStringList lists                              ;
  QStringList enums                              ;
  QString     Enum                               ;
  ZMAPs       IDs                                ;
  int         MaxEnum = 0                        ;
  int         MaxLine = 0                        ;
  int         Count   = 0                        ;
  QString     Space ( " " )                      ;
  QString     Line                               ;
  Data . clear ( )                               ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * item                       ;
    item = topLevelItem(i)                       ;
    if (IsMask(item->checkState(0),Qt::Checked)) {
      Enum         = item->text(2)               ;
      enums       << Enum                        ;
      IDs [ Enum ] = nTreeUuid(item,0)           ;
      if (Enum.length()>MaxEnum)                 {
        MaxEnum = Enum.length()                  ;
      }                                          ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  MaxLine = MaxEnum + 9                          ;
  Count   = 5 + EnumName.length()                ;
  Line    = "enum "                              ;
  Line   += EnumName                             ;
  Line   += Space.repeated(MaxLine-Count)        ;
  Line   += "{"                                  ;
  lists << Line                                  ;
  ////////////////////////////////////////////////
  int TE = 0                                     ;
  for (int i=0;i<enums.count();i++)              {
    int     Value                                ;
    QString V                                    ;
    Enum    = enums [   i  ]                     ;
    Value   = IDs   [ Enum ]                     ;
    V       = QString::number(Value)             ;
    Line    = "  "                               ;
    Line   += Enum                               ;
    if (MaxEnum>Enum.length())                   {
      int L = Enum.length()                      ;
      Line += Space.repeated(MaxEnum-L)          ;
    }                                            ;
    Line   += " = "                              ;
    if (V.length()<3)                            {
      int L = V.length()                         ;
      Line += Space.repeated(3-L)                ;
    }                                            ;
    Line   += V                                  ;
    Line   += " ,"                               ;
    lists  << Line                               ;
    if (Value>=TE) TE = Value + 1                ;
  }                                              ;
  ////////////////////////////////////////////////
  Line    = "  TypesEnd"                         ;
  Line   += Space.repeated(MaxEnum-8)            ;
  Line   += " = "                                ;
  Line   += QString::number(TE)                  ;
  lists  << Line                                 ;
  ////////////////////////////////////////////////
  Line    = "}"                                  ;
  Line   += Space.repeated(MaxLine-1)            ;
  Line   += ";"                                  ;
  lists  << Line                                 ;
  ////////////////////////////////////////////////
  Enum = lists . join ("\r\n")                   ;
  Data . append ( Enum.toUtf8() )                ;
}

void N::ObjectTypeEditor::ExportNames(void)
{
  QString filename = QFileDialog::getSaveFileName     (
                       this                           ,
                       tr("Export names into file")   ,
                       plan->Temporary("")            ,
                       tr("Plain text file(*.txt)") ) ;
  nDropOut ( filename.length() <= 0 )                 ;
  /////////////////////////////////////////////////////
  QStringList L                                       ;
  for (int i=0;i<topLevelItemCount();i++)             {
    QTreeWidgetItem * it = topLevelItem(i)            ;
    L << it->text(1)                                  ;
  }                                                   ;
  nDropOut ( L.count() <= 0 )                         ;
  /////////////////////////////////////////////////////
  QString    M = L.join("\r\n")                       ;
  QByteArray B = M.toUtf8()                           ;
  QFile      F(filename)                              ;
  if (F.open(QIODevice::WriteOnly))                   {
    F . write ( B     )                               ;
    F . close (       )                               ;
    Alert     ( Done  )                               ;
  } else                                              {
    Alert     ( Error )                               ;
  }                                                   ;
}

void N::ObjectTypeEditor::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch (Type)     {
    case 1001       :
      AllTypes ( )  ;
    break           ;
    case 1002       :
      AllItems ( )  ;
    break           ;
  }                 ;
}
