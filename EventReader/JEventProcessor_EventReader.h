// $Id$
//
//    File: JEventProcessor_EventReader.h
// Created: Fri Apr  7 14:45:08 EDT 2017
// Creator: tbritton (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_EventReader_
#define _JEventProcessor_EventReader_

#include <JANA/JEventProcessor.h>
#include <TApplication.h>
#include <TEvePointSet.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TCanvas.h>

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
#include <TGeoNode.h>


//"/group/halld/Software/builds/Linux_CentOS7-x86_64-gcc4.8.5/jana/jana_0.7.7p1/Linux_CentOS7-x86_64-gcc4.8.5/include/JANA/JEventProcessor.h"
extern vector<string> toprint;
extern bool ACTIVATE_ALL;

class JEventProcessor_EventReader:public jana::JEventProcessor{
	public:
		JEventProcessor_EventReader(TGeoNode* node);
		~JEventProcessor_EventReader();
		const char* className(void){return "JEventProcessor_EventReader";}
		void setRootApplication(TApplication *app)
		{
			mApplication = app;
		}

		void setCanvas(TCanvas *cnv)
		{
			canvas = cnv;
		}

		typedef struct{
			string dataClassName;
			string tag;
		}factory_info_t;
		vector<factory_info_t> fac_info;

		TFile *ROOTfile ;
		TH2F* h2;
        TGeoNode* hallD;
        TEvePointSet* FCAL_ps=new TEvePointSet();
        vector<TEvePointSet*> FCAL_points;



	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.
		TCanvas *canvas;
		TApplication *mApplication;
        TEveCaloDataHist* data;
};

#endif // _JEventProcessor_EventReader_

