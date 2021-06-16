#include <datawidgets.h>

N::KeywordLists:: KeywordLists    ( QWidget * parent , Plan * p )
                : TreeDock        (           parent ,        p )
                , DocumentManager (                           p )
                , Object          ( 0 , Types::Division         )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::KeywordLists::~KeywordLists(void)
{
}

QMimeData * N::KeywordLists::dragMime (void)
{
  QMimeData * mime = standardMime("keyword") ;
  nKickOut ( IsNull(mime) , NULL )           ;
  return mime                                ;
}

bool N::KeywordLists::hasItem(void)
{
  UUIDs Uuids = selectedUuids(0) ;
  return ( Uuids.count() > 0 )   ;
}

bool N::KeywordLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::KeywordLists::fetchDrag(QMouseEvent * event)
{
  nKickOut ( !IsMask(event->buttons(),Qt::LeftButton) , false) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::KeywordLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::KeywordLists::finishDrag(QMouseEvent * event)
{
  return true ;
}

bool N::KeywordLists::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual(source,this) , false ) ;
  return dropHandler(mime)                 ;
}

bool N::KeywordLists::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

void N::KeywordLists::Configure(void)
{
  setWindowTitle           ( tr("Keyword lists"  ) ) ;
  NewTreeWidgetItem        ( head                  ) ;
  head->setText            ( 0 , tr("Keyword"    ) ) ;
  head->setText            ( 1 , tr("Language"   ) ) ;
  head->setText            ( 2 , tr("Phonogram"  ) ) ;
  head->setText            ( 3 , tr("Vocal"      ) ) ;
  head->setText            ( 4 , ""                ) ;
  ////////////////////////////////////////////////////
  setFocusPolicy           ( Qt::WheelFocus        ) ;
  setDragDropMode          ( DragDrop              ) ;
  setAlternatingRowColors  ( true                  ) ;
  setRootIsDecorated       ( false                 ) ;
  setSelectionMode         ( ExtendedSelection     ) ;
  setColumnCount           ( 5                     ) ;
  assignHeaderItems        ( head                  ) ;
  MountClicked             ( 2                     ) ;
  plan -> setFont          ( this                  ) ;
  ////////////////////////////////////////////////////
  setDropFlag              ( DropFont    , true    ) ;
  setDropFlag              ( DropPen     , true    ) ;
  setDropFlag              ( DropBrush   , true    ) ;
  setDropFlag              ( DropKeyword , true    ) ;
  ////////////////////////////////////////////////////
  PassDragDrop = false                               ;
  Language     = vLanguageId                         ;
}

bool N::KeywordLists::FocusIn(void)
{
  AssignAction ( Label      , windowTitle     ( ) ) ;
  LinkAction   ( Refresh    , startup         ( ) ) ;
  LinkAction   ( Insert     , New             ( ) ) ;
  LinkAction   ( Delete     , Delete          ( ) ) ;
  LinkAction   ( Paste      , Paste           ( ) ) ;
  LinkAction   ( Export     , Export          ( ) ) ;
  LinkAction   ( Copy       , CopyToClipboard ( ) ) ;
  LinkAction   ( SelectNone , SelectNone      ( ) ) ;
  LinkAction   ( SelectAll  , SelectAll       ( ) ) ;
  LinkAction   ( Font       , setFont         ( ) ) ;
  LinkAction   ( Properties , setProperties   ( ) ) ;
  return true                                       ;
}

void N::KeywordLists::showKeyword(SqlConnection & SC,SUID uuid)
{
  QString Q                                    ;
  UUIDs   Nuids                                ;
  SUID    nuid                                 ;
  Nuids . clear ( )                            ;
  Q = SC.sql.SelectFrom                        (
        "name"                                 ,
        PlanTable(KeywordNames)                ,
        QString("where uuid = %1 "
                "order by id asc"              )
        .arg(uuid)                           ) ;
  SqlLoopNow(SC,Q)                             ;
    nuid   = SC.Uuid(0)                        ;
    Nuids << nuid                              ;
  SqlLoopErr(SC,Q)                             ;
  SqlLoopEnd(SC,Q)                             ;
  if (Nuids.count()>0)                         {
    foreach (nuid,Nuids)                       {
      Q = SC.sql.SelectFrom                    (
            "language,name"                    ,
            PlanTable(Names)                   ,
            SC.WhereUuid(nuid)               ) ;
      if (SC.Fetch(Q))                         {
        int     lang = SC . Int    ( 0 )       ;
        QString name = SC . String ( 1 )       ;
        QString lame = (plan->languages)[lang] ;
        QString tk   = QString::number(uuid)   ;
        QString tn   = QString::number(nuid)   ;
        NewTreeWidgetItem(item)                ;
        item->setData(0,Qt::UserRole,uuid)     ;
        item->setData(1,Qt::UserRole,lang)     ;
        item->setData(2,Qt::UserRole,nuid)     ;
        item->setText(0,name             )     ;
        item->setText(1,lame             )     ;
        item->setToolTip(0,tk            )     ;
        item->setToolTip(1,tn            )     ;
        addTopLevelItem ( item )               ;
      }                                        ;
    }                                          ;
  }                                            ;
}

void N::KeywordLists::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( Type )   {
    case 10001      :
      List ( )      ;
    break           ;
  }                 ;
}

bool N::KeywordLists::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::KeywordLists::List(void)
{
  nDropOut      ( uuid <= 0       )                ;
  setEnabled    ( false           )                ;
  GroupItems GI ( plan            )                ;
  EnterSQL      ( SC , plan->sql  )                ;
    QString Q                                      ;
    UUIDs   Uuids                                  ;
    SUID    uu                                     ;
    Q = SC.sql.SelectFrom                          (
          "value" , PlanTable(Variables)           ,
          "where uuid = :UUID and name = :NAME" )  ;
    SC . Prepare ( Q                   )           ;
    SC . Bind    ( "uuid" , uuid       )           ;
    SC . Bind    ( "name" , "Language" )           ;
    if (SC.Exec() && SC.Next())                    {
      QString ln = SC . String ( 0 )               ;
      Language   = ln . toInt  (   )               ;
    } else                                         {
      QString ln = QString::number(Language)       ;
      Q = SC.sql.InsertInto                        (
            PlanTable(Variables)                   ,
            3                                      ,
            "uuid"                                 ,
            "name"                                 ,
            "value"                              ) ;
      SC . Prepare ( Q                     )       ;
      SC . Bind    ( "uuid"  , uuid        )       ;
      SC . Bind    ( "name"  , "Language"  )       ;
      SC . Bind    ( "value" , ln.toUtf8() )       ;
      SC . Exec    (                       )       ;
    }                                              ;
    ////////////////////////////////////////////////
    Uuids = GI.Subordination                       (
              SC                                   ,
              uuid                                 ,
              type                                 ,
              Types  :: Keyword                    ,
              Groups :: Subordination              ,
              "order by position asc"            ) ;
    foreach (uu,Uuids) showKeyword(SC,uu)          ;
  LeaveSQL        ( SC , plan->sql )               ;
  setEnabled      ( true           )               ;
  reportItems     (                )               ;
  emit AutoFit    (                )               ;
  Alert           ( Done           )               ;
}

bool N::KeywordLists::startup(UUIDs & Uuids)
{
  clear           (                       ) ;
  setEnabled      ( false                 ) ;
  EnterSQL        ( SC , plan->sql        ) ;
    SUID uu                                 ;
    foreach ( uu , Uuids )                  {
      showKeyword ( SC , uu               ) ;
    }                                       ;
  LeaveSQL        ( SC , plan->sql        ) ;
  setEnabled      ( true                  ) ;
  reportItems     (                       ) ;
  emit AutoFit    (                       ) ;
  Alert           ( Done                  ) ;
  return true                               ;
}

void N::KeywordLists::doubleClicked(QTreeWidgetItem * item,int column)
{
  nDropOut ( column > 0 )              ;
  QLineEdit * line                     ;
  removeOldItem()                      ;
  line = setLineEdit                   (
           item                        ,
           column                      ,
           SIGNAL(editingFinished())   ,
           SLOT  (editingFinished()) ) ;
  line->setFocus(Qt::TabFocusReason)   ;
}

void N::KeywordLists::New(void)
{
  NewTreeWidgetItem ( IT )      ;
  IT->setData(0,Qt::UserRole,0) ;
  IT->setData(2,Qt::UserRole,0) ;
  setAlignments   ( IT     )    ;
  addTopLevelItem ( IT     )    ;
  scrollToItem    ( IT     )    ;
  doubleClicked   ( IT , 0 )    ;
}

void N::KeywordLists::editingFinished(void)
{
  nDropOut ( IsNull(ItemEditing) )                     ;
  QLineEdit * line = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line)) return                             ;
  ItemEditing -> setText ( ItemColumn , line->text() ) ;
  //////////////////////////////////////////////////////
  UUIDs   Uuids  = itemUuids  ( 0 )                    ;
  SUID    uu     = nTreeUuid(ItemEditing,0)            ;
  SUID    nuid   = nTreeUuid(ItemEditing,2)            ;
  QString name   = line->text (   )                    ;
  GroupItems GI ( plan )                               ;
  EnterSQL ( SC , plan->sql )                          ;
    if (uu<=0)                                         {
      QTreeWidgetItem * item = ItemEditing             ;
      uu = assureKeyword(SC,Language,name)             ;
      removeOldItem ( )                                ;
      takeItem(item)                                   ;
      if (uu>0 && !Uuids.contains(uu))                 {
        UUIDs Kuids                                    ;
        showKeyword(SC,uu)                             ;
        Kuids << uu                                    ;
        GI.Join                                        (
          SC                                           ,
          uuid                                         ,
          type                                         ,
          Types  :: Keyword                            ,
          Groups :: Subordination                      ,
          0,Kuids                                    ) ;
      }                                                ;
    } else                                             {
      int l = nTreeInt(ItemEditing,1)                  ;
      SC.assureName(PlanTable(Names),nuid,l,name)      ;
      removeOldItem ( )                                ;
    }                                                  ;
  LeaveSQL ( SC , plan->sql )                          ;
  Alert    ( Done           )                          ;
}

void N::KeywordLists::Delete(void)
{
  UUIDs Uuids = selectedUuids(0)             ;
  nDropOut ( Uuids.count() <= 0 )            ;
  GroupItems GI ( plan )                     ;
  EnterSQL ( SC , plan->sql )                ;
    TreeWidgetItems Items = selectedItems()  ;
    for (int i=0;i<Items.count();i++)        {
      QTreeWidgetItem * item = Items[i]      ;
      takeItem(item)                         ;
    }                                        ;
    GI.Detach                                (
      SC                                     ,
      uuid                                   ,
      type                                   ,
      Types  :: Keyword                      ,
      Groups :: Subordination                ,
      Uuids                                ) ;
  LeaveSQL ( SC , plan->sql )                ;
  Alert    ( Done           )                ;
}

void N::KeywordLists::Paste(QString text)
{
  nDropOut ( text.length() <= 0 )           ;
  QStringList sl    = text.split("\n")      ;
  UUIDs       Uuids = itemUuids  ( 0 )      ;
  GroupItems GI ( plan )                    ;
  EnterSQL ( SC , plan->sql )               ;
    UUIDs Kuids                             ;
    for (int i=0;i<sl.count();i++)          {
      QString s = sl[i]                     ;
      s = s.replace("\r","")                ;
      s = s.replace("\n","")                ;
      if (s.length()>0)                     {
        SUID uu                             ;
        uu = assureKeyword(SC,Language,s)   ;
        if (!Uuids.contains(uuid))          {
          showKeyword(SC,uu)                ;
          Kuids << uu                       ;
          Uuids << uu                       ;
        }                                   ;
      }                                     ;
    }                                       ;
    GI.Join                                 (
      SC                                    ,
      uuid                                  ,
      type                                  ,
      Types  :: Keyword                     ,
      Groups :: Subordination               ,
      0,Kuids                             ) ;
  LeaveSQL ( SC , Plan->Sql )               ;
  Alert    ( Done           )               ;
}

void N::KeywordLists::Paste(void)
{
  Paste ( nClipboardText ) ;
}

void N::KeywordLists::Export(void)
{
  QString filename = QFileDialog::getSaveFileName  (
                       this                        ,
                       tr("Export keywords"      ) ,
                       plan->Temporary("")         ,
                       "*.*\n"
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

void N::KeywordLists::Copy(void)
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

void N::KeywordLists::setProperties(void)
{
  TextProperties * NTP = new TextProperties(this,plan)   ;
  NTP->setLanguage(Language)                             ;
  NTP->setHidden  (1       )                             ;
  if ( NTP->exec() != QDialog::Accepted )                {
    NTP -> deleteLater ( )                               ;
    return                                               ;
  }                                                      ;
  Language = NTP->Language  (   )                        ;
  NTP -> deleteLater ( )                                 ;
  ////////////////////////////////////////////////////////
  EnterSQL ( SC , plan->sql )                            ;
    QString Q                                            ;
    QString ln = QString::number(Language)               ;
    Q = SC.sql.Update                                    (
          PlanTable(Variables)                           ,
          "where uuid = :UUID and name = :NAME"          ,
          1                                              ,
          "value"                                      ) ;
    SC . Prepare ( Q                     )               ;
    SC . Bind    ( "uuid"  , uuid        )               ;
    SC . Bind    ( "name"  , "Language"  )               ;
    SC . Bind    ( "value" , ln.toUtf8() )               ;
    SC . Exec    (                       )               ;
  LeaveSQL ( SC , plan->sql )                            ;
  Alert    ( Done           )                            ;
}

bool N::KeywordLists::dropKeywords(QWidget * source,QPointF pos,const UUIDs & Uuids)
{
  UUIDs Muids = itemUuids(0)           ;
  dropAction = true                    ;
  GroupItems GI ( plan )               ;
  EnterSQL ( SC , plan->sql )          ;
    UUIDs Kuids                        ;
    SUID  uu                           ;
    foreach (uu,Uuids)                 {
      if (!Muids.contains(uu))         {
        showKeyword(SC,uu)             ;
        Muids << uu                    ;
        Kuids << uu                    ;
      }                                ;
    }                                  ;
    GI.Join                            (
      SC                               ,
      uuid                             ,
      type                             ,
      Types  :: Keyword                ,
      Groups :: Subordination          ,
      0,Kuids                        ) ;
  LeaveSQL ( SC , plan->sql )          ;
  Alert    ( Done           )          ;
  dropAction = false                   ;
  return true                          ;
}

bool N::KeywordLists::ListKeywordName(SqlConnection & SC,SUID uu,QByteArray & Body)
{
  QString Q                                      ;
  QString m                                      ;
  UUIDs Uuids                                    ;
  SUID  nuid                                     ;
  Q = SC.sql.SelectFrom                          (
        "name"                                   ,
        PlanTable(KeywordNames)                  ,
        QString("where uuid = %1 order by id asc")
        .arg(uu)                               ) ;
  SqlLoopNow(SC,Q)                               ;
    Uuids << SC.Uuid(0)                          ;
  SqlLoopErr(SC,Q)                               ;
  SqlLoopEnd(SC,Q)                               ;
  if (Uuids.count()<=0) return false             ;
  QStringList L                                  ;
  foreach (nuid,Uuids)                           {
    QString N = ""                               ;
    Q = SC.sql.SelectFrom                        (
          "name,language"                        ,
          PlanTable(Names)                       ,
          SC.WhereUuid(nuid)                   ) ;
    if (SC.Fetch(Q))                             {
      QString name = SC . String (0)             ;
      int     lang = SC . Int    (1)             ;
      N = tr("%1 [%2]"                           )
              .arg(name                          )
              .arg(plan->languages[lang]       ) ;
    }                                            ;
    if (N.length()>0) L << N                     ;
  }                                              ;
  if (L.count()<=0) return false                 ;
  m = L.join("\n")                               ;
  Body.append(m.toUtf8())                        ;
  return true                                    ;
}

bool N::KeywordLists::ListDictionary(SqlConnection & SC,SUID uu,QByteArray & Body)
{
  DocumentManager DM(plan)                                      ;
  QString Q                                                     ;
  QString m                                                     ;
  UUIDs   Uuids                                                 ;
  SMAPs   LMaps                                                 ;
  SUID    puid                                                  ;
  ListKeywordName(SC,uu,Body)                                   ;
  m = tr("\n\n")                                                ;
  Body.append(m.toUtf8())                                       ;
  ///////////////////////////////////////////////////////////////
  Uuids . clear ( )                                             ;
  LMaps . clear ( )                                             ;
  Q = SC.sql.SelectFrom("second,third",PlanTable(Triples)       ,
        QString("where first = %1 and type = %2 order by id asc")
        .arg(uu).arg(1001)                                    ) ;
  SqlLoopNow ( SC , Q )                                         ;
    puid        = SC.Uuid(0)                                    ;
    Uuids << puid                                               ;
    LMaps[puid] = SC.Uuid(1)                                    ;
  SqlLoopErr ( SC , Q )                                         ;
  SqlLoopEnd ( SC , Q )                                         ;
  ///////////////////////////////////////////////////////////////
  if (Uuids.count()>0)                                          {
    m = tr("[Pronouncation]\n")                                 ;
    Body.append(m.toUtf8())                                     ;
    foreach (puid,Uuids)                                        {
      SUID       luid = LMaps                   [ puid ]        ;
      int        lang = plan->languages.UuidIDs [ luid ]        ;
      QString    lnam = plan->languages         [ lang ]        ;
      QByteArray sect = DM.PlainText(SC,puid)                   ;
      if (sect.size()>0)                                        {
        m = tr("[%1] ").arg(lnam)                               ;
        Body.append(m.toUtf8())                                 ;
        Body.append(sect      )                                 ;
        Body.append("\n"      )                                 ;
      }                                                         ;
    }                                                           ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  Uuids . clear ( )                                             ;
  LMaps . clear ( )                                             ;
  Q = SC.sql.SelectFrom("second,third",PlanTable(Triples)       ,
        QString("where first = %1 and type = %2 order by id asc")
        .arg(uuid).arg(Groups::Explain)                       ) ;
  SqlLoopNow ( SC , Q )                                         ;
    puid        = SC.Uuid(0)                                    ;
    Uuids << puid                                               ;
    LMaps[puid] = SC.Uuid(1)                                    ;
  SqlLoopErr ( SC , Q )                                         ;
  SqlLoopEnd ( SC , Q )                                         ;
  ///////////////////////////////////////////////////////////////
  if (Uuids.count()>0)                                          {
    m = tr("[Explanation]\n")                                   ;
    Body.append("\n"      )                                     ;
    Body.append(m.toUtf8())                                     ;
    Body.append("\n"      )                                     ;
    foreach (puid,Uuids)                                        {
      SUID       luid = LMaps                   [ puid ]        ;
      int        lang = plan->languages.UuidIDs [ luid ]        ;
      QString    lnam = plan->languages         [ lang ]        ;
      QByteArray sect = DM.PlainText(SC,puid)                   ;
      if (sect.size()>0)                                        {
        m = tr("[%1]\n").arg(lnam)                              ;
        Body.append(m.toUtf8())                                 ;
        Body.append(sect      )                                 ;
        Body.append("\n"      )                                 ;
        Body.append("\n"      )                                 ;
      }                                                         ;
    }                                                           ;
  }                                                             ;
  return true                                                   ;
}

void N::KeywordLists::Explain(SUID uuid)
{
}

bool N::KeywordLists::Menu(QPoint pos)
{
  nScopedMenu     ( mm , this )                 ;
  QTreeWidgetItem * item  = itemAt  ( pos     ) ;
  QAction         * aa                          ;
  mm . add ( 101 , tr("Refresh"              )) ;
  if (NotNull(item))                            {
    mm.add ( 701 , tr("Assign vocal patterns")) ;
    mm.add ( 702 , tr("Deacon"               )) ;
    mm.add ( 703 , tr("Explain"              )) ;
  }                                             ;
  mm . addSeparator (                         ) ;
  mm . add ( 501 , tr("Select all"           )) ;
  mm . add ( 502 , tr("Select none"          )) ;
  mm . add ( 503 , tr("Set font"             )) ;
  mm . add ( 504 , tr("Copy selections"      )) ;
  mm . add ( 505 , tr("Copy"                 )) ;
  mm . addSeparator (                         ) ;
  DockingMenu ( mm )                            ;
  mm . setFont(plan)                            ;
  aa = mm.exec()                                ;
  if (IsNull(aa)) return true                   ;
  switch (mm[aa])                               {
    case 101                                    :
      startup         ( )                       ;
    break                                       ;
    case 201                                    :
      New             ( )                       ;
    break                                       ;
    case 301                                    :
      Export          ( )                       ;
    break                                       ;
    case 501                                    :
      SelectAll       ( )                       ;
    break                                       ;
    case 502                                    :
      SelectNone      ( )                       ;
    break                                       ;
    case 503                                    :
      setFont         ( )                       ;
    break                                       ;
    case 504                                    :
      Copy            ( )                       ;
    break                                       ;
    case 505                                    :
      CopyToClipboard ( )                       ;
    break                                       ;
    case 702                                    :
      plan -> Talk ( 30 , item->text(0) )       ;
    break                                       ;
    case 703                                    :
      Explain ( nTreeUuid(item,0) )             ;
    break                                       ;
    default                                     :
      if (RunDocking(mm,aa)) return true        ;
    break                                       ;
  }                                             ;
  return true                                   ;
}
