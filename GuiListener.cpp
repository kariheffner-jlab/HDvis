#include "GuiListener.h"

#include <iostream>


ClassImp(GuiListener)


void GuiListener::OnNextButtonPressed()
{
    WhenNextEventFired();
}

void GuiListener::OnAutoplayCheckedChanged(Bool_t newValue)
{
    WhenAutoplayCheckedChanged(newValue);

}