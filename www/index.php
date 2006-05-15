<?php

$domain=ereg_replace('[^\.]*\.(.*)$','\1',$_SERVER['HTTP_HOST']);
$group_name=ereg_replace('([^\.]*)\..*$','\1',$_SERVER['HTTP_HOST']);

echo '<?xml version="1.0" encoding="UTF-8"?>';
?>
<!DOCTYPE html
	PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en   ">

  <head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title><?php echo $project_name; ?></title>
	<script language="JavaScript" type="text/javascript">
	<!--
	function help_window(helpurl) {
		HelpWin = window.open( helpurl,'HelpWindow','scrollbars=yes,resizable=yes,toolbar=no,height=400,width=400');
	}
	// -->
		</script>

<style type="text/css">
	<!--
	BODY {
		margin-top: 3;
		margin-left: 3;
		margin-right: 3;
		margin-bottom: 3;
		background: #5651a1;
	}
	ol,ul,p,body,td,tr,th,form { font-family: verdana,arial,helvetica,sans-serif; font-size:small;
		color: #333333; }

	h1 { font-size: x-large; font-family: verdana,arial,helvetica,sans-serif; }
	h2 { font-size: large; font-family: verdana,arial,helvetica,sans-serif; }
	h3 { font-size: medium; font-family: verdana,arial,helvetica,sans-serif; }
	h4 { font-size: small; font-family: verdana,arial,helvetica,sans-serif; }
	h5 { font-size: x-small; font-family: verdana,arial,helvetica,sans-serif; }
	h6 { font-size: xx-small; font-family: verdana,arial,helvetica,sans-serif; }

	pre,tt { font-family: courier,sans-serif }

	a:link { text-decoration:none }
	a:visited { text-decoration:none }
	a:active { text-decoration:none }
	a:hover { text-decoration:underline; color:red }

	.titlebar { color: black; text-decoration: none; font-weight: bold; }
	a.tablink { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tablink:visited { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tablink:hover { text-decoration: none; color: black; font-weight: bold; font-size: x-small; }
	a.tabsellink { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tabsellink:visited { color: black; text-decoration: none; font-weight: bold; font-size: x-small; }
	a.tabsellink:hover { text-decoration: none; color: black; font-weight: bold; font-size: x-small; }
	-->
</style>

</head>

<body>

<table border="0" width="100%" cellspacing="0" cellpadding="0">

	<tr>
		<td><a href="/"><img src="http://<?php echo $domain; ?>/themes/inria/images/logo.png" border="0" alt="" width="198" height="52" /></a></td>
	</tr>

</table>

<table border="0" width="100%" cellspacing="0" cellpadding="0">

	<tr>
		<td>&nbsp;</td>
		<td colspan="3">



		<!-- start tabs -->

	<tr>
		<td align="left" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topleft.png" height="9" width="9" alt="" /></td>
		<td bgcolor="#E0E0E0" width="30"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="30" height="1" alt="" /></td>
		<td bgcolor="#E0E0E0"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
		<td bgcolor="#E0E0E0" width="30"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="30" height="1" alt="" /></td>
		<td align="right" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topright.png" height="9" width="9" alt="" /></td>
	</tr>

	<tr>

		<!-- Outer body row -->

		<td bgcolor="#E0E0E0"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="10" height="1" alt="" /></td>
		<td valign="top" width="99%" bgcolor="#E0E0E0" colspan="3">

			<!-- Inner Tabs / Shell -->

			<table border="0" width="100%" cellspacing="0" cellpadding="0">
			<tr>
				<td align="left" bgcolor="#ffffff" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topleft-inner.png" height="9" width="9" alt="" /></td>
				<td bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
				<td align="right" bgcolor="#ffffff" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/topright-inner.png" height="9" width="9" alt="" /></td>
			</tr>

			<tr>
				<td bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="10" height="1" alt="" /></td>
				<td valign="top" width="99%" bgcolor="white">

	<!-- whole page table -->
<table width="100%" cellpadding="5" cellspacing="0" border="0">
<tr><td width="65%" valign="top" bgcolor="#ffaa77">

<HR>

<?php if ($handle=fopen('http://'.$domain.'/export/projtitl.php?group_name='.$group_name,'r')){
$contents = '';
while (!feof($handle)) {
	$contents .= fread($handle, 8192);
}
fclose($handle);
echo $contents; } ?>

<!-------------------------------------------------------------------------------------------->
<!--  OUR ADDITIONS -------------------------------------------------------------------------->

<HR>

<P>
<H3>Introduction:</H3>

<B>MPTK is a complete toolkit for the demonstration and exploration of the Matching Pursuit algorithm</B>.
It is:
<UL>
<LI> <B>FAST:</B> e.g., extract 1.5 million atoms from a 1 hour long, 16kHz audio signal (15dB extracted)
  in <B>0.25x real time</B> on a Pentium IV@2.4GHz, out of a dictionary of 178M Gabor atoms. Such incredible
	  speed makes it possible to process "real world" signals;
<LI> <B>FLEXIBLE:</B> multi-channel, various signal input formats, flexible syntax to describe the dictionaries =>
	  reproducible results, cleaner experiments;
<LI> <B>OPEN:</B> modular architecture => add your own atoms ! Free distribution under the GPL.
</UL>
You can have a more detailed taste of what the software is all about by looking
at the <B><A HREF="http://gforge.inria.fr/docman/?group_id=36">documentation</A></B> available on the
<B><A HREF="http://gforge.inria.fr/docman/?group_id=36">Doc Manager</A></B> page.


<P>
<H3>Warning:</H3>

Most of the MPTK package is fairly stable now. However, we haven't reached version 1.0 yet.
In particular, the data format of the "books" (the
collections of atoms resulting from the MPTK processing) and of the XML
dictionaries is bound to change in the upcoming versions. We are working on a
system of backwards compatibility, but it is not ready yet: in the meantime,
the formats may change with each new version (we will warn the users about
it). We are conscious that this is a nuisance, and we are working on it.


<P>
<H3>Download and install:</H3>

Relevant packages are available from the <B><A
HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">Released
Files</A></B> section in the <B><A
HREF="http://gforge.inria.fr/projects/mptk/">Project Summary</A></B> menu, on
the right of this page.

<P>The MPTK software corresponds to the most recent version of the packages
available in the <B>MPTK</B> section of the <B><A
HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">Released
Files</A></B> page.  A limited number of older versions are kept there as
history.

<P>The mptk package depends on a few external libraries:  FFTW3, libsndfile and
(if you want to compile the GUI) wxWidgets. It is mandatory to have these
libraries installed on your system before you can compile MPTK. The versions
which worked for us when compiling the latest release of the MPTK package are
mirrored in the <B>MPTK_externals</B> section of the <A
HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">Released
Files</A></B> page.

<P>As an option, contributions from other labs than the original authors are
distributed in the <B>MPTK_contributions</B> section of the <A
HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">Released
Files</A></B> page, but it is not mandatory to install them in order to compile
and use the original MPTK package. Some reference articles are also available,
from the <B>MPTK_related_articles</B> section of the <A
HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">Released
Files</A></B> page.


<P>
<H3>Help and forums:</H3> If you need help with the software:
<OL>
<LI> check if a more recent
<B><A HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">release</A></B>
fixes your problem;
<LI> if not, use the <B><A HREF="http://gforge.inria.fr/forum/forum.php?forum_id=109">Help forum</A></B>
to ask questions.
</OL>

<P> Other <B><A HREF="http://gforge.inria.fr/forum/?group_id=36">Forums</A></B> are available for
open discussions about the Matching Pursuit algorithm and its MPTK implementation.

<P>
<H3>Related articles:</H3> Some articles exposing scientific results related to
MPTK are available in PDF format through the 
<B><A HREF="http://gforge.inria.fr/project/showfiles.php?group_id=36">Released Files</A></B>
page.

<P>
<H3>Contact:</H3> If you are confused by this page, or if you want to communicate privately with us,
please write to <img src="./addrmatchp.jpg" align="middle">.<BR>
Request for help sent to this address won't be answered. Please use the
<B><A HREF="http://gforge.inria.fr/forum/forum.php?forum_id=109">Help forum</A></B> instead.

<P>
This software is currently developed and maintained by
<B><A HREF="http://gforge.inria.fr/users/sacha/">Sacha Krstulovic</A></B>
and
<B><A HREF="http://gforge.inria.fr/users/remi/">R&eacute;mi Gribonval</A></B>
within the <B><A HREF="http://www.irisa.fr/metiss/">METISS Research Group</A></B>,
at the <B><A HREF="http://www.irisa.fr/">IRISA Institute</A></B> in Rennes, France.

<P>
<B>Thank you for your interest in The Matching Pursuit ToolKit !</B>

<BR><BR>

<HR>

<!-------------------------------------------------------------------------------------------->

<?php if ($handle=fopen('http://'.$domain.'/export/projnews.php?group_name='.$group_name,'r')){
$contents = '';
while (!feof($handle)) {
	$contents .= fread($handle, 8192);
}
fclose($handle);
$contents=str_replace('href="/','href="http://'.$domain.'/',$contents);
echo $contents; } ?>

</td>

<td width="35%" valign="top">

		<table cellspacing="0" cellpadding="1" width="100%" border="0" bgcolor="#d5d5d7">
		<tr><td>
			<table cellspacing="0" cellpadding="2" width="100%" border="0" bgcolor="#eaecef">
				<tr style="background-color:#d5d5d7" align="center">
					<td colspan="2"><span class="titlebar">Project Summary</span></td>
				</tr>
				<tr align="left">
					<td colspan="2">

<?php if($handle=fopen('http://'.$domain.'/export/projhtml.php?group_name='.$group_name,'r')){
$contents = '';
while (!feof($handle)) {
	$contents .= fread($handle, 8192);
}
fclose($handle);
$contents=str_replace('href="/','href="http://'.$domain.'/',$contents);
$contents=str_replace('src="/','src="http://'.$domain.'/',$contents);
echo $contents; } ?>

					</td>
				</tr>
			</table>
		</td></tr>
		</table><p>&nbsp;</p>
</td></tr></table>
			&nbsp;<p>
                        <center>
                                Help: <a href="mailto:siteadmin-help@lists.gforge.inria.fr">siteadmin-help@lists.gforge.inria.fr</a> Webmaster: <a href="mailto:webmaster@gforge.inria.fr">webmaster@gforge.inria.fr</a>
                        </center>
			<!-- end main body row -->


				</td>
				<td width="10" bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="2" height="1" alt="" /></td>
			</tr>
			<tr>
				<td align="left" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomleft-inner.png" height="11" width="11" alt="" /></td>
				<td bgcolor="#ffffff"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
				<td align="right" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomright-inner.png" height="11" width="11" alt="" /></td>
			</tr>
			</table>

		<!-- end inner body row -->

		</td>
		<td width="10" bgcolor="#E0E0E0"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="2" height="1" alt="" /></td>
	</tr>
	<tr>
		<td align="left" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomleft.png" height="9" width="9" alt="" /></td>
		<td bgcolor="#E0E0E0" colspan="3"><img src="http://<?php echo $domain; ?>/themes/inria/images/clear.png" width="1" height="1" alt="" /></td>
		<td align="right" bgcolor="#E0E0E0" width="9"><img src="http://<?php echo $domain; ?>/themes/inria/images/tabs/bottomright.png" height="9" width="9" alt="" /></td>
	</tr>
</table>

<!-- PLEASE LEAVE "Powered By Gforge" on your site -->
<br />
<center>
<a href="http://gforge.org/"><img src="http://gforge.org/images/pow-gforge.png" alt="Powered By GForge Collaborative Development Environment" border="0" /></a>
</center>


</body>
</html>
