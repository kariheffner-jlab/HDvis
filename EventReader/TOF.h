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
        ofstream event_out;
        event_out.open("../js/event.json", ios::app);//JSON
        event_out<<"\"TOF_points\": "<<"[\n";//JSON

        TGeoNode* hallnode= gGeoManager->GetTopVolume()->FindNode("HALL_1");


        TEvePointSet* TOFPoint_ps=new TEvePointSet();
        TOFPoint_ps->SetMainColorRGB(float(255.),255.,255.);

        for(uint i=0;i<TOFPoints.size();i++)
        {
            WritePointJSON(event_out,i,TOFPoints[i]->t,TOFPoints[i]->dE, TOFPoints[i]->tErr,TOFPoints[i]->pos);
            if(i!=TOFPoints.size()-1)
                event_out<<","<<endl;

            DVector3 pos=TOFPoints[i]->pos;
            TOFPoint_ps->SetName("TOF Points");

            TOFPoint_ps->SetNextPoint(pos.X(),pos.Y(),pos.Z());
        }

        event_out<<"]"<<endl;
        event_out.close();

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
    void WritePointJSON(ofstream& event_out, int id, double t, double dE, double tErr, DVector3 fPosition)
    {
        tao::json::value TOFPoint({
                                            {"id", id},
                                            {"t", t},
                                            {"dE", dE},
                                            {"tErr", tErr},
                                            {"x", fPosition.X()},
                                            {"y", fPosition.Y()},
                                            {"z", fPosition.Z()}
                                    });

        event_out << tao::json::to_string(TOFPoint, 4);
    }
private:

};
#endif //EVESTANDALONE_TOF_H
