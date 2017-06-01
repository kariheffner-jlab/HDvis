//
// Created by romanov on 6/1/17.
//

#ifndef EVESTANDALONE_ROOTLOOPTASKS_H
#define EVESTANDALONE_ROOTLOOPTASKS_H


#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>

namespace hdvis {

    class RootLoopCommander {
    public:
        /// Mutex between ROOT graphical thread and EvenRead function
        static std::mutex InnerLoopMutex;


        RootLoopCommander() :
                _mustRedraw3D(false) {
        }

        /// Root GUI loop will
        void EveFullRedraw3D() {
            _mustRedraw3D = true;
        }

        bool IsPendingRedraw3D() const
        {
            return _mustRedraw3D;
        }

        bool ClearRedraw3D()  {
            _mustRedraw3D = false;
        }

    protected:
        std::atomic<bool> _mustRedraw3D;


    };
}

#endif //EVESTANDALONE_ROOTLOOPTASKS_H
