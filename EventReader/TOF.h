//
// Created by tbritton on 6/1/17.
//

#ifndef EVESTANDALONE_TOF_H
#define EVESTANDALONE_TOF_H

#include <TOF/DTOFPoint.h>
#include <TOF/DTOFHit.h>
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

    void Add_TOFHits(vector<const DTOFHit*> TOFHits)
    {

        auto globalScene = gEve->GetGlobalScene();
        for(uint i=0;i<TOFHits.size();i++)
        {
            if(i==0)
                std::cout<<"Hits"<<std::endl;

            auto tofpad = globalScene->FindChild("SITE_1")->FindChild("HALL_1")->FindChild("FTOF_1")->FindChild("FTON_1")->FindChild("yd13_1");
            tofpad->SetMainColorRGB(UChar_t(255),0,0);
            tofpad->SetMainAlpha(0);

        }

    }
private:

};
#endif //EVESTANDALONE_TOF_H
