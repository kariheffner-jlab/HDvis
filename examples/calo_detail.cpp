/// \file
/// \ingroup tutorial_eve
/// Calorimeter detailed view by using TEveCaloDataVec as data-source.
/// Demonstrates how to plot calorimeter data with irregular bins.
///
/// \image html eve_calo_detail.png
/// \macro_code
///
/// \author Alja Mrak-Tadel

#include <TApplication.h>
#include "TEveProjections.h"
#include "TSystem.h"
#include "TEveCalo.h"
#include "TEveWindow.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveProjectionAxes.h"
#include "TEveScene.h"
#include "TEveViewer.h"
#include "TEveTrans.h"
#include "TEveCaloLegoOverlay.h"
#include "TEveLegoEventHandler.h"
#include "TEveGedEditor.h"
#include "TEveJetCone.h"

#include "TGLWidget.h"
#include "TGLViewer.h"
#include "TGTab.h"

#include "TFile.h"
#include "TAxis.h"
#include "TH2.h"


const char* histFile =
        "http://amraktad.web.cern.ch/amraktad/cms_calo_hist.root";

TEveCaloLego* MakeCaloLego(TEveCaloData* data, TEveWindowSlot* slot);
TEveCalo2D* MakeCalo2D(TEveCalo3D* calo3d, TEveWindowSlot* slot, TEveProjection::EPType_e t);
TEveCalo3D* MakeCalo3D(TEveCaloData* data, TEveWindowSlot* slot);
TEveCalo2D* MakeCalo2D(TEveCalo3D* calo3d, TEveWindowSlot* slot, TEveProjection::EPType_e t);

void MakeViewerScene(TEveWindowSlot* slot, TEveViewer*& v, TEveScene*& s);
void add_jet(TEveElement*, const char*, Float_t, Float_t, Float_t, Float_t);

TEveCaloDataVec* MakeVecData(Int_t ncells=0);

void calo_detail()
{
    TEveManager::Create();

    // data
    TEveCaloDataVec* data = MakeVecData(20);
    data->IncDenyDestroy(); // don't delete if zero parent

    // frames
    TEveWindowSlot* slot =
            TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* packH = slot->MakePack();
    packH->SetElementName("Projections");
    packH->SetHorizontal();
    packH->SetShowTitleBar(kFALSE);

    slot = packH->NewSlot();
    TEveWindowPack* pack0 = slot->MakePack();
    pack0->SetShowTitleBar(kFALSE);
    TEveWindowSlot*  slotLeftTop   = pack0->NewSlot();
    TEveWindowSlot* slotLeftBottom = pack0->NewSlot();

    slot = packH->NewSlot();
    TEveWindowPack* pack1 = slot->MakePack();
    pack1->SetShowTitleBar(kFALSE);
    TEveWindowSlot* slotRightTop    = pack1->NewSlot();
    TEveWindowSlot* slotRightBottom = pack1->NewSlot();

    // viewers ans scenes in second tab
    Float_t maxH = 300;
    TEveCalo3D* calo3d = MakeCalo3D(data, slotRightTop);
    calo3d->SetMaxTowerH(maxH);

    TEveCalo2D* calo2d;
    calo2d = MakeCalo2D(calo3d, slotLeftTop, TEveProjection::kPT_RPhi);
    calo2d->SetMaxTowerH(maxH);
    calo2d = MakeCalo2D(calo3d, slotLeftBottom, TEveProjection::kPT_RhoZ);
    calo2d->SetMaxTowerH(maxH);

    TEveCaloLego* lego = MakeCaloLego(data, slotRightBottom);
    lego->SetAutoRebin(kFALSE);
    lego->Set2DMode(TEveCaloLego::kValSizeOutline);

    gEve->AddElement(lego);
    gEve->GetDefaultGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);

    gEve->GetBrowser()->GetTabRight()->SetTab(1);
    gEve->FullRedraw3D(kTRUE);

}

//______________________________________________________________________________
TEveCaloDataVec* MakeVecData(Int_t ncells)
{
    // Example how to fill data when bins can be iregular.
    // If ncells = 0 (default) whole histogram is taken,
    // otherwise just ncells cells around the maximum.

    TFile::SetCacheFileDir(".");
    TFile* hf = TFile::Open(histFile, "CACHEREAD");
    TH2F* h1 = (TH2F*)hf->Get("ecalLego");
    TH2F* h2 = (TH2F*)hf->Get("hcalLego");

    TEveCaloDataVec* data = new TEveCaloDataVec(2);
    data->RefSliceInfo(0).Setup("ECAL", 0.3, kRed);
    data->RefSliceInfo(1).Setup("HCAL", 0.1, kBlue);

    TAxis *ax =  h1->GetXaxis();
    TAxis *ay =  h1->GetYaxis();

    Int_t xm = 1, xM = ax->GetNbins();
    Int_t ym = 1, yM = ay->GetNbins();
    if (ncells != 0)
    {
        Int_t cx, cy, cz;
        h1->GetMaximumBin(cx, cy, cz);
        xm = TMath::Max(xm, cx-ncells);
        xM = TMath::Min(xM, cx+ncells);
        ym = TMath::Max(ym, cy-ncells);
        yM = TMath::Min(yM, cy+ncells);
    }

    // Take every second cell and set a random size.
    for(Int_t i=xm; i<=xM; i+=2)
    {
        for(Int_t j=ym; j<=yM; j+=2)
        {
            if ( (i+j) % 3)
            {
                data->AddTower(ax->GetBinLowEdge(i), ax->GetBinUpEdge(i),
                               ay->GetBinLowEdge(j), ay->GetBinUpEdge(j));
                data->FillSlice(0, h1->GetBinContent(i, j));
                data->FillSlice(1, h2->GetBinContent(i, j));
            }
            else
            {
                data->AddTower(ax->GetBinLowEdge(i),
                               2 * ax->GetBinWidth(i) + ax->GetBinLowEdge(i),
                               ay->GetBinLowEdge(j),
                               2 * ay->GetBinWidth(j) + ay->GetBinLowEdge(j));
                data->FillSlice(0, h2->GetBinContent(i, j));
                data->FillSlice(1, h2->GetBinContent(i, j));
            }
        }
    }

    data->SetEtaBins(ax);
    data->SetPhiBins(ay);
    data->DataChanged();
    return data;
}

int main(int narg, char* argv[])
{
    using namespace std;
    auto gApp = new TApplication("Hahaha it works!", &narg, argv);

    calo_detail();

    gApp->Run();
    return 0;
}


// ==========================================================================================
//                   F R O M   C A L O R I M E T E R S . C P P
// ==========================================================================================


void calorimeters()
{
    // gSystem->IgnoreSignal(kSigSegmentationViolation, true);
    TEveManager::Create();

    // event data
    TFile::SetCacheFileDir(".");
    TFile* hf = TFile::Open(histFile, "CACHEREAD");
    TH2F* ecalHist = (TH2F*)hf->Get("ecalLego");
    TH2F* hcalHist = (TH2F*)hf->Get("hcalLego");
    TEveCaloDataHist* data = new TEveCaloDataHist();
    data->AddHistogram(ecalHist);
    data->RefSliceInfo(0).Setup("ECAL", 0.3, kBlue);
    data->AddHistogram(hcalHist);
    data->RefSliceInfo(1).Setup("HCAL", 0.1, kRed);
    data->GetEtaBins()->SetTitleFont(120);
    data->GetEtaBins()->SetTitle("h");
    data->GetPhiBins()->SetTitleFont(120);
    data->GetPhiBins()->SetTitle("f");
    data->IncDenyDestroy();
    gEve->AddToListTree(data, kFALSE);


    // first tab
    TEveCaloLego* lego = MakeCaloLego(data, 0);

    //
    // second tab
    //

    // frames
    TEveWindowSlot* slot =
            TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* packH = slot->MakePack();
    packH->SetElementName("Projections");
    packH->SetHorizontal();
    packH->SetShowTitleBar(kFALSE);

    slot = packH->NewSlot();
    TEveWindowPack* pack0 = slot->MakePack();
    pack0->SetShowTitleBar(kFALSE);
    TEveWindowSlot*  slotLeftTop   = pack0->NewSlot();
    TEveWindowSlot* slotLeftBottom = pack0->NewSlot();

    slot = packH->NewSlot();
    TEveWindowPack* pack1 = slot->MakePack();
    pack1->SetShowTitleBar(kFALSE);
    TEveWindowSlot* slotRightTop    = pack1->NewSlot();
    TEveWindowSlot* slotRightBottom = pack1->NewSlot();

    // viewers ans scenes in second tab
    TEveCalo3D* calo3d = MakeCalo3D(data, slotRightTop);
    MakeCalo2D(calo3d, slotLeftTop, TEveProjection::kPT_RPhi);
    MakeCalo2D(calo3d, slotLeftBottom, TEveProjection::kPT_RhoZ);
    lego = MakeCaloLego(data, slotRightBottom);


    gEve->GetBrowser()->GetTabRight()->SetTab(1);
    gEve->Redraw3D(kTRUE);
}

//______________________________________________________________________________
TEveCaloLego* MakeCaloLego(TEveCaloData* data, TEveWindowSlot* slot)
{
    // Eta-phi lego view.

    TEveViewer* v;
    TEveScene* s;
    if (slot)
    {
        MakeViewerScene(slot, v, s);
    } else {
        v = gEve->GetDefaultViewer();
        s = gEve->GetEventScene();
    }
    v->SetElementName("Viewer - Lego");
    s->SetElementName("Scene - Lego");

    TEveCaloLego* lego = new TEveCaloLego(data);
    s->AddElement(lego);

    // By the default lego extends is (1x1x1). Resize it to put in 'natural'
    // coordinates, so that y extend in 2*Pi and set height of lego two times
    //  smaller than y extend to have better view in 3D perspective.
    lego->InitMainTrans();
    lego->RefMainTrans().SetScale(TMath::TwoPi(), TMath::TwoPi(), TMath::Pi());

    // draws scales and axis on borders of window
    TGLViewer* glv = v->GetGLViewer();
    TEveCaloLegoOverlay* overlay = new TEveCaloLegoOverlay();
    glv->AddOverlayElement(overlay);
    overlay->SetCaloLego(lego);

    // set event handler to move from perspective to orthographic view.
    glv->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    glv->SetEventHandler
            (new TEveLegoEventHandler(glv->GetGLWidget(), glv, lego));
    gEve->AddToListTree(lego, kTRUE);

    return lego;
}

//______________________________________________________________________________
TEveCalo3D* MakeCalo3D(TEveCaloData* data, TEveWindowSlot* slot)
{
    // 3D catersian view.

    TEveViewer* v; TEveScene* s;
    MakeViewerScene(slot, v, s);
    v->SetElementName("Viewer - 3D");
    s->SetElementName("Scene - 3D");

    TEveCalo3D* calo3d = new TEveCalo3D(data);
    calo3d->SetBarrelRadius(129.00);
    calo3d->SetEndCapPos(268.36);
    s->AddElement(calo3d);

    add_jet(calo3d, "JetCone Lojz",  1.4,  1.0, 0.4, 0.2);
    add_jet(calo3d, "JetCone Mici", -2.0, -2.1, 0.2, 0.4);

    return calo3d;
}

//______________________________________________________________________________
TEveCalo2D* MakeCalo2D(TEveCalo3D* calo3d, TEveWindowSlot* slot,
                       TEveProjection::EPType_e t)
{
    // Projected calorimeter.

    TEveViewer* v; TEveScene* s;
    MakeViewerScene(slot, v, s);
    v->SetElementName("Viewer - 2D");
    s->SetElementName("Scene - 2D");

    TEveProjectionManager* mng = new TEveProjectionManager();
    mng->SetProjection(t);

    TEveProjectionAxes* axes = new TEveProjectionAxes(mng);
    s->AddElement(axes);
    TEveCalo2D* calo2d = (TEveCalo2D*) mng->ImportElements(calo3d);
    s->AddElement(calo2d);

    v->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);

    gEve->AddToListTree(mng, kTRUE);
    gEve->AddToListTree(calo2d, kTRUE);

    return calo2d;
}

//______________________________________________________________________________
void MakeViewerScene(TEveWindowSlot* slot, TEveViewer*& v, TEveScene*& s)
{
    // Create a scene and a viewer in the given slot.

    v = new TEveViewer("Viewer");
    v->SpawnGLViewer(gEve->GetEditor());
    slot->ReplaceWindow(v);
    gEve->GetViewers()->AddElement(v);
    s = gEve->SpawnNewScene("Scene");
    v->AddScene(s);
}

//______________________________________________________________________________
void add_jet(TEveElement* parent, const char* name,
             Float_t eta, Float_t phi,
             Float_t deta, Float_t dphi)
{
    TEveJetCone* jet = new TEveJetCone(name, name);
    jet->SetMainTransparency(60);
    jet->SetLineColor(kRed);
    jet->SetCylinder(129 - 10, 268.36 - 10);
    jet->AddEllipticCone(eta, phi, deta, dphi);
    jet->SetPickable(kTRUE);
    jet->SetHighlightFrame(kFALSE);
    parent->AddElement(jet);
}






