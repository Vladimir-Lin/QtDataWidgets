#include <datawidgets.h>

#pragma message("DocumentItems requires some additional working")

N::DocumentItems:: DocumentItems   ( QWidget * parent , Plan * p )
                 : TreeDock        (           parent ,        p )
                 , DocumentManager (                           p )
                 , ProgressTimer   ( new QTimer ( this )         )
                 , Division        ( 0                           )
                 , SourceType      ( Types::Division             )
                 , Sorting         ( Qt::DescendingOrder         )
                 , Editor          ( NULL                        )
                 , Progress        ( NULL                        )
                 , singleItem      ( NULL                        )
                 , singleColumn    ( -1                          )
                 , ProgressValue   ( 0                           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::DocumentItems::~DocumentItems (void)
{
}

void N::DocumentItems::Configure(void)
{
  NewTreeWidgetItem            ( head                                ) ;
  setWindowIcon                ( QIcon(":/images/documentitems.png") ) ;
  plan->setFont                ( this                                ) ;
  setAccessibleName            ( "N::DocumentItems"                  ) ;
  setObjectName                ( "N::DocumentItems"                  ) ;
  setDragDropMode              ( DragDrop                            ) ;
  setRootIsDecorated           ( true                                ) ;
  setAlternatingRowColors      ( true                                ) ;
  setSelectionMode             ( ExtendedSelection                   ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded               ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded               ) ;
  setColumnCount               ( 5                                   ) ;
  head -> setText              ( 0 , tr("Name"    )                  ) ;
  head -> setText              ( 1 , tr("Rating"  )                  ) ;
  head -> setText              ( 2 , tr("Size"    )                  ) ;
  head -> setText              ( 3 , tr("Encoding")                  ) ;
  head -> setText              ( 4 , ""                              ) ;
  assignHeaderItems            ( head                                ) ;
  setDropFlag                  ( DropDocument , true                 ) ;
  MountClicked                 ( 1                                   ) ;
  MountClicked                 ( 2                                   ) ;
  //////////////////////////////////////////////////////////////////////
  singleTime = QDateTime::currentDateTime ( )                          ;
  //////////////////////////////////////////////////////////////////////
  addConnector  ( "Append"                                             ,
                  SIGNAL ( AppendItems     ( ) )                       ,
                  SLOT   ( AppendTopItems  ( ) )                     ) ;
  addConnector  ( "Timer"                                              ,
                  ProgressTimer , SIGNAL ( timeout         ( ) )       ,
                  this          , SLOT   ( ProgressChanged ( ) )     ) ;
  //////////////////////////////////////////////////////////////////////
  onlyConnector ( "Append"                                           ) ;
  onlyConnector ( "Timer"                                            ) ;
}

bool N::DocumentItems::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Insert       , Add             ( ) ) ;
  LinkAction        ( Refresh      , List            ( ) ) ;
  LinkAction        ( Rename       , Rename          ( ) ) ;
  LinkAction        ( Search       , Search          ( ) ) ;
  LinkAction        ( Delete       , Remove          ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( SaveAs       , SaveAs          ( ) ) ;
  LinkAction        ( Export       , Export          ( ) ) ;
  LinkAction        ( Import       , Import          ( ) ) ;
  Alert             ( Action                             ) ;
  return true                                              ;
}

bool N::DocumentItems::FocusOut(void)
{
  return true ;
}

void N::DocumentItems::setItemIcon(QTreeWidgetItem * item,int column,int extension)
{
  switch ( extension )                                             {
    case   0                                                       : // Neutrin Organized Complex Document Format
      item -> setIcon ( column , QIcon(":/images/organize.png")  ) ;
    break                                                          ;
    case 125                                                       : // doc
    case 126                                                       : // docm
    case 127                                                       : // docx
      item -> setIcon ( column , QIcon(":/images/word.png")      ) ;
    break                                                          ;
    case 156                                                       : // eml
      item -> setIcon ( column , QIcon(":/images/eml.png")       ) ;
    break                                                          ;
    case 240                                                       : // htm
    case 241                                                       : // html
      item -> setIcon ( column , QIcon(":/images/html.png")      ) ;
    break                                                          ;
    case 443                                                       : // odb
    case 444                                                       : // odc
    case 445                                                       : // odf
    case 446                                                       : // odg
    case 447                                                       : // odi
    case 448                                                       : // odm
    case 449                                                       : // odp
    case 450                                                       : // ods
    break                                                          ;
    case 451                                                       : // odt
      item -> setIcon ( column , QIcon(":/images/odt.png")       ) ;
    break                                                          ;
    case 483                                                       : // pdf
      item -> setIcon ( column , QIcon(":/images/pdf.png")       ) ;
    break                                                          ;
    case 737                                                       : // text
      item -> setIcon ( column , QIcon(":/images/plaintext.png") ) ;
    break                                                          ;
    case 838                                                       : // xml
      item -> setIcon ( column , QIcon(":/images/xml.png")       ) ;
    break                                                          ;
  }                                                                ;
}

QMimeData * N::DocumentItems::dragMime (void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->isSelected())                        {
      Uuids << nTreeUuid(it,0)                   ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  if (Uuids.count()<=0 && uuid == 0) return NULL ;
  if (Uuids.count()==1)                          {
    uuid  = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "document/uuid"  , uuid  )  ;
  } else                                         {
    setMime ( mime , "document/uuids" , Uuids )  ;
  }                                              ;
  return mime                                    ;
}

bool N::DocumentItems::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::DocumentItems::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())           ;
  nKickOut ( IsNull(atItem) , false )                       ;
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton),false) ;
  dragPoint = event->pos()                                  ;
  nKickOut (!atItem->isSelected(),false)                    ;
  nKickOut (!PassDragDrop,true)                             ;
  return true                                               ;
}

bool N::DocumentItems::fetchDrag(QMouseEvent * event)
{
  nKickOut (!IsMask(event->buttons(),Qt::LeftButton) , false ) ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::DocumentItems::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::DocumentItems::finishDrag(QMouseEvent * event)
{
  return true   ;
}

bool N::DocumentItems::startup(SUID division)
{
  Division = division ;
  List ( )            ;
  return true         ;
}

bool N::DocumentItems::startup(UUIDs & Uuids)
{
  Division = 0       ;
  ExtraUuids = Uuids ;
  List ( )           ;
  return true        ;
}

void N::DocumentItems::List(void)
{
  Progress      = plan -> Progress(tr("%v/%m documents")) ;
  ProgressValue = 0                                       ;
  Progress -> hide       (       )                        ;
  emit assignEnabling    ( false )                        ;
  emit clearItems        (       )                        ;
  start                  (       )                        ;
  ProgressTimer -> start ( 1000  )                        ;
}

void N::DocumentItems::AppendTopItems(void)
{
  SUID duid                                         ;
  ProgressTimer -> stop (      )                    ;
  emit clearItems       (      )                    ;
  foreach (duid,documentUuids)                      {
    QTreeWidgetItem * IT = documentItems [ duid ]   ;
    addTopLevelItem     ( IT   )                    ;
    IT->setExpanded     ( true )                    ;
  }                                                 ;
  QString m                                         ;
  m = tr("%1 documents").arg(documentUuids.count()) ;
  emit assignToolTip    ( m    )                    ;
  DoProcessEvents                                   ;
  emit AutoFit          (      )                    ;
  plan -> Talk          ( m    )                    ;
  emit assignEnabling   ( true )                    ;
  Alert                 ( Done )                    ;
  if (NotNull(Progress))                            {
    QProgressBar * P  = Progress                    ;
    Progress          = NULL                        ;
    P                -> deleteLater ( )             ;
    ProgressValue     = 0                           ;
  }                                                 ;
}

void N::DocumentItems::ProgressChanged(void)
{
  nDropOut ( IsNull(Progress) ) ;
  QProgressBar * P = Progress   ;
  nDropOut ( !P->isVisible()  ) ;
  P->setValue ( ProgressValue ) ;
}

void N::DocumentItems::run(int Type,ThreadData * data)
{ Q_UNUSED ( data ) ;
  switch ( Type )   {
    case 10001      :
    break           ;
  }                 ;
}

void N::DocumentItems::run(void)
{
  documentItems . clear ( )                                       ;
  documentUuids . clear ( )                                       ;
  QMap<SUID,QTreeWidgetItem *> Items                              ;
  UUIDs Duids                                                     ;
  SUID  duid                                                      ;
  EnterSQL   ( SC , plan->sql )                                   ;
    bool progress = false                                         ;
    if (ExtraUuids.count()>0)                                     {
      Duids = ExtraUuids                                          ;
    } else
    if (Division<=0)                                              {
      emit assignWindowTitle ( tr ( "All documents" ) )           ;
      Duids = Documents(SC,Sorting)                               ;
    } else                                                        {
      QString title = Name(SC,Division,plan->LanguageId)          ;
      emit assignWindowTitle ( title                  )           ;
      Duids = Documents(SC,Division,SourceType,Sorting)           ;
    }                                                             ;
    progress = ( Duids . count() > 100 )                          ;
    if (progress)                                                 {
      Progress -> setRange ( 0 , Duids.count() )                  ;
      Progress -> show     (                   )                  ;
    }                                                             ;
    foreach (duid,Duids)                                          {
      NewTreeWidgetItem(IT)                                       ;
      QString name = Name(SC,duid,plan->LanguageId)               ;
      Items[duid] = IT                                            ;
      IT->setData     (0,Qt::UserRole,duid)                       ;
      IT->setText     (0,name             )                       ;
      IT->setTextAlignment(2,Qt::AlignRight|Qt::AlignVCenter)     ;
    }                                                             ;
    foreach (duid,Duids)                                          {
      QString Q                                                   ;
      QList<QTreeWidgetItem *> CITEMs                             ;
      int rating                                                  ;
      ProgressValue ++                                            ;
      QString Sort                                                ;
      if (Sorting==Qt::DescendingOrder)                           {
        Sort = QString("where document = %1 order by prefer desc" )
               .arg(duid)                                         ;
      } else                                                      {
        Sort = QString("where document = %1 order by prefer asc"  )
               .arg(duid)                                         ;
      }                                                           ;
      Q = SC . sql . SelectFrom ( "fileuuid,extension,version"    ,
                               PlanTable(DocumentGroups) , Sort ) ;
      SqlLoopNow(SC,Q)                                            ;
        int extension = SC.Value(1).toInt()                       ;
        NewTreeWidgetItem(CT)                                     ;
        Items[duid]->addChild(CT)                                 ;
        CT->setData(0,Qt::UserRole  ,SC.Uuid(0))                  ;
        CT->setData(0,Qt::UserRole+1,extension )                  ;
        CT->setText(0,SC.String(2))                               ;
        setItemIcon(CT,0,extension)                               ;
        CITEMs << CT                                              ;
      SqlLoopErr(SC,Q)                                            ;
      SqlLoopEnd(SC,Q)                                            ;
      rating = Rating(SC,duid,"Complete")                         ;
      if (rating>=0)                                              {
        Items[duid]->setText(1,QString::number(rating))           ;
        Items[duid]->setTextAlignment(1,Qt::AlignRight            |
                                        Qt::AlignVCenter        ) ;
      }                                                           ;
      Items[duid]->setText(2,QString::number(CITEMs.count()))     ;
      for (int i=0;i<CITEMs.count();i++)                          {
        QTreeWidgetItem * ctem = CITEMs[i]                        ;
        QString encoding                                          ;
        SUID cuid = nTreeUuid(ctem,0)                             ;
        int  ext  = ctem->data(0,Qt::UserRole+1).toInt()          ;
        int  fs   = 0                                             ;
        rating = Rating(SC,cuid,"Complete")                       ;
        ctem->setTextAlignment(1,Qt::AlignRight|Qt::AlignVCenter) ;
        ctem->setTextAlignment(2,Qt::AlignRight|Qt::AlignVCenter) ;
        if (rating>=0)                                            {
          ctem->setText(1,QString::number(rating))                ;
        }                                                         ;
        switch (ext)                                              {
          case 0                                                  :
          break                                                   ;
          case 737                                                :
            fs       = PlainTextSize ( SC , cuid )                ;
            encoding = PlainEncoding ( SC , cuid )                ;
            ctem->setText(2,QString::number(fs))                  ;
            ctem->setText(3,encoding           )                  ;
          break                                                   ;
          default                                                 :
            fs = 0                                                ;
            Q = SC.sql.SelectFrom                                 (
                  "size"                                          ,
                  PlanTable(DocumentFiles)                        ,
                  SC.WhereUuid(cuid)                            ) ;
            if (SC.Fetch(Q))                                      {
              fs = SC.Int(0)                                      ;
              ctem->setText(2,QString::number(fs))                ;
            }                                                     ;
          break                                                   ;
        }                                                         ;
      }                                                           ;
    }                                                             ;
  LeaveSQL   ( SC , plan->sql )                                   ;
  documentItems = Items                                           ;
  documentUuids = Duids                                           ;
  emit AppendItems ( )                                            ;
}

void N::DocumentItems::Export(void)
{
  QString filename = QFileDialog::getSaveFileName (
                       this                       ,
                       tr("Export lists")         ,
                       ""                         ,
                       tr("Plain text (*.txt)") ) ;
  nDropOut ( filename . length() <= 0 )           ;
  QFile F(filename)                               ;
  nDropOut (!F.open(QIODevice::WriteOnly))        ;
  nFullLoop(i,topLevelItemCount())                {
    QTreeWidgetItem * it = topLevelItem(i)        ;
    QString n = it->text(0)                       ;
    n += "\r\n"                                   ;
    F.write(n.toUtf8())                           ;
  }                                               ;
  F.close()                                       ;
}

void N::DocumentItems::SaveAs(void)
{
  QTreeWidgetItem * item = currentItem ( )                         ;
  QTreeWidgetItem * ptem = NULL                                    ;
  if ( NotNull(item) ) ptem = item -> parent ( )                   ;
  nDropOut ( IsNull(ptem) )                                        ;
  SUID    fileuuid  = nTreeUuid(item,0)                            ;
  int     extension = item->data(0,Qt::UserRole+1).toInt()         ;
  QString title     = ptem->text(0)                                ;
  QString suffix    = ""                                           ;
  QString mimetype  = ""                                           ;
  //////////////////////////////////////////////////////////////////
  switch ( extension )                                             {
    case   0                                                       : // Neutrin Organized Complex Document Format
    break                                                          ;
    case 125                                                       : // doc
      suffix   = "doc"                                             ;
      mimetype = tr("Microsoft Word")                              ;
    break                                                          ;
    case 126                                                       : // docm
      suffix = "docm"                                              ;
      mimetype = tr("Microsoft Word")                              ;
    break                                                          ;
    case 127                                                       : // docx
      suffix = "docx"                                              ;
      mimetype = tr("Microsoft Word")                              ;
    break                                                          ;
    case 156                                                       : // eml
      suffix = "eml"                                               ;
      mimetype = tr("Email HTML")                                  ;
    break                                                          ;
    case 240                                                       : // htm
      suffix = "htm"                                               ;
      mimetype = tr("HTML")                                        ;
    break                                                          ;
    case 241                                                       : // html
      suffix = "html"                                              ;
      mimetype = tr("HTML")                                        ;
    break                                                          ;
    case 443                                                       : // odb
      suffix = "odb"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 444                                                       : // odc
      suffix = "odc"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 445                                                       : // odf
      suffix = "odf"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 446                                                       : // odg
      suffix = "odg"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 447                                                       : // odi
      suffix = "odi"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 448                                                       : // odm
      suffix = "odm"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 449                                                       : // odp
      suffix = "odp"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 450                                                       : // ods
      suffix = "ods"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 451                                                       : // odt
      suffix = "odt"                                               ;
      mimetype = tr("Open Office")                                 ;
    break                                                          ;
    case 483                                                       : // pdf
      suffix = "pdf"                                               ;
      mimetype = tr("Adobe PDF")                                   ;
    break                                                          ;
    case 737                                                       : // text
      suffix = "txt"                                               ;
      mimetype = tr("Plain text")                                  ;
    break                                                          ;
    case 838                                                       : // xml
      suffix = "xml"                                               ;
      mimetype = tr("XML")                                         ;
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  if ( suffix . length ( ) <= 0 ) return                           ;
  //////////////////////////////////////////////////////////////////
  QByteArray Body                                                  ;
  EnterSQL ( SC , plan -> sql )                                    ;
    QString Q                                                      ;
    switch ( extension )                                           {
      case   0                                                     :
      break                                                        ;
      case 737                                                     :
        PlainText ( SC , fileuuid , Body )                         ;
      break                                                        ;
      case 838                                                     :
        GetXml    ( SC , fileuuid , Body )                         ;
      break                                                        ;
      default                                                      :
        Q = SC . sql . SelectFrom                                  (
              "body"                                               ,
              PlanTable(DocumentFiles)                             ,
              SC.WhereUuid(fileuuid)                             ) ;
        if ( SC . Fetch ( Q ) ) Body = SC . ByteArray ( 0 )        ;
      break                                                        ;
    }                                                              ;
  ErrorSQL ( SC , plan -> sql )                                    ;
  LeaveSQL ( SC , plan -> sql )                                    ;
  //////////////////////////////////////////////////////////////////
  if ( Body . size ( ) <= 0 )                                      {
    if ( QMessageBox::No == QMessageBox::question                  (
           this                                                    ,
           title                                                   ,
           tr("This document does not have content.\n Do you really wish to save it?")
                                                               ) ) {
      return                                                       ;
    }                                                              ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  QString filters  = ""                                            ;
  QString filename = ""                                            ;
  QDir    D = plan->Path(Directory::Documents)                     ;
  //////////////////////////////////////////////////////////////////
  filters += QString("%1 (*.%2)").arg(mimetype).arg(suffix)        ;
  filters += tr(";;Any file format (*.*)")                         ;
  filename = tr("%1.%2").arg(title).arg(suffix)                    ;
  filename = D . absoluteFilePath ( filename )                     ;
  filename = QFileDialog::getSaveFileName                          (
               this                                                ,
               tr("Save %1 as...").arg(title)                      ,
              filename                                             ,
              filters                                            ) ;
  if ( filename . length ( ) <= 0 ) return                         ;
  //////////////////////////////////////////////////////////////////
  File::toFile ( filename , Body )                                 ;
  Alert        ( Done            )                                 ;
}

void N::DocumentItems::fileClicked(QTreeWidgetItem * item)
{
  QTreeWidgetItem * ptem = item->parent()                          ;
  nDropOut ( IsNull(ptem) )                                        ;
  SUID    document  = nTreeUuid(ptem,0)                            ;
  SUID    fileuuid  = nTreeUuid(item,0)                            ;
  int     extension = item->data(0,Qt::UserRole+1).toInt()         ;
  QString version   = item->text(0)                                ;
  QString title     = ptem->text(0)                                ;
  emit Open ( document , fileuuid , extension , version , title )  ;
}

void N::DocumentItems::rateClicked(QTreeWidgetItem * item)
{
  QSpinBox * sb = new QSpinBox(this)      ;
  setItemWidget ( item , 1 , sb )         ;
  ItemEditing = item                      ;
  ItemColumn  = 1                         ;
  ItemWidget  = sb                        ;
  sb->setRange(0,1000)                    ;
  sb->setValue(item->text(1).toInt())     ;
  connect(sb  ,SIGNAL(editingFinished())  ,
          this,SLOT  (rateFinished   ())) ;
}

void N::DocumentItems::rateFinished(void)
{
  nDropOut ( IsNull(ItemWidget) )                 ;
  QSpinBox * sb = Casting(QSpinBox,ItemWidget)    ;
  SUID uuid     = nTreeUuid(ItemEditing,0)        ;
  int  rating   = sb->value()                     ;
  ItemEditing->setText(1,QString::number(rating)) ;
  removeOldItem ( )                               ;
  EnterSQL    ( SC , plan->sql                  ) ;
    setRating ( SC , uuid , "Complete" , rating ) ;
  LeaveSQL    ( SC , plan->sql                  ) ;
  Alert       ( Done                            ) ;
}

void N::DocumentItems::singleClicked(QTreeWidgetItem * item,int column)
{
  if (IsNull(singleItem))                           {
    singleItem   = item                             ;
    singleColumn = column                           ;
    singleTime   = QDateTime::currentDateTime()     ;
  } else                                            {
    if (column>0)                                   {
      singleItem   = NULL                           ;
      singleColumn = -1                             ;
    } else                                          {
      if (item==singleItem && singleColumn==column) {
        QDateTime Now                               ;
        Now = QDateTime::currentDateTime()          ;
        int delay = singleTime.msecsTo(Now)         ;
        if (delay>800 && delay<1400)                {
          Rename ( item )                           ;
          singleItem   = NULL                       ;
          singleColumn = -1                         ;
        }                                           ;
      } else                                        {
        singleItem   = item                         ;
        singleColumn = column                       ;
        singleTime   = QDateTime::currentDateTime() ;
      }                                             ;
    }                                               ;
  }                                                 ;
}

void N::DocumentItems::doubleClicked(QTreeWidgetItem * item,int column)
{
  switch (column)              {
    case 0                     :
      if (NotNull(singleItem)) {
        fileClicked ( item )   ;
      }                        ;
    break                      ;
    case 1                     :
      singleItem   = NULL      ;
      singleColumn = -1        ;
      rateClicked ( item )     ;
    break                      ;
  }                            ;
  singleItem   = NULL          ;
  singleColumn = -1            ;
}

bool N::DocumentItems::Menu(QPoint pos)
{
  nKickOut ( NotNull(ItemWidget) , false )                      ;
  nScopedMenu ( mm , this )                                     ;
  QTreeWidgetItem * item = NULL                                 ;
  QTreeWidgetItem * ptem = NULL                                 ;
  QAction         * aa                                          ;
  QMenu           * mi                                          ;
  QMenu           * me                                          ;
  UUIDs             DUIDs                                       ;
  ///////////////////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)                       {
    QTreeWidgetItem * it = topLevelItem(i)                      ;
    if (it->isSelected())                                       {
      DUIDs << nTreeUuid(it,0)                                  ;
    }                                                           ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  item = itemAt ( pos )                                         ;
  me = mm.addMenu(tr("Edit"))                                   ;
  mm.add(me,301,tr("Add document"))                             ;
  if (NotNull(item))                                            {
    ptem = item->parent()                                       ;
    if (NotNull(ptem))                                          {
      mm.add(101,tr("Open"                    ))                ;
      mm.add(102,tr("Save as ..."             ))                ;
      mm.add(me,805,tr("Detach"               ))                ;
    }                                                           ;
    mm.addSeparator(me)                                         ;
    mm.add(me,302,tr("Add plain text document"))                ;
    mm.add(me,303,tr("Add VCF document"       ))                ;
    mm.add(me,304,tr("Import document members"))                ;
    mm.addSeparator(me)                                         ;
    mm.add(me,801,tr("Set rating"             ))                ;
    mm.add(me,802,tr("Rename"                 ))                ;
    mm.add(me,803,tr("Search"                 ))                ;
    mm.add(me,804,tr("Remove"                 ))                ;
    mm.addSeparator(me)                                         ;
    if (DUIDs.count()>0)                                        {
      mm.add(me,701,tr("Document processors"  ))                ;
    }                                                           ;
    mm.add(me,711,tr("Import documents"       ))                ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  mi = mm.addMenu(tr("Organize"))                               ;
  aa  = mm.add(mi,911,tr("Enable sorting"))                     ;
  aa -> setCheckable(true)                                      ;
  aa -> setChecked(isSortingEnabled())                          ;
  if (nEqual(Sorting,Qt::AscendingOrder))                       {
    mm .add(mi,913,tr("Descending list"))                       ;
  } else                                                        {
    mm .add(mi,912,tr("Ascending list" ))                       ;
  }                                                             ;
  mm.addSeparator(mi)                                           ;
  mm.add(mi,901,tr("Expand all"  ))                             ;
  mm.add(mi,902,tr("Collapse all"))                             ;
  mm.addSeparator(mi)                                           ;
  mm.add(mi,903,tr("Export lists"))                             ;
  mm.addSeparator(mi)                                           ;
  DockingMenu ( mm )                                            ;
  mm.setFont(plan)                                              ;
  aa = mm.exec()                                                ;
  nKickOut ( IsNull(aa) , true )                                ;
  if ( RunDocking ( mm , aa ) ) return true                     ;
  ///////////////////////////////////////////////////////////////
  SUID    document  = 0                                         ;
  SUID    fileuuid  = 0                                         ;
  int     extension = 0                                         ;
  QString version   = ""                                        ;
  QString title     = ""                                        ;
  ///////////////////////////////////////////////////////////////
  if (NotNull(item) && NotNull(ptem))                           {
    document  = nTreeUuid(ptem,0)                               ;
    fileuuid  = nTreeUuid(item,0)                               ;
    extension = item->data(0,Qt::UserRole+1).toInt()            ;
    version   = item->text(0)                                   ;
    title     = ptem->text(0)                                   ;
  }                                                             ;
  ///////////////////////////////////////////////////////////////
  switch (mm[aa])                                               {
    case 101                                                    :
      emit Open ( document                                      ,
                  fileuuid                                      ,
                  extension                                     ,
                  version                                       ,
                  title                                       ) ;
    break                                                       ;
    case 102                                                    :
      SaveAs            (                 )                     ;
    break                                                       ;
    case 301                                                    :
      addDocument       (                 )                     ;
    break                                                       ;
    case 302                                                    :
      if (NotNull(ptem)) addPlainText(ptem)                     ;
                    else addPlainText(item)                     ;
    break                                                       ;
    case 303                                                    :
      if (NotNull(ptem)) addVcf(ptem)                           ;
                    else addVcf(item)                           ;
    break                                                       ;
    case 304                                                    :
      importFiles       ( item            )                     ;
    break                                                       ;
    case 701                                                    :
      emit Processors   ( DUIDs           )                     ;
    break                                                       ;
    case 711                                                    :
      Import            (                 )                     ;
    break                                                       ;
    case 801                                                    :
      rateClicked       ( item            )                     ;
    break                                                       ;
    case 802                                                    :
      Rename            ( item            )                     ;
    break                                                       ;
    case 803                                                    :
      Search            (                 )                     ;
    break                                                       ;
    case 804                                                    :
      Remove            (                 )                     ;
    break                                                       ;
    case 805                                                    :
      Detach            (                 )                     ;
    break                                                       ;
    case 901                                                    :
      setAllExpanded    ( true            )                     ;
    break                                                       ;
    case 902                                                    :
      setAllExpanded    ( false           )                     ;
    break                                                       ;
    case 903                                                    :
      Export            (                 )                     ;
    break                                                       ;
    case 911                                                    :
      setSortingEnabled ( aa->isChecked() )                     ;
    break                                                       ;
    case 912                                                    :
      Sorting = Qt::AscendingOrder                              ;
      List ( )                                                  ;
    break                                                       ;
    case 913                                                    :
      Sorting = Qt::DescendingOrder                             ;
      List ( )                                                  ;
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

void N::DocumentItems::Rename(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  nDropOut ( IsNull(item) )                ;
  Rename   ( item         )                ;
}

void N::DocumentItems::Rename(QTreeWidgetItem * item)
{
  QTreeWidgetItem * ptem = item->parent()          ;
  QLineEdit       * le                             ;
  QString           m                              ;
  if (IsNull(ptem))                                {
    le = setLineEdit                               (
           item                                    ,
           0                                       ,
           SIGNAL(editingFinished ())              ,
           SLOT  (documentFinished()) )            ;
    le->setFocus(Qt::TabFocusReason)               ;
  } else                                           {
    int ext = item->data(0,Qt::UserRole+1).toInt() ;
    switch (ext)                                   {
      case 0                                       :
        le = setLineEdit                           (
               item                                ,
               0                                   ,
               SIGNAL(editingFinished ())          ,
               SLOT  (vcfFinished     ()) )        ;
        le->setFocus(Qt::TabFocusReason)           ;
      break                                        ;
      case 737                                     :
        le = setLineEdit                           (
               item                                ,
               0                                   ,
               SIGNAL(editingFinished ())          ,
               SLOT  (plainFinished   ()) )        ;
        le->setFocus(Qt::TabFocusReason)           ;
      break                                        ;
      default                                      :
        m = tr("Format is not supported now")      ;
        plan -> Talk ( m )                         ;
      break                                        ;
    }                                              ;
  }                                                ;
}

void N::DocumentItems::Add(void)
{
  QTreeWidgetItem * item = currentItem()  ;
  nDropOut ( IsNull(item) )               ;
  QTreeWidgetItem * ctem = item->parent() ;
  if (IsNull(ctem)) addDocument (      )  ;
               else addVcf      ( ctem )  ;
}

void N::DocumentItems::addDocument(void)
{
  NewTreeWidgetItem(item)             ;
  item->setData(0,Qt::UserRole,0)     ;
  if (Sorting==Qt::DescendingOrder)   {
    insertTopLevelItem ( 0 , item )   ;
    scrollToItem       ( item     )   ;
  }  else                             {
    addTopLevelItem    ( item     )   ;
    scrollToItem       ( item     )   ;
  }                                   ;
  QLineEdit * le                      ;
  le = setLineEdit                    (
         item                         ,
         0                            ,
         SIGNAL(editingFinished ())   ,
         SLOT  (documentFinished()) ) ;
  le->setFocus(Qt::TabFocusReason)    ;
  QString m                           ;
  m = tr("Add document")              ;
  plan -> Talk ( m )                  ;
}

void N::DocumentItems::documentFinished(void)
{
  nDropOut ( IsNull(ItemWidget))                      ;
  QLineEdit * le = Casting(QLineEdit,ItemWidget )     ;
  nDropOut ( IsNull(le        ))                      ;
  SUID    uuid = nTreeUuid(ItemEditing,0)             ;
  QString name = le->text()                           ;
  GroupItems GI ( plan )                              ;
  EnterSQL    ( SC , plan->sql                      ) ;
    if (uuid<=0)                                      {
      if (name.length()>0)                            {
        uuid = appendDocument(SC)                     ;
        assureName(SC,uuid,plan->LanguageId,name    ) ;
        ItemEditing->setText(0,name)                  ;
        ItemEditing->setData(0,Qt::UserRole,uuid)     ;
        if (Division>0)                               {
          UUIDs US                                    ;
          US << uuid                                  ;
          GI . AutoMap    = true                      ;
          GI . GroupTable = GI.LookTable              (
            SourceType                                ,
            Types::Document                           ,
            Groups::Subordination                   ) ;
          GI . Join                                   (
            SC                                        ,
            Division                                  ,
            SourceType                                ,
            Types::Document                           ,
            Groups::Subordination                     ,
            0                                         ,
            US                                      ) ;
        }                                             ;
        Alert ( Done  )                               ;
      } else                                          {
        Alert ( Error )                               ;
      }                                               ;
    } else                                            {
      assureName(SC,uuid,plan->LanguageId,name      ) ;
      ItemEditing->setText(0,name)                    ;
      Alert   ( Done  )                               ;
    }                                                 ;
  LeaveSQL    ( SC , plan->sql                      ) ;
  removeOldItem    ( true , 0                       ) ;
}

void N::DocumentItems::addPlainText(QTreeWidgetItem * item)
{
  item->setExpanded(true)                             ;
  NewTreeWidgetItem(ctem)                             ;
  ctem->setData(0,Qt::UserRole  ,0  )                 ;
  ctem->setData(0,Qt::UserRole+1,737)                 ;
  item->addChild(ctem)                                ;
  setItemIcon(ctem,0,737)                             ;
  QLineEdit * le                                      ;
  QString version                                     ;
  QDateTime Now                                       ;
  Now = QDateTime::currentDateTime()                  ;
  version = tr("%1:%2"                                )
            .arg(plan->Hostname                       )
            .arg(Now.toString("yyyy/MM/dd-hh:mm:ss")) ;
  le = setLineEdit                                    (
         ctem                                         ,
         0                                            ,
         SIGNAL(editingFinished())                    ,
         SLOT  (plainFinished  ())                  ) ;
  le->setText (version           )                    ;
  le->setFocus(Qt::TabFocusReason)                    ;
  QString m = tr("Add plain text")                    ;
  plan -> Talk ( m )                                  ;
}

void N::DocumentItems::plainFinished(void)
{
  nDropOut ( IsNull(ItemWidget))                  ;
  QLineEdit * le = Casting(QLineEdit,ItemWidget ) ;
  nDropOut ( IsNull(le        ))                  ;
  QTreeWidgetItem * ptem = ItemEditing->parent()  ;
  nDropOut ( IsNull(ptem      ))                  ;
  SUID    duid = nTreeUuid(ptem,0)                ;
  SUID    uuid = nTreeUuid(ItemEditing,0)         ;
  QString name = le->text()                       ;
  EnterSQL    ( SC , plan->sql                  ) ;
    if (uuid==0)                                  {
      if (name.length()>0)                        {
        QString Q                                 ;
        uuid = acquirePlainText(SC)               ;
        emptyPlainText ( SC,uuid                ) ;
        JoinDocument   ( SC,duid,uuid,737,name  ) ;
        ///////////////////////////////////////////
        ItemEditing->setText(0,name)              ;
        ItemEditing->setData(0,Qt::UserRole,uuid) ;
      }                                           ;
    } else                                        {
      UpdateVersion ( SC , duid , uuid , name )   ;
      ItemEditing->setText(0,name)                ;
    }                                             ;
  LeaveSQL      ( SC , plan->sql                ) ;
  removeOldItem ( true , 0                      ) ;
  Alert         ( Done                          ) ;
}

void N::DocumentItems::addVcf(QTreeWidgetItem * item)
{
  item->setExpanded(true)                             ;
  NewTreeWidgetItem(ctem)                             ;
  ctem->setData(0,Qt::UserRole  ,0  )                 ;
  ctem->setData(0,Qt::UserRole+1,0  )                 ;
  item->addChild(ctem)                                ;
  setItemIcon(ctem,0,0)                               ;
  QLineEdit * le                                      ;
  QString version                                     ;
  QDateTime Now                                       ;
  Now = QDateTime::currentDateTime()                  ;
  version = tr("%1:%2"                                )
            .arg(plan->Hostname                       )
            .arg(Now.toString("yyyy/MM/dd-hh:mm:ss")) ;
  le = setLineEdit                                    (
         ctem                                         ,
         0                                            ,
         SIGNAL(editingFinished())                    ,
         SLOT  (vcfFinished    ())                  ) ;
  le->setText (version           )                    ;
  le->setFocus(Qt::TabFocusReason)                    ;
  QString m = tr("Add VCF")                           ;
  plan -> Talk ( m )                                  ;
}

void N::DocumentItems::vcfFinished(void)
{
  nDropOut ( IsNull(ItemWidget) )                     ;
  QLineEdit * le = Casting(QLineEdit,ItemWidget )     ;
  nDropOut ( IsNull(le        ) )                     ;
  QTreeWidgetItem * ptem = ItemEditing->parent()      ;
  nDropOut ( IsNull(ptem      ) )                     ;
  SUID    duid = nTreeUuid(ptem,0)                    ;
  SUID    uuid = nTreeUuid(ItemEditing,0)             ;
  QString name = le->text()                           ;
  EnterSQL    ( SC , plan->sql                      ) ;
    if (uuid==0)                                      {
      if (name.length()>0)                            {
        QString Q                                     ;
        int     prefer = 0                            ;
        Q = SC.sql.SelectFrom                         (
              "prefer"                                ,
               PlanTable(DocumentGroups)              ,
               QString("where document = %1 "
                       "order by prefer desc "
                       "limit 0,1").arg(duid)       ) ;
        if (SC.Fetch(Q)) prefer = SC.Int(0)           ;
        prefer++                                      ;
        uuid = SC.Unique(PlanTable(MajorUuid),"uuid") ;
        SC.assureUuid                                 (
          PlanTable(MajorUuid)                        ,
          uuid                                        ,
          Types::VCF                      ) ;
        SC.insertUuid(PlanTable(VCF),uuid,0)          ;
        Q = SC.sql.InsertInto                         (
              PlanTable(DocumentGroups)               ,
              5                                       ,
              "document"                              ,
              "fileuuid"                              ,
              "extension"                             ,
              "prefer"                                ,
              "version"                             ) ;
        SC . Prepare ( Q                            ) ;
        SC . Bind    ( "document"  , duid           ) ;
        SC . Bind    ( "fileuuid"  , uuid           ) ;
        SC . Bind    ( "extension" , 0              ) ;
        SC . Bind    ( "prefer"    , prefer         ) ;
        SC . Bind    ( "version"   , name.toUtf8()  ) ;
        SC . Exec    (                              ) ;
        ItemEditing->setText(0,name)                  ;
        ItemEditing->setData(0,Qt::UserRole,uuid)     ;
      }                                               ;
    } else                                            {
      QString Q                                       ;
      Q = SC.sql.Update                               (
             PlanTable(DocumentGroups)                ,
             "where document = :DOCUMENT "
             "and fileuuid = :FILEUUID"               ,
             1,"version"                            ) ;
      SC . Prepare ( Q                              ) ;
      SC . Bind    ( "document"  , duid             ) ;
      SC . Bind    ( "fileuuid"  , uuid             ) ;
      SC . Bind    ( "version"   , name.toUtf8()    ) ;
      SC . Exec    (                                ) ;
      ItemEditing->setText(0,name)                    ;
    }                                                 ;
  LeaveSQL      ( SC , plan->sql                    ) ;
  removeOldItem ( true , 0                          ) ;
  Alert         ( Done                              ) ;
}

void N::DocumentItems::lineEditor(void)
{
  if (NotNull(Editor)) Editor->deleteLater()        ;
  Editor = new LineEdit()                           ;
  QFont          font = plan->status->font()        ;
  QSize          s    = plan->status->size()        ;
  QSize          m(s.width()/6,s.height())          ;
  if (font.pixelSize()>0)                           {
    font.setPixelSize (m.height()-4)                ;
    Editor ->setFont  (font        )                ;
  }                                                 ;
  Editor -> setMinimumSize ( m )                    ;
  Editor -> setMaximumSize ( m )                    ;
  plan   -> status -> addPermanentWidget ( Editor ) ;
  Editor -> show                         (        ) ;
}

void N::DocumentItems::Search(void)
{
  lineEditor()                               ;
  Editor->setPlaceholderText(tr("Search"))   ;
  connect(Editor,SIGNAL(editingFinished())   ,
          this  ,SLOT  (searchFinished ()) ) ;
  Editor -> setFocus ( Qt::TabFocusReason )  ;
  Alert ( Selected )                         ;
}

void N::DocumentItems::searchFinished(void)
{
  nDropOut ( IsNull(Editor) )         ;
  QString keyword = Editor->text()    ;
  Editor->deleteLater()               ;
  Editor = NULL                       ;
  QTreeWidgetItem * item              ;
  int index = 0                       ;
  item = currentItem()                ;
  if (NotNull(item))                  {
    if (NotNull(item->parent()))      {
      item = item->parent()           ;
    }                                 ;
    index = indexOfTopLevelItem(item) ;
  }                                   ;
  /////////////////////////////////////
  while (index<topLevelItemCount())   {
    QString k                         ;
    item = topLevelItem(index)        ;
    k = item->text(0)                 ;
    if (k.contains(keyword))          {
      setCurrentItem ( item   )       ;
      scrollToItem   ( item   )       ;
      Alert          ( Action )       ;
      return                          ;
    }                                 ;
    index++                           ;
  }                                   ;
  Alert ( Error )                     ;
}

void N::DocumentItems::Remove(void)
{
  UUIDs Uuids                              ;
  QList<QTreeWidgetItem *> Items           ;
  for (int i=0;i<topLevelItemCount();i++)  {
    QTreeWidgetItem * it = topLevelItem(i) ;
    if (it->isSelected())                  {
      SUID uuid = nTreeUuid(it,0)          ;
      Uuids << uuid                        ;
      Items << it                          ;
    }                                      ;
  }                                        ;
  nDropOut ( Uuids.count() <= 0 )          ;
  for (int i=0;i<Items.count();i++)        {
    QTreeWidgetItem * it = Items[i]        ;
    int index = indexOfTopLevelItem(it)    ;
    takeTopLevelItem(index)                ;
  }                                        ;
  GroupItems GI ( plan )                   ;
  EnterSQL    ( SC , plan->sql           ) ;
    GI . AutoMap    = true                 ;
    GI . GroupTable = GI.LookTable         (
      Types::Division                      ,
      Types::Document                      ,
      Groups::Subordination              ) ;
    GI . Detach                            (
      SC                                   ,
      Division                             ,
      Types::Division                      ,
      Types::Document                      ,
      Groups::Subordination                ,
      Uuids                              ) ;
  LeaveSQL    ( SC , plan->sql           ) ;
  Alert       ( Done                     ) ;
}

void N::DocumentItems::Detach(void)
{
  QTreeWidgetItem * item = currentItem () ;
  nDropOut ( IsNull(item) )               ;
  QTreeWidgetItem * ctem = item->parent() ;
  nDropOut ( IsNull(ctem) )               ;
  SUID duid  = nTreeUuid(ctem,0)          ;
  SUID fuid  = nTreeUuid(item,0)          ;
  int  index = ctem->indexOfChild(item)   ;
  nDropOut ( index < 0 )                  ;
  nDropOut ( duid == 0 )                  ;
  nDropOut ( fuid == 0 )                  ;
  EnterSQL    ( SC , plan->sql          ) ;
    QString Q                             ;
    Q = SC.sql.DeleteFrom                 (
          PlanTable(DocumentGroups)       ,
          QString("where document = %1 "
                  "and fileuuid = %2 ;"   )
          .arg(duid).arg(fuid)          ) ;
    SC . Query ( Q )                      ;
    ctem -> takeChild ( index )           ;
  LeaveSQL    ( SC , plan->sql          ) ;
  Alert       ( Done                    ) ;
}

bool N::DocumentItems::acceptDrop(QWidget * source,const QMimeData * mime)
{
  nKickOut ( nEqual ( source   , this ) , false ) ;
  nKickOut ( nEqual ( Division , 0    ) , false ) ;
  return dropHandler(mime)                        ;
}

bool N::DocumentItems::dropNew(QWidget * source,const QMimeData * mime,QPoint pos)
{
  Alert ( Action ) ;
  return true      ;
}

bool N::DocumentItems::dropDocuments(QWidget * source,QPointF pos,const UUIDs & Uuids)
{ Q_UNUSED ( source )                   ;
  Q_UNUSED ( pos    )                   ;
  nKickOut( Division     <= 0 , false ) ;
  nKickOut( Uuids.count()<= 0 , false ) ;
  UUIDs US = Uuids                      ;
  GroupItems GI  ( plan               ) ;
  EnterSQL       ( SC , plan->sql     ) ;
    GI . AutoMap    = true              ;
    GI . GroupTable = GI . LookTable    (
      SourceType                        ,
      Types::Document                   ,
      Groups::Subordination           ) ;
    GI . Join                           (
      SC                                ,
      Division                          ,
      SourceType                        ,
      Types::Document                   ,
      Groups::Subordination             ,
      0                                 ,
      US                              ) ;
  LeaveSQL    ( SC , plan->sql        ) ;
  Alert       ( Done                  ) ;
  List        (                       ) ;
  return true                           ;
}

void N::DocumentItems::importFiles(QTreeWidgetItem * item)
{
  QStringList files = QFileDialog::getOpenFileNames   (
                        this                          ,
                        tr("Import document members") ,
                        ""                            ,
                        "*.*"                       ) ;
  nDropOut ( files.count() <= 0 )                     ;
  if (NotNull(item->parent())) item = item->parent()  ;
  SUID duid = nTreeUuid(item,0)                       ;
  nDropOut ( duid <= 0 )                              ;
  MimeManager MM (plan)                               ;
  EnterSQL(SC,plan->sql)                              ;
    QString Q                                         ;
    MM.load(SC)                                       ;
    for (int i=0;i<files.count();i++)                 {
      QString   name   = files[i]                     ;
      QFileInfo FXI(name)                             ;
      QString   suffix = FXI.suffix()                 ;
      SUID      eid    = 0                            ;
      suffix = suffix.toLower()                       ;
      for (int i=0                                    ;
           eid==0 && i<MM.Extensions.count();i++)     {
        SUID EUID = MM.Extensions[i]                  ;
        if (MM.Suffixes[EUID]==suffix) eid = EUID     ;
      }                                               ;
      if (eid>0)                                      {
        SUID       FUID     = 0                       ;
        int        checksum = 0                       ;
        QByteArray Body                               ;
        File::toByteArray(name,Body)        ;
        checksum = qChecksum(Body.data(),Body.size()) ;
        FUID = SC.Unique(PlanTable(MajorUuid),"uuid") ;
        SC.assureUuid                                 (
          PlanTable(MajorUuid)                        ,
          FUID                                        ,
          Types::DocumentFile             ) ;
        Q = SC.sql.InsertInto                         (
              PlanTable(DocumentFiles)                ,
              5                                       ,
              "uuid"                                  ,
              "extension"                             ,
              "size"                                  ,
              "checksum"                              ,
              "body"                                ) ;
        SC . Prepare ( Q                            ) ;
        SC . Bind    ( "uuid"      , FUID           ) ;
        SC . Bind    ( "extension" , eid            ) ;
        SC . Bind    ( "size"      , Body.size()    ) ;
        SC . Bind    ( "checksum"  , checksum       ) ;
        SC . Bind    ( "body"      , Body           ) ;
        SC . Exec    (                              ) ;
        if (FUID>0)                                   {
          int     prefer = 0                          ;
          Q = SC.sql.SelectFrom                       (
                "prefer"                              ,
                 PlanTable(DocumentGroups)            ,
                 QString("where document = %1 "
                         "order by prefer desc "
                         "limit 0,1").arg(duid)     ) ;
          if (SC.Fetch(Q)) prefer = SC.Int(0)         ;
          prefer++                                    ;
          Q = SC.sql.InsertInto                       (
                PlanTable(DocumentGroups)             ,
                5                                     ,
                "document"                            ,
                "fileuuid"                            ,
                "extension"                           ,
                "prefer"                              ,
                "version"                           ) ;
          SC . Prepare ( Q                          ) ;
          SC . Bind    ( "document"  , duid         ) ;
          SC . Bind    ( "fileuuid"  , FUID         ) ;
          SC . Bind    ( "extension" , eid          ) ;
          SC . Bind    ( "prefer"    , prefer       ) ;
          SC . Bind    ( "version"   , name.toUtf8()) ;
          SC . Exec    (                            ) ;
        }                                             ;
      }                                               ;
    }                                                 ;
  LeaveSQL(SC,plan->sql)                              ;
  List ( )                                            ;
}

void N::DocumentItems::Import(void)
{
  QStringList files = QFileDialog::getOpenFileNames        (
                        this                               ,
                        tr("Import documents")             ,
                        ""                                 ,
                        tr("All files (*.*)")            ) ;
  nDropOut ( files . count ( ) <= 0 )                      ;
  plan -> Talk( tr("Request to finish this importation") ) ;
}
