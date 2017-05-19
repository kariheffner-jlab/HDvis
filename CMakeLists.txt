cmake_minimum_required(VERSION 3.1)

project(EveStandAlone C CXX)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})



#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO RGL Hist Tree Net Core Geom Eve TMVA)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})


link_directories(
        $ENV{JANA_HOME}/lib
        $ENV{HALLD_HOME}/$ENV{BMS_OSNAME}/lib
        $ENV{EVIOROOT}/lib
        $ENV{CCDB_HOME}/lib
        $ENV{XERCESCROOT}/lib/
        /group/halld/Software/builds/Linux_CentOS7-x86_64-gcc4.8.5/xerces-c/xerces-c-3.1.2/lib/
        /usr/lib64/mysql/
)


add_definitions(${ROOT_CXX_FLAGS})
#ROOT_GENERATE_DICTIONARY(G__Event Event.h LINKDEF EventLinkDef.h)
#---Create a shared library with geneated dictionary
#add_library(Event SHARED Event.cxx G__Event.cxx)


include_directories(
        $ENV{JANA_HOME}/include
        $ENV{HALLD_HOME}/$ENV{BMS_OSNAME}/include
        EventReader
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(SOURCE_FILES
        hd_vis.cpp
        geometry.cpp
        EventReader/JEventProcessor_EventReader.cpp

        )
set(GEO_SOURCE_FILES
        hd_vis_geo.cpp
        geometry.cpp

        )

add_executable(EveStandAlone ${SOURCE_FILES})
add_executable(EveGeo ${GEO_SOURCE_FILES})

target_link_libraries(EveGeo
        ${ROOT_LIBRARIES}

)

target_link_libraries(EveStandAlone

        #sim-recon
        DANA
        TTAB
        AMPTOOLS_AMPS
        AMPTOOLS_DATAIO
        AMPTOOLS_MCGEN
        ANALYSIS
        BCAL
        CCAL
        CDC
        CERE
        DAQ
        DIRC
        EVENTSTORE
        FCAL
        FDC
        FMWPC
        gelhad
        HDDM

        hitutil
        KINFITTER
        PAIR_SPECTROMETER
        PID
        RF
        START_COUNTER
        TAGGER
        TOF
        TPOL
        TRACKING
        HDGEOMETRY
        TRIGGER
        xstream

        ${ROOT_LIBRARIES}



        #JANA
        JANA
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
        rt

        )

add_subdirectory(examples)