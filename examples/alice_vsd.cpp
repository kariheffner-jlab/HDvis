/// \file
/// \ingroup tutorial_eve
/// Complex example showing ALICE VSD visualization.
///
///  alice_vsd.C - a simple event-display for ALICE
///
///  Only standard ROOT is used to process the ALICE VSD files.
///
///  No ALICE code is needed -- the VSD file is exported from AliRoot into
///  VSD format -- see TEveVSDStructs.h and TEveVSD.h.
///
///  A simple geometry of 10KB, extracted from the full TGeo-geometry, is
///  used to outline the central detectors of ALICE.
///
///  All files are access from the web by using the "CACHEREAD" option.
///
/// \image html eve_alice_vsd.png
/// \macro_code
///
/// \author Matevz Tadel


#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveVSD.h>
#include <TEveVSDStructs.h>

#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TEveGeoShape.h>

#include <TGTab.h>
#include <TGButton.h>

#include <TFile.h>
#include <TKey.h>
#include <TSystem.h>
#include <TPRegexp.h>

#include "MultiView.h"
#include "TVSDReader.h"


// Include componets -- compile time link :)

/// \file
/// \ingroup tutorial_eve
/// Multi-view (3d, rphi, rhoz) service class using EVE Window Manager.
///
/// \macro_code
///
/// \author Matevz Tadel

#include <TEveManager.h>

#include <TEveViewer.h>
#include <TGLViewer.h>

#include <TEveScene.h>

#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>

#include <TEveBrowser.h>
#include <TEveWindow.h>
#include <TApplication.h>


MultiView* gMultiView = 0;

TVSDReader* gVSDReader = 0;


// Forward declaration.
void make_gui();

//______________________________________________________________________________
void alice_vsd(const char* vsd_file_name=
"http://mtadel.home.cern.ch/mtadel/root/AliVSD.root")
{
    // Main function, initializes the application.
    //
    // 1. Load the auto-generated library holding ESD classes and
    //    ESD dictionaries.
    // 2. Open ESD data-files.
    // 3. Load cartoon geometry.
    // 4. Spawn simple GUI.
    // 5. Load first event.

    TFile::SetCacheFileDir(".");

    TEveVSD::DisableTObjectStreamersForVSDStruct();

    gVSDReader = new TVSDReader((char *) vsd_file_name);

    TEveManager::Create();

    TEveGeoShape *gentle_geom = 0;

    { // Simple geometry
        TFile* geom =
                TFile::Open("http://mtadel.home.cern.ch/mtadel/root/alice_mini_geom.root",
                            "CACHEREAD");
        if (!geom)
            return;
        TEveGeoShapeExtract* gse = (TEveGeoShapeExtract*) geom->Get("Gentle");
        gentle_geom = TEveGeoShape::ImportShapeExtract(gse, 0);
        geom->Close();

        delete geom;
        gEve->AddGlobalElement(gentle_geom);
    }


    // Standard multi-view
    //=====================

    gMultiView = new MultiView;
    gMultiView->f3DView->GetGLViewer()->SetStyle(TGLRnrCtx::kOutline);

    gMultiView->SetDepth(-10);
    gMultiView->ImportGeomRPhi(gentle_geom);
    gMultiView->ImportGeomRhoZ(gentle_geom);
    gMultiView->SetDepth(0);


    // Final stuff
    //=============

    gEve->GetViewers()->SwitchColorSet();
    gEve->GetDefaultGLViewer()->SetStyle(TGLRnrCtx::kOutline);

    gEve->GetBrowser()->GetTabRight()->SetTab(1);

    make_gui();

    gEve->AddEvent(new TEveEventManager("Event", "ALICE VSD Event"));

    gVSDReader->GotoEvent(0);

    gEve->Redraw3D(kTRUE); // Reset camera after the first event has been shown.
}


//______________________________________________________________________________
void make_gui()
{
    // Create minimal GUI for event navigation.

    TEveBrowser* browser = gEve->GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);

    TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    frmMain->SetWindowName("XX GUI");
    frmMain->SetCleanup(kDeepCleanup);

    TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
    {
        TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")));
        TGPictureButton* b = 0;

        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "TVSDReader", gVSDReader, "PrevEvent()");

        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "TVSDReader", gVSDReader, "NextEvent()");
    }
    frmMain->AddFrame(hf);

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Control", 0);
}

int main(int narg, char* argv[])
{
    using namespace std;
    auto gApp = new TApplication("Hahaha it works!", &narg, argv);

    alice_vsd();

    gApp->Run();
    return 0;
}
