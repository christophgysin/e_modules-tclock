#!/bin/bash
set -e

./autogen.sh
make
make install DESTDIR=$PWD/dest
cp -r dest/usr/lib/enlightenment/modules/tclock ~/.e/e/modules/
enlightenment_remote -restart
