#!/bin/bash
./setup.sh

export MPTK_CONFIG_FILENAME=./mptk/mptk/path.xml
SNR=60
MAXIT=200000

dictstr=blackman,512,2048
a=512
M=2048

for FILE in 39ch1.wav
do
    bname=(`basename ${FILE%%.*}`)

    echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
    for k in 1
    do
        cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' >> tmpdict.xml
    done
    echo '</dict>' >> tmpdict.xml

    ./mptk/bin/mpd --nIter=${MAXIT} -d tmpdict.xml -s $SNR $FILE ${bname}.bin ${bname}_rec.wav -q
done

