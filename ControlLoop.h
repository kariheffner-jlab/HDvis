//
// Created by romanov on 6/1/17.
//

#ifndef EVESTANDALONE_ROOTLOOP_H
#define EVESTANDALONE_ROOTLOOP_H

#include "ApplicationContext.h"
#include "KeyboardControl.h"
#include "JEventProcessor_EventReader.h"
#include "HttpController.h"
#include <mutex>


namespace hdvis
{
    class ControlLoop {
    public:

        ControlLoop():
                _isMultithreaded(false)
        {

            _httpControl.AddApiGetRoute("/api/v1/status", [this](const httplib::Request &req) {
                std::cout<<"/api/v1/status"<<endl;
                std::ostringstream buffer;
                buffer << "{\"janaState\":\""<< _context.JanaStateStr() << "\""
                       << ",\"eventNumber\": " << _context.CurrentEventNumber()
                       << ",\"isAutoPlay\": " << (int)_context.JanaWaitingLogic().IsAutoPlay()
                       <<"}";


                return buffer.str();
            });

            _httpControl.AddApiGetRoute("/api/v1/next", [this](const httplib::Request &req) {
                std::cout<<"/api/v1/next"<<endl;
                _context.JanaWaitingLogic().ProceedToNextEvent();
                std::cout<<"COMMAND ProceedToNextEvent: "<<std::endl;
                return "[1]";
            });

            _httpControl.AddApiGetRoute("/api/v1/autoplay-on", [this](const httplib::Request &req) {
                std::cout<<"/api/v1/autoplay-on"<<endl;
                _context.JanaWaitingLogic().SetAutoPlay(true);
                std::cout<<"COMMAND autoplay: "<<(_context.JanaWaitingLogic().IsAutoPlay()? "on": "off")<<std::endl;
                return "[1]";
            });

            _httpControl.AddApiGetRoute("/api/v1/autoplay-off", [this](const httplib::Request &req) {
                std::cout<<"/api1/autoplay-off"<<endl;
                _context.JanaWaitingLogic().SetAutoPlay(false);
                std::cout<<"COMMAND autoplay: "<<(_context.JanaWaitingLogic().IsAutoPlay()? "on": "off")<<std::endl;
                return "[1]";
            });
        }

        void RunRootAppMultithreaded()
        {
            _isMultithreaded = true;
            _runThread = std::thread([this](){this->RunRootAppThisThread();});
        }


        void RunRootAppThisThread()
        {
            _keyboardControl.PrintMenu();
            _keyboardControl.StartListening();

            _httpControl.StartListening();


            while (!_keyboardControl.IsQuitCommand()) {

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
        HttpController _httpControl;

    };

}



#endif //EVESTANDALONE_ROOTLOOP_H
