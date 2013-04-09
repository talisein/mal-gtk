mal-gtk
=======

GTK+3 Interface for myanimelist.net

Arch package here: https://aur.archlinux.org/packages/mal-gtk-git/

Requirements
============
gtkmm3, libsecret, libxml2, libcurl, and a org.freedesktop.secrets
provider such as gnome-keyring-daemon or ksecretservice.

Building
========

autoreconf --force --install
./configure
make
sudo make install

mal-gtk is the executable name.