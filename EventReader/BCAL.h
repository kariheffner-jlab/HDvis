//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_BCAL_H
#define EVESTANDALONE_BCAL_H

#include <BCAL/DBCALHit.h>
#include <BCAL/DBCALShower.h>
#include <BCAL/DBCALPoint.h>

class BCAL
{
public:

    static tao::json::value Add_BCALHits(vector<const DBCALHit*> BCALHits)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<BCALHits.size();i++)
        {
            arr.emplace_back(WriteHitJSON(i, BCALHits[i]->module, BCALHits[i]->layer, BCALHits[i]->sector, BCALHits[i]->end, BCALHits[i]->pulse_peak, BCALHits[i]->E, BCALHits[i]->t, BCALHits[i]->t_raw, BCALHits[i]->cellId));

        }
        return arr;
    }
    static tao::json::value Add_BCALPoints(vector<const DBCALPoint*> BCALPoints)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<BCALPoints.size();i++)
        {

            arr.emplace_back(WritePointJSON(i,BCALPoints[i]->r(), BCALPoints[i]->phi(), BCALPoints[i]->z(), BCALPoints[i]->E(), BCALPoints[i]->t(), BCALPoints[i]->module() ));

        }
        return arr;
    }
    static tao::json::value Add_BCALShowers(vector<const DBCALShower*> BCALShowers)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<BCALShowers.size();i++)
        {

            arr.emplace_back(WriteShowerJSON(i,BCALShowers[i]->x, BCALShowers[i]->y, BCALShowers[i]->z, BCALShowers[i]->E,  BCALShowers[i]->t ));
        }
        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int module, int layer, int sector, DBCALGeometry::End end, int pulse_peak, float E, float t, float t_raw, int cellId)
    {
        return tao::json::value({
                                        {"id", id},
                                        {"module", module},
                                        {"layer", layer},
                                        {"sector", sector},
                                        {"end", int(end)},
                                        {"pulse_peak", pulse_peak},
                                        {"E", E},
                                        {"t", t},
                                        {"t_raw", t_raw},
                                        {"cellId", cellId}
                                });
    }
    static tao::json::value WritePointJSON(int id,double r, double theta, double z, double E, double t, int module )
    {

        return tao::json::value({
                                        {"id", id},
                                        {"z", z},
                                        {"r", r},
                                        {"theta", theta},
                                        {"E", E},
                                        {"t", t},
                                        {"module", module}

                                });
    }

static tao::json::value WriteShowerJSON(int id,double x, double y, double z, double E, double t )
{

    return tao::json::value({
                                    {"id", id},
                                    {"z", z},
                                    {"x", x},
                                    {"y", y},
                                    {"E", E},
                                    {"t", t}

                            });
}

};
#endif //EVESTANDALONE_BCAL_H
