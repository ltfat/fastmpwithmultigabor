#!/bin/bash
export MPTK_CONFIG_FILENAME=/home/susnak/dev/mptk/mptk/path.xml
SNR=60
MAXIT=200000

for FILE in 39ch1.wav
do
bname=(`basename ${FILE%%.*}`)


for SHIFT in 4 8
do
truncate -s 0 mptk5_${bname}_${SHIFT}.dat
echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
for M in 512 1024 2048 4096 8192
do
a=$(( $M/$SHIFT )) 
cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' >> tmpdict.xml
done
echo '</dict>' >> tmpdict.xml
cat tmpdict.xml
./mptk/bin/mpd --nIter=${MAXIT} -d tmpdict.xml -s $SNR $FILE ${bname}.bin ${bname}_rec.wav  > tmpfile.dat 
sed 's/ /'"${M}"' /' tmpfile.dat >> mptk5_${bname}_${SHIFT}.dat
echo '' >>  mptk5_${bname}_${SHIFT}.dat
done
done


for SHIFT in 4 8 16 32
do
truncate -s 0 mptk_${bname}_${SHIFT}.dat
for M in 512 1024 2048 4096 8192
do
echo '<?xml version="1.0" encoding="ISO-8859-1"?> <dict> <libVersion>0.2</libVersion>' > tmpdict.xml
a=$(( $M/$SHIFT )) 
cat blocktemp.xml | sed 's/%M/'"${M}"'/g' | sed 's/%a/'"${a}"'/g' >> tmpdict.xml
echo '</dict>' >> tmpdict.xml
cat tmpdict.xml
./mptk/bin/mpd --nIter=${MAXIT} -d tmpdict.xml -s $SNR $FILE ${bname}.bin ${bname}_rec.wav  > tmpfile.dat 
sed 's/ /'"${M}"' /' tmpfile.dat >> mptk_${bname}_${SHIFT}.dat
echo '' >>  mptk_${bname}_${SHIFT}.dat
done
done
done

