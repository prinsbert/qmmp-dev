SET PKG_CONFIG_PATH=C:\devel\mingw64-libs\lib\pkgconfig;C:\devel\qmmp-win64\lib\pkgconfig;;C:\devel\qt6-win64\lib\pkgconfig
SET NO_COLOR=1
cmake . -G "MSYS Makefiles" -GNinja -DUSE_LIBRCD=ON ^
-DCMAKE_REQUIRED_INCLUDES=C:\devel\mingw64-libs\include ^
-DCMAKE_SYSTEM_LIBRARY_PATH=C:\devel\mingw64-libs\lib ^
-DCMAKE_INSTALL_PREFIX=C:\devel\qmmp-win64
