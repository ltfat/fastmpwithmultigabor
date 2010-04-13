#!/bin/sh
echo "moving files..."
cp -a ./bin/ /usr/local/bin/;
cp -a ./include/ /usr/local/include/;
cp -a ./lib/ /usr/local/lib;
cp -a ./mptk/ /usr/local/mptk/;
echo "updating dynamic link libraries..."
echo "purging temp files..."
rm -rf ../MPTK-0.5.7-Linux
echo "MPTK has extracted itself."
