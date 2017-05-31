#include <thread>

#include <TFile.h>
#include <TApplication.h>

#include <TSystem.h>
#include <TEveCalo.h>
#include <TEveWindow.h>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveScene.h>
#include <TEveViewer.h>
#include <TGeoManager.h>
#include <TEveGedEditor.h>
#include <TGTab.h>
#include "EventReader/JEventProcessor_EventReader.h"
#include "DANA/DApplication.h"
#include <TEveGeoNode.h>
#include <mutex>
#include <GuiListener.h>
#include <functional>

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
DApplication *gDana;


std::mutex gEventMutex;     /// Mutex between ROOT graphical thread and EvenRead function
GuiListener gGuiListener;


TGTextButton *fPrevEventButton;

TGTextButton *fDrawEventButton;

TGTextButton *fNextEventButton;

TGCheckButton *fShowFitsButton;

TGCheckButton *fShowTrajectoriesButton;

TGCheckButton *fShowG4HitsButton;

TGTextButton *fDrawXDigitsButton;

void RunRootApp()
{
    // Here is what happens in gApp->Run();
    // 3 root functions should be called in this order:
    // gApplication->StartIdleing() -> gSystem->InnerLoop() -> gApplication->StartIdleing()
    // In this function we want to pause ROOT graphical thread, run EventRead, continue ROOT graph. thread
    // In order to do it we use gEventMutex

    gApplication->StartIdleing();

    while (1) {

        while(!gEventMutex.try_lock()) std::this_thread::yield();

            std::lock_guard<std::mutex> eventMutexLockGuard(gEventMutex, std::adopt_lock);

            gSystem->InnerLoop();
            gApplication->StopIdleing();
            gApplication->StartIdleing();

    }
}



void MakeControlTab() {
	TEveBrowser* browser = gEve->GetBrowser();

    auto toolBarFrame = browser->GetToolbarFrame();
    toolBarFrame->SetHeight(200);
    ((TGVerticalFrame*)toolBarFrame->GetParent())->ShowFrame(toolBarFrame);


	// Define the generic layout.  The last four parameters are the padding
	// around the widgets.
	TGLayoutHints* layoutHints
			= new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,
								2, 2, 2, 2);

	// Embed a new frame in the event browser to have control buttons.
	browser->StartEmbedding(TRootBrowser::kLeft);
	TGMainFrame* mainFrame = new TGMainFrame(gClient->GetRoot(), 1000, 600);
	mainFrame->SetWindowName("Event Control");
	mainFrame->SetCleanup(kDeepCleanup);

	TGVerticalFrame* hf = new TGVerticalFrame(mainFrame);
	TGTextButton* textButton;

	// The general action buttons.
	textButton = new TGTextButton(hf, "Previous Event");
	textButton->SetToolTipText("Go to previous event.");
	textButton->SetTextJustify(36);
	textButton->SetMargins(0,0,0,0);
	textButton->SetWrapLength(-1);
	hf->AddFrame(textButton, layoutHints);
	fPrevEventButton = textButton;

	textButton = new TGTextButton(hf, "Redraw Event");
	textButton->SetToolTipText("Refresh the current view.");
	textButton->SetTextJustify(36);
	textButton->SetMargins(0,0,0,0);
	textButton->SetWrapLength(-1);
	hf->AddFrame(textButton, layoutHints);
	fDrawEventButton = textButton;

	textButton = new TGTextButton(hf, "Next Event");
	textButton->SetToolTipText("Go to previous event.");
	textButton->SetTextJustify(36);
	textButton->SetMargins(0,0,0,0);
	textButton->SetWrapLength(-1);
	hf->AddFrame(textButton, layoutHints);
	fNextEventButton = textButton;

	// Create the buttons to select which types of objects are showed.
	TGCheckButton *checkButton;

	checkButton = new TGCheckButton(hf,"Show Recon Objects");
	checkButton->SetToolTipText(
			"Show the reconstructed objects.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	checkButton->SetOn();
	hf->AddFrame(checkButton, layoutHints);
	fShowFitsButton = checkButton;

	checkButton = new TGCheckButton(hf,"Show Trajectories");
	checkButton->SetToolTipText(
			"Show the GEANT4 trajectories and trajectory points.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	hf->AddFrame(checkButton, layoutHints);
	fShowTrajectoriesButton = checkButton;

	checkButton = new TGCheckButton(hf,"Show G4 Hits");
	checkButton->SetToolTipText(
			"Show the GEANT4 hits.  This shows the energy deposition in the "
					"defined sensitive detectors.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	hf->AddFrame(checkButton, layoutHints);
	fShowG4HitsButton = checkButton;

	/////////////////////
	// Buttons to draw the digits.
	/////////////////////
	textButton = new TGTextButton(hf, "Draw X Digits");
	fDrawXDigitsButton = textButton;
	textButton->SetTextJustify(36);
	textButton->SetMargins(0,0,0,0);
	textButton->SetWrapLength(-1);
	hf->AddFrame(textButton, layoutHints);
	textButton->SetToolTipText("Draw the X digits vs wire number."
									   "  Also show the X hits.");

	/*

	textButton = new TGTextButton(hf, "Draw V Digits");
	fDrawVDigitsButton = textButton;
	textButton->SetTextJustify(36);
	textButton->SetMargins(0,0,0,0);
	textButton->SetWrapLength(-1);
	hf->AddFrame(textButton, layoutHints);
	textButton->SetToolTipText("Draw the V digits vs wire number."
									   "  Also show the V hits.");

	textButton = new TGTextButton(hf, "Draw U Digits");
	fDrawUDigitsButton = textButton;
	textButton->SetTextJustify(36);
	textButton->SetMargins(0,0,0,0);
	textButton->SetWrapLength(-1);
	hf->AddFrame(textButton, layoutHints);
	textButton->SetToolTipText("Draw the U digits vs wire number."
									   "  Also show the U hits.");

	checkButton = new TGCheckButton(hf,"Show raw digits");
	checkButton->SetToolTipText(
			"Show the raw, not calibrated digits.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	hf->AddFrame(checkButton, layoutHints);
	fShowRawDigitsButton = checkButton;

	/////////////////////
	// Buttons to control how reconstruction objects are drawn.
	/////////////////////

	checkButton = new TGCheckButton(hf,"Show Recon Object Hits");
	checkButton->SetToolTipText(
			"Show the hits associated with the draw reconstructed objects.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	hf->AddFrame(checkButton, layoutHints);
	fShowFitsHitsButton = checkButton;

	checkButton = new TGCheckButton(hf,"Show Recon Object Clusters");
	checkButton->SetToolTipText(
			"Show the constituent clusters for draw reconstructed objects.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	hf->AddFrame(checkButton, layoutHints);
	fShowConstituentClustersButton = checkButton;

	checkButton = new TGCheckButton(hf,"Show cluster uncertainty, not moments");
	checkButton->SetToolTipText(
			"Show the cluster uncertainty, not the cluster charge moments.");
	checkButton->SetTextJustify(36);
	checkButton->SetMargins(0,0,0,0);
	checkButton->SetWrapLength(-1);
	hf->AddFrame(checkButton, layoutHints);
	fShowClusterUncertaintyButton = checkButton;
	 */

	// Do the final layout and mapping.
	mainFrame->AddFrame(hf, layoutHints);
	mainFrame->MapSubwindows();
	mainFrame->Resize();
	mainFrame->MapWindow();
	browser->StopEmbedding();
	browser->SetTabTitle("Control", 0);
}

//______________________________________________________________________________
void make_gui()
{
    // Create minimal GUI for event navigation.

    TEveBrowser* browser = gEve->GetBrowser();
    //browser->StartEmbedding(TRootBrowser::kLeft);

    auto toolBarFrame = browser->GetToolbarFrame();
    toolBarFrame->SetHeight(200);
    ((TGVerticalFrame*)toolBarFrame->GetParent())->ShowFrame(toolBarFrame);



    TGHorizontalFrame* hf = (TGHorizontalFrame *) toolBarFrame; //new TGHorizontalFrame(frmMain);
    {

        TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")));
        TGPictureButton* b = 0;

        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "GuiListener", &gGuiListener, "OnNextButtonPressed()");


        auto checkButton = new TGCheckButton(hf,"Autoplay");
        hf->AddFrame(checkButton, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY,20,2,2,2));
        checkButton->SetToolTipText("Autoplay events one by one with some interval");
        checkButton->SetTextJustify(36);
        checkButton->SetMargins(30,0,5,0);
        checkButton->Connect("Toggled(Bool_t)", "GuiListener", &gGuiListener, "OnAutoplayCheckedChanged(Bool_t)");

    }

    ((TGVerticalFrame*)toolBarFrame->GetParent())->MapSubwindows();
    ((TGVerticalFrame*)toolBarFrame->GetParent())->Resize();
    ((TGVerticalFrame*)toolBarFrame->GetParent())->MapWindow();
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
	DApplication dana(narg, argv);
    gDana=&dana;

	gApp = new TApplication("Hahaha it works!", &narg, argv);

    new TGeoManager("GLUEX", "GlueX Geometry");

    auto geometry = hddsroot();
    gGeoManager->DefaultColors();

    TEveManager::Create();
	MakeControlTab();
    make_gui();

    TGeoNode* hallNode = (TGeoNode *) gGeoManager->GetTopVolume()->FindNode("HALL_1");
    //hallNode->Draw();
    //cout<<"hallNode is "<<hallNode<<endl;
    cout<<hallNode->GetMotherVolume()<<endl;

    TGeoNode* fcalNode = (TGeoNode *) hallNode->GetNodes()->FindObject("FCAL_1");
    //cout<<"fcalNode is "<<fcalNode<<endl;

    TGeoNode* tofNode = (TGeoNode *) hallNode->GetNodes()->FindObject("FTOF_1");
    cout<<"tofNode is "<<tofNode<<endl;
    //TGeoTranslation shift(-150,350,500);

    //add independent
    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume *topVolume = gGeoManager->MakeBox("TOP", Vacuum, 1000, 1000., 1000.);
    topVolume->AddNode(hallNode->GetVolume(),2,new TGeoTranslation(-150.501,349.986,500));
    //topVolume->AddNode(hallNode->GetVolume(),2,new TGeoTranslation(0,0,0));

    TGeoNode* Det5Node = (TGeoNode *) hallNode->GetNodes()->FindObject("DET5_1");
    Det5Node->SetVisibility(0);

    TGeoNode* Det6Node = (TGeoNode *) hallNode->GetNodes()->FindObject("DET6_1");
    Det6Node->SetVisibility(0);


    TGeoNode* MagNode1 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("IYUP_1");
    MagNode1->SetVisibility(0);

    TGeoNode* MagNode2 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("IYOK_1");
    MagNode2->SetVisibility(0);

    TGeoNode* MagNode3 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("IYDN_1");
    MagNode3->SetVisibility(0);
/*
    TGeoNode* BarrelCap =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("BCSU_1");
    BarrelCap->SetVisibility(0);

    TGeoNode* BarrelCap2 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("FDQ4_1");
    BarrelCap2->SetVisibility(0);

    TGeoNode* BarrelCap3 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("FDQ3_1");
    BarrelCap3->SetVisibility(0);

    TGeoNode* BarrelCap4 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("FDQ2_1");
    BarrelCap4->SetVisibility(0);

    TGeoNode* BarrelCap5 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("FDQ1_1");
    BarrelCap5->SetVisibility(0);
*/

    //The lines below actually crash the first event on track draw....for some reason.  May need a copy of the geometry seperately....
    /*
    //TGeoNode* BCSDNone =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("BCSD_1");
    //BCSDNone->SetVisibility(0);

    //TGeoNode* CDCBcab =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("CDCB_1");
    //CDCBcab->SetVisibility(0);

    //TGeoNode* BCALCab2 =(TGeoNode*) ((TGeoNode *) hallNode->GetNodes()->FindObject("LASS_1"))->GetNodes()->FindObject("CDCB_1");
   // BCALCab2->SetVisibility(0);
*/


    //cout<<"fcalNode is "<<fcalNode<<endl;

    gGeoManager->Print();

    gGeoManager->SetTopVolume(topVolume);


    gEve->AddGlobalElement(new TEveGeoTopNode(gGeoManager, gGeoManager->GetNode(0)));
    //gEve->AddGlobalElement(new TEveGeoTopNode(gGeoManager,hallNode));
	//gEve->AddGlobalElement(new TEveGeoTopNode(gGeoManager, fcalNode));
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
    auto myproc = new JEventProcessor_EventReader(hallNode);
	myproc->setRootApplication(gApp);
    myproc->setCanvas(gEve->AddCanvasTab("FCAL histogram"));

    // Wire GUI buttons to myproc actions
    gGuiListener.WhenNextEventFired = [myproc](){
        std::cout<<"WOOOOOOOOOOORRRRRKKKKKKSSSSSS AAAAAA !!!!!!!!!11111oneoneone"<<std::endl;
        myproc->ProceedToNextEvent();
    };

    gGuiListener.WhenAutoplayCheckedChanged = [myproc](bool t){
        std::cout<<"Autoplay changed to: "<<t<<std::endl;
        myproc->SetAutoPlay(t);
    };

    gEve->FullRedraw3D(kTRUE);
    gEve->EditElement(sv);

    std::thread t1(RunRootApp);

    //RunRootApp();
	// Decide on the output filename
	DecideOutputFilename();
	
	// Run though all events, calling our event processor's methods
	dana.monitor_heartbeat = 0;
	dana.Run(myproc);

	return dana.GetExitCode();

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
