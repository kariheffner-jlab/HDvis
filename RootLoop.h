//
// Created by romanov on 6/1/17.
//

#ifndef EVESTANDALONE_ROOTLOOP_H
#define EVESTANDALONE_ROOTLOOP_H

#include "RootLoopCommander.h"

namespace hdvis
{
    class RootLoop {
    public:

        RootLoop()
        {

        }

        RootLoopCommander& Commands()
        {
            return _commands;
        }

    private:
        RootLoopCommander _commands;




    };

}



#endif //EVESTANDALONE_ROOTLOOP_H
