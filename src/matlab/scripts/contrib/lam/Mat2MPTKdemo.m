load handel;
[B, res, dec]=mpd(y, Fs, 'n', 1000, 'R', 100);%will run mpd with the 'default.xml' dictionary file.
figure;plot(10*log(dec)); title('Decay');xlabel('iterations');ylabel('Energy [dB]');
[B1, B2] = mpf(B, 'i', '[1:500]');%split the book into two books, B1 contains the 500 first atoms, B2 the 500 following ones
figure;bookplot(B1);title('First 500 atoms');
figure;bookplot(B2);title('Following 500 atoms');

y1 = mpr(B1); %build signal from B1
y2 = mpr(B2, res); % build signal from B2 and add the initial residual
B3 = mpcat(B1, B2); % B3 must be equal to B!
