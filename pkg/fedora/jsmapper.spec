Name:           jsmapper
Version:        1.1.0
Release:        1%{?dist}
Summary:        Joystick programming tool for Linux
License:        GPLv2        
URL:            http://jsmapper.org
Source:         %{name}-%{version}.tar.gz
Group:          Applications/System
BuildRequires:  cmake,libxml2-devel,ncurses-devel
Requires:       jsmapper-data
Requires:       kmod-jsmapper = %{version}


%description
JSMapper is a Linux joystick programming tool designed for users who want to 
take full profit of their advanced game devices (joysticks, HOTAS, etc...) on 
Linux. It allows the user to map keys (and key sequences) to any button and 
axis, while also supporting an arbitrary number of modes & shift states based 
on button states & axis positions (bands).


%prep
%setup -q


%clean
rm -rf %{buildroot}


%build
%cmake -DWITH_KMOD=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo ./src/
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%check
make test


#
# Main package
#

%files
%doc README
%defattr(-,root,root,-)
%{_bindir}/jsmapper-ctrl
%{_bindir}/jsmapper-device
%{_libdir}/libjsmapper.so.*

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


#
# Sub-package: -data
#

%package data
Summary:        JSMapper common data files
BuildArch:      noarch

%description data
This package contains various device map files for JSMapper

%files data
%{_datarootdir}/%{name}/devices/*


#
# Sub-package: -devel
#

%package devel
Summary:        JSMapper development files
Requires:       %{name} = %{version}-%{release}

%description devel
This package contains development files for JSMapper. Includes kernel module 
API and wrapper library header files. 

%files devel
%{_includedir}/jsmapper/*
%{_libdir}/libjsmapper.so


#
# Sub-package -chooser-kde 
#

%package chooser-kde
Summary:	JSMapper profile chooser for KDE
Requires:	%{name} = %{version}-%{release}
BuildRequires:  kdelibs-devel

%description chooser-kde
This package contains the JSMapper desktop profile switcher for KDE.

%files chooser-kde
%defattr(-,root,root,-)
%{_bindir}/jsmapper-chooser-kde
%{_datadir}/kde4/apps/jsmapper-chooser-kde/*


%changelog

* Sun Nov 25 2013 Eduard Huguet <eduardhc@gmail.com> - 1.1.0
- New release, featuring a profile switcher for KDE and various bugfixings.

* Sun May 12 2013 Eduard Huguet <eduardhc@gmail.com> - 1.0.0
- Initial release
