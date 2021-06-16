/****************************************************************************
 *                                                                          *
 * Copyright (C) 2001 ~ 2016 Neutrino International Inc.                    *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_DATAWIDGETS_H
#define QT_DATAWIDGETS_H

#include <QtManagers>
#include <SqlAnalyzer>
#include <QtVCF>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_DATAWIDGETS_LIB)
#      define Q_DATAWIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_DATAWIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_DATAWIDGETS_EXPORT
#endif

namespace Ui                                  {
class Q_DATAWIDGETS_EXPORT nSqlStatistics     ;
class Q_DATAWIDGETS_EXPORT nSqlEditor         ;
class Q_DATAWIDGETS_EXPORT nScriptEditor      ;
class Q_DATAWIDGETS_EXPORT nSearchBar         ;
class Q_DATAWIDGETS_EXPORT nCppComponentLists ;
class Q_DATAWIDGETS_EXPORT PadEditor          ;
class Q_DATAWIDGETS_EXPORT PadBrowser         ;
}

namespace N
{

class Q_DATAWIDGETS_EXPORT ArrayLists        ;
class Q_DATAWIDGETS_EXPORT VariableLists     ;
class Q_DATAWIDGETS_EXPORT ObjectTypeEditor  ;
class Q_DATAWIDGETS_EXPORT ObjectLists       ;
class Q_DATAWIDGETS_EXPORT ParamentEditor    ;
class Q_DATAWIDGETS_EXPORT Classifiers       ;
class Q_DATAWIDGETS_EXPORT ParagraphEditor   ;
class Q_DATAWIDGETS_EXPORT FileLaunchers     ;
class Q_DATAWIDGETS_EXPORT PolyglotLists     ;
class Q_DATAWIDGETS_EXPORT SearchBar         ;
class Q_DATAWIDGETS_EXPORT ScriptEditor      ;
class Q_DATAWIDGETS_EXPORT InteractiveEditor ;
class Q_DATAWIDGETS_EXPORT TagsEditor        ;
class Q_DATAWIDGETS_EXPORT TagsList          ;
class Q_DATAWIDGETS_EXPORT MimeLists         ;
class Q_DATAWIDGETS_EXPORT MimeSuffix        ;
class Q_DATAWIDGETS_EXPORT PlainEditor       ;
class Q_DATAWIDGETS_EXPORT PadEditor         ;
class Q_DATAWIDGETS_EXPORT DocumentItems     ;
class Q_DATAWIDGETS_EXPORT DocumentView      ;
class Q_DATAWIDGETS_EXPORT PhraseLists       ;
class Q_DATAWIDGETS_EXPORT ParagraphLists    ;
class Q_DATAWIDGETS_EXPORT Terminologies     ;
class Q_DATAWIDGETS_EXPORT KnowledgeUnits    ;
class Q_DATAWIDGETS_EXPORT KnowledgeFields   ;
class Q_DATAWIDGETS_EXPORT KnowledgeBases    ;
class Q_DATAWIDGETS_EXPORT KeywordGroups     ;
class Q_DATAWIDGETS_EXPORT KeywordLists      ;
class Q_DATAWIDGETS_EXPORT UnicodeTable      ;
class Q_DATAWIDGETS_EXPORT CheckLists        ;
class Q_DATAWIDGETS_EXPORT CheckItems        ;
class Q_DATAWIDGETS_EXPORT FloatingPanel     ;
class Q_DATAWIDGETS_EXPORT DataTypeEditor    ;
class Q_DATAWIDGETS_EXPORT SourcesView       ;
class Q_DATAWIDGETS_EXPORT SourceVersions    ;
class Q_DATAWIDGETS_EXPORT VersionView       ;
class Q_DATAWIDGETS_EXPORT ProjectsView      ;
class Q_DATAWIDGETS_EXPORT ProjectItems      ;
class Q_DATAWIDGETS_EXPORT PdlExtensions     ;
class Q_DATAWIDGETS_EXPORT PdlSites          ;
class Q_DATAWIDGETS_EXPORT PlatformEditor    ;
class Q_DATAWIDGETS_EXPORT CppComponentLists ;
class Q_DATAWIDGETS_EXPORT CodingLists       ;
class Q_DATAWIDGETS_EXPORT QtBuilder         ;
class Q_DATAWIDGETS_EXPORT QtGenerator       ;
class Q_DATAWIDGETS_EXPORT FileView          ;
class Q_DATAWIDGETS_EXPORT WatchFiles        ;
class Q_DATAWIDGETS_EXPORT WatchPictures     ;
class Q_DATAWIDGETS_EXPORT DirectoryEditor   ;
class Q_DATAWIDGETS_EXPORT PictureBrowser    ;
class Q_DATAWIDGETS_EXPORT PickPicture       ;
class Q_DATAWIDGETS_EXPORT ArchiveList       ;
class Q_DATAWIDGETS_EXPORT ArchivePick       ;
class Q_DATAWIDGETS_EXPORT PadBrowser        ;
class Q_DATAWIDGETS_EXPORT VirtualFiles      ;
class Q_DATAWIDGETS_EXPORT SqlTableItems     ;
class Q_DATAWIDGETS_EXPORT DatabaseDock      ;
class Q_DATAWIDGETS_EXPORT DatabaseLinks     ;
class Q_DATAWIDGETS_EXPORT SqlServers        ;
class Q_DATAWIDGETS_EXPORT SqlItems          ;
class Q_DATAWIDGETS_EXPORT SqlRelations      ;
class Q_DATAWIDGETS_EXPORT SqlTables         ;
class Q_DATAWIDGETS_EXPORT SqlStatistics     ;
class Q_DATAWIDGETS_EXPORT SqlEditor         ;
class Q_DATAWIDGETS_EXPORT SqlQuery          ;
class Q_DATAWIDGETS_EXPORT SqlMonitor        ;
class Q_DATAWIDGETS_EXPORT SettingsEditor    ;
class Q_DATAWIDGETS_EXPORT SMAPsEditor       ;
class Q_DATAWIDGETS_EXPORT CloneFiles        ;
class Q_DATAWIDGETS_EXPORT IdentifierEditor  ;

/*****************************************************************************
 *                                                                           *
 *                               Data widgets                                *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT ArrayLists : public TreeDock
                                      , public Ownership
{
  Q_OBJECT
  public:

    explicit            ArrayLists    (StandardConstructor) ;
    virtual            ~ArrayLists    (void);

  protected:

    NAMEs  ArrayTypes ;
    QPoint dragPoint  ;
    bool   dropAction ;

    virtual bool        FocusIn       (void) ;
    virtual void        Configure     (void) ;

    virtual bool        hasItem       (void) ;
    virtual bool        startDrag     (QMouseEvent * event) ;
    virtual bool        fetchDrag     (QMouseEvent * event) ;
    virtual QMimeData * dragMime      (void) ;
    virtual void        dragDone      (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag    (QMouseEvent * event) ;

    virtual bool        acceptDrop    (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew       (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual void        run           (int Type,ThreadData * data) ;
    virtual bool        Load          (ThreadData * data) ;

  private:

  public slots:

    virtual bool        startup       (void) ;
    virtual void        Insert        (void) ;
    virtual void        Import        (void) ;
    virtual void        Replace       (void) ;
    virtual void        Export        (void) ;
    virtual void        ExportCpp     (void) ;

    virtual void        Replace       (QTreeWidgetItem * item) ;
    virtual void        Export        (QTreeWidgetItem * item) ;
    virtual void        ExportCpp     (QTreeWidgetItem * item) ;

  protected slots:

    virtual bool        Menu          (QPoint pos) ;
    virtual void        doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void        typeFinished  (int index) ;
    virtual void        nameFinished  (void) ;

  private slots:

  signals:

    void                Edit          (SUID uuid,int type) ;

} ;

class Q_DATAWIDGETS_EXPORT VariableLists : public TreeDock
                                         , public Object
                                         , public Relation
{
  Q_OBJECT
  public:

    explicit VariableLists         (StandardConstructor) ;
    virtual ~VariableLists         (void) ;

  protected:

    NAMEs  ValueTypes ;
    QPoint dragPoint  ;
    bool   dropAction ;

    virtual bool        FocusIn    (void) ;
    virtual void        Configure  (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool        acceptDrop (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew    (QWidget * source,const QMimeData * mime,QPoint pos) ;

    QByteArray          setValue   (int type,QString text) ;
    bool                setValue   (int type,QByteArray & Value,QTreeWidgetItem * item) ;

    virtual void        run        (int Type,ThreadData * data) ;
    virtual bool        Load       (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual void Insert            (void) ;
    virtual void Delete            (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void typeFinished      (int index) ;
    virtual void nameFinished      (void) ;
    virtual void idFinished        (void) ;
    virtual void valueFinished     (void) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT ObjectTypeEditor : public TreeWidget
{
  Q_OBJECT
  public:

    explicit ObjectTypeEditor  (StandardConstructor) ;
    virtual ~ObjectTypeEditor  (void) ;

    virtual QSize SuitableSize (void) ;

  protected:

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;
    virtual void run           (int Type,ThreadData * Data) ;

  private:

  public slots:

    virtual void List          (void) ;
    virtual void AllTypes      (void) ;
    virtual void AllItems      (void) ;
    virtual void DisplayCpp    (void) ;
    virtual void ExportNames   (void) ;
    virtual void ExportCpp     (void) ;
    virtual void ExportCpp     (QString filename,QString EnumName) ;
    virtual void ExportCpp     (QByteArray & Data,QString EnumName) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;

    virtual void nameChanged   (void) ;
    virtual void enumChanged   (void) ;
    virtual void removeOldItem (void) ;

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void stateChanged  (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void CppEnumeration        (QByteArray Enumeration) ;
    void Adjustment            (QWidget * widget,QSize size) ;

} ;

class Q_DATAWIDGETS_EXPORT ObjectLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit ObjectLists       (StandardConstructor) ;
    virtual ~ObjectLists       (void);

  protected:

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;

    virtual void run           (int Type,ThreadData * data) ;
    virtual bool Load          (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (UUIDs & Uuids,RMAPs & Types,NAMEs & Names) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT ParamentEditor : public TreeWidget
{
  Q_OBJECT
  public:

    explicit ParamentEditor    (StandardConstructor) ;
    virtual ~ParamentEditor    (void);

  protected:

    RMAPs           ParamentMaps  ;
    CMAPs           MapParaments  ;
    UUIDs           ParamentUuids ;
    NAMEs           ParamentNames ;
    QIntValidator * IntValidator  ;

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;

    virtual void run           (int Type,ThreadData * data) ;
    virtual bool Load          (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void Insert        (void) ;
    virtual void Rename        (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished  (void) ;
    virtual void typeFinished  (int index) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT Classifiers : public QObject
                                       , public Thread
{
  Q_OBJECT
  public:

    explicit Classifiers    (QObject * parent,Plan * plan) ;
    virtual ~Classifiers    (void) ;

  protected:

    Plan      * plan       ;
    int         ProgressId ;
    bool        ProgressGo ;
    qint64      Value      ;
    VarArgLists Commands   ;

    virtual void run        (void) ;

    virtual void TagUrlSets (VarArgs & command) ;
    virtual void Tag        (VarArgs & command) ;
    virtual void SiteTo     (VarArgs & command) ;
    virtual void PageTo     (VarArgs & command) ;
    virtual void Operate    (VarArgs & command) ;
    virtual void doTLDs     (VarArgs & command) ;
    virtual void doSLDs     (VarArgs & command) ;
    virtual void Ordering   (VarArgs & command) ;

    bool UrlFromPage        (SqlConnection & Connection ,
                             QString         Page       ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromMime        (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromEncoding    (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromTag         (SqlConnection & Connection ,
                             int             Language   ,
                             int             TagType    ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromParent      (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromSite        (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromTLD         (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromSLD         (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    virtual void Join       (SqlConnection & Connection ,
                             SUID            group      ,
                             int             t1         ,
                             int             t2         ,
                             int             relation   ,
                             UUIDs         & Uuids    ) ;
    void doTLDs             (SqlConnection & Connection ,
                             VarArgs       & command  ) ;
    void doSLDs             (SqlConnection & Connection ,
                             VarArgs       & command  ) ;

  private:

    void ReportIt           (QString syntax,int total) ;

  public slots:

    virtual void startup    (VarArgs     & command) ;
    virtual void startup    (VarArgLists & command) ;

  protected slots:

  private slots:

  signals:

    void Finished           (Classifiers * classifier) ;

} ;

class Q_DATAWIDGETS_EXPORT ParagraphEditor : public TreeWidget
{
  Q_OBJECT
  public:

    explicit ParagraphEditor  (StandardConstructor) ;
    virtual ~ParagraphEditor  (void);

    int Total                 (SqlConnection & Connection);
    int Page                  (void) ;

  protected:

    int       Paragraphs ;
    int       StartId    ;
    int       Items      ;
    SpinBox * valueAt    ;

    virtual void Configure    (void);

  private:

  public slots:

    void         Organize     (void) ;

    virtual bool startup      (int StartId,int Items = -1) ;

    void         Home         (void) ;
    void         End          (void) ;
    void         PageUp       (void) ;
    void         PageDown     (void) ;

  protected slots:

    void sectionPressed       (int index) ;
    void sectionDoubleClicked (int index) ;
    void startFinished        (void) ;
    void numberFinished       (void) ;
    void singleClicked        (QTreeWidgetItem * item,int column) ;
    void doubleClicked        (QTreeWidgetItem * item,int column) ;
    void removeOldItem        (void) ;
    void languageIndex        (int index) ;
    void editingFinished      (void) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT FileLaunchers : public TreeWidget
                                         , public MimeManager
{
  Q_OBJECT
  public:

    explicit FileLaunchers         (StandardConstructor) ;
    virtual ~FileLaunchers         (void);

  protected:

    virtual void Configure         (void) ;
    virtual bool FocusIn           (void) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual void Insert            (void) ;
    virtual void Delete            (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void extensionFinished (int) ;
    virtual void paramentsFinished (void) ;
    virtual void fetchApplication  (QTreeWidgetItem * item) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT PolyglotLists : public TreeDock
{
  Q_OBJECT
  public:

    explicit PolyglotLists     (StandardConstructor) ;
    virtual ~PolyglotLists     (void);

  protected:

    NAMEs Titles ;

    virtual void Configure     (void) ;

    void   addType             (QString name,SUID type) ;

  private:

  public slots:

    virtual bool startup       (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Translations (QString title,UUIDs       & Uuids) ;
    void Translations (QString title,QStringList & Names,UUIDs & Uuids) ;

} ;

class Q_DATAWIDGETS_EXPORT SearchBar : public ToolBar
{
  Q_OBJECT
  public:

    enum SearchMode     {
      Everything   =  0 ,
      NameIndex    =  1 ,
      PeopleOnly   = 51 ,
      VideoOnly    = 52 ,
      ClipOnly     = 53 ,
      DocumentOnly = 54 ,
      KeywordOnly  = 55 ,
      SystemOnly   = 56 ,
      DatabaseOnly = 57 ,
      PageOnly     = 58
    }                   ;

    enum MatchPattern {
      Exact = 0       ,
      SQL   = 1       ,
      Regex = 2     } ;

    enum SearchMode   Mode    ;
    enum MatchPattern Pattern ;
    QString           Keyword ;

    explicit SearchBar       (StandardConstructor) ;
    virtual ~SearchBar       (void);

  protected:

    Ui::nSearchBar     *  ui               ;
    QMenu              *  Menu             ;
    QComboBox          *  Search           ;
    LineEdit           *  Editor           ;
    QMutex                SearchMutex      ;
    bool                  Searching        ;
    QList<QAction      *> ItemActions      ;
    QList<QAction      *> MatchActions     ;
    QList<QAction      *> LanguageActions  ;
    QList<QActionGroup *> ActionGroups     ;
    BMAPs                 Booleans         ;
    UUIDs                 ResultUuids      ;
    RMAPs                 ResultTypes      ;
    NAMEs                 ResultNames      ;
    QString               ResultTitle      ;
    QString               SqlQueryString   ;
    QByteArray            DictionaryResult ;
    int                   DictionaryAlign  ;

    virtual void Configure    (void) ;
    virtual void run          (void) ;

    QString LanguageQuery     (void) ;
    QString NameQuery         (void) ;
    QString WildName          (nConnectUuid,int language,QString keyword) ;
    bool    ListKeywordName   (nConnectUuid,QByteArray & Body) ;
    bool    ListDictionary    (nConnectUuid,QByteArray & Body) ;

  private:

    void SearchEverything     (void) ;
    void SearchName           (void) ;
    void SearchPeople         (void) ;
    void SearchVideo          (void) ;
    void SearchClip           (void) ;
    void SearchDocument       (void) ;
    void SearchKeyword        (void) ;
    void SearchSystem         (void) ;
    void SearchSql            (void) ;
    void SearchPage           (void) ;

    bool UrlFromPage        (SqlConnection & Connection ,
                             QString         Page       ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromTag         (SqlConnection & Connection ,
                             int             Language   ,
                             int             TagType    ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromParent      (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromSite        (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromTLD         (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromSLD         (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;
    bool UrlFromColumn      (SqlConnection & Connection ,
                             Binary        * binary     ,
                             UUIDs         & Uuids    ) ;

  public slots:

    virtual bool Relocation   (void) ;
    virtual void Find         (QString keyword,enum SearchMode mode = Everything) ;

  protected slots:

    void popupMenu            (void) ;
    void nameFinished         (void) ;

  private slots:

    void SearchResults        (void) ;
    void PeopleResults        (void) ;
    void VideoResults         (void) ;
    void ClipResults          (void) ;
    void DocumentResults      (void) ;
    void UrlResults           (void) ;
    void KeywordResults       (void) ;
    void MatchKeywordResults  (void) ;
    void SqlQueryResults      (void) ;

  signals:

    void SendItems            (void) ;
    void UuidItems            (UUIDs & Uuids,RMAPs & Types,NAMEs & Names,QString title) ;
    void SendPeople           (void) ;
    void SendVideo            (void) ;
    void SendClip             (void) ;
    void SendDocuments        (void) ;
    void SendUrls             (void) ;
    void SendKeywords         (void) ;
    void SendMatchedKeywords  (void) ;
    void SendSqlQuery         (void) ;
    void ListPeople           (QString name,UUIDs & Uuids) ;
    void ListVideo            (QString name,UUIDs & Uuids) ;
    void ListClip             (QString name,UUIDs & Uuids) ;
    void ListDocuments        (QString name,UUIDs & Uuids) ;
    void ListUrls             (QString name,UUIDs & Uuids) ;
    void DictionaryExplain    (QString title,QByteArray & Explain,int arrangement) ;
    void KeywordMatched       (QString title,NAMEs & Keywords) ;
    void SqlQuery             (QString query) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                             Script widgets                                *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT ScriptEditor : public Widget
                                        , public Object
{
  Q_OBJECT
  public:

    explicit ScriptEditor    (StandardConstructor);
    virtual ~ScriptEditor    (void);

  protected:

    Ui::nScriptEditor * ui;

    virtual void Configure   (void) ;

    virtual void closeEvent  (QCloseEvent  * event) ;
    virtual void resizeEvent (QResizeEvent * event) ;
    virtual void showEvent   (QShowEvent   * event) ;

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool load        (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup     (void) ;
    virtual void Relocation  (void) ;

    virtual void New         (void) ;
    virtual void Save        (void) ;
    virtual void Evaluate    (void) ;
    virtual void ScriptIndex (int index) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT InteractiveEditor : public Widget
{
  Q_OBJECT
  public:

    QScrollArea * scrollArea ;

    explicit InteractiveEditor        (StandardConstructor) ;
    virtual ~InteractiveEditor        (void) ;

    virtual void  Join                (QScrollArea * area) ;
    virtual void  Report              (QString message) ;
    virtual void  Clear               (void) ;

    QStringList & CommandQueue        (void) ;

    virtual void  addDispatcher       (SyntaxDispatcher * dispatcher) ;
    virtual bool  canDispatch         (QString command,int & Type,int & Id) ;
    virtual bool  Dispatching         (int Type,int Id) ;

  protected:

    QByteArray                   History          ;
    QStringList                  Symbols          ;
    QStringList                  Queues           ;
    QStringList                  CommandList      ;
    QString                      Prompt           ;
    QString                      Commands         ;
    QString                      imString         ;
    QString                      peString         ;
    QTimer                     * cursorTimer      ;
    QRect                        PrevRect         ;
    bool                         blink            ;
    bool                         overwriteMode    ;
    bool                         isToolTip        ;
    int                          charWidth        ;
    int                          charHeight       ;
    int                          cursorX          ;
    int                          cursorY          ;
    int                          Mode             ;
    int                          Verbose          ;
    int                          HistoryIndex     ;
    LMAPs                        States           ;
    AlmostAnything               CommandVariables ;
    QMap<int,SyntaxDispatcher *> dispatchers      ;

    virtual bool FocusIn              (void) ;
    virtual void Configure            (void) ;

    virtual void contextMenuEvent     (QContextMenuEvent * event) ;
    virtual void focusInEvent         (QFocusEvent       * event) ;
    virtual void focusOutEvent        (QFocusEvent       * event) ;
    virtual void resizeEvent          (QResizeEvent      * event) ;
    virtual void showEvent            (QShowEvent        * event) ;
    virtual void keyPressEvent        (QKeyEvent         * event) ;
    virtual void mouseMoveEvent       (QMouseEvent       * event) ;
    virtual void mousePressEvent      (QMouseEvent       * event) ;
    virtual void paintEvent           (QPaintEvent       * event) ;
    virtual void closeEvent           (QCloseEvent       * event) ;

    virtual void inputMethodEvent     (QInputMethodEvent * event) ;
    virtual QVariant inputMethodQuery (Qt::InputMethodQuery query) const ;
    virtual bool InsertIM             (QString commit,QString preedit) ;

    virtual void paintCursor          (QPainter & painter) ;
    virtual void paintContents        (QPainter & painter) ;

  private:

    int          cursorAt             (QString L) ;
    bool         canDo                (QString value) ;
    QStringList  Split                (QString command) ;

  public slots:

    virtual bool startCursor          (void) ;
    virtual bool shutdown             (void) ;
    virtual void Adjust               (void) ;
    virtual void ensureVisible        (void) ;
    virtual void Evaluate             (void) ;
    virtual void Enter                (void) ;
    virtual void Enter                (QString command) ;
    virtual void setFont              (void) ;
    virtual void Copy                 (void) ;
    virtual void Paste                (void) ;
    virtual void setPrompt            (QString prompt) ;
    virtual void connectWidget        (QWidget * widget) ;
    virtual void BrowseFiles          (void) ;
    virtual void ClearCommands        (void) ;
    virtual bool LoadCommands         (QString filename) ;
    virtual bool SaveCommands         (QString filename) ;

  protected slots:

    virtual bool Menu                 (QPoint pos) ;

    virtual void GoUp                 (void) ;
    virtual void GoDown               (void) ;

  private slots:

    void         updateCursor         (void) ;

  signals:

    void         EmitUpdate           (void) ;
    void         LineInput            (void) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                                  Tag Widgets                              *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT TagsEditor : public ItemEditor
                                      , public TagsManager
{
  Q_OBJECT
  public:

    enum TagsEditMsgIDs                 {
         TagsEditJoinTags  = 12210001   ,
         TagsEditCounting  = 12210002   ,
         TagsEditShowItems = 12210003   ,
         TagsEditListItems = 12210004 } ;

    bool TagsCheckable ;

    explicit TagsEditor            (StandardConstructor) ;
    virtual ~TagsEditor            (void);

  protected:

    QList<void *> PendingItems ;
    Mutexz        mutex        ;

    virtual void  Configure        (void) ;
    virtual void  DispatchCommands (void) ;

    SUID          atTag            (QPoint pos) ;

    virtual void  setCheckable     (QTreeWidgetItem * item) ;

    virtual QMimeData * dragMime   (void) ;

    virtual bool  acceptDrop       (QWidget * source,const QMimeData * mime);
    virtual bool  dropNew          (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool  dropMoving       (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual UUIDs LoadUuids        (SqlConnection & Connection) ;
    virtual SUID  AppendUuid       (SqlConnection & Connection);

    virtual bool  dropColor        (nDeclDrops,const QColor & color ) ;
    virtual bool  dropTags         (nDeclDrops,const UUIDs  & Uuids ) ;
    virtual bool  dropPictures     (nDeclDrops,const UUIDs  & Uuids ) ;
    virtual bool  dropPeople       (nDeclDrops,const UUIDs  & Uuids ) ;
    virtual bool  dropVideos       (nDeclDrops,const UUIDs  & Uuids ) ;
    virtual bool  dropAlbums       (nDeclDrops,const UUIDs  & Uuids ) ;
    virtual bool  dropSetsAlgebras (nDeclDrops,const UUIDs  & Uuids ) ;

    bool          dropObjects      (QPoint pos,const UUIDs & Uuids,int ObjectType) ;

    virtual bool  addMenuItems     (MenuManager & menu,QPoint pos) ;
    QTreeWidgetItem * uuidAt       (SUID uuid) ;

    virtual void  run              (int Type,ThreadData * data) ;

    virtual void  JoinTag          (ThreadData * data) ;
    virtual void  Statistics       (ThreadData * data) ;
    virtual void  TagsUnion        (ThreadData * data) ;
    virtual void  TagsIntersection (ThreadData * data) ;

    virtual bool  GetFirstUuids    (SqlConnection & SC         ,
                                    UUIDs         & U          ,
                                    SUID            u          ,
                                    int             t1         ,
                                    int             t2         ,
                                    int             relation ) ;
    virtual bool  GetSecondUuids   (SqlConnection & SC         ,
                                    UUIDs         & U          ,
                                    SUID            u          ,
                                    int             t1         ,
                                    int             t2         ,
                                    int             relation ) ;

  private:

  public slots:

    virtual bool  JoinTag          (SUID tag,int ObjectType,const UUIDs & Uuids) ;
    virtual bool  RunMenuResult    (QAction * action,MenuManager & menu,QPoint pos) ;

    virtual void  Statistics       (void) ;

  protected slots:

    virtual bool  Menu             (QPoint pos);

    virtual void  SendPending      (void) ;

  private slots:

  signals:

    void nameCollection            (SUID uuid) ;
    void DispatchPending           (void) ;

} ;

class Q_DATAWIDGETS_EXPORT TagsList : public ComboBox
                                    , public TagsManager
{
  Q_OBJECT
  Q_PROPERTY(int TagType READ tagType WRITE setType)
  public:

    explicit TagsList        (StandardConstructor) ;
    virtual ~TagsList        (void);

    int      tagType         (void) ;

  protected:

    int TagType ;

    virtual void  Configure  (void) ;
    virtual void  run        (int Type,ThreadData * data) ;
    virtual void  run        (void) ;

    virtual bool  load       (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup     (void) ;

    void         setType     (int type) ;
    virtual void setEdit     (bool editable) ;
    virtual void List        (void) ;
    virtual void Append      (QString name) ;

  protected slots:

    virtual void textChanged (void) ;

  private slots:

  signals:

} ;

/*****************************************************************************
 *                                                                           *
 *                                  MIME Widgets                             *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT MimeLists : public ComboBox
                                     , public MimeManager
{
  Q_OBJECT
  public:

    explicit MimeLists        (StandardConstructor) ;
    virtual ~MimeLists        (void) ;

  protected:

  private:

  public slots:

    virtual void list         (QString Default = "") ;
    virtual void setEditable  (bool editable) ;

  protected slots:

    virtual void mimeFinished (void) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT MimeSuffix : public TreeWidget
                                      , public MimeManager
{
  Q_OBJECT
  public:

    explicit MimeSuffix        (StandardConstructor) ;
    virtual ~MimeSuffix        (void) ;

  protected:

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

    virtual void run           (int type,ThreadData * data) ;
    virtual bool ItemRunner    (int type,ThreadData * data) ;

    virtual void Listing       (ThreadData * data) ;

    virtual void Final         (QTreeWidgetItem * item) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void List          (void) ;
    virtual void Insert        (void) ;
    virtual void Delete        (void) ;
    virtual void Copy          (void) ;

  protected slots:

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;
    virtual void removeOldItem (void) ;
    virtual void mimeChanged   (QString mime) ;
    virtual void extensionEnd  (void) ;

  private slots:

  signals:

} ;

/*****************************************************************************
 *                                                                           *
 *                              Document widgets                             *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT PlainEditor : public QPlainTextEdit
                                       , public VirtualGui
                                       , public DocumentManager
                                       , public Thread
{
  Q_OBJECT
  public:

    SUID  document    ;
    SUID  uuid        ;
    UUIDs ScriptUuids ;
    NAMEs ScriptNames ;

    explicit PlainEditor          (StandardConstructor) ;
    virtual ~PlainEditor          (void);

  protected:

    QLineEdit * Editor   ;
    int         Language ;
    QString     Encoding ;
    QString     TempText ;

    virtual void Configure        (void) ;

    virtual void contextMenuEvent (QContextMenuEvent * event) ;
    virtual void focusInEvent     (QFocusEvent  * event) ;
    virtual void focusOutEvent    (QFocusEvent  * event) ;

    virtual bool FocusIn          (void) ;
    virtual bool FocusOut         (void) ;

    virtual void LoadScripts      (SqlConnection & Connection) ;

    virtual void run              (int Type,ThreadData * data) ;
    virtual void addParagraphs    (ThreadData * data) ;
    virtual void Saving           (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup          (QString title,QString version,SUID document,SUID fileuuid) ;
    virtual bool Load             (QString title,QString version,SUID document,SUID fileuuid) ;

    virtual void ZoomIn           (void) ;
    virtual void ZoomOut          (void) ;
    virtual void setFont          (void) ;
    virtual void Save             (void) ;
    virtual void SaveAs           (void) ;
    virtual void Merge            (void) ;
    virtual void Simplify         (void) ;
    virtual void Search           (void) ;
    virtual void Search           (QString keyword) ;
    virtual void Replace          (void) ;
    virtual void Replace          (QString keyword,QString replacement) ;

    virtual void MarkKeywords     (void) ;
    virtual void Mark             (QString selection) ;
    virtual void MarkKeywords     (QStringList keywords) ;

    virtual void addPeople        (QString text) ;
    virtual void addKeyword       (QString text) ;
    virtual void addKnowledge     (QString text) ;
    virtual void addTerminology   (QString text) ;
    virtual void addPhrase        (QString text) ;
    virtual void addParagraph     (QString text) ;

    virtual void toSimplified     (void) ;
    virtual void toTraditional    (void) ;

    virtual void setProperties    (void) ;
    virtual void addParagraphs    (void) ;

    virtual void RunScript        (SUID uuid) ;

  protected slots:

    virtual bool Menu             (QPoint pos);
    virtual void lineEditor       (void) ;
    virtual void editingFinished  (void) ;
    virtual void searchPressed    (void) ;
    virtual void replacePressed   (void) ;

    virtual void StartBusy        (void) ;
    virtual void StopBusy         (void) ;

    virtual void tempText         (void) ;

  private slots:

  signals:

    void OnBusy                   (void) ;
    void GoRelax                  (void) ;
    void assignText               (void) ;
    void assignPlainText          (QString text) ;
    void RelatedPeople            (SUID uuid,QString title) ;
    void RelatedParagraphs        (SUID uuid,QString title) ;
    void RelatedKeywords          (SUID uuid,QString title) ;
    void RelatedPhrases           (SUID uuid,QString title) ;
    void RelatedKnowledge         (SUID uuid,QString title) ;

} ;

class Q_DATAWIDGETS_EXPORT PadEditor : public Splitter
{
  Q_OBJECT
  public:

    QDir Root    ;
    QDir Current ;

    explicit PadEditor       (StandardConstructor) ;
    virtual ~PadEditor       (void) ;

    virtual QSize sizeHint   (void) const ;

  protected:

    QMap<int,QAction *> Actions  ;
    QMap<int,BMAPs    > Displays ;

    Ui::PadEditor * ui     ;
    int             LastId ;  // remove this next time you see this

    virtual void Configure   (void) ;

  private:

    void addAction           (int Id,QString text,QIcon icon) ;

  public slots:

    void         setBack     (QString text) ;
    void         Display     (int conf) ;
    void         setLoadable (bool enable) ;

    virtual void Load        (QString filename) ;
    virtual void Save        (QString filename) ;
    virtual void setFilename (QString filename) ;
    virtual void setBody     (QByteArray & body) ;

    virtual void New         (void) ;
    virtual void Load        (void) ;
    virtual void Save        (void) ;
    virtual void Edit        (void) ;
    virtual void File        (void) ;
    virtual void Cancel      (void) ;
    virtual void SaveTo      (void) ;
    virtual void LoadFrom    (void) ;

    virtual void Find        (void) ;
    virtual void Look        (void) ;
    virtual void Replace     (void) ;
    virtual void Replacement (void) ;
    virtual void ZoomIn      (void) ;
    virtual void ZoomOut     (void) ;

  protected slots:

    void Mark                (QString selection,int total = 1,int from = -1) ;
    void Replace             (QString keyword,QString replacement) ;

  private slots:

  signals:

    void Back                (void) ;

} ;

class Q_DATAWIDGETS_EXPORT DocumentItems : public TreeDock
                                         , public DocumentManager
{
  Q_OBJECT
  public:

    SUID Division   ;
    int  SourceType ;

    explicit DocumentItems         (StandardConstructor) ;
    virtual ~DocumentItems         (void) ;

  protected:

    QMap<SUID,QTreeWidgetItem *> documentItems ;
    UUIDs                        documentUuids ;
    UUIDs                        ExtraUuids    ;
    enum Qt::SortOrder           Sorting       ;
    QPoint                       dragPoint     ;
    QTreeWidgetItem           *  singleItem    ;
    int                          singleColumn  ;
    QDateTime                    singleTime    ;
    LineEdit                  *  Editor        ;
    QProgressBar              *  Progress      ;
    int                          ProgressValue ;
    QTimer                    *  ProgressTimer ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime);
    virtual bool dropNew           (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropDocuments     (nDeclDrops,const UUIDs   & Uuids) ;

    virtual bool FocusIn           (void) ;
    virtual bool FocusOut          (void) ;

    virtual void Configure         (void) ;

    virtual void setItemIcon       (QTreeWidgetItem * item,int column,int extension) ;
    void         lineEditor        (void) ;

    virtual void run               (void) ;
    virtual void run               (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (SUID division) ;
    virtual bool startup           (UUIDs & Uuids) ;

    virtual void List              (void) ;
    virtual void SaveAs            (void) ;
    virtual void Export            (void) ;
    virtual void Import            (void) ;

    virtual void Remove            (void) ;
    virtual void Detach            (void) ;
    virtual void Search            (void) ;
    virtual void Rename            (void) ;
    virtual void Rename            (QTreeWidgetItem * item) ;

    virtual void Add               (void) ;
    virtual void addDocument       (void) ;
    virtual void addPlainText      (QTreeWidgetItem * item) ;
    virtual void addVcf            (QTreeWidgetItem * item) ;
    virtual void importFiles       (QTreeWidgetItem * item) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void documentFinished  (void) ;
    virtual void plainFinished     (void) ;
    virtual void vcfFinished       (void) ;
    virtual void searchFinished    (void) ;
    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void fileClicked       (QTreeWidgetItem * item) ;
    virtual void rateClicked       (QTreeWidgetItem * item) ;
    virtual void rateFinished      (void) ;

  private slots:

    void AppendTopItems            (void) ;
    void ProgressChanged           (void) ;

  signals:

    void AppendItems               (void) ;
    void Open                      (SUID    document  ,
                                    SUID    fileuuid  ,
                                    int     extension ,
                                    QString version   ,
                                    QString title   ) ;
    void Processors                (UUIDs documents) ;

} ;

class Q_DATAWIDGETS_EXPORT DocumentView : public GroupView
                                        , public DocumentManager
{
  Q_OBJECT
  public:

    explicit DocumentView         (StandardConstructor) ;
    virtual ~DocumentView         (void) ;

  protected:

    virtual void    Configure     (void) ;

    virtual bool    dropDocuments (nDeclDrops,const UUIDs & Uuids) ;
    virtual QString ToolTipTotoal (int total) ;
    virtual int     Counting      (SqlConnection & Connection,SUID uuid) ;
    virtual void    reportItems   (void) ;

  private:

  public slots:

  protected slots:

    virtual bool Menu             (QPoint pos) ;

    virtual bool JoinImage        (SUID uuid,const QImage & image) ;
    virtual bool JoinPictures     (SUID uuid,const UUIDs & Uuids) ;

  private slots:

  signals:

    void Translations             (QString title,UUIDs & Uuids) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                              Knowledge widgets                            *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT PhraseLists : public TreeWidget
                                       , public Object
                                       , public Relation
                                       , public DocumentManager
{
  Q_OBJECT
  public:

    explicit PhraseLists   (StandardConstructor) ;
    virtual ~PhraseLists   (void);

  protected:

    virtual void Configure (void) ;
    virtual bool FocusIn   (void) ;

  private:

  public slots:

    virtual bool startup   (void) ;

  protected slots:

    virtual bool Menu      (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT ParagraphLists : public TreeWidget
                                          , public Object
                                          , public Relation
                                          , public DocumentManager
{
  Q_OBJECT
  public:

    explicit ParagraphLists  (StandardConstructor) ;
    virtual ~ParagraphLists  (void);

  protected:

    virtual void Configure   (void) ;
    virtual bool FocusIn     (void) ;

  private:

  public slots:

    virtual bool startup     (void) ;

  protected slots:

    virtual bool Menu        (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT Terminologies : public TreeDock
                                         , public Object
                                         , public Relation
                                         , public KnowledgeManager
{
  Q_OBJECT
  public:

    explicit Terminologies           (StandardConstructor) ;
    virtual ~Terminologies           (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        Configure    (void) ;
    virtual bool        FocusIn      (void) ;

    virtual bool        hasItem      (void);
    virtual bool        startDrag    (QMouseEvent * event);
    virtual bool        fetchDrag    (QMouseEvent * event);
    virtual QMimeData * dragMime     (void);
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag   (QMouseEvent * event);

    virtual bool acceptDrop          (QWidget * source,const QMimeData * mime);
    virtual bool dropNew             (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropTerminologies   (QWidget * source,QPointF pos,const UUIDs & Uuids ) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual void Insert              (void) ;
    virtual void Rename              (void) ;
    virtual void Delete              (void) ;
    virtual void Paste               (void) ;
    virtual void Import              (void) ;
    virtual void Export              (void) ;

    virtual void HeaderMenu          (QHeaderView * view,QPoint pos) ;
    virtual void Import              (QStringList terms) ;

  protected slots:

    virtual bool Menu                (QPoint pos) ;
    virtual void doubleClicked       (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished        (void) ;

  private slots:

  signals:

    void Edit                        (QString title,UUIDs & Uuids) ;

} ;

class Q_DATAWIDGETS_EXPORT KnowledgeUnits : public TreeDock
                                          , public Object
                                          , public Relation
                                          , public KnowledgeManager
{
  Q_OBJECT
  public:

    explicit KnowledgeUnits          (StandardConstructor) ;
    virtual ~KnowledgeUnits          (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        Configure    (void) ;
    virtual bool        FocusIn      (void) ;

    virtual bool        hasItem      (void);
    virtual bool        startDrag    (QMouseEvent * event);
    virtual bool        fetchDrag    (QMouseEvent * event);
    virtual QMimeData * dragMime     (void);
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag   (QMouseEvent * event);

    virtual bool acceptDrop          (QWidget * source,const QMimeData * mime);
    virtual bool dropNew             (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool dropTerminologies   (nDeclDrops,const UUIDs & Uuids ) ;
    virtual bool dropKnowledge       (nDeclDrops,const UUIDs & Uuids ) ;

  private:

  public slots:

    virtual bool startup        (void) ;
    virtual void Insert         (void) ;
    virtual void Rename         (void) ;
    virtual void Delete         (void) ;
    virtual void Export         (void) ;

    virtual void HeaderMenu     (QHeaderView * view,QPoint pos) ;

  protected slots:

    virtual bool Menu           (QPoint pos) ;
    virtual void doubleClicked  (QTreeWidgetItem * item,int column) ;
    virtual void nameFinished   (void) ;

  private slots:

  signals:

    void Terminology (QWidget * widget,SUID uuid,int SourceType,QString knowledge) ;
    void Edit        (QString title,UUIDs & Uuids) ;

} ;

class Q_DATAWIDGETS_EXPORT KnowledgeFields : public ListDock
                                           , public Object
                                           , public Relation
                                           , public GroupItems
                                           , public KnowledgeManager
{
  Q_OBJECT
  public:

    explicit KnowledgeFields         (StandardConstructor) ;
    virtual ~KnowledgeFields         (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        Configure     (void) ;
    virtual bool        FocusIn       (void) ;

    virtual bool        hasItem       (void);
    virtual bool        startDrag     (QMouseEvent * event);
    virtual bool        fetchDrag     (QMouseEvent * event);
    virtual QMimeData * dragMime      (void);
    virtual void        dragDone      (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag    (QMouseEvent * event);

    virtual QString     MimeType      (const QMimeData * mime);
    virtual UUIDs       MimeUuids     (const QMimeData * mime,QString mimetype) ;

    virtual bool        acceptDrop    (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew       (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving    (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend    (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropTags      (nDeclDrops,const UUIDs & Uuids ) ;
    virtual bool        dropPictures  (nDeclDrops,const UUIDs & Uuids ) ;
    virtual bool        dropKnowledge (nDeclDrops,const UUIDs & Uuids ) ;
    virtual bool        dropFields    (nDeclDrops,const UUIDs & Uuids ) ;

  private:

  public slots:

    virtual bool startup              (void) ;
    virtual void Copy                 (void) ;
    virtual void Insert               (void) ;
    virtual void Delete               (void) ;
    virtual void Rename               (void) ;
    virtual void Rename               (QListWidgetItem * item) ;
    virtual void Refresh              (QListWidgetItem * item);

  protected slots:

    virtual bool Menu                 (QPoint pos) ;

    virtual void renameFinished       (void) ;
    virtual void editingFinished      (void) ;

  private slots:

  signals:

    void         Knowledge            (QWidget * widget,SUID uuid,int SourceType,QString knowledge) ;

} ;

class Q_DATAWIDGETS_EXPORT KnowledgeBases : public ListDock
                                          , public GroupItems
                                          , public KnowledgeManager
{
  Q_OBJECT
  public:

    explicit KnowledgeBases          (StandardConstructor) ;
    virtual ~KnowledgeBases          (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        Configure    (void) ;
    virtual bool        FocusIn      (void) ;

    virtual bool        hasItem      (void);
    virtual bool        startDrag    (QMouseEvent * event);
    virtual bool        fetchDrag    (QMouseEvent * event);
    virtual QMimeData * dragMime     (void);
    virtual void        dragDone     (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag   (QMouseEvent * event);

    virtual QString     MimeType     (const QMimeData * mime);
    virtual UUIDs       MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual bool        acceptDrop   (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving   (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend   (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual bool        dropTags     (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropPictures (nDeclDrops,const UUIDs       & Uuids ) ;
    virtual bool        dropFields   (nDeclDrops,const UUIDs       & Uuids ) ;

  private:

  public slots:

    virtual bool startup             (void) ;
    virtual void Copy                (void) ;
    virtual void Insert              (void) ;
    virtual void Rename              (void) ;
    virtual void Rename              (QListWidgetItem * item) ;
    virtual void Refresh             (QListWidgetItem * item);

  protected slots:

    virtual bool Menu                (QPoint pos) ;

    virtual void renameFinished      (void) ;
    virtual void editingFinished     (void) ;

  private slots:

  signals:

    void         Field               (QWidget * widget     ,
                                      SUID      uuid       ,
                                      int       SourceType ,
                                      QString   title    ) ;

} ;

class Q_DATAWIDGETS_EXPORT KeywordGroups : public TreeDock
                                         , public GroupItems
{
  Q_OBJECT
  public:

    explicit KeywordGroups       (StandardConstructor) ;
    virtual ~KeywordGroups       (void);

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual bool FocusIn         (void) ;
    virtual void Configure       (void) ;

    virtual bool acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool dropNew         (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool dropKeywords    (nDeclDrops,const UUIDs & Uuids) ;

    virtual void run             (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void List            (void) ;
    virtual void New             (void) ;
    virtual void Paste           (void) ;
    virtual void Paste           (QString text);
    virtual void Export          (void) ;
    virtual void Copy            (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;
    virtual void editingFinished (void) ;

  private slots:

  signals:

    void Group                   (QString name,SUID uuid) ;
    void Edit                    (QString title,UUIDs & Uuids) ;

} ;

class Q_DATAWIDGETS_EXPORT KeywordLists : public TreeDock
                                        , public Object
                                        , public DocumentManager
{
  Q_OBJECT
  public:

    int  Language   ;

    explicit KeywordLists          (StandardConstructor) ;
    virtual ~KeywordLists          (void) ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual bool FocusIn           (void) ;
    virtual void Configure         (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime) ;
    virtual bool dropNew           (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool dropKeywords      (nDeclDrops,const UUIDs & Uuids) ;

    virtual void showKeyword       (SqlConnection & SC,SUID uuid) ;

    bool ListKeywordName           (SqlConnection & SC,SUID uuid,QByteArray & Body) ;
    bool ListDictionary            (SqlConnection & SC,SUID uuid,QByteArray & Body) ;

    virtual void run               (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual bool startup           (UUIDs & Uuids) ;
    virtual void List              (void) ;
    virtual void New               (void) ;
    virtual void Delete            (void) ;
    virtual void Paste             (void) ;
    virtual void Paste             (QString text) ;
    virtual void Export            (void) ;
    virtual void Copy              (void) ;
    virtual void setProperties     (void) ;
    virtual void Explain           (SUID uuid) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void editingFinished   (void) ;

  private slots:

  signals:

    void DictionaryExplain (QString title,QByteArray & Explain,int arrangement) ;

} ;

class Q_DATAWIDGETS_EXPORT UnicodeTable : public TableWidget
{
  Q_OBJECT
  public:

    explicit UnicodeTable         (StandardConstructor) ;
    virtual ~UnicodeTable         (void);

    virtual QSize sizeHint        (void) const ;

  protected:

    qreal         Factor    ;
    QFont         Font      ;
    Qt::ItemFlags CellFlags ;

    virtual bool FocusIn          (void) ;
    virtual void Configure        (void) ;

    virtual void run              (int Type,ThreadData * data) ;

  private:

    qreal   FactorLevel             (qreal factor,bool enlarge);
    int     toUtf8                  (uint32_t character,unsigned char * utf8) ;
    QString toUtf8                  (uint32_t character) ;

  public slots:

    void assignFont               (QFont font);
    void selectFont               (void);
    void ZoomIn                   (void);
    void ZoomOut                  (void);

    void load (Qt::ItemFlags flags = Qt::ItemIsSelectable    |
                                     Qt::ItemIsEditable      |
                                     Qt::ItemIsDragEnabled   |
                                     Qt::ItemIsDropEnabled   |
                                     Qt::ItemIsUserCheckable |
                                     Qt::ItemIsEnabled     ) ;

    void BestSize                 (void) ;
    void startup                  (void) ;
    void List                     (void) ;
    void Export                   (void) ;

  protected slots:

    virtual bool Menu             (QPoint pos) ;
    void fontChanged              (void);

  private slots:

  signals:

    void AutoFit                  (void) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                                Manifest GUIs                              *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT CheckLists : public TreeDock
                                      , public Object
{
  Q_OBJECT
  public:

    bool backgrounding ;

    explicit CheckLists        (StandardConstructor) ;
    virtual ~CheckLists        (void) ;

  protected:

    Qt::SortOrder Sorting      ;

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;

    virtual void run           (int Type,ThreadData * Data) ;

    virtual bool acceptTapHold (QTapAndHoldGesture * gesture) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void List          (void) ;
    virtual void Insert        (void) ;

  protected slots:

    bool         Menu          (QPoint pos) ;

    virtual void doubleClicked (QTreeWidgetItem *item, int column) ;
    virtual void enumFinished  (void) ;
    virtual void nameFinished  (void) ;

  private slots:

  signals:

    void         Edit          (QWidget * widget,QString name,SUID uuid) ;

} ;

class Q_DATAWIDGETS_EXPORT CheckItems : public TreeDock
                                      , public Object
{
  Q_OBJECT
  public:

    bool backgrounding ;

    explicit CheckItems        (StandardConstructor) ;
    virtual ~CheckItems        (void) ;

  protected:

    int           Position     ;
    Qt::SortOrder Sorting      ;

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;

    virtual void run           (int Type,ThreadData * Data) ;

    virtual bool acceptTapHold (QTapAndHoldGesture * gesture) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void List          (void) ;
    virtual void Insert        (void) ;
    virtual void Export        (void) ;
    virtual void Delete        (void) ;
    virtual void Paste         (void) ;

  protected slots:

    bool         Menu          (QPoint pos) ;

    virtual void doubleClicked (QTreeWidgetItem *item, int column) ;
    virtual void enumFinished  (void) ;
    virtual void valueFinished (void) ;
    virtual void rateFinished  (void) ;

  private slots:

  signals:

    void AppendItem            (void) ;

} ;

class Q_DATAWIDGETS_EXPORT FloatingPanel : public Widget
{
  Q_OBJECT
  public:

    bool Fading ;

    explicit FloatingPanel     (StandardConstructor) ;
    virtual ~FloatingPanel     (void) ;

  protected:

    SUID                 uuid       ;
    int                  type       ;
    QList<QToolButton *> buttons    ;
    QList<QAction     *> actions    ;
    QMap<QAction *,SUID> CatalogMap ;

    virtual void focusOutEvent (QFocusEvent * event);

  private:

  public slots:

    void clear                 (void);
    void add                   (SUID Id,QString text,QIcon icon);
    void assign                (SUID uuid,int type,QRect Rect,int user) ;
    void FadeOut               (void);
    void Arrangement           (void);
    void display               (void);

  protected slots:

    void activate              (QAction * action);

  private slots:

  signals:

    void Finished              (void) ;
    void Execute               (SUID uuid,QString name) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                                Coding widgets                             *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT DataTypeEditor : public TreeWidget
                                          , public CodeManager
{
  Q_OBJECT
  public:

    explicit DataTypeEditor  (StandardConstructor) ;
    virtual ~DataTypeEditor  (void) ;

  protected:

    virtual void Configure   (void) ;

    virtual bool FocusIn     (void) ;
    virtual bool FocusOut    (void) ;

  private:

  public slots:

    virtual bool startup     (void) ;
    virtual void New         (void) ;

  protected slots:

    virtual void removeOldItem (void) ;
    virtual void nameFinished  (void) ;
    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SourcesView : public SnapView
{
  Q_OBJECT
  public:

    explicit SourcesView                (StandardConstructor) ;
    virtual ~SourcesView                (void);

  protected:

    bool dropAction ;

    virtual bool        FocusIn         (void);

    virtual void        Configure       (void);

    virtual QMimeData * dragMime        (void);

    virtual bool        acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType        (const QMimeData * mime);
    virtual UUIDs       MimeUuids       (const QMimeData * mime,QString mimetype) ;

    virtual bool        dropImage       (nDeclDrops,const QImage      & image) ;
    virtual bool        dropPictures    (nDeclDrops,const UUIDs       & Uuids) ;
    virtual bool        dropSources     (nDeclDrops,const UUIDs       & Uuids) ;

    bool                setGroupIcon    (SUID uuid,const QImage & image) ;

    virtual void        run             (void) ;

  private:

  public slots:

    virtual bool        startup         (void);

    virtual void        New             (void) ;
    virtual void        Delete          (void) ;
    virtual void        Rename          (QListWidgetItem * item) ;
    virtual void        Remove          (SUID uuid) ;
    virtual void        Remove          (UUIDs & Uuids) ;
    virtual void        Refresh         (QListWidgetItem * item);

  protected slots:

    virtual bool        Menu            (QPoint pos) ;

    virtual void        renameFinished  (void) ;
    virtual void        editingFinished (void) ;

  private slots:

  signals:

    DockSignals ;

} ;

class Q_DATAWIDGETS_EXPORT SourceVersions : public TreeWidget
{
  Q_OBJECT
  public:

    SUID Source ;

    explicit SourceVersions    (StandardConstructor) ;
    virtual ~SourceVersions    (void) ;

    virtual QSize SuitableSize (void) ;

  protected:

    virtual void Configure     (void) ;

  private:

  public slots:

    virtual bool startup       (void) ;

  protected slots:

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Version               (SUID uuid);
    void Version               (SUID uuid,QString name);

} ;

class Q_DATAWIDGETS_EXPORT VersionView : public TreeWidget
{
  Q_OBJECT
  public:

    SUID Version ;

    explicit VersionView       (StandardConstructor) ;
    virtual ~VersionView       (void) ;

    virtual QSize SuitableSize (void) ;

  protected:

    virtual void Configure     (void) ;

  private:

  public slots:

    virtual bool startup       (void) ;

  protected slots:

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void FileUUID              (SUID file,QString name,QString version) ;

} ;

class Q_DATAWIDGETS_EXPORT ProjectsView : public SnapView
                                        , public Object
{
  Q_OBJECT
  public:

    explicit ProjectsView               (StandardConstructor) ;
    virtual ~ProjectsView               (void);

  protected:

    bool dropAction ;
    int  objectType ;
    int  assignType ;

    virtual bool        FocusIn         (void);

    virtual void        Configure       (void);

    virtual QMimeData * dragMime        (void);

    virtual bool        acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType        (const QMimeData * mime);
    virtual UUIDs       MimeUuids       (const QMimeData * mime,QString mimetype) ;

    virtual bool        dropImage       (nDeclDrops,const QImage      & image) ;
    virtual bool        dropPictures    (nDeclDrops,const UUIDs       & Uuids) ;

    bool                setGroupIcon    (SUID uuid,const QImage & image) ;

    virtual void        run             (int Type,ThreadData * data) ;
    virtual bool        Load            (ThreadData * data) ;

  private:

  public slots:

    virtual bool        startup         (void);

    virtual void        New             (void) ;
    virtual void        New             (int Type) ;
    virtual void        Delete          (void) ;
    virtual void        Rename          (void) ;

    virtual void        Rename          (QListWidgetItem * item) ;
    virtual void        Remove          (SUID uuid) ;
    virtual void        Remove          (UUIDs & Uuids) ;
    virtual void        Refresh         (QListWidgetItem * item);

    virtual void        SqlPlan         (void) ;

  protected slots:

    virtual bool        Menu            (QPoint pos) ;

    virtual void        renameFinished  (void) ;
    virtual void        editingFinished (void) ;
    virtual void        itemFinished    (void) ;
    virtual void        sqlFinished     (void) ;

    virtual void singleClicked          (QListWidgetItem * item) ;
    virtual void doubleClicked          (QListWidgetItem * item) ;

  private slots:

  signals:

    void Project    (QWidget * widget,QString name,SUID uuid,int type) ;

} ;

class Q_DATAWIDGETS_EXPORT ProjectItems : public IconDock
                                        , public GroupItems
{
  Q_OBJECT
  public:

    QIcon Icon  ;
    SUID  Group ;
    int   Type  ;

    explicit            ProjectItems    (StandardConstructor) ;
    virtual            ~ProjectItems    (void);

    QListWidgetItem *   uuidAt          (SUID uuid) ;

  protected:

    bool            dropAction ;
    QPoint          dragPoint  ;
    ListWidgetItems Items      ;

    virtual bool        FocusIn         (void);

    virtual void        Configure       (void);

    virtual bool        hasItem         (void);
    virtual bool        startDrag       (QMouseEvent * event);
    virtual bool        fetchDrag       (QMouseEvent * event);
    virtual QMimeData * dragMime        (void);
    virtual void        dragDone        (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag      (QMouseEvent * event);

    virtual bool        acceptDrop      (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew         (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend      (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        removeDrop      (void);

    virtual QString     MimeType        (const QMimeData * mime);
    virtual UUIDs       MimeUuids       (const QMimeData * mime,QString mimetype) ;

    virtual QString     ToolTipTotoal   (int total) ;
    virtual int         Counting        (SqlConnection & Connection,SUID uuid) ;
    virtual UUIDs       GroupUuids      (SqlConnection & Connection) ;
    QIcon               GetIcon         (SqlConnection & Connection,SUID uuid) ;
    virtual SUID        AppendGroup     (SqlConnection & Connection,QString name) ;

    virtual bool        dropText        (nDeclDrops,const QString & text) ;
    virtual bool        dropUrls        (nDeclDrops,const QList<QUrl> & urls) ;
    virtual bool        dropImage       (nDeclDrops,const QImage & image) ;
    virtual bool        dropHtml        (nDeclDrops,const QString & html) ;
    virtual bool        dropColor       (nDeclDrops,const QColor & color) ;
    virtual bool        dropTags        (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropPictures    (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropPeople      (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropVideos      (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropAlbums      (nDeclDrops,const UUIDs & Uuids) ;
    virtual bool        dropGender      (nDeclDrops,const SUID gender) ;
    virtual bool        dropDivisions   (nDeclDrops,const UUIDs & Uuids) ;

    virtual bool        setGroupIcon    (SUID uuid,const QImage & image) ;
    virtual bool        setGroupIcon    (SUID uuid,SUID picture) ;

  private:

  public slots:

    virtual bool        Shutdown        (void);
    virtual bool        Relocation      (void);

    virtual void        New             (void) ;
    virtual void        Rename          (void) ;
    virtual void        Delete          (void) ;
    virtual void        Rename          (QListWidgetItem * item) ;
    virtual void        Remove          (SUID uuid) ;
    virtual void        Remove          (UUIDs & Uuids) ;
    virtual void        Refresh         (QListWidgetItem * item);

    virtual bool        startup         (void);

    virtual bool        JoinText        (SUID uuid,const QString & text) ;
    virtual bool        JoinUrls        (SUID uuid,const QList<QUrl> & urls) ;
    virtual bool        JoinImage       (SUID uuid,const QImage & image) ;
    virtual bool        JoinHtml        (SUID uuid,const QString & html) ;
    virtual bool        JoinColor       (SUID uuid,const QColor & color) ;
    virtual bool        JoinTags        (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinPictures    (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinPeople      (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinVideos      (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinAlbums      (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinGender      (SUID uuid,const SUID gender) ;
    virtual bool        JoinGroups      (SUID uuid,const UUIDs & Uuids) ;

    virtual void        GroupPositions  (RMAPs & Position,UUIDs & Uuids) ;
    virtual bool        MoveGroups      (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinGallery     (SUID uuid,const UUIDs & Uuids) ;
    virtual bool        JoinGallery     (SUID uuid,const QImage & image) ;

  protected slots:

    virtual void        renameFinished  (void) ;
    virtual void        editingFinished (void) ;

    bool                Menu            (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT PdlExtensions : public TreeWidget
                                         , public CodeManager
{
  Q_OBJECT
  public:

    explicit PdlExtensions          (StandardConstructor) ;
    virtual ~PdlExtensions          (void) ;

  protected:

    UUIDs PDLs     ;
    RMAPs PIDs     ;
    NAMEs PdlNames ;
    NAMEs IdNames  ;
    NAMEs EXTs     ;

    virtual void Configure          (void) ;

    virtual bool FocusIn            (void) ;
    virtual bool FocusOut           (void) ;

  private:

  public slots:

    virtual bool startup            (void) ;
    virtual void New                (void) ;

  protected slots:

    virtual bool Menu               (QPoint pos) ;
    virtual void doubleClicked      (QTreeWidgetItem * item,int column) ;

    virtual void languageChanged    (int index) ;
    virtual void extensionChanged   (int index) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT PdlSites : public TreeWidget
                                    , public CodeManager
{
  Q_OBJECT
  public:

    explicit PdlSites            (StandardConstructor) ;
    virtual ~PdlSites            (void) ;

  protected:

    UUIDs PDLs     ;
    NAMEs PdlNames ;

    virtual void Configure       (void) ;
    virtual bool FocusIn         (void) ;
    virtual bool FocusOut        (void) ;

    bool Attach                  (SqlConnection & SC,SUID pdl,SUID url) ;
    bool Detach                  (SqlConnection & SC,SUID pdl,SUID url) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void New             (void) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;
    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    virtual void languageChanged (int index) ;
    virtual void nameChanged     (void) ;

  private slots:

  signals:

    void Go                      (QUrl url,bool reuse) ;

} ;

class Q_DATAWIDGETS_EXPORT PlatformEditor : public TreeWidget
                                          , public CodeManager
{
  Q_OBJECT
  public:

    explicit PlatformEditor    (StandardConstructor) ;
    virtual ~PlatformEditor    (void) ;

  protected:

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;
    virtual bool FocusOut      (void) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void New           (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT CppComponentLists : public Widget
                                             , public CodeManager
{
  Q_OBJECT
  public:

    Cpp::CppObjects What ;

    explicit CppComponentLists   (StandardConstructor) ;
    virtual ~CppComponentLists   (void) ;

  protected:

    Ui::nCppComponentLists * ui;

    virtual void resizeEvent     (QResizeEvent * event) ;
    virtual void showEvent       (QShowEvent   * event) ;
    virtual void closeEvent      (QCloseEvent  * event) ;
    virtual void focusInEvent    (QFocusEvent  * event) ;
    virtual void focusOutEvent   (QFocusEvent  * event) ;

    virtual void Configure       (void) ;

    virtual bool FocusIn         (void) ;
    virtual bool FocusOut        (void) ;

    void ListSource              (SqlConnection & SC) ;
    void ListVersion             (SqlConnection & SC,SUID sourceUuid ) ;
    void ListItems               (SqlConnection & SC,SUID versionUuid) ;

  private:

    QString Table                (void) ;
    void    setToolTips          (void) ;

  public slots:

    virtual bool startup         (void) ;
    virtual void Relocation      (void) ;
    virtual void Refresh         (void) ;
    virtual void CopyToClipboard (void) ;

  protected slots:

    void SourceChanged           (int index) ;
    void VersionChanged          (int index) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT CodingLists : public TreeWidget
                                       , public CodeManager
{
  Q_OBJECT
  public:

    explicit CodingLists           (StandardConstructor) ;
    virtual ~CodingLists           (void) ;

  protected:

    QPoint dragPoint ;

    virtual bool FocusIn           (void) ;
    virtual void Configure         (void) ;

    virtual QString MimeType       (const QMimeData * mime) ;
    virtual UUIDs   MimeUuids      (const QMimeData * mime,QString mimetype) ;

    virtual bool acceptDrop        (QWidget * source,const QMimeData * mime);

    virtual bool dropUrls          (nDeclDrops,const QList<QUrl> & urls ) ;
    virtual bool dropURIs          (nDeclDrops,const UUIDs       & Uuids) ;

  private:

  public slots:

    virtual bool List              (void) ;

    virtual void New               (void) ;
    virtual void Paste             (void) ;
    virtual void Export            (void) ;
    virtual void Copy              (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;

    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    void removeOldItem             (void) ;
    void nameFinished              (void) ;
    bool Ending                    (QTreeWidgetItem * item) ;

  private slots:

  signals:

    void Bookmark                  (SUID uuid,int Type,QString name) ;

} ;

class Q_DATAWIDGETS_EXPORT QtBuilder : public TextBrowser
                                     , public AttachDock
{
  Q_OBJECT
  public:

    QString Program   ;
    QString Directory ;

    explicit QtBuilder                   (StandardConstructor) ;
    virtual ~QtBuilder                   (void) ;

  protected:

    bool         ReportError ;
    bool         Ooops       ;
    QProcess     Process     ;
    QTextCodec * Codec       ;

    virtual void Configure               (void) ;

    virtual void closeEvent              (QCloseEvent       * event) ;
    virtual void contextMenuEvent        (QContextMenuEvent * event) ;

  private:

    void         OrganizePathes          (void) ;
    QString      Soliton                 (QString S) ;

  public slots:

    virtual void Docking                 (QMainWindow * Main,QString title,Qt::DockWidgetArea area,Qt::DockWidgetAreas areas) ;
    virtual void DockIn                  (bool shown) ;

    virtual void run                     (QString program,QStringList arguments,QString directory) ;
    virtual void command                 (QString sh) ;
    virtual void Parse                   (QString stdIn) ;

  protected slots:

    void         Visible                 (bool visible) ;
    virtual bool Menu                    (QPoint pos) ;

    void         error                   (QProcess::ProcessError error) ;
    void         finished                (int exitCode,QProcess::ExitStatus exitStatus) ;
    void         readyReadStandardError  (void) ;
    void         readyReadStandardOutput (void) ;
    void         started                 (void) ;
    void         stateChanged            (QProcess::ProcessState newState) ;

  private slots:

  signals:

    DockSignals                                 ;
    void Started                         (void) ;
    void Finished                        (void) ;
    void Result                          (QString stdIn) ;

} ;

class Q_DATAWIDGETS_EXPORT QtGenerator
{
  public:

    Plan * plan ;

    explicit QtGenerator (Plan * plan = NULL) ;
    virtual ~QtGenerator (void) ;

    QFileInfoList Belong (QFileInfoList & List,QStringList Suffix) ;

    void     CreatePri   (QWidget * parent) ;

  protected:

  private:

} ;

/*****************************************************************************
 *                                                                           *
 *                              File GUI enhancements                        *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT VirtualFiles : public TreeDock
                                        , public VirtualIO
{
  Q_OBJECT
  public:

    explicit VirtualFiles          (StandardConstructor) ;
    virtual ~VirtualFiles          (void);

  protected:

    QPoint                          dragPoint      ;
    bool                            dropAction     ;
    bool                            Unpacking      ;
    qint64                          TotalSize      ;
    int                             TotalFiles     ;
    int                             UnpackMode     ;
    int                             ProgressID     ;
    qint64                          CurrentSize    ;
    QStringList                     ImageFilters   ;
    QStringList                     CppFilters     ;
    QStringList                     GeneralFilters ;
    QFileInfo                       SourceFI       ;
    QMap<QString,QTreeWidgetItem *> DirItems       ;

    virtual bool        FocusIn    (void) ;
    virtual void        Configure  (void) ;

    virtual bool        hasItem    (void) ;
    virtual bool        startDrag  (QMouseEvent * event) ;
    virtual bool        fetchDrag  (QMouseEvent * event) ;
    virtual QMimeData * dragMime   (void) ;
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag (QMouseEvent * event) ;

    virtual bool        acceptDrop (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew    (QWidget * source,const QMimeData * mime,QPoint pos) ;

    virtual void        run        (void) ;

    virtual void resetSize         (void) ;
    virtual void appendSize        (int size) ;
    virtual void appendFile        (FileInfo & newFile) ;

    void ListFile                  (FileInfo & newFile) ;
    void UnpackItem                (FileInfo & newFile) ;

  private:

    QTreeWidgetItem * setDirectory (FileInfo & newFile) ;
    QTreeWidgetItem * getDirectory (FileInfo & newFile) ;
    QTreeWidgetItem * getEntry     (FileInfo & newFile) ;

  public slots:

    virtual bool startup           (QFileInfo & FileInformation) ;
    virtual void Save              (void) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void Arrangement       (void) ;
    virtual void statusMessage     (QString msg) ;

  private slots:

  signals:

    void Arrange                   (void) ;
    void Unmessage                 (QString msg) ;

} ;

class Q_DATAWIDGETS_EXPORT FileView : public SnapView
{
  Q_OBJECT
  public:

    explicit FileView                 (StandardConstructor) ;
    virtual ~FileView                 (void);

  protected:

    virtual void        Configure     (void);

    virtual QMimeData * dragMime      (void);

    virtual bool        dropNew       (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropMoving    (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropAppend    (QWidget * source,const QMimeData * mime,QPoint pos);

    virtual QString     MimeType      (const QMimeData * mime);
    virtual UUIDs       MimeUuids     (const QMimeData * mime,QString mimetype) ;

    virtual QStringList       Filters (void) = 0;
    virtual QListWidgetItem * Load    (QFileInfo & info) = 0 ;

  private:

  public slots:

    virtual void startup              (QDir directory) ;

  protected slots:

    void singleClicked                (QListWidgetItem * item) ;
    void doubleClicked                (QListWidgetItem * item) ;

  private slots:

  signals:

    void Clicked                      (QFileInfo info) ;

} ;

class Q_DATAWIDGETS_EXPORT PictureBrowser : public FileView
{
  Q_OBJECT
  public:

    explicit PictureBrowser           (StandardConstructor) ;
    virtual ~PictureBrowser           (void);

    QStringList selectedFiles         (void) ;

  protected:

    virtual QStringList       Filters (void);
    virtual QListWidgetItem * Load    (QFileInfo & info);

  private:

  public slots:

    void Import                       (void) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT PickPicture : public PictureBrowser
{
  Q_OBJECT
  public:

    QDir Root  ;
    int  Index ;

    explicit PickPicture           (StandardConstructor) ;
    virtual ~PickPicture           (void) ;

    virtual QSize sizeHint         (void) const ;

    bool          CurrentPath      (QString & path) ;

  protected:

    QFileSystemWatcher * watcher ;

    virtual void resizeEvent       (QResizeEvent * event) ;
    virtual void showEvent         (QShowEvent   * event) ;

    virtual void Configure         (void) ;

    virtual QListWidgetItem * Load (QFileInfo & info);

  private:

  public slots:

    virtual void relocation        (void) ;

    virtual void startup           (QDir directory) ;
    virtual void Refresh           (void) ;
    virtual void setRoot           (QString path) ;

    virtual void Previous          (void) ;
    virtual void Next              (void) ;

  protected slots:

    virtual void imageChanged      (const QString & path) ;
    virtual void singleClicked     (QListWidgetItem * item) ;

  private slots:

  signals:

    void Full                      (int index,bool full) ;
    void Empty                     (void) ;
    void FileSelected              (QString path) ;

} ;

class Q_DATAWIDGETS_EXPORT ArchiveList : public TreeWidget
{
  Q_OBJECT
  public:

    explicit ArchiveList       (StandardConstructor) ;
    virtual ~ArchiveList       (void) ;

    QDir &      Path           (void) ;
    int         Selections     (void) ;
    QStringList PickFiles      (void) ;

  protected:

    QDir Root ;

    virtual void Configure     (void) ;

  private:

  public slots:

    virtual void setRoot       (QString path) ;
    virtual void Delete        (void) ;
    virtual void Append        (QStringList files,bool bypass = false) ;

  protected slots:

    virtual void singleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void selectionChanged      (void) ;

} ;

class Q_DATAWIDGETS_EXPORT ArchivePick : public TreeWidget
{
  Q_OBJECT
  public:

    explicit ArchivePick       (StandardConstructor) ;
    virtual ~ArchivePick       (void) ;

    QDir &      Path           (void) ;
    QDir &      CurrentPath    (void) ;
    int         Selections     (void) ;
    QStringList PickFiles      (void) ;

  protected:

    QDir Root    ;
    QDir Current ;

    virtual void Configure     (void) ;

  private:

  public slots:

    virtual void setRoot       (QString path) ;
    virtual void List          (void) ;
    virtual void CdUp          (void) ;
    virtual void Directory     (QString path) ;

  protected slots:

    virtual void singleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Listing               (void) ;
    void Ready                 (void) ;
    void selectionChanged      (void) ;
    void Folders               (bool topmost,QStringList folders) ;

} ;

class Q_DATAWIDGETS_EXPORT PadBrowser : public Splitter
{
  Q_OBJECT
  public:

    QDir        Root      ;
    QDir        Current   ;
    QStringList TopLevels ;
    QIcon       Join      ;

    explicit PadBrowser    (StandardConstructor) ;
    virtual ~PadBrowser    (void) ;

    virtual QSize sizeHint (void) const ;

  protected:

    Ui::PadBrowser   *  ui       ;
    QMap<int,QAction *> Actions  ;
    QMap<int,BMAPs    > Displays ;
    int                 Flags    ;

    virtual void Configure (void) ;

  private:

    bool doFile            (void) ;
    bool doDirectory       (void) ;
    bool doPick            (void) ;
    void addAction         (int Id,QString text,QIcon icon) ;
    void setAlignments     (QTreeWidgetItem * item) ;
    void DeletePath        (QDir & root,QStringList files) ;

  public slots:

    void setFlags          (int flags) ;
    void setBack           (QString text) ;
    void setAnswer         (bool enable) ;
    void Display           (int conf) ;

    void List              (void) ;
    void Okay              (void) ;
    void Up                (void) ;
    void Cd                (void) ;
    void New               (void) ;
    void NewDirectory      (void) ;
    void Delete            (void) ;
    void Rename            (void) ;
    void RenameFinished    (void) ;
    void Edit              (void) ;

  protected slots:

    void itemClicked       (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Back              (void) ;
    void Listing           (void) ;
    void Ready             (void) ;
    void selectionChanged  (void) ;
    void Folders           (bool topmost,QStringList folders) ;
    void SelectFiles       (QStringList files) ;

} ;

class Q_DATAWIDGETS_EXPORT WatchFiles : public QObject
                                      , public Thread
{
  Q_OBJECT
  Q_PROPERTY(bool Deletion READ canDelete WRITE setDeletion DESIGNABLE false)
  public:

    QFileSystemWatcher * watcher ;
    Plan               * plan    ;

    explicit WatchFiles           (QObject * parent = NULL,Plan * plan = NULL) ;
    virtual ~WatchFiles           (void) ;

    bool     canDelete            (void) const ;

  protected:

    QString     Scope       ;
    bool        Watching    ;
    int         Delay       ;
    bool        Deletion    ;
    QDateTime   LastChanged ;
    QStringList Pathes      ;
    QMutex      FileMutex   ;

    virtual void run              (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual void startup          (void) ;
    virtual void Stop             (void) ;
    virtual void Watch            (void) ;
    virtual void setDeletion      (bool deletion) ;
    virtual void Deal             (QStringList & files) ;

  protected slots:

    virtual void directoryChanged (const QString & path) ;
    virtual void fileChanged      (const QString & path) ;

    virtual void sendStart        (void) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT WatchPictures : public WatchFiles
                                         , public PictureManager
{
  Q_OBJECT
  public:

    explicit WatchPictures (QObject * parent = NULL,Plan * plan = NULL) ;
    virtual ~WatchPictures (void) ;

  protected:

    QString trayTitle   ;
    QString trayMessage ;

    virtual void run       (int Type,ThreadData * data) ;

  private:

  public slots:

    virtual void setScope  (QString scope) ;
    virtual void Reload    (QString scope) ;
    virtual void Deal      (QStringList & files) ;

  protected slots:

    virtual void sendStart (void) ;
    virtual void sendTray  (void) ;

  private slots:

  signals:

    void showTray          (void) ;
    void showMessage       (QString title,QString message) ;

} ;

class Q_DATAWIDGETS_EXPORT DirectoryEditor : public TreeWidget
                                           , public Group
{
  Q_OBJECT
  public:

    explicit DirectoryEditor (StandardConstructor) ;
    virtual ~DirectoryEditor (void) ;

  protected:

    QString     scope       ;
    QStringList Directories ;

    virtual void Configure   (void) ;
    virtual bool FocusIn     (void) ;

    virtual bool acceptDrop  (QWidget * source,const QMimeData * mime) ;
    virtual bool dropNew     (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool dropMoving  (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool dropAppend  (QWidget * source,const QMimeData * mime,QPoint pos) ;
    virtual bool dropUrls    (nDeclDrops,const QList<QUrl> & urls) ;

    virtual void run         (int Type,ThreadData * data) ;
    virtual bool Load        (ThreadData * data) ;
    virtual bool Saving      (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup     (void) ;
    virtual void Reload      (void) ;
    virtual void setScope    (QString scope) ;

    virtual void Insert      (void) ;
    virtual void Delete      (void) ;
    virtual void Paste       (void) ;
    virtual void Store       (void) ;

    virtual void GoUp        (void) ;
    virtual void GoDown      (void) ;

    virtual bool AppendUrls  (const QList<QUrl> & urls) ;

    virtual void setModified (bool enabled) ;

  protected slots:

    virtual bool Menu        (QPoint pos) ;

    virtual void SendReload  (void) ;

  private slots:

  signals:

    void EmitReload          (void) ;
    void Reload              (QString scope) ;

} ;

/*****************************************************************************
 *                                                                           *
 *                               Database widgets                            *
 *                                                                           *
 *****************************************************************************/

class Q_DATAWIDGETS_EXPORT SqlTableItems : public TreeWidget
{
  Q_OBJECT
  public:

    explicit SqlTableItems (StandardConstructor) ;
    virtual ~SqlTableItems (void) ;

    QSize SuitableSize     (void) ;

  protected:

    virtual void Configure (void);

  private:

  public slots:

    bool load              (QString database,QString table) ;

  protected slots:

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT DatabaseDock : public TreeDock
{
  Q_OBJECT
  public:

    enum IconIds     {
      Database = 0   ,
      Table    = 1   ,
      View     = 2   ,
      Unknown  = 3   ,
      Error    = 4 } ;

    ICONs   Icons    ;
    QString Username ;
    QString Password ;
    Sql     sql      ;

    explicit DatabaseDock  (StandardConstructor) ;
    virtual ~DatabaseDock  (void);

  protected:

    virtual void Configure (void);

    virtual bool FocusIn   (void) ;

    virtual void run       (int type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup   (void) ;
    virtual bool Shutdown  (void) ;
    virtual void List      (void) ;
    void         Copy      (void) ;
    virtual void Organize  (SUID uuid) ;
    void         Optimize  (QString database) ;
    void         Optimize  (QString database,QString table) ;

  protected slots:

    bool Menu              (QPoint pos) ;
    void ImportSequence    (QTreeWidgetItem * item);
    void Export            (QTreeWidgetItem * item);

  private slots:

  signals:

    void ModifyTable       (QString database,QString table);
    void TableItems        (QString database,QString table);
    void ParseTable        (QString database,QString table,QString creation);
    void RunSets           (VarArgLists & Operations) ;

} ;

class Q_DATAWIDGETS_EXPORT DatabaseLinks : public TreeDock
{
  Q_OBJECT
  public:

    explicit DatabaseLinks     (StandardConstructor) ;
    virtual ~DatabaseLinks     (void);

  protected:

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;

    virtual void run           (int type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual bool Shutdown      (void) ;
    virtual void List          (void) ;
    virtual void New           (void) ;
    virtual void Apply         (SUID uuid,Sql * sql) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void NewConnection         (QWidget * widget,SUID uuid) ;
    void Organize              (SUID uuid) ;
    void Monitor               (SUID uuid) ;

} ;

class Q_DATAWIDGETS_EXPORT SqlServers : public TreeDock
{
  Q_OBJECT
  public:

    explicit SqlServers        (StandardConstructor) ;
    virtual ~SqlServers        (void);

  protected:

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;
    virtual bool FocusOut      (void) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void New           (void) ;

  protected slots:

    virtual void removeOldItem (void) ;
    virtual void nameFinished  (void) ;
    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SqlItems : public TreeDock
                                    , public Object
{
  Q_OBJECT
  public:

    explicit SqlItems          (StandardConstructor) ;
    virtual ~SqlItems          (void);

  protected:

    QStringList Items      ;
    QPoint      dragPoint  ;
    bool        dropAction ;

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;
    virtual bool FocusOut      (void) ;

    virtual UUIDs GetUuids     (SqlConnection & Connection) ;

    QString DataType           (SqlConnection & Connection,int Type) ;
    int     DataType           (SqlConnection & Connection,QString name) ;
    QTreeWidgetItem * GetItem  (SqlConnection & Connection,SUID uuid) ;
    SUID    GetSqlItem         (SqlConnection & Connection,SqlParser & Parser,int index) ;
    void    addParserItem      (SqlConnection & Connection,SqlParser & Parser,int index) ;

    virtual bool        hasItem        (void) ;
    virtual bool        startDrag      (QMouseEvent * event) ;
    virtual bool        fetchDrag      (QMouseEvent * event) ;
    virtual QMimeData * dragMime       (void) ;
    virtual void        dragDone       (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag     (QMouseEvent * event) ;

    virtual bool        acceptDrop     (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew        (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropSqlColumn  (nDeclDrops,const UUIDs & Uuids) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void New           (void) ;
    virtual void Delete        (void) ;
    virtual void DeleteAll     (void) ;
    virtual void addTable      (SqlConnection & Connection,SqlParser & Parser) ;

  protected slots:

    virtual void removeOldItem (void) ;
    virtual void nameFinished  (void) ;
    virtual void typeChanged   (int index) ;
    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Changed               (SUID Group,int Type,SUID item,int How) ;

} ;

class Q_DATAWIDGETS_EXPORT SqlRelations : public TreeDock
                                        , public Object
{
  Q_OBJECT
  public:

    explicit SqlRelations      (StandardConstructor) ;
    virtual ~SqlRelations      (void) ;

  protected:

    QStringList Items ;

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;
    virtual bool FocusOut      (void) ;

    virtual UUIDs GetUuids     (SqlConnection & Connection) ;
    SUID          GetSqlItem   (SqlConnection & SC,SqlParser & Parser,int i) ;
    QTreeWidgetItem * GetItem  (SqlConnection & Connection,SUID uuid) ;
    void addParserItem         (SqlConnection & Connection,SqlParser & Parser,int index) ;

    QString addReverseQuote    (QString item) ;
    QString toReverseQuotes    (QString item) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void New           (void) ;
    virtual void Delete        (void) ;
    virtual void DeleteAll     (void) ;
    virtual void addTable      (SqlConnection & Connection,SqlParser & Parser) ;

  protected slots:

    virtual void removeOldItem (void) ;
    virtual void nameFinished  (void) ;
    virtual void typeChanged   (int index) ;
    virtual bool Menu          (QPoint pos) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Changed               (SUID Group,int Type,SUID item,int How) ;

} ;

class Q_DATAWIDGETS_EXPORT SqlTables : public TreeDock
                                     , public Object
{
  Q_OBJECT
  public:

    explicit SqlTables                 (StandardConstructor) ;
    virtual ~SqlTables                 (void);

    bool                contains       (QString database,QString table) ;

  protected:

    QPoint dragPoint  ;
    bool   dropAction ;

    virtual void        Configure      (void) ;
    virtual bool        FocusIn        (void) ;
    virtual bool        FocusOut       (void) ;

    virtual bool        hasItem        (void) ;
    virtual bool        startDrag      (QMouseEvent * event) ;
    virtual bool        fetchDrag      (QMouseEvent * event) ;
    virtual QMimeData * dragMime       (void) ;
    virtual void        dragDone       (Qt::DropAction dropIt,QMimeData * mime) ;
    virtual bool        finishDrag     (QMouseEvent * event) ;

    virtual bool        acceptDrop     (QWidget * source,const QMimeData * mime);
    virtual bool        dropNew        (QWidget * source,const QMimeData * mime,QPoint pos);
    virtual bool        dropSqlTable   (nDeclDrops,const UUIDs & Uuids) ;

    virtual UUIDs       GetUuids       (SqlConnection & Connection) ;
    QTreeWidgetItem *   GetItem        (SqlConnection & Connection,SUID uuid) ;

    bool                assureDatabase (SqlConnection & Connection,SUID uuid,QString name) ;
    bool                assureTable    (SqlConnection & Connection,SUID uuid,QString name) ;
    bool                assureEnum     (SqlConnection & Connection,SUID uuid,QString name) ;
    bool                assureID       (SqlConnection & Connection,SUID uuid,QString name) ;

  private:

  public slots:

    virtual bool        startup        (void) ;
    virtual void        New            (void) ;
    virtual void        Rename         (void) ;
    virtual void        ExportCpp      (void) ;
    virtual void        Ordering       (void) ;
    virtual void        addTable       (QString database,QString table) ;

  protected slots:

    virtual void        removeOldItem  (void) ;
    virtual void        nameFinished   (void) ;
    virtual bool        Menu           (QPoint pos) ;
    virtual void        Rename         (QTreeWidgetItem * item) ;
    virtual void        doubleClicked  (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Edit                          (SUID uuid) ;

} ;

class Q_DATAWIDGETS_EXPORT SqlStatistics : public Widget
{
  Q_OBJECT
  public:

    explicit SqlStatistics     (StandardConstructor) ;
    virtual ~SqlStatistics     (void);

  protected:

    Ui::nSqlStatistics * ui            ;
    QString              DefaultScheme ;

    virtual void Configure     (void) ;

    virtual bool FocusIn       (void) ;
    virtual bool FocusOut      (void) ;

    virtual void focusInEvent  (QFocusEvent  * event) ;
    virtual void focusOutEvent (QFocusEvent  * event) ;
    virtual void resizeEvent   (QResizeEvent * event) ;
    virtual void showEvent     (QShowEvent   * event) ;

    void         CountSql      (SqlConnection & SC,QString Scheme) ;

    virtual void run           (void) ;

  private:

  public slots:

    virtual void Relocation    (void) ;
    virtual bool startup       (void) ;
    virtual void Copy          (void) ;

  protected slots:

    virtual void schemeChanged (int index) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SqlEditor : public TabWidget
                                     , public Object
{
  Q_OBJECT
  public:

    explicit SqlEditor  (StandardConstructor) ;
    virtual ~SqlEditor  (void) ;

  protected:

    Ui::nSqlEditor * ui               ;
    VcfEditor      * Editor           ;
    SqlTables      * Tables           ;
    SqlItems       * TableColumns     ;
    SqlRelations   * TableConstraints ;
    SqlItems       * Columns          ;
    SqlRelations   * Constraints      ;
    Sql              Source           ;
    SqlParsers       Parsers          ;

    void resizeEvent    (QResizeEvent * event) ;
    void showEvent      (QShowEvent * event) ;

    void Configure      (void) ;

  private:

  public slots:

    bool startup        (void) ;
    bool Relocation     (void) ;
    void Analyze        (void) ;
    void Creations      (void) ;
    void EditTable      (SUID uuid) ;

  protected slots:

    void SourceChanged  (int index) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SqlQuery : public TableWidget
{
  Q_OBJECT
  public:

    explicit SqlQuery      (StandardConstructor) ;
    virtual ~SqlQuery      (void) ;

  protected:

    bool    isQuery ;
    QString Query   ;
    int     Limit   ;

    virtual bool FocusIn   (void) ;
    virtual void Configure (void) ;

    virtual void run       (void) ;

    void showColumns       (QSqlRecord & record) ;
    void showValues        (int index,QSqlRecord & record,QSqlQuery & query) ;

  private:

  public slots:

    virtual bool startup   (QString query) ;
    virtual void Refresh   (void) ;

  protected slots:

    virtual bool Menu      (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SqlMonitor : public TreeDock
{
  Q_OBJECT
  public:

    explicit SqlMonitor      (StandardConstructor) ;
    virtual ~SqlMonitor      (void) ;

  protected:

    bool       Monitoring ;
    bool       autoFit    ;
    int        Interval   ;
    QList<Sql> SQLs       ;

    virtual bool FocusIn     (void) ;
    virtual void Configure   (void) ;
    virtual void run         (int type,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup     (void) ;
    virtual bool Shutdown    (void) ;

    virtual void Dispatch    (void) ;
    virtual void Monitor     (int id) ;
    virtual void Append      (SUID uuid) ;

  protected slots:

    virtual void TimeChanged (int ms) ;
    virtual bool Menu        (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SettingsEditor : public TableWidget
{
  Q_OBJECT
  public:

    explicit SettingsEditor  (StandardConstructor) ;
    virtual ~SettingsEditor  (void) ;

  protected:

    virtual void Configure   (void) ;
    virtual void run         (void) ;

  private:

  public slots:

    virtual bool Shutdown    (void);

  protected slots:

    virtual bool Menu        (QPoint pos) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT SMAPsEditor : public TreeWidget
{
  Q_OBJECT
  public:

    SMAPs * maps ;

    explicit     SMAPsEditor   (StandardConstructor) ;
    virtual     ~SMAPsEditor   (void) ;

  protected:

    virtual void Configure     (void) ;

    virtual bool FocusIn       (void) ;

  private:

  public slots:

    virtual void prepare       (void) ;
    virtual void Insert        (void) ;
    virtual void Delete        (void) ;

  protected slots:

    virtual void singleClicked (QTreeWidgetItem *item,int column) ;
    virtual void doubleClicked (QTreeWidgetItem *item,int column) ;

    virtual void keyPressed    (void) ;
    virtual void valuePressed  (void) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT CloneFiles : public TreeDock
{
  Q_OBJECT
  public:

    explicit     CloneFiles    (StandardConstructor) ;
    virtual     ~CloneFiles    (void) ;

  protected:

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

    virtual void run           (int Type,ThreadData * data) ;

  private:

    void         KeepPath      (QString path) ;
    QString      GetPath       (void) ;
    QTreeWidgetItem * findItem (QString source,QString target) ;

    void         CreateDirs    (QDir          & source  ,
                                QDir          & target  ,
                                QFileInfoList & infos ) ;
    void         Copy          (QDir          & source  ,
                                QDir          & target  ,
                                QFileInfoList & infos ) ;

  public slots:

    virtual void Load          (QString filename) ;
    virtual void Load          (void) ;
    virtual void Save          (void) ;
    virtual void Insert        (void) ;
    virtual void Delete        (void) ;
    virtual void StartAll      (void) ;
    virtual void RunAll        (void) ;
    virtual void Start         (void) ;
    virtual void Stop          (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;

    virtual void singleClicked (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

    virtual void BrowseSource  (void) ;
    virtual void BrowseTarget  (void) ;

    virtual void Copy          (VarArgs & args) ;

  private slots:

  signals:

} ;

class Q_DATAWIDGETS_EXPORT IdentifierEditor : public TreeDock
                                            , public Ownership
{
  Q_OBJECT
  public:

    explicit     IdentifierEditor  (StandardConstructor) ;
    virtual     ~IdentifierEditor  (void) ;

  protected:

    virtual void Configure         (void) ;

    virtual bool FocusIn           (void) ;

    virtual void run               (int Type,ThreadData * data) ;

    virtual void ModifyIdentifiers (ThreadData * data,VarArgs & args) ;

  private:

  public slots:

    virtual void startByThread     (enum QThread::Priority priority = QThread::NormalPriority) ;
    virtual bool startup           (void) ;

    virtual void FetchIdentifiers  (ThreadData * data) ;
    virtual void DeleteIdentifiers (ThreadData * data) ;

    virtual void Insert            (void) ;
    virtual void Delete            (void) ;
    virtual void Export            (void) ;

  protected slots:

    virtual void singleClicked     (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

    virtual void nameFinished      (void) ;

    virtual bool Menu              (QPoint pos) ;

  private slots:

  signals:

} ;

}

QT_END_NAMESPACE

#endif
