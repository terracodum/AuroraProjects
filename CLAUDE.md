# AuroraApplications

Набор приложений под **ОС Аврора** (Aurora OS, форк Sailfish OS). Каждая подпапка — отдельный
самостоятельный проект. Стек: **C++ / Qt5 + QML / Sailfish.Silica**, сборка под Aurora SDK.

## Проекты

| Проект | ID | Сборка | Особенности |
|--------|----|--------|-------------|
| `Notes` | `ru.template.Notes` | CMake | заметки, C++ модель + JSON-хранилище |
| `Converter` | `ru.template.Converter` | CMake | конвертер величин |
| `AntivirusDemo` | `ru.auroraos.AntivirusDemo` | CMake | демо OMP, D-Bus сервис, permissions |
| `PdfViewer` | `ru.auroraos.PdfViewer` | CMake (subdirs) | QML-плагин + pdfium/libjpeg в `3rdparty/` |
| `BleScanner` | `ru.auroraos.BleScanner` | **qmake** (`.pro`) | BLE через SimpleBLE в `libs/` |

Два «происхождения»:
- **`ru.template.*`** — мои учебные проекты из шаблона Qt Creator. Простые, один исполняемый файл.
- **`ru.auroraos.*`** — официальные примеры OMP (BSD-3, `gitlab-ci/`, `AUTHORS.md`, README ru/en).

## Типовая структура (простой проект)

```
ProjectName/
  CMakeLists.txt              # project() = полный ID, напр. ru.template.Notes
  ru.template.Name.desktop    # точка входа, иконка, Permissions, Org/AppName
  src/
    main.cpp                  # Aurora::Application + QQuickView
    <model>.cpp/.h            # C++ модели (опционально)
  qml/
    Name.qml                  # ApplicationWindow: initialPage + cover
    pages/MainPage.qml        # экраны (Page)
    pages/AboutPage.qml
    cover/DefaultCoverPage.qml# обложка приложения (плитка в меню)
  icons/{86x86,108x108,128x128,172x172}/<ID>.png
  rpm/
    <ID>.spec                 # RPM: BuildRequires, %files
    <ID>.changes.in
  translations/*.ts           # переводы (qsTr → ts → qm)
  build/                      # вывод Qt Creator: AuroraOS_5_2_0_180_*-{aarch64,armv7hl}
```

## Ключевые паттерны

**main.cpp** — всегда через Aurora-обёртки:
```cpp
QScopedPointer<QGuiApplication> application(Aurora::Application::application(argc, argv));
application->setOrganizationName(QStringLiteral("ru.template"));
application->setApplicationName(QStringLiteral("Notes"));
QScopedPointer<QQuickView> view(Aurora::Application::createView());
view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/Notes.qml")));
view->show();
return application->exec();
```

**C++ ↔ QML** — модель пробрасывается context property и зовётся в QML по имени:
```cpp
view->rootContext()->setContextProperty(QStringLiteral("notesModel"), &notesModel);
```
Модели наследуют `QAbstractListModel`: `roleNames()` (роли = имена для `model.<role>` в делегате),
методы для QML помечаются `Q_INVOKABLE`, свойства — `Q_PROPERTY`.

**Хранение данных** — JSON в `QStandardPaths::AppDataLocation`, каталог создаётся `QDir().mkpath()`.

**QML/UI** — только Sailfish.Silica:
- корень — `ApplicationWindow { initialPage; cover; allowedOrientations }`
- экраны — `Page` внутри `pageStack` (`push`/`pop`); навигация через `Qt.resolvedUrl(...)`
- размеры/цвета **только** через `Theme.*` (`Theme.paddingMedium`, `Theme.highlightColor`,
  `Theme.fontSizeMedium`, `Theme.horizontalPageMargin`) — не хардкодить px
- списки — `SilicaListView` + `PageHeader` в header, `PullDownMenu`, `ContextMenu`, `ViewPlaceholder`,
  `VerticalScrollDecorator`, удаление через `remorseDelete(...)`
- иконки — `image://theme/icon-*`
- все строки в UI оборачивать в `qsTr(...)`

**.desktop** — `X-Nemo-Application-Type=silica-qt5`, `Permissions=` (напр. `UserDirs`,
`LogSecurityEvents`), `OrganizationName`/`ApplicationName` совпадают с main.cpp.
Сервис: добавляется `X-Service-Exec=... --service`.

## Сборка

Под капотом Aurora Platform SDK (PSDK), команда — `mb2`:
```bash
mb2 --target <PSDK_TARGET> build <projectDir>        # см. BleScanner/build.sh
```
Цели в этом дереве: **AuroraOS 5.2.0.180**, архитектуры **aarch64** и **armv7hl**.
Обычно проекты собираются из Qt Creator с Aurora SDK (папка `build/`). Локально на macOS
полноценно не собрать — нужен SDK/таргет; правки проверяю чтением кода, не запуском.

CMake-проекты: `project()` = полный ID; список QML/исходников ведётся вручную в `set(SOURCES ...)`
(в простых) либо `file(GLOB_RECURSE ...)` (в OMP-примерах). RPM-спека ссылается на этот ID везде.

## Рабочие договорённости

- Отвечаю по-русски, файлы целиком не переписываю без явной просьбы — даю блоки кода для вставки.
- При добавлении страницы: создать `qml/pages/*.qml` **и** дописать в `set(SOURCES)` в CMakeLists.
- Новые C++ модели: завести в `src/`, добавить в `SOURCES`, пробросить через `setContextProperty`.
- Менять ID/Org/App — синхронно в: `CMakeLists.txt`, `.desktop`, `rpm/*.spec`, `main.cpp`, именах иконок.
- Неочевидные решения в коде — задаю наводящий вопрос, не переписываю молча (см. глобальные правила).
