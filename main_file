#include <iostream>
//#include "HDvis.h"
//#include <JANA/JEventProcessor.h>
//#include <JANA/JApplication.h>
#include "EventReader/JEventProcessor_EventReader.h"
#include <TApplication.h>

#include "DANA/DApplication.h"


int main(int narg, char* argv[])
{
  using namespace std;

  // create an instance of TApplication

   //auto gMyRootApp = new TApplication("My ROOT Application", &narg, argv);

   // tell application to return from run
   //gMyRootApp->SetReturnFromRun(true);

   // Instantiate our event processor
	auto myproc = new JEventProcessor_EventReader();
  
  cout<<"hello"<<endl;

  // Instantiate an event loop object
	DApplication app(narg, argv);

  //jana::JApplication japp(narg, argv);

  // Run though all events, calling our event processor's methods
	//app.monitor_heartbeat = 0;
	//app.Run(myproc);
	
	//delete myproc;
	
	//return japp.GetExitCode();

}
