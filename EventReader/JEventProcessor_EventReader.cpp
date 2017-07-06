// $Id$
//
//    File: JEventProcessor_EventReader.cc
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include <thread>
#include "JEventProcessor_EventReader.h"
#include "Tracking.h"
#include "FCAL.h"
#include "TOF.h"
#include "BCAL.h"
#include "CDC.h"
#include "FDC.h"
#include "SC.h"
#include <TRACKING/DTrackCandidate.h>
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

#include <mutex>

extern DApplication *gDana;

//------------------
// JEventProcessor_EventReader (Constructor)
//------------------
JEventProcessor_EventReader::JEventProcessor_EventReader(hdvis::ApplicationContext &context):
    _context(context)

{

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

    {
        static bool isFirstGoodEvent = true;
        auto lock = std::unique_lock<std::mutex>(hdvis::ApplicationContext::InnerLoopMutex);

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
        event_out.open("event.json");
        std::cout<<"opened/created event json "<<endl;
        event_out<<"{\n";
        event_out.close();

        vector<const DChargedTrack*> ChargedTracks;
        vector<const DNeutralParticle*> NeutralTracks;
        vector<const DTrackCandidate *> TrackCandidates;

        vector<const DFCALHit *> FCALHits;
        //vector<const DFDCHit *> FDCHits;
        //vector<const DFCALDigiHit *> FCALDigiHits;
        vector<const DFCALCluster *> FCALClusters;
        vector<const DFCALShower *> FCALShowers;
        //vector<const DFCALTruthShower *> FCALTruthShowers;

        vector<const DTOFHit *> TOFHits;
        vector<const DTOFPoint *> TOFPoints;

        vector<const DBCALHit *> BCALHits;
        vector<const DCDCHit *> CDCHits;
        vector<const DFDCHit *> FDCHits;
        vector<const DSCHit *> SCHits;

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
        loop->Get(BCALHits);
        loop->Get(CDCHits);
        loop->Get(FDCHits);
        loop->Get(SCHits);


        //Setup the tracking to display tracking info
        Tracking Tracks(Bfield,RootGeom);

        //Will take the Charged Tracks given and visualize them
        Tracks.Add_DChargedTracks(ChargedTracks);
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();
        Tracks.Add_DNeutralParticles(NeutralTracks);
//------------------------------------------------------------------------------------------
        StartC SCDet;

        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();

        SCDet.Add_SCHits(SCHits);
//------------------------------------------------------------------------------------------
        TOF TOFDet;

        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();

        TOFDet.Add_TOFPoints(TOFPoints);
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();
        TOFDet.Add_TOFHits(TOFHits);
//------------------------------------------------------------------------------------------
        //Decalre the FCAL "module"
        FCAL FCALDet;
        //Take the hits and visualize them
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();

        FCALDet.Add_FCALHits(FCALHits);
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();

        FCALDet.Add_FCALShowers(FCALShowers);
//------------------------------------------------------------------------------------------
        BCAL BCALDet;
        //Take the hits and visualize them
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();

        BCALDet.Add_BCALHits(BCALHits);
//------------------------------------------------------------------------------------------
        CDC CDCDet;
        //Take the hits and visualize them
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();

        CDCDet.Add_CDCHits(CDCHits);
//------------------------------------------------------------------------------------------
        FDC FDCDet;
        //Take the hits and visualize them
        event_out.open("event.json",ios::app);
        event_out<<",";
        event_out.close();
        //cout<<"ADDING FDC HITS"<<endl;
        FDCDet.Add_FDCHits(FDCHits);



        event_out.open("event.json",ios::app);
        event_out<<"\"event_number\":"<<eventnumber<<endl;
        event_out<<"}";
        event_out.close();

        cout<<"EVENT JSON CLOSED.  PLEASE REFRESH BROWSER"<<endl;

    }   // <- unlock EventMutex


    _context.JanaWaitingLogic().Wait();


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

