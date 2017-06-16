#include <thread>

#include <TFile.h>
#include <TApplication.h>

#include <TEveCalo.h>
#include <TEveWindow.h>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveScene.h>
#include <TEveViewer.h>
#include <TGeoManager.h>
#include <TEveGeoNode.h>
#include <TEveGedEditor.h>
#include <TGTab.h>
#include <iostream>
#include <TRint.h>
#include "TGDMLWrite.h"
#include "GeometryConverter.h"


using namespace std;

//TGeoManager * hddsroot();
TGeoManager * getGeom();

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


	// Create ROOT application 
	// Instantiate an event loop object

	gApp = new TApplication("Hahaha it works!", &narg, argv);

    new TGeoManager("GLUEX", "GlueX Geometry");

    gGeoManager=getGeom();
    //gEve = new TEveManager(800,600);

    //TEveGeoTopNode* enode = new TEveGeoTopNode(gGeoManager, gGeoManager->GetNode(0));
    //gEve->AddGlobalElement(enode);
    //enode->ExpandIntoListTreesRecursively();

    gGeoManager->Print();
    //gGeoManager->Export("/home/romanov/gluex/hdvis/github/halld.gdml");
    //TGDMLWrite::StartGDMLWriting(gGeoManager,"", "vg");
    hdvis::GeometryConverter converter;
    converter.ExtractVolumesNew(gGeoManager->GetTopNode(), gGeoManager->GetTopVolume());

    //gGeoManager->SetVisLevel(4);
    //gGeoManager->GetTopVolume()->Draw("ogle");
    //gEve->GetBrowser()->Draw();

    gApp->Run(false);

}
