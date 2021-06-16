#include <datawidgets.h>

N::SettingsEditor:: SettingsEditor (QWidget * parent,Plan * p)
                  : TableWidget    (          parent,       p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SettingsEditor::~SettingsEditor(void)
{
}

void N::SettingsEditor::Configure(void)
{
}

void N::SettingsEditor::run(void)
{
}

bool N::SettingsEditor::Shutdown(void)
{
  return true ;
}

bool N::SettingsEditor::Menu(QPoint pos)
{
  return true ;
}
