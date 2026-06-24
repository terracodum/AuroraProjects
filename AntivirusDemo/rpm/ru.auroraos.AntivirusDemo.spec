Name:       ru.auroraos.AntivirusDemo
Summary:    Antivirus API Demo
Version:    0.1
Release:    1
License:    BSD-3-Clause
URL:        https://developer.auroraos.ru/open-source
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(auroraapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(libomplog)
BuildRequires:  pkgconfig(nemonotifications-qt5)

%description
%{summary}

%prep
%autosetup

%build
%cmake -GNinja
%ninja_build

%install
%ninja_install

mkdir -p %{buildroot}%{_sysconfdir}/av-launcher
ln -s %{_datadir}/applications/%{name}.desktop \
    %{buildroot}%{_sysconfdir}/av-launcher/antivirus.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_sysconfdir}/av-launcher/antivirus.desktop
