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

        for(uint i=0;i<TOFPoints.size();i++)
        {
            WritePointJSON(event_out,i,TOFPoints[i]->t,TOFPoints[i]->dE, TOFPoints[i]->tErr,TOFPoints[i]->pos);
            if(i!=TOFPoints.size()-1)
                event_out<<","<<endl;
        }

        event_out<<"]"<<endl;
        event_out.close();

    }

    void Add_TOFHits(vector<const DTOFHit*> TOFHits)
    {

        ofstream event_out;
        event_out.open("../js/event.json", ios::app);//JSON
        event_out<<"\"TOF_hits\": "<<"[\n";//JSON

        for(uint i=0;i<TOFHits.size();i++)
        {
            WriteHitJSON(event_out, i, TOFHits[i]->plane,TOFHits[i]->bar, TOFHits[i]->end, TOFHits[i]->dE, TOFHits[i]->Amp, TOFHits[i]->t_fADC, TOFHits[i]->t_TDC, TOFHits[i]->t, TOFHits[i]->has_fADC, TOFHits[i]->has_TDC);
            if(i!=TOFHits.size()-1)
                event_out<<","<<endl;

        }

        event_out<<"]"<<endl;
        event_out.close();

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
    void WriteHitJSON(ofstream& event_out, int id, int plane,int bar, int end, float dE, float Amp, float t_fADC, float t_TDC, float t, bool has_fADC, bool has_TDC)
    {
        tao::json::value TOFHit({
                                          {"id", id},
                                          {"plane", plane},
                                          {"bar", bar},
                                          {"end", end},
                                          {"dE", dE},
                                          {"Amp", Amp},
                                          //{"t_fADC", t_fADC},
                                          //{"t_TDC", t_TDC},
                                          {"t", t},
                                          {"has_fADC", has_fADC},
                                          {"has_TDC", has_TDC}
                                  });

        event_out << tao::json::to_string(TOFHit, 4);
    }

};
#endif //EVESTANDALONE_TOF_H
