#!/bin/bash
./setup.sh

export MPTK_CONFIG_FILENAME=./mptk/mptk/path.xml
SNR=60
MAXIT=200000
g=blackman

for FILE in 39ch1.wav
do
    bname=(`basename ${FILE%%.*}`)

    for SHIFT in 4 8
    do
        truncate -s 0 mptk5_${bname}_${SHIFT}.dat
        dictstr=''
        echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
        for M in 512 1024 2048 4096 8192
        do
            a=$(( $M/$SHIFT )) 
            dictstr+=blackman,$a,$M:
            cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' | sed 's/%g/'"${g}"'/g' >> tmpdict.xml
        done
        echo '</dict>' >> tmpdict.xml
        echo $dictstr >> mptk5_${bname}_${SHIFT}.dat
        ./mptk/bin/mpd --nIter=${MAXIT} -d tmpdict.xml -s $SNR $FILE ${bname}.bin ${bname}_rec.wav  > tmpfile.dat 
        sed 's/ /'"${M}"' /' tmpfile.dat >> mptk5_${bname}_${SHIFT}.dat
    done


    for SHIFT in 4 8 16 32
    do
        truncate -s 0 mptk_${bname}_${SHIFT}.dat
        for M in 512 1024 2048 4096 8192
        do
            echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
            a=$(( $M/$SHIFT )) 
            cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' | sed 's/%g/'"${g}"'/g' >> tmpdict.xml
            echo '</dict>' >> tmpdict.xml
            cat tmpdict.xml
            dictstr=blackman,$a,$M
            echo $dictstr >> proposed_${bname}_${SHIFT}.dat
            ./mptk/bin/mpd --nIter=${MAXIT} -d tmpdict.xml -s $SNR $FILE ${bname}.bin ${bname}_rec.wav  > tmpfile.dat 
            sed 's/ /'"${M}"' /' tmpfile.dat >> mptk_${bname}_${SHIFT}.dat
            echo '' >>  mptk_${bname}_${SHIFT}.dat
        done
    done
done

