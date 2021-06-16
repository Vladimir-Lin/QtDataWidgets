#include <datawidgets.h>
#include "ui_nScriptEditor.h"

N::ScriptEditor:: ScriptEditor (QWidget * parent,Plan * p)
                : Widget       (          parent,       p)
                , ui           (new Ui::nScriptEditor    )
                , Object       (0,Types::None            )
{
  WidgetClass            ;
  ui -> setupUi ( this ) ;
  Configure     (      ) ;
}

N::ScriptEditor::~ScriptEditor (void)
{
  delete ui;
}

void N::ScriptEditor::closeEvent(QCloseEvent * event)
{
  QWidget :: closeEvent(event) ;
}

void N::ScriptEditor::resizeEvent(QResizeEvent * event)
{
  QWidget :: resizeEvent ( event ) ;
  Relocation             (       ) ;
}

void N::ScriptEditor::showEvent(QShowEvent * event)
{
  QWidget :: showEvent ( event ) ;
  Relocation           (       ) ;
}

void N::ScriptEditor::Configure(void)
{
  LineEdit * sl = new LineEdit ( ui -> List                            ) ;
  sl   -> setToolTip           ( ui -> List -> toolTip ( )             ) ;
  ui   -> List -> setLineEdit  ( sl                                    ) ;
  ui   -> Type -> addItem ( tr("String mapper"  ) , Scripts::Mapper    ) ;
  ui   -> Type -> addItem ( tr("Forex script"   ) , Scripts::Forex     ) ;
  ui   -> Type -> addItem ( tr("Database"       ) , Scripts::SQL       ) ;
  ui   -> Type -> addItem ( tr("Version control") , Scripts::Version   ) ;
  ui   -> Type -> addItem ( tr("Image process"  ) , Scripts::Image     ) ;
  ui   -> Type -> addItem ( tr("Code generator" ) , Scripts::Generator ) ;
  ui   -> Type -> addItem ( tr("Document"       ) , Scripts::Document  ) ;
  ui   -> Type -> addItem ( tr("Matching"       ) , Scripts::Matching  ) ;
  plan -> setFont         ( this                                       ) ;
}

void N::ScriptEditor::Relocation(void)
{
  if ( IsNull ( plan ) ) return                                            ;
  ui -> List      -> resize ( width  ( ) - ui -> List      -> x      ( )   ,
                                           ui -> List      -> height ( ) ) ;
  ui -> Paraments -> resize ( width  ( ) - ui -> Paraments -> x      ( )   ,
                                           ui -> Paraments -> height ( ) ) ;
  ui -> Script    -> resize ( width  ( )                                   ,
                              height ( ) - ui -> Script    -> y      ( ) ) ;
}

void N::ScriptEditor::run(int T,ThreadData * data)
{
  if ( ! IsContinue ( data ) ) return ;
  switch ( T )                        {
    case 10001                        :
      startLoading (      )           ;
      load         ( data )           ;
      stopLoading  (      )           ;
    break                             ;
  }                                   ;
}

bool N::ScriptEditor::load(ThreadData * d)
{
  ui -> List -> blockSignals ( true        )                               ;
  ui -> List -> clear        (             )                               ;
  SqlConnection SC           ( plan -> sql )                               ;
  if ( SC . open ( FunctionString ) )                                      {
    SUID  u                                                                ;
    UUIDs Uuids = SC.Uuids                                                 (
      PlanTable(Scripts)                                                   ,
      "uuid"                                                               ,
      SC . OrderByAsc ( "id" )                                           ) ;
    for (int i = 0 ; IsContinue ( d ) && ( i < Uuids . count ( ) ) ; i++ ) {
      QString name                                                         ;
      u    = Uuids [ i ]                                                   ;
      name = SC . getName                                                  (
        PlanTable(Names)                                                   ,
        "uuid"                                                             ,
        vLanguageId                                                        ,
        u                                                                ) ;
      ui -> List -> addItem ( name , u )                                   ;
    }                                                                      ;
    SC . close  ( )                                                        ;
  }                                                                        ;
  SC   . remove ( )                                                        ;
  ui -> List -> lineEdit() -> setText ( "" )                               ;
  ui -> List -> blockSignals (false)                                       ;
  return true                                                              ;
}

bool N::ScriptEditor::startup(void)
{
  start ( 10001 ) ;
  return true     ;
}

void N::ScriptEditor::New(void)
{
  uuid = 0                                             ;
  ui -> Script    -> clear                   (       ) ;
  ui -> Paraments -> clear                   (       ) ;
  ui -> List      -> blockSignals            ( true  ) ;
  ui -> List      -> lineEdit ( ) -> setText ( ""    ) ;
  ui -> List      -> blockSignals            ( false ) ;
}

void N::ScriptEditor::Save(void)
{
  int Type = ui->Type->currentIndex()             ;
  if (Type<0) return                              ;
  Type = ui->Type->itemData(Type).toInt()         ;
  QString name   = ui->List  ->lineEdit()->text() ;
  QString script = ui->Script->toPlainText()      ;
  if (name  .length()<=0) return                  ;
  if (script.length()<=0) return                  ;
  SqlConnection SC(plan->sql)                     ;
  if (SC.open("nScriptEditor","Save"))            {
    QString Q                                     ;
    if (uuid<=0)                                  {
      uuid = SC.Unique                            (
        PlanTable(MajorUuid)                      ,
        "uuid"                                  ) ;
      SC.assureUuid                               (
        PlanTable(MajorUuid)                      ,
        uuid                                      ,
        Types::Script                           ) ;
      SC.assureUuid                               (
        PlanTable(Scripts  )                      ,
        uuid                                      ,
        Type                                    ) ;
      Q = SC.NameSyntax                           (
        PlanTable(Names)                          ,
        "uuid,language,name"                    ) ;
      SC.insertName(Q,uuid,vLanguageId,name)      ;
    } else                                        {
      Q = SC.sql.Update                           (
        PlanTable(Names)                          ,
    "where uuid = :UUID and language = :LANGUAGE" ,
            1,"name"                            ) ;
      SC.insertName(Q,uuid,vLanguageId,name)      ;
    }                                             ;
    Q = SC.sql.Update                             (
          PlanTable(Scripts)                      ,
          "where uuid = :UUID"                    ,
            2,"script","type"                   ) ;
    SC.Prepare ( Q )                              ;
    SC.Bind    ( "uuid"   , uuid                ) ;
    SC.Bind    ( "type"   , Type                ) ;
    SC.Bind    ( "script" , script.toUtf8()     ) ;
    SC.Exec    (   )                              ;
    SC.close()                                    ;
  }                                               ;
  SC.remove()                                     ;
  ui->List->blockSignals ( true        )          ;
  ui->List->addItem      ( name , uuid )          ;
  ui->List->blockSignals ( false       )          ;
  Alert ( Done )                                  ;
}

void N::ScriptEditor::Evaluate(void)
{
  int Type = ui->Type->currentIndex()                             ;
  if (Type<0) return                                              ;
  Type = ui->Type->itemData(Type).toInt()                         ;
  QString script = ui->Script->toPlainText()                      ;
  QString source = ui->Paraments->text()                          ;
  if (script.length()<=0) return                                  ;
  if (source.length()<=0) return                                  ;
  QString result                                                  ;
  switch ( Type )                                                 {
    case Scripts::Mapper                                          :
      result = Scripts::StringMapper(script,source)               ;
      QMessageBox::information(this,tr("String mapper"),result)   ;
    break                                                         ;
    case Scripts::Matching                                        :
      if (Scripts::StringMatching(script,source))                 {
        result = tr("Yes")                                        ;
      } else                                                      {
        result = tr("No" )                                        ;
      }                                                           ;
      QMessageBox::information(this,tr("String matching"),result) ;
    break                                                         ;
    case Scripts::Document                                        :
      result = Scripts::DocumentMapper(script,source)             ;
      QMessageBox::information(this,tr("Document mapper"),result) ;
    break                                                         ;
  }                                                               ;
}

void N::ScriptEditor::ScriptIndex(int index)
{
  if (index<0) return                                ;
  uuid = ui->List->itemData(index).toULongLong()     ;
  SqlConnection SC ( plan -> sql )                   ;
  if ( SC . open ( FunctionString ) )                {
    QString Q                                        ;
    Q = SC.sql.SelectFrom                            (
          "type,script"                              ,
          PlanTable(Scripts)                         ,
          SC.WhereUuid(uuid)                       ) ;
    if (SC.Fetch(Q))                                 {
      int        T = SC.Int  (0)                     ;
      QByteArray S = SC.Value(1).toByteArray()       ;
      ui->Type->blockSignals(true )                  ;
      (*ui->Type) <= T                               ;
      ui->Script->clear()                            ;
      ui->Script->setPlainText(QString::fromUtf8(S)) ;
      ui->Type->blockSignals(false)                  ;
    }                                                ;
    SC.close()                                       ;
  }                                                  ;
  SC.remove()                                        ;
}
