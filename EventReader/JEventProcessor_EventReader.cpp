// $Id$
//
//    File: JEventProcessor_EventReader.cc
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include <thread>
#include <TEveEventManager.h>
#include "JEventProcessor_EventReader.h"
#include "Tracking.h"
#include "FCAL.h"
#include "TOF.h"
#include <TRACKING/DTrackCandidate.h>
#include <TEveGeoNode.h>


using namespace jana;

vector<string> toprint;
bool ACTIVATE_ALL = false;
extern string OUTPUT_FILENAME;

#define ansi_escape        ((char)0x1b)
#define ansi_bold        ansi_escape<<"[1m"
#define ansi_red        ansi_escape<<"[31m"
#define ansi_normal        ansi_escape<<"[0m"

// Routine used to create our JEventProcessor
#include <TEveManager.h>
#include <mutex>

extern DApplication *gDana;

extern std::mutex gEventMutex;

//------------------
// JEventProcessor_EventReader (Constructor)
//------------------
JEventProcessor_EventReader::JEventProcessor_EventReader(hdvis::RootLoopCommander &rootLoopCommander):
    _rootLoopCommander(rootLoopCommander)

{
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

}

//------------------
// init
//------------------
jerror_t JEventProcessor_EventReader::init(void) {
    // This is called once at program startup.
    // open ROOT file
    gEve->AddElement(dummy);

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
        unsigned int pos = uint(name.rfind(":", name.size() - 1));
        if (pos != (unsigned int) string::npos) {
            tag = name.substr(pos + 1, name.size());
            name.erase(pos);
        }
        factory_info_t f;
        f.dataClassName = name;
        f.tag = tag;
        fac_info.push_back(f);
    }

    Bfield=gDana->GetBfield(uint(runnumber));
    RootGeom=gDana->GetRootGeom(uint(runnumber));
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
    while (!hdvis::RootLoopCommander::InnerLoopMutex.try_lock()) std::this_thread::yield();  // <- (!!!) leave ; there!
    {
        std::lock_guard<std::mutex> eventMutexLockGuard(hdvis::RootLoopCommander::InnerLoopMutex, std::adopt_lock);

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

        vector<const DChargedTrack*> ChargedTracks;
        vector<const DTrackCandidate *> TrackCandidates;

        vector<const DFCALHit *> FCALHits;
        //vector<const DFDCHit *> FDCHits;
        //vector<const DFCALDigiHit *> FCALDigiHits;
        //vector<const DFCALCluster *> FCALClusters;
        vector<const DFCALShower *> FCALShowers;
        //vector<const DFCALTruthShower *> FCALTruthShowers;

        vector<const DTOFPoint *> TOFPoints;

        loop->Get(FCALHits);
        //loop->Get(FCALDigiHits);
        //loop->Get(FCALClusters);
        loop->Get(FCALShowers);
        //loop->Get(FCALTruthShowers);
        loop->Get(TrackCandidates);
        loop->Get(ChargedTracks);
        loop->Get(TOFPoints);

        //Skips the first few non-Physics events (find a better way)
        if (FCALHits.size() ==0 /*&& FCALDigiHits.size()==0 && FCALClusters.size()==0 && FCALShowers.size()==0 && FCALTruthShowers.size()==0*/) {
            return NOERROR;
        }

        //Clear the event...unless it is empty
        gEve->GetCurrentEvent()->DestroyElements();

        //Setup the tracking to display tracking info
        Tracking Tracks(Bfield,RootGeom);

        //Will take the Charged Tracks given and visualize them
        Tracks.Add_DChargedTracks(ChargedTracks);

        TOF TOFDet;
        TOFDet.Add_TOFPoints(TOFPoints);

        //Decalre the FCAL "module"
        FCAL FCALDet;
        //Take the hits and visualize them
        FCALDet.Add_FCALHits(FCALHits);
        //FCALDet.Add_FCALShowers(FCALShowers);

        //Redraw the scene(s)
        //sleep(1);
        //gEve->FullRedraw3D();
        //sleep(1);
        _rootLoopCommander.EveFullRedraw3D();


    }   // <- unlock EventMutex

    _waitingLogic.Wait();


    //gEve->EveFullRedraw3D();
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

