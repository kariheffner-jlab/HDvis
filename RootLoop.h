//
// Created by romanov on 6/1/17.
//

#ifndef EVESTANDALONE_ROOTLOOP_H
#define EVESTANDALONE_ROOTLOOP_H

#include "RootLoopCommander.h"

#include <TApplication.h>
#include <TSystem.h>

#include <mutex>


namespace hdvis
{
    class RootLoop {
    public:

        RootLoop():
                _isMultithreaded(false)
        {


        }

        void RunRootAppMultithreaded()
        {
            _isMultithreaded = true;
            _runThread = std::thread([this](){this->RunRootAppThisThread();});
        }


        void RunRootAppThisThread()
        {
            // Here is what happens in gApp->Run();
            // 3 root functions should be called in this order:
            // gApplication->StartIdleing() -> gSystem->InnerLoop() -> gApplication->StartIdleing()
            // In this function we want to pause ROOT graphical thread, run EventRead, continue ROOT graph. thread
            // In order to do it we use gEventMutex

            gApplication->StartIdleing();

            while (1) {

                //while(!gEventMutex.try_lock()) std::this_thread::yield();

                std::lock_guard<std::mutex> eventMutexLockGuard(RootLoopCommander::InnerLoopMutex, std::adopt_lock);

                gSystem->InnerLoop();
                gApplication->StopIdleing();
                gApplication->StartIdleing();

                if(_commands.IsPendingRedraw3D())
                {
                    _commands.ClearRedraw3D();
                    gEve->RegisterRedraw3D();
                }
            }
        }


        RootLoopCommander& Commander()
        {
            return _commands;
        }

    private:
        RootLoopCommander _commands;
        std::thread _runThread;
        std::atomic<bool> _isMultithreaded;

    };

}



#endif //EVESTANDALONE_ROOTLOOP_H
