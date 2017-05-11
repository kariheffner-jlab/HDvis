#include <thread>

#include <TFile.h>
#include <TApplication.h>

#include <TSystem.h>
#include <TEveCalo.h>
#include <TEveWindow.h>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveProjectionAxes.h>
#include <TEveScene.h>
#include <TEveViewer.h>
#include <TGeoManager.h>
#include <TEveGeoNode.h>
#include <TEveGedEditor.h>
#include <TGTab.h>
#include "EventReader/JEventProcessor_EventReader.h"
#include "DANA/DApplication.h"



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

TApplication *gApp;

void RunRootApp()
{
    gApp->Run();
}


//-----------
// main
//-----------
int main(int narg, char *argv[])
{
	// Parse the command line
	ParseCommandLineArguments(narg, argv);

	// Create ROOT application 
	// Instantiate an event loop object
	DApplication app(narg, argv);

	gApp = new TApplication("Hahaha it works!", &narg, argv);

    new TGeoManager("GLUEX", "GlueX Geometry");

    auto geometry = hddsroot();
    gGeoManager->DefaultColors();


    TEveManager::Create();

    TGeoNode* hallNode = (TGeoNode *) gGeoManager->GetTopVolume()->FindNode("HALL_1");
    cout<<"hallNode is "<<hallNode<<endl;


    TGeoNode* fcalNode = (TGeoNode *) hallNode->GetNodes()->FindObject("FCAL_1");
    cout<<"fcalNode is "<<fcalNode<<endl;




    gEve->AddGlobalElement(new TEveGeoTopNode(gGeoManager, hallNode));
    gEve->AddGlobalElement(new TEveGeoTopNode(gGeoManager, fcalNode));

    TEveWindowSlot* slot = 0;
    slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());

    TEveViewer* sv = new TEveViewer("Stereo GL", "Stereoscopic view");
    sv->SpawnGLViewer(gEve->GetEditor(), kTRUE, false);
    sv->AddScene(gEve->GetGlobalScene());

    slot->ReplaceWindow(sv);

    gEve->GetViewers()->AddElement(sv);

    gEve->GetBrowser()->GetTabRight()->SetTab(1);

    // --- Redraw ---



    //RunRootApp();

	// Instantiate our event processor
    auto myproc = new JEventProcessor_EventReader();
	myproc->setRootApplication(gApp);
    myproc->setCanvas(gEve->AddCanvasTab("FCAL histogram"));

    gEve->FullRedraw3D(kTRUE);
    gEve->EditElement(sv);

    std::thread t1(RunRootApp);


	// Decide on the output filename
	DecideOutputFilename();
	
	// Run though all events, calling our event processor's methods
	app.monitor_heartbeat = 0;
	app.Run(myproc);
	
	return app.GetExitCode();

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
// DecideOutputFilename
//-----------
void DecideOutputFilename(void)
{
	/// Decide on the output filename to use based on the command line
	/// input and configuration parameter input. The command line takes
	/// precedence. This also makes sure to copy the filename that is
	/// being used into the configuration parameter.

	// Set the default output filename (avoids later warnings from JANA)
	gPARMS->SetDefaultParameter("OUTPUT_FILENAME", OUTPUT_FILENAME,"Output filename used by hd_root");
	
	// If the user specified an output filename on the command line,
	// use it to overwrite the config. parameter/default one
	if(filename_from_command_line){
		OUTPUT_FILENAME = COMMAND_LINE_OUTPUT_FILENAME;
	
		// Set the actual output filename in config. param.
		gPARMS->SetParameter("OUTPUT_FILENAME", OUTPUT_FILENAME);
	}

	jout<<"OUTPUT_FILENAME: "<<OUTPUT_FILENAME<<endl;

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
