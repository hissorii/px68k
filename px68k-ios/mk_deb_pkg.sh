#!/bin/sh
# This scrpit makes .deb package and other related files for px68k

export LANG=C

if [ ! -e ~/Desktop/px68k-ios.app ]; then
echo 'First of all, put ~/Library/Developer/Xcode/Archives/20XX-XX-XX/px68k-ios 20XX-XX-XX XX:XX.xarchive/Products/Applications/px68k-ios.app to the Desktop'
exit
fi

which ldid > /dev/null
if [ "$?" -eq 1 ]; then
echo 'You need ldid command'
exit
fi

# Add the SHA1 hashes to the app binary
ldid ~/Desktop/px68k-ios.app/px68k-ios


cd ~

# backup the old directory if exists
mkdir -p tmp/apt
if [ -e tmp/apt/px68k-ios ]; then
	mv tmp/apt/px68k-ios tmp/apt/px68k-ios_`date "+%Y-%m-%d_%H-%M"`
fi

cd tmp/apt

mkdir -p px68k-ios/var/mobile/px68k

cd px68k-ios

mkdir DEBIAN

VER=`cat ~/src/px68k/version.txt | sed 's/PX68K_VERSION=//'`

# make DEBIAN/control file
cat <<EOF > DEBIAN/control
Name: px68k
Package: com.fc2.blog45.hissorii.px68k
Author: hissorii
Maintainer: hissorii <hissory@gmail.com>
Section: Games
Version: ${VER}
Architecture: iphoneos-arm
Description: Portable X68000 Emulator
Depiction: http://hissorii.blog45.fc2.com
Homepage: http://hissorii.blog45.fc2.com

EOF

mkdir Applications
cp -pr ~/Desktop/px68k-ios.app Applications/

cd ~/tmp/apt

dpkg-deb -b px68k-ios

## Modify below if you have other package(s).
# workaround for "dpkg-scanpackages ignore Name: field"
grep Name: px68k-ios/DEBIAN/control > Packages
dpkg-scanpackages . /dev/null >> Packages
gzip Packages

# make Release file
cat <<EOF > Release
Origin: hissorii
Label: hissorii
Suite: stable
Version: 1.0
Codename: stable
Architectures: iphoneos-arm
Components: main
Description: hissorii's repository
EOF

gpg -abs -o Release.gpg Release
