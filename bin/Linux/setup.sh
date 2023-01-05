#!/bin/sh

echo "::group::configure kinetic"
sudo add-apt-repository "deb http://mirror.math.princeton.edu/pub/ubuntu kinetic main universe"
echo "::endgroup::"

echo "::group::install dependencies"
sudo apt-get -qqqq update
sudo apt-get -qqqq install libboost-all-dev libwxgtk3.2-dev libsdl2-dev libgtk-3-dev
echo "::endgroup::"
