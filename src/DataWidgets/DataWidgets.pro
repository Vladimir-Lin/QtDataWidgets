NAME         = DataWidgets
TARGET       = $${NAME}

QT           = core
QT          += gui
QT          += network
QT          += sql
QT          += script
QT          += positioning
QT          += widgets
QT          += opengl
QT          += printsupport
QT          += multimedia
QT          += multimediawidgets
QT          += opengl
QT          += QtCUDA
QT          += QtOpenCV
QT          += Essentials
QT          += QtCalendar
QT          += SqlAnalyzer
QT          += QtGMP
QT          += QtGSL
QT          += QtAlgebra
QT          += QtDiscrete
QT          += QtFFT
QT          += Mathematics
QT          += QtFuzzy
QT          += QtFLP
QT          += QtFoundation
QT          += QtGeometry
QT          += QtGadgets
QT          += QtWorld
QT          += QtComponents
QT          += QtManagers
QT          += QtVCF

load(qt_build_config)
load(qt_module)

INCLUDEPATH += $${PWD}/../../include/$${NAME}

HEADERS     += $${PWD}/../../include/$${NAME}/datawidgets.h

include ($${PWD}/Files/Files.pri)
include ($${PWD}/SQL/SQL.pri)
include ($${PWD}/Documents/Documents.pri)
include ($${PWD}/Knowledge/Knowledge.pri)
include ($${PWD}/Manifest/Manifest.pri)
include ($${PWD}/Forth/Forth.pri)
include ($${PWD}/Scripts/Scripts.pri)
include ($${PWD}/Tags/Tags.pri)
include ($${PWD}/Mimes/Mimes.pri)
include ($${PWD}/Source/Source.pri)
include ($${PWD}/Data/Data.pri)

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri

include ($${PWD}/../../doc/Qt/Qt.pri)

TRNAME       = $${NAME}
include ($${PWD}/../../Translations.pri)
