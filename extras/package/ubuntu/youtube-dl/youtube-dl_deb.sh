#!/bin/sh

VERSION=2021.01.16
UBUNTU_CODENAMES='xenial bionic focal groovy'
BUILD_ROOT=build-root


prepare ()
{
    cp ../youtube-dl-$VERSION.tar.gz ./
    mv ./youtube-dl-$VERSION.tar.gz ./youtube-dl_$VERSION.orig.tar.gz
}

build ()
{
    echo '****'$1'****'
    mkdir $1
    cd $1
    tar xvzf ../../youtube-dl-$VERSION.tar.gz
    mkdir youtube-dl/debian
    cp -rv ../../debian-$1/* -t youtube-dl/debian/
    cp ../youtube-dl_$VERSION.orig.tar.gz ./
    cd youtube-dl
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
    dch -m --newversion ${VERSION}-1ubuntu1~${1}0 -D ${1} -c debian-$1/changelog "New upstream release."
}

upload ()
{
	dput ppa:forkotov02/ppa $1/*.changes
}

clean ()
{
    rm -rf $BUILD_ROOT
    rm youtube-dl-$VERSION.tar.gz
}

case $1 in
    --download)
		wget https://youtube-dl.org/downloads/latest/youtube-dl-${VERSION}.tar.gz
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
