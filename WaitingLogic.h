//
// Created by romanov on 5/30/17.
//

#ifndef EVESTANDALONE_WAITINGLOGIC_H
#define EVESTANDALONE_WAITINGLOGIC_H

#include <atomic>
#include <chrono>
#include <thread>

class WaitingLogic {
public:
    WaitingLogic():
            _autoplayDuration(std::chrono::seconds(2)),
            _isAutoplay(false),
            _mustProceedToNextEvent(false),
            _mustQuit(false)
        {

        }

    void ProceedToNextEvent(){_mustProceedToNextEvent = true;}
    void SetAutoPlay(bool value){_isAutoplay = value;}
    bool IsAutoPlay() { return _isAutoplay; }

    void Wait()
    {
        using namespace std::chrono;


        // time of waiting start
        auto start = high_resolution_clock::now();

        // go to semi infinite loop
        while(!_mustQuit)
        {
            std::this_thread::sleep_for(milliseconds(1));

            if(_isAutoplay)
            {
                auto finish = high_resolution_clock::now();
                auto microseconds = duration_cast<milliseconds>(finish-start);

                if (microseconds > _autoplayDuration)
                {
                    return;
                }
            }

            // User (or something) want proceed to the next event
            if(_mustProceedToNextEvent)
            {
                _mustProceedToNextEvent = false;
                return;
            }
        }
    }

private:
    std::chrono::seconds _autoplayDuration;
    std::atomic<bool> _isAutoplay;
    std::atomic<bool> _mustProceedToNextEvent;
    std::atomic<bool> _mustQuit;
};


#endif //EVESTANDALONE_WAITINGLOGIC_H
