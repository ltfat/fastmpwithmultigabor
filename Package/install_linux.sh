#!/bin/sh
echo "moving files..."
cp -a ./bin/ /usr/local/;
cp -a ./include/ /usr/local/;
cp -a ./lib/ /usr/local/;
cp -a ./mptk/ /usr/local/;
echo "updating dynamic link libraries..."
ldconfig
echo "purging temp files..."
rm -rf ../MPTK-0.5.7-Linux
echo "MPTK has extracted itself."
