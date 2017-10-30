//
// Created by tbritton on 5/31/17.
//

#ifndef EVESTANDALONE_FCAL_H
#define EVESTANDALONE_FCAL_H

#include <TEveBoxSet.h>
#include <TEveManager.h>
#include <TEveArrow.h>
#include <TRandom3.h>
#include <FCAL/DFCALHit.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALCluster.h>

#include <tao/json.hpp>
#include <tao/json/from_string.hpp>
#include <tao/json/value.hpp>
class FCAL
{
public:

    static tao::json::value Add_FCALHits(vector<const DFCALHit *> FCALHits)
    {
        auto arr = tao::json::value::array({});

        for (uint i = 0; i < FCALHits.size(); i++) {
            arr.emplace_back(WriteHitJSON(i,FCALHits[i]->row,FCALHits[i]->column,FCALHits[i]->x,FCALHits[i]->y,FCALHits[i]->E,FCALHits[i]->t,FCALHits[i]->intOverPeak));
        }
        return arr;
    }

    static tao::json::value Add_FCALShowers(vector<const DFCALShower *> FCALShowers)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<FCALShowers.size();i++)
        {
            arr.emplace_back(WriteShowerJSON(i,FCALShowers[i]->getEnergy(),FCALShowers[i]->getTime(),FCALShowers[i]->getPosition()));
        }

        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int row,  int column, float x, float y, float E, float t, float intOverPeak)
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

    static tao::json::value WriteShowerJSON(int id, double fEnergy, double fTime, DVector3 fPosition)
    {
        tao::json::value FCALShower({
                                         {"id", id},
                                         {"fEnergy", fEnergy},
                                         {"fTime", fTime},

                                 });

        auto jsonposArray = tao::json::value::array({});

        //std::cout<<track_points[j].X()<<" , "<< track_points[j].Y()<<" , "<<track_points[j].Z()<<" , "<<track_point_times[j]<<std::endl;
        jsonposArray.emplace_back(tao::json::value::array({fPosition.X() , fPosition.Y() , fPosition.Z()}));


        FCALShower["position"] = jsonposArray;

        return FCALShower;// << tao::json::to_string(FCALShower, 4);

    }
private:

};
#endif //EVESTANDALONE_FCAL_H
