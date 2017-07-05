#include <thread>

#include "EventReader/JEventProcessor_EventReader.h"
#include "DANA/DApplication.h"
#include <mutex>
#include <functional>
#include <TFile.h>
#include "ControlLoop.h"
#include <HttpServerController.h>

using namespace std;

TGeoManager * hddsroot();
//TGeoManager * getGeom();

typedef void SetTFilePtrAddress_t(TFile **);
TFile* tfilePtr = NULL;
string OUTPUT_FILENAME = "hd_root.root";
string COMMAND_LINE_OUTPUT_FILENAME = "";
bool filename_from_command_line = false;

void ParseCommandLineArguments(int &narg, char *argv[]);
void DecideOutputFilename(void);
void Usage(void);


DApplication *gDana;
hdvis::ControlLoop gControlLoop;


//-----------
// main
//-----------
int main(int narg, char *argv[])
{
	// Parse the command line
	ParseCommandLineArguments(narg, argv);

	// Create ROOT application 
	// Instantiate an event loop object
    //new TGeoManager("GLUEX", "GlueX Geometry");
	DApplication dana(narg, argv);
    gDana=&dana;

	// Instantiate our event processor
    auto myproc = new JEventProcessor_EventReader(gControlLoop.Context());

    HttpServerController srv;
    srv.StartListening();

    gControlLoop.RunRootAppMultithreaded();
    //gControlLoop.RunRootAppThisThread();



    // Run though all events, calling our event processor's methods
    dana.monitor_heartbeat = false;
    dana.Run(myproc);
	return dana.GetExitCode();



    return 0;

}


//-----------
// ParseCommandLineArguments
//-----------
void ParseCommandLineArguments(int &narg, char *argv[])
{
	if(narg==1)Usage();
	
	for(int i=1;i<narg;i++){
		if(argv[i][0] != '-')continue;
		switch(argv[i][1]){
			case 'h':
				Usage();
				break;
			case 'D':
				toprint.push_back(&argv[i][2]);
				break;
			case 'A':
				ACTIVATE_ALL = 1;
			case 'o':
				if(i>=narg-1){
					cerr<<"\"-o\" requires a filename!"<<endl;
					exit(-1);
				}
				COMMAND_LINE_OUTPUT_FILENAME = argv[i+1];
				filename_from_command_line = true;
				
				// Remove the "-o fname" arguments from file list so
				// JANA won't think the "fname" is an input file.
				for(int j=i; j<(narg-2); j++)argv[j] = argv[j+2];
				narg -= 2;
				break;
		}
	}
}


//-----------
// Usage
//-----------
void Usage(void)
{
	// Make sure a JApplication object exists so we can call Usage()
	JApplication *app = japp;
	if(app == NULL) app = new DApplication(0, NULL);

	cout<<"Usage:"<<endl;
	cout<<"       hd_root [options] source1 source2 ..."<<endl;
	cout<<endl;
	cout<<"Process events from a Hall-D data source (e.g. a file)"<<endl;
	cout<<"This will create a ROOT file that plugins or debug histos"<<endl;
	cout<<"can write into."<<endl;
	cout<<endl;
	cout<<"Options:"<<endl;
	cout<<endl;
	app->Usage();
	cout<<endl;
	cout<<"   -h        Print this message"<<endl;
	cout<<"   -Dname    Activate factory for data of type \"name\" (can be used multiple times)"<<endl;
	cout<<"   -A        Activate factories (overrides and -DXXX options)"<<endl;
	cout<<"   -o fname  Set output filename (default is \"hd_root.root\")"<<endl;
	cout<<endl;

	exit(0);
}
