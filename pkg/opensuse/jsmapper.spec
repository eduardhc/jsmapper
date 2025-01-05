# norootforbuild

Name:           jsmapper
Version:        1.0.0
Release:        1%{?dist}
Summary:        Joystick programming tool for Linux
License:        GPL-2.0
Url:            http://www.assembla.com/spaces/jsmapper
Source:         %{name}-%{version}.tar.gz
Group:          Hardware/Joystick

BuildRequires:	%kernel_module_package_buildreqs 
BuildRequires:  gcc-c++,cmake,gcc-c++ 
BuildRequires:  libxml2-devel,ncurses-devel,
BuildRequires:  fdupes 

Requires:       jsmapper-data
Requires:       jsmapper-kmp = %{version}
BuildRoot:      %{_tmppath}/%{name}-%{version}-build


%kernel_module_package
%debug_package


%description
JSMapper is a Linux joystick programming tool designed for users who want to 
take full profit of their advanced game devices (joysticks, HOTAS, etc...) on 
Linux. It allows the user to map keys (and key sequences) to any button and 
axis, while also supporting an arbitrary number of modes & shift states based 
on button states & axis positions (bands).

%prep
%setup -q -n %{name}-%{version}

# prepare kernel part
# set -- src/linux/drivers/input/* 
# mkdir source
# cp -a "$@" source/ 
mkdir obj 


%clean
rm -rf %{buildroot}


%build
# build kernel part:
for flavor in %flavors_to_build; do 
        rm -rf obj/$flavor 
        mkdir obj/$flavor 
        cp -a src/linux/drivers/input/*  obj/$flavor 
        make -C %{kernel_source $flavor} modules M=$PWD/obj/$flavor 
done 

# build userland part:
mkdir build 
pushd build
cmake \
    -DCMAKE_BUILD_TYPE=RelWithDebugInfo \
	-DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIB_INSTALL_DIR=%{_libdir} \
%ifarch x86_64
	-LIB_SUFFIX=64 \
%endif
    -DWITH_KMOD=OFF \
    ../src

%{__make} %{?jobs:-j%jobs}
popd


%install
%fdupes -s %buildroot

# install kernel part
export INSTALL_MOD_PATH=$RPM_BUILD_ROOT 
export INSTALL_MOD_DIR=updates 
for flavor in %flavors_to_build; do 
	make -C %{kernel_source $flavor} modules_install M=$PWD/obj/$flavor 
done

# install userland part
pushd build
%makeinstall
popd


%check
pushd build
make test
popd


%files
%defattr(-,root,root,-)
%doc README
%{_bindir}/jsmapper-ctrl
%{_bindir}/jsmapper-device
%{_libdir}/libjsmapper.so.*


%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


#
# subpackage: data 
#

%package data
Summary:        Common data files for JSMapper
BuildArch:      noarch

%description data
This package contains various device map files for JSMapper

%files data
%defattr(-,root,root)
%{_datadir}/jsmapper



#
# subpackage: devel
#

%package devel
Summary:        Development files for JSMapper
Group:          Development/Libraries/C and C++  
Requires:       %{name} = %{version}-%{release}

%description devel
This package contains development files for JSMapper

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/libjsmapper.so


#
# changelog
#

%changelog
* Sun May 12 2013 Eduard Huguet <eduardhc@gmail.com> - 1.0.0
- Initial release
