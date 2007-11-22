This document describes how to use THE MPTK utilities.

On all plateform, the utilities need an environment variable called MPTK_CONFIG_FILENAME to be set with the path 
of the path.xml file located in the bin directory.
This file defines the path to the Atoms/Block plugin location. On Windows system in the case you have installed MPTK library and utilities in an non standard directory
(eg different of "C:/Program Files/MPTK-0.5.4") you have to modify the path inside of this file with the real installation path.

In order to set an environment variable on Linux like os:
-with Bash shell:
export MPTK_CONFIG_FILENAME="path_to_MPTK/bin/path.xml"
-with C-shell:
setenv MPTK_CONFIG_FILENAME "path_to_MPTK/bin/path.xml"
You can check if the environment variable is correctly set with:
echo $MPTK_CONFIG_FILENAME
In order to set environment variable on Windows like system:
Lauch a command line and use the SET command:
SET MPTK_CONFIG_FILENAME=path_to_MPTK/bin/path.xml 
for example SET MPTK_CONFIG_FILENAME=C:/Program Files/MPTK-0.5.4/bin/path.xml 
To check if the environment variable is correctly set you can use the ECHO command:
ECHO %MPTK_CONFIG_FILENAME%

Windows OS like system may comply with a missing dll: MSVCR71D.DLL
You may install the files by using the Microsoft (TM) c runtime library SDK installer 
Or download it with this adress for example: http://www.dll-files.com/dllindex/dll-files.shtml?msvcr71d

All the command line executable have a context help accessible by using the --help option.

For the MPTK-GUI executable, on Linux and OS X platform you need to launch this GUI application in the bin directory of your installation