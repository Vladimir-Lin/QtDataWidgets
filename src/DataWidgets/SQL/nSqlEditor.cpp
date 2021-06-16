#include <datawidgets.h>
#include "ui_nSqlEditor.h"

N::SqlEditor:: SqlEditor        ( QWidget * parent , Plan * p )
             : TabWidget        (           parent ,        p )
             , ui               ( new Ui::nSqlEditor          )
             , Object           ( 0 , Types::SqlTable         )
//             , Editor           ( NULL                        )
             , TableColumns     ( NULL                        )
             , TableConstraints ( NULL                        )
             , Columns          ( NULL                        )
             , Constraints      ( NULL                        )
             , Tables           ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SqlEditor::~SqlEditor(void)
{
  delete ui ;
}

void N::SqlEditor::resizeEvent(QResizeEvent * event)
{
  QTabWidget :: resizeEvent ( event ) ;
  Relocation                (       ) ;
}

void N::SqlEditor::showEvent(QShowEvent * event)
{
  QTabWidget :: showEvent ( event ) ;
  Relocation              (       ) ;
}

void N::SqlEditor::Configure(void)
{
  ui    -> setupUi             ( this                             ) ;
  setCornerWidget              ( ui->Tools , Qt::BottomLeftCorner ) ;
//  Editor           = new VcfEditor       ( this            , Plan ) ;
  TableColumns     = new SqlItems       ( ui->Creations   , plan ) ;
  TableConstraints = new SqlRelations   ( ui->Creations   , plan ) ;
  Columns          = new SqlItems       ( ui->Columns     , plan ) ;
  Constraints      = new SqlRelations   ( ui->Constraints , plan ) ;
  Tables           = new SqlTables      ( ui->Tables      , plan ) ;
  ui->Tables   ->insertWidget( 0           , Tables               ) ;
  ui->Creations->addWidget   ( TableColumns                       ) ;
  ui->Creations->addWidget   ( TableConstraints                   ) ;
//  insertTab                  ( 0    , Editor , tr("Planner")      ) ;
//  setCurrentWidget           ( Editor                             ) ;
  ///////////////////////////////////////////////////////////////////
  NewTreeWidgetItem          ( schema                             ) ;
  schema -> setText          ( 0 , tr("Table")                    ) ;
  schema -> setTextAlignment ( 0 , Qt::AlignCenter                ) ;
  ui->Schema->setHeaderItem  ( schema                             ) ;
  ///////////////////////////////////////////////////////////////////
  plan  -> setFont           ( this                               ) ;
  ///////////////////////////////////////////////////////////////////
  connect(Tables          ,SIGNAL(Edit          (SUID))             ,
          this            ,SLOT  (EditTable     (SUID))           ) ;
}

bool N::SqlEditor::Relocation(void)
{
  QWidget * w = currentWidget          (            ) ;
  nKickOut            ( IsNull(w)           , false ) ;
  QSize s  = w  -> size                (            ) ;
  QSize sh = ui -> Schemes -> size     (            ) ;
  QRect gh = ui -> Schema  -> geometry (            ) ;
  QSize zz        ( s.width() , s.height() - gh.y() ) ;
  ui -> Schemes -> resize ( s.width() , sh.height() ) ;
  ui -> Schema  -> resize ( zz                      ) ;
  nKickOut            ( IsNull(Columns    ) , false ) ;
  Columns -> move                           ( 0 , 0 ) ;
  Columns -> resize                         ( s     ) ;
  nKickOut            ( IsNull(Constraints) , false ) ;
  Constraints -> move                       ( 0 , 0 ) ;
  Constraints -> resize                     ( s     ) ;
  return true                                         ;
}

bool N::SqlEditor::startup(void)
{
  Columns       -> uuid = uuid              ;
  Constraints   -> uuid = uuid              ;
  Tables        -> uuid = uuid              ;
  ui -> Schemes -> blockSignals ( true  )   ;
  ui -> Schemes -> clear   ( )              ;
  Columns       -> startup ( )              ;
  Constraints   -> startup ( )              ;
  Tables        -> startup ( )              ;
  EnterSQL ( SC , plan->sql )               ;
    QString Q                               ;
    Q = SC.sql.SelectFrom                   (
        "uuid,name"                         ,
        PlanTable(SqlConnections)           ,
        "order by id asc"                 ) ;
    SqlLoopNow ( SC , Q )                   ;
      SUID    u    = SC.Uuid  (0)           ;
      QString name = SC.String(1)           ;
      ui -> Schemes -> addItem ( name , u ) ;
    SqlLoopErr ( SC , Q )                   ;
    SqlLoopEnd ( SC , Q )                   ;
    if (uuid>0)                             {
    }                                       ;
  LeaveSQL ( SC , plan->sql )               ;
  ui -> Schemes -> blockSignals ( false )   ;
  Alert    ( Done           )               ;
  return true                               ;
}

void N::SqlEditor::SourceChanged(int)
{
  SUID puid = N::GetUuid(ui->Schemes)        ;
  EnterSQL(SC,plan->sql)                     ;
    Source = SC.GetConnection                (
             PlanTable(SqlConnections),puid) ;
  LeaveSQL(SC,plan->sql)                     ;
  Analyze ( )                                ;
}

void N::SqlEditor::Analyze(void)
{
  ui -> Schema     -> clear   ( )                     ;
  Tables           -> startup ( )                     ;
  TableColumns     -> clear   ( )                     ;
  TableConstraints -> clear   ( )                     ;
  EnterSQL( SC , Source )                             ;
    QString     Q                                     ;
    QStringList T = SC.Tables()                       ;
    for (int i=0;i<T.count();i++)                     {
      QString   Table    = T[i]                       ;
      QString   Creation = SC.Creation(Table)         ;
      SqlParser Parser                                ;
      NewTreeWidgetItem(item)                         ;
      item->setText(0,Table)                          ;
      ui->Schema->addTopLevelItem(item)               ;
      if (Parser.Parse(Source.dbName,Table,Creation)) {
        Columns     -> addTable ( SC , Parser )       ;
        Constraints -> addTable ( SC , Parser )       ;
      }                                               ;
      Tables -> addTable ( Source.dbName , Table    ) ;
    }                                                 ;
  LeaveSQL( SC , Source )                             ;
  Alert   ( Done        )                             ;
  Columns     -> SuitableColumns ( )                  ;
  Columns     -> reportItems     ( )                  ;
  Constraints -> SuitableColumns ( )                  ;
  Constraints -> reportItems     ( )                  ;
  Tables      -> reportItems     ( )                  ;
}

void N::SqlEditor::EditTable(SUID u)
{
  nDropOut ( uuid <= 0 )                                     ;
  nDropOut ( u    <= 0 )                                     ;
  TableColumns     -> clear   ( )                            ;
  TableConstraints -> clear   ( )                            ;
  TableColumns     -> uuid = u                               ;
  TableConstraints -> uuid = u                               ;
  TableColumns     -> type = Types::SqlTable                 ;
  TableConstraints -> type = Types::SqlTable                 ;
  TableColumns     -> startup ( )                            ;
  TableConstraints -> startup ( )                            ;
  EnterSQL( SC , plan->sql )                                 ;
    QString    Q                                             ;
    QString    Database                                      ;
    QString    Table                                         ;
    QString    Creation                                      ;
    SqlParser  Parser                                        ;
    Q = SC.sql.SelectFrom                                    (
          "value"                                            ,
          PlanTable(Variables)                               ,
          SC.sql.Where(2,"uuid","name" )                   ) ;
    SC . Prepare ( Q                   )                     ;
    SC . Bind    ( "uuid" , u          )                     ;
    SC . Bind    ( "name" , "Database" )                     ;
    if (SC.Exec() && SC.Next())                              {
      Database = SC.String(0)                                ;
    }                                                        ;
    if (Database.length()>0)                                 {
      Q = SC.sql.SelectFrom                                  (
            "value"                                          ,
            PlanTable(Variables)                             ,
            SC.sql.Where(2,"uuid","name" )                 ) ;
      SC . Prepare ( Q                   )                   ;
      SC . Bind    ( "uuid" , u          )                   ;
      SC . Bind    ( "name" , "Table"    )                   ;
      if (SC.Exec() && SC.Next())                            {
        Table = SC.String(0)                                 ;
      }                                                      ;
      if (Table.length()>0)                                  {
        Q = QString("use %1 ;").arg(Database)                ;
        SC . Query ( Q )                                     ;
        Creation = SC.Creation(Table)                        ;
        if (Creation.length()>0)                             {
          if (Parser.Parse(Database,Table,Creation))         {
            TableColumns     -> addTable    ( SC , Parser )  ;
            TableConstraints -> addTable    ( SC , Parser )  ;
            TableColumns     -> SuitableColumns (         )  ;
            TableConstraints -> SuitableColumns (         )  ;
            TableColumns     -> reportItems (             )  ;
            TableConstraints -> reportItems (             )  ;
          }                                                  ;
        }                                                    ;
      }                                                      ;
    }                                                        ;
  LeaveSQL( SC , plan->sql )                                 ;
  Alert   ( Done           )                                 ;
}

void N::SqlEditor::Creations(void)
{
  SUID puid = N::GetUuid(ui->Schemes)                           ;
  EnterSQL( SC , plan->sql )                                    ;
    QString    Q                                                ;
    for (int i=0;i<Tables->topLevelItemCount();i++)             {
      QTreeWidgetItem * item = Tables->topLevelItem(i)          ;
      SUID    u = nTreeUuid(item,0)                             ;
      QString Creation                                          ;
      Q = QString("use %1 ;").arg(item->text(1))                ;
      SC.Query(Q)                                               ;
      Q = QString("show create table %1 ;").arg(item->text(2))  ;
      if (SC.Fetch(Q)) Creation = SC.String(1)                  ;
      Q = QString("use %1 ;").arg(SC.sql.dbName)                ;
      SC.Query(Q)                                               ;
      if (Creation.length()>0)                                  {
        Q = SC.sql.SelectFrom("server" ,PlanTable(SqlCreations) ,
              SC.WhereUuid(u)                                 ) ;
        if (SC.Fetch(Q))                                        {
          Q = SC.sql.Update                                     (
                PlanTable(SqlCreations)                         ,
                "where uuid = :UUID"                            ,
                2                                               ,
                "server"                                        ,
                "creation"                                    ) ;
        } else                                                  {
          Q = SC.sql.InsertInto                                 (
                PlanTable(SqlCreations)                         ,
                3                                               ,
                "uuid"                                          ,
                "server"                                        ,
                "creation"                                    ) ;
        }                                                       ;
        SC . Prepare ( Q                              )         ;
        SC . Bind    ( "uuid"     , u                 )         ;
        SC . Bind    ( "server"   , puid              )         ;
        SC . Bind    ( "creation" , Creation.toUtf8() )         ;
        SC . Exec    (                                )         ;
      }                                                         ;
    }                                                           ;
  LeaveSQL( SC , plan->sql )                                    ;
  Alert   ( Done           )                                    ;
}
