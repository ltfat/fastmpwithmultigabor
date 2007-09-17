#!/bin/sh

echo >> code_stats.txt
echo >> code_stats.txt
echo "CODE STATISTICS FOR THE MATCHING PURSUIT SOFTWARE" >> code_stats.txt
date >> code_stats.txt

echo >> code_stats.txt
echo "Number of lines in the core library:" >> code_stats.txt
cat src/libmptk/*.{h,cpp,c,lpp} src/libmptk/atom_classes/*.{h,cpp,lpp} | wc -l >> code_stats.txt

echo >> code_stats.txt
echo "Number of lines in the DSP Windows library:" >> code_stats.txt
cat src/libdsp_windows/*.{h,c} | wc -l >> code_stats.txt

echo >> code_stats.txt
echo "Number of lines in the Utils apps (excluding getopt):" >> code_stats.txt
cat src/utils/mp*.cpp | wc -l >> code_stats.txt

echo >> code_stats.txt
echo "Number of lines in the Test apps:" >> code_stats.txt
cat src/tests/*.{c,cpp,sh.in} | wc -l >> code_stats.txt

echo >> code_stats.txt
echo "Number of lines in the GUI apps:" >> code_stats.txt
cat src/gui/*.{h,cpp} | wc -l >> code_stats.txt

echo >> code_stats.txt
echo "Number of lines in the Matlab extras:" >> code_stats.txt
cat extras/matlab/*.m | wc -l >> code_stats.txt

echo >> code_stats.txt
echo "Grand total:" >> code_stats.txt
cat src/libmptk/*.{h,cpp,c,lpp} src/libmptk/atom_classes/*.{h,cpp,lpp} src/libdsp_windows/*.{h,c} src/utils/mp*.cpp src/tests/*.{c,cpp,sh.in} src/gui/*.{h,cpp} extras/matlab/*.m | wc -l >> code_stats.txt

