#!/bin/sh

cd ../../qmmp-plugin-pack-0.10
find -name '*.ts' -print0 | xargs --null cp --parents -t ..//qmmp-plugin-pack-1.1 -v

