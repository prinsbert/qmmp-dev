#!/bin/sh

cd ../../../trunk/qmmp-plugin-pack
find -name '*.ts' -print0 | xargs --null cp --parents -t ../../branches/qmmp-plugin-pack-1.0 -v

