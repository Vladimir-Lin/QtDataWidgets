#include <datawidgets.h>
#include "ui_nCppComponentLists.h"

#define TABLES(ID) CodePlan->Tables[Tables::ID]

N::CppComponentLists:: CppComponentLists (QWidget * parent,Plan * p )
                     : Widget            (          parent,       p )
                     , CodeManager       (                        p )
                     , ui                (new Ui::nCppComponentLists)
                     , What              (Cpp::Define               )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::CppComponentLists::~CppComponentLists(void)
{
  delete ui;
}

void N::CppComponentLists::Configure(void)
{
  ui -> setupUi ( this ) ;
}

void N::CppComponentLists::resizeEvent(QResizeEvent * event)
{
  QWidget :: resizeEvent ( event ) ;
  Relocation             (       ) ;
}

void N::CppComponentLists::showEvent(QShowEvent * event)
{
  QWidget :: showEvent   ( event ) ;
  Relocation             (       ) ;
}

void N::CppComponentLists::closeEvent(QCloseEvent * event)
{
  QWidget :: closeEvent  ( event ) ;
}

void N::CppComponentLists::focusInEvent(QFocusEvent * event)
{
  if (!focusIn (event)) QWidget::focusInEvent (event) ;
}

void N::CppComponentLists::focusOutEvent(QFocusEvent * event)
{
  if (!focusOut(event)) QWidget::focusOutEvent(event) ;
}

bool N::CppComponentLists::FocusIn(void)
{
  connectAction(Menus::Refresh,this,SLOT(Refresh        ())) ;
  connectAction(Menus::Copy   ,this,SLOT(CopyToClipboard())) ;
  return true                                                          ;
}

bool N::CppComponentLists::FocusOut(void)
{
  return true ;
}

void N::CppComponentLists::Relocation(void)
{
  ui -> Source  -> resize ( width() ,          ui->Source ->height() ) ;
  ui -> Version -> resize ( width() ,          ui->Version->height() ) ;
  ui -> Items   -> resize ( width() , height()-ui->Items  ->y     () ) ;
}

void N::CppComponentLists::CopyToClipboard(void)
{
  QTreeWidgetItem * item = ui->Items->currentItem () ;
  if (IsNull(item)) return                           ;
  int cc = ui->Items->currentColumn()                ;
  QString text = item -> text (cc)                   ;
  qApp -> clipboard() -> setText (text)              ;
}

QString N::CppComponentLists::Table(void)
{
  switch (What)                                                   {
    case Cpp::Define      : return TABLES(CppDefines  ) ;
    case Cpp::Include     : return TABLES(Includes    ) ;
    case Cpp::Enumeration : return TABLES(Enumerations) ;
    case Cpp::Namespace   : return TABLES(Namespaces  ) ;
  }                                                               ;
  return ""                                                       ;
}

void N::CppComponentLists::setToolTips(void)
{
  switch (What)                                      {
    case Cpp::Define                       :
      ui->Items->setToolTip(tr("`#define` lists"  )) ;
    break                                            ;
    case Cpp::Include                      :
      ui->Items->setToolTip(tr("`#include` lists" )) ;
    break                                            ;
    case Cpp::Enumeration                  :
      ui->Items->setToolTip(tr("`enum` lists"     )) ;
    break                                            ;
    case Cpp::Namespace                    :
      ui->Items->setToolTip(tr("`namespace` lists")) ;
    break                                            ;
  }                                                  ;
}

bool N::CppComponentLists::startup(void)
{
  switch (What)                                {
    case Cpp::Define                 :
      setWindowTitle (tr("`#define` lists"  )) ;
    break                                      ;
    case Cpp::Include                :
      setWindowTitle (tr("`#include` lists" )) ;
    break                                      ;
    case Cpp::Enumeration            :
      setWindowTitle (tr("`enum` lists"     )) ;
    break                                      ;
    case Cpp::Namespace              :
      setWindowTitle (tr("`namespace` lists")) ;
    break                                      ;
  }                                            ;
  ui -> Items -> clear   (      )              ;
  setToolTips  (                )              ;
  EnterSQL     ( SC , plan->sql )              ;
    ListSource ( SC             )              ;
  LeaveSQL     ( SC , plan->sql )              ;
  FocusIn      (                )              ;
  return true                                  ;
}

void N::CppComponentLists::Refresh(void)
{
  SUID sourceUuid  = N::GetUuid ( ui->Source       ) ;
  SUID versionUuid = N::GetUuid ( ui->Version      ) ;
  ui -> Source  -> blockSignals        ( true             ) ;
  ui -> Version -> blockSignals        ( true             ) ;
  ui -> Items   -> clear               (                  ) ;
  ui -> Source  -> clear               (                  ) ;
  ui -> Version -> clear               (                  ) ;
  ui -> Version -> setEnabled          ( false            ) ;
  ui -> Items   -> setEnabled          ( false            ) ;
  ui -> Source  -> blockSignals        ( false            ) ;
  ui -> Version -> blockSignals        ( false            ) ;
  EnterSQL                             ( SC , plan->sql   ) ;
    ListSource                         ( SC               ) ;
    if (sourceUuid>0)                                       {
      *(ui->Source) <= sourceUuid                           ;
      ListVersion                      ( SC , sourceUuid  ) ;
      if (versionUuid>0)                                    {
        *(ui->Version) <= versionUuid                       ;
        ListItems                      ( SC , versionUuid ) ;
      }                                                     ;
    }                                                       ;
  LeaveSQL                             ( SC , plan->sql   ) ;
}

void N::CppComponentLists::ListSource(SqlConnection & SC)
{
  ui -> Source  -> blockSignals ( true   ) ;
  ui -> Version -> blockSignals ( true   ) ;
  ui -> Items   -> clear        (        ) ;
  ui -> Source  -> clear        (        ) ;
  ui -> Source  -> addItem      ( "" , 0 ) ;
  ui -> Version -> clear        (        ) ;
  ui -> Version -> setEnabled   ( false  ) ;
  //////////////////////////////////////////
  SUID  uuid                               ;
  int   language = vLanguageId             ;
  UUIDs Uuids    = SC.Uuids                (
    TABLES(Sources)                        ,
    "uuid"                                 ,
    "order by id desc ;"                 ) ;
  foreach (uuid,Uuids)                     {
    QString name = Name(SC,uuid,language)  ;
    ui -> Source -> addItem ( name,uuid )  ;
  }                                        ;
  //////////////////////////////////////////
  ui -> Source  -> blockSignals ( false )  ;
  ui -> Version -> blockSignals ( false )  ;
}

void N::CppComponentLists::ListVersion(SqlConnection & SC,SUID sourceUuid)
{
  QString Q                                     ;
  QString name                                  ;
  SUID    uuid                                  ;
  UUIDs   Uuids = SourceVersions(SC,sourceUuid) ;
  if (Uuids.count()<=0) return                  ;
  ui -> Version -> addItem ( "" , 0 )           ;
  foreach (uuid,Uuids)                          {
    Q = SC.sql.SelectFrom                       (
          "name"                                ,
          TABLES(SourceVersion)                 ,
          SC.WhereUuid(uuid)                  ) ;
    name = ""                                   ;
    if (SC.Fetch(Q)) name = SC.String(0)        ;
    if (name.length()>0)                        {
      ui -> Version -> addItem ( name , uuid )  ;
    }                                           ;
  }                                             ;
  ui -> Version -> setEnabled ( true )          ;
}

void N::CppComponentLists::ListItems(SqlConnection & SC,SUID versionUuid)
{
  QString Q                                                        ;
  QString name                                                     ;
  SUID    uuid                                                     ;
  UUIDs   Items                                                    ;
  UUIDs   Components                                               ;
  UUIDs   Files = SC.Uuids                                         (
            TABLES(SourceMaps),"file",SC.WhereUuid(versionUuid)  ) ;
  //////////////////////////////////////////////////////////////////
  foreach (uuid,Files)                                             {
    Q = SC.sql.SelectFrom                                          (
          "second"                                                 ,
          TABLES(Groups)                                           ,
          QString("where first = %1 "
                  "and t1 = %2 "
                  "and t2 = %3 "
                  "and relation = %4 "
                  "order by position,id asc ;"                     )
          .arg(uuid                                                )
          .arg(Types::File                               )
          .arg(Types::Component                          )
          .arg(Groups::Contains                      ) ) ;
    SqlLoopNow ( SC , Q )                                          ;
      SUID puid = SC . Uuid ( 0 )                                  ;
      if (!Components.contains(puid)) Components << puid           ;
    SqlLoopErr ( SC , Q )                                          ;
    SqlLoopEnd ( SC , Q )                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  int pdl  = 0                                                     ;
  int type = 0                                                     ;
  switch (What)                                                    {
    case Cpp::Define                                     :
      pdl  = 100079                                                ;
      type = (int)What                                             ;
    break                                                          ;
    case Cpp::Include                                    :
      pdl  = 100079                                                ;
      type = (int)What                                             ;
    break                                                          ;
    case Cpp::Enumeration                                :
      pdl  = 100081                                                ; // need to modify to 100079
      type = (int)What                                             ;
    break                                                          ;
    case Cpp::Namespace                                  :
      pdl  = 100081                                                ;
      type = (int)What                                             ;
    break                                                          ;
  }                                                                ;
  foreach (uuid,Components)                                        {
    Q = SC.sql.SelectFrom                                          (
          "type,pdl",TABLES(Codes),SC.WhereUuid(uuid)            ) ;
    if (SC.Fetch(Q))                                               {
      int TYPE = SC . Int ( 0 )                                    ;
      int PDL  = SC . Int ( 1 )                                    ;
      if (PDL==100079 || PDL==100081 || PDL==100900)               {
        if (TYPE==type) Items << uuid                              ;
      }                                                            ;
    }                                                              ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  foreach (uuid,Items)                                             {
    Q = SC.sql.SelectFrom("name",Table(),SC.WhereUuid(uuid)      ) ;
    if (SC.Fetch(Q))                                               {
      name = SC . String ( 0 )                                     ;
      NewTreeWidgetItem(item)                                      ;
      item -> setData ( 0,Qt::UserRole,uuid   )                    ;
      item -> setText ( 0,name                )                    ;
      ui   -> Items -> addTopLevelItem ( item )                    ;
    }                                                              ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  int total = ui->Items->topLevelItemCount()                       ;
  switch (What)                                                    {
    case Cpp::Define                                     :
      ui->Items->setToolTip(tr("Total %1 `#define`"  ).arg(total)) ;
    break                                                          ;
    case Cpp::Include                                    :
      ui->Items->setToolTip(tr("Total %1 `#include`" ).arg(total)) ;
    break                                                          ;
    case Cpp::Enumeration                                :
      ui->Items->setToolTip(tr("Total %1 `enum`"     ).arg(total)) ;
    break                                                          ;
    case Cpp::Namespace                                  :
      ui->Items->setToolTip(tr("Total %1 `namespace`").arg(total)) ;
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  ui -> Items -> setEnabled ( true  )                              ;
  Alert ( Done )                                                   ;
}

void N::CppComponentLists::SourceChanged(int)
{
  SUID uuid = N::GetUuid ( ui->Source     ) ;
  ui -> Version -> blockSignals ( true           ) ;
  ui -> Items   -> clear        (                ) ;
  ui -> Version -> clear        (                ) ;
  ui -> Version -> setEnabled   ( false          ) ;
  ui -> Items   -> setEnabled   ( false          ) ;
  setToolTips                   (                ) ;
  EnterSQL                      ( SC , plan->sql ) ;
    if (uuid>0) ListVersion     ( SC , uuid      ) ;
  LeaveSQL                      ( SC , plan->sql ) ;
  ui -> Version -> blockSignals ( false          ) ;
}

void N::CppComponentLists::VersionChanged(int)
{
  SUID uuid = N::GetUuid ( ui->Version    ) ;
  ui -> Items   -> clear        (                ) ;
  ui -> Items   -> setEnabled   ( false          ) ;
  setToolTips                   (                ) ;
  EnterSQL                      ( SC , plan->sql ) ;
    if (uuid>0) ListItems       ( SC , uuid      ) ;
  LeaveSQL                      ( SC , plan->sql ) ;
}
