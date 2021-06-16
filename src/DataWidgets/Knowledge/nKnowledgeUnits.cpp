#include <datawidgets.h>

//#define USENHEADER 1

N::KnowledgeUnits:: KnowledgeUnits   ( QWidget * parent , Plan * p )
                  : TreeDock         (           parent ,        p )
                  , KnowledgeManager (                           p )
                  , Object           ( 0 , Types  :: Field         )
                  , Relation         ( 0 , Groups :: Subordination )
                  , dropAction       ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::KnowledgeUnits::~KnowledgeUnits(void)
{
}

QMimeData * N::KnowledgeUnits::dragMime (void)
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
    setMime ( mime,"knowledge/uuid" ,uuid  )        ;
  } else                                            {
    setMime ( mime,"knowledge/uuids",Uuids )        ;
  }                                                 ;
  ///////////////////////////////////////////////////
  QImage image = QImage(":/images/knowunit.png")    ;
  mime -> setImageData(image)                       ;
  return mime                                       ;
}

bool N::KnowledgeUnits::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::KnowledgeUnits::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::KnowledgeUnits::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::KnowledgeUnits::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::KnowledgeUnits::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::KnowledgeUnits::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::KnowledgeUnits::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::KnowledgeUnits::Configure(void)
{
#ifdef USENHEADER
  installHeader                (                                ) ;
#endif
  NewTreeWidgetItem            ( head                           ) ;
  head -> setText              ( 0 , tr("Name")                 ) ;
  setWindowTitle               ( tr("Knowledge units")          ) ;
  setWindowIcon                ( QIcon(":/images/knowunit.png") ) ;
  setDragDropMode              ( DragDrop                       ) ;
  setRootIsDecorated           ( false                          ) ;
  setAlternatingRowColors      ( true                           ) ;
  setSelectionMode             ( ExtendedSelection              ) ;
  setColumnCount               ( 1                              ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded          ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded          ) ;
  assignHeaderItems            ( head                           ) ;
  MountClicked                 ( 2                              ) ;
  plan -> setFont              ( this                           ) ;
  /////////////////////////////////////////////////////////////////
  setDropFlag                  ( DropFont        , true         ) ;
  setDropFlag                  ( DropPen         , true         ) ;
  setDropFlag                  ( DropBrush       , true         ) ;
  setDropFlag                  ( DropTerminology , true         ) ;
  setDropFlag                  ( DropKnowledge   , true         ) ;
}

bool N::KnowledgeUnits::FocusIn(void)
{
  LinkAction   ( Refresh  , startup         () ) ;
  LinkAction   ( Insert   , Insert          () ) ;
  LinkAction   ( Copy     , CopyToClipboard () ) ;
  LinkAction   ( Font     , setFont         () ) ;
  LinkAction   ( Export   , Export          () ) ;
  if (uuid>0)                                    {
    LinkAction ( Delete   , Delete          () ) ;
  }                                              ;
  return true                                    ;
}

bool N::KnowledgeUnits::startup(void)
{
  clear            (                          )     ;
  GroupItems    GI ( plan                     )     ;
  EnterSQL         ( SC , plan->sql           )     ;
    QString         Q                               ;
    SUID            uu                              ;
    UUIDs           Uuids                           ;
    if (uuid>0)                                     {
      Uuids = GI.Subordination                      (
                SC                                  ,
                uuid                                ,
                type                                ,
                Types :: Knowledge                  ,
                linkType                            ,
                "order by position asc"           ) ;
    } else                                          {
      Q = SC.sql.SelectFrom("uuid"                  ,
                            PlanTable(Knowledge)    ,
                            "order by id desc"    ) ;
      SqlLoopNow(SC,Q)                              ;
        uu = SC.Uuid(0)                             ;
        Uuids << uu                                 ;
      SqlLoopErr(SC,Q)                              ;
      SqlLoopEnd(SC,Q)                              ;
    }                                               ;
    foreach (uu,Uuids)                              {
      NewTreeWidgetItem(item)                       ;
      QString name = Name(SC,uu,vLanguageId   )     ;
      item->setData(0,Qt::UserRole,uu         )     ;
      item->setText(0,name                    )     ;
      addTopLevelItem(item)                         ;
    }                                               ;
  LeaveSQL         ( SC , plan->sql           )     ;
  SuitableColumns  (                          )     ;
  reportItems      (                          )     ;
  Alert            ( Done                     )     ;
  return true                                       ;
}

void N::KnowledgeUnits::Insert(void)
{
  NewTreeWidgetItem(item)         ;
  item->setData(0,Qt::UserRole,0) ;
  insertTopLevelItem(0,item)      ;
  doubleClicked(item,0)           ;
}

void N::KnowledgeUnits::Rename(void)
{
  QTreeWidgetItem * item = currentItem() ;
  nDropOut ( IsNull(item)        )       ;
  nDropOut ( currentColumn() < 0 )       ;
  doubleClicked(item,currentColumn())    ;
}

void N::KnowledgeUnits::Delete(void)
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
      Types :: Knowledge                 ,
      linkType                           ,
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

void N::KnowledgeUnits::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Save knowledge units") ,
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

void N::KnowledgeUnits::doubleClicked(QTreeWidgetItem * item,int column)
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

void N::KnowledgeUnits::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                    ;
  QLineEdit * line = Casting(QLineEdit,ItemWidget)   ;
  if (IsNull(line)) return                           ;
  SUID    uu   = nTreeUuid(ItemEditing,0)            ;
  QString name = line->text()                        ;
  GroupItems GI    ( plan                    )       ;
  EnterSQL         ( SC , plan->sql          )       ;
    if (uu<=0)                                       {
      if (name.length()>0)                           {
        uu = SC.Unique(PlanTable(MajorUuid)          ,
                         "uuid",6164               ) ;
        SC.assureUuid(PlanTable(MajorUuid),uu        ,
                      Types::Knowledge             ) ;
        SC.insertUuid(PlanTable(Knowledge)           ,
                      uu,"uuid"                    ) ;
        assureName(SC,uu,vLanguageId,name          ) ;
        if (uuid>0)                                  {
          UUIDs Juids                                ;
          Juids << uu                                ;
          GI.Join                                    (
            SC                                       ,
            uuid                                     ,
            type                                     ,
            Types :: Knowledge                       ,
            linkType                                 ,
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

void N::KnowledgeUnits::HeaderMenu(QHeaderView *,QPoint)
{
}

bool N::KnowledgeUnits::Menu(QPoint pos)
{
  nScopedMenu   ( mm , this )                                   ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  QTreeWidgetItem * item = itemAt  ( pos                      ) ;
  QAction    * aa                                               ;
  mm.add(901,tr("Edit multilingual names"))                     ;
  if (NotNull(item))                                            {
    mm.add(501,tr("Deacon"))                                    ;
    if (uuid>0) mm.add(502,tr("Detach"))                        ;
    mm.add(503,tr("Terminologies"))                             ;
  }                                                             ;
  nSafeExec(dock) mm.add(1000001,tr("Move to window area"))     ;
  nSafeExec(mdi ) mm.add(1000002,tr("Move to dock area"  ))     ;
  mm.setFont ( plan    )                                        ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 501                                                    :
      plan -> Talk ( item->text(0) )                            ;
    break                                                       ;
    case 502                                                    :
      Delete ( )                                                ;
    break                                                       ;
    case 503                                                    :
      emit Terminology                                          (
             this                                               ,
             nTreeUuid(item,0)                                  ,
             Types::Knowledge                         ,
             item->text(0)                                    ) ;
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

bool N::KnowledgeUnits::dropTerminologies(QWidget * source,QPointF psf,const UUIDs & Uuids)
{
  QPoint pos = psf.toPoint()           ;
  QTreeWidgetItem * item               ;
  item = itemAt(pos)                   ;
  nKickOut ( IsNull(item) , true     ) ;
  SUID kuid = nTreeUuid ( item , 0   ) ;
  GroupItems  GI ( plan              ) ;
  EnterSQL       ( SC , plan->sql    ) ;
    UUIDs Juids = Uuids                ;
    GI.Join                            (
      SC                               ,
      kuid                             ,
      Types  :: Knowledge              ,
      Types  :: Terminology            ,
      Groups :: Subordination          ,
      0,Juids                        ) ;
  LeaveSQL       ( SC , plan->sql    ) ;
  Alert          ( Done              ) ;
  return true                          ;
}

bool N::KnowledgeUnits::dropKnowledge(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                  ;
  Q_UNUSED ( pos    )                  ;
  nKickOut       ( uuid <= 0 , false ) ;
  GroupItems  GI ( plan              ) ;
  EnterSQL       ( SC , plan->sql    ) ;
    UUIDs Juids = Uuids                ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      type                             ,
      Types :: Knowledge               ,
      linkType                         ,
      0,Juids                        ) ;
  LeaveSQL       ( SC , plan->sql    ) ;
  startup        (                   ) ;
  return true                          ;
}
