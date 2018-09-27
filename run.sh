#!/bin/bash
./setup.sh

export MPTK_CONFIG_FILENAME=./mptk/mptk/path.xml
SNR=60
MAXIT=200000
export IFS=":"

dictstr=blackman,512,2048
#dictstr=blackman,512,2048:blackman,2048,8192

for FILE in 39ch1.wav
do
    bname=(`basename ${FILE%%.*}`)

    echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
    for dictstrel in $dictstr
    do
        g="$(echo $dictstrel | cut -d',' -f1)"
        a="$(echo $dictstrel | cut -d',' -f2)"
        M="$(echo $dictstrel | cut -d',' -f3)"
        cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' | sed 's/%g/'"${g}"'/g' >> tmpdict.xml
    done
    echo '</dict>' >> tmpdict.xml

    echo 'Running MPTK's mpd'
    ./mptk/bin/mpd -d tmpdict.xml -q \
        --nIter=${MAXIT} -s $SNR $FILE ${bname}.bin ${bname}_rec_mptk.wav

    echo 'Running libltfat's mpd'
    ./libltfat/examples/multigabormp/multigabormpd \
        -i $FILE --maxit=$MAXIT -s $SNR -o ${bname}_rec_prop.wav \
        -d $dictstr
done

