#include <datawidgets.h>

N::VariableLists:: VariableLists ( QWidget * parent , Plan * p )
                 : TreeDock      (           parent ,        p )
                 , Object        ( 0 , Types::None             )
                 , Relation      ( 0 , Groups::Using           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::VariableLists::~VariableLists(void)
{
}

QMimeData * N::VariableLists::dragMime (void)
{
  QMimeData * mime = standardMime("variable") ;
  nKickOut ( IsNull(mime) , NULL )            ;
  return mime                                 ;
}

bool N::VariableLists::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0) ;
  return ( Uuids.count() > 0 )   ;
}

bool N::VariableLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::VariableLists::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::VariableLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::VariableLists::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::VariableLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::VariableLists::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::VariableLists::Configure(void)
{
  NewTreeWidgetItem            ( head                  )                     ;
  head -> setText              ( 0 , tr("Name"      )  )                     ;
  head -> setText              ( 1 , tr("Identifier")  )                     ;
  head -> setText              ( 2 , tr("Type"      )  )                     ;
  head -> setText              ( 3 , tr("Value"     )  )                     ;
  head -> setText              ( 4 , ""                )                     ;
  for (int i=0;i<5;i++)                                                      {
    head -> setTextAlignment   ( i , Qt::AlignCenter   )                     ;
  }                                                                          ;
  setWindowTitle               ( tr("Variables")       )                     ;
  setDragDropMode              ( NoDragDrop            )                     ;
  setRootIsDecorated           ( false                 )                     ;
  setAlternatingRowColors      ( true                  )                     ;
  setSelectionMode             ( ExtendedSelection     )                     ;
  setColumnCount               ( 5                     )                     ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded )                     ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded )                     ;
  setHeaderItem                ( head                  )                     ;
  plan -> setFont              ( this                  )                     ;
  ////////////////////////////////////////////////////////////////////////////
  setAlignment ( 3 , Qt::AlignRight | Qt::AlignVCenter )                     ;
  ////////////////////////////////////////////////////////////////////////////
  ValueTypes[Cpp::Undecided ] = tr("Undecided"                   ) ;
  ValueTypes[Cpp::Char      ] = tr("char (1 byte)"               ) ;
  ValueTypes[Cpp::Byte      ] = tr("unsigned char (1 byte)"      ) ;
  ValueTypes[Cpp::Short     ] = tr("short (2 bytes)"             ) ;
  ValueTypes[Cpp::UShort    ] = tr("unsigned short (2 bytes)"    ) ;
  ValueTypes[Cpp::Integer   ] = tr("int (4 bytes)"               ) ;
  ValueTypes[Cpp::UInt      ] = tr("unsigned int (4 bytes)"      ) ;
  ValueTypes[Cpp::LongLong  ] = tr("long long (8 bytes)"         ) ;
  ValueTypes[Cpp::ULongLong ] = tr("unsigned long long (8 bytes)") ;
  ValueTypes[Cpp::Float     ] = tr("float (4 bytes)"             ) ;
  ValueTypes[Cpp::Double    ] = tr("double (8 bytes)"            ) ;
  ValueTypes[Cpp::LDouble   ] = tr("long double (16 bytes)"      ) ;
  ////////////////////////////////////////////////////////////////////////////
  MountClicked ( 2 )                                                         ;
}

bool N::VariableLists::FocusIn(void)
{
  LinkAction   ( Refresh , startup         () ) ;
  LinkAction   ( Copy    , CopyToClipboard () ) ;
  LinkAction   ( Insert  , Insert          () ) ;
  if (uuid>0 && type>0 && linkType>0) {
    LinkAction ( Delete  , Delete          () ) ;
  }                                             ;
  return true                                   ;
}

void N::VariableLists::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::VariableLists::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false ) ;
  return true                             ;
}

bool N::VariableLists::startup(void)
{
  if (uuid<=0) setDragDropMode ( DragOnly )         ;
          else setDragDropMode ( DragDrop )         ;
  clear    (                )                       ;
  EnterSQL ( SC , plan->sql )                       ;
    QString Q                                       ;
    UUIDs Uuids                                     ;
    SUID  uu                                        ;
    int   language = vLanguageId                    ;
    if (uuid<=0)                                    {
      Uuids = SC.Uuids                              (
           PlanTable(MajorUuid) , "uuid"            ,
           QString("where type = %1 order by id asc")
                  .arg(Types::Variable) ) ;
    } else                                          {
      QString title = SC.getName                    (
                       PlanTable(Names)             ,
                       "uuid"                       ,
                       vLanguageId             ,
                       uuid                       ) ;
      setWindowTitle(title)                         ;
      GroupItems GI(plan)                           ;
      Uuids = GI.Subordination                      (
                SC                                  ,
                uuid                                ,
                type                                ,
                Types::Variable                     ,
                linkType                            ,
                "order by id position asc"        ) ;
    }                                               ;
    foreach (uu,Uuids)                              {
      QString name = SC.getName                     (
                     PlanTable(Names)               ,
                     "uuid"                         ,
                     language                       ,
                     uu                           ) ;
      Q = SC.sql.SelectFrom                         (
            "type,name,value"                       ,
            PlanTable(Variables)                    ,
            SC.WhereUuid(uu)                      ) ;
      if (SC.Fetch(Q))                              {
        int        t     = SC . Int       ( 0 )     ;
        QString    ident = SC . String    ( 1 )     ;
        QByteArray V     = SC . ByteArray ( 2 )     ;
        NewTreeWidgetItem ( item )                  ;
        item->setData   ( 0 , Qt::UserRole,uu   )   ;
        item->setData   ( 2 , Qt::UserRole,t    )   ;
        item->setText   ( 0 , name              )   ;
        item->setText   ( 1 , ident             )   ;
        item->setText   ( 2 , ValueTypes[t]     )   ;
        setValue        ( t , V , item          )   ;
        setAlignments   ( item                  )   ;
        addTopLevelItem ( item                  )   ;
      }                                             ;
    }                                               ;
  LeaveSQL ( SC , plan->sql )                       ;
  SuitableColumns (         )                       ;
  Alert    ( Done           )                       ;
  return true                                       ;
}

bool N::VariableLists::Menu(QPoint pos)
{
  nScopedMenu(mm,this)                                          ;
  QMdiSubWindow   * mdi  = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget     * dock = Casting ( QDockWidget   , parent() ) ;
  QAction    * aa                                               ;
  QMenu      * im                                               ;
  mm.add(101,tr("Refresh"))                                     ;
  mm.add(201,tr("Insert" ))                                     ;
  if (uuid>0 && type>0 && linkType>0)                           {
    mm.add(202,tr("Delete" ))                                   ;
  }                                                             ;
  im  = mm .addMenu(tr("Items"))                                ;
  aa  = mm . add ( im , 601 , tr("Identifier") )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(1) )                     ;
  aa  = mm . add ( im , 602 , tr("Type"      ) )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(2) )                     ;
  aa  = mm . add ( im , 603 , tr("Value"     ) )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(3) )                     ;
  mm.addSeparator()                                             ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 101                                                    :
      startup ()                                                ;
    break                                                       ;
    case 201                                                    :
      Insert  ()                                                ;
    break                                                       ;
    case 202                                                    :
      Delete  ()                                                ;
    break                                                       ;
    case 601                                                    :
      setColumnHidden ( 1 , !aa->isChecked() )                  ;
    break                                                       ;
    case 602                                                    :
      setColumnHidden ( 2 , !aa->isChecked() )                  ;
    break                                                       ;
    case 603                                                    :
      setColumnHidden ( 3 , !aa->isChecked() )                  ;
    break                                                       ;
    case 1000001                                                :
      emit attachMdi  ( this , 0             )                  ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::BottomDockWidgetArea                                ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

QByteArray N::VariableLists::setValue(int t,QString text)
{
  QByteArray E                          ;
  switch (t)                            {
    case Cpp::Char                      :
      if (text.length()>0)              {
        bool ok = false                 ;
        int  v  = text.toInt(&ok)       ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::Byte            :
      if (text.length()>0)              {
        bool ok = false                 ;
        int  v  = text.toInt(&ok)       ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::Short           :
      if (text.length()>0)              {
        bool ok = false                 ;
        int  v  = text.toInt(&ok)       ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::UShort          :
      if (text.length()>0)              {
        bool ok = false                 ;
        int  v  = text.toInt(&ok)       ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::Integer         :
      if (text.length()>0)              {
        bool ok = false                 ;
        int  v  = text.toInt(&ok)       ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::UInt            :
      if (text.length()>0)              {
        bool ok = false                 ;
        TUID v  = text.toLongLong(&ok)  ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::LongLong        :
      if (text.length()>0)              {
        bool ok = false                 ;
        TUID v  = text.toLongLong(&ok)  ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::ULongLong       :
      if (text.length()>0)              {
        bool ok = false                 ;
        SUID v  = text.toULongLong(&ok) ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::Float           :
      if (text.length()>0)              {
        bool  ok = false                ;
        float v  = text.toFloat(&ok)    ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::Double          :
      if (text.length()>0)              {
        bool   ok = false               ;
        double v  = text.toDouble(&ok)  ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
    case Cpp::LDouble         :
      if (text.length()>0)              {
        bool   ok = false               ;
        double v  = text.toDouble(&ok)  ;
        if (ok) E = text.toUtf8()       ;
      }                                 ;
    break                               ;
  }                                     ;
  return E                              ;
}

bool N::VariableLists::setValue(int type,QByteArray & Value,QTreeWidgetItem * item)
{
  switch (type)                                 {
    case Cpp::Char                    :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::Byte                    :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::Short                   :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::UShort                  :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::Integer                 :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::UInt                    :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::LongLong                :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::ULongLong               :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::Float                   :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::Double                  :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
    case Cpp::LDouble                 :
      item->setText(3,QString::fromUtf8(Value)) ;
    break                                       ;
  }                                             ;
  return true                                   ;
}

void N::VariableLists::Insert(void)
{
  NewTreeWidgetItem(item)         ;
  item->setData(0,Qt::UserRole,0) ;
  item->setData(2,Qt::UserRole,0) ;
  setAlignments   ( item     )    ;
  addTopLevelItem ( item     )    ;
  scrollToItem    ( item     )    ;
  doubleClicked   ( item , 0 )    ;
}

void N::VariableLists::Delete(void)
{
}

void N::VariableLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  QComboBox * cc                           ;
  SUID        uu = nTreeUuid(item,0)       ;
  int         t  = nTreeInt (item,2)       ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 1                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (idFinished     ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 2                                 :
      cc   = setComboBox                   (
               item,column                 ,
               SIGNAL(activated   (int))   ,
               SLOT  (typeFinished(int)) ) ;
      N::AddItems (*cc,ValueTypes)         ;
      if (t>=0) (*cc) <= t                 ;
      cc->setMaxVisibleItems(30)           ;
      cc->setEditable(true)                ;
      cc->showPopup()                      ;
    break                                  ;
    case 3                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (valueFinished  ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
  }                                        ;
}

void N::VariableLists::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                        ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)       ;
  if (IsNull(line)) return                               ;
  SUID    uu   = nTreeUuid(ItemEditing,0)                ;
  QString name = line->text()                            ;
  GroupItems GI  ( plan           )                      ;
  EnterSQL       ( SC , plan->sql )                      ;
    QString Q                                            ;
    if (uuid<=0)                                         {
      uuid = SC.Unique(PlanTable(MajorUuid),"uuid",7884) ;
      SC.assureUuid(PlanTable(MajorUuid)                 ,
                    uu                                   ,
                    Types::Variable                    ) ;
      SC.assureUuid(PlanTable(Variables),uu,0)           ;
      if (uuid>0 && type>0 && linkType>0)                {
        UUIDs Uuids                                      ;
        Uuids << uu                                      ;
        GI.Join                                          (
          SC                                             ,
          uuid                                           ,
          type                                           ,
          Types::Variable                                ,
          linkType                                       ,
          0                                              ,
          Uuids                                        ) ;
      }                                                  ;
      ItemEditing->setData(0,Qt::UserRole,uu)            ;
    }                                                    ;
    SC.assureName(PlanTable(Names)                       ,
                  uu                                     ,
                  vLanguageId                            ,
                  name                                 ) ;
    ItemEditing->setText(0,name)                         ;
  LeaveSQL         ( SC , plan->sql )                    ;
  removeOldItem    ( true , 0       )                    ;
  Alert            ( Done           )                    ;
}

void N::VariableLists::idFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    uu   = nTreeUuid(ItemEditing,0)          ;
  QString name = line->text()                      ;
  EnterSQL       ( SC , plan->sql         )        ;
    QString Q                                      ;
    Q = SC.sql.Update( PlanTable(Variables)        ,
             "where uuid = :UUID",1,"name")        ;
    SC . Prepare ( Q                      )        ;
    SC . Bind    ( "uuid" , uu            )        ;
    SC . Bind    ( "name" , name.toUtf8() )        ;
    SC . Exec    (                        )        ;
    ItemEditing->setText(1,name)                   ;
  LeaveSQL       ( SC , plan->sql         )        ;
  removeOldItem  ( true , 0               )        ;
  Alert          ( Done                   )        ;
}

void N::VariableLists::typeFinished(int)
{
  if (IsNull(ItemEditing)) return                ;
  QComboBox * cc = Casting(QComboBox,ItemWidget) ;
  if (IsNull(cc)) return                         ;
  SUID uu   = nTreeUuid(ItemEditing,0)           ;
  SUID tuid = N::GetUuid(cc)                     ;
  EnterSQL       ( SC , plan->sql )              ;
    QString Q                                    ;
    int t = (int)tuid                            ;
    ItemEditing->setData(2,Qt::UserRole,t)       ;
    ItemEditing->setText(2,cc->currentText())    ;
    Q = SC.sql.Update(PlanTable(Variables)       ,
                "where uuid = :UUID",1,"type")   ;
    SC . Prepare ( Q             )               ;
    SC . Bind    ( "uuid" , uu   )               ;
    SC . Bind    ( "type" , t    )               ;
    SC . Exec    (               )               ;
  LeaveSQL       ( SC , plan->sql )              ;
  removeOldItem  ( true , 0       )              ;
  Alert          ( Done           )              ;
}

void N::VariableLists::valueFinished(void)
{
  if (IsNull(ItemEditing)) return                  ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget) ;
  if (IsNull(line)) return                         ;
  SUID    uu   = nTreeUuid(ItemEditing,0)          ;
  QString name = line->text()                      ;
  EnterSQL       ( SC , plan->sql          )       ;
    QString    Q                                   ;
    QByteArray V                                   ;
    int        t = nTreeInt(ItemEditing,2)         ;
    V = setValue(t,name)                           ;
    Q = SC.sql.Update(PlanTable( Variables )       ,
             "where uuid = :UUID",1,"value")       ;
    SC . Prepare ( Q                       )       ;
    SC . Bind    ( "uuid"  , uu            )       ;
    SC . Bind    ( "value" , name.toUtf8() )       ;
    SC . Exec    (                         )       ;
    ItemEditing->setText(3,QString::fromUtf8(V))   ;
  LeaveSQL       ( SC , plan->sql          )       ;
  removeOldItem  ( true , 0                )       ;
  Alert          ( Done                    )       ;
}
