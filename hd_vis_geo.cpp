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

    auto geometry = getGeom();
    gGeoManager->DefaultColors();


    TEveManager::Create();

    auto hallNode = gGeoManager->GetTopVolume()->FindNode("HALL_1");
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


    gEve->FullRedraw3D(kTRUE);
    gEve->EditElement(sv);

    RunRootApp();



}
