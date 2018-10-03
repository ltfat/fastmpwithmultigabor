#!/bin/bash
# Can be run as ./batch_run_comparison.sh | tee out.txt

# Figure 4 (a)
./run.sh blackman,128,512
./run.sh blackman,256,1024
./run.sh blackman,512,2048
./run.sh blackman,1024,4096
./run.sh blackman,2048,8192

# Figure 4 (b)
./run.sh blackman,64,512
./run.sh blackman,128,1024
./run.sh blackman,256,2048
./run.sh blackman,512,4096
./run.sh blackman,1024,8192

# Figure 4 (c)
./run.sh blackman,32,512
./run.sh blackman,64,1024
./run.sh blackman,128,2048
./run.sh blackman,256,4096
./run.sh blackman,512,8192

# Figure 4 (d)
./run.sh blackman,16,512
./run.sh blackman,32,1024
./run.sh blackman,64,2048
./run.sh blackman,128,4096
./run.sh blackman,256,8192

# 5 dictionaries (Sec. V)
./run.sh blackman,128,512:blackman,256,1024:blackman,512,2048:blackman,1024,4096:blackman,2048,8192

./run.sh blackman,64,512:blackman,128,1024:blackman,256,2048:blackman,512,4096:blackman,1024,8192



