//
// Created by romanov on 6/1/17.
//

#ifndef EVESTANDALONE_ROOTLOOPTASKS_H
#define EVESTANDALONE_ROOTLOOPTASKS_H


#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include "WaitingLogic.h"

namespace hdvis {

    class ApplicationContext {
    public:
        /// Mutex between ROOT graphical thread and EvenRead function
        static std::mutex InnerLoopMutex;


        ApplicationContext()
                {
        };




        WaitingLogic& JanaWaitingLogic() { return _waitingLogic; }


    protected:

    private:
        WaitingLogic _waitingLogic;



    };
}

#endif //EVESTANDALONE_ROOTLOOPTASKS_H
