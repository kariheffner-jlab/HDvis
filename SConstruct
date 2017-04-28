import os
 
jana_home = os.environ['JANA_HOME']
BMS_OS_Name= os.environ['BMS_OSNAME']
jana_include = os.path.join(jana_home, 'include')
hdpmgx_top = os.environ['GLUEX_TOP']
jana_lib = os.path.join(jana_home, 'lib')
print(jana_include)
print(jana_lib)

jana_plugin_path = os.environ['JANA_PLUGIN_PATH']
print(jana_plugin_path)

simrecon_include =os.path.join(hdpmgx_top, "sim-recon/master/", BMS_OS_Name, 'include')
sim_recon_lib = os.path.join(hdpmgx_top, "sim-recon/master/", BMS_OS_Name, 'lib')

ccdb_home = os.environ['CCDB_HOME']
ccdb_lib = os.path.join(ccdb_home, 'lib')
print(simrecon_include)
print()
print()
# SConstruct
env = Environment(CPPPATH=[jana_include, simrecon_include], CXXFLAGS="-std=c++11")

print(WhereIs('root-config'))

env.ParseConfig(WhereIs('root-config') +' --cflags --libs')
#env.ParseConfig('mysql_config --libs --cflags')
libs = [

#sim-recon
'libDANA.a',
'libTTAB.a',
'libAMPTOOLS_AMPS.a',
'libAMPTOOLS_DATAIO.a',
'libAMPTOOLS_MCGEN.a',
'libANALYSIS.a',
'libBCAL.a',
'libCCAL.a',
'libCDC.a',
'libCERE.a',
'libDAQ.a',
'libDIRC.a',
'libEVENTSTORE.a',
'libFCAL.a',
'libFDC.a',
'libFMWPC.a',
'libgelhad.a',
'libHDDM.a',
'libHDGEOMETRY.a',
'libhitutil.a',
'libKINFITTER.a',
'libPAIR_SPECTROMETER.a',
'libPID.a',
'libRF.a',
'libSTART_COUNTER.a',
'libTAGGER.a',
'libTOF.a',
'libTPOL.a',
'libTRACKING.a',
'libTRIGGER.a',

'libxstream.a',

#system
'libdl.a',


#JANA
'libJANA.a',

'libevio.so',
'libevioxx.so',

#ROOT of CERN
#'libASImageGui.so',
#'libASImage.so',
#'libCling.so',
#'libcomplexDict.so',
'libCore.so',
#'libdequeDict.so',
#'libEG.so',
#'libEve.so',
#'libFFTW.so',
#'libFitPanel.so',
#'libFoam.so',
#'libforward_listDict.so',
#'libFTGL.so',
#'libFumili.so',
#'libGed.so',
#'libGenetic.so',
#'libGenVector.so',
#'libGeomBuilder.so',
#'libGeomPainter.so',
'libGeom.so',
#'libGLEW.so',
'libGpad.so',
'libGraf3d.so',
'libGraf.so',
#'libGuiBld.so',
#'libGuiHtml.so',
#'libGui.so',
#'libGviz3d.so',
#'libGX11.so',
#'libGX11TTF.so',
#'libHbook.so',
#'libHistPainter.so',
'libHist.so',
#'libHtml.so',
#'libJupyROOT.so',
#'liblistDict.so',
#'libmap2Dict.so',
#'libmapDict.so',
'libMathCore.so',
'libMatrix.so',
#'libMemStat.so',
'libMinuit.so',
'libMLP.so',
#'libmultimap2Dict.so',
#'libmultimapDict.so',
#'libMultiProc.so',
#'libmultisetDict.so',
'libNet.so',
#'libNew.so',
'libPhysics.so',
#'libPostscript.so',
#'libProofBench.so',
#'libProofDraw.so',
#'libProofPlayer.so',
#'libProof.so',
#'libPyROOT.so',
#'libQuadp.so',
#'libRecorder.so',
#'libRGL.so',
#'libRint.so',
'libRIO.so',
#'libRMySQL.so',
#'libRooFitCore.so',
#'libRooFit.so',
#'libRooStats.so',
#'libRootAuth.so',
#'libRSQLite.so',
#'libSessionViewer.so',
#'libsetDict.so',
#'libSmatrix.so',
#'libSpectrumPainter.so',
#'libSpectrum.so',
#'libSPlot.so',
#'libSQLIO.so',
'libThread.so',
#'libTMVAGui.so',
'libTMVA.so',
'libTreePlayer.so',
'libTree.so',
'libTreeViewer.so',
#'libunordered_mapDict.so',
#'libunordered_multimapDict.so',
#'libunordered_multisetDict.so',
#'libunordered_setDict.so',
#'libvalarrayDict.so',
#'libvectorDict.so',
#'libVMC.so',
#'libX3d.so',
'libXMLIO.so',


# xerces
'libxerces-c-3.1.so',

# ccdb
'libccdb.a',


#system
'libdl.a',
'libz.a',
'libm.a',
#'libMySql.a',
'mysqlclient', 
'pthread',
'z',
'm',
'rt',
'dl',
#'bzip',


]

gluex_top = os.environ['GLUEX_TOP']

lib_pathes = [
    sim_recon_lib,
    gluex_top + '/root/6.08.06/lib', 
    jana_lib,
    gluex_top + '/xerces-c/3.1.4/lib/',
    gluex_top + '/evio/4.4.6/Linux-x86_64/lib',
    ccdb_lib
]

hello = env.Program(["main.cpp", "EventReader/JEventProcessor_EventReader.cc"], LIBS=libs, LIBPATH=lib_pathes)