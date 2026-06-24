# Antivirus Demo

The application demonstrates the operation of the API with which an antivirus application can interact. In Aurora OS there is av-launcher service, which
is launched after system boot on behalf of the current user with the minimum required set of permissions, has read and write access to the current user's files, 
as well as access to some system files, user's files for reading and writing, as well as access to some system files.  
To make it work, the following should be added to the install section of the .spec file:  
```
mkdir -p %{buildroot}%{_sysconfdir}/av-launcher
ln -s %{_datadir}/applications/%{name}.desktop \
    %{buildroot}%{_sysconfdir}/av-launcher/antivirus.desktop
%files
%{_sysconfdir}/av-launcher/antivirus.desktop
```  
The service has the following features: Access to standard user directories, additional audit events with libomplog(AV_SERVICE_STARTED,
AV_SCAN_STARTED, ...). LogSecurityEvents permission is specified to write event data to the desktop file. Access to the 
security module die-hard.  
The application log is written to a separate file. No log will be created on the emulator.

## Important

To install the application, its RPM package must be signed with the **antivirus** profile key.

## Usage

After launching the application, you will need to launch the service. To do this, connect to the device or emulator and start the service:
```
$ /usr/bin/en.auroraos.AntivirusDemo --service
```
The event log will be written through the service application. Files with rpm extension will be considered as threat files.  

## Terms of Use and Participation

The source code of the project is provided under [the license](LICENSE.BSD-3-Clause.md),
that allows it to be used in third-party applications.

The [contributor agreement](CONTRIBUTING.md) documents the rights granted by contributors
to the Open Mobile Platform.

For information about contributors see [AUTHORS](AUTHORS.md).

[Code of conduct](CODE_OF_CONDUCT.md) is a current set of rules of the Open Mobile
Platform which informs you how we expect the members of the community will interact
while contributing and communicating.

## Project Structure

The project has a common structure
of an application based on C++ and QML for Aurora OS.

* **[CMakeLists.txt](CMakeLists.txt)** file describes the project structure for the CMake build system.
* **[icons](icons)** directory contains application icons for different screen resolutions.
* **[qml](qml)** directory contains the QML source code and the UI resources.
  * **[assets](qml/assets)** directory contains the custom UI icons.
  * **[cover](qml/cover)** directory contains the application cover implementations.
  * **[icons](qml/icons)** directory contains the custom UI icons.
  * **[pages](qml/pages)** directory contains the application pages.
  * **[utils](qml/utils)** directory contains additional functions.
  * **[AntivirusDemo.qml](qml/AntivirusDemo.qml)** file provides the application window implementation.
* **[rpm](rpm)** directory contains the rpm-package build settings.  
  * **[ru.auroraos.AntivirusDemo.spec](rpm/ru.auroraos.AntivirusDemo.spec)** file is used by rpmbuild tool.
* **[src](src)** directory contains the C++ source code.
  * **[av](src/av)** directory contains interfacing with the antivirus API.
  * **[logger](src/logger)** directory contains an application event logger.
  * **[main.cpp](src/main.cpp)** file is the application entry point.
* **[translations](translations)** directory contains the UI translation files.
* **[ru.auroraos.AntivirusDemo.desktop](ru.auroraos.AntivirusDemo.desktop)** file defines the display and parameters for launching the application.

## Screenshots

![screenshots](screenshots/screenshots.png)

## This document in Russian / Перевод этого документа на русский язык

- [README.ru.md](README.ru.md)
