if (Armadillo_required)
    find_package(Armadillo CONFIG REQUIRED)
    list(APPEND PROJECT_LIBRARIES armadillo)
endif(Armadillo_required)

if(mlpack_required)
    find_path(MLPACK_INCLUDE_DIRS "mlpack.hpp")
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MLPACK_INCLUDE_DIRS})
endif(mlpack_required)

if(Eigen_required)
    find_package(Eigen3 3.4 REQUIRED NO_MODULE)
    list(APPEND PROJECT_LIBRARIES Eigen3::Eigen)
endif(Eigen_required)

if(LibTorch_required)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(Torch_DIR "$ENV{LIBTORCH_DIR_DEBUG}/share/cmake/Torch")
    else()
        set(Torch_DIR "$ENV{LIBTORCH_DIR_RELEASE}/share/cmake/Torch")
    endif()
    find_package(Torch REQUIRED)
    list(APPEND PROJECT_LIBRARIES ${TORCH_LIBRARIES})
endif(LibTorch_required)

if(fmt_required)
    if (MSVC)
    set(fmt_DIR "E:/Diao_Yiya/code/package/vcpkg/pakage/packages/fmt_x64-windows/share/fmt")
    endif()
    find_package(fmt CONFIG REQUIRED)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE fmt::fmt)
endif(fmt_required)

if(Mynlohmann_required)
    if (MSVC)
    set(nlohmann_json_DIR "E:/Diao_Yiya/code/package/vcpkg/pakage/packages/nlohmann-json_x86-windows/share/nlohmann_json")
    endif()
    find_package(nlohmann_json CONFIG REQUIRED)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif(Mynlohmann_required)

if(VTK_required)
    find_package(VTK REQUIRED)
    list(APPEND PROJECT_LIBRARIES ${VTK_LIBRARIES})
    vtk_module_autoinit(TARGETS ${CMAKE_PROJECT_NAME} MODULES ${VTK_LIBRARIES})
endif(VTK_required)

if(QCustomPlot_required)
   add_definitions(-DQCUSTOMPLOT_USE_LIBRARY)
   find_package(QCustomPlot REQUIRED)
   list(APPEND PROJECT_LIBRARIES ${QCUSTOMPLOT_LIBRARY})
endif(QCustomPlot_required)

if(ChartDirector_required)
    find_package(chartdir CONFIG REQUIRED)
    if(chartdir_FOUND)
        message(STATUS "Searching for ChartDirector - found")
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE chartdir)
    else()
        message(STATUS "Searching for ChartDirector - failed")
    endif()
endif(ChartDirector_required)

if(matlab_required)

    message(STATUS "linking matlab1")
    IF (MSVC)
        include("${OFEC_DIR}/FindMatlab.cmake")
        add_definitions("-DOFEC_MSVC")
        include("${OFEC_DIR}/matlab_win_subfix.cmake")
    else()
        message(STATUS "linking matlab")
        set(MATLAB_ROOT /user/local/Polyspace/R2020a)
        include("${OFEC_DIR}/FindMatlab.cmake")
        include_directories(${MATLAB_ROOT}/extern/include/)
        set(LIBMWM /user/local/Polyspace/R2020a/runtime/glnxa64/)
        include_directories( ${LIBMWM} )
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "$ENV{MATLAB_ROOT}/bin/glnxa64/libmwmclmcr.so")
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "$ENV{MATLAB_ROOT}/runtime/glnxa64/libmwmclmcrrt.so")
        add_definitions("-DOFEC_UNIX")
        include("${OFEC_DIR}/matlab_unix_subfix.cmake")
    endif()
    add_definitions("-DOFEC_MATLAB")
endif(matlab_required)

if(Qt_DataVisualization_required)   
    if(Qt6_FOUND OR Qt5_FOUND)
    message(STATUS "using Qt datavisualization")
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS DataVisualization)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DataVisualization)
    list(APPEND PROJECT_LIBRARIES Qt${QT_VERSION_MAJOR}::DataVisualization)
    add_definitions(-DOFEC_DEMO_QT_DATAVISUALIZATION)
    endif()
endif(Qt_DataVisualization_required)

if(Python_required)   
     add_definitions("-DOFEC_PYTHON")
    message(STATUS "Looking for this Python: $ENV{PYTHON_DIR}")
    set(Python3_ROOT_DIR  "$ENV{PYTHON_DIR}")
    unset(Python_EXECUTABLE)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${Python3_LIBRARIES})
endif(Python_required)


if(Boost_required)
#set(BOOST_ROOT C:/Boost)
#set(Boost_LIBRARY_DIRS C:/Boost/lib)
#set (BOOST_ROOT "C:/Boost") 
set(BOOST_ROOT D:/boost_lib)
set(CMAKE_PREFIX_PATH D:/boost_lib)

set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem iostreams)
MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
MESSAGE( STATUS "Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}.")
MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
# cmake ×¢ÒâÊÂÏî
#link_directories(${BOOST_ROOT}/lib)
#include_directories(${Boost_INCLUDE_DIRS})
 
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::filesystem Boost::iostreams)
#FILE(GLOB CPP_FILE0 ${PROJECT_SOURCE_DIR}/Boost-test/*.cpp)
#add_executable(Boost_test ${CPP_FILE0})


#find_package(Boost 1.84 REQUIRED COMPONENTS
#             filesystem iostreams)
#add_executable(foo foo.cc)
#target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Boost::filesystem
 #                         Boost::iostreams)
                           #target_link_libraries( ${CMAKE_PROJECT_NAME} PRIVATE Boost::filesystem
  #                        Boost::iostreams )

#set(Boost_NO_BOOST_CMAKE  ON)
#link_directories(${Boost_LIBRARY_DIRS})
#include_directories(${Boost_INCLUDE_DIRS})
 
 
#message(Boost\ version: ${Boost_VERSION_STRING})
#message(Boost\ include\ dir: ${Boost_INCLUDE_DIRS})
#message(Boost\ library\ dir: ${Boost_LIBRARY_DIRS})

#find_package(Boost 1.84 REQUIRED COMPONENTS
   #           filesystem iostreams)

    #target_link_libraries( ${CMAKE_PROJECT_NAME} PRIVATE Boost::filesystem
  #                        Boost::iostreams )



endif(Boost_required)
