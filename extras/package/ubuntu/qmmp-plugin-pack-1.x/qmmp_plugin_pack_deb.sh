#!/bin/sh

PLUGIN_PACK_VERSION=1.4.0
UBUNTU_CODENAMES='xenial bionic focal groovy'
BUILD_ROOT=build-root


prepare ()
{
    cp ../qmmp-plugin-pack-$PLUGIN_PACK_VERSION.tar.bz2 ./
    mv ./qmmp-plugin-pack-$PLUGIN_PACK_VERSION.tar.bz2 ./qmmp-plugin-pack_$PLUGIN_PACK_VERSION.orig.tar.bz2
}

build ()
{
    echo '****'$1'****'
    mkdir $1
    cd $1
    tar xvjf ../../qmmp-plugin-pack-$PLUGIN_PACK_VERSION.tar.bz2
    mkdir qmmp-plugin-pack-$PLUGIN_PACK_VERSION/debian
    cp -rv ../../debian-$1/* -t qmmp-plugin-pack-$PLUGIN_PACK_VERSION/debian/
    cp ../qmmp-plugin-pack_$PLUGIN_PACK_VERSION.orig.tar.bz2 ./
    cd qmmp-plugin-pack-$PLUGIN_PACK_VERSION
    if [ "$1" = "xenial" ] ; then
        debuild -S -sa -d -kF594F6B4
    else
        debuild -S -sd -d -kF594F6B4
    fi
    cd ..
    cd ..
}

update ()
{
    dch -m --newversion ${PLUGIN_PACK_VERSION}-1ubuntu1~${1}0 -D ${1} -c debian-$1/changelog "New upstream release."
}

upload ()
{
	dput ppa:forkotov02/ppa $1/*.changes
}

clean ()
{
    rm -rf $BUILD_ROOT
    rm qmmp-plugin-pack-$PLUGIN_PACK_VERSION.tar.bz2
}

case $1 in
    --download)
		wget http://qmmp.ylsoftware.com/files/plugins/qmmp-plugin-pack-$PLUGIN_PACK_VERSION.tar.bz2
    ;;
    --update)
		for CODENAME in $UBUNTU_CODENAMES
		do
			update $CODENAME
		done
    ;;
    --build)
		rm -rf $BUILD_ROOT
		mkdir $BUILD_ROOT
		cd $BUILD_ROOT
		prepare
		for CODENAME in $UBUNTU_CODENAMES
		do
			build $CODENAME
		done
    ;;
    --upload)
       cd $BUILD_ROOT
       for CODENAME in $UBUNTU_CODENAMES
		do
			upload $CODENAME
		done
    ;;
    --clean)
		clean
    ;;
    *)
		echo "Commands:"
		echo "--download"
		echo "--update"
		echo "--build"
		echo "--upload"
		echo "--clean"
esac
