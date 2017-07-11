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

    string Add_FCALHits(vector<const DFCALHit *> FCALHits)
    {
        ostringstream event_out;
        event_out<<"\"FCAL_hits\": "<<"[\n";//JSON

        for (uint i = 0; i < FCALHits.size(); i++) {

            event_out<<tao::json::to_string(WriteHitJSON(i,FCALHits[i]->row,FCALHits[i]->column,FCALHits[i]->x,FCALHits[i]->y,FCALHits[i]->E,FCALHits[i]->t,FCALHits[i]->intOverPeak),4);
            if(i!=FCALHits.size()-1)
                event_out<<","<<endl;

        }
        event_out<<"]"<<endl;
        string outstr = event_out.str();
        return outstr;


    }
    string Add_FCALShowers(vector<const DFCALShower *> FCALShowers)
    {
        ostringstream event_out;
        event_out<<"\"FCAL_showers\": "<<"[\n";//JSON

        for(uint i=0;i<FCALShowers.size();i++)
        {
            event_out<<WriteShowerJSON(i,FCALShowers[i]->getEnergy(),FCALShowers[i]->getTime(),FCALShowers[i]->getPosition());
            if(i!=FCALShowers.size()-1)
                event_out<<","<<endl;
        }

        event_out<<"]"<<endl;
        string outstr = event_out.str();
        return outstr;
    }

    tao::json::value WriteHitJSON(int id, int row,  int column, float x, float y, float E, float t, float intOverPeak)
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

        return FCALHit;//event_out << tao::json::to_string(FCALHit, 4);

    }
    tao::json::value WriteShowerJSON(int id, double fEnergy, double fTime, DVector3 fPosition)
    {
        tao::json::value FCALShower({
                                         {"id", id},
                                         {"fEnergy", fEnergy},
                                         {"fTime", fTime},
                                         {"x", fPosition.X()},
                                         {"y", fPosition.Y()},
                                         {"z", fPosition.Z()}
                                 });

        return FCALShower;// << tao::json::to_string(FCALShower, 4);

    }
private:

};
#endif //EVESTANDALONE_FCAL_H
