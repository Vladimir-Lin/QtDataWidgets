#include <datawidgets.h>

N::KeywordGroups:: KeywordGroups ( QWidget * parent , Plan * p )
                 : TreeDock      (           parent ,        p )
                 , GroupItems    (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::KeywordGroups::~KeywordGroups(void)
{
}

bool N::KeywordGroups::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::KeywordGroups::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{ Q_UNUSED ( source ) ;
  Q_UNUSED ( mime   ) ;
  Q_UNUSED ( pos    ) ;
  Alert    ( Action ) ;
  return true         ;
}

void N::KeywordGroups::Configure(void)
{
  NewTreeWidgetItem       ( head                  ) ;
  setWindowTitle          ( tr("Keyword groups" ) ) ;
  head->setText           ( 0 , tr("Group"      ) ) ;
  ///////////////////////////////////////////////////
  setFocusPolicy          ( Qt::WheelFocus        ) ;
  setDragDropMode         ( DropOnly              ) ;
  setAlternatingRowColors ( true                  ) ;
  setRootIsDecorated      ( false                 ) ;
  setSelectionMode        ( ExtendedSelection     ) ;
  setColumnCount          ( 1                     ) ;
  assignHeaderItems       ( head                  ) ;
  ///////////////////////////////////////////////////
  setDropFlag             ( DropFont    , true    ) ;
  setDropFlag             ( DropPen     , true    ) ;
  setDropFlag             ( DropBrush   , true    ) ;
  setDropFlag             ( DropKeyword , true    ) ;
  MountClicked            ( 2                     ) ;
  plan -> setFont         ( this                  ) ;
  ///////////////////////////////////////////////////
  PassDragDrop = false                              ;
}

bool N::KeywordGroups::FocusIn(void)
{
  AssignAction ( Label      , windowTitle     ( ) ) ;
  LinkAction   ( Refresh    , startup         ( ) ) ;
  LinkAction   ( Insert     , New             ( ) ) ;
  LinkAction   ( Paste      , Paste           ( ) ) ;
  LinkAction   ( Export     , Export          ( ) ) ;
  LinkAction   ( Copy       , CopyToClipboard ( ) ) ;
  LinkAction   ( SelectNone , SelectNone      ( ) ) ;
  LinkAction   ( SelectAll  , SelectAll       ( ) ) ;
  LinkAction   ( Font       , setFont         ( ) ) ;
  return true                                       ;
}

void N::KeywordGroups::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( Type )   {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

bool N::KeywordGroups::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::KeywordGroups::List(void)
{
  setEnabled ( false          )         ;
  EnterSQL   ( SC , plan->sql )         ;
    SUID  uu                            ;
    UUIDs Uuids = Groups                (
                    SC                  ,
                    Types::Keyword    ) ;
    foreach (uu,Uuids)                  {
      QString name = SC.getName         (
                       PlanTable(Names) ,
                       "uuid"           ,
                       vLanguageId      ,
                       uu             ) ;
      addItem(name,uu,0)                ;
    }                                   ;
  LeaveSQL        ( SC , plan->sql )    ;
  setEnabled      ( true           )    ;
  reportItems     (                )    ;
  emit AutoFit    (                )    ;
  Alert           ( Done           )    ;
}

void N::KeywordGroups::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column > 0 )              ;
  QLineEdit * line                     ;
  removeOldItem()                      ;
  line = setLineEdit                   (
           item                        ,
           column                      ,
           SIGNAL(returnPressed  ())   ,
           SLOT  (editingFinished()) ) ;
  line->setFocus(Qt::TabFocusReason)   ;
}

void N::KeywordGroups::New(void)
{
  NewTreeWidgetItem ( IT )      ;
  IT->setData(0,Qt::UserRole,0) ;
  setAlignments   ( IT     )    ;
  addTopLevelItem ( IT     )    ;
  scrollToItem    ( IT     )    ;
  doubleClicked   ( IT , 0 )    ;
}

void N::KeywordGroups::editingFinished(void)
{
  nDropOut ( IsNull(ItemEditing) )                     ;
  QLineEdit * line = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line)) return                             ;
  ItemEditing -> setText ( ItemColumn , line->text() ) ;
  //////////////////////////////////////////////////////
  SUID    uu     = nTreeUuid(ItemEditing,0)            ;
  QString name   = line->text()                        ;
  EnterSQL ( SC , plan->sql )                          ;
    if (uu<=0)                                         {
      uu = NewDivision(SC,name,Types::Keyword)         ;
      ItemEditing->setData(0,Qt::UserRole,uu)          ;
    } else                                             {
      SC . assureName ( PlanTable(Names)               ,
        uu,vLanguageId,name                          ) ;
    }                                                  ;
  LeaveSQL      ( SC , plan->sql )                     ;
  removeOldItem (                )                     ;
  Alert         ( Done           )                     ;
}

void N::KeywordGroups::Paste(QString text)
{
  nDropOut ( text.length() <= 0 )    ;
  QStringList sl = text.split("\n")  ;
  EnterSQL ( SC , plan->sql )        ;
    for (int i=0;i<sl.count();i++)   {
      QString s = sl[i]              ;
      s = s.replace("\r","")         ;
      s = s.replace("\n","")         ;
      if (s.length()>0)              {
        SUID uu = NewDivision        (
                    SC , s           ,
                    Types::Keyword ) ;
        addItem(s,uu,0)              ;
      }                              ;
    }                                ;
  LeaveSQL ( SC , plan->sql )        ;
  Alert    ( Done           )        ;
}

void N::KeywordGroups::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::KeywordGroups::Export(void)
{
  QString filename = QFileDialog::getSaveFileName  (
                       this                        ,
                       tr("Export keyword groups") ,
                       plan->Temporary("")         ,
                       "*.*;;"
                       "*.txt"                   ) ;
  nDropOut ( filename.length() <= 0 )              ;
  QFile F  ( filename )                            ;
  nDropOut ( !F.open(QIODevice::WriteOnly) )       ;
  for (int i=0;i<topLevelItemCount();i++)          {
    QTreeWidgetItem * item = topLevelItem(i)       ;
    QString n = item->text(0)                      ;
    if (!isColumnHidden(1))                        {
      n += "    "                                  ;
      n += item->text(1)                           ;
    }                                              ;
    n += "\r\n"                                    ;
    F.write(n.toUtf8())                            ;
  }                                                ;
  F.close()                                        ;
}

void N::KeywordGroups::Copy(void)
{
  QStringList s                           ;
  for (int i=0;i<topLevelItemCount();i++) {
    if (topLevelItem(i)->isSelected())    {
      s << topLevelItem(i)->text(0)       ;
    }                                     ;
  }                                       ;
  nDropOut      ( s.count() <= 0 )        ;
  nSetClipboard ( s.join("\r\n") )        ;
}

bool N::KeywordGroups::dropKeywords(QWidget * source,QPointF psf,const UUIDs & Uuids)
{ Q_UNUSED ( source )                  ;
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item = itemAt(pos) ;
  if (IsNull(item))                    {
    Alert ( Error )                    ;
    return false                       ;
  }                                    ;
  dropAction = true                    ;
  EnterSQL ( SC , plan->sql )          ;
    UUIDs PUIDs = Uuids                ;
    SUID  uu    = nTreeUuid(item,0)    ;
    Join                               (
      SC                               ,
      uu                               ,
      Types  :: Division               ,
      Types  :: Keyword                ,
      Groups :: Subordination          ,
      0,PUIDs                        ) ;
  LeaveSQL ( SC , plan->sql )          ;
  Alert    ( Done           )          ;
  dropAction = false                   ;
  return true                          ;
}

bool N::KeywordGroups::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                                ;
  QMdiSubWindow   * mdi   = Casting ( QMdiSubWindow,parent() ) ;
  QDockWidget     * dock  = Casting ( QDockWidget  ,parent() ) ;
  QTreeWidgetItem * item  = itemAt  ( pos                    ) ;
  UUIDs             Uuids = itemUuids ( 0 )                    ;
  QAction         * aa                                         ;
  nSafeExec(item)                                              {
    mm.add ( 401 , tr("Keyword list"                ))         ;
    mm.addSeparator()                                          ;
  }                                                            ;
  mm . add ( 101 , tr("Refresh"                     ))         ;
  mm . addSeparator (                                )         ;
  mm . add ( 201 , tr("Insert"                      ))         ;
  mm . addSeparator (                                )         ;
  mm . add ( 501 , tr("Select all"                  ))         ;
  mm . add ( 502 , tr("Select none"                 ))         ;
  mm . add ( 503 , tr("Set font"                    ))         ;
  mm . addSeparator (                                )         ;
  mm . add ( 601 , tr("Edit multilingual names"     ))         ;
  nSafeExec(dock) mm.add(1000001,tr("Move to window area"))    ;
  nSafeExec(mdi ) mm.add(1000002,tr("Move to dock area"  ))    ;
  mm . setFont(plan)                                           ;
  aa = mm.exec()                                               ;
  if (IsNull(aa)) return true                                  ;
  switch (mm[aa])                                              {
    case 101                                                   :
      startup    ( )                                           ;
    break                                                      ;
    case 201                                                   :
      New        ( )                                           ;
    break                                                      ;
    case 301                                                   :
      Export     ( )                                           ;
    break                                                      ;
    case 401                                                   :
      emit Group ( item->text(0) , nTreeUuid(item,0) )         ;
    break                                                      ;
    case 501                                                   :
      SelectAll  ( )                                           ;
    break                                                      ;
    case 502                                                   :
      SelectNone ( )                                           ;
    break                                                      ;
    case 503                                                   :
      setFont    ( )                                           ;
    break                                                      ;
    case 504                                                   :
      Copy       ( )                                           ;
    break                                                      ;
    case 601                                                   :
      emit Edit  ( windowTitle() , Uuids )                     ;
    break                                                      ;
    case 1000001                                               :
      emit attachMdi ( this , Qt::Vertical )                   ;
    break                                                      ;
    case 1000002                                               :
      emit attachDock                                          (
        this                                                   ,
        windowTitle()                                          ,
        Qt::RightDockWidgetArea                                ,
        Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea  | Qt::BottomDockWidgetArea    ) ;
    break                                                      ;
  }                                                            ;
  return true                                                  ;
}
