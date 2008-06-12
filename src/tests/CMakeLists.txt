# compil test
   if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
    endif(COMMAND cmake_policy)

ADD_EXECUTABLE(test_anywave ${MPTK_SOURCE_DIR}/src/tests/test_anywave.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_anywave PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_anywave PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_anywave PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_anywave mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_anywave mptk)


ADD_EXECUTABLE(test_fft ${MPTK_SOURCE_DIR}/src/tests/test_fft.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_fft PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_fft PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_fft PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_fft mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_fft mptk)

ADD_EXECUTABLE(test_messaging ${MPTK_SOURCE_DIR}/src/tests/test_messaging.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_messaging PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_messaging PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_messaging PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_messaging mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_messaging mptk)

IF(LINUX)
ADD_CUSTOM_COMMAND(TARGET test_messaging
                     POST_BUILD
                     COMMAND ${MPTK_BINARY_DIR}/bin/test_messaging)
ELSE(LINUX)
IF(MINGW)
ADD_CUSTOM_COMMAND(TARGET test_messaging
                     POST_BUILD
                  COMMAND ${MPTK_BINARY_DIR}/bin/test_messaging)
ENDIF(MINGW)  
ENDIF(LINUX)

ADD_EXECUTABLE(test_win ${MPTK_SOURCE_DIR}/src/tests/test_win.c)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_win PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_win PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_win PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_win mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_win mptk)

ADD_EXECUTABLE(test_parabolic_regression ${MPTK_SOURCE_DIR}/src/tests/test_parabolic_regression.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_parabolic_regression PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_parabolic_regression PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_parabolic_regression PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_parabolic_regression mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_parabolic_regression mptk)

ADD_EXECUTABLE(test_mp ${MPTK_SOURCE_DIR}/src/tests/test_mp.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_mp PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_mp PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_mp PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_mp mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_mp mptk)

ADD_EXECUTABLE(test_mask ${MPTK_SOURCE_DIR}/src/tests/test_mask.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_mask PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_mask PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_mask PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_mask mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_mask mptk)

ADD_EXECUTABLE(test_behaviour_in_noise ${MPTK_SOURCE_DIR}/src/tests/test_behaviour_in_noise.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(test_behaviour_in_noise PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(test_behaviour_in_noise PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(test_behaviour_in_noise PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(test_behaviour_in_noise mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(test_behaviour_in_noise mptk)


CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/dictionary/dict-list.in.txt ${MPTK_BINARY_DIR}/bin/reference/dictionary/dict-list.txt @ONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/dictionary/dic_gabor_two_scales.xml ${MPTK_BINARY_DIR}/bin/reference/dictionary/dic_gabor_two_scales.xml
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/dico_test.xml ${MPTK_BINARY_DIR}/bin/reference/dictionary/dico_test.xml
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/signal/glockenspiel.wav ${MPTK_BINARY_DIR}/bin/reference/signal/glockenspiel.wav
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/2_cosines.flt ${MPTK_BINARY_DIR}/bin/reference/signal/2_cosines.flt
                 COPYONLY)     
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_signal.wav ${MPTK_BINARY_DIR}/bin/reference/signal/anywave_signal.wav
                 COPYONLY)      
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/dico_test_anywave.in.xml ${MPTK_BINARY_DIR}/bin/reference/dictionary/dico_test_anywave.xml @ONLY)          
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_table.bin ${MPTK_BINARY_DIR}/bin/reference/dictionary/anywave_table.xml @ONLY)                 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_table_data.bin ${MPTK_BINARY_DIR}/bin/reference/signal/anywave_table_data.bin
                 COPYONLY)    
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/mixer/mix_58_mixer.txt ${MPTK_BINARY_DIR}/bin/reference/mixer/mix_58_mixer.txt
                 COPYONLY)                             
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/mix_58_stereo.wav ${MPTK_BINARY_DIR}/bin/reference/signal/mix_58_stereo.wav
                 COPYONLY)


