#!/bin/sh
set -e
set -u

export LC_ALL=C.UTF-8

cp -rv devscripts "$AUTOPKGTEST_TMP"
cp -rv test "$AUTOPKGTEST_TMP"
cp -rv yt_dlp "$AUTOPKGTEST_TMP"

cd "$AUTOPKGTEST_TMP"

for python in $(py3versions --supported);do
   printf "###\\n### Now testing %s\\n###\\n" "$python"
   "$python" -m pytest -k "not download"
done
