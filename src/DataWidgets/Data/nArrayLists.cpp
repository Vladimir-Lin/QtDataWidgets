#include <datawidgets.h>

N::ArrayLists:: ArrayLists ( QWidget * parent , Plan * p )
              : TreeDock   (           parent ,        p )
              , Ownership  ( 0                           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::ArrayLists::~ArrayLists(void)
{
}

QMimeData * N::ArrayLists::dragMime (void)
{
  QMimeData * mime = standardMime("blob") ;
  nKickOut ( IsNull(mime) , NULL )        ;
  return mime                             ;
}

bool N::ArrayLists::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0) ;
  return ( Uuids.count() > 0 )   ;
}

bool N::ArrayLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::ArrayLists::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::ArrayLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::ArrayLists::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::ArrayLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut           ( nEqual ( source , this ) , false ) ;
  return dropHandler ( mime                             ) ;
}

bool N::ArrayLists::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::ArrayLists::Configure(void)
{
  NewTreeWidgetItem            ( head                                      ) ;
  head -> setText              ( 0 , tr("Name"      )                      ) ;
  head -> setText              ( 1 , tr("Identifier")                      ) ;
  head -> setText              ( 2 , tr("Type"      )                      ) ;
  head -> setText              ( 3 , tr("Size"      )                      ) ;
  head -> setText              ( 4 , tr("Checksum"  )                      ) ;
  head -> setText              ( 5 , ""                                    ) ;
  for                          (int i = 0 ; i < 6 ; i++                    ) {
    head -> setTextAlignment   ( i , Qt::AlignCenter                       ) ;
  }                                                                          ;
  setWindowTitle               ( tr("Data blobs")                          ) ;
  setDragDropMode              ( DragDrop                                  ) ;
  setRootIsDecorated           ( false                                     ) ;
  setAlternatingRowColors      ( true                                      ) ;
  setSelectionMode             ( ExtendedSelection                         ) ;
  setColumnCount               ( 6                                         ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded                     ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded                     ) ;
  setHeaderItem                ( head                                      ) ;
  MountClicked                 ( 2                                         ) ;
  plan -> setFont              ( this                                      ) ;
  ////////////////////////////////////////////////////////////////////////////
  setAlignment                 ( 3 , Qt::AlignRight | Qt::AlignVCenter     ) ;
  setAlignment                 ( 4 , Qt::AlignRight | Qt::AlignVCenter     ) ;
  ////////////////////////////////////////////////////////////////////////////
  ArrayTypes [ Cpp::Undecided  ] = tr ( "Undecided"                        ) ;
  ArrayTypes [ Cpp::Char       ] = tr ( "char (1 byte)"                    ) ;
  ArrayTypes [ Cpp::Byte       ] = tr ( "unsigned char (1 byte)"           ) ;
  ArrayTypes [ Cpp::Short      ] = tr ( "short (2 bytes)"                  ) ;
  ArrayTypes [ Cpp::UShort     ] = tr ( "unsigned short (2 bytes)"         ) ;
  ArrayTypes [ Cpp::Integer    ] = tr ( "int (4 bytes)"                    ) ;
  ArrayTypes [ Cpp::UInt       ] = tr ( "unsigned int (4 bytes)"           ) ;
  ArrayTypes [ Cpp::LongLong   ] = tr ( "long long (8 bytes)"              ) ;
  ArrayTypes [ Cpp::ULongLong  ] = tr ( "unsigned long long (8 bytes)"     ) ;
  ArrayTypes [ Cpp::Float      ] = tr ( "float (4 bytes)"                  ) ;
  ArrayTypes [ Cpp::Double     ] = tr ( "double (8 bytes)"                 ) ;
  ArrayTypes [ Cpp::LDouble    ] = tr ( "long double (16 bytes)"           ) ;
  ArrayTypes [ Cpp::Blob       ] = tr ( "Pure blob data (dynamic size)"    ) ;
  ArrayTypes [ Cpp::StringBlob ] = tr ( "QString (dynamic size)"           ) ;
  ArrayTypes [ Cpp::Ann        ] = tr ( "Artificial Neural Network"        ) ;
}

bool N::ArrayLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  LinkAction        ( Import       , Import          ( ) ) ;
  LinkAction        ( Export       , ExportCpp       ( ) ) ;
  LinkAction        ( Save         , Export          ( ) ) ;
  return true                                              ;
}

void N::ArrayLists::run(int T,ThreadData * d)
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

bool N::ArrayLists::Load(ThreadData * d)
{
  nKickOut                ( ! IsContinue ( d ) , false    ) ;
  WaitClear               (                               ) ;
  ///////////////////////////////////////////////////////////
  EnterSQL                ( SC , plan -> sql              ) ;
    QString Q                                               ;
    UUIDs Uuids                                             ;
    SUID  uu                                                ;
    int   language = vLanguageId                            ;
    if ( isStandby ( ) )                                    {
      Uuids = SC . Uuids                                    (
                PlanTable(Arrays)                           ,
                "uuid"                                      ,
                SC . OrderByAsc ( "id" )                  ) ;
    } else                                                  {
      GroupItems GI ( plan )                                ;
      Uuids = GI . Subordination                            (
                SC                                          ,
                ObjectUuid ( )                              ,
                ObjectType ( )                              ,
                Types::Array                                ,
                Connexion ( )                               ,
                SC . OrderByAsc ( "position" )            ) ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    foreach ( uu , Uuids )                                  {
      QString name = SC . getName                           (
                       PlanTable(Names)                     ,
                       "uuid"                               ,
                       language                             ,
                       uu                                 ) ;
      Q = SC . sql . SelectFrom                             (
            "type,size,checksum,name"                       ,
            PlanTable(Arrays)                               ,
            SC . WhereUuid ( uu )                         ) ;
      if ( SC . Fetch ( Q ) )                               {
        int     t         = SC . Int    ( 0 )               ;
        int     size      = SC . Int    ( 1 )               ;
        int     checksum  = SC . Int    ( 2 )               ;
        QString ident     = SC . String ( 3 )               ;
        NewTreeWidgetItem ( item                          ) ;
        item -> setData   ( 0 , Qt::UserRole,uu           ) ;
        item -> setData   ( 2 , Qt::UserRole,t            ) ;
        item -> setText   ( 0 , name                      ) ;
        item -> setText   ( 1 , ident                     ) ;
        item -> setText   ( 2 , ArrayTypes [ t ]          ) ;
        item -> setText   ( 3 , QString::number(size    ) ) ;
        item -> setText   ( 4 , QString::number(checksum) ) ;
        setAlignments     ( item                          ) ;
        addTopLevelItem   ( item                          ) ;
      }                                                     ;
    }                                                       ;
  LeaveSQL                ( SC , plan -> sql              ) ;
  ///////////////////////////////////////////////////////////
  emit AutoFit            (                               ) ;
  Alert                   ( Done                          ) ;
  return true                                               ;
}

bool N::ArrayLists::startup(void)
{
  if ( isStandby ( ) ) setDragDropMode ( DragOnly ) ;
                  else setDragDropMode ( DragDrop ) ;
  start                                ( 10001    ) ;
  return true                                       ;
}

void N::ArrayLists::Insert(void)
{
  NewTreeWidgetItem ( item                    ) ;
  item -> setData   ( 0    , Qt::UserRole , 0 ) ;
  item -> setData   ( 2    , Qt::UserRole , 0 ) ;
  setAlignments     ( item                    ) ;
  addTopLevelItem   ( item                    ) ;
  scrollToItem      ( item                    ) ;
  doubleClicked     ( item , 0                ) ;
}

void N::ArrayLists::Import(void)
{
  QString filename = QFileDialog::getOpenFileName                 (
                       this                                       ,
                       tr("Import data source")                   ,
                       plan->Temporary("")                        ,
                       tr("*.*")                                ) ;
  nDropOut     ( filename . length ( ) <= 0                     ) ;
  QByteArray Body                                                 ;
  if ( ! File::toByteArray ( filename , Body )                  ) {
    Alert ( Error )                                               ;
    return                                                        ;
  }                                                               ;
  int  checksum = qChecksum ( Body . data ( ) , Body . size ( ) ) ;
  SUID uu       = 0                                               ;
  int  t        = Cpp::Undecided                                  ;
  int  s        = Body . size ( )                                 ;
  EnterSQL       ( SC , plan -> sql                             ) ;
    QString Q                                                     ;
    uu = SC . Unique                                              (
           PlanTable(MajorUuid)                                   ,
           "uuid"                                                 ,
           3492                                                 ) ;
    SC . assureUuid                                               (
           PlanTable(MajorUuid)                                   ,
           uu                                                     ,
           Types::Array                                         ) ;
    Q = SC . sql . InsertInto                                     (
          PlanTable(Arrays)                                       ,
          5                                                       ,
          "uuid"                                                  ,
          "type"                                                  ,
          "size"                                                  ,
          "checksum"                                              ,
          "value"                                               ) ;
    SC . Prepare    ( Q                                         ) ;
    SC . Bind       ( "uuid"     , uu                           ) ;
    SC . Bind       ( "type"     , t                            ) ;
    SC . Bind       ( "size"     , s                            ) ;
    SC . Bind       ( "checksum" , checksum                     ) ;
    SC . Bind       ( "value"    , Body                         ) ;
    SC . Exec       (                                           ) ;
  LeaveSQL          ( SC , plan -> sql                          ) ;
  if                ( uu <= 0                                   ) {
    Alert           ( Error                                     ) ;
    return                                                        ;
  }                                                               ;
  Alert             ( Done                                      ) ;
  NewTreeWidgetItem ( item                                      ) ;
  item -> setData   ( 0 , Qt::UserRole , uu                     ) ;
  item -> setText   ( 2 , ArrayTypes [ t ]                      ) ;
  item -> setText   ( 3 , QString::number(s       )             ) ;
  item -> setText   ( 4 , QString::number(checksum)             ) ;
  setAlignments     ( item                                      ) ;
  addTopLevelItem   ( item                                      ) ;
  scrollToItem      ( item                                      ) ;
  doubleClicked     ( item , 0                                  ) ;
}

void N::ArrayLists::Replace(void)
{
  QTreeWidgetItem * item              ;
  item = currentItem (              ) ;
  nDropOut           ( IsNull(item) ) ;
  Replace            ( item         ) ;
}

void N::ArrayLists::Replace(QTreeWidgetItem * item)
{
  nDropOut            ( IsNull ( item )            ) ;
  SUID uu = nTreeUuid ( item , 0                   ) ;
  nDropOut            ( uu <= 0                    ) ;
  QString filename = QFileDialog::getOpenFileName    (
                       this                          ,
                       tr("Import replacement data") ,
                       plan -> Temporary ( "" )      ,
                       tr("*.*")                   ) ;
  nDropOut            ( filename . length ( ) <= 0 ) ;
  QByteArray Body                                    ;
  if ( !File::toByteArray(filename,Body) ) {
    Alert ( Error )                                  ;
    return                                           ;
  }                                                  ;
  int  checksum = qChecksum(Body.data(),Body.size()) ;
  int  s        = Body.size()                        ;
  EnterSQL       ( SC , plan->sql )                  ;
    QString Q                                        ;
    Q = SC.sql.Update                                (
          PlanTable(Arrays) , "where uuid = :UUID"   ,
          3,"size","checksum","value"              ) ;
    SC . Prepare ( Q                               ) ;
    SC . Bind    ( "uuid"     , uu                 ) ;
    SC . Bind    ( "size"     , s                  ) ;
    SC . Bind    ( "checksum" , checksum           ) ;
    SC . Bind    ( "value"    , Body               ) ;
    SC . Exec    (                                 ) ;
  LeaveSQL       ( SC , plan->sql )                  ;
  item -> setText( 3 , QString::number(s       )   ) ;
  item -> setText( 4 , QString::number(checksum)   ) ;
  Alert          ( Done                            ) ;
}

void N::ArrayLists::Export(void)
{
  QTreeWidgetItem * item                 ;
  item = currentItem (                 ) ;
  nDropOut           ( IsNull ( item ) ) ;
  Export             ( item            ) ;
}

void N::ArrayLists::Export(QTreeWidgetItem * item)
{
  nDropOut            ( IsNull ( item ) )                     ;
  SUID uu = nTreeUuid ( item , 0        )                     ;
  nDropOut            ( uu <= 0         )                     ;
  /////////////////////////////////////////////////////////////
  QString filename = QFileDialog::getSaveFileName             (
            this                                              ,
            tr("Export %1 binary to file").arg(item->text(0)) ,
            plan -> Temporary ( "" )                          ,
            tr("*.*")                                       ) ;
  nDropOut ( filename . length ( ) <= 0                     ) ;
  QByteArray Body                                             ;
  EnterSQL     ( SC , plan -> sql )                           ;
    QString Q                                                 ;
    Q = SC . sql . SelectFrom                                 (
          "value"                                             ,
          PlanTable(Arrays)                                   ,
          SC . WhereUuid ( uu )                             ) ;
    if ( SC . Fetch ( Q ) ) Body = SC . ByteArray ( 0 )       ;
  LeaveSQL     ( SC , plan -> sql )                           ;
  File::toFile ( filename , Body  )                           ;
  Alert        ( Done             )                           ;
}

void N::ArrayLists::ExportCpp(void)
{
  QTreeWidgetItem * item                 ;
  item = currentItem (                 ) ;
  nDropOut           ( IsNull ( item ) ) ;
  ExportCpp          ( item            ) ;
}

void N::ArrayLists::ExportCpp(QTreeWidgetItem * item)
{
  nDropOut            ( IsNull ( item )            ) ;
  SUID uu = nTreeUuid ( item , 0                   ) ;
  int  t  = nTreeInt  ( item , 2                   ) ;
  nDropOut            ( uu <= 0                    ) ;
  QByteArray Body                                    ;
  EnterSQL            ( SC , plan -> sql           ) ;
    QString Q                                        ;
    Q = SC . sql . SelectFrom                        (
          "value"                                    ,
          PlanTable(Arrays)                          ,
          SC . WhereUuid ( uu )                    ) ;
    if ( SC . Fetch ( Q ) )                          {
      Body = SC . ByteArray ( 0 )                    ;
    }                                                ;
  LeaveSQL            ( SC , plan -> sql           ) ;
  if                  ( Body . size ( ) <= 0       ) {
    Alert             ( Error                      ) ;
    return                                           ;
  }                                                  ;
  ////////////////////////////////////////////////////
  BlobToCpp * BTC = new BlobToCpp ( this , plan    ) ;
  QString     ht  = BTC -> windowTitle (           ) ;
  ht = tr("%1 %2") . arg ( item->text(0) ) . arg(ht) ;
  BTC -> setWindowTitle ( ht )                       ;
  if ( BTC -> setData ( &Body , t ) )                {
    if ( QDialog::Accepted != BTC -> exec()        ) {
      Alert   ( Error )                              ;
    } else                                           {
      if ( BTC -> Export ( ) )                       {
        Alert ( Done  )                              ;
      } else                                         {
        Alert ( Error )                              ;
      }                                              ;
    }                                                ;
  } else                                             {
    Alert     ( Error )                              ;
  }                                                  ;
  ////////////////////////////////////////////////////
  BTC->deleteLater()                                 ;
}

void N::ArrayLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  QComboBox * cc                           ;
  SUID        uu = nTreeUuid ( item , 0 )  ;
  int         t  = nTreeInt  ( item , 2 )  ;
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
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
    case 2                                 :
      cc   = setComboBox                   (
               item,column                 ,
               SIGNAL(activated   (int))   ,
               SLOT  (typeFinished(int)) ) ;
      AddItems (*cc,ArrayTypes)            ;
      if (t>=0) (*cc) <= t                 ;
      cc->setMaxVisibleItems(30)           ;
      cc->setEditable(true)                ;
      cc->showPopup()                      ;
    break                                  ;
  }                                        ;
}

void N::ArrayLists::typeFinished(int)
{
  if ( IsNull ( ItemEditing ) ) return                   ;
  QComboBox * cc = Casting ( QComboBox , ItemWidget    ) ;
  if ( IsNull ( cc          ) ) return                   ;
  SUID uu = nTreeUuid      ( ItemEditing , 0           ) ;
  SUID au = N::GetUuid     ( cc                        ) ;
  EnterSQL                 ( SC , plan -> sql          ) ;
    QString Q                                            ;
    int     t = (int)au                                  ;
    ItemEditing -> setData ( 2 , Qt::UserRole , t      ) ;
    ItemEditing -> setText ( 2 , cc -> currentText ( ) ) ;
    Q = SC . sql . Update                                (
          PlanTable(Arrays)                              ,
          "where uuid = :UUID"                           ,
          1                                              ,
          "type"                                       ) ;
    SC . Prepare           ( Q                         ) ;
    SC . Bind              ( "uuid" , uu               ) ;
    SC . Bind              ( "type" , t                ) ;
    SC . Exec              (                           ) ;
  LeaveSQL                 ( SC     , plan -> sql      ) ;
  removeOldItem            ( true   , 0                ) ;
  Alert                    ( Done                      ) ;
}

void N::ArrayLists::nameFinished(void)
{
  if ( IsNull ( ItemEditing ) ) return                         ;
  QLineEdit * line = Casting ( QLineEdit , ItemWidget )        ;
  if (IsNull(line)) return                                     ;
  SUID    uu   = nTreeUuid ( ItemEditing , 0 )                 ;
  QString name = line -> text ( )                              ;
  EnterSQL         ( SC   , plan -> sql )                      ;
    QString Q                                                  ;
    switch ( ItemColumn )                                      {
      case 0                                                   :
        if ( uu <= 0 )                                         {
          if ( name . length ( ) > 0 )                         {
            int udt = Cpp::Undecided                           ;
            uu = SC . Unique                                   (
                   PlanTable(MajorUuid)                        ,
                   "uuid"                                      ,
                    3492                                     ) ;
            SC . assureUuid                                    (
                  PlanTable(MajorUuid)                         ,
                  uu                                           ,
                  Types::Array                               ) ;
            SC . assureUuid ( PlanTable(Arrays) , uu , udt   ) ;
            SC . assureName                                    (
              PlanTable(Names)                                 ,
              uu                                               ,
              vLanguageId                                      ,
              name                                           ) ;
            ItemEditing -> setData ( 0 , Qt::UserRole , uu   ) ;
            ItemEditing -> setData ( 2 , Qt::UserRole , udt  ) ;
            ItemEditing -> setText ( 0 , name                ) ;
            ItemEditing -> setText ( 2 , ArrayTypes [ udt ]  ) ;
            ItemEditing -> setText ( 3 , "0"                 ) ;
            ItemEditing -> setText ( 4 , "0"                 ) ;
            Alert                  ( Done                    ) ;
          } else                                               {
            Alert                  ( Error                   ) ;
          }                                                    ;
        } else                                                 {
          SC . assureName                                      (
            PlanTable(Names)                                   ,
            uu                                                 ,
            vLanguageId                                        ,
            name                                             ) ;
          ItemEditing -> setText ( 0 , name                  ) ;
          Alert                  ( Done                      ) ;
        }                                                      ;
      break                                                    ;
      case 1                                                   :
        Q = SC . sql . Update                                  (
              PlanTable(Arrays)                                ,
              "where uuid = :UUID"                             ,
              1                                                ,
              "name"                                         ) ;
        SC . Prepare           ( Q                           ) ;
        SC . Bind              ( "uuid" , uu                 ) ;
        SC . Bind              ( "name" , name . toUtf8 ( )  ) ;
        SC . Exec              (                             ) ;
        Alert                  ( Done                        ) ;
        ItemEditing -> setText ( 1 , name                    ) ;
      break                                                    ;
    }                                                          ;
  LeaveSQL         ( SC   , plan -> sql )                      ;
  removeOldItem    ( true , 0           )                      ;
}

bool N::ArrayLists::Menu(QPoint pos)
{
  QTreeWidgetItem * item = itemAt  ( pos )                      ;
  MenuManager       mm ( this )                                 ;
  QAction         * aa                                          ;
  QMenu           * im                                          ;
  if (NotNull(item))                                            {
    mm.add(101,tr("Edit"))                                      ;
    mm.addSeparator()                                           ;
    mm.add(102,tr("Export to C/C++"))                           ;
    mm.add(103,tr("Export to file" ))                           ;
    mm.add(104,tr("Replace with data from file" ))              ;
    mm.addSeparator()                                           ;
  }                                                             ;
  mm.add(201,tr("Insert" ))                                     ;
  mm.add(202,tr("Import" ))                                     ;
  mm.addSeparator()                                             ;
  mm.add(501,tr("Refresh"))                                     ;
  im  = mm .addMenu(tr("Items"))                                ;
  aa  = mm . add ( im , 602 , tr("Identifier") )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(1) )                     ;
  aa  = mm . add ( im , 603 , tr("Type"      ) )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(2) )                     ;
  aa  = mm . add ( im , 604 , tr("Size"      ) )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(3) )                     ;
  aa  = mm . add ( im , 605 , tr("Checksum"  ) )                ;
  aa -> setCheckable ( true               )                     ;
  aa -> setChecked   ( !isColumnHidden(4) )                     ;
  DockingMenu        ( mm                 )                     ;
  ///////////////////////////////////////////////////////////////
  mm . setFont       ( plan               )                     ;
  aa = mm . exec     (                    )                     ;
  ///////////////////////////////////////////////////////////////
  if ( IsNull     (      aa ) ) return true                     ;
  if ( RunDocking ( mm , aa ) ) return true                     ;
  ///////////////////////////////////////////////////////////////
  switch (mm[aa])                                               {
    case 101                                                    :
      emit Edit       ( nTreeUuid(item,0) , nTreeInt(item,2)  ) ;
    break                                                       ;
    case 102                                                    :
      ExportCpp       ( item                 )                  ;
    break                                                       ;
    case 103                                                    :
      Export          ( item                 )                  ;
    break                                                       ;
    case 104                                                    :
      Replace         ( item                 )                  ;
    break                                                       ;
    case 201                                                    :
      Insert          (                      )                  ;
    break                                                       ;
    case 202                                                    :
      Import          (                      )                  ;
    break                                                       ;
    case 501                                                    :
      startup         (                      )                  ;
    break                                                       ;
    case 602                                                    :
      setColumnHidden ( 1 , !aa->isChecked() )                  ;
    break                                                       ;
    case 603                                                    :
      setColumnHidden ( 2 , !aa->isChecked() )                  ;
    break                                                       ;
    case 604                                                    :
      setColumnHidden ( 3 , !aa->isChecked() )                  ;
    break                                                       ;
    case 605                                                    :
      setColumnHidden ( 4 , !aa->isChecked() )                  ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  return true                                                   ;
}
