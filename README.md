This is a reproducible research addendum to the paper

> Průša, Balazs, Holighaus: Fast Matching Pursuit with Multi-Gabor Dictionaries
 [preprint](http://ltfat.github.io/notes/ltfatnote052.pdf)

The provided bash scripts will obviously only work on Linux.

## Contents

* repr_fig1.m -- Reproduces Figure 1 from the paper. Requires LTFAT.
* setup.sh -- Compiles the matching pursuit decomposition executables from MPTK and LIBLTFAT
* run.sh -- Runs a head-to-head comparison of the two implementations of MP using provided dictionary.
* batch_run_comparison.sh -- Runs a batch comparision as presented in Section V in the paper.

A snippet from the run.sh output:
```
------------------------Running MPTK's mpd------------------------------
MP DURATION  3.88 s  <---- MP decomposition runtime
 ...
------------------------Running libltfat's mpd--------------------------
INIT DURATION: 114 ms
DURATION: 914 ms <---- MP decomposition runtime
SYN DURATION: 84 ms
```

## Requirements

Required libraries are fftw3, sndfile and pthreads.

## License
PhaseReT is distributed under terms of
[GPL3](http://www.gnu.org/licenses/gpl-3.0.en.html)



