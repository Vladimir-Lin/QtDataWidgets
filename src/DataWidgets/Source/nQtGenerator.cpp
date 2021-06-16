#include <datawidgets.h>

N::QtGenerator:: QtGenerator ( Plan * p )
               : plan        (        p )
{
}

N::QtGenerator::~QtGenerator(void)
{
}

QFileInfoList N::QtGenerator::Belong(QFileInfoList & List,QStringList Suffix)
{
  QFileInfoList L                     ;
  for (int i=0;i<List.count();i++)    {
    QString suffix = List[i].suffix() ;
    suffix = suffix.toLower()         ;
    if (Suffix.contains(suffix))      {
      L << List[i]                    ;
    }                                 ;
  }                                   ;
  return L                            ;
}

void N::QtGenerator::CreatePri(QWidget * parent)
{
  QDir    D   = plan->Dirs[Directory::Development]                   ;
  QString dir = QFileDialog::getExistingDirectory                    (
                  parent                                             ,
                  QObject::tr("Create Qt pri by scanning directory") ,
                  D.absoluteFilePath("")                           ) ;
  nDropOut ( dir . length ( ) <= 0 )                                 ;
  QDir       PRI ( dir )                                             ;
  QFileInfo  FI  ( dir )                                             ;
  QString    FIN = FI.absoluteFilePath()                             ;
  QFileInfoList FIL = File::FileInfos(PRI)                           ;
  nDropOut ( FIL . count  ( ) <= 0 )                                 ;
  QStringList   Hpp                                                  ;
  QStringList   Cpp                                                  ;
  QStringList   UIs                                                  ;
  Hpp << "h" ; Hpp << "hxx" ; Hpp << "hpp"                           ;
  Cpp << "c" ; Cpp << "cc"  ; Cpp << "cxx" ; Cpp << "cpp"            ;
  UIs << "ui"                                                        ;
  QByteArray    QtPri                                                ;
  QFileInfoList HPP = Belong(FIL,Hpp)                                ;
  QFileInfoList CPP = Belong(FIL,Cpp)                                ;
  QFileInfoList UIS = Belong(FIL,UIs)                                ;
  QtPri.append("INCLUDEPATH += $${PWD}/\n")                          ;
  if (HPP.count()>0)                                                 {
    QtPri.append("\n")                                               ;
    for (int i=0;i<HPP.count();i++)                                  {
      QString f = HPP[i].absoluteFilePath()                          ;
      f = f.replace(FIN,"")                                          ;
      f = QString("HEADERS     += $${PWD}%1\n").arg(f)               ;
      QtPri.append(f.toUtf8())                                       ;
    }                                                                ;
  }                                                                  ;
  if (CPP.count()>0)                                                 {
    QtPri.append("\n")                                               ;
    for (int i=0;i<CPP.count();i++)                                  {
      QString f = CPP[i].absoluteFilePath()                          ;
      f = f.replace(FIN,"")                                          ;
      f = QString("SOURCES     += $${PWD}%1\n").arg(f)               ;
      QtPri.append(f.toUtf8())                                       ;
    }                                                                ;
  }                                                                  ;
  if (UIS.count()>0)                                                 {
    QtPri.append("\n")                                               ;
    for (int i=0;i<UIS.count();i++)                                  {
      QString f = UIS[i].absoluteFilePath()                          ;
      f = f.replace(FIN,"")                                          ;
      f = QString("FORMS       += $${PWD}%1\n").arg(f)               ;
      QtPri.append(f.toUtf8())                                       ;
    }                                                                ;
  }                                                                  ;
  QString filename = FI.fileName()                                   ;
  filename += ".pri"                                                 ;
  filename  = PRI.absoluteFilePath(filename)                         ;
  if (QtPri.size()>0) File::toFile(filename,QtPri)                   ;
}
