#ifndef EVESTANDALONE_KEYBOARDCONTROL_H
#define EVESTANDALONE_KEYBOARDCONTROL_H

#include <iostream>
#include <memory>
#include <string>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

class KeyboardControl
{
public:

    KeyboardControl():
        _isQuitCommand(false),
        _isNextCommand(false),
        _isAutoplayCommand(false)
    {

    }

    void PrintMenu() {
        std::cout<<"a - autoplay, n - next, q - quit"<<std::endl;
    }

    void StartListening() {


        auto io_thread = std::thread([&]{
            char nextChar;
            bool error = false;
            while(!error && (nextChar = std::getchar()))
            {

                std::cout<<"getchar code: "<<(int)nextChar<<" char: '"<<nextChar<<"'"<<endl;

                switch (nextChar){
                    case 'n':
                        _isNextCommand = true;
                        break;
                    case 'a':
                        _isAutoplayCommand = true;
                        break;
                    case 'q':
                        _isQuitCommand = true;
                        break;
                }
            }
        });

        io_thread.join();
    }

    bool IsQuitCommand() {return _isQuitCommand;}
    void ClearQuitCommand() {_isQuitCommand = false; }

    bool IsNextCommand() {return _isNextCommand;}
    void ClearNextCommand() {_isNextCommand = false; }

    bool IsAutoplayCommand() {return _isAutoplayCommand;}
    void ClearAutoplayCommand() {_isAutoplayCommand = false; }

private:
    std::atomic<bool> _isQuitCommand;
    std::atomic<bool> _isNextCommand;
    std::atomic<bool> _isAutoplayCommand;

};

#endif //EVESTANDALONE_KEYBOARDCONTROL_H
