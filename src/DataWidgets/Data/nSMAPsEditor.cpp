#include <datawidgets.h>

N::SMAPsEditor:: SMAPsEditor ( QWidget * parent , Plan * p )
               : TreeWidget  (           parent ,        p )
               , maps        ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SMAPsEditor::~SMAPsEditor (void)
{
}

void N::SMAPsEditor::Configure(void)
{
  setSuggestion                ( QSize ( 240 , 480 )   ) ;
  setDragDropMode              ( NoDragDrop            ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( ExtendedSelection     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setColumnCount               ( 3                     ) ;
  ////////////////////////////////////////////////////////
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , tr("ID"   )       ) ;
  head -> setText              ( 1 , tr("Value")       ) ;
  head -> setText              ( 2 , ""                ) ;
  ////////////////////////////////////////////////////////
  assignHeaderItems            ( head                  ) ;
  setColumnWidth               ( 2 , 3                 ) ;
  ////////////////////////////////////////////////////////
  MountClicked                 ( 1                     ) ;
  MountClicked                 ( 2                     ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::SMAPsEditor::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false           ) ;
  DisableAllActions (                                ) ;
  AssignAction      ( Label        , windowTitle ( ) ) ;
  LinkAction        ( Insert       , Insert      ( ) ) ;
  LinkAction        ( Delete       , Delete      ( ) ) ;
  return true                                          ;
}

void N::SMAPsEditor::prepare(void)
{
  UUIDs           U                             ;
  SUID            u                             ;
  TreeWidgetItems Items                         ;
  clear ( )                                     ;
  if ( IsNull ( maps ) ) return                 ;
  U = (*maps) . keys ( )                        ;
  foreach ( u , U )                             {
    NewTreeWidgetItem ( it )                    ;
    SUID v = (*maps) [ u ]                      ;
    it    -> setText ( 0 , QString::number(u) ) ;
    it    -> setText ( 1 , QString::number(v) ) ;
    Items << it                                 ;
  }                                             ;
  ///////////////////////////////////////////////
  addTopLevelItems ( Items )                    ;
  emit AutoFit ( )                              ;
}

void N::SMAPsEditor::Insert(void)
{
  NewTreeWidgetItem ( it     ) ;
  addTopLevelItem   ( it     ) ;
  doubleClicked     ( it , 0 ) ;
}

void N::SMAPsEditor::Delete(void)
{
  QTreeWidgetItem * it = currentItem ( ) ;
  if ( IsNull ( it ) ) return            ;
  QString k = it -> text ( 0 )           ;
  int     i = indexOfTopLevelItem ( it ) ;
  bool    b = false                      ;
  SUID    u                              ;
  if ( i >= 0 ) takeTopLevelItem  ( i  ) ;
  u = k . toULongLong ( &b )             ;
  if ( ! b ) return                      ;
  (*maps) . remove ( u )                 ;
}

void N::SMAPsEditor::singleClicked(QTreeWidgetItem *item,int column)
{
  Alert ( Action ) ;
}

void N::SMAPsEditor::doubleClicked(QTreeWidgetItem *item,int column)
{
  QLineEdit * le = NULL                  ;
  switch ( column )                      {
    case 0                               :
      le = setLineEdit                   (
             item                        ,
             0                           ,
             SIGNAL(editingFinished())   ,
             SLOT  (keyPressed     ()) ) ;
      le -> setFocus(Qt::TabFocusReason) ;
    break                                ;
    case 1                               :
      le = setLineEdit                   (
             item                        ,
             1                           ,
             SIGNAL(editingFinished())   ,
             SLOT  (valuePressed   ()) ) ;
      le -> setFocus(Qt::TabFocusReason) ;
    break                                ;
  }                                      ;
}

void N::SMAPsEditor::keyPressed(void)
{
  QLineEdit * name = Casting      ( QLineEdit  , ItemWidget ) ;
  if (IsNull(name))                                           {
    removeOldItem                 (                         ) ;
    return                                                    ;
  }                                                           ;
  QString s = name        -> text (                         ) ;
  QString k = ItemEditing -> text ( 0                       ) ;
  QString v = ItemEditing -> text ( 1                       ) ;
  bool    b = false                                           ;
  SUID    u                                                   ;
  SUID    o                                                   ;
  SUID    z                                                   ;
  if ( s . length ( ) > 0 ) u = s . toULongLong ( &b )        ;
  if ( ! b ) s = k ; else                                     {
    b = false                                                 ;
    z = v . toULongLong ( &b )                                ;
    if ( b )                                                  {
      if ( k . length ( ) > 0 )                               {
        b = false                                             ;
        o = k . toULongLong ( &b )                            ;
        if ( b ) (*maps) . remove ( o )                       ;
      }                                                       ;
      (*maps) [ u ] = z                                       ;
    }                                                         ;
  }                                                           ;
  ItemEditing -> setText          ( 0 , s                   ) ;
  removeOldItem                   (                         ) ;
}

void N::SMAPsEditor::valuePressed(void)
{
  QLineEdit * name = Casting      ( QLineEdit  , ItemWidget ) ;
  if (IsNull(name))                                           {
    removeOldItem                 (                         ) ;
    return                                                    ;
  }                                                           ;
  QString s = name        -> text (                         ) ;
  QString k = ItemEditing -> text ( 0                       ) ;
  QString v = ItemEditing -> text ( 1                       ) ;
  bool    b = false                                           ;
  SUID    u                                                   ;
  SUID    z                                                   ;
  if ( k . length ( ) > 0 )                                   {
    u = s . toULongLong ( &b )                                ;
    if ( ! b ) s = v ; else                                   {
      b = false                                               ;
      z = k . toULongLong ( &b )                              ;
      if ( b ) (*maps) [ z ] = u                              ;
    }                                                         ;
  } else                                                      {
    u = s . toULongLong ( &b )                                ;
    if ( ! b ) s = ""                                         ;
  }                                                           ;
  ItemEditing -> setText          ( 1 , s                   ) ;
  removeOldItem                   (                         ) ;
}
