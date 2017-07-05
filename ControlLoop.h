//
// Created by romanov on 6/1/17.
//

#ifndef EVESTANDALONE_ROOTLOOP_H
#define EVESTANDALONE_ROOTLOOP_H

#include "ApplicationContext.h"
#include "KeyboardControl.h"

#include <mutex>


namespace hdvis
{
    class ControlLoop {
    public:

        ControlLoop():
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
            while (!_keyboardControl.IsQuitCommand()) {

                _keyboardControl.PrintMenu();
                _keyboardControl.StartListening();

                //while(!gEventMutex.try_lock()) std::this_thread::yield();

                //std::lock_guard<std::mutex> eventMutexLockGuard(ApplicationContext::InnerLoopMutex);
                auto lock = std::unique_lock<std::mutex>(ApplicationContext::InnerLoopMutex);

                try {
                    //LOOP?
                    if(_keyboardControl.IsNextCommand()) {
                        _keyboardControl.ClearNextCommand();

                        _context.JanaWaitingLogic().ProceedToNextEvent();
                        std::cout<<"COMMAND ProceedToNextEvent: "<<std::endl;
                    }

                    if(_keyboardControl.IsAutoplayCommand()) {
                        _keyboardControl.ClearAutoplayCommand();

                        _context.JanaWaitingLogic().SetAutoPlay(!_context.JanaWaitingLogic().IsAutoPlay());
                        std::cout<<"COMMAND autoplay: "<<(_context.JanaWaitingLogic().IsAutoPlay()? "on": "off")<<std::endl;
                    }


                }
                catch(...)
                {
                    std::cerr<<"InnerLoop exception is throwed"<<std::endl;
                }
            }
        }


        ApplicationContext& Context() { return _context; }

    private:
        ApplicationContext _context;
        std::thread _runThread;
        std::atomic<bool> _isMultithreaded;
        KeyboardControl _keyboardControl;

    };

}



#endif //EVESTANDALONE_ROOTLOOP_H
