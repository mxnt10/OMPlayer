#!/bin/bash

pkgver='1.2'
install_root=${install_root:-""}

set -e
mkdir -p bin
cd bin
cmake ..
make
cd ..

[ "$install_root" != "" ] && {
  mkdir -p "$install_root"/usr/{bin,share/{applications,pixmaps,OMPlayer/{icons,qss,logo}},doc/OMPlayer-"$pkgver"}
} || {
  mkdir -p /usr/{share/OMPlayer/{icons/qss,logo},doc/OMPlayer-"$pkgver"}
}

install -Dm 0755 bin/OMPlayer "$install_root"/usr/bin
install -Dm 0644 appdata/OMPlayer.png "$install_root"/usr/share/pixmaps
install -Dm 0644 appdata/logo.png "$install_root"/usr/share/OMPlayer/logo
install -Dm 0644 appdata/OMPlayer.desktop "$install_root"/usr/share/applications

cp -a ChangeLog LICENSE README.md "$install_root"/usr/doc/OMPlayer-"$pkgver"
cp -Tr icons "$install_root"/usr/share/OMPlayer/icons
cp -Tr qss "$install_root"/usr/share/OMPlayer/qss
exit 0
