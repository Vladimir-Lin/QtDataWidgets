#include <datawidgets.h>

N::IdentifierEditor:: IdentifierEditor ( QWidget * parent , Plan * p    )
                    : TreeDock         (           parent ,        p    )
                    , Ownership        ( 0                , Types::None )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::IdentifierEditor::~IdentifierEditor (void)
{
}

void N::IdentifierEditor::Configure(void)
{
  PassDragDrop = false                                         ;
  NewTreeWidgetItem            ( head                        ) ;
  head -> setText              ( 0 , tr("Id"      )          ) ;
  head -> setText              ( 1 , tr("Name"    )          ) ;
  setWindowTitle               (     tr("Identifier editor") ) ;
  setDragDropMode              ( NoDragDrop                  ) ;
  setRootIsDecorated           ( false                       ) ;
  setAlternatingRowColors      ( true                        ) ;
  setSelectionMode             ( ExtendedSelection           ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded       ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded       ) ;
  setColumnCount               ( 2                           ) ;
  assignHeaderItems            ( head                        ) ;
  setColumnHidden              ( 0 , true                    ) ;
  setColumnHidden              ( 1 , false                   ) ;
  setHeaderHidden              (     true                    ) ;
  setFunction                  ( 11241212 , true             ) ;
  MountClicked                 ( 1                           ) ;
  MountClicked                 ( 2                           ) ;
  plan -> setFont              ( this                        ) ;
}

bool N::IdentifierEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true                ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  LinkAction        ( Export       , Export          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  return true                                              ;
}

void N::IdentifierEditor::run(int T,ThreadData * data)
{
  if ( ! IsContinue ( data ) ) return                ;
  switch ( T )                                       {
    case 10001                                       :
      startLoading      (                          ) ;
      FetchIdentifiers  ( data                     ) ;
      stopLoading       (                          ) ;
    break                                            ;
    case 10002                                       :
      startLoading      (                          ) ;
      DeleteIdentifiers ( data                     ) ;
      stopLoading       (                          ) ;
    break                                            ;
    case 10003                                       :
      startLoading      (                          ) ;
      ModifyIdentifiers ( data , data -> Arguments ) ;
      stopLoading       (                          ) ;
    break                                            ;
  }                                                  ;
}

bool N::IdentifierEditor::startup(void)
{
  emit clearItems (       ) ;
  start           ( 10001 ) ;
  return true               ;
}

void N::IdentifierEditor::startByThread(enum QThread::Priority priority)
{
  QThread * thread = new QThread ( )           ;
  moveToThread ( thread )                      ;
  nConnect ( thread , SIGNAL ( started ( ) )   ,
             this   , SLOT   ( startup ( ) ) ) ;
  thread -> start ( priority )                 ;
}

void N::IdentifierEditor::FetchIdentifiers(ThreadData * d)
{
  if ( isStandby ( ) ) return                                ;
  SqlConnection SC ( plan -> sql )                           ;
  GroupItems    GI ( plan        )                           ;
  if ( SC . open ( FunctionString ) )                        {
    QString Q                                                ;
    UUIDs   U                                                ;
    SUID    u                                                ;
    //////////////////////////////////////////////////////////
    GI . AutoMap    = true                                   ;
    GI . GroupTable = GI . LookTable                         (
                        Types::Commodity                     ,
                        ObjectType ( )                       ,
                        Groups::Subordination              ) ;
    U = GI . GetOwners                                       (
          SC                                                 ,
          ObjectUuid ( )                                     ,
          Types::Commodity                                   ,
          ObjectType ( )                                     ,
          Groups::Subordination                              ,
          SC.OrderByDesc("id")                             ) ;
    if ( U . count()>0)                                      {
      foreach ( u , U )                                      {
        Q = SC . sql . SelectFrom                            (
              "id,name"                                      ,
              PlanTable(Identifiers)                         ,
              SC . WhereUuid   ( u    )                      ,
              SC . OrderByDesc ( "id" )                    ) ;
        //////////////////////////////////////////////////////
        SqlLoopNow ( SC , Q )                                ;
          qint64            id   = SC . Tuid           ( 0 ) ;
          QString           name = SC . String         ( 1 ) ;
          QTreeWidgetItem * it   = new QTreeWidgetItem (   ) ;
          it -> setData   ( 0 , Qt::UserRole , id          ) ;
          it -> setData   ( 1 , Qt::UserRole , u           ) ;
          it -> setText   ( 0 , QString::number ( id )     ) ;
          it -> setText   ( 1 , name                       ) ;
          addTopLevelItem ( it                             ) ;
        SqlLoopErr ( SC , Q )                                ;
        SqlLoopEnd ( SC , Q )                                ;
      }                                                      ;
    }                                                        ;
    //////////////////////////////////////////////////////////
    SC . close  ( )                                          ;
  }                                                          ;
  SC   . remove ( )                                          ;
}

void N::IdentifierEditor::DeleteIdentifiers(ThreadData * d)
{
  TreeWidgetItems items = selectedItems ( )              ;
  UUIDs           U                                      ;
  ////////////////////////////////////////////////////////
  if ( items . count ( ) <= 0 ) return                   ;
  for (int i = 0 ; i < items . count ( ) ; i++ )         {
    SUID u                                               ;
    u = items[i]->data(1,Qt::UserRole).toULongLong()     ;
    takeItem ( items [ i ] )                             ;
    if ( ( u > 0 ) && ( ! U . contains ( u ) ) ) U << u  ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  if ( isStandby ( ) ) return                            ;
  ////////////////////////////////////////////////////////
  SqlConnection SC ( plan -> sql )                       ;
  GroupItems    GI ( plan        )                       ;
  if ( SC . open ( FunctionString ) )                    {
    QString Q                                            ;
    SUID    u                                            ;
    //////////////////////////////////////////////////////
    GI . AutoMap    = true                               ;
    GI . GroupTable = GI . LookTable                     (
                        Types::Commodity                 ,
                        ObjectType ( )                   ,
                        Groups::Subordination          ) ;
    //////////////////////////////////////////////////////
    foreach ( u , U )                                    {
      Q = SC . sql . DeleteFrom                          (
            GI . GroupTable                              ,
            GI . ExactItem                               (
              u                                          ,
              ObjectUuid ( )                             ,
              Types::Commodity                           ,
              ObjectType ( )                             ,
              Groups::Subordination                  ) ) ;
      SC . Query ( Q )                                   ;
    }                                                    ;
    //////////////////////////////////////////////////////
    SC . close  (      )                                 ;
  }                                                      ;
  SC   . remove (      )                                 ;
  ////////////////////////////////////////////////////////
  Alert         ( Done )                                 ;
}

void N::IdentifierEditor::ModifyIdentifiers(ThreadData * d,VarArgs & args)
{
  if ( args . count ( ) < 4 ) return                     ;
  int               it = args [ 0 ] . toInt       (    ) ;
  qint64            id = args [ 1 ] . toLongLong  (    ) ;
  SUID              uu = args [ 2 ] . toULongLong (    ) ;
  QString           nn = args [ 3 ] . toString    (    ) ;
  QString           tn = windowTitle              (    ) ;
  QTreeWidgetItem * IT = topLevelItem             ( it ) ;
  ////////////////////////////////////////////////////////
  if ( IsNull ( IT ) )                                   {
    Alert ( Error )                                      ;
    return                                               ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  if ( isStandby ( ) ) return                            ;
  ////////////////////////////////////////////////////////
  SqlConnection  SC ( plan -> sql )                      ;
  GroupItems     GI ( plan        )                      ;
  FinanceManager FM ( plan        )                      ;
  if ( SC . open ( FunctionString ) )                    {
    QString Q                                            ;
    bool    processed = false                            ;
    //////////////////////////////////////////////////////
    GI . AutoMap    = true                               ;
    GI . GroupTable = GI . LookTable                     (
                        Types::Commodity                 ,
                        ObjectType ( )                   ,
                        Groups::Subordination          ) ;
    //////////////////////////////////////////////////////
    if ( ( ! processed ) && ( id > 0 ) && ( uu > 0 ) )   {
      if ( nn . length ( ) > 0 )                         {
        Q = SC . sql . Update                            (
              PlanTable(Identifiers)                     ,
              SC . sql . Where ( 2 , "id" , "uuid" )     ,
              1                                          ,
              "name"                                   ) ;
        SC . Prepare ( Q                               ) ;
        SC . Bind    ( "id"   , id                     ) ;
        SC . Bind    ( "uuid" , uu                     ) ;
        SC . Bind    ( "name" , nn . toUtf8 ( )        ) ;
        SC . Exec    (                                 ) ;
        processed = true                                 ;
      }                                                  ;
    }                                                    ;
    //////////////////////////////////////////////////////
    if ( ( ! processed ) && ( id <= 0 ) && ( uu <= 0 ) ) {
      if ( ( nn.length() > 0 ) && ( tn.length() > 0 )  ) {
        SUID iid = FM . FindIdentifier ( SC , nn )       ;
        if ( iid >  0 )                                  {
          Q = SC . sql . SelectFrom                      (
                "type"                                   ,
                PlanTable(MajorUuid)                     ,
                SC . WhereUuid ( iid )                 ) ;
          if ( SC . Fetch ( Q ) )                        {
            int tt = SC . Int ( 0 )                      ;
            if ( Types::Commodity != tt ) iid = 0        ;
          }                                              ;
        }                                                ;
        if ( iid <= 0 )                                  {
          SUID cid = FM . appendCommodity ( SC )         ;
          FM . appendNamemap                             (
            SC                                           ,
            cid                                          ,
            plan -> LanguageId                           ,
            tn                                         ) ;
          FM . assureIdentifier ( SC , cid , nn  )       ;
          uu = cid                                       ;
        } else                                           {
          uu = iid                                       ;
        }                                                ;
        //////////////////////////////////////////////////
        if ( uu > 0 )                                    {
          GI . Insert ( SC                               ,
                        uu                               ,
                        ObjectUuid ( )                   ,
                        Types::Commodity                 ,
                        ObjectType ( )                   ,
                        Groups::Subordination          ) ;
        }                                                ;
        //////////////////////////////////////////////////
        Q = SC . sql . SelectFrom                        (
              "id"                                       ,
              PlanTable(Identifiers)                     ,
              SC . sql . Where ( 2 , "uuid" , "name" ) ) ;
        SC . Prepare ( Q                               ) ;
        SC . Bind    ( "uuid" , uu                     ) ;
        SC . Bind    ( "name" , nn . toUtf8 ( )        ) ;
        if ( SC . Exec ( ) && SC . Next ( ) )            {
          id = SC . Tuid ( 0 )                           ;
        }                                                ;
        //////////////////////////////////////////////////
        IT -> setData ( 0 , Qt::UserRole , id )          ;
        IT -> setData ( 1 , Qt::UserRole , uu )          ;
      }                                                  ;
    }                                                    ;
    //////////////////////////////////////////////////////
    SC . close  (      )                                 ;
  }                                                      ;
  SC   . remove (      )                                 ;
  ////////////////////////////////////////////////////////
  Alert         ( Done )                                 ;
}

void N::IdentifierEditor::Insert(void)
{
  NewTreeWidgetItem ( IT                    ) ;
  addTopLevelItem   ( IT                    ) ;
  IT -> setText     ( 0  , "0"              ) ;
  IT -> setData     ( 0  , Qt::UserRole , 0 ) ;
  IT -> setData     ( 1  , Qt::UserRole , 0 ) ;
  doubleClicked     ( IT , 1                ) ;
}

void N::IdentifierEditor::Delete(void)
{
  start ( 10002 ) ;
}

void N::IdentifierEditor::Export(void)
{
  if ( topLevelItemCount ( ) <= 0 ) return    ;
  QString filename = plan -> Temporary ( "" ) ;
  filename = QFileDialog::getOpenFileName     (
               this                           ,
               tr("Export identifiers")       ,
               filename                       ,
               tr("Plain text (*.txt)")     ) ;
  if ( filename . length ( ) <= 0 ) return    ;
  QString m                                   ;
  for (int i=0;i<topLevelItemCount();i++)     {
    QTreeWidgetItem * it                      ;
    it = topLevelItem ( i )                   ;
    m += it -> text   ( 1 )                   ;
    m += "\n"                                 ;
  }                                           ;
  QByteArray b = m . toUtf8 ( )               ;
  File::toFile ( filename , b )               ;
  Alert        ( Done         )               ;
}

void N::IdentifierEditor::singleClicked(QTreeWidgetItem*,int)
{
  Alert ( Action ) ;
}

void N::IdentifierEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * le = NULL                        ;
  removeOldItem ( )                            ;
  switch (column)                              {
    case 1                                     :
      le = setLineEdit                         (
             item                              ,
             column                            ,
             SIGNAL  ( editingFinished ( ) )   ,
             SLOT    ( nameFinished    ( ) ) ) ;
      le -> setFocus ( Qt::TabFocusReason    ) ;
    break                                      ;
  }                                            ;
}

void N::IdentifierEditor::nameFinished(void)
{
  QLineEdit * name = Casting ( QLineEdit   , ItemWidget )                 ;
  /////////////////////////////////////////////////////////////////////////
  if ( IsNull ( name ) )                                                  {
    removeOldItem ( )                                                     ;
    return                                                                ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  int     it = indexOfTopLevelItem ( ItemEditing      )                   ;
  qint64  id = ItemEditing -> data ( 0 , Qt::UserRole ) . toLongLong  ( ) ;
  SUID    uu = ItemEditing -> data ( 1 , Qt::UserRole ) . toULongLong ( ) ;
  QString nn = name        -> text (                  )                   ;
  /////////////////////////////////////////////////////////////////////////
  ItemEditing -> setText           ( 1 , nn           )                   ;
  removeOldItem                    (                  )                   ;
  /////////////////////////////////////////////////////////////////////////
  VarArgs args                                                            ;
  args << it                                                              ;
  args << id                                                              ;
  args << uu                                                              ;
  args << nn                                                              ;
  start ( 10003 , args )                                                  ;
}

bool N::IdentifierEditor::Menu(QPoint pos)
{
  if ( ! isFunction ( 11241212 ) ) return false                    ;
  nScopedMenu ( mm , this )                                        ;
  //////////////////////////////////////////////////////////////////
  QAction         * aa = NULL                                      ;
  QTreeWidgetItem * it = itemAt ( pos )                            ;
  //////////////////////////////////////////////////////////////////
  mm . add ( 101 , tr("Add")    )                                  ;
  if ( NotNull ( it ) && it -> isSelected ( ) )                    {
    mm . add ( 102 , tr("Delete") )                                ;
  }                                                                ;
  mm . addSeparator ( )                                            ;
  mm . add          ( 801                                          ,
                      tr("Header")                                 ,
                      true                                         ,
                      ! isHeaderHidden ( )                       ) ;
  mm . add          ( 802                                          ,
                      tr("Position")                               ,
                      true                                         ,
                      ! isColumnHidden ( 0 )                     ) ;
  //////////////////////////////////////////////////////////////////
  mm . setFont   ( plan    )                                       ;
  aa = mm . exec (         )                                       ;
  if ( IsNull ( aa ) )                                             {
    return false                                                   ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  switch (mm[aa])                                                  {
    case 101                                                       :
      Insert ( )                                                   ;
    return true                                                    ;
    case 102                                                       :
      Delete ( )                                                   ;
    return true                                                    ;
    case 801                                                       :
      setHeaderHidden (     ! aa -> isChecked ( ) )                ;
    return true                                                    ;
    case 802                                                       :
      setColumnHidden ( 0 , ! aa -> isChecked ( ) )                ;
      if ( ! isColumnHidden ( 0 ) )                                {
        for (int i=0;i<2;i++) resizeColumnToContents ( i )         ;
      }                                                            ;
    return true                                                    ;
  }                                                                ;
  return true                                                      ;
}
