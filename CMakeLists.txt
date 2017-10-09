cmake_minimum_required(VERSION 2.8)

project(hdvis C CXX)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

set(CMAKE_CXX_STANDARD 11)



#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO RGL Hist Tree Net Core Geom Eve TMVA Gui)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})


link_directories(
        $ENV{JANA_HOME}/lib
        $ENV{HALLD_HOME}/$ENV{BMS_OSNAME}/lib
        $ENV{EVIOROOT}/lib
        $ENV{CCDB_HOME}/lib
        $ENV{XERCESCROOT}/lib/
        /usr/lib64/mysql/
)

add_definitions(${ROOT_CXX_FLAGS})


#---Create a shared library with geneated dictionary

include_directories(
        $ENV{JANA_HOME}/include
        $ENV{HALLD_HOME}/$ENV{BMS_OSNAME}/include
        EventReader
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

#geometry.cpp EventReader/Tracking.h EventReader/FCAL.h
set(SOURCE_FILES
        hd_vis.cpp
        EventReader/JEventProcessor_EventReader.cpp
        EventReader/TOF.h
        EventReader/BCAL.h
        EventReader/CDC.h
        EventReader/SC.h
        EventReader/FDC.h


        WaitingLogic.h
        ControlLoop.h
        ApplicationContext.h
        ApplicationContext.cpp

        httplib.h
        HttpController.h
        KeyboardControl.h
        StringUtils.h

        tao/json.hpp
        )


add_executable(hdvis ${SOURCE_FILES})


#AMPTOOLS_AMPS
#AMPTOOLS_DATAIO
#AMPTOOLS_MCGEN
# gelhad
#        rt
#hitutil
target_link_libraries(hdvis

        #sim-recon
        DANA
        ANALYSIS
        KINFITTER
        PID
        TAGGER
        TRACKING
        START_COUNTER
        CERE
        DIRC
        CDC
        TRIGGER
        PAIR_SPECTROMETER
        RF
        FDC
        TOF
        BCAL
        FCAL
        CCAL
        TPOL
        HDGEOMETRY
        TTAB
        FMWPC
        TAC
        DAQ
        JANA
        EVENTSTORE


        HDDM
        xstream

        ${ROOT_LIBRARIES}



        #JANA

        evio
        evioxx

        # xerces
        xerces-c-3.1

        # ccdb
        ccdb

        #xml parsing lib
        expat

        #system
        dl
        z
        m
        bz2

        mysqlclient
        pthread

        DANA


        )

add_custom_command(TARGET hdvis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/js
        $<TARGET_FILE_DIR:hdvis>/www
        )
