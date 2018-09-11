% Get mptk4matlab information
mptkInfo = getmptkinfo;
bookfile = mptkInfo.path.exampleBook;
sigfile  = mptkInfo.path.exampleSignal;

% Read book with script
book{1} = bookread_deprecated(bookfile);
book{1}
pause;
% Read book with MEX implementation
book{2} = bookread_exp(bookfile);
book{2}
pause;

numTestBook = length(book);

% Test bookplot
disp(['Testing bookplot.m']);
figure(1);
subplot(numTestBook,1,1);
for i=1:numTestBook
  subplot(numTestBook,1,i)
  bookplot(book{i});
  title(['Test bookplot with book.format:' book{i}.format]);
end

% Test bookover
disp(['Testing bookover.m']);
figure(2);
subplot(numTestBook,1,1);
for i=1:numTestBook
  subplot(numTestBook,1,i)
  bookover(book{i},sigfile);
  title(['Test bookover with book.format:' book{i}.format]);
end

% Test mpview

% Test bookmpr_exp (documentation is erroneous)

% Test bookedit_exp



% Test mpr mpr_wrap

% Test mpcap mpcat_wrap

% Test mpd mpd_demix mpd_demix_wrap mpd_wrap 



disp('End of book io debug. Successfull!');