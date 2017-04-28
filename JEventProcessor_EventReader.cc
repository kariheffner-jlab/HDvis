// $Id$
//
//    File: JEventProcessor_EventReader.cc
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include "EventReader/JEventProcessor_EventReader.h"
#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALDigiHit.h>
#include <FCAL/DFCALCluster.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALTruthShower.h>

using namespace jana;

vector<string> toprint;
bool ACTIVATE_ALL=false;
extern string OUTPUT_FILENAME;

#define ansi_escape		((char)0x1b)
#define ansi_bold 		ansi_escape<<"[1m"
#define ansi_black		ansi_escape<<"[30m"
#define ansi_red			ansi_escape<<"[31m"
#define ansi_green		ansi_escape<<"[32m"
#define ansi_blue			ansi_escape<<"[34m"
#define ansi_normal		ansi_escape<<"[0m"
#define ansi_up(A)		ansi_escape<<"["<<(A)<<"A"
#define ansi_down(A)		ansi_escape<<"["<<(A)<<"B"
#define ansi_forward(A)	ansi_escape<<"["<<(A)<<"C"
#define ansi_back(A)		ansi_escape<<"["<<(A)<<"D"

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
ROOTfile = NULL;
}

//------------------
// ~JEventProcessor_EventReader (Destructor)
//------------------
JEventProcessor_EventReader::~JEventProcessor_EventReader()
{
//Close the ROOT file
	if(ROOTfile!=NULL){
		ROOTfile->Write();
		ROOTfile->Close();
		delete ROOTfile;
		ROOTfile=NULL;
		cout<<endl<<"Closed ROOT file"<<endl;
	}
}

//------------------
// init
//------------------
jerror_t JEventProcessor_EventReader::init(void)
{
	// This is called once at program startup. 
	// open ROOT file
	ROOTfile = new TFile(OUTPUT_FILENAME.c_str(),"RECREATE","Produced by hd_root");
	if(!ROOTfile->IsOpen()){
		cout << "Cannot open ROOT file. Quitting now." << endl;
		exit(0);
	}
	
	cout<<"Opened ROOT file \""<<OUTPUT_FILENAME<<"\" ..."<<endl;
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_EventReader::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	// This is called whenever the run number changes
vector<string> factory_names;
	eventLoop->GetFactoryNames(factory_names);

	usleep(100000); //this just gives the Main thread a chance to finish printing the "Launching threads" message
	cout<<endl;

	// If ACTIVATE_ALL is set then add EVERYTHING.
	if(ACTIVATE_ALL){
		toprint = factory_names;
	}else{
		// make sure factories exist for all requested data types
		// If a factory isn't found, but one with a "D" prefixed
		// is, go ahead and correct the name.
		vector<string> really_toprint;
		for(unsigned int i=0; i<toprint.size();i++){
			int found = 0;
			int dfound = 0;
			for(unsigned int j=0;j<factory_names.size();j++){
				if(factory_names[j] == toprint[i])found = 1;
				if(factory_names[j] == "D" + toprint[i])dfound = 1;
			}
			if(found)
				really_toprint.push_back(toprint[i]);
			else if(dfound)
				really_toprint.push_back("D" + toprint[i]);
			else
				cout<<ansi_red<<"WARNING:"<<ansi_normal
					<<" Couldn't find factory for \""
					<<ansi_bold<<toprint[i]<<ansi_normal
					<<"\"!"<<endl;
		}
		
		toprint = really_toprint;
	}
	
	// At this point, toprint should contain a list of all factories
	// in dataClassName:tag format, that both exist and were requested.
	// Seperate the tag from the name and fill the fac_info vector.
	fac_info.clear();
	for(unsigned int i=0;i<toprint.size();i++){
		string name = toprint[i];
		string tag = "";
		unsigned int pos = name.rfind(":",name.size()-1);
		if(pos != (unsigned int)string::npos){
			tag = name.substr(pos+1,name.size());
			name.erase(pos);
		}
		factory_info_t f;
		f.dataClassName = name;
		f.tag = tag;
		fac_info.push_back(f);
	}
	
	cout<<endl;

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

	for(unsigned int i=0;i<toprint.size();i++){
		string name =fac_info[i].dataClassName;
		string tag = fac_info[i].tag;
		JFactory_base *factory = loop->GetFactory(name,tag.c_str());
		if(!factory)factory = loop->GetFactory("D" + name,tag.c_str());
		if(factory){
			try{
				factory->GetNrows();
			}catch(...){
				// someone threw an exception
			}
		}
	}
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

//------------------
// fini
//------------------
jerror_t JEventProcessor_EventReader::fini(void)
{
	// Called before program exit after event processing is finished.
	return NOERROR;
}

