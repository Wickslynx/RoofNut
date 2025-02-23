#!/bin/bash

# Check if correct number of arguments are provided
if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <file_location> <app_name> <logo_path>"
  exit 1
fi

FILE_LOCATION=$1
APP_NAME=$2
LOGO_PATH=$3

mkdir -p myapp/DEBIAN
mkdir -p myapp/usr/local/bin
mkdir -p myapp/usr/share/applications
mkdir -p myapp/usr/share/pixmaps


cp "$FILE_LOCATION" "myapp/usr/local/bin/$APP_NAME"
chmod +x "myapp/usr/local/bin/$APP_NAME"


cp "$LOGO_PATH" "myapp/usr/share/pixmaps/$APP_NAME.png"


cat <<EOL > myapp/DEBIAN/control
Package: $APP_NAME
Version: 1.0
Section: base
Priority: optional
Architecture: amd64
Depends: libc6 (>= 2.29)
Maintainer: Your Name <youremail@example.com>
Description: $APP_NAME packaged as a DEB
EOL

cat <<EOL > myapp/usr/share/applications/$APP_NAME.desktop
[Desktop Entry]
Name=$APP_NAME
Exec=/usr/local/bin/$APP_NAME
Icon=/usr/share/pixmaps/$APP_NAME.png
Type=Application
Categories=Utility;
EOL

dpkg-deb --build myapp

mv myapp.deb "$APP_NAME.deb"
rm -rf myapp

echo "$APP_NAME.deb package created successfully."
