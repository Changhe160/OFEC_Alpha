# - this module looks for Matlab
# Defines:
#  MATLAB_INCLUDE_DIR: include path for mex.h
#  MATLAB_LIBRARIES:   required libraries: libmex, libmx
#  MATLAB_MEX_LIBRARY: path to libmex
#  MATLAB_MX_LIBRARY:  path to libmx

SET(MATLAB_FOUND 0)
IF( "$ENV{MATLAB_ROOT}" STREQUAL "" )
    MESSAGE(STATUS " " )
    MESSAGE(STATUS " " )
    MESSAGE(STATUS " " )
    MESSAGE(STATUS " MATLAB_ROOT environment variable not set." )
    MESSAGE(STATUS " In Linux append the following to your .bashrc file:" )
    MESSAGE(STATUS " >>> export MATLAB_ROOT=/usr/local/MATLAB/R2014a <<<" )
    MESSAGE(STATUS " be sure to pick the correct version of Matlab" )
    MESSAGE(STATUS " " )
    MESSAGE(STATUS " " )
    MESSAGE(STATUS " " )
    MESSAGE(FATAL_ERROR " No matlab found. Follow the instructions above." )
ELSE("$ENV{MATLAB_ROOT}" STREQUAL "" )

        #find_path(MATLAB_INCLUDE_DIR
        #   mclmcrrt.h
        #   HINTS ${MATLAB_ROOT}/extern/include
        #    DOC "Matlab C/C++ include directory")

        FIND_PATH(MATLAB_INCLUDE_DIR mclmcrrt.h
                  $ENV{MATLAB_ROOT}/extern/include)
   
       MESSAGE (STATUS "MATLAB_INCLUDE_DIR: ${MATLAB_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

        FIND_LIBRARY( MATLAB_MEX_LIBRARY
                      NAMES libmex mex
                      PATHS $ENV{MATLAB_ROOT}/bin $ENV{MATLAB_ROOT}/extern/lib
                      $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft
                      PATH_SUFFIXES glnxa64 glnx86)

        FIND_LIBRARY( MATLAB_ENG_LIBRARY
                      NAMES libeng eng
                      PATHS $ENV{MATLAB_ROOT}/bin $ENV{MATLAB_ROOT}/extern/lib 
                      $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft
                      PATH_SUFFIXES glnxa64 glnx86)

        FIND_LIBRARY( MATLAB_MX_LIBRARY
                      NAMES libmx mx
                      PATHS $ENV{MATLAB_ROOT}/bin $ENV{MATLAB_ROOT}/extern/lib 
                      $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft
                      PATH_SUFFIXES glnxa64 glnx86)

        FIND_LIBRARY( MATLAB_mwmclmcr_LIBRARY
                      NAMES mwmclmcr  mclmcr 
                      PATHS $ENV{MATLAB_ROOT}/bin $ENV{MATLAB_ROOT}/extern/lib 
                      $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft
                      $ENV{MATLAB_ROOT}/bin/glnxa64
                      $ENV{MATLAB_ROOT}/runtime/glnxa64
                      PATH_SUFFIXES glnxa64 glnx86)

      
      #  FIND_LIBRARY( MATLAB_mwmclmcrrt_LIBRARY
      #                NAMES libmwmclmcrrt mclmcrrt
      #                PATHS  $ENV{MATLAB_ROOT}/runtime/glnxa64 
      #                $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft)


      #  FIND_LIBRARY(Matlab_LIBRARIES
      #                NAMES  mwmclmcrrt  mwmclmcr  libmwmclmcrrt libmwmclmcr   mclmcrrt  mclmcr
      #                PATHS $ENV{MATLAB_ROOT}/bin $ENV{MATLAB_ROOT}/extern/lib 
      #                PATH_SUFFIXES glnxa64 glnx86)


      #target_link_libraries(${CMAKE_PROJECT_NAME}  ${MATLAB_mwmclmcr_LIBRARY})
      #target_link_libraries(${CMAKE_PROJECT_NAME}  ${MATLAB_mwmclmcrrt_LIBRARY})
      
      MESSAGE (STATUS "MATLAB_MEX_LIBRARY: ${MATLAB_MEX_LIBRARY}")
      MESSAGE (STATUS "MATLAB_ENG_LIBRARY: ${MATLAB_ENG_LIBRARY}")
      MESSAGE (STATUS "MATLAB_MX_LIBRARY: ${MATLAB_MX_LIBRARY}")
      MESSAGE (STATUS "MATLAB_mwmclmcr_LIBRARY: ${MATLAB_mwmclmcr_LIBRARY}")
     # MESSAGE (STATUS "MATLAB_mwmclmcrrt_LIBRARY: ${MATLAB_mwmclmcrrt_LIBRARY}")


      target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${MATLAB_MEX_LIBRARY})
      target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE  ${MATLAB_ENG_LIBRARY})
      target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE  ${MATLAB_MX_LIBRARY})
      target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE  ${MATLAB_mwmclmcr_LIBRARY})

     IF (MSVC)
         message(STATUS "MSVC $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft/mclmcrrt.lib")
         target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "$ENV{MATLAB_ROOT}/extern/lib/win64/microsoft/mclmcrrt.lib")

    else()

         message(STATUS "UNIX $ENV{MATLAB_ROOT}/runtime/glnxa64/libmwmclmcrrt.so")
         target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "$ENV{MATLAB_ROOT}/runtime/glnxa64/libmwmclmcrrt.so")
         target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "$ENV{MATLAB_ROOT}/extern/bin/glnxa64/libMatlabEngine.so")
         target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "$ENV{MATLAB_ROOT}/extern/bin/glnxa64/libMatlabDataArray.so")
     endif()
 
      #if(MSVC)
      #   target_link_libraries(${CMAKE_PROJECT_NAME}  "$ENV{MATLAB_ROOT}/extern/lib/win64/microsoft/mclmcr.lib")
        #  message(STATUS "mclmcr found  $ENV{MATLAB_ROOT}/extern/lib/win64/microsoft/mclmcr.lib ")
    #  else    
          #target_link_libraries(${CMAKE_PROJECT_NAME}  "$ENV{MATLAB_ROOT}/extern/lib/win64/microsoft/mclmcr.lib")
     #     target_link_libraries(${CMAKE_PROJECT_NAME}  "$ENV{MATLAB_ROOT}/runtime/glnxa64/libmwmclmcrrt.so")
     #      MESSAGE (STATUS "MATLAB_mwmclmcr_LIBRARY: $ENV{MATLAB_ROOT}/runtime/glnxa64/libmwmclmcrrt.so")
          # message(STATUS "mclmcr found  $ENV{MATLAB_ROOT}/runtime/glnxa64/libmwmclmcrrt.so ")
      #endif()


      #target_link_libraries(${CMAKE_PROJECT_NAME}  ${MATLAB_mwmclmcrrt_LIBRARY})
      #target_link_libraries(${CMAKE_PROJECT_NAME}  "$ENV{MATLAB_ROOT}/extern/lib/win64/microsoft/mclmcr.lib")


    MESSAGE (STATUS "MATLAB_ROOT: $ENV{MATLAB_ROOT}")

ENDIF("$ENV{MATLAB_ROOT}" STREQUAL "" )


# This is common to UNIX and Win32:
SET(MATLAB_LIBRARIES
  ${MATLAB_MEX_LIBRARY}
  ${MATLAB_MX_LIBRARY}
  ${MATLAB_ENG_LIBRARY}
  ${MATLAB_mwmclmcr_LIBRARY}
)

MESSAGE (STATUS "MATLAB_ROOT: ${MATLAB_INCLUDE_DIR}")
MESSAGE (STATUS "MATLAB_LIBRARIES: ${MATLAB_LIBRARIES}")
IF(MATLAB_INCLUDE_DIR AND MATLAB_LIBRARIES)
  SET(MATLAB_FOUND 1)
  MESSAGE(STATUS "Matlab libraries will be used")
ENDIF(MATLAB_INCLUDE_DIR AND MATLAB_LIBRARIES)

MARK_AS_ADVANCED(
  MATLAB_LIBRARIES
  MATLAB_ENG_LIBRARY
  MATLAB_MEX_LIBRARY
  MATLAB_MX_LIBRARY
  MATLAB_mwmclmcr_LIBRARY
  MATLAB_INCLUDE_DIR
  MATLAB_FOUND
  MATLAB_ROOT
)
