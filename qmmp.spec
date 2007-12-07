%if %{_target_vendor} == fedora
%define fedora_build 1
%else
%define fedora_build 0
%endif


Name:		qmmp
Version:	0.1.5
%if %{fedora_build}
Release:	1%{?dist}
%else
Release:        %mkrel 1
%endif
License:	GPL
Summary:	Qt-based multimedia player
Group:		Multimedia
Packager:	Eugene A. Pivnev <ti DOT eugene AT gmail DOT com>
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root
Source:		http://www.ylsoftware.com/files/%{name}-%{version}.tar.bz2
BuildRequires:	gcc-c++ libmad-devel 
BuildRequires:  ffmpeg-devel >= 0.4.9-pre1 libmpcdec-devel >= 1.2.2 libvorbis-devel libogg-devel 
BuildRequires:  libsamplerate-devel alsa-lib-devel taglib-devel qt4-devel >= 4.2 desktop-file-utils
%if %{fedora_build}
Requires:       taglib
Requires:	flac >= 1.1.3 libmad jack-audio-connection-kit >= 0.102.5 
Requires:       ffmpeg >= 0.4.9-pre1 libmpcdec >= 1.2.2 libvorbis libogg libsamplerate alsa-lib qt4 >= 4.2
BuildRequires:  flac-devel >= 1.1.3 jack-audio-connection-kit-devel >= 0.102.5
%else
Requires:       libtaglib0
Requires:	libalsa2 libffmpeg51
Requires:	libflac8 libjack0 libmad0 liblame0
Requires:	libmpcdec5 libogg0 libqtcore4 libqtgui4
Requires:	libqtnetwork4 libqtxml4 libsamplerate0
BuildRequires:  libflac-devel >= 1.1.3 libjack0-devel >= 0.102.5
%endif 

%description
This program is an audio-player, written with help of Qt library.
The user interface is similar to winamp or xmms.
Main opportunities:

    * unpacked winamp skins support;
    * plugins support;
    * MPEG1 layer 1/2/3 support;
    * Ogg Vorbis support;
    * Native FLAC support;
    * Musepack support;
    * WMA support;
    * AlSA sound output;
    * JACK sound output.

%prep
%setup -q

%build
%{_libdir}/qt4/bin/qmake LIB_DIR="/%{_lib}" "QMAKE_CFLAGS=%optflags" "QMAKE_CXXFLAGS=%optflags"
make

%install
%{__rm} -rf %{buildroot}
%{makeinstall} INSTALL_ROOT=/%{buildroot}/usr

%clean
%{__rm} -rf %{buildroot}

%post
 
%postun

%files
%doc AUTHORS COPYING ChangeLog ChangeLog.rus ChangeLog.svn README README.RUS
%defattr(-,root,root,-)
%{_bindir}/qmmp
%{_libdir}/qmmp
%{_libdir}/libqmmp.*
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.xpm


%changelog
* Fri Dec 7 2007 Ilya Kotov <forkotov02@newmail.ru> 0.1.5-1
- 0.1.5
- mandriva support

* Mon Sep 10 2007 Eugene A. Pivnev <ti.eugene@gmail.com> 0.1.4-1
- 0.1.4

* Tue Sep 4 2007 Karel Volny <kvolny@redhat.com> 0.1.3.1-2
- patched for multilib Fedora setup
- added .desktop entry and icon
- fixed BuildRequires

* Wed Aug 1 2007 Eugene Pivnev <ti.eugene@gmail.com> 1.1.9-1
- Initial release for Fedora 7
