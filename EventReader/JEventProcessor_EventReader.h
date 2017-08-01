// $Id$
//
//    File: JEventProcessor_EventReader.h
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_EventReader_
#define _JEventProcessor_EventReader_

#include <JANA/JEventProcessor.h>

#include <HDGEOMETRY/DMagneticFieldMap.h>
#include <HDGEOMETRY/DRootGeom.h>

#include <ApplicationContext.h>

#include "WaitingLogic.h"

extern vector<string> toprint;
extern bool ACTIVATE_ALL;

class JEventProcessor_EventReader:public jana::JEventProcessor{
	public:
		JEventProcessor_EventReader(hdvis::ApplicationContext &context);
		~JEventProcessor_EventReader();
		const char* className(void){return "JEventProcessor_EventReader";}

		typedef struct{
			string dataClassName;
			string tag;
		}factory_info_t;
		vector<factory_info_t> fac_info;

		DMagneticFieldMap* Bfield;
        DGeometry* Geom;
		vector<vector<DFDCWire*> >FDCwires;
		vector<vector<DFDCCathode*> >FDCcathodes;





	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

        hdvis::ApplicationContext &_context;
};

#endif // _JEventProcessor_EventReader_

