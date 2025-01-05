#!/bin/bash 

#######################################################################################################################
#
# Tarball release script for JSMapper
#
# Versions:
#		- 1.0		Eduard Huguet		Initial version
#
#######################################################################################################################

# abort on any error
set -e

# find project folder from script path:
pushd `dirname $0` > /dev/null
SCRIPT_DIR=`pwd`
popd > /dev/null

JSMAPPER_DIR=${SCRIPT_DIR}/..

# get version string:
VERSION=`cat ${SCRIPT_DIR}/VERSION`

# create temporary dir using project name & version:
TMP_DIR=/tmp/jsmapper-${VERSION}
mkdir $TMP_DIR

# copy tarball contents to temporary folder:
TARBALL_CONTENTS="${JSMAPPER_DIR}/src \
${JSMAPPER_DIR}/CONTENTS \
${JSMAPPER_DIR}/COPYING \
${JSMAPPER_DIR}/INSTALL \
${JSMAPPER_DIR}/README"

for CONTENT in $TARBALL_CONTENTS
do
	cp -rv $CONTENT $TMP_DIR/
done

# cleanup backup files, etc... 
find $TMP_DIR/ -name '*~' -delete
find $TMP_DIR/ -name '*.user' -delete
find $TMP_DIR/ -name '*.kdev4' -delete


# switch to tmp folder & compress tarball folder from there (so it will get extracted in 
# proper subdirectory):
pushd /tmp > /dev/null
tar cvzf jsmapper-${VERSION}.tar.gz jsmapper-${VERSION}/
popd > /dev/null

# get tarball file back & cleanup temporary dir
mv /tmp/jsmapper-${VERSION}.tar.gz .
rm -rf $TMP_DIR

