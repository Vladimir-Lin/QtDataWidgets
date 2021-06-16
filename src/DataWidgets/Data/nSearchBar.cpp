#include <datawidgets.h>
#include "ui_nSearchBar.h"

N::SearchBar:: SearchBar  ( QWidget * parent , Plan * p )
             : ToolBar    (           parent ,        p )
             , ui         ( new Ui::nSearchBar          )
             , Mode       ( NameIndex                   )
             , Pattern    ( SQL                         )
             , Keyword    ( ""                          )
             , Searching  ( false                       )
             , Menu       ( NULL                        )
             , Search     ( NULL                        )
             , Editor     ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SearchBar::~SearchBar(void)
{
  delete ui ;
}

void N::SearchBar::Configure(void)
{
  ui     -> setupUi            ( this                      ) ;
  Search  = new QComboBox      ( this                      ) ;
  Editor  = new LineEdit       ( Search , plan             ) ;
  Search -> setLineEdit        ( Editor                    ) ;
  Search -> setEditable        ( true                      ) ;
  Search -> setInsertPolicy    ( QComboBox::InsertAtTop    ) ;
  Editor -> setPlaceholderText ( tr("Search")              ) ;
  Editor -> setMaxLength       ( 100                       ) ;
  addWidget                    ( Search                    ) ;
  ////////////////////////////////////////////////////////////
  setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea ) ;
  ////////////////////////////////////////////////////////////
  connect( Editor , SIGNAL ( returnPressed() )               ,
           this   , SLOT   ( nameFinished () )             ) ;
  ////////////////////////////////////////////////////////////
  Reservation = true                                         ;
  ////////////////////////////////////////////////////////////
  QMenu        * Items                                       ;
  QMenu        * Match                                       ;
  QMenu        * Languages                                   ;
  QActionGroup * ag                                          ;
  QAction      * aa                                          ;
  Menu      = new QMenu(this)                                ;
  Items     = Menu->addMenu(tr("Items"        ))             ;
  Match     = Menu->addMenu(tr("Pattern match"))             ;
  Languages = Menu->addMenu(tr("Languages"    ))             ;
  ////////////////////////////////////////////////////////////
  aa    = Items -> addAction    ( tr("Everything"  ) )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setData      ( 101                )       ;
  ItemActions   << aa                                        ;
          Items -> addSeparator (                    )       ;
  aa    = Items -> addAction    ( tr("Object names") )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( true               )       ;
  aa            -> setData      ( 201                )       ;
  ItemActions   << aa                                        ;
  Items         -> addSeparator (                    )       ;
  ////////////////////////////////////////////////////////////
  ag = new QActionGroup         ( Items              )       ;
  ActionGroups  << ag                                        ;
  ////////////////////////////////////////////////////////////
  aa    = ag    -> addAction    ( tr("Generic search"              ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( true               )       ;
  aa            -> setData      ( 301                )       ;
  aa    = ag    -> addAction    ( tr("Search people only"          ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 302                )       ;
  aa    = ag    -> addAction    ( tr("Search albums only"          ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 303                )       ;
  aa    = ag    -> addAction    ( tr("Search clips only"           ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 304                )       ;
  aa    = ag    -> addAction    ( tr("Search documents only"       ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 305                )       ;
  aa    = ag    -> addAction    ( tr("Search dictionary only"      ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 306                )       ;
  aa    = ag    -> addAction    ( tr("Search system paraments only") ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 307                )       ;
  aa    = ag    -> addAction    ( tr("Database operations")) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 308                )       ;
  aa    = ag    -> addAction    ( tr("Search web pages"  ) ) ;
          Items -> addAction    ( aa                 )       ;
  aa            -> setCheckable ( true               )       ;
  aa            -> setChecked   ( false              )       ;
  aa            -> setData      ( 309                )       ;
  ////////////////////////////////////////////////////////////
  ag        = new QActionGroup(Match    )                    ;
  ActionGroups  << ag                                        ;
  ////////////////////////////////////////////////////////////
  aa    = ag    -> addAction    ( tr("Exact match"       ) ) ;
  aa            -> setCheckable ( true                     ) ;
  Match         -> addAction    ( aa                       ) ;
  MatchActions  << aa                                        ;
  aa    = ag    -> addAction    ( tr("SQL syntax"        ) ) ;
  aa            -> setCheckable ( true                     ) ;
  aa            -> setChecked   ( true                     ) ;
  Match         -> addAction    ( aa                       ) ;
  MatchActions  << aa                                        ;
  aa    = ag    -> addAction    ( tr("Regular expression") ) ;
  aa            -> setCheckable ( true                     ) ;
  Match         -> addAction    ( aa                       ) ;
  MatchActions  << aa                                        ;
  ////////////////////////////////////////////////////////////
  aa = Languages->addAction ( tr("All languages") )          ;
  aa -> setCheckable   ( true )                              ;
  aa -> setChecked     ( true )                              ;
  aa -> setData        ( 0    )                              ;
  LanguageActions << aa                                      ;
  Languages -> addSeparator ( )                              ;
  for (int i=0;i<plan->languages.Supports.count();i++)       {
    int     language = plan->languages.Supports [ i        ] ;
    SUID    uuid     = plan->languages.Uuids    [ language ] ;
    QString name     = plan->languages.Names    [ uuid     ] ;
    aa  = Languages->addAction ( name     )                  ;
    aa -> setCheckable         ( true     )                  ;
    aa -> setData              ( language )                  ;
    if (language==vLanguageId) aa->setChecked(true)     ;
    LanguageActions << aa                                    ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  plan -> setFont ( this )                                   ;
  ////////////////////////////////////////////////////////////
  connect(this,SIGNAL(SendItems           ())                ,
          this,SLOT  (SearchResults       ())              ) ;
  connect(this,SIGNAL(SendPeople          ())                ,
          this,SLOT  (PeopleResults       ())              ) ;
  connect(this,SIGNAL(SendVideo           ())                ,
          this,SLOT  (VideoResults        ())              ) ;
  connect(this,SIGNAL(SendClip            ())                ,
          this,SLOT  (ClipResults         ())              ) ;
  connect(this,SIGNAL(SendDocuments       ())                ,
          this,SLOT  (DocumentResults     ())              ) ;
  connect(this,SIGNAL(SendUrls            ())                ,
          this,SLOT  (UrlResults          ())              ) ;
  connect(this,SIGNAL(SendKeywords        ())                ,
          this,SLOT  (KeywordResults      ())              ) ;
  connect(this,SIGNAL(SendMatchedKeywords ())                ,
          this,SLOT  (MatchKeywordResults ())              ) ;
  connect(this,SIGNAL(SendSqlQuery        ())                ,
          this,SLOT  (SqlQueryResults     ())              ) ;
}

void N::SearchBar::popupMenu(void)
{
  QRect  g = geometry  () ;
  QPoint p (0,g.height()) ;
  p = mapToGlobal(p)      ;
  Menu -> exec ( p )      ;
}

bool N::SearchBar::Relocation(void)
{
  nKickOut ( IsNull(Search) , false )      ;
  QRect g = geometry           ( )         ;
  QRect s = Search -> geometry ( )         ;
  int   w = g . width  ( ) - s . x ( ) - 4 ;
  int   h = g . height ( ) - 8             ;
  QRect S ( s . x ( ) , 4 , w , h )        ;
  Search -> setGeometry ( S )              ;
  return false                             ;
}

QString N::SearchBar::LanguageQuery(void)
{
  QString L = ""                                     ;
  Booleans . clear ( )                               ;
  for (int i=0;i<LanguageActions.count();i++)        {
    QAction * aa = LanguageActions[i]                ;
    int       id = aa->data().toInt()                ;
    bool      on = aa->isChecked()                   ;
    if (on) Booleans[id] = true                      ;
  }                                                  ;
  nKickOut ( Booleans . contains(0) , L )            ;
  QList<int> Languages = Booleans . keys ()          ;
  QStringList LN                                     ;
  for (int i=0;i<Languages.count();i++)              {
    LN << QString("language = %1").arg(Languages[i]) ;
  }                                                  ;
  nKickOut ( LN . count () <= 0 , L )                ;
  QString S = ""                                     ;
  if (LN.count()==1) S = LN[0] ; else                {
    S = LN . join (" or ")                           ;
  }                                                  ;
  L = QString("( %1 )") . arg ( S )                  ;
  return  L                                          ;
}

QString N::SearchBar::NameQuery(void)
{
  QString N                                      ;
  switch (Pattern)                               {
    case Exact : N = "name = :NAME"      ; break ;
    case SQL   : N = "name like :NAME"   ; break ;
    case Regex : N = "name regexp :NAME" ; break ;
  }                                              ;
  return N                                       ;
}

QString N::SearchBar::WildName(SqlConnection & SC,SUID uuid,int language,QString keyword)
{
  QString name =  ""                   ;
  name = SC.getName                    (
           PlanTable(Names) , "uuid"   ,
           language         ,  uuid  ) ;
  if (name.length()>0) return name     ;
  QStringList names                    ;
  QString     Q                        ;
  Q = SC.sql.SelectFrom                (
        "name"                         ,
        PlanTable(Names)               ,
        SC.WhereUuid(uuid)           ) ;
  SqlLoopNow ( SC , Q )                ;
    names << SC . String ( 0 )         ;
  SqlLoopErr ( SC , Q )                ;
  SqlLoopEnd ( SC , Q )                ;
  for (int i=0;i<names.count();i++)    {
    QString n = names[i]               ;
    if (n.length()>0)                  {
      if (n.contains(keyword))         {
        return n                       ;
      }                                ;
    }                                  ;
  }                                    ;
  if (names.count()>0)                 {
    for (int i=0;i<names.count();i++)  {
      QString n = names[i]             ;
      if (n.length()>0) return n       ;
    }                                  ;
  }                                    ;
  return name                          ;
}

void N::SearchBar::Find(QString keyword,enum SearchMode mode)
{
  nDropOut(Searching) ;
  Mode    = mode      ;
  Keyword = keyword   ;
}

void N::SearchBar::nameFinished(void)
{
  nDropOut(Searching)        ;
  Keyword = Editor->text ( ) ;
  start                  ( ) ;
}

void N::SearchBar::run(void)
{
  nDropOut ( Keyword.length() <= 0 )                     ;
  SearchMutex . lock   ( )                               ;
  Searching   = true                                     ;
  ////////////////////////////////////////////////////////
  QAction * mono    = ActionGroups[0]->checkedAction()   ;
  bool      generic = true                               ;
  ////////////////////////////////////////////////////////
  if (NotNull(mono))                                     {
    int id = mono->data().toInt()                        ;
    switch (id)                                          {
      case 302                                           :
        generic = false                                  ;
        Mode    = PeopleOnly                             ;
      break                                              ;
      case 303                                           :
        generic = false                                  ;
        Mode    = VideoOnly                              ;
      break                                              ;
      case 304                                           :
        generic = false                                  ;
        Mode    = ClipOnly                               ;
      break                                              ;
      case 305                                           :
        generic = false                                  ;
        Mode    = DocumentOnly                           ;
      break                                              ;
      case 306                                           :
        generic = false                                  ;
        Mode    = KeywordOnly                            ;
      break                                              ;
      case 307                                           :
        generic = false                                  ;
        Mode    = SystemOnly                             ;
      break                                              ;
      case 308                                           :
        generic = false                                  ;
        Mode    = DatabaseOnly                           ;
      break                                              ;
      case 309                                           :
        generic = false                                  ;
        Mode    = PageOnly                               ;
      break                                              ;
    }                                                    ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  if (generic)                                           {
    if (ItemActions [0]->isChecked())                    {
      Mode    = Everything                               ;
    } else                                               {
      if (ItemActions [1]->isChecked())                  {
        Mode  = NameIndex                                ;
      }                                                  ;
    }                                                    ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  if (MatchActions[0]->isChecked()) Pattern = Exact ; else
  if (MatchActions[1]->isChecked()) Pattern = SQL   ; else
  if (MatchActions[2]->isChecked()) Pattern = Regex      ;
  ////////////////////////////////////////////////////////
  switch (Mode)                                          {
    nFastCast ( Everything   , SearchEverything() )      ;
    nFastCast ( NameIndex    , SearchName      () )      ;
    nFastCast ( PeopleOnly   , SearchPeople    () )      ;
    nFastCast ( VideoOnly    , SearchVideo     () )      ;
    nFastCast ( ClipOnly     , SearchClip      () )      ;
    nFastCast ( DocumentOnly , SearchDocument  () )      ;
    nFastCast ( KeywordOnly  , SearchKeyword   () )      ;
    nFastCast ( SystemOnly   , SearchSystem    () )      ;
    nFastCast ( DatabaseOnly , SearchSql       () )      ;
    nFastCast ( PageOnly     , SearchPage      () )      ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  Keyword     = ""                                       ;
  Searching   = false                                    ;
  SearchMutex . unlock ( )                               ;
}

void N::SearchBar::SearchEverything(void)
{
}

void N::SearchBar::SearchName(void)
{
  bool  success = true                                      ;
  UUIDs Uuids                                               ;
  RMAPs Types                                               ;
  NAMEs Names                                               ;
  ResultUuids . clear ( )                                   ;
  ResultTypes . clear ( )                                   ;
  ResultNames . clear ( )                                   ;
  ResultTitle . clear ( )                                   ;
  QString m = tr("Search %1").arg(Keyword)                  ;
  plan   -> Talk       ( m     )                            ;
  Search -> setEnabled ( false )                            ;
  EnterSQL(SC,plan->sql)                                    ;
    QString Q                                               ;
    QString K = Keyword                                     ;
    UUIDs   TUIDs                                           ;
    SUID    uuid                                            ;
    QString L = LanguageQuery ()                            ;
    QString N = NameQuery     ()                            ;
    /////////////////////////////////////////////////////////
    //               Search contents in names              //
    /////////////////////////////////////////////////////////
    Q  = "select uuid,name from "                           ;
    Q += PlanTable(Names)                                   ;
    Q += " where "                                          ;
    if (L.length()>0)                                       {
      Q += L                                                ;
      Q += " and "                                          ;
    }                                                       ;
    Q += N                                                  ;
    if (Pattern==SQL)                                       {
      if (!K.contains('%')) K = "%" + K + "%"               ;
    }                                                       ;
    SC.Prepare(Q)                                           ;
    SC.Bind("name",K.toUtf8())                              ;
    if (SC.Exec()) while (SC.Next())                        {
      QString name   = SC . String (1)                      ;
      uuid           = SC . Uuid   (0)                      ;
      if (!Uuids.contains(uuid))                            {
        Uuids         << uuid                               ;
        if (name.length()>0) Names [ uuid ] = name          ;
      }                                                     ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    //           Search contents in identifiers            //
    /////////////////////////////////////////////////////////
    Q  = "select uuid from "                                ;
    Q += PlanTable(Identifiers)                             ;
    Q += " where "                                          ;
    Q += N                                                  ;
    SC.Prepare(Q)                                           ;
    SC.Bind("name",K.toUtf8())                              ;
    if (SC.Exec()) while (SC.Next())                        {
      uuid = SC . Uuid (0)                                  ;
      if (!Uuids.contains(uuid)) Uuids << uuid              ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    foreach (uuid,Uuids)                                    {
      Q = SC.sql.SelectFrom                                 (
            "uuid",PlanTable(NameMaps)                      ,
            QString("where name = %1").arg(uuid)          ) ;
      if (SC.Fetch(Q))                                      {
        SUID tuid = SC.Uuid(0)                              ;
        if (!TUIDs.contains(tuid))                          {
          TUIDs << tuid                                     ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
    foreach (uuid,TUIDs)                                    {
      if (!Uuids.contains(uuid))                            {
        QString name                                        ;
        Uuids << uuid                                       ;
        name = WildName(SC,uuid,vLanguageId,Keyword)   ;
        if (name.length()>0) Names [ uuid ] = name          ;
      }                                                     ;
    }                                                       ;
    TUIDs . clear ( )                                       ;
    foreach (uuid,Uuids)                                    {
      SUID type = 0                                         ;
      Q  = "select type from "                              ;
      Q += PlanTable(MajorUuid)                             ;
      Q += " "                                              ;
      Q += SC.WhereUuid(uuid)                               ;
      Q += " ;"                                             ;
      if (SC.Fetch(Q)) type = SC.Uuid(0)                    ;
      Types[uuid] = (int)type                               ;
      if (type==Types::Tag)                       {
        if (!TUIDs.contains(uuid)) TUIDs << uuid            ;
      }                                                     ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    //               Search relational items               //
    /////////////////////////////////////////////////////////
    if (TUIDs.count()>0)                                    {
      foreach (uuid,TUIDs)                                  {
        Q = SC.sql.SelectFrom                               (
              "second",PlanTable(Groups)                    ,
              SC.sql.Where(3,"first","t1","relation"    ) ) ;
        SC.Prepare ( Q )                                    ;
        SC.Bind("first"   ,uuid                           ) ;
        SC.Bind("t1"      ,Types::Tag           ) ;
        SC.Bind("relation",Groups::Subordination) ;
        if (SC.Exec()) while (SC.Next())                    {
          SUID zuid = SC.Uuid(0)                            ;
          if (!Uuids.contains(zuid)) Uuids << zuid          ;
        }                                                   ;
      }                                                     ;
      foreach (uuid,Uuids)                                  {
        SUID type = 0                                       ;
        Q  = "select type from "                            ;
        Q += PlanTable(MajorUuid)                           ;
        Q += " "                                            ;
        Q += SC.WhereUuid(uuid)                             ;
        Q += " ;"                                           ;
        if (SC.Fetch(Q)) type = SC.Uuid(0)                  ;
        Types[uuid] = (int)type                             ;
      }                                                     ;
    }                                                       ;
    /////////////////////////////////////////////////////////
    //                    Search names                     //
    /////////////////////////////////////////////////////////
    foreach (uuid,Uuids)                                    {
      if (!Names.contains(uuid))                            {
        QString name                                        ;
        name = WildName(SC,uuid,vLanguageId,Keyword)   ;
        if (name.length()>0)                                {
          Names[uuid] = name                                ;
        } else                                              {
          UUIDs NUIDs                                       ;
          SUID  nuid  = 0                                   ;
          NUIDs = SC.Uuids                                  (
                    PlanTable(NameMaps)                     ,
                    "name"                                  ,
                    SC.WhereUuid(uuid)                    ) ;
          QString n = ""                                    ;
          for (int i=0;n.length()<=0 && i<NUIDs.count();i++){
            nuid = NUIDs[i]                                 ;
            n = WildName(SC,nuid,vLanguageId,Keyword ) ;
          }                                                 ;
          if (n.length()>0) Names[uuid] = n                 ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
  LeaveSQL(SC,plan->sql)                                    ;
  if (Uuids.count()<=0) success = false                     ;
  if (success) Alert ( Done  )                              ;
          else Alert ( Error )                              ;
  Search -> setEnabled ( true  )                            ;
  if (success)                                              {
    QString R                                               ;
    R = tr("Search result for [%1]").arg(Keyword)           ;
    ResultTitle = R                                         ;
    ResultUuids = Uuids                                     ;
    ResultTypes = Types                                     ;
    ResultNames = Names                                     ;
    emit SendItems ( )                                      ;
  }                                                         ;
}

void N::SearchBar::SearchPeople(void)
{
  bool  success = true                                      ;
  UUIDs Uuids                                               ;
  RMAPs Types                                               ;
  NAMEs Names                                               ;
  ResultUuids . clear ( )                                   ;
  ResultTypes . clear ( )                                   ;
  ResultNames . clear ( )                                   ;
  ResultTitle . clear ( )                                   ;
  QString m = tr("Search %1").arg(Keyword)                  ;
  plan   -> Talk       ( m     )                            ;
  Search -> setEnabled ( false )                            ;
  EnterSQL(SC,plan->sql)                                    ;
    QString Q                                               ;
    QString K = Keyword                                     ;
    UUIDs   TUIDs                                           ;
    SUID    uuid                                            ;
    QString L = LanguageQuery ()                            ;
    QString N = NameQuery     ()                            ;
    Q  = "select uuid from "                                ;
    Q += PlanTable(Names)                                   ;
    Q += " where "                                          ;
    if (L.length()>0)                                       {
      Q += L                                                ;
      Q += " and "                                          ;
    }                                                       ;
    Q += N                                                  ;
    if (Pattern==SQL)                                       {
      if (!K.contains('%')) K = "%" + K + "%"               ;
    }                                                       ;
    SC.Prepare(Q)                                           ;
    SC.Bind("name",K.toUtf8())                              ;
    if (SC.Exec()) while (SC.Next())                        {
      uuid = SC . Uuid   (0)                                ;
      if (!Uuids.contains(uuid)) Uuids << uuid              ;
    }                                                       ;
    foreach (uuid,Uuids)                                    {
      Q = SC.sql.SelectFrom                                 (
            "uuid",PlanTable(NameMaps)                      ,
            QString("where name = %1").arg(uuid)          ) ;
      if (SC.Fetch(Q))                                      {
        SUID tuid = SC.Uuid(0)                              ;
        if (!TUIDs.contains(tuid))                          {
          TUIDs << tuid                                     ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
    Uuids . clear ( )                                       ;
    foreach (uuid,TUIDs)                                    {
      SUID type = 0                                         ;
      Q  = "select type from "                              ;
      Q += PlanTable(MajorUuid)                             ;
      Q += " "                                              ;
      Q += SC.WhereUuid(uuid)                               ;
      Q += " ;"                                             ;
      if (SC.Fetch(Q)) type = SC.Uuid(0)                    ;
      if (type==Types::People)                    {
        if (!Uuids.contains(uuid)) Uuids << uuid            ;
      }                                                     ;
    }                                                       ;
  LeaveSQL(SC,plan->sql)                                    ;
  if (Uuids.count()<=0) success = false                     ;
  if (success) Alert ( Done  )                              ;
          else Alert ( Error )                              ;
  Search -> setEnabled ( true  )                            ;
  if (success)                                              {
    QString R                                               ;
    R = tr("Search result for [%1]").arg(Keyword)           ;
    ResultTitle = R                                         ;
    ResultUuids = Uuids                                     ;
    emit SendPeople ( )                                     ;
  }                                                         ;
}

void N::SearchBar::SearchVideo(void)
{
  bool  success = true                                      ;
  UUIDs Uuids                                               ;
  RMAPs Types                                               ;
  NAMEs Names                                               ;
  ResultUuids . clear ( )                                   ;
  ResultTypes . clear ( )                                   ;
  ResultNames . clear ( )                                   ;
  ResultTitle . clear ( )                                   ;
  QString m = tr("Search %1").arg(Keyword)                  ;
  plan   -> Talk       ( m     )                            ;
  Search -> setEnabled ( false )                            ;
  EnterSQL(SC,plan->sql)                                    ;
    QString Q                                               ;
    QString K = Keyword                                     ;
    UUIDs   TUIDs                                           ;
    SUID    uuid                                            ;
    QString L = LanguageQuery ()                            ;
    QString N = NameQuery     ()                            ;
    Q  = "select uuid from "                                ;
    Q += PlanTable(Names)                                   ;
    Q += " where "                                          ;
    if (L.length()>0)                                       {
      Q += L                                                ;
      Q += " and "                                          ;
    }                                                       ;
    Q += N                                                  ;
    if (Pattern==SQL)                                       {
      if (!K.contains('%')) K = "%" + K + "%"               ;
    }                                                       ;
    SC.Prepare(Q)                                           ;
    SC.Bind("name",K.toUtf8())                              ;
    if (SC.Exec()) while (SC.Next())                        {
      uuid = SC . Uuid   (0)                                ;
      if (!Uuids.contains(uuid)) Uuids << uuid              ;
    }                                                       ;
    TUIDs = Uuids                                           ;
    Uuids . clear ( )                                       ;
    foreach (uuid,TUIDs)                                    {
      SUID type = 0                                         ;
      Q  = "select type from "                              ;
      Q += PlanTable(MajorUuid)                             ;
      Q += " "                                              ;
      Q += SC.WhereUuid(uuid)                               ;
      Q += " ;"                                             ;
      if (SC.Fetch(Q)) type = SC.Uuid(0)                    ;
      if (type==Types::Album)                     {
        if (!Uuids.contains(uuid)) Uuids << uuid            ;
      }                                                     ;
    }                                                       ;
  LeaveSQL(SC,plan->sql)                                    ;
  if (Uuids.count()<=0) success = false                     ;
  if (success) Alert ( Done  )                              ;
          else Alert ( Error )                              ;
  Search -> setEnabled ( true  )                            ;
  if (success)                                              {
    QString R                                               ;
    R = tr("Search result for [%1]").arg(Keyword)           ;
    ResultTitle = R                                         ;
    ResultUuids = Uuids                                     ;
    emit SendVideo ( )                                      ;
  }                                                         ;
}

void N::SearchBar::SearchClip(void)
{
  bool  success = true                                      ;
  UUIDs Uuids                                               ;
  RMAPs Types                                               ;
  NAMEs Names                                               ;
  ResultUuids . clear ( )                                   ;
  ResultTypes . clear ( )                                   ;
  ResultNames . clear ( )                                   ;
  ResultTitle . clear ( )                                   ;
  QString m = tr("Search %1").arg(Keyword)                  ;
  plan   -> Talk       ( m     )                            ;
  Search -> setEnabled ( false )                            ;
  EnterSQL(SC,plan->sql)                                    ;
    QString Q                                               ;
    QString K = Keyword                                     ;
    UUIDs   TUIDs                                           ;
    SUID    uuid                                            ;
    QString L = LanguageQuery ()                            ;
    QString N = NameQuery     ()                            ;
    Q  = "select uuid from "                                ;
    Q += PlanTable(Names)                                   ;
    Q += " where "                                          ;
    if (L.length()>0)                                       {
      Q += L                                                ;
      Q += " and "                                          ;
    }                                                       ;
    Q += N                                                  ;
    if (Pattern==SQL)                                       {
      if (!K.contains('%')) K = "%" + K + "%"               ;
    }                                                       ;
    SC.Prepare(Q)                                           ;
    SC.Bind("name",K.toUtf8())                              ;
    if (SC.Exec()) while (SC.Next())                        {
      uuid = SC . Uuid   (0)                                ;
      if (!Uuids.contains(uuid)) Uuids << uuid              ;
    }                                                       ;
    foreach (uuid,Uuids)                                    {
      Q = SC.sql.SelectFrom                                 (
            "uuid",PlanTable(NameMaps)                      ,
            QString("where name = %1").arg(uuid)          ) ;
      if (SC.Fetch(Q))                                      {
        SUID tuid = SC.Uuid(0)                              ;
        if (!TUIDs.contains(tuid))                          {
          TUIDs << tuid                                     ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
    Uuids . clear ( )                                       ;
    foreach (uuid,TUIDs)                                    {
      SUID type = 0                                         ;
      Q  = "select type from "                              ;
      Q += PlanTable(MajorUuid)                             ;
      Q += " "                                              ;
      Q += SC.WhereUuid(uuid)                               ;
      Q += " ;"                                             ;
      if (SC.Fetch(Q)) type = SC.Uuid(0)                    ;
      if (type==Types::Video)                     {
        if (!Uuids.contains(uuid)) Uuids << uuid            ;
      }                                                     ;
    }                                                       ;
  LeaveSQL(SC,plan->sql)                                    ;
  if (Uuids.count()<=0) success = false                     ;
  if (success) Alert ( Done  )                              ;
          else Alert ( Error )                              ;
  Search -> setEnabled ( true  )                            ;
  if (success)                                              {
    QString R                                               ;
    R = tr("Search result for [%1]").arg(Keyword)           ;
    ResultTitle = R                                         ;
    ResultUuids = Uuids                                     ;
    emit SendClip ( )                                       ;
  }                                                         ;
}

void N::SearchBar::SearchDocument(void)
{
  bool  success = true                                          ;
  UUIDs Uuids                                                   ;
  UUIDs Kuids                                                   ;
  NAMEs Names                                                   ;
  ResultUuids . clear ( )                                       ;
  ResultTypes . clear ( )                                       ;
  ResultNames . clear ( )                                       ;
  ResultTitle . clear ( )                                       ;
  QString m = tr("Search %1").arg(Keyword)                      ;
  plan   -> Talk       ( m     )                                ;
  Search -> setEnabled ( false )                                ;
  EnterSQL(SC,plan->sql)                                        ;
    QString Q                                                   ;
    QString K = Keyword                                         ;
    UUIDs   TUIDs                                               ;
    SUID    uuid                                                ;
    QString N                                                   ;
    switch (Pattern)                                            {
      case Exact : N = "text = :TEXT"      ; break              ;
      case SQL   : N = "text like :TEXT"   ; break              ;
      case Regex : N = "text regexp :TEXT" ; break              ;
    }                                                           ;
    Q = QString("select uuid from %1 where %2 order by id asc ;")
            .arg(PlanTable(PlainText)).arg(N)                   ;
    TUIDs . clear ( )                                           ;
    SC . Prepare ( Q          )                                 ;
    SC . Bind    ( "text" , K )                                 ;
    if (SC.Exec()) while (SC.Next())                            {
      SUID fuid = SC.Uuid(0)                                    ;
      if (!TUIDs.contains(fuid)) TUIDs << fuid                  ;
    }                                                           ;
    foreach (uuid,TUIDs)                                        {
      Q = SC.sql.SelectFrom                                     (
            "document" , PlanTable(DocumentGroups)              ,
            QString("where fileuuid = %1").arg(uuid)          ) ;
      SqlLoopNow ( SC , Q )                                     ;
        SUID duid = SC.Uuid(0)                                  ;
        if (!Uuids.contains(duid)) Uuids << duid                ;
      SqlLoopErr ( SC , Q )                                     ;
      SqlLoopEnd ( SC , Q )                                     ;
      Q = SC.sql.SelectFrom                                     (
            "first",PlanTable(Triples)                          ,
            QString("where second = %1 and type = %2 "          )
            .arg(uuid).arg(Groups::Explain)         ) ;
      SqlLoopNow ( SC , Q )                                     ;
        SUID kuid = SC.Uuid(0)                                  ;
        if (!Kuids.contains(kuid)) Uuids << kuid                ;
      SqlLoopErr ( SC , Q )                                     ;
      SqlLoopEnd ( SC , Q )                                     ;
    }                                                           ;
    if (Kuids.count()>0)                                        {
      foreach (uuid,Kuids)                                      {
        Q = SC.sql.SelectFrom                                   (
             "name"                                             ,
             PlanTable(KeywordNames)                            ,
             QString("where uuid = %1 order by id asc limit 0,1")
             .arg(uuid)                                       ) ;
        if (SC.Fetch(Q))                                        {
          SUID nuid = SC.Uuid(0)                                ;
          Q = SC.sql.SelectFrom("name",PlanTable(Names)         ,
             QString("where uuid = %1 order by id asc limit 0,1")
             .arg(nuid)                                       ) ;
          if (SC.Fetch(Q)) Names[uuid] = SC.String(0)           ;
        }                                                       ;
      }                                                         ;
    }                                                           ;
  LeaveSQL(SC,plan->sql)                                        ;
  if (Uuids.count()<=0 && Names.count()<=0)                     {
    success = false                                             ;
  }                                                             ;
  if (success) Alert ( Done  )                                  ;
          else Alert ( Error )                                  ;
  Search -> setEnabled ( true  )                                ;
  if (success)                                                  {
    QString R                                                   ;
    R = tr("Search result for [%1]").arg(Keyword)               ;
    ResultTitle = R                                             ;
    ResultUuids = Uuids                                         ;
    ResultNames = Names                                         ;
    if (ResultUuids.count()>0)                                  {
      emit SendDocuments()                                      ;
    }                                                           ;
    if (ResultNames.count()>0)                                  {
      emit SendMatchedKeywords()                                ;
    }                                                           ;
  }                                                             ;
}

bool N::SearchBar::ListKeywordName(SqlConnection & SC,SUID uuid,QByteArray & Body)
{
  QString Q                                      ;
  QString m                                      ;
  UUIDs Uuids                                    ;
  SUID  nuid                                     ;
  Q = SC.sql.SelectFrom                          (
        "name"                                   ,
        PlanTable(KeywordNames)                  ,
        QString("where uuid = %1 order by id asc")
        .arg(uuid)                             ) ;
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

bool N::SearchBar::ListDictionary(SqlConnection & SC,SUID uuid,QByteArray & Body)
{
  DocumentManager DM(plan)                                      ;
  QString Q                                                     ;
  QString m                                                     ;
  UUIDs   Uuids                                                 ;
  SMAPs   LMaps                                                 ;
  SUID    puid                                                  ;
  ListKeywordName(SC,uuid,Body)                                 ;
  m = tr("\n\n")                                                ;
  Body.append(m.toUtf8())                                       ;
  ///////////////////////////////////////////////////////////////
  Uuids . clear ( )                                             ;
  LMaps . clear ( )                                             ;
  Q = SC.sql.SelectFrom("second,third",PlanTable(Triples)       ,
        QString("where first = %1 and type = %2 order by id asc")
        .arg(uuid).arg(1001)                                  ) ;
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
        .arg(uuid).arg(Groups::Explain)             ) ;
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

void N::SearchBar::SearchKeyword(void)
{
  DictionaryAlign  = Qt::Vertical                           ;
  DictionaryResult . clear ( )                              ;
  bool  success = true                                      ;
  UUIDs Uuids                                               ;
  NAMEs Names                                               ;
  QString m = tr("Search %1").arg(Keyword)                  ;
  plan   -> Talk       ( m     )                            ;
  Search -> setEnabled ( false )                            ;
  EnterSQL(SC,plan->sql)                                    ;
    QString Q                                               ;
    QString K = Keyword                                     ;
    UUIDs   TUIDs                                           ;
    SUID    uuid                                            ;
    QString L = LanguageQuery ()                            ;
    QString N = NameQuery     ()                            ;
    Q  = "select uuid,name from "                           ;
    Q += PlanTable(Names)                                   ;
    Q += " where "                                          ;
    if (L.length()>0)                                       {
      Q += L                                                ;
      Q += " and "                                          ;
    }                                                       ;
    Q += N                                                  ;
    if (Pattern==SQL)                                       {
      if (!K.contains('%')) K = "%" + K + "%"               ;
    }                                                       ;
    SC.Prepare(Q)                                           ;
    SC.Bind("name",K.toUtf8())                              ;
    if (SC.Exec()) while (SC.Next())                        {
      QString name   = SC . String (1)                      ;
      uuid           = SC . Uuid   (0)                      ;
      if (!Uuids.contains(uuid))                            {
        Uuids         << uuid                               ;
        if (name.length()>0) Names [ uuid ] = name          ;
      }                                                     ;
    }                                                       ;
    foreach (uuid,Uuids)                                    {
      Q = SC.sql.SelectFrom                                 (
            "uuid",PlanTable(KeywordNames)                  ,
            QString("where name = %1").arg(uuid)          ) ;
      if (SC.Fetch(Q))                                      {
        SUID tuid = SC.Uuid(0)                              ;
        if (!TUIDs.contains(tuid))                          {
          TUIDs << tuid                                     ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
    if (TUIDs.count()>0)                                    {
      m = tr("%1 items found").arg(TUIDs.count())           ;
      plan -> Talk ( m )                                    ;
      if (TUIDs.count()>50)                                 {
        m = tr("Retrieve explanations")                     ;
        plan -> Talk ( m )                                  ;
      }                                                     ;
      foreach (uuid,TUIDs)                                  {
        if (ListKeywordName(SC,uuid,DictionaryResult))      {
          DictionaryResult.append("\n")                     ;
        }                                                   ;
      }                                                     ;
      DictionaryResult.append("\n")                         ;
      DictionaryResult.append("\n")                         ;
      QString m = tr("[Explanation]\n")                     ;
      DictionaryResult.append(m.toUtf8())                   ;
      DictionaryResult.append("\n")                         ;
      DictionaryResult.append("\n")                         ;
      foreach (uuid,TUIDs)                                  {
        if (ListDictionary(SC,uuid,DictionaryResult))       {
          DictionaryResult.append("\n\n")                   ;
        }                                                   ;
      }                                                     ;
    }                                                       ;
  LeaveSQL(SC,plan->sql)                                    ;
  if (DictionaryResult.size()<=0) success = false           ;
  if (success) Alert ( Done  )                              ;
          else Alert ( Error )                              ;
  Search -> setEnabled ( true  )                            ;
  if (success)                                              {
    ResultTitle = tr("Search result for [%1]").arg(Keyword) ;
    emit SendKeywords ( )                                   ;
  }                                                         ;
}

void N::SearchBar::SearchSystem(void)
{
}

void N::SearchBar::SearchSql(void)
{
  QStringList  K = Keyword.split(' ')          ;
  SqlQueryString = Keyword                     ;
  if (K.count()<=0) return                     ;
  QString F = K[0]                             ;
  F = F.toLower()                              ;
  QStringList R                                ;
  R << "select"                                ;
  R << "explain"                               ;
  R << "describe"                              ;
  R << "show"                                  ;
  R << "set"                                   ;
  if (R.contains(F))                           {
    emit SendSqlQuery()                        ;
    return                                     ;
  }                                            ;
  QString m = tr("Execute SQL command")        ;
  plan   -> Talk       ( m     )               ;
  Search -> setEnabled ( false )               ;
  EnterSQL(SC,plan->sql)                       ;
    if (SC.Query(Keyword))                     {
      plan->Talk(tr("Execution successfully")) ;
    } else                                     {
      plan->Talk(tr("Execution failure"     )) ;
    }                                          ;
  LeaveSQL(SC,plan->sql)                       ;
  Search -> setEnabled ( true  )               ;
}

void N::SearchBar::SearchPage(void)
{
  if (Keyword.count()<=0) return                               ;
  bool  success = true                                         ;
  ResultTitle = Keyword                                        ;
  ResultUuids . clear ( )                                      ;
  QString m = tr("Search web pages")                           ;
  plan   -> Talk       ( m     )                               ;
  Search -> setEnabled ( false )                               ;
  SetsAlgebra            NSA                                   ;
  QMap<QString,Binary *> Binaries                              ;
  if (!NSA.Decode(Keyword))                                    {
    plan -> Talk  ( 45 , tr("Sets algebra syntax error") )     ;
    plan -> Debug ( 45 , Keyword                         )     ;
    Alert         ( Error                                )     ;
    return                                                     ;
  }                                                            ;
  for (int i=0;i<NSA.Inputs.count();i++)                       {
    QString S = NSA.Inputs[i]                                  ;
    if (S.contains("=") || S.contains("@"))                    {
      bool      correct = true                                 ;
      Binary * nb      = new Binary ( )                        ;
      Binaries[S] = nb                                         ;
      if (Binaries[S]->Pair(S))                                {
        QString n = Binaries[S]->nodes[0]->name                ;
        n = n.toLower()                                        ;
        if (n=="nothing" ) correct = true                 ; else
        if (n=="tag"     ) correct = true                 ; else
        if (n=="from"    ) correct = true                 ; else
        if (n=="site"    ) correct = true                 ; else
        if (n=="tld"     ) correct = true                 ; else
        if (n=="sld"     ) correct = true                 ; else
        if (n=="protocol") correct = true                 ; else
        if (n=="url"     ) correct = true                 ; else
        if (n=="page"    ) correct = true                 ; else
        if (n=="uri"     ) correct = true                 ; else
        if (n=="title"   ) correct = true                 ; else
        if (n=="mimetype") correct = true                 ; else
        if (n=="encoding") correct = true                 ; else
                           correct = false                     ;
      } else correct = false                                   ;
      if (!correct)                                            {
        plan -> Talk  ( 45 , tr("Sets algebra syntax error") ) ;
        plan -> Debug ( 45 , Keyword                         ) ;
        Alert         ( Error                                ) ;
        return                                                 ;
      }                                                        ;
    }                                                          ;
  }                                                            ;
  EnterSQL(SC,plan->sql)                                       ;
    QString Q                                                  ;
    for (int i=0;i<NSA.Inputs.count();i++)                     {
      QString n = NSA.Inputs[i]                                ;
      if (Binaries.contains(n))                                {
        QString A      = Binaries[n]->nodes[0]->name           ;
        bool    column = false                                 ;
        A = A.toLower()                                        ;
        if (A=="protocol") column = true                  ; else
        if (A=="url"     ) column = true                  ; else
        if (A=="page"    ) column = true                  ; else
        if (A=="uri"     ) column = true                  ; else
        if (A=="title"   ) column = true                  ; else
        if (A=="mimetype") column = true                  ; else
        if (A=="encoding") column = true                       ;
        if (column)                                            {
          if (UrlFromColumn(SC,Binaries[n],NSA.Uuids[n]))      {
          }                                                    ;
        } else                                                 {
          if (A=="tag" )                                       {
            if (UrlFromTag                                     (
                  SC                                           ,
                  vLanguageId                             ,
                  Types::URL                         ,
                  Binaries[n]                                  ,
                  NSA.Uuids[n])                              ) {
            }                                                  ;
          } else
          if (A=="from")                                       {
            if (UrlFromParent(SC,Binaries[n],NSA.Uuids[n]))    {
            }                                                  ;
          } else
          if (A=="site")                                       {
            if (UrlFromSite(SC,Binaries[n],NSA.Uuids[n]))      {
            }                                                  ;
          } else
          if (A=="tld" )                                       {
            if (UrlFromTLD(SC,Binaries[n],NSA.Uuids[n]))       {
            }                                                  ;
          } else
          if (A=="sld" )                                       {
            if (UrlFromSLD(SC,Binaries[n],NSA.Uuids[n]))       {
            }                                                  ;
          } else                                               {
            UUIDs EmptyUuid                                    ;
            NSA.Uuids[n] = EmptyUuid                           ;
          }                                                    ;
        }                                                      ;
        delete Binaries[n]                                     ;
      } else                                                   {
        UUIDs   Uuids                                          ;
        if (UrlFromPage(SC,n,Uuids))                           {
          NSA.Uuids[n] = Uuids                                 ;
        }                                                      ;
      }                                                        ;
    }                                                          ;
    Binaries . clear ( )                                       ;
    ////////////////////////////////////////////////////////////
    NSA.Execute(ResultUuids)                                   ;
  LeaveSQL(SC,plan->sql)                                       ;
  Search -> setEnabled ( true  )                               ;
  if (ResultUuids.count()<=0) success = false                  ;
  if (success) Alert ( Done  )                                 ;
          else Alert ( Error )                                 ;
  Search -> setEnabled ( true  )                               ;
  if (success) emit SendUrls ( )                               ;
}

bool N::SearchBar::UrlFromPage (
       SqlConnection & SC      ,
       QString         Page    ,
       UUIDs         & Uuids   )
{
  QString Q                      ;
  Q = SC.sql.SelectFrom          (
        "uuid"                   ,
        PlanTable(WebPages)      ,
        SC.sql.Where(1,"page") ) ;
  SC.Prepare(Q)                  ;
  SC.Bind("page",Page.toUtf8() ) ;
  if (SC.Exec() && SC.Next())    {
    Uuids << SC.Uuid(0)          ;
    return true                  ;
  }                              ;
  return false                   ;
}

bool N::SearchBar::UrlFromTag    (
       SqlConnection & SC       ,
       int             Language ,
       int             TagType  ,
       Binary        * binary   ,
       UUIDs         & Uuids    )
{
  QString Q                                          ;
  QString name = binary->nodes[1]->name              ;
  nKickOut ( name.length() <= 0 , false )            ;
  UUIDs Nuids                                        ;
  UUIDs Tuids                                        ;
  UUIDs Muids                                        ;
  SUID  uuid                                         ;
  ////////////////////////////////////////////////////
  Q = SC.sql.SelectFrom                              (
        "uuid"                                       ,
        PlanTable(Names)                             ,
        SC.sql.Where(2,"language","name")   )        ;
  SC . Prepare ( Q                          )        ;
  SC . Bind    ( "language" , Language      )        ;
  SC . Bind    ( "name"     , name.toUtf8() )        ;
  if (SC.Exec())                                     {
    while (SC.Next())                                {
      Nuids << SC.Uuid(0)                            ;
    }                                                ;
  }                                                  ;
  if (Nuids.count()<=0) return false                 ;
  ////////////////////////////////////////////////////
  for (int i=0;i<Nuids.count();i++)                  {
    int type = 0                                     ;
    uuid = Nuids[i]                                  ;
    Q = SC.sql.SelectFrom                            (
          "type"                                     ,
          PlanTable(MajorUuid)                       ,
          SC.WhereUuid(uuid)                       ) ;
    if (SC.Fetch(Q)) type = SC.Int(0)                ;
    if (type==Types::Tag)                  {
      Tuids << uuid                                  ;
    }                                                ;
  }                                                  ;
  if (Tuids.count()<=0) return false                 ;
  ////////////////////////////////////////////////////
  for (int i=0;i<Tuids.count();i++)                  {
    int type = 0                                     ;
    uuid = Tuids[i]                                  ;
    Q = SC.sql.SelectFrom                            (
          "type"                                     ,
          PlanTable(Tags)                            ,
          SC.WhereUuid(uuid)                       ) ;
    if (SC.Fetch(Q)) type = SC.Int(0)                ;
    if (type==TagType)                               {
      Muids << uuid                                  ;
    }                                                ;
  }                                                  ;
  ////////////////////////////////////////////////////
  if (Muids.count()<=0) return false                 ;
  ////////////////////////////////////////////////////
  for (int i=0;i<Muids.count();i++)                  {
    uuid = Muids[i]                                  ;
    Q = SC.sql.SelectFrom                            (
          "second"                                   ,
          PlanTable(Groups)                          ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4"                )
          .arg(uuid                                  )
          .arg(Types::Tag                  )
          .arg(Types::URL                  )
          .arg(Groups::Subordination       )
        )                                            ;
    if (SC.Query(Q)) while (SC.Next())               {
      SUID puid = SC.Uuid(0)                         ;
      if (!Uuids.contains(puid)) Uuids << puid       ;
    }                                                ;
  }                                                  ;
  return true                                        ;
}

bool N::SearchBar::UrlFromParent (
       SqlConnection & SC        ,
       Binary        * binary    ,
       UUIDs         & Uuids     )
{
  QString Q                                               ;
  QString Pair                                            ;
  QString Where                                           ;
  UUIDs   Fuids                                           ;
  SUID    uuid                                            ;
  QString B = binary->nodes[1]->name                      ;
  int     R = binary->relation.linkType                   ;
  if (R==Groups::Equivalent)                    {
    Pair = "page = :PAGE"                                 ;
  } else
  if (R==Groups::Contains)                      {
    Pair = "page like :PAGE"                              ;
    if (!B.contains('%'))                                 {
      B . prepend ( '%' )                                 ;
      B . append  ( '%' )                                 ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  Where = QString("where %1").arg(Pair)                   ;
  Q = SC.sql.SelectFrom("uuid",PlanTable(WebPages),Where) ;
  SC . Prepare( Q                   )                     ;
  SC . Bind   ( "page" , B.toUtf8() )                     ;
  if (SC.Exec()) while (SC.Next())                        {
    uuid = SC.Uuid(0)                                     ;
    if (!Fuids.contains(uuid)) Fuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;i<Fuids.count();i++)                       {
    uuid = Fuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "uuid"                                          ,
          PlanTable(WebPages)                             ,
          QString("where fromPage = %1").arg(uuid) )      ;
    if (SC.Query(Q)) while (SC.Next())                    {
      uuid = SC.Uuid(0)                                   ;
      if (!Uuids.contains(uuid)) Uuids << uuid            ;
    }                                                     ;
  }                                                       ;
  return true                                             ;
}

bool N::SearchBar::UrlFromSite (
       SqlConnection & SC      ,
       Binary        * binary  ,
       UUIDs         & Uuids   )
{
  QString Q                                               ;
  QString Pair                                            ;
  QString Where                                           ;
  UUIDs   Fuids                                           ;
  SUID    uuid                                            ;
  QString B = binary->nodes[1]->name                      ;
  int     R = binary->relation.linkType                   ;
  if (R==Groups::Equivalent)                    {
    Pair = "url = :URL"                                   ;
  } else
  if (R==Groups::Contains)                      {
    Pair = "url like :URL"                                ;
    if (!B.contains('%'))                                 {
      B . prepend ( '%' )                                 ;
      B . append  ( '%' )                                 ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  Where = QString("where %1").arg(Pair)                   ;
  Q = SC.sql.SelectFrom                                   (
        "uuid"                                            ,
        PlanTable(SiteDepot)                              ,
        Where                                           ) ;
  SC . Prepare( Q                   )                     ;
  SC . Bind   ( "url" , B.toUtf8() )                      ;
  if (SC.Exec()) while (SC.Next())                        {
    uuid = SC.Uuid(0)                                     ;
    if (!Fuids.contains(uuid)) Fuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;i<Fuids.count();i++)                       {
    uuid = Fuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "uuid"                                          ,
          PlanTable(WebPages)                             ,
          QString("where site = %1").arg(uuid) )          ;
    if (SC.Query(Q)) while (SC.Next())                    {
      uuid = SC.Uuid(0)                                   ;
      if (!Uuids.contains(uuid)) Uuids << uuid            ;
    }                                                     ;
  }                                                       ;
  return true                                             ;
}

bool N::SearchBar::UrlFromTLD (
       SqlConnection & SC     ,
       Binary        * binary ,
       UUIDs         & Uuids  )
{
  QString Q                                               ;
  QString Pair                                            ;
  QString Where                                           ;
  UUIDs   Tuids                                           ;
  UUIDs   Fuids                                           ;
  SUID    uuid                                            ;
  QString B = binary->nodes[1]->name                      ;
  int     R = binary->relation.linkType                   ;
  B = B.toUpper( )                                        ;
  if (R==Groups::Equivalent)                    {
    Pair = "tld = :TLD"                                   ;
  } else
  if (R==Groups::Contains)                      {
    Pair = "tld like :TLD"                                ;
    if (!B.contains('%'))                                 {
      B . prepend ( '%' )                                 ;
      B . append  ( '%' )                                 ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  Where = QString("where %1").arg(Pair)                   ;
  Q = SC.sql.SelectFrom("uuid",PlanTable(TLD),Where)      ;
  SC . Prepare( Q                  )                      ;
  SC . Bind   ( "tld" , B.toUtf8() )                      ;
  if (SC.Exec()) while (SC.Next())                        {
    uuid = SC.Uuid(0)                                     ;
    if (!Tuids.contains(uuid)) Tuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;i<Tuids.count();i++)                       {
    uuid = Tuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "site"                                          ,
          PlanTable(SitesTld)                             ,
          QString("where tld = %1").arg(uuid) )           ;
    if (SC.Query(Q)) while (SC.Next())                    {
      uuid = SC.Uuid(0)                                   ;
      if (!Fuids.contains(uuid)) Fuids << uuid            ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;i<Fuids.count();i++)                       {
    uuid = Fuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "uuid"                                          ,
          PlanTable(WebPages)                             ,
          QString("where site = %1").arg(uuid) )          ;
    if (SC.Query(Q)) while (SC.Next())                    {
      uuid = SC.Uuid(0)                                   ;
      if (!Uuids.contains(uuid)) Uuids << uuid            ;
    }                                                     ;
  }                                                       ;
  return true                                             ;
}

bool N::SearchBar::UrlFromSLD (
       SqlConnection & SC     ,
       Binary        * binary ,
       UUIDs         & Uuids  )
{
  QString Q                                               ;
  QString Pair                                            ;
  QString Where                                           ;
  UUIDs   Tuids                                           ;
  UUIDs   Fuids                                           ;
  SUID    uuid                                            ;
  QString B = binary->nodes[1]->name                      ;
  int     R = binary->relation.linkType                   ;
  B = B.toUpper( )                                        ;
  if (R==Groups::Equivalent)                    {
    Pair = "gtld = :GTLD"                                 ;
  } else
  if (R==Groups::Contains)                      {
    Pair = "gtld like :GTLD"                              ;
    if (!B.contains('%'))                                 {
      B . prepend ( '%' )                                 ;
      B . append  ( '%' )                                 ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  Where = QString("where %1").arg(Pair)                   ;
  Q = SC.sql.SelectFrom("uuid",PlanTable(GTLD),Where)     ;
  SC . Prepare( Q                   )                     ;
  SC . Bind   ( "gtld" , B.toUtf8() )                     ;
  if (SC.Exec()) while (SC.Next())                        {
    uuid = SC.Uuid(0)                                     ;
    if (!Tuids.contains(uuid)) Tuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;i<Tuids.count();i++)                       {
    uuid = Tuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "site"                                          ,
          PlanTable(SitesSld)                             ,
          QString("where sld = %1").arg(uuid) )           ;
    if (SC.Query(Q)) while (SC.Next())                    {
      uuid = SC.Uuid(0)                                   ;
      if (!Fuids.contains(uuid)) Fuids << uuid            ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;i<Fuids.count();i++)                       {
    uuid = Fuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "uuid"                                          ,
          PlanTable(WebPages)                             ,
          QString("where site = %1").arg(uuid) )          ;
    if (SC.Query(Q)) while (SC.Next())                    {
      uuid = SC.Uuid(0)                                   ;
      if (!Uuids.contains(uuid)) Uuids << uuid            ;
    }                                                     ;
  }                                                       ;
  return true                                             ;
}

bool N::SearchBar::UrlFromColumn (
       SqlConnection & SC        ,
       Binary        * binary    ,
       UUIDs         & Uuids     )
{
  QString               Q                                        ;
  QString               A = binary->nodes[0]->name               ;
  QString               B = binary->nodes[1]->name               ;
  int                   R = binary->relation.linkType            ;
  QString               Where                                    ;
  QString               K                                        ;
  QStringList           Keys                                     ;
  QStringList           Pairs                                    ;
  QMap<QString,QString> KVs                                      ;
  A     = A.toLower()                                            ;
  Keys << A                                                      ;
  if (R==Groups::Equivalent)                           {
    K = QString("%1 = :%2").arg(A).arg(A.toUpper())              ;
    Pairs << K                                                   ;
  } else
  if (R==Groups::Contains)                             {
    K = QString("%1 like :%2").arg(A).arg(A.toUpper())           ;
    if (!B.contains('%'))                                        {
      B . prepend ( '%' )                                        ;
      B . append  ( '%' )                                        ;
    }                                                            ;
    Pairs << K                                                   ;
  }                                                              ;
  KVs [ A ] = B                                                  ;
  Where = QString("where %1").arg(Pairs.join(" and ") )          ;
  Q = SC.sql.SelectFrom("uuid",PlanTable(WebPages),Where)        ;
  SC . Prepare ( Q )                                             ;
  foreach (K,Keys)                                               {
    SC . Bind ( K , KVs[K].toUtf8() )                            ;
  }                                                              ;
  if (SC . Exec ( ) ) while (SC.Next())                          {
    SUID u = SC.Uuid(0)                                          ;
    Uuids << u                                                   ;
  }                                                              ;
  return true                                                    ;
}

void N::SearchBar::SearchResults(void)
{
  emit UuidItems(ResultUuids,ResultTypes,ResultNames,ResultTitle) ;
}

void N::SearchBar::PeopleResults(void)
{
  emit ListPeople(ResultTitle,ResultUuids) ;
}

void N::SearchBar::VideoResults(void)
{
  emit ListVideo(ResultTitle,ResultUuids) ;
}

void N::SearchBar::ClipResults(void)
{
  emit ListClip(ResultTitle,ResultUuids) ;
}

void N::SearchBar::DocumentResults(void)
{
  emit ListDocuments(ResultTitle,ResultUuids) ;
}

void N::SearchBar::UrlResults(void)
{
  emit ListUrls(ResultTitle,ResultUuids) ;
}

void N::SearchBar::KeywordResults(void)
{
  emit DictionaryExplain(ResultTitle,DictionaryResult,DictionaryAlign) ;
}

void N::SearchBar::MatchKeywordResults(void)
{
  emit KeywordMatched(ResultTitle,ResultNames) ;
}

void N::SearchBar::SqlQueryResults(void)
{
  emit SqlQuery(SqlQueryString) ;
}
