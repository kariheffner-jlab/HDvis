import sbms

# get env object and clone it
Import('*')
env = env.Clone()

sbms.AddDANA(env)
sbms.AddROOT(env)
#sbms.executable(env)

# Executable name comes from directory     name
exename = "hd_vis"

env.PrependUnique(CPPPATH = ['.'])

# Push commonly used libraries to end of list
sbms.ReorderCommonLibraries(env)

# Build program from all source
sources = ["hd_vis.cc", "JEventProcessor_EventReader.cc"]
myobjs = env.Object(sources)
myexe = env.Program(target = exename, source = myobjs)

# Cleaning and installation are restricted to the directory
# scons was launched from or its descendents
CurrentDir = env.Dir('.').srcnode().abspath
if not CurrentDir.startswith(env.GetLaunchDir()):
	# Not in launch directory. Tell scons not to clean these targets
    env.NoClean([myobjs, myexe])
else:
	# We're in launch directory (or descendent) schedule installation

	# Installation directories for executable and headers
    installdir = env.subst('$INSTALLDIR')
    includedir = env.subst('$INCDIR')
    bindir = env.subst('$BINDIR')

    # Install targets 
    env.Install(bindir, myexe)




