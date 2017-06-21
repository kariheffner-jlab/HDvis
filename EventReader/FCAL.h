//
// Created by tbritton on 5/31/17.
//

#ifndef EVESTANDALONE_FCAL_H
#define EVESTANDALONE_FCAL_H

#include <TEveBoxSet.h>
#include <TEveManager.h>
#include <TEveArrow.h>
#include <TRandom3.h>

#include <tao/json.hpp>
#include <tao/json/from_string.hpp>
#include <tao/json/value.hpp>
class FCAL
{
public:

    void Add_FCALHits(vector<const DFCALHit *> FCALHits)
    {
        ofstream event_out;
        event_out.open("../js/event.json", ios::app);//JSON
        event_out<<"\"FCAL_hits\": "<<"[\n";//JSON

        for (uint i = 0; i < FCALHits.size(); i++) {

            WriteHitJSON(event_out,i,FCALHits[i]->row,FCALHits[i]->column,FCALHits[i]->x,FCALHits[i]->y,FCALHits[i]->E,FCALHits[i]->t,FCALHits[i]->intOverPeak);
            if(i!=FCALHits.size()-1)
                event_out<<","<<endl;

        }
        event_out<<"]"<<endl;
        event_out.close();
        //gEve->AddElement(FCAL_bs);
    }
    void Add_FCALShowers(vector<const DFCALShower *> FCALShowers)
    {
        ofstream event_out;
        event_out.open("../js/event.json", ios::app);//JSON
        event_out<<"\"FCAL_showers\": "<<"[\n";//JSON

        for(uint i=0;i<FCALShowers.size();i++)
        {
            WriteShowerJSON(event_out,i,FCALShowers[i]->getEnergy(),FCALShowers[i]->getTime(),FCALShowers[i]->getPosition());
            if(i!=FCALShowers.size()-1)
                event_out<<","<<endl;
        }

        event_out<<"]"<<endl;
        event_out.close();
    }
    void WriteHitJSON(ofstream& event_out, int id, int row,  int column, float x, float y, float E, float t, float intOverPeak)
    {
        tao::json::value FCALHit({
                                        {"id", id},
                                        {"row", row},
                                        {"column", column},
                                        {"x", x},
                                        {"y", y},
                                        {"E", E},
                                        {"t", t},
                                        {"intOverPeak", intOverPeak}
                                });

        event_out << tao::json::to_string(FCALHit, 4);

    }
    void WriteShowerJSON(ofstream& event_out, int id, double fEnergy, double fTime, DVector3 fPosition)
    {
        tao::json::value FCALShower({
                                         {"id", id},
                                         {"fEnergy", fEnergy},
                                         {"fTime", fTime},
                                         {"x", fPosition.X()},
                                         {"y", fPosition.Y()},
                                         {"z", fPosition.Z()}
                                 });

        event_out << tao::json::to_string(FCALShower, 4);

    }
private:

};
#endif //EVESTANDALONE_FCAL_H
