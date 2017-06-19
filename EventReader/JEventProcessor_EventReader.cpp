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
#include <DANA/DStatusBits.h>
#include <fstream>


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



void MakeElementVisible(TEveElement *e)
{
    e->SetRnrState(true);
    for (auto i=e->BeginParents(); i!=e->EndParents(); ++i)
    {
        auto parent =*i;

        //cout<<"turn on "<<parent->GetElementName()<<endl;
        parent->SetRnrState(true);
        if(parent == gEve->GetGlobalScene())
        {
            return;
        } else {
            MakeElementVisible(parent);
        }
    }
}

void MakeDescendantRecursiveVisible(TEveElement *e, bool isVisible)
{
    for (auto i=e->BeginChildren(); i!=e->EndChildren(); ++i)
    {
        auto child =*i;

        //cout<<"turn off "<<child->GetElementName()<<endl;
        child->SetRnrState(isVisible);
        MakeDescendantRecursiveVisible(child, isVisible);

    }
}

void MakeDescendantRecursiveColor(TEveElement *e, UChar_t r, UChar_t g, UChar_t b)
{
    for (auto i=e->BeginChildren(); i!=e->EndChildren(); ++i)
    {
        auto child =*i;

        //cout<<"color "<<child->GetElementName()<<endl;
        child->SetMainColorRGB(r,g,b);

        MakeDescendantRecursiveColor(child, r, g, b);
    }
}
void MakeDescendantRecursiveTransparancey(TEveElement *e, float alpha)
{
    for (auto i=e->BeginChildren(); i!=e->EndChildren(); ++i)
    {
        auto child =*i;

        //cout<<"color "<<child->GetElementName()<<endl;
        child->SetMainAlpha(alpha);

        MakeDescendantRecursiveTransparancey(child, alpha);
    }
}

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
    //while (!hdvis::RootLoopCommander::InnerLoopMutex.try_lock()) std::this_thread::yield();  // <- (!!!) leave ; there!
    {
        static bool isFirstGoodEvent = true;
        //std::lock_guard<std::mutex> eventMutexLockGuard(hdvis::RootLoopCommander::InnerLoopMutex);
        auto lock = std::unique_lock<std::mutex>(hdvis::RootLoopCommander::InnerLoopMutex);

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

        //Skips the first few non-Physics events (find a better way)
       if(!loop->GetJEvent().GetStatusBit(kSTATUS_PHYSICS_EVENT))//&& FCALDigiHits.size()==0 && FCALClusters.size()==0 && FCALShowers.size()==0 && FCALTruthShowers.size()==0) {
        {
            return NOERROR;
        }

        std::ofstream event_out;
        event_out.open("../js/eventw.json");
        std::cout<<"opened/created eventw json "<<endl;
        event_out<<"{\n";
        event_out.close();

        vector<const DChargedTrack*> ChargedTracks;
        vector<const DNeutralParticle*> NeutralTracks;
        vector<const DTrackCandidate *> TrackCandidates;

        vector<const DFCALHit *> FCALHits;
        //vector<const DFDCHit *> FDCHits;
        //vector<const DFCALDigiHit *> FCALDigiHits;
        //vector<const DFCALCluster *> FCALClusters;
        vector<const DFCALShower *> FCALShowers;
        //vector<const DFCALTruthShower *> FCALTruthShowers;

        vector<const DTOFHit *> TOFHits;
        vector<const DTOFPoint *> TOFPoints;

        loop->Get(FCALHits);
        //loop->Get(FCALDigiHits);
        //loop->Get(FCALClusters);
        loop->Get(FCALShowers);
        //loop->Get(FCALTruthShowers);
        loop->Get(TrackCandidates);
        loop->Get(ChargedTracks);
        loop->Get(NeutralTracks);
        loop->Get(TOFPoints);
        loop->Get(TOFHits);



        if(isFirstGoodEvent)
        {
            isFirstGoodEvent =false;



            TEveGeoTopNode* enode = new TEveGeoTopNode(gGeoManager, gGeoManager->GetNode(0));
            gEve->AddGlobalElement(enode);
            enode->ExpandIntoListTreesRecursively();

            //gEve->GetGlobalScene()->SetRnrChildren(kFALSE);
            auto globalScene = gEve->GetGlobalScene();

            MakeDescendantRecursiveVisible(globalScene, false);

            auto target = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("LASS_1")->FindChild("TARG_1");
            MakeDescendantRecursiveVisible(target, true);
            MakeElementVisible(target);
            target->SetMainColorRGB(UChar_t(255),155,155);
            MakeDescendantRecursiveColor(target,255,155,155);
            MakeDescendantRecursiveTransparancey(target, .40);

            auto sc = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("LASS_1")->FindChild("STRT_1");
            MakeDescendantRecursiveVisible(sc, true);
            MakeElementVisible(sc);
            sc->SetMainColorRGB(UChar_t(155),180,255);
            MakeDescendantRecursiveColor(sc,155,180,255);
            MakeDescendantRecursiveTransparancey(sc,.6);

            auto bcal = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("LASS_1")->FindChild("BCAL_1");
            MakeDescendantRecursiveVisible(bcal, true);
            MakeElementVisible(bcal);
            bcal->SetMainColorRGB(UChar_t(180),220,255);
            MakeDescendantRecursiveColor(bcal,180,220,255);
            MakeDescendantRecursiveTransparancey(bcal, 5);


            auto cdc = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("LASS_1")->FindChild("CDC_1");
            MakeDescendantRecursiveVisible(cdc, true);
            MakeElementVisible(cdc);
            cdc->SetMainColorRGB(UChar_t(200),230,255);
            MakeDescendantRecursiveColor(cdc,200,230,255);
            MakeDescendantRecursiveTransparancey(cdc, 4);

            auto fdc = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("LASS_1")->FindChild("FDC_1");
            //auto fdcP1 = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("LASS_1")->FindChild("FDC_1")->FindChild("FDP1_1");
            MakeDescendantRecursiveVisible(fdc, true);
            MakeElementVisible(fdc);
            //MakeElementVisible(fdcP1);
            fdc->SetMainColorRGB(UChar_t(170),220,255);
            MakeDescendantRecursiveColor(fdc,170,220,255);
            fdc->SetMainTransparency(.8);
            MakeDescendantRecursiveTransparancey(fdc, 4);

            auto tof1 = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("FTOF_1");
            auto tof2 = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("FTOF_2");

            MakeDescendantRecursiveVisible(tof1, true);
            MakeElementVisible(tof1);
            tof1->SetMainAlpha(1);
            MakeDescendantRecursiveColor(tof1,53,143,254);
            MakeDescendantRecursiveTransparancey(tof1, .7);

            MakeDescendantRecursiveVisible(tof2, true);
            MakeElementVisible(tof2);
            MakeDescendantRecursiveColor(tof2,53,143,254);
            MakeDescendantRecursiveTransparancey(tof2, .7);

            auto fcal = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("FCAL_1");

            MakeDescendantRecursiveVisible(fcal, true);
            MakeElementVisible(fcal);
            MakeDescendantRecursiveColor(fcal,53,143,254);
            MakeDescendantRecursiveTransparancey(fcal, .7);


            _rootLoopCommander.EveFullRedraw3D();
        }

        //Clear the event...unless it is empty
        gEve->GetCurrentEvent()->DestroyElements();

        //Setup the tracking to display tracking info
        Tracking Tracks(Bfield,RootGeom);

        //Will take the Charged Tracks given and visualize them
        Tracks.Add_DChargedTracks(ChargedTracks);
        event_out.open("../js/eventw.json",ios::app);
        event_out<<",";
        event_out.close();
        Tracks.Add_DNeutralParticles(NeutralTracks);

        TOF TOFDet;
        TOFDet.Add_TOFPoints(TOFPoints);
        //TOFDet.Add_TOFHits(TOFHits);

        //Decalre the FCAL "module"
        FCAL FCALDet;
        //Take the hits and visualize them
        event_out.open("../js/eventw.json",ios::app);
        event_out<<",";
        event_out.close();
        FCALDet.Add_FCALHits(FCALHits);
        FCALDet.Add_FCALShowers(FCALShowers);

        //Redraw the scene(s)
        //sleep(1);
        //gEve->FullRedraw3D();
        //sleep(1);
        _rootLoopCommander.EveFullRedraw3D();

        event_out.open("../js/eventw.json",ios::app);
        event_out<<"}";
        event_out.close();
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

