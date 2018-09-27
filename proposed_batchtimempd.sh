#!/bin/bash
./setup.sh

SNR=60
MAXIT=200000

for FILE in 39ch1.wav
do
    bname=(`basename ${FILE%%.*}`)

    for SHIFT in 4 8
    do
        truncate -s 0 proposed5_${bname}_${SHIFT}.dat
        dictstr=''
        for M in 512 1024 2048 4096 8192
        do
            a=$(( $M/$SHIFT ))
            dictstr+=blackman,$a,$M:
        done
        echo $dictstr >> proposed5_${bname}_${SHIFT}.dat
        ./libltfat/examples/multigabormp/multigabormpd \
        -i $FILE --maxit=$MAXIT -s $SNR -o ${bname}_rec.wav \
        -d $dictstr  \
        >> proposed5_${bname}_${SHIFT}.dat
    done

    for SHIFT in 4 8 16 32
    do
        truncate -s 0 proposed_${bname}_${SHIFT}.dat
        for M in 512 1024 2048 4096 8192 16384
        do
            a=$(( $M/$SHIFT )) 
            dictstr=blackman,$a,$M
            echo $dictstr >> proposed_${bname}_${SHIFT}.dat
            ./libltfat/examples/multigabormp/multigabormpd \
            -i $FILE --maxit=$MAXIT -s $SNR -o ${bname}_rec.wav \
            -d $dictstr \
            >> proposed_${bname}_${SHIFT}.dat
            echo '' >>  proposed_${bname}_${SHIFT}.dat
        done
    done
done

