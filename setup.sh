#/bin/bash

# Install fftw3, sndfile, pthreads

# Setup and compile mptk
(cd mptk && cmake .  && make -j8)

# Compile multigabormp  
make -C libltfat/examples/multigabormp
