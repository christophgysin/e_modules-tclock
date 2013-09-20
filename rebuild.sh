#!/bin/bash
# git clean -fxd

d=$(readlink -f $(dirname $0))

sh autogen.sh
make
make install DESTDIR=${d}/dest
cp -r ${d}/dest/usr/lib/enlightenment/modules/tclock ~/.e/e/modules/
enlightenment_remote -restart
