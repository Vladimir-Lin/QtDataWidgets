#include <datawidgets.h>

N::TagsList:: TagsList    (QWidget * parent,Plan * p)
            : ComboBox    (          parent,       p)
            , TagsManager (                        p)
            , TagType     (0                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::TagsList::~TagsList(void)
{
}

void N::TagsList::Configure(void)
{
  clear           (      ) ;
  plan -> setFont ( this ) ;
}

int N::TagsList::tagType(void)
{
  return TagType ;
}

void N::TagsList::setType(int type)
{
  TagType = type ;
}

void N::TagsList::setEdit(bool editable)
{
  setEditable ( editable )                              ;
  if ( editable )                                       {
    nConnect ( lineEdit() , SIGNAL(editingFinished())   ,
               this       , SLOT  (textChanged    ()) ) ;
  }                                                     ;
}

void N::TagsList::textChanged (void)
{
  QLineEdit * le = lineEdit ( ) ;
  if (IsNull(le)) return        ;
  QString name  = le->text()    ;
  if (name.length()<=0) return  ;
  int index = findText(name)    ;
  if (index>=0)                 {
    setCurrentIndex(index)      ;
    return                      ;
  }                             ;
  Append ( name )               ;
}

void N::TagsList::Append(QString name)
{
  SqlConnection SC(plan->sql)           ;
  if ( SC . open ( FunctionString ) )   {
    SUID uuid = NewTag ( SC , TagType ) ;
    if (uuid>0)                         {
      SC . assureName                   (
        PlanTable(Names)                ,
        uuid                            ,
        vLanguageId                     ,
        name                          ) ;
      addItem ( name , uuid )           ;
      int index = findText(name)        ;
      if (index>=0)                     {
        setCurrentIndex(index)          ;
        Alert ( Click )                 ;
      } else                            {
        Alert ( Error )                 ;
      }                                 ;
    } else                              {
      Alert ( Error )                   ;
    }                                   ;
    SC.close()                          ;
  } else                                {
    SC.close()                          ;
    Alert ( Error )                     ;
  }                                     ;
  SC.remove()                           ;
}

bool N::TagsList::load(ThreadData * d)
{
  NAMEs         names                                   ;
  SqlConnection SC ( plan -> sql )                      ;
  if ( SC . open ( FunctionString ) )                   {
    UUIDs uuids = TagsManager :: Uuids ( SC , TagType ) ;
    if ( uuids . count ( ) > 0 )                        {
      toNames   ( SC , vLanguageId , uuids , names )    ;
    }                                                   ;
    SC . close  (       )                               ;
  } else                                                {
    SC . close  (       )                               ;
    Alert       ( Error )                               ;
  }                                                     ;
  SC   . remove (       )                               ;
  if ( names . count ( ) <= 0 ) return false            ;
  emit assignNames ( names )                            ;
  return true                                           ;
}

void N::TagsList::List(void)
{
  clear (       ) ;
  start ( 10001 ) ;
}

void N::TagsList::run(int T,ThreadData * d)
{
  if ( ! IsContinue ( d ) ) return ;
  switch ( T )                     {
    case 10001                     :
      startLoading (   )           ;
      load         ( d )           ;
      stopLoading  (   )           ;
    break                          ;
  }                                ;
}

void N::TagsList::run(void)
{
  load ( &Data ) ;
}

bool N::TagsList::startup(void)
{
  List ( )    ;
  return true ;
}
