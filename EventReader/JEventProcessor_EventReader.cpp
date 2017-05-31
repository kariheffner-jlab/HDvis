// $Id$
//
//    File: JEventProcessor_EventReader.cc
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include <thread>
#include <TEveEventManager.h>
#include "JEventProcessor_EventReader.h"
#include <FDC/DFDCHit.h>
#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALDigiHit.h>
#include <FCAL/DFCALCluster.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALTruthShower.h>
#include <TRACKING/DTrackCandidate.h>
#include <PID/DChargedTrack.h>
#include <TRACKING/DReferenceTrajectory.h>

#include <TGLViewer.h>
#include <TCanvas.h>
#include <TEveViewer.h>
#include <TEveGedEditor.h>
#include <TEveScene.h>
#include <TEveCaloLegoEditor.h>
#include <TEveCaloLegoOverlay.h>
#include <TEveCaloLegoGL.h>
#include <TEveBrowser.h>
#include <TEveTrans.h>
#include <TEveCaloLegoOverlay.h>
#include <TEveLegoEventHandler.h>
#include <TGLWidget.h>
#include <TEveGeoNode.h>


using namespace jana;

vector<string> toprint;
bool ACTIVATE_ALL = false;
extern string OUTPUT_FILENAME;

#define ansi_escape        ((char)0x1b)
#define ansi_bold        ansi_escape<<"[1m"
#define ansi_black        ansi_escape<<"[30m"
#define ansi_red        ansi_escape<<"[31m"
#define ansi_green        ansi_escape<<"[32m"
#define ansi_blue        ansi_escape<<"[34m"
#define ansi_normal        ansi_escape<<"[0m"
#define ansi_up(A)        ansi_escape<<"["<<(A)<<"A"
#define ansi_down(A)    ansi_escape<<"["<<(A)<<"B"
#define ansi_forward(A)    ansi_escape<<"["<<(A)<<"C"
#define ansi_back(A)    ansi_escape<<"["<<(A)<<"D"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
#include <TEveCaloData.h>
#include <TEveManager.h>
#include <mutex>
#include <DANA/DApplication.h>
extern DApplication *gDana;

extern "C" {
void InitPlugin(JApplication *app) {
    InitJANAPlugin(app);
    TGeoNode *hall;
    //app->AddProcessor(new JEventProcessor_EventReader(hall));
}
} // "C"

extern std::mutex gEventMutex;

//------------------
// JEventProcessor_EventReader (Constructor)
//------------------
JEventProcessor_EventReader::JEventProcessor_EventReader(TGeoNode *node) {
    hallD = node;
    //gEve->AddElement(FCAL_ps);
    ROOTfile = NULL;
    h2 = new TH2F("FCAL Hits", "FCAL Hits", 100, -50, 50, 100, -50, 50);

    data = new TEveCaloDataHist();
    data->AddHistogram(h2);
    data->RefSliceInfo(0).Setup("FCAL", 0.3, kBlue);


    gEve->AddToListTree(data, kFALSE);

    TEveWindowSlot *slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveViewer *v = new TEveViewer("Viewer");
    v->SpawnGLViewer(gEve->GetEditor());
    slot->ReplaceWindow(v);
    gEve->GetViewers()->AddElement(v);
    TEveScene *s = gEve->SpawnNewScene("Scene");
    v->AddScene(s);


    v->SetElementName("Viewer - Lego");
    s->SetElementName("Scene - Lego");


    TEveCaloLego *lego = new TEveCaloLego(data);

    s->AddElement(lego);

    // By the default lego extends is (1x1x1). Resize it to put in 'natural'
    // coordinates, so that y extend in 2*Pi and set height of lego two times
    //  smaller than y extend to have better view in 3D perspective.
    lego->InitMainTrans();
    lego->RefMainTrans().SetScale(TMath::TwoPi(), TMath::TwoPi(), TMath::Pi());

    // draws scales and axis on borders of window
    TGLViewer *glv = v->GetGLViewer();
    TEveCaloLegoOverlay *overlay = new TEveCaloLegoOverlay();
    glv->AddOverlayElement(overlay);
    overlay->SetCaloLego(lego);

    // set event handler to move from perspective to orthographic view.
    glv->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    glv->SetEventHandler
            (new TEveLegoEventHandler(glv->GetGLWidget(), glv, lego));
    gEve->AddToListTree(lego, kTRUE);

}

//------------------
// ~JEventProcessor_EventReader (Destructor)
//------------------
JEventProcessor_EventReader::~JEventProcessor_EventReader() {
//Close the ROOT file
    if (ROOTfile != NULL) {
        ROOTfile->Write();
        ROOTfile->Close();
        delete ROOTfile;
        ROOTfile = NULL;
        cout << endl << "Closed ROOT file" << endl;
    }
}

//------------------
// init
//------------------
jerror_t JEventProcessor_EventReader::init(void) {
    // This is called once at program startup.
    // open ROOT file
    FCAL_ps->SetOwnIds(kTRUE);
    Track_ps->SetOwnIds(kTRUE);

    TEveRGBAPalette* pal = new TEveRGBAPalette(0, 130);

    FCAL_bs->SetPalette(pal);
    //FCAL_bs->SetFrame(frm);
    FCAL_bs->Reset(TEveBoxSet::kBT_AABox, kFALSE, 64);
    gEve->AddElement(FCAL_bs);
    ROOTfile = new TFile(OUTPUT_FILENAME.c_str(), "RECREATE", "Produced by hd_root");
    if (!ROOTfile->IsOpen()) {
        cout << "Cannot open ROOT file. Quitting now." << endl;
        exit(0);
    }

    cout << "Opened ROOT file \"" << OUTPUT_FILENAME << "\" ..." << endl;
    return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_EventReader::brun(JEventLoop *eventLoop, int32_t runnumber) {
    // This is called whenever the run number changes
    vector<string> factory_names;
    eventLoop->GetFactoryNames(factory_names);

    usleep(100000); //this just gives the Main thread a chance to finish printing the "Launching threads" message
    cout << endl;

    // If ACTIVATE_ALL is set then add EVERYTHING.
    if (ACTIVATE_ALL) {
        toprint = factory_names;
    } else {
        // make sure factories exist for all requested data types
        // If a factory isn't found, but one with a "D" prefixed
        // is, go ahead and correct the name.
        vector<string> really_toprint;
        for (unsigned int i = 0; i < toprint.size(); i++) {
            int found = 0;
            int dfound = 0;
            for (unsigned int j = 0; j < factory_names.size(); j++) {
                if (factory_names[j] == toprint[i])found = 1;
                if (factory_names[j] == "D" + toprint[i])dfound = 1;
            }
            if (found)
                really_toprint.push_back(toprint[i]);
            else if (dfound)
                really_toprint.push_back("D" + toprint[i]);
            else
                cout << ansi_red << "WARNING:" << ansi_normal
                     << " Couldn't find factory for \""
                     << ansi_bold << toprint[i] << ansi_normal
                     << "\"!" << endl;
        }

        toprint = really_toprint;
    }

    // At this point, toprint should contain a list of all factories
    // in dataClassName:tag format, that both exist and were requested.
    // Seperate the tag from the name and fill the fac_info vector.
    fac_info.clear();
    for (unsigned int i = 0; i < toprint.size(); i++) {
        string name = toprint[i];
        string tag = "";
        unsigned int pos = name.rfind(":", name.size() - 1);
        if (pos != (unsigned int) string::npos) {
            tag = name.substr(pos + 1, name.size());
            name.erase(pos);
        }
        factory_info_t f;
        f.dataClassName = name;
        f.tag = tag;
        fac_info.push_back(f);
    }

    Bfield=gDana->GetBfield(runnumber);
    RootGeom=gDana->GetRootGeom(runnumber);
    cout << endl;

    return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_EventReader::evnt(JEventLoop *loop, uint64_t eventnumber) {

    // This is called for every event. Use of common resources like writing
    // to a file or filling a histogram should be mutex protected. Using
    // loop->Get(...) to get reconstructed objects (and thereby activating the
    // reconstruction algorithm) should be done outside of any mutex lock
    // since multiple threads may call this method at the same time.
    // Here's an example:
    //
    // vector<const MyDataClass*> mydataclasses;
    // loop->Get(mydataclasses);
    //
    // japp->RootFillLock(this);
    //  ... fill historgrams or trees ...
    // japp->RootFillUnLock(this);
    while (!gEventMutex.try_lock()) std::this_thread::yield();  // <- (!!!) leave ; there!
    {
        std::lock_guard<std::mutex> eventMutexLockGuard(gEventMutex, std::adopt_lock);

        for (unsigned int i = 0; i < toprint.size(); i++) {
            string name = fac_info[i].dataClassName;
            string tag = fac_info[i].tag;
            JFactory_base *factory = loop->GetFactory(name, tag.c_str());
            if (!factory)factory = loop->GetFactory("D" + name, tag.c_str());
            if (factory) {
                try {
                    factory->GetNrows();
                } catch (...) {
                    // someone threw an exception
                }
            }
        }
        //FCAL_ps=new TEvePointSet();gEve->GetCurrentEvent()->DestroyElements();
        //FCAL_ps->Reset();
        //gEve->DoRedraw3D();


        vector<const DChargedTrack*> ChargedTracks;
        vector<const DTrackCandidate *> TrackCandidates;
        vector<const DFCALHit *> FCALHits;
        //vector<const DFDCHit *> FDCHits;
        //vector<const DFCALDigiHit *> FCALDigiHits;
        //vector<const DFCALCluster *> FCALClusters;
        //vector<const DFCALShower *> FCALShowers;
        //vector<const DFCALTruthShower *> FCALTruthShowers;

        loop->Get(FCALHits);
        //loop->Get(FCALDigiHits);
        //loop->Get(FCALClusters);
        //loop->Get(FCALShowers);
        //loop->Get(FCALTruthShowers);
        loop->Get(TrackCandidates);
        loop->Get(ChargedTracks);


        if (FCALHits.size() ==
            0 /*&& FCALDigiHits.size()==0 && FCALClusters.size()==0 && FCALShowers.size()==0 && FCALTruthShowers.size()==0*/) {
            //std::cout<<"skip"<<std::endl;
            return NOERROR;
        }

        for (int i = 0; i < FCAL_points.size(); i++) {
            gEve->GetCurrentEvent()->RemoveElement(FCAL_points[i]);
        }

        for (int i = 0; i <Track_points.size(); i++) {
            gEve->GetCurrentEvent()->RemoveElement(Track_points[i]);
        }

            gEve->GetCurrentEvent()->RemoveElement(FCAL_bs);
        //gEve->GetCurrentEvent()->DestroyElements();
        FCAL_points.clear();
        Track_points.clear();



        FCAL_bs = new TEveBoxSet("FCAL_hits");
        TEveRGBAPalette* pal = new TEveRGBAPalette(-150,150);

        FCAL_bs->SetPalette(pal);
        //FCAL_bs->SetFrame(frm);
        FCAL_bs->Reset(TEveBoxSet::kBT_AABox, kTRUE, 64);

        //h2->Reset();
        for (uint i = 0; i < FCALHits.size(); i++) {
            FCAL_ps = new TEvePointSet();
            //std::cout<<FCALHits[i]->x<<","<<FCALHits[i]->y<<"|"<<FCALHits[i]->E<<std::endl;
            FCAL_ps->SetNextPoint(FCALHits[i]->x, FCALHits[i]->y,500+173.906); //FCAL alignment is 150.501,-349.986,147.406
            //FCAL_ps->SetNextPoint(FCALHits[i]->x+150.501, FCALHits[i]->y-349.986, 26.5+147.406); //FCAL alignment is 150.501,-349.986,147.406
            FCAL_ps->SetMainColorRGB((FCALHits[i]->E * 10), 0., 0.);
            FCAL_ps->SetPointId(new TNamed(Form("Point %d", i), ""));
            FCAL_ps->SetMarkerSize(1);
            FCAL_ps->SetMarkerStyle(4);
            FCAL_ps->SetElementName(Form("FCAL point %i", i));
            FCAL_points.push_back(FCAL_ps);
            h2->Fill(FCALHits[i]->x, FCALHits[i]->y);

            FCAL_bs->AddBox(FCALHits[i]->x-1, FCALHits[i]->y-1, 500+173.9,2, 2,  FCALHits[i]->E*100/*20*log(FCALHits[i]->E*1000)*/);

            int redness=255;
            if(abs(FCALHits[i]->t)*10<255)
                redness=abs(FCALHits[i]->t)*10;

            if(FCALHits[i]->t>=0.0)
                FCAL_bs->DigitColor(redness, 0,0,50);
                //FCAL_bs->DigitValue(FCALHits[i]->t);
            else
                FCAL_bs->DigitColor(0,redness,0,50);
                //FCAL_bs->DigitColor(0,abs(FCALHits[i]->t),0);
        }

        int RMAX_INTERIOR=65;
        int RMAX_EXTERIOR=89;
        DReferenceTrajectory rt(Bfield);
        rt.Rsqmax_interior = RMAX_INTERIOR*RMAX_INTERIOR;//innerBCAL radius
        rt.Rsqmax_exterior = RMAX_EXTERIOR*RMAX_EXTERIOR;

        rt.SetDRootGeom(RootGeom);
        rt.SetDGeometry(NULL);

        for(int i=0;i<ChargedTracks.size()/*TrackCandidates.size()*/;i++)
        {
            rt.Reset();
            Track_ps = new TEvePointSet();


            rt.SetMass(ChargedTracks[i]->Get_BestFOM()->mass());
            //rt.SetMass(TrackCandidates[i]->mass());

            rt.Swim(ChargedTracks[i]->Get_BestFOM()->position(), ChargedTracks[i]->Get_BestFOM()->momentum(), ChargedTracks[i]->Get_BestFOM()->charge());
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt.swim_steps;

            for(int j=0; j<rt.Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;
                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>625 )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                Track_ps->SetNextPoint(step_loc.X(), step_loc.Y(),step_loc.Z()); //FCAL alignment is 150.501,-349.986,147.406
                //FCAL_ps->SetNextPoint(FCALHits[i]->x+150.501, FCALHits[i]->y-349.986, 26.5+147.406); //FCAL alignment is 150.501,-349.986,147.406

                //if(TrackCandidates[i]->charge()==1)
                if(ChargedTracks[i]->Get_BestFOM()->charge()==1)
                    Track_ps->SetMainColorRGB(0,float(250.), 0.);
                else
                    Track_ps->SetMainColorRGB(float(250.),0, 0.);

                Track_ps->SetPointId(new TNamed(Form("Track Point %d", i), ""));
                Track_ps->SetMarkerSize(1);
                Track_ps->SetMarkerStyle(4);
                Track_ps->SetElementName(Form("Track points %i", i));

            }
            Track_points.push_back(Track_ps);
        }


        h2->SetStats(0);
        h2->Draw("colzsame");

        data->AddHistogram(h2);

        //TGeoNode *fcalNode = (TGeoNode *) hallD->GetNodes()->FindObject("FCAL_1");
        //cout<<"fcalNode is "<<fcalNode<<endl;


       /* for (int i = 0; i < FCAL_points.size(); i++) {
            gEve->AddElement(FCAL_points[i]);
        }*/

         for (int i = 0; i < Track_points.size(); i++) {
            gEve->AddElement(Track_points[i]);
        }

        gEve->AddElement(FCAL_bs);
        //gEve->FullRedraw3D();//This crashes on event8....

        //sleep(.01);
        //gEve->Redraw3D();
        //FCAL_ps->Destroy();
        //canvas->Update();

        //gEve->GetEventScene()->Draw();

        //     jout<<"this event has: "<<FCALHits.size()<<" FCALHits "<<FCALDigiHits.size()<<" FCALDigiHits "<<FCALClusters.size()<< " Clusters "<<FCALShowers.size()<<" showers and "<<FCALTruthShowers.size()<<" TruthShowers "<<endl;
        sleep(.5);
        gEve->FullRedraw3D();
        sleep(.5);
        //gEve->DoRedraw3D();
    }   // <- unlock EventMutex

    _waitingLogic.Wait();


    //gEve->Redraw3D();
    /*int x;
    cin>>x;*/
    return NOERROR;
}


//------------------
// erun
//------------------
jerror_t JEventProcessor_EventReader::erun(void) {
    // This is called whenever the run number changes, before it is
    // changed to give you a chance to clean up before processing
    // events from the next run number.

    return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_EventReader::fini(void) {
    // Called before program exit after event processing is finished.
    return NOERROR;
}

