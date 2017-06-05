//
// Created by tbritton on 6/1/17.
//

#ifndef EVESTANDALONE_TOF_H
#define EVESTANDALONE_TOF_H

#include <TOF/DTOFPoint.h>
#include <TEvePointSet.h>
#include <TEveManager.h>
#include <TGeoManager.h>

class TOF
{
public:
    void Add_TOFPoints(vector<const DTOFPoint*> TOFPoints)
    {
        TGeoNode* hallnode= gGeoManager->GetTopVolume()->FindNode("HALL_1");


        TEvePointSet* TOFPoint_ps=new TEvePointSet();
        TOFPoint_ps->SetMainColorRGB(float(255.),255.,255.);

        for(uint i=0;i<TOFPoints.size();i++)
        {
            DVector3 pos=TOFPoints[i]->pos;
            TOFPoint_ps->SetName("TOF Points");

            TOFPoint_ps->SetNextPoint(pos.X(),pos.Y(),pos.Z());
        }
        gEve->AddElement(TOFPoint_ps);
    }
private:

};
#endif //EVESTANDALONE_TOF_H
