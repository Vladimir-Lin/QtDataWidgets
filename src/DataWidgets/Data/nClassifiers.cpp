#include <datawidgets.h>

N::Classifiers:: Classifiers (QObject * parent,Plan * p)
               : QObject     (          parent         )
               , Thread      (0,false                  )
               , plan        (                        p)
               , ProgressId  (-1                       )
               , ProgressGo  (false                    )
               , Value       (0                        )
{
  WidgetClass ;
}

N::Classifiers::~Classifiers (void)
{
}

void N::Classifiers::startup(VarArgs & command)
{
  ProgressId = plan->Progress(tr("Classifier"),tr("%v/%m")) ;
  Commands << command                                       ;
  if (ProgressGo) return                                    ;
  ProgressGo = true                                         ;
  plan -> Start ( ProgressId , &Value , &ProgressGo )       ;
  start ( )                                                 ;
}

void N::Classifiers::startup(VarArgLists & command)
{
  ProgressId = plan->Progress(tr("Classifier"),tr("%v/%m")) ;
  Commands << command                                       ;
  if (ProgressGo) return                                    ;
  ProgressGo = true                                         ;
  plan -> Start ( ProgressId , &Value , &ProgressGo )       ;
  start ( )                                                 ;
}

void N::Classifiers::run(void)
{
  plan->setRange(ProgressId,0,Commands.count()) ;
  while (ProgressGo && Commands.count()>0)      {
    VarArgs command = Commands[0]               ;
    Commands . takeAt ( 0       )               ;
    Operate           ( command )               ;
  }                                             ;
  plan -> Finish ( ProgressId )                 ;
  emit Finished  ( this       )                 ;
}

void N::Classifiers::Operate(VarArgs & command)
{
  nDropOut ( command . count ( ) <= 0 )    ;
  QString ST = command[0].toString()       ;
  if (ST=="Tag"     ) Tag      ( command ) ;
  if (ST=="SiteTo"  ) SiteTo   ( command ) ;
  if (ST=="PageTo"  ) PageTo   ( command ) ;
  if (ST=="TLD"     ) doTLDs   ( command ) ;
  if (ST=="SLD"     ) doSLDs   ( command ) ;
  if (ST=="Ordering") Ordering ( command ) ;
}

bool N::Classifiers::UrlFromPage (
       SqlConnection & SC        ,
       QString         Page      ,
       UUIDs         & Uuids     )
{
  NetworkManager NM(plan)        ;
  SUID    uuid                   ;
  uuid = NM.PageUuid(SC,Page)    ;
  nKickOut ( uuid <= 0 , false ) ;
  Uuids << uuid                  ;
  return true                    ;
}

bool N::Classifiers::UrlFromMime (
       SqlConnection & SC       ,
       Binary        * binary   ,
       UUIDs         & Uuids    )
{
  QString     Q                                                  ;
  QString     A = binary->nodes[0]->name                         ;
  QString     B = binary->nodes[1]->name                         ;
  int         R = binary->relation.linkType                      ;
  QString     Where                                              ;
  QString     K                                                  ;
  QStringList Keys                                               ;
  QStringList Pairs                                              ;
  KMAPs       KVs                                                ;
  ////////////////////////////////////////////////////////////////
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
  Where = Where + " and type = :TYPE"                            ;
  Q = SC.sql.SelectFrom("uuid",PlanTable(DocumentMimes),Where)   ;
  K = QString("[%1] => :%2=[%3]").arg(Q).arg(A.toUpper()).arg(B) ;
  plan->Debug(55,K)                                              ;
  ////////////////////////////////////////////////////////////////
  SC . Prepare ( Q )                                             ;
  foreach (K,Keys)                                               {
    SC . Bind ( K , KVs[K].toUtf8() )                            ;
  }                                                              ;
  SC . Bind ( "type" , Types::URL )                    ;
  if (SC . Exec ( ) )                                            {
    while (SC.Next())                                            {
      SUID u = SC.Uuid(0)                                        ;
      Uuids << u                                                 ;
    }                                                            ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  return true                                                    ;
}

bool N::Classifiers :: UrlFromEncoding (
       SqlConnection & SC              ,
       Binary        * binary          ,
       UUIDs         & Uuids           )
{
  QString               Q                                        ;
  QString               A = binary->nodes[0]->name               ;
  QString               B = binary->nodes[1]->name               ;
  int                   R = binary->relation.linkType            ;
  QString               Where                                    ;
  QString               K                                        ;
  QStringList           Keys                                     ;
  QStringList           Pairs                                    ;
  SUID                  uuid                                     ;
  QMap<QString,QString> KVs                                      ;
  ////////////////////////////////////////////////////////////////
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
  Q = SC.sql.SelectFrom("uuid",PlanTable(DocumentCodecs),Where)  ;
  K = QString("[%1] => :%2=[%3]").arg(Q).arg(A.toUpper()).arg(B) ;
  plan->Debug(55,K)                                              ;
  ////////////////////////////////////////////////////////////////
  UUIDs DUIDs                                                    ;
  SC . Prepare ( Q )                                             ;
  foreach (K,Keys)                                               {
    SC . Bind ( K , KVs[K].toUtf8() )                            ;
  }                                                              ;
  if (SC . Exec ( ) )                                            {
    while (SC.Next())                                            {
      SUID u = SC.Uuid(0)                                        ;
      DUIDs << u                                                 ;
    }                                                            ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  foreach (uuid,DUIDs)                                           {
    Q = SC.sql.SelectFrom                                        (
          "type"                                                 ,
          PlanTable(MajorUuid)                                   ,
          SC.WhereUuid(uuid)                                   ) ;
    if (SC.Fetch(Q))                                             {
      int t = SC.Int(0)                                          ;
      if (t==Types::URL)                               {
        if (!Uuids.contains(uuid)) Uuids << uuid                 ;
      }                                                          ;
    }                                                            ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  return true                                                    ;
}

bool N::Classifiers :: UrlFromTag (
       SqlConnection & SC         ,
       int             Language   ,
       int             TagType    ,
       Binary        * binary     ,
       UUIDs         & Uuids      )
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
    while (ProgressGo && SC.Next())                  {
      Nuids << SC.Uuid(0)                            ;
    }                                                ;
  }                                                  ;
  if (!ProgressGo) return false                      ;
  if (Nuids.count()<=0) return false                 ;
  ////////////////////////////////////////////////////
  for (int i=0;ProgressGo && i<Nuids.count();i++)    {
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
  if (!ProgressGo) return false                      ;
  if (Tuids.count()<=0) return false                 ;
  ////////////////////////////////////////////////////
  for (int i=0;ProgressGo && i<Tuids.count();i++)    {
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
  if (!ProgressGo) return false                      ;
  if (Muids.count()<=0) return false                 ;
  ////////////////////////////////////////////////////
  for (int i=0;ProgressGo && i<Muids.count();i++)    {
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
    if (SC.Query(Q)) while (ProgressGo && SC.Next()) {
      SUID puid = SC.Uuid(0)                         ;
      if (!Uuids.contains(puid)) Uuids << puid       ;
    }                                                ;
  }                                                  ;
  return true                                        ;
}

bool N::Classifiers::UrlFromParent (
       SqlConnection & SC          ,
       Binary        * binary      ,
       UUIDs         & Uuids       )
{
  NetworkManager NM ( plan ) ;
  QString        Pair                                     ;
  QString        Where                                    ;
  UUIDs          Fuids                                    ;
  SUID           uuid                                     ;
  QString        B = binary->nodes[1]->name               ;
  int            R = binary->relation.linkType            ;
  if (R==Groups::Equivalent)                              {
    Pair = "page = :PAGE"                                 ;
  } else
  if (R==Groups::Contains)                                {
    Pair = "page like :PAGE"                              ;
    if (!B.contains('%'))                                 {
      B . prepend ( '%' )                                 ;
      B . append  ( '%' )                                 ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  Where = QString("where %1").arg(Pair)                   ;
  if (!NM.PageLike(SC,Where,B,"page",Fuids,ProgressGo))   {
    return true                                           ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  plan -> setRange ( ProgressId , 0 , Fuids.count() )     ;
  for (int i=0;ProgressGo && i<Fuids.count();i++)         {
    uuid = Fuids[i]                                       ;
    Value = i                                             ;
    NM.FromPage(SC,uuid,Uuids,ProgressGo)                 ;
  }                                                       ;
  return true                                             ;
}

bool N::Classifiers::UrlFromSite (
       SqlConnection & SC        ,
       Binary        * binary    ,
       UUIDs         & Uuids     )
{
  NetworkManager NM ( plan )                              ;
  QString Q                                               ;
  QString Pair                                            ;
  QString Where                                           ;
  UUIDs   Fuids                                           ;
  SUID    uuid                                            ;
  QString B = binary->nodes[1]->name                      ;
  int     R = binary->relation.linkType                   ;
  if (R==Groups::Equivalent)                              {
    Pair = "url = :URL"                                   ;
  } else
  if (R==Groups::Contains)                                {
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
  if (SC.Exec()) while (ProgressGo && SC.Next())          {
    uuid = SC.Uuid(0)                                     ;
    if (!Fuids.contains(uuid)) Fuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  plan -> setRange ( ProgressId , 0 , Fuids.count() )     ;
  for (int i=0;ProgressGo && i<Fuids.count();i++)         {
    uuid = Fuids[i]                                       ;
    Value = i                                             ;
    NM . FromSite ( SC , uuid , Uuids , ProgressGo )      ;
  }                                                       ;
  return true                                             ;
}

bool N::Classifiers::UrlFromTLD (
       SqlConnection & SC       ,
       Binary        * binary   ,
       UUIDs         & Uuids    )
{
  NetworkManager NM ( plan )                              ;
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
  if (SC.Exec()) while (ProgressGo && SC.Next())          {
    uuid = SC.Uuid(0)                                     ;
    if (!Tuids.contains(uuid)) Tuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;ProgressGo && i<Tuids.count();i++)         {
    uuid = Tuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "site"                                          ,
          PlanTable(SitesTld)                             ,
          QString("where tld = %1").arg(uuid) )           ;
    if (SC.Query(Q)) while (ProgressGo && SC.Next())      {
      uuid = SC.Uuid(0)                                   ;
      if (!Fuids.contains(uuid)) Fuids << uuid            ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  plan -> setRange ( ProgressId , 0 , Fuids.count() )     ;
  for (int i=0;ProgressGo && i<Fuids.count();i++)         {
    uuid = Fuids[i]                                       ;
    Value = i                                             ;
    NM . FromSite ( SC , uuid , Uuids , ProgressGo )      ;
  }                                                       ;
  return true                                             ;
}

bool N::Classifiers::UrlFromSLD (
       SqlConnection & SC       ,
       Binary        * binary   ,
       UUIDs         & Uuids    )
{
  NetworkManager NM ( plan )                              ;
  QString Q                                               ;
  QString Pair                                            ;
  QString Where                                           ;
  UUIDs   Tuids                                           ;
  UUIDs   Fuids                                           ;
  SUID    uuid                                            ;
  QString B = binary->nodes[1]->name                      ;
  int     R = binary->relation.linkType                   ;
  B = B.toUpper( )                                        ;
  if (R==Groups::Equivalent)                              {
    Pair = "gtld = :GTLD"                                 ;
  } else
  if (R==Groups::Contains)                                {
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
  if (SC.Exec()) while (ProgressGo && SC.Next())          {
    uuid = SC.Uuid(0)                                     ;
    if (!Tuids.contains(uuid)) Tuids << uuid              ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  for (int i=0;ProgressGo && i<Tuids.count();i++)         {
    uuid = Tuids[i]                                       ;
    Q = SC.sql.SelectFrom                                 (
          "site"                                          ,
          PlanTable(SitesSld)                             ,
          QString("where sld = %1").arg(uuid) )           ;
    if (SC.Query(Q)) while (ProgressGo && SC.Next())      {
      uuid = SC.Uuid(0)                                   ;
      if (!Fuids.contains(uuid)) Fuids << uuid            ;
    }                                                     ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  plan -> setRange ( ProgressId , 0 , Fuids.count() )     ;
  for (int i=0;ProgressGo && i<Fuids.count();i++)         {
    uuid = Fuids[i]                                       ;
    Value = i                                             ;
    NM . FromSite ( SC , uuid , Uuids , ProgressGo )      ;
  }                                                       ;
  return true                                             ;
}

void N::Classifiers::Join       (
       SqlConnection & SC       ,
       SUID            group    ,
       int             t1       ,
       int             t2       ,
       int             relation ,
       UUIDs         & Uuids    )
{
  Value = 0                                              ;
  plan -> Start    ( ProgressId , &Value , &ProgressGo ) ;
  plan -> setRange ( ProgressId , 0 , Uuids.count()    ) ;
  ////////////////////////////////////////////////////////
  int final = 0                                          ;
  QString Q                                              ;
  Q = SC.sql.SelectFrom                                  (
        "position"                                       ,
        PlanTable(Groups)                                ,
        QString("where first = %1 "
                "and t1 = %2 "
                "and t2 = %3 "
                "and relation = %4 "
                "order by position desc "
                "limit 0,1;"                             )
        .arg( group                                      )
        .arg( t1                                         )
        .arg( t2                                         )
        .arg( relation                                   )
      )                                                  ;
  if (SC.Fetch(Q)) final = SC.Int(0)                     ;
  final++                                                ;
  ////////////////////////////////////////////////////////
  for (int i=0;ProgressGo && i<Uuids.count();i++)        {
    SUID uuid = Uuids[i]                                 ;
    Value = i                                            ;
    Q = SC.sql.InsertInto                                (
          "first,second,t1,t2,relation,position"         ,
          PlanTable(Groups)                              ,
          QString("%1,%2,%3,%4,%5,%6"                    )
          .arg( group                                    )
          .arg( uuid                                     )
          .arg( t1                                       )
          .arg( t2                                       )
          .arg( relation                                 )
          .arg( final                                    )
        ) + ";"                                          ;
    if (SC.Query(Q)) final++                             ;
  }                                                      ;
}

void N::Classifiers::Tag(VarArgs & command)
{
  SUID    TagUuid     = command[2].toULongLong() ;
  int     TagType     = command[3].toInt      () ;
  int     TargetType  = command[4].toInt      () ;
  int     ObjectType  = command[5].toInt      () ;
  int     Relation    = command[6].toInt      () ;
  QString OpType      = command[7].toString   () ;
  QString OpWay       = command[8].toString   () ;
  if (TagType!=Types::Tag) return      ;
  if (TargetType==Types::URL)          {
    if (ObjectType==Types::URL)        {
      if (OpType=="Url" && OpWay=="Sets")        {
          TagUrlSets ( command )                 ;
      }                                          ;
    }                                            ;
  }                                              ;
}

void N::Classifiers::SiteTo(VarArgs & command)
{
  QString Site        = command[1].toString    ( )  ;
  SUID    TargetUuid  = command[2].toULongLong ( )  ;
  int     ObjectType  = command[3].toInt       ( )  ;
  int     Relation    = command[4].toInt       ( )  ;
  int     vLanguageId = plan->LanguageId            ;
  ///////////////////////////////////////////////////
  SqlConnection  SC ( plan->sql           )         ;
  NetworkManager NM ( plan                )         ;
  QString CN = QtUUID::createUuidString()          ;
  if (SC.open("nClassifiers",CN))                   {
    QUrl    url                                     ;
    QString M                                       ;
    QString Name                                    ;
    UUIDs   Uuids                                   ;
    SUID    site  = 0                               ;
    Name = SC . getName                             (
             PlanTable(Names)                       ,
             "uuid"                                 ,
             vLanguageId                            ,
             TargetUuid                           ) ;
    M    = "http://"                                ;
    M   += Site                                     ;
    url  = QUrl(M)                                  ;
    site = NM.UrlUuid(SC,url)                       ;
    if (site>0)                                     {
      ProgressGo = true                             ;
      M = tr("Searching %1").arg(Site)              ;
      plan->ProgressText(ProgressId,M)              ;
      plan -> Start (ProgressId,&Value,&ProgressGo) ;
      NM . FromSite ( SC,site,Uuids,ProgressGo )    ;
    }                                               ;
    if (Uuids.count()>0)                            {
      M = tr("%1 join %2").arg(Site).arg(Name)      ;
      plan->ProgressText(ProgressId,M)              ;
      Join                                          (
        SC                                          ,
        TargetUuid                                  ,
        ObjectType                                  ,
        Types::URL                        ,
        Relation                                    ,
        Uuids                                     ) ;
    }                                               ;
    SC . close()                                    ;
  }                                                 ;
  SC.remove()                                       ;
}

void N::Classifiers::PageTo(VarArgs & command)
{
  QString Site        = command[1].toString    ( )      ;
  SUID    TargetUuid  = command[2].toULongLong ( )      ;
  int     ObjectType  = command[3].toInt       ( )      ;
  int     Relation    = command[4].toInt       ( )      ;
  ///////////////////////////////////////////////////////
  SqlConnection  SC ( plan->sql )                       ;
  NetworkManager NM ( plan      )                       ;
  QString CN = QtUUID::createUuidString()              ;
  if (SC.open("nClassifiers",CN))                       {
    QUrl    url                                         ;
    QString M                                           ;
    UUIDs   Uuids                                       ;
    SUID    page  = 0                                   ;
    url  = QUrl(Site)                                   ;
    page = NM.PageUuid(SC,url)                          ;
    if (page>0)                                         {
      M = tr("Searching %1").arg(Site)                  ;
      plan->ProgressText(ProgressId,M)                  ;
      plan -> Start (ProgressId,&Value,&ProgressGo)     ;
      NM.FromPage(SC,page,Uuids,ProgressGo)             ;
    }                                                   ;
    if (Uuids.count()>0)                                {
      Join                                              (
        SC                                              ,
        TargetUuid                                      ,
        ObjectType                                      ,
        Types::URL                            ,
        Relation                                        ,
        Uuids                                         ) ;
    }                                                   ;
    SC . close()                                        ;
  }                                                     ;
  SC.remove()                                           ;
}

void N::Classifiers :: ReportIt (QString syntax,int total)
{
  QString M                                                       ;
  M = tr("[%1] syntax found %2 web pages").arg(syntax).arg(total) ;
  plan->Debug(55,M)                                               ;
}

void N::Classifiers::TagUrlSets(VarArgs & command)
{
  int     vLanguageId = command [ 1 ] . toInt       ( )                       ;
  SUID    TagUuid     = command [ 2 ] . toULongLong ( )                       ;
  int     TagType     = command [ 3 ] . toInt       ( )                       ;
  int     TargetType  = command [ 4 ] . toInt       ( )                       ;
  int     ObjectType  = command [ 5 ] . toInt       ( )                       ;
  int     Relation    = command [ 6 ] . toInt       ( )                       ;
  QString Rule        = command [ 9 ] . toString    ( )                       ;
  /////////////////////////////////////////////////////////////////////////////
  plan -> Talk  ( 45 , tr("Classify urls into tag by sets algebra rules") )   ;
  /////////////////////////////////////////////////////////////////////////////
  SetsAlgebra            NSA                                                  ;
  QMap<QString,Binary *> Binaries                                             ;
  if (!NSA.Decode(Rule))                                                      {
    plan -> Talk  ( 45 , tr("Sets algebra syntax error")                    ) ;
    plan -> Debug ( 45 , Rule                                               ) ;
    return                                                                    ;
  }                                                                           ;
  for (int i=0;i<NSA.Inputs.count();i++)                                      {
    QString S = NSA.Inputs[i]                                                 ;
    if (S.contains("=") || S.contains("@"))                                   {
      bool      correct = true                                                ;
      Binary  * nb      = new Binary ( )                                      ;
      Binaries[S] = nb                                                        ;
      if (Binaries[S]->Pair(S))                                               {
        QString n = Binaries[S]->nodes[0]->name                               ;
        n = n.toLower()                                                       ;
        if (n=="nothing" ) correct = true                                ; else
        if (n=="tag"     ) correct = true                                ; else
        if (n=="from"    ) correct = true                                ; else
        if (n=="site"    ) correct = true                                ; else
        if (n=="tld"     ) correct = true                                ; else
        if (n=="sld"     ) correct = true                                ; else
        if (n=="protocol") correct = true                                ; else
        if (n=="page"    ) correct = true                                ; else
        if (n=="uri"     ) correct = true                                ; else
        if (n=="title"   ) correct = true                                ; else
        if (n=="mimetype") correct = true                                ; else
        if (n=="encoding") correct = true                                ; else
                           correct = false                                    ;
      } else correct = false                                                  ;
      if (!correct)                                                           {
        plan -> Talk  ( 45 , tr("Sets algebra syntax error")                ) ;
        plan -> Debug ( 45 , Rule                                           ) ;
        return                                                                ;
      }                                                                       ;
    }                                                                         ;
  }                                                                           ;
  plan->ProgressText(ProgressId,tr("Sets algebra syntax decoded"))            ;
  /////////////////////////////////////////////////////////////////////////////
  QString cn = QString::number(QtUUID::createUpperUuid())                    ;
  SqlConnection  SC ( plan -> sql )                                           ;
  NetworkManager NM ( plan        )                                           ;
  #define SEARCH(FUNC)                                                        \
    if ( NM . FUNC ( SC , ( *(Binaries[n]) ) , NSA.Uuids[n] ) )             { \
      ReportIt(n,NSA.Uuids[n].count())                                      ; \
    }
  if (SC.open("nClassifiers",cn))                                             {
    Value = 0                                                                 ;
    plan -> Start    ( ProgressId , &Value , &ProgressGo         )            ;
    plan -> Talk     ( 45 , tr("Fetch pattern matched webpages") )            ;
    for (int i=0;ProgressGo && i<NSA.Inputs.count();i++)                      {
      QString n = NSA.Inputs[i]                                               ;
      Value = i                                                               ;
      plan -> setRange ( ProgressId , 0 , NSA.Inputs.count()     )            ;
      plan -> ProgressText(ProgressId,n)                                      ;
      if (Binaries.contains(n))                                               {
        QString A      = Binaries[n]->nodes[0]->name                          ;
        A = A.toLower()                                                       ;
        if (A=="protocol")                                                    {
          SEARCH ( FromProtocol )                                             ;
        } else
        if (A=="page")                                                        {
          SEARCH ( FromPage     )                                             ;
        } else
        if (A=="uri")                                                         {
        } else
        if (A=="title")                                                       {
          SEARCH ( FromTitle    )                                             ;
        } else
        if (A=="tag" )                                                        {
          if (UrlFromTag(SC,vLanguageId,TargetType,Binaries[n],NSA.Uuids[n])) {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else
        if (A=="from")                                                        {
          if (UrlFromParent(SC,Binaries[n],NSA.Uuids[n]))                     {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else
        if (A=="site")                                                        {
          if (UrlFromSite(SC,Binaries[n],NSA.Uuids[n]))                       {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else
        if (A=="tld" )                                                        {
          if (UrlFromTLD(SC,Binaries[n],NSA.Uuids[n]))                        {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else
        if (A=="sld" )                                                        {
          if (UrlFromSLD(SC,Binaries[n],NSA.Uuids[n]))                        {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else
        if (A=="mimetype")                                                    {
          if (UrlFromMime(SC,Binaries[n],NSA.Uuids[n]))                       {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else
        if (A=="encoding")                                                    {
          if (UrlFromEncoding(SC,Binaries[n],NSA.Uuids[n]))                   {
            ReportIt(n,NSA.Uuids[n].count())                                  ;
          }                                                                   ;
        } else                                                                {
          UUIDs EmptyUuid                                                     ;
          NSA.Uuids[n] = EmptyUuid                                            ;
        }                                                                     ;
        delete Binaries[n]                                                    ;
      } else                                                                  {
        UUIDs   Uuids                                                         ;
        if (UrlFromPage(SC,n,Uuids))                                          {
          NSA.Uuids[n] = Uuids                                                ;
          int t = Uuids.count()                                               ;
          plan->Debug(55,tr("[%1] syntax found %2 web pages").arg(n).arg(t))  ;
        }                                                                     ;
      }                                                                       ;
    }                                                                         ;
    Binaries . clear ( )                                                      ;
    ///////////////////////////////////////////////////////////////////////////
    if (ProgressGo)                                                           {
      UUIDs Uuids                                                             ;
      plan -> Talk ( 45 , tr("Start sets operations") )                       ;
      if (NSA.Execute(Uuids))                                                 {
        if (Uuids.count()>0)                                                  {
          QString name                                                        ;
          QString M                                                           ;
          M = tr("Found %1 match web pages").arg(Uuids.count())               ;
          plan -> Talk  ( 45 , M )                                            ;
          plan -> Debug ( 45 , M )                                            ;
          name = SC.getName(PlanTable(Names),"uuid",vLanguageId,TagUuid)      ;
          M = tr("%1 web pages join %2").arg(Uuids.count()).arg(name)         ;
          plan->ProgressText(ProgressId,M)                                    ;
          Join(SC,TagUuid,TagType,ObjectType,Relation,Uuids)                  ;
        } else                                                                {
          plan->Talk(45,tr("No matched web pages found"))                     ;
        }                                                                     ;
      } else                                                                  {
        plan -> Talk ( 45 , tr("Sets operations failure") )                   ;
      }                                                                       ;
    }                                                                         ;
    SC.close()                                                                ;
  }                                                                           ;
  SC.remove()                                                                 ;
  #undef  SEARCH
}

void N::Classifiers::doTLDs(SqlConnection & Connection,VarArgs & command)
{
  if (command.count()!=2) return                               ;
  //////////////////////////////////////////////////////////////
  NetworkManager NM ( plan )                                    ;
  QString M     = ""                                           ;
  bool    done  = false                                        ;
  int     Chunk = command[1].toInt()                           ;
  QString Limit = QString ( " limit 0,%1 ;" ) . arg(Chunk)     ;
  TUID    TOTAL = NM.SiteTotal  ( Connection )                 ;
  SUID    LUID  = NM.LastestTld ( Connection )                 ;
  TUID    FUID  = 0                                            ;
  TUID    AUID  = 0                                            ;
  UUIDs   Uuids                                                ;
  NAMEs   URLs                                                 ;
  QMap<SUID,TUID> IDs                                          ;
  QString Q                                                    ;
  QString Options                                              ;
  if (LUID>0) FUID = NM.SiteId( Connection , LUID )            ;
  M     = tr("Classify top level domains")                     ;
  Value = 0                                                    ;
  plan -> Start        ( ProgressId , &Value , &ProgressGo   ) ;
  plan -> Talk         ( 45 , M                              ) ;
  plan -> setRange     ( ProgressId , (int)FUID , (int)TOTAL ) ;
  plan -> ProgressText ( ProgressId , M                      ) ;
  Limit = " order by id asc" + Limit                           ;
  AUID  = FUID                                                 ;
  while (ProgressGo && !done)                                  {
    Uuids . clear ( )                                          ;
    URLs  . clear ( )                                          ;
    IDs   . clear ( )                                          ;
    Options = QString(" where id > %1").arg(FUID)              ;
    Q = Connection.sql.SelectFrom                              (
          "id,uuid,url"                                        ,
          PlanTable(SiteDepot)                                 ,
          Options + Limit                                    ) ;
    SqlLoopNow ( Connection , Q )                              ;
      TUID    fuid = Connection . Value ( 0 ) . toLongLong ()  ;
      SUID    uuid = Connection . Uuid  ( 1 )                  ;
      QString url  = Connection.String( 2 )                    ;
      Uuids        << uuid                                     ;
      IDs  [ uuid ] = fuid                                     ;
      URLs [ uuid ] = url                                      ;
    SqlLoopErr ( Connection , Q )                              ;
    SqlLoopEnd ( Connection , Q )                              ;
    if (Uuids.count()<=0) done = true; else                    {
      for (int i=0;ProgressGo && i<Uuids.count();i++)          {
        SUID uuid = Uuids [ i    ]                             ;
        FUID      = IDs   [ uuid ]                             ;
        QUrl url                                               ;
        Value     = (int)FUID                                  ;
        url       . setHost (URLs[uuid])                       ;
        SUID tld  = NM.UrlTLD ( url )                          ;
        if (tld>0) NM.InsertTldMap ( Connection , uuid , tld ) ;
      }                                                        ;
    }                                                          ;
  }                                                            ;
}

void N::Classifiers::doTLDs(VarArgs & command)
{
  QString cn = QString::number(QtUUID::createUpperUuid()) ;
  SqlConnection SC(plan->sql)                              ;
  if (SC.open("nClassifiers",cn))                          {
    doTLDs(SC,command)                                     ;
    SC.close()                                             ;
  }                                                        ;
  SC.remove()                                              ;
}

void N::Classifiers::doSLDs(SqlConnection & Connection,VarArgs & command)
{
  if (command.count()!=2) return                               ;
  //////////////////////////////////////////////////////////////
  NetworkManager NM ( plan )                                    ;
  QString M     = ""                                           ;
  bool    done  = false                                        ;
  int     Chunk = command[1].toInt()                           ;
  QString Limit = QString ( " limit 0,%1 ;" ) . arg(Chunk)     ;
  TUID    TOTAL = NM.SiteTotal  ( Connection )                 ;
  SUID    LUID  = NM.LastestSld ( Connection )                 ;
  TUID    FUID  = 0                                            ;
  TUID    AUID  = 0                                            ;
  UUIDs   Uuids                                                ;
  NAMEs   URLs                                                 ;
  QMap<SUID,TUID> IDs                                          ;
  QString Q                                                    ;
  QString Options                                              ;
  if (LUID>0) FUID = NM.SiteId( Connection , LUID )            ;
  M     = tr("Classify second level domains")                  ;
  Value = 0                                                    ;
  plan -> Start        ( ProgressId , &Value , &ProgressGo   ) ;
  plan -> Talk         ( 45 , M                              ) ;
  plan -> setRange     ( ProgressId , (int)FUID , (int)TOTAL ) ;
  plan -> ProgressText ( ProgressId , M                      ) ;
  Limit   = " order by id asc" + Limit                         ;
  AUID    = FUID                                               ;
  while (ProgressGo && !done)                                  {
    Uuids . clear ( )                                          ;
    URLs  . clear ( )                                          ;
    IDs   . clear ( )                                          ;
    Options = QString(" where id > %1").arg(FUID)              ;
    Q = Connection.sql.SelectFrom                              (
          "id,uuid,url"                                        ,
          PlanTable(SiteDepot)                                 ,
          Options + Limit                                    ) ;
    SqlLoopNow ( Connection , Q )                              ;
      TUID    fuid = Connection . Value ( 0 ) . toLongLong ()  ;
      SUID    uuid = Connection . Uuid  ( 1 )                  ;
      QString url  = Connection.String( 2 )                    ;
      Uuids        << uuid                                     ;
      IDs  [ uuid ] = fuid                                     ;
      URLs [ uuid ] = url                                      ;
    SqlLoopErr ( Connection , Q )                              ;
    SqlLoopEnd ( Connection , Q )                              ;
    if (Uuids.count()<=0) done = true; else                    {
      for (int i=0;ProgressGo && i<Uuids.count();i++)          {
        SUID uuid = Uuids [ i    ]                             ;
        FUID      = IDs   [ uuid ]                             ;
        QUrl url                                               ;
        Value     = (int)FUID                                  ;
        url       . setHost (URLs[uuid])                       ;
        SUID sld  = NM.UrlSLD ( url )                          ;
        if (sld>0) NM.InsertSldMap ( Connection , uuid , sld ) ;
      }                                                        ;
    }                                                          ;
  }                                                            ;
}

void N::Classifiers::doSLDs(VarArgs & command)
{
  QString cn = QString::number(QtUUID::createUpperUuid()) ;
  SqlConnection SC(plan->sql)                              ;
  if (SC.open("nClassifiers",cn))                          {
    doSLDs(SC,command)                                     ;
    SC.close()                                             ;
  }                                                        ;
  SC.remove()                                              ;
}

void N::Classifiers::Ordering(VarArgs & command)
{
  QString dbName  = command [ 1 ] . toString ( )                        ;
  QString Table   = command [ 2 ] . toString ( )                        ;
  int     Chunk   = 1000                                                ;
  int     Lastest = 0                                                   ;
  nDropOut ( dbName . length ( ) <= 0 )                                 ;
  nDropOut ( Table  . length ( ) <= 0 )                                 ;
  if ( command . count ( ) > 3 ) Chunk = command [ 3 ] . toInt()        ;
  ///////////////////////////////////////////////////////////////////////
  SqlConnection SC(plan->sql)                                           ;
  QString CN = QtUUID::createUuidString()                              ;
  if (SC.open("Ordering",CN))                                           {
    QString M                                                           ;
    CUIDs   IDs                                                         ;
    SC  . Depot ( dbName )                                              ;
    IDs . clear (        )                                              ;
    M     = tr("Counting `%1`.`%2`").arg(dbName).arg(Table)             ;
    plan -> Start        ( ProgressId , &Value , &ProgressGo )          ;
    plan -> Talk         ( 45 , M                            )          ;
    plan -> ProgressText ( ProgressId , M                    )          ;
    if ( SC . Cuids ( IDs,Table,"id",SC.OrderByAsc("id") ) > 0 )        {
      int  total    = IDs.count()                                       ;
      int  position = 1                                                 ;
      bool match    = true                                              ;
      int  id                                                           ;
      Value = 0                                                         ;
      for (int i=0;match && i<total;i++)                                {
        id = IDs [ i ]                                                  ;
        if (id!=position)                                               {
          match = false                                                 ;
        } else                                                          {
          position ++                                                   ;
          Value    ++                                                   ;
        }                                                               ;
      }                                                                 ;
      M     = tr("Organize `%1`.`%2`").arg(dbName).arg(Table)           ;
      plan -> Start        ( ProgressId , &Value , &ProgressGo  )       ;
      plan -> Talk         ( 45 , M                             )       ;
      plan -> setRange     ( ProgressId , 0 , total             )       ;
      plan -> ProgressText ( ProgressId , M                     )       ;
      if (SC.Sequential(Table,IDs,Value,ProgressGo,Chunk))              {
        Lastest = total + 1                                             ;
      }                                                                 ;
    }                                                                   ;
    /////////////////////////////////////////////////////////////////////
    //                      Check extra paraments                      //
    /////////////////////////////////////////////////////////////////////
    if (ProgressGo && ( command . count() > 4 ) )                       {
      QString Q                                                         ;
      QString FixUp = command [ 4 ] . toString ( )                      ;
      if ( ( FixUp == "AutoIncrement" ) && ( Lastest > 0 ) )            {
        M = tr("Modify `%1`.`%2` AutoIncrement").arg(dbName).arg(Table) ;
        plan -> Talk         ( 45         , M )                         ;
        plan -> ProgressText ( ProgressId , M )                         ;
        Q = QString ( "alter table %1 auto_increment = %2"              )
            . arg   ( SC . sql . EncapTable ( Table )                   )
            . arg   ( Lastest                                         ) ;
        SC . Query  ( Q                                               ) ;
      }                                                                 ;
    }                                                                   ;
    /////////////////////////////////////////////////////////////////////
    SC.close()                                                          ;
  }                                                                     ;
  SC.remove()                                                           ;
  if (ProgressGo)                                                       {
    QString M                                                           ;
    M = tr("Table `%1`.`%2` organized").arg(dbName).arg(Table)          ;
    plan -> Talk         ( 45         , M )                             ;
    plan -> ProgressText ( ProgressId , M )                             ;
    Time :: msleep ( 3000 )                                            ;
  } else                                                                {
    QString M                                                           ;
    M = tr("Organizing `%1`.`%2` interrupted").arg(dbName).arg(Table)   ;
    plan -> Talk         ( 45         , M )                             ;
    plan -> ProgressText ( ProgressId , M )                             ;
    Time :: msleep ( 3000 )                                            ;
    plan->Debug(60,QString("Value = %1").arg(Value))                    ;
  }                                                                     ;
}
