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
        // make sure factories exist fdor all requested data types
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
    Geom=gDana->GetDGeometry(uint(runnumber));


    Geom->GetFDCWires(FDCwires);
    Geom->GetFDCCathodes(FDCcathodes);

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

       if(!loop->GetJEvent().GetStatusBit(kSTATUS_PHYSICS_EVENT))
        {
            return NOERROR;
        }

        std::ostringstream event_out;
        std::cout<<"opened/created event json "<<endl;

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
        vector<const DBCALPoint *> BCALPoints;
        vector<const DBCALShower *> BCALShowers;

        vector<const DCDCHit *> CDCHits;
        vector<const DFDCHit *> FDCHits;
        vector<const DFDCPseudo *> FDCPseudos;
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
        loop->Get(BCALPoints);
        loop->Get(BCALShowers);
        loop->Get(CDCHits);
        loop->Get(FDCHits);
        loop->Get(FDCPseudos);
        loop->Get(SCHits);


        try {
            //Setup the tracking to display tracking info
            Tracking Tracks(Bfield,Geom);

            //Will take the Charged Tracks given and visualize them
            auto chargedTracksJson = Tracks.Add_DChargedTracks(ChargedTracks);
            auto neutralTracksJson = Tracks.Add_DNeutralParticles(NeutralTracks);


            // StartCounter
            auto scdHitsJson = StartC::Add_SCHits(SCHits);

            //TOF
            auto tofPointsJson = TOF::Add_TOFPoints(TOFPoints);
            auto tofHitsJson = TOF::Add_TOFHits(TOFHits);

            // FCAL;
            auto fcalHitsJson = FCAL::Add_FCALHits(FCALHits);
            auto fcalShowersJson = FCAL::Add_FCALShowers(FCALShowers);

            // BCAL
            auto bcalHitsJson = BCAL::Add_BCALHits(BCALHits);
            auto bcalPointsJson = BCAL::Add_BCALPoints(BCALPoints);
            auto bcalShowersJson = BCAL::Add_BCALShowers(BCALShowers);
            
            // CDC
            auto cdcHitsJson = CDC::Add_CDCHits(CDCHits);

            // FDC
            auto fdcHitsJson = FDC::Add_FDCHits(FDCHits,FDCwires,FDCcathodes);
            auto fdcPseudosJson = FDC::Add_FDCPseudos(FDCPseudos);

            tao::json::value eventJson ({
                                            { "charged_tracks", chargedTracksJson },
                                            { "neutral_tracks", neutralTracksJson },
                                            { "SC_hits", scdHitsJson },
                                            { "TOF_points", tofPointsJson },
                                            { "TOF_hits", tofHitsJson },
                                            { "FCAL_hits", fcalHitsJson },
                                            { "FCAL_showers", fcalShowersJson },
                                            { "BCAL_hits", bcalHitsJson },
                                            { "BCAL_points", bcalPointsJson },
                                            { "BCAL_showers", bcalShowersJson },
                                            { "CDC_hits", cdcHitsJson },
                                            { "FDC_hits", fdcHitsJson },
                                            { "FDC_pseudos", fdcPseudosJson },
                                            {"event_number",eventnumber}
                                        });

            std::ofstream eventFile;
            eventFile.open("www/event.json");

            std::cout<<"opened/created event json "<<endl;
            eventFile<< tao::json::to_string(eventJson, 4);

            eventFile.close();

            cout<<"EVENT JSON CLOSED.  PLEASE REFRESH BROWSER"<<endl;

        }
        catch (std::exception& ex)
        {
            std::cout<<"caught exception"<<endl;
            std::cout<< ex.what() << std::endl;
        }
    }   // <- unlock EventMutex

    _context.SetCurrentEventNumber(eventnumber);
    _context.SetJanaState(hdvis::JanaStates::Idle);
    _context.JanaWaitingLogic().Wait();
    _context.SetJanaState(hdvis::JanaStates::ProcessingEvent);


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

