//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_BCAL_H
#define EVESTANDALONE_BCAL_H

#include <BCAL/DBCALHit.h>
#include <BCAL/DBCALShower.h>
#include <BCAL/DBCALPoint.h>
#include <math.h>

class BCAL {
public:

    static tao::json::value Add_BCALHits(vector<const DBCALHit *> BCALHits) {
        auto arr = tao::json::value::array({});

        for (uint i = 0; i < BCALHits.size(); i++) {
            arr.emplace_back(
                    WriteHitJSON(i, BCALHits[i]->module, BCALHits[i]->layer, BCALHits[i]->sector, BCALHits[i]->end,
                                 BCALHits[i]->pulse_peak, BCALHits[i]->E, BCALHits[i]->t, BCALHits[i]->t_raw,
                                 BCALHits[i]->cellId));

        }
        return arr;
    }

    static tao::json::value Add_BCALPoints(vector<const DBCALPoint *> BCALPoints) {
        auto arr = tao::json::value::array({});

        for (uint i = 0; i < BCALPoints.size(); i++) {

            arr.emplace_back(
                    WritePointJSON(i, BCALPoints[i]->r(), BCALPoints[i]->phi(), BCALPoints[i]->z(), BCALPoints[i]->E(),
                                   BCALPoints[i]->t(), BCALPoints[i]->module(), BCALPoints[i]->E_US(),
                                   BCALPoints[i]->E_DS(), BCALPoints[i]->t_US(), BCALPoints[i]->t_DS(), BCALPoints[i]->layer(),BCALPoints[i]->sector()));

        }

        return arr;
    }

    static tao::json::value Add_BCALShowers(vector<const DBCALShower *> BCALShowers) {
        auto arr = tao::json::value::array({});

        for (uint i = 0; i < BCALShowers.size(); i++) {
            arr.emplace_back(WriteShowerJSON(i,BCALShowers[i]->x, BCALShowers[i]->y, BCALShowers[i]->z, BCALShowers[i]->E,  BCALShowers[i]->t, BCALShowers[i]->E_preshower, BCALShowers[i]->N_cell ));
        }
        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int module, int layer, int sector, DBCALGeometry::End end, int pulse_peak, float E, float t,
                 float t_raw, int cellId) {
        tao::json::value BCALHit({
                                         {"id",         id},
                                         {"module",     module},
                                         {"layer",      layer},
                                         {"sector",     sector},
                                         {"end",        int(end)},
                                         {"pulse_peak", pulse_peak},
                                         {"E",          E},
                                         {"t",          t},
                                         {"t_raw",      t_raw},
                                         {"cellId",     cellId}
                                 });

        return BCALHit;
    }

    static tao::json::value WritePointJSON(int id, double r, double theta, double z, double E, double t, int module, double E_US, double E_DS,
                   double t_US, double t_DS, int layer, int sector) {

        tao::json::value BCALPoint({
                                           {"id",     id},
                                           {"E",      E},
                                           {"E_US",   E_US},
                                           {"E_DS",   E_DS},
                                           {"t",      t},
                                           {"t_US",   t_US},
                                           {"t_DS",   t_DS},
                                           {"module", module},
                                           {"layer", layer},
                                           {"sector", sector}

                                   });
        auto jsonposArray = tao::json::value::array({});

        //std::cout<<track_points[j].X()<<" , "<< track_points[j].Y()<<" , "<<track_points[j].Z()<<" , "<<track_point_times[j]<<std::endl;

        jsonposArray.emplace_back(tao::json::value::array({r * cos(theta), r * sin(theta), z}));


        BCALPoint["position"] = jsonposArray;

        return BCALPoint;
    }

static tao::json::value WriteShowerJSON(int id,double x, double y, double z, double E, double t, double Epreshower, int Ncell )
{

   tao::json::value BCALShower({
                                    {"id", id},
                                    {"E", E},
                                    {"Epreshower", Epreshower},
                                    {"t", t},
                                    {"Ncell", Ncell}

                            });

    auto jsonposArray = tao::json::value::array({});


    jsonposArray.emplace_back(tao::json::value::array({x , y , z}));


    BCALShower["position"] = jsonposArray;

    return BCALShower;
}

};
#endif //EVESTANDALONE_BCAL_H
