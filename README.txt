The Matching Pursuit Tool Kit (MPTK) provides a fast implementation of the Matching Pursuit algorithm for the sparse decomposition of multichannel audio signals. It comprises a library, standalone utilities and wrappers for Matlab and Python enabling you to use MPTK directly and plot its results.

What is MPTK ?
MPTK provides an implementation of Matching Pursuit which is:

    * FAST: e.g., extract 1.5 million atoms from a 1 hour long, 16kHz audio signal (15dB extracted) in 0.25x real time on a Pentium IV@2.4GHz,
           out of a dictionary of 178M Gabor atoms. Such incredible speed makes it possible to process "real world" signals.
    * FLEXIBLE: multi-channel, various signal input formats, flexible syntax to describe the dictionaries => reproducible results, cleaner experiments.
    * OPEN: modular architecture => add your own atoms ! Free distribution under the GPL.

MPTK is mainly developed and maintained within the PANAMA Research Group (http://www.irisa.fr/panama) on audio signal processing, at the IRISA Research Institute (http://www.irisa.fr) in Rennes, France. 

To build MPTK yo will need some packages:
-Libsndfile: at least version 1.0.11 
-fftw: at least version 3.0.1 
-Qt(OpenSource): at least version 4.3.1

For further informations see Build MPTK for Linux and OSX or Build MPTK for Windows (Win 32) in pdf.
Available on the INRIA GFORGE MPTK project (https://gforge.inria.fr/projects/mptk/) under Docs/Misc

For using MPTK utilities, please read the readme.txt located in the bin directory.

If you have some questions about Matching Pursuit and MPTK, please refer to http://mptk.irisa.fr/
