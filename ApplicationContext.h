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

    enum class JanaStates {
        Startup,
        ProcessingEvent,
        Idle
    };

    class ApplicationContext {
    public:
        /// Mutex between ROOT graphical thread and EvenRead function
        static std::mutex InnerLoopMutex;


        ApplicationContext():
                _janaState(JanaStates::Startup)
        {
        };

        WaitingLogic& JanaWaitingLogic() { return _waitingLogic; }


        std::string JanaStateStr() const
        {
            JanaStates state = _janaState;

            if (state == JanaStates::Startup) return "Startup";

            if (state == JanaStates::ProcessingEvent) return "ProcessingEvent";

            if (state == JanaStates::Idle) return "Idle";

            return std::string();
        }

        JanaStates JanaState() const {return _janaState; }
        void SetJanaState(JanaStates state) { _janaState = state; }

        uint64_t CurrentEventNumber() const {return _currentEventNumber; }
        void SetCurrentEventNumber(uint64_t eventNumber) {_currentEventNumber = eventNumber; }


    protected:

    private:
        WaitingLogic _waitingLogic;
        std::atomic<JanaStates> _janaState;
        std::atomic<uint64_t> _currentEventNumber;



    };
}

#endif //EVESTANDALONE_ROOTLOOPTASKS_H
