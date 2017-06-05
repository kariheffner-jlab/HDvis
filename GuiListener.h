//
// Created by romanov on 5/30/17.
//

#ifndef EVESTANDALONE_GUILISTENER_H
#define EVESTANDALONE_GUILISTENER_H
#include "TObject.h"
#include <functional>
#include <iostream>

class GuiListener {
public:
    void OnNextButtonPressed();
    void OnAutoplayCheckedChanged(Bool_t newValue);


    std::function<void()> WhenNextEventFired = [](){std::cout<<"WOOOOOOOOOOORRRRRKKKKKKSSSSSS AAAAAA !!!!!!!!!11111oneoneone"<<std::endl;};

    std::function<void(bool)> WhenAutoplayCheckedChanged = [](bool t){std::cout<<"Autoplay changed to: "<<t<<std::endl;};

ClassDef(GuiListener,2)
};


#endif //EVESTANDALONE_GUILISTENER_H
