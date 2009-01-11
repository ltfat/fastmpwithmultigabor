This directory contains a fast gammatone signal model based on matched gammatone filters as described in Appendix A in "Analysis and Design of Overcomplete Gammatone Signal Models" by S. Strahl and A. Mertins submitted January 2009 to JASA. The numerical methods as described in `Improved numerical methods for gammatone filterbank analysis and synthesis' by T. Herzkeand V. Hohmann, published 2007 in `Acta Acustica' have been used.
Note that a gammatone signal model with filter bandwidths not being dependent on the center frequency of the gammatone filter can be realized using the already existing MDCT atom class and the newly implemented "gamma" window:
<blockproperties name="GAMMA-WINDOW">   <param name="windowtype" value="gamma"/>   <param name="windowopt" value="4.01"/>
</blockproperties>
<block uses="GAMMA-WINDOW">
   <param name="type" value="mdct"/>	   <param name="windowLen" value="4096"/>   <param name="windowShift" value="1"/>   <param name="fftSize" value="4096"/></block>
:) stef@nstrahl.de
