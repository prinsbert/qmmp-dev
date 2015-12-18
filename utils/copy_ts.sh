#!/bin/sh

cd ../../qmmp-plugin-pack-0.9
find -name '*.ts' -print0 | xargs --null cp --parents -t ../../branches/qmmp-plugin-pack-1.0 -v

