#include <datawidgets.h>

N::DirectoryEditor:: DirectoryEditor ( QWidget * parent , Plan * p             )
                   : TreeWidget      (           parent ,        p             )
                   , Group           ( 0 , Types::None , Groups::Subordination )
                   , scope           ( ""                                      )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DirectoryEditor::~DirectoryEditor (void)
{
}

void N::DirectoryEditor::Configure(void)
{
  setAccessibleName       ( "N::DirectoryEditor"             ) ;
  setObjectName           ( "N::DirectoryEditor"             ) ;
  NewTreeWidgetItem       ( head                             ) ;
  setWindowTitle          ( tr("Directories")                ) ;
  head -> setText         ( 0 , tr("Directory")              ) ;
  setFocusPolicy          ( Qt::WheelFocus                   ) ;
  setDragDropMode         ( DropOnly                         ) ;
  setDropFlag             ( DropUrls , true                  ) ;
  setAlternatingRowColors ( true                             ) ;
  setRootIsDecorated      ( false                            ) ;
  setSelectionMode        ( ExtendedSelection                ) ;
  setColumnCount          ( 1                                ) ;
  assignHeaderItems       ( head                             ) ;
  plan -> setFont         ( this                             ) ;
  //////////////////////////////////////////////////////////////
  LocalIcons [ "Delete" ] = QIcon ( ":/images/delete.png" )    ;
  LocalIcons [ "Insert" ] = QIcon ( ":/images/plus.png"   )    ;
  LocalIcons [ "Update" ] = QIcon ( ":/images/update.png" )    ;
  LocalIcons [ "Save"   ] = QIcon ( ":/images/save.png"   )    ;
  //////////////////////////////////////////////////////////////
  nConnect                ( this , SIGNAL ( EmitReload ( ) )   ,
                            this , SLOT   ( SendReload ( ) ) ) ;
}

bool N::DirectoryEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , Insert          ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( Paste        , Paste           ( ) ) ;
  LinkAction        ( SelectAll    , SelectAll       ( ) ) ;
  LinkAction        ( SelectNone   , SelectNone      ( ) ) ;
  LinkAction        ( Refresh      , startup         ( ) ) ;
  LinkAction        ( Save         , Store           ( ) ) ;
  LinkAction        ( GoUp         , GoUp            ( ) ) ;
  LinkAction        ( GoDown       , GoDown          ( ) ) ;
  return true                                              ;
}

bool N::DirectoryEditor::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

bool N::DirectoryEditor::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if ( source == this ) return false ;
  alert ( "Click" , "" )             ;
  return mime -> hasUrls ( )         ;
}

bool N::DirectoryEditor::dropMoving(QWidget * source,const QMimeData * mime,QPoint pos)
{
  if ( source == this ) return false ;
  return true                        ;
}

bool N::DirectoryEditor::dropAppend(QWidget * source,const QMimeData * mime,QPoint pos)
{
  return dropItems ( source , mime , pos ) ;
}

bool N::DirectoryEditor::dropUrls(QWidget *,QPointF,const QList<QUrl> & urls)
{
  return AppendUrls ( urls ) ;
}

void N::DirectoryEditor::run(int T,ThreadData * d)
{
  nDropOut         ( ! IsContinue ( d ) ) ;
  switch           ( T                  ) {
    case 10001                            :
      startLoading (                    ) ;
      Load         ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
    case 10002                            :
      startLoading (                    ) ;
      Saving       ( d                  ) ;
      stopLoading  (                    ) ;
    break                                 ;
  }                                       ;
}

bool N::DirectoryEditor::Load(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false )        ;
  ////////////////////////////////////////////////
  QStringList L                                  ;
  QString     S                                  ;
  ////////////////////////////////////////////////
  emit OnBusy           (             )          ;
  WaitClear             (             )          ;
  emit assignEnabling   ( false       )          ;
  ////////////////////////////////////////////////
  L = plan -> settings . getStringList ( scope ) ;
  ////////////////////////////////////////////////
  Directories . clear   (             )          ;
  foreach ( S , L ) if ( S . length ( ) > 0 )    {
    if ( ! Directories . contains ( S ) )        {
      NewTreeWidgetItem ( it          )          ;
      it -> setText     ( 0 , S       )          ;
      addTopLevelItem   ( it          )          ;
      Directories << S                           ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  emit assignEnabling   ( true        )          ;
  emit GoRelax          (             )          ;
  setModified           ( false       )          ;
  alert                 ( "Done" , "" )          ;
  return true                                    ;
}

bool N::DirectoryEditor::Saving(ThreadData * d)
{
  nKickOut ( ! IsContinue ( d ) , false )            ;
  if ( scope . length ( ) <= 0 ) return false        ;
  ////////////////////////////////////////////////////
  QStringList       L                                ;
  QTreeWidgetItem * it                               ;
  emit OnBusy         (             )                ;
  emit assignEnabling ( false       )                ;
  for (int i = 0 ; i < topLevelItemCount ( ) ; i++ ) {
    it = topLevelItem ( i           )                ;
    L << it -> text   ( 0           )                ;
  }                                                  ;
  plan -> settings . setStringList ( scope , L )     ;
  emit assignEnabling ( true        )                ;
  emit GoRelax        (             )                ;
  setModified         ( false       )                ;
  alert               ( "Done" , "" )                ;
  ////////////////////////////////////////////////////
  return true                                        ;
}

bool N::DirectoryEditor::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::DirectoryEditor::Reload(void)
{
  start ( 10001 ) ;
}

void N::DirectoryEditor::setScope(QString s)
{
  scope = s   ;
  startup ( ) ;
}

void N::DirectoryEditor::SendReload(void)
{
  emit Reload ( scope ) ;
}

bool N::DirectoryEditor::AppendUrls(const QList<QUrl> & urls)
{
  bool dropIn = false                           ;
  ///////////////////////////////////////////////
  for (int i = 0 ; i < urls . count ( ) ; i++ ) {
    QUrl u = urls [ i ]                         ;
    if ( u . isLocalFile ( ) )                  {
      QString dir = u . toLocalFile ( )         ;
      if ( dir . length ( ) > 0 )               {
        if ( ! Directories . contains ( dir ) ) {
          QFileInfo FI ( dir )                  ;
          if ( FI . isDir ( ) )                 {
            NewTreeWidgetItem ( it      )       ;
            it -> setText     ( 0 , dir )       ;
            addTopLevelItem   ( it      )       ;
            Directories << dir                  ;
            dropIn = true                       ;
          }                                     ;
        }                                       ;
      }                                         ;
    }                                           ;
  }                                             ;
  ///////////////////////////////////////////////
  if            ( dropIn      )                 {
    setModified ( true        )                 ;
    alert       ( "Done" , "" )                 ;
  }                                             ;
  return true                                   ;
}

void N::DirectoryEditor::Insert(void)
{
  QString dir                                                        ;
  dir = plan -> Dirs [ Directory::Images ] . absoluteFilePath ( "" ) ;
  if ( plan -> Variables . contains ( "DirectoryEditor" ) )          {
    dir = plan -> Variables [ "DirectoryEditor" ] . toString ( )     ;
  }                                                                  ;
  dir = QFileDialog::getExistingDirectory                            (
          this                                                       ,
          tr("Import monitoring directory")                          ,
          dir                                                      ) ;
  if ( dir . length ( ) <= 0 ) return                                ;
  plan -> Variables [ "DirectoryEditor" ] = dir                      ;
  if ( Directories . contains ( dir ) )                              {
    alert             ( "Error" , "" )                               ;
  } else                                                             {
    NewTreeWidgetItem ( it           )                               ;
    it -> setText     ( 0 , dir      )                               ;
    addTopLevelItem   ( it           )                               ;
    setModified       ( true         )                               ;
    alert             ( "Done"  , "" )                               ;
    Directories << dir                                               ;
  }                                                                  ;
}

void N::DirectoryEditor::Delete(void)
{
  QTreeWidgetItem * it                ;
  QString           s                 ;
  it = currentItem (                ) ;
  nDropOut         ( IsNull ( it )  ) ;
  s = it -> text   ( 0              ) ;
  takeItem         ( it             ) ;
  setModified      ( true           ) ;
  alert            ( "Done" , ""    ) ;
  if ( Directories . contains ( s ) ) {
    int idx                           ;
    idx = Directories . indexOf ( s ) ;
    if             ( idx >= 0       ) {
      Directories . takeAt ( idx )    ;
    }                                 ;
  }                                   ;
}

void N::DirectoryEditor::Paste(void)
{
  QString T = nClipboardText                  ;
  if ( T . length ( ) <= 0 ) return           ;
  QStringList L = T . split ( '\n' )          ;
  QString     S                               ;
  L = File::PurifyLines ( L )                 ;
  foreach ( S , L ) if ( S . length ( ) > 0 ) {
    if ( ! Directories . contains ( S ) )     {
      NewTreeWidgetItem ( it )                ;
      it -> setText   ( 0 , S )               ;
      addTopLevelItem ( it    )               ;
      Directories << S                        ;
    }                                         ;
  }                                           ;
  setModified       ( true  )                 ;
}

void N::DirectoryEditor::Store(void)
{
  if ( scope . length ( ) <= 0 ) return ;
  start ( 10002 )                       ;
}

void N::DirectoryEditor::GoUp(void)
{
  QTreeWidgetItem * it               ;
  it = currentItem (      )          ;
  if (IsNull(it)) return             ;
  int index                          ;
  index = indexOfTopLevelItem ( it ) ;
  if (index<0) return                ;
  takeTopLevelItem ( index )         ;
  ////////////////////////////////////
  index --                           ;
  if (index<0) index = 0             ;
  insertTopLevelItem ( index , it )  ;
  setCurrentItem     ( it         )  ;
  setModified        ( true       )  ;
}

void N::DirectoryEditor::GoDown(void)
{
  QTreeWidgetItem * it               ;
  it = currentItem (      )          ;
  if (IsNull(it)) return             ;
  int index                          ;
  index = indexOfTopLevelItem ( it ) ;
  if (index<0) return                ;
  takeTopLevelItem ( index )         ;
  ////////////////////////////////////
  index ++                           ;
  if (index>=topLevelItemCount())    {
    index = topLevelItemCount()      ;
  }                                  ;
  insertTopLevelItem ( index , it )  ;
  setCurrentItem     ( it         )  ;
  setModified        ( true       )  ;
}

void N::DirectoryEditor::setModified (bool enabled)
{
  if ( ! localModified ) localTitle = windowTitle ( )           ;
  localModified = enabled                                       ;
  ///////////////////////////////////////////////////////////////
  QString t                                                     ;
  if ( enabled )                                                {
    t = QString("* %1").arg(localTitle)                         ;
  } else                                                        {
    t = localTitle                                              ;
  }                                                             ;
  emit assignWindowTitle ( t )                                  ;
}

bool N::DirectoryEditor::Menu(QPoint pos)
{ Q_UNUSED    ( pos       )                                   ;
  nScopedMenu ( mm , this )                                   ;
  QAction * aa                                                ;
  mm   . add ( 101 , LocalIcons [ "Update" ] , tr("Reload") ) ;
  if         ( localModified                                ) {
    mm . add ( 102 , LocalIcons [ "Save"   ] , tr("Store" ) ) ;
  }                                                           ;
  mm   . addSeparator (                                     ) ;
  mm   . add ( 201 , LocalIcons [ "Insert" ] , tr("Insert") ) ;
  mm   . add ( 202 , LocalIcons [ "Delete" ] , tr("Delete") ) ;
  mm   . setFont   ( plan )                                   ;
  aa   = mm . exec (      )                                   ;
  if ( IsNull ( aa ) ) return true                            ;
  switch      ( mm [ aa ] )                                   {
    case 101                                                  :
      startup (           )                                   ;
    break                                                     ;
    case 102                                                  :
      Store   (           )                                   ;
    break                                                     ;
    case 201                                                  :
      Insert  (           )                                   ;
    break                                                     ;
    case 202                                                  :
      Delete  (           )                                   ;
    break                                                     ;
  }                                                           ;
  return true                                                 ;
}
