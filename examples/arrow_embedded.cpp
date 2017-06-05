/// \file
/// \ingroup tutorial_eve
/// How to use EVE without the standard window.
/// Type
///   `gEve->GetBrowser()->MapWindow()`
/// to bring it up for object interaction, debugging, etc.
///
/// \image html eve_arrow_standalone.png
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
#include <TGLEmbeddedViewer.h>
#include <TEveViewer.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TEveArrow.h>
#include <TEveText.h>
#include <TEveTrans.h>

void arrow()
{
    TEvePointSet* marker = new TEvePointSet(8);
    marker->SetName("Origin marker");
    marker->SetMarkerColor(6);
    marker->SetMarkerStyle(3);
    Float_t a = 10;
    marker->SetPoint(0, a,  +a, +a);
    marker->SetPoint(1, a,  -a, +a);
    marker->SetPoint(2, -a, -a, +a);
    marker->SetPoint(3, -a, +a, +a);
    marker->SetPoint(4, +a, +a, -a);
    marker->SetPoint(5, +a, -a, -a);
    marker->SetPoint(6, -a, +a, -a);
    marker->SetPoint(7, -a, -a, -a);
    gEve->AddElement(marker);

    TEveArrow* a1 = new TEveArrow(1., 1., 10., 10., 4., 0.);
    a1->SetMainColor(kBlue);
    a1->SetTubeR(0.02);
    a1->SetPickable(kTRUE);
    gEve->AddElement(a1);
    TEveText* t1 = new TEveText("blue");
    t1->SetFontSize(20);
    TEveVector tv = a1->GetVector()*0.5f+a1->GetOrigin();
    t1->RefMainTrans().SetPos(tv.Arr());
    a1->AddElement(t1);

    TEveArrow* a2 = new TEveArrow(20., 1., 10., 3., 0., 4.);
    a2->SetMainColor(kGreen);
    a2->SetPickable(kTRUE);
    gEve->AddElement(a2);

    TEveArrow* a3 = new TEveArrow(1., 10., 10., 0., 20., 0.);
    a3->SetMainColor(kOrange);
    a3->SetPickable(kTRUE);
    gEve->AddElement(a3);

    gEve->FullRedraw3D(kTRUE);
}


void arrow_standalone()
{
    TEveManager::Create(kFALSE);

    // ----------------------------------------------------------------------

    TGMainFrame* mf = new TGMainFrame(gClient->GetRoot(), 800, 400,
                                      kHorizontalFrame);
    mf->SetWindowName("Arrow Foo");

    // ----------------------------------------------------------------------

    TGCompositeFrame* evf = new TGCompositeFrame(mf, 400, 400);
    mf->AddFrame(evf, new TGLayoutHints(kLHintsNormal  |
                                        kLHintsExpandX | kLHintsExpandY));

    TGLEmbeddedViewer* ev = new TGLEmbeddedViewer(evf);
    evf->AddFrame(ev->GetFrame(),
                  new TGLayoutHints(kLHintsNormal  |
                                    kLHintsExpandX | kLHintsExpandY));

    TEveViewer* eve_v = new TEveViewer("YourViewer");
    eve_v->SetGLViewer(ev, ev->GetFrame());
    eve_v->IncDenyDestroy();
    eve_v->AddScene(gEve->GetEventScene());
    gEve->GetViewers()->AddElement(eve_v);

    // ----------------------------------------------------------------------

    // To create embedded canvas ... no menus on top.

    // TRootEmbeddedCanvas* ec =
    //    new TRootEmbeddedCanvas("EmbeddedCanvas", mf, 400, 400);
    // mf->AddFrame(ec, new TGLayoutHints(kLHintsNormal  |
    //                                    kLHintsExpandX | kLHintsExpandY));

    // --------------------------------

    // This one is tricky - must be after embedded canvas but before std canvas!
    mf->MapSubwindows();

    // --------------------------------

    // To create full canvas with menus.

    mf->SetEditable();
    TCanvas* c = new TCanvas("Foo", "Bar", 400, 400);
    mf->SetEditable(kFALSE);

    // ----------------------------------------------------------------------

    mf->Layout();
    mf->MapWindow();

    // ----------------------------------------------------------------------

    // Populate the viewer ... here we just call the arrow.C.

    arrow();
}
int main(int narg, char* argv[])
{
    using namespace std;
    auto gApp = new TApplication("Hahaha it works!", &narg, argv);

    arrow_standalone();

    gApp->Run();
    return 0;
}