#include <datawidgets.h>

N::Terminologies:: Terminologies    ( QWidget * parent , Plan * p )
                 : TreeDock         (           parent ,        p )
                 , KnowledgeManager (                           p )
                 , Object           ( 0 , Types  :: Field         )
                 , Relation         ( Groups :: Subordination , 0 )
                 , dropAction       ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::Terminologies::~Terminologies(void)
{
}

QMimeData * N::Terminologies::dragMime (void)
{
  UUIDs Uuids                                       ;
  SUID  uuid  = 0                                   ;
  ///////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)           {
    QTreeWidgetItem * it = topLevelItem(i)          ;
    if (it->isSelected())                           {
      Uuids << nTreeUuid(it,0)                      ;
    }                                               ;
  }                                                 ;
  if (Uuids.count()<=0)                             {
    QTreeWidgetItem * it = currentItem()            ;
    if (NotNull(it))                                {
      uuid = nTreeUuid(it,0)                        ;
    }                                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  nKickOut(Uuids.count()<=0 && uuid == 0,NULL)      ;
  if (Uuids.count()==1)                             {
    uuid  = Uuids [0]                               ;
    Uuids . clear ( )                               ;
  }                                                 ;
  ///////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()                ;
  if (Uuids.count()==0)                             {
    setMime ( mime,"terminology/uuid" ,uuid  )      ;
  } else                                            {
    setMime ( mime,"terminology/uuids",Uuids )      ;
  }                                                 ;
  ///////////////////////////////////////////////////
  QImage image = QImage(":/images/terminology.png") ;
  mime -> setImageData(image)                       ;
  return mime                                       ;
}

bool N::Terminologies::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::Terminologies::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::Terminologies::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::Terminologies::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::Terminologies::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::Terminologies::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::Terminologies::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::Terminologies::Configure(void)
{
#ifdef USENHEADER
  installHeader                (                                   ) ;
#endif
  NewTreeWidgetItem            ( head                              ) ;
  head -> setText              ( 0 , tr("Name"  )                  ) ;
  head -> setText              ( 1 , tr("Type"  )                  ) ;
  head -> setText              ( 2 , tr("Flags" )                  ) ;
  setWindowTitle               ( tr("Terminologies")               ) ;
  setWindowIcon                ( QIcon(":/images/terminology.png") ) ;
  setDragDropMode              ( DragDrop                          ) ;
  setRootIsDecorated           ( false                             ) ;
  setAlternatingRowColors      ( true                              ) ;
  setSelectionMode             ( ExtendedSelection                 ) ;
  setColumnCount               ( 3                                 ) ;
  setColumnHidden              ( 1 , true                          ) ;
  setColumnHidden              ( 2 , true                          ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded             ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded             ) ;
  assignHeaderItems            ( head                              ) ;
  plan -> setFont              ( this                              ) ;
  setDropFlag                  ( DropTerminology , true            ) ;
  MountClicked                 ( 2                                 ) ;
}

bool N::Terminologies::FocusIn(void)
{
  AssignAction ( Label   , windowTitle     ( ) ) ;
  LinkAction   ( Refresh , startup         ( ) ) ;
  LinkAction   ( Insert  , Insert          ( ) ) ;
  LinkAction   ( Copy    , CopyToClipboard ( ) ) ;
  LinkAction   ( Font    , setFont         ( ) ) ;
  LinkAction   ( Export  , Export          ( ) ) ;
  LinkAction   ( Import  , Import          ( ) ) ;
  LinkAction   ( Paste   , Paste           ( ) ) ;
  if (uuid>0)                                    {
    LinkAction ( Delete  , Delete          ( ) ) ;
  }                                              ;
  return true                                    ;
}

bool N::Terminologies::startup(void)
{
  clear            (                          )        ;
  if (uuid>0)                                          {
    setDragDropMode( DragDrop                 )        ;
  } else                                               {
    setDragDropMode( DragOnly                 )        ;
  }                                                    ;
  GroupItems    GI ( plan                     )        ;
  EnterSQL         ( SC , plan->sql           )        ;
    QString         Q                                  ;
    SUID            uu                                 ;
    UUIDs           Uuids                              ;
    QMap<SUID,int > Types                              ;
    QMap<SUID,TUID> Flags                              ;
    if (uuid>0)                                        {
      Uuids = GI.Subordination                         (
                SC                                     ,
                uuid                                   ,
                type                                   ,
                Types::Terminology                     ,
                relation                               ,
                "order by position asc"              ) ;
      foreach (uu,Uuids)                               {
        Q = SC.sql.SelectFrom                          (
              "type,flags"                             ,
              PlanTable(Terminology)                   ,
              SC.WhereUuid(uu)                       ) ;
        if (SC.Fetch(Q))                               {
          Types[uu] = SC . Int  ( 0 )                  ;
          Flags[uu] = SC . Tuid ( 1 )                  ;
        }                                              ;
      }                                                ;
    } else                                             {
      Q = SC.sql.SelectFrom("uuid,type,flags"          ,
                            PlanTable(Terminology)     ,
                            "order by id desc"       ) ;
      SqlLoopNow(SC,Q)                                 ;
        uu        = SC . Uuid ( 0 )                    ;
        Uuids    << uu                                 ;
        Types[uu] = SC . Int  ( 1 )                    ;
        Flags[uu] = SC . Tuid ( 2 )                    ;
      SqlLoopErr(SC,Q)                                 ;
      SqlLoopEnd(SC,Q)                                 ;
    }                                                  ;
    foreach (uu,Uuids)                                 {
      NewTreeWidgetItem(item)                          ;
      QString name = Name(SC,uu,vLanguageId          ) ;
      item->setData(0,Qt::UserRole,      uu          ) ;
      item->setData(1,Qt::UserRole,Types[uu]         ) ;
      item->setData(2,Qt::UserRole,Flags[uu]         ) ;
      item->setText(0,name                           ) ;
      item->setText(1,QString::number(Types[uu])     ) ;
      item->setText(2,QString::number(Flags[uu])     ) ;
      addTopLevelItem(item)                            ;
    }                                                  ;
  LeaveSQL         ( SC , plan->sql           )        ;
  SuitableColumns  (                          )        ;
  reportItems      (                          )        ;
  Alert            ( Done                     )        ;
  return true                                          ;
}

void N::Terminologies::Insert(void)
{
  NewTreeWidgetItem(item)         ;
  item->setData(0,Qt::UserRole,0) ;
  insertTopLevelItem(0,item)      ;
  doubleClicked(item,0)           ;
}

void N::Terminologies::Rename(void)
{
  QTreeWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item)        )       ;
  nDropOut ( currentColumn() < 0 )       ;
  doubleClicked(item,currentColumn())    ;
}

void N::Terminologies::Delete(void)
{
  nDropOut ( uuid <= 0    )              ;
  QTreeWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item) )              ;
  SUID  uu = nTreeUuid(item,0)           ;
  UUIDs Uuids                            ;
  Uuids << uu                            ;
  GroupItems GI ( plan )                 ;
  EnterSQL ( SC , plan->sql )            ;
    GI.Detach                            (
      SC                                 ,
      uuid                               ,
      type                               ,
      Types::Terminology                 ,
      relation                           ,
      Uuids                            ) ;
  LeaveSQL ( SC , plan->sql )            ;
  ////////////////////////////////////////
  int index = indexOfTopLevelItem(item)  ;
  if (index<0)                           {
    Alert            ( Error )           ;
  } else                                 {
    takeTopLevelItem ( index )           ;
    Alert ( Done  )                      ;
  }                                      ;
}

void N::Terminologies::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Save terminologies")   ,
                       plan->Temporary("")        ,
                       tr("Text file (*.txt)")  ) ;
  if (filename.length()<=0) return                ;
  QFile F(filename)                               ;
  if (!F.open(QIODevice::WriteOnly)) return       ;
  for (int i=0;i<topLevelItemCount();i++)         {
    QTreeWidgetItem * item  = topLevelItem(i)     ;
    QString name   = item->text(0)                ;
    name += "\n"                                  ;
    F.write(name.toUtf8())                        ;
  }                                               ;
  F.close()                                       ;
  Alert ( Done )                                  ;
}

void N::Terminologies::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                         ;
  switch (column)                          {
    case 0                                 :
      line = setLineEdit                   (
               item,column                 ,
               SIGNAL(editingFinished())   ,
               SLOT  (nameFinished   ()) ) ;
      line->setFocus( Qt::TabFocusReason ) ;
    break                                  ;
  }                                        ;
}

void N::Terminologies::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                    ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)   ;
  if (IsNull(line)) return                           ;
  SUID    uu   = nTreeUuid(ItemEditing,0)            ;
  QString name = line->text()                        ;
  GroupItems    GI ( plan                    )       ;
  EnterSQL         ( SC , plan->sql          )       ;
    if (uu<=0)                                       {
      if (name.length()>0)                           {
        uuid = SC.Unique(PlanTable(MajorUuid)        ,
                         "uuid",6156               ) ;
        SC.assureUuid(PlanTable(MajorUuid),uu        ,
                      Types::Terminology           ) ;
        SC.insertUuid(PlanTable(Terminology),uu,0  ) ;
        assureName(SC,uu,vLanguageId,name)           ;
        if (uuid>0)                                  {
          UUIDs Juids                                ;
          Juids << uu                                ;
          GI.Join                                    (
            SC                                       ,
            uuid                                     ,
            type                                     ,
            Types::Terminology                       ,
            relation                                 ,
            0,Juids                                ) ;
        }                                            ;
        ItemEditing->setData(0,Qt::UserRole,uu)      ;
        ItemEditing->setText(0,name)                 ;
        Alert ( Done  )                              ;
      } else                                         {
        Alert ( Error )                              ;
      }                                              ;
    } else                                           {
      assureName(SC,uu,vLanguageId,name)             ;
      ItemEditing->setText(0,name)                   ;
      Alert        ( Done                    )       ;
    }                                                ;
  LeaveSQL         ( SC   , plan->sql        )       ;
  removeOldItem    ( true , 0                )       ;
}

void N::Terminologies::HeaderMenu(QHeaderView *,QPoint)
{
  nScopedMenu ( mm , this )          ;
  QAction    * aa                    ;
  bool         state                 ;
  aa = mm.add(101,tr("Sorting"))     ;
  aa->setCheckable(true)             ;
  aa->setChecked(isSortingEnabled()) ;
  aa = mm.add(201,tr("Type"))        ;
  aa->setCheckable(true)             ;
  aa->setChecked(!isColumnHidden(1)) ;
  aa = mm.add(202,tr("Flags"))       ;
  aa->setCheckable(true)             ;
  aa->setChecked(!isColumnHidden(2)) ;
  mm.setFont ( plan )                ;
  aa = mm.exec()                     ;
  if (IsNull(aa)) return             ;
  switch (mm[aa])                    {
    case 101                         :
      state = isSortingEnabled()     ;
      state = !state                 ;
      setSortingEnabled ( state )    ;
    break                            ;
    case 201                         :
      state = isColumnHidden(1)      ;
      state = !state                 ;
      setColumnHidden ( 1 , state )  ;
      SuitableColumns (           )  ;
    break                            ;
    case 202                         :
      state = isColumnHidden(2)      ;
      state = !state                 ;
      setColumnHidden ( 2 , state )  ;
      SuitableColumns (           )  ;
    break                            ;
  }                                  ;
}

bool N::Terminologies::Menu(QPoint pos)
{
  nScopedMenu   ( mm , this )                                   ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QTreeWidgetItem * item = itemAt  ( pos                      ) ;
  QAction    * aa                                               ;
  bool         state                                            ;
  aa = mm.add(101,tr("Sorting"))                                ;
  aa->setCheckable(true)                                        ;
  aa->setChecked(isSortingEnabled())                            ;
  aa = mm.add(201,tr("Type"))                                   ;
  aa->setCheckable(true)                                        ;
  aa->setChecked(!isColumnHidden(1))                            ;
  aa = mm.add(202,tr("Flags"))                                  ;
  aa->setCheckable(true)                                        ;
  aa->setChecked(!isColumnHidden(2))                            ;
  mm.add(901,tr("Edit multilingual names"))                     ;
  nSafeExec(item)                                               {
    mm.add(501,tr("Deacon"))                                    ;
    if (uuid>0) mm.add(502,tr("Detach"))                        ;
  }                                                             ;
  nSafeExec(dock) mm.add(1000001,tr("Move to window area"))     ;
  nSafeExec(mdi ) mm.add(1000002,tr("Move to dock area"  ))     ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 101                                                    :
      state = isSortingEnabled()                                ;
      state = !state                                            ;
      setSortingEnabled ( state )                               ;
    break                                                       ;
    case 201                                                    :
      state = isColumnHidden(1)                                 ;
      state = !state                                            ;
      setColumnHidden ( 1 , state )                             ;
      SuitableColumns (           )                             ;
    break                                                       ;
    case 202                                                    :
      state = isColumnHidden(2)                                 ;
      state = !state                                            ;
      setColumnHidden ( 2 , state )                             ;
      SuitableColumns (           )                             ;
    break                                                       ;
    case 501                                                    :
      plan -> Talk ( item->text(0) )                            ;
    break                                                       ;
    case 502                                                    :
      Delete ( )                                                ;
    break                                                       ;
    case 901 :                                                  {
      UUIDs Uuids                                               ;
      for (int i=0;i<topLevelItemCount();i++)                   {
        QTreeWidgetItem * item = topLevelItem(i)                ;
        SUID uu = nTreeUuid(item,0)                             ;
        Uuids << uu                                             ;
      }                                                         ;
      if (Uuids.count()>0)                                      {
        QString title = windowTitle()                           ;
        emit Edit(title,Uuids)                                  ;
      }                                                         ;
    }                                                           ;
    break                                                       ;
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

bool N::Terminologies::dropTerminologies(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED      ( source            ) ;
  Q_UNUSED      ( pos               ) ;
  nKickOut      ( uuid <= 0 , false ) ;
  GroupItems GI ( plan              ) ;
  EnterSQL      ( SC , plan->sql    ) ;
    UUIDs Juids = Uuids               ;
    GI.Join                           (
      SC                              ,
      uuid                            ,
      type                            ,
      Types::Terminology              ,
      relation                        ,
      0,Juids                       ) ;
  LeaveSQL      ( SC , plan->sql    ) ;
  startup       (                   ) ;
  return true                         ;
}

void N::Terminologies::Paste(void)
{
  QString T = nClipboardText   ;
  nDropOut ( T.length() <= 0 ) ;
  QStringList S                ;
  S = T.split("\n")            ;
  S = File::PurifyLines(S)     ;
  nDropOut ( S.count() <= 0  ) ;
  Import   ( S               ) ;
}

void N::Terminologies::Import(void)
{
  QString filename                         ;
  filename = QFileDialog::getOpenFileName  (
              this                         ,
              tr("Import terminologies")   ,
              plan->Temporary("")          ,
              "*.txt\n*.*"               ) ;
  nDropOut ( filename.length() <= 0 )      ;
  QStringList S                            ;
  S = File::StringList(filename,"\n")      ;
  S = File::PurifyLines(S)                 ;
  nDropOut ( S.count() <= 0              ) ;
  Import ( S )                             ;
}

void N::Terminologies::Import(QStringList terms)
{
  GroupItems  GI ( plan           )                ;
  EnterSQL       ( SC , plan->sql )                ;
    UUIDs   Uuids                                  ;
    SUID    uu                                     ;
    for (int i=0;i<terms.count();i++)              {
      QString name = terms[i]                      ;
      if (name.length()>0)                         {
        uu = SC.Unique(PlanTable(MajorUuid)        ,
                         "uuid",6156             ) ;
        SC.assureUuid(PlanTable(MajorUuid),uu      ,
                      Types::Terminology         ) ;
        SC.insertUuid(PlanTable(Terminology),uu,0) ;
        assureName(SC,uu,vLanguageId,name)         ;
        if (uuid>0) Uuids << uu                    ;
      }                                            ;
    }                                              ;
    if (uuid>0)                                    {
      GI.Join                                      (
        SC                                         ,
        uuid                                       ,
        type                                       ,
        Types::Terminology                         ,
        relation                                   ,
        0,Uuids                                  ) ;
    }                                              ;
  LeaveSQL       ( SC , plan->sql )                ;
  startup        (                )                ;
}
