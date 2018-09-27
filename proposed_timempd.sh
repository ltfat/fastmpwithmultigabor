#!/bin/bash
./setup.sh

SNR=60
MAXIT=200000

dictstr=blackman,512,2048
#dictstr=blackman,512,2048:blackman,2048,8192


for FILE in 39ch1.wav
do
    bname=(`basename ${FILE%%.*}`)
    ./libltfat/examples/multigabormp/multigabormpd \
        -i $FILE --maxit=$MAXIT -s $SNR -o ${bname}_rec.wav \
        -d $dictstr
done

