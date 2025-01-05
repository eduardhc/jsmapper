# (un)define the next line to either build for the newest or all current kernels
#define buildforkernels newest
#define buildforkernels current
%define buildforkernels akmod

# name should have a -kmod suffix
Name:           jsmapper-kmod
Version:        1.1.0
Release:        1%{?dist}.1
Summary:        Kernel module(s)
Group:          System Environment/Kernel
License:        GPLv2
URL:            http://jsmapper.org
Source:         jsmapper-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  %{_bindir}/kmodtool


# needed for plague to make sure it builds for i586 and i686
ExclusiveArch:  i586 i686 x86_64 ppc ppc64

# get the proper build-sysbuild package from the repo, which
# tracks in all the kernel-devel packages
BuildRequires:  %{_bindir}/kmodtool

%{!?kernels:BuildRequires: buildsys-build-rpmfusion-kerneldevpkgs-%{?buildforkernels:%{buildforkernels}}%{!?buildforkernels:current}-%{_target_cpu} }

# kmodtool does its magic here
%{expand:%(kmodtool --target %{_target_cpu} --repo rpmfusion --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null) }


%description
JSMapper is a Linux joystick programming tool designed for users who want to 
take full profit of their advanced game devices (joysticks, HOTAS, etc...) on 
Linux. It allows the user to map keys (and key sequences) to any button and 
axis, while also supporting an arbitrary number of modes & shift states based 
on button states & axis positions (bands).

This package contains the kernel module that provides the input filtering for 
JSMapper.


%prep
# error out if there was something wrong with kmodtool
%{?kmodtool_check}

# print kmodtool output for debugging purposes:
# kmodtool  --target %{_target_cpu}  --repo %{repo} --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null
kmodtool  --target %{_target_cpu}  --repo rpmfusion --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null

%setup -q -c -T -a 0

for kernel_version in %{?kernel_versions} ; do
    cp -a jsmapper-%{version}/src/linux/drivers/input _kmod_build_${kernel_version%%___*}
done


%build
for kernel_version in %{?kernel_versions}; do
    make %{?_smp_mflags} -C "${kernel_version##*___}" SUBDIRS=${PWD}/_kmod_build_${kernel_version%%___*} modules
done


%install
rm -rf ${RPM_BUILD_ROOT}

for kernel_version in %{?kernel_versions}; do
	# make install DESTDIR=${RPM_BUILD_ROOT} KMODPATH=%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}
    install -D -m 755 _kmod_build_${kernel_version%%___*}/jsmapperdev.ko  \
		${RPM_BUILD_ROOT}%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}/jsmapperdev.ko
done
%{?akmod_install}


%clean
rm -rf $RPM_BUILD_ROOT



%package common
Summary:        JSMapper kernel module common data files
BuildArch:      noarch

%description common
This package contains common files for JSMapper kernel module

%files common


%changelog
* Sun Nov 24 2013 Eduard Huguet <eduardhc@gmail.com> - 1.1.0-1
- New version released, including a profile switcher for KDE and various bugfixings.

* Sun May 12 2013 Eduard Huguet <eduardhc@gmail.com> - 1.0.0-1
- Initial release
