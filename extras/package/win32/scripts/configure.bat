SET PKG_CONFIG_PATH=C:\devel\mingw32-libs\lib\pkgconfig;C:\devel\qmmp-win32\lib\pkgconfig;C:\devel\qt6\lib\pkgconfig
cmake . -G "MSYS Makefiles" -GNinja -DUSE_LIBRCD=ON ^
-DCMAKE_REQUIRED_INCLUDES=C:\devel\mingw32-libs\include ^
-DCMAKE_SYSTEM_LIBRARY_PATH=C:\devel\mingw32-libs\lib ^
-DCMAKE_INSTALL_PREFIX=C:\devel\qmmp-win32