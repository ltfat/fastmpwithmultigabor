#/bin/bash

# Install fftw3, sndfile, pthreads

# Setup and compile mptk
if [ ! -f ./mptk/bin/mpd ]; then
    (cd mptk && cmake .  && make -j8)
else
    echo "MPTK mpd utility found!"
fi

# Compile multigabormp  
if [ ! -f ./libltfat/examples/multigabormp/multigabormpd ]; then
    make -C libltfat/examples/multigabormp
else
    echo "libltfat multigabormpd utility found!"
fi
