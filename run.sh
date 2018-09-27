#!/bin/bash
#
# This script runs the same MP decomposition task using MPTK and libltfat
# In particular, it show the MP decomposition execution time and the 
# achieved SNR.

# Make sure both MPTK's mpd and libltfat's mpd are compiled
./setup.sh

export MPTK_CONFIG_FILENAME=./mptk/mptk/path.xml

# Array of wav files to be decomposed
FILES=39ch1.wav
# Maximum number of iterations
MAXIT=200000
# Stopping SNR
SNR=60
# (Multi-)Gabor dictionary definition in the format: window1,a1,M1:window2,a2,M2...
dictstr=blackman,512,2048
#dictstr=blackman,512,2048:blackman,2048,8192

# Create a temporary dict xml for MPTK
echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
IFS=":"
for dictstrel in $dictstr
do
    g="$(echo $dictstrel | cut -d',' -f1)"
    a="$(echo $dictstrel | cut -d',' -f2)"
    M="$(echo $dictstrel | cut -d',' -f3)"
    cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' | sed 's/%g/'"${g}"'/g' >> tmpdict.xml
done
echo '</dict>' >> tmpdict.xml

IFS=" "
for FILE in $FILES
do
    bname=(`basename ${FILE%%.*}`)


    echo "------------------------Running MPTK's mpd------------------------------"
    ./mptk/bin/mpd -d tmpdict.xml -q \
        --nIter=${MAXIT} -s $SNR $FILE ${bname}.bin ${bname}_res_mptk.wav
    echo "------------------------------------------------------------------------"

    echo "------------------------Running libltfat's mpd--------------------------"
    ./libltfat/examples/multigabormp/multigabormpd \
        -i $FILE --maxit=$MAXIT -s $SNR -o ${bname}_rec_prop.wav -r ${bname}_res_prop.wav \
        -d $dictstr
    echo "------------------------------------------------------------------------"
done

