#!/bin/sh

set -e

if [ -e "$1/CMakeCache.txt" ] ; then
    builddir="$1"
else
    mkdir -p tmp
    builddir=$(mktemp -d tmp/tmp.XXXXXXXXXX)
fi
cd $builddir

# TODO: fix version number
if [ "$GITHUB_ENV" != "" ] ; then
    echo "git_tag=2.0.0-$(date +%Y%m%d.%H%M%S)" >>$GITHUB_ENV
fi

echo "::group::cmake"
cmake ../..
echo "::endgroup::"

echo "::group::cmake --build"
cmake --build .
echo "::endgroup::"

if [ "$1" = "-n" ] ; then
	exit 0
fi

echo "::group::cpack"
cpack -B ../cpack/
echo "::endgroup::"
