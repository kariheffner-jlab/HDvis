// $Id$
//
//    File: JEventProcessor_EventReader.cc
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include "JEventProcessor_EventReader.h"
#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALDigiHit.h>
#include <FCAL/DFCALCluster.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALTruthShower.h>

using namespace jana;


// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_EventReader());
}
} // "C"


//------------------
// JEventProcessor_EventReader (Constructor)
//------------------
JEventProcessor_EventReader::JEventProcessor_EventReader()
{

}

//------------------
// ~JEventProcessor_EventReader (Destructor)
//------------------
JEventProcessor_EventReader::~JEventProcessor_EventReader()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_EventReader::init(void)
{
	// This is called once at program startup. 

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_EventReader::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	// This is called whenever the run number changes
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_EventReader::evnt(JEventLoop *loop, uint64_t eventnumber)
{
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
  vector<const DFCALHit*> FCALHits;
  vector<const DFCALDigiHit*> FCALDigiHits;
  vector<const DFCALCluster*> FCALClusters;
  vector<const DFCALShower*> FCALShowers;
  vector<const DFCALTruthShower*> FCALTruthShowers;
  
  loop->Get(FCALHits);
  loop->Get(FCALDigiHits);
  loop->Get(FCALClusters);
  loop->Get(FCALShowers);
  loop->Get(FCALTruthShowers);


  jout<<"this event has: "<<FCALHits.size()<<" FCALHits "<<FCALDigiHits.size()<<" FCALDigiHits "<<FCALClusters.size()<< " Clusters "<<FCALShowers.size()<<" showers and "<<FCALTruthShowers.size()<<" TruthShowers "<<endl;

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_EventReader::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_EventReader::fini(void)
{
	// Called before program exit after event processing is finished.
	return NOERROR;
}

