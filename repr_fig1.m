function repr_fig1
do_export = 0;


tol = 1e-4;
L = 10*2048;
fontsize = 22;
figsize = [480,640];
ylimm = [-15,15];

dp1 = struct('g',firwin('hann',2048,'2'),'a',512,'M',2048);
[kerns] = projkernels(dp1,dp1,L,tol,0);

kern = abs(kerns{1});
kern(abs(kern)<max(abs(kern(:)))*tol) = tol;

[kernh,kernw] = size(kern);
yaxislabels =   -(floor(kernh/2)) + (0:kernh-1);
xaxislabels =  - (floor(kernw/2)) + (0:kernw-1);

figure(2); 
imagesc(xaxislabels,yaxislabels,20*log10(kern)); colormap(flipud(gray));

cbh = colorbar();
set(cbh,'YTick',[-80:10:0])
set(gca,'FontSize',fontsize); 
set(gca,'XTick',xaxislabels);
ylim(ylimm);

if do_export
    set(gca,'YTick',[]);
    pos = get(gcf,'Position');set(gcf,'Position',[pos(1:2),figsize]);
    set(gca, 'Color', 'none');set(gcf, 'Color', 'none');
    export_fig('hannkern.pdf','-painters', '-transparent');
end

dp1 = struct('g',pgauss(L,51.2,'2'),'a',512,'M',2048);
[kerns] = projkernels(dp1,dp1,L,tol,0);

kern = abs(kerns{1});
kern(abs(kern)<max(abs(kern(:)))*tol) = tol;
[kernh,kernw] = size(kern);
yaxislabels =   -(floor(kernh/2)) + (0:kernh-1);
xaxislabels =  - (floor(kernw/2)) + (0:kernw-1);

figure(1);
imagesc(xaxislabels,yaxislabels,20*log10(kern)); colormap(flipud(gray));
ylim(ylimm);

cbh = colorbar();
set(cbh,'YTick',[-80:10:0])
set(gca,'FontSize',fontsize);
set(gca,'XTick',xaxislabels);
%set(gca,'YTick',[]);

if do_export
    pos = get(gcf,'Position');set(gcf,'Position',[pos(1:2),figsize]);
    set(gca, 'Color', 'none');set(gcf, 'Color', 'none');
    export_fig gausskern.pdf -transparent 
end

dp1 = struct('g',firwin('blackman',2048,'2'),'a',512,'M',2048);
[kerns] = projkernels(dp1,dp1,L,tol,0);

kern = abs(kerns{1});
kern(abs(kern)<max(abs(kern(:)))*tol) = tol;
[kernh,kernw] = size(kern);
yaxislabels =   -(floor(kernh/2)) + (0:kernh-1);
xaxislabels =  - (floor(kernw/2)) + (0:kernw-1);

figure(3); 
imagesc(xaxislabels,yaxislabels,20*log10(kern)); colormap(flipud(gray));
ylim(ylimm);

cbh = colorbar();
set(cbh,'YTick',[-80:10:0])
set(gca,'FontSize',fontsize);
set(gca,'XTick',xaxislabels);

ylim(ylimm);

if do_export
    set(gca,'YTick',[]);
    pos = get(gcf,'Position');set(gcf,'Position',[pos(1:2),figsize]);
    set(gca, 'Color', 'none');set(gcf, 'Color', 'none');
    export_fig blackmankern.pdf -transparent 
end


function [kerns,kmods,kmid,mask,kernnorm] = projkernels(dp1,dp2,Llong,tol,do_all)
if nargin < 5
    do_all = 1;
end

a1 = dp1.a; M1 = dp1.M; g1 = dp1.g; 
a2 = dp2.a; M2 = dp2.M; g2 = dp2.g; 

a = min([a1,a2]);
M = max([M1,M2]);
%Lshort = min([Llong,2*max(dp1.tailsSize) + 2*max(dp2.tailsSize)]);
Lshort = Llong;
L = dgtlength(Lshort,a,M);

N = L/a;
M2 = floor(M/2) + 1;

kern = dgt(middlepad(g1,L),middlepad(g2,L),a,M);
kernnorm = norm(kern,'fro');

[ksize,kmid]=findsmallkernelsize(kern(1:M2,:),tol);

if ksize(1)>M, ksize(1) = M; end
if ksize(2)>N, ksize(2) = N; end
kernh = ksize(1); 
kernw = ksize(2);

kern = middlepad(kern,kernh);
kernw1 = kmid(2);
kernw2 = kernw - (kmid(2)+1);
kern = [kern(:,1:kernw1),kern(:,end-kernw2:end)];


if a1 > a2
    kernno = lcm(M,a)/a1; 
    arat = a1/a2;
else
    kernno = lcm(M,a)/a;
    arat = 1;
end
kern =  circshift(kern,kmid-1);

thr = tol*max(abs(kern(:)));
mask = abs(kern(:)) > thr;
kmods = zeros(kernh,kernno);

for n=1:kernno
    kmods(:,n) = phasekernfi(kernh,kmid,arat*(n-1),a,M);
end

if do_all
    kerns = cell(kernno,1);

    for n=1:kernno
      kerns{n} = bsxfun(@times,kern,kmods(:,n));
      %  kerns{n} = phasekernfi(kern,kmid,arat*(n-1),a,M);
    end
else
    kerns = cell(1);
    kerns{1} = kern;
end  

function [ksize,kmid]=findsmallkernelsize(kern,relthr)
[M2,N] = size(kern);
thr = relthr*max(abs(kern(:)));

lastrow = 1;
for n=1:N
    newlastrow = find(abs(kern(:,n))>thr,1,'last');
    if newlastrow > lastrow
        lastrow = newlastrow;
    end
end

lastcol1 = 1;
lastcol2 = 1;
for m=1:M2
    newlastcol = find(abs(kern(m,1:floor(end/2)+1))>thr,1,'last');
    if newlastcol > lastcol1
        lastcol1 = newlastcol;
    end
    newlastcol = find(abs(kern(m,end:-1:floor(end/2)+1))>=thr,1,'last');
    if newlastcol > lastcol2
        lastcol2 = newlastcol;
    end
end
%lastcol2 = floor((lastcol2/2));

ksize = [2*lastrow-1, lastcol1 + lastcol2];
kmid = [lastrow,lastcol1];
    
function kernmod = phasekernfi(kernh,kmid,n,a,M,offset,subs)
if nargin < 6
    offset = 0;
    subs = 1;
end
if nargin < 7
    subs = 1;
end

%kernh = size(kern,1);
kmidh = kmid(1) + offset;
midx = (-kmidh + 1:kernh - kmidh)'*subs;
%midx = circshift(fftindex(kernh),kmidh-1);
kernmod = exp(-1i*2*pi*n*midx*a/M);
    
