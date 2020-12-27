#!/bin/sh
if [ -z $MESON_INSTALL_PREFIX ]; then
    echo 'This is meant to be ran from Meson only!'
    exit 1
fi

if [ -z $DESTDIR ]; then
    echo 'Updating desktop database'
    update-desktop-database -q "$MESON_INSTALL_PREFIX/share/applications"
fi
