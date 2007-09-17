from distutils.core import setup,Extension

src = ['pyMPTK.cpp']

inc_dir = ['/usr/include', '@MPTK_SOURCE_DIR@','@MPTK_SOURCE_DIR@/src/tinyxml', '@MPTK_SOURCE_DIR@/src/libmptk/', '@MPTK_BINARY_DIR@' , '@MPTK_BINARY_DIR@/src/libmptk/', '@LIBDSP_INCLUDE_DIR@','@FFTW3_INCLUDE_DIR@', '@SNDFILE_INCLUDE_DIR@']

lib =['@MPTK_BINARY_DIR@/lib/@LIB_MPTK_OUTPUT_NAME@','@MPTK_BINARY_DIR@/lib/dsp_windows']

setup (name = "pyMPTK",
	version = "0.1",
	description = "Python bindings for Matching Pursuit ToolKit",
	author = "Emmanuel Ravelli",
	author_email = "ravelli@lam.jussieu.fr",
	ext_modules = [Extension('pyMPTK', src, include_dirs=inc_dir, libraries=lib)]
)