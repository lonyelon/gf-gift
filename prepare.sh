#!/bin/sh

PROJ_DIR=LilyGo-EPD47
CONF_FILE=./src/config.hpp

git submodule deinit -f . >/dev/null 2>/dev/null
git submodule update --init --recursive --checkout >/dev/null 2>/dev/null

# Copy our ini to the other repo.
cp platformio.ini $PROJ_DIR

# Modify the scripts to allow (a) Spanish font conversion and (b) use the new
# antialias, preventing python from being mean to us.
sed -i 's/# (160, 255),/ (160, 255),/' $PROJ_DIR/scripts/fontconvert.py
sed -i 's/Image.ANTIALIAS/Image.LANCZOS/' $PROJ_DIR/scripts/imgconvert.py

[ -f $CONF_FILE ] \
    || printf "Please create a '$CONF_FILE' based on '$CONF_FILE.example'\n" >&2
