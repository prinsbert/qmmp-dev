

Name:		qmmp
Summary: 	QT4-based multimeadia player for Linux
Version: 	0.1.4
Release: 	%mkrel 1
URL:		http://qmmp.ylsoftware.com/index_en.html
Source:     	%name-%version.tar.bz2
Source1:	qmmp-16.png
Source2:	qmmp-32.png
Source3:	qmmp-48.png
License:  	GPL
Group: 		Graphical desktop/KDE
BuildRoot: 	%_tmppath/%name-%version-%release-root
Requires:	libalsa2 libffmpeg51 libfaac0 libfaad2_0
Requires:	libflac8 libjack0 libmad0 liblame0
Requires:	libmpcdec5 libogg0 libqtcore4 libqtgui4
Requires:	libqtnetwork4 libqtxml4 libsamplerate0

%description
Qmmp (Qt-based Multimedia Player) is an audio-player
written with help of Qt library.


%package devel
Summary:	qmmp development files
Group:		Development/KDE and Qt

%description devel 
Development files for qmmp


%prep
%setup -q

%build
/usr/lib/qt4/bin/qmake "QMAKE_CFLAGS=%optflags" "QMAKE_CXXFLAGS=%optflags"
make

%install
rm -rf %{buildroot}
%makeinstall INSTALL_ROOT=/%{buildroot}/usr

%clean
if [ -d $RPM_BUILD_ROOT ]; then rm -r $RPM_BUILD_ROOT; fi

%post
/sbin/ldconfig

%postun
/sbin/ldconfig


%files
%defattr(0755,root,root,0755)
%{_bindir}/qmmp
%defattr(0644,root,root,0755)
%dir %{_libdir}/qmmp/Input
%dir %{_libdir}/qmmp/Output
%{_libdir}/libqmmp.so.*
%{_libdir}/qmmp/Input/*.so
%{_libdir}/qmmp/Output/*.so
%{_datadir}/pixmaps/%{name}.xpm
%{_datadir}/applications/%{name}.desktop

%files devel
%defattr(-,root,root)
%_libdir/libqmmp.so

