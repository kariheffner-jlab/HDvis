//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_FDC_H
#define EVESTANDALONE_FDC_H

#include <FDC/DFDCHit.h>
#include <FDC/DFDCPseudo.h>
#include <FDC/DFDCWire.h>


class FDC
{
public:

    static tao::json::value Add_FDCHits(vector<const DFDCHit*> FDCHits,vector<vector<DFDCWire*> >FDCwires,vector<vector<DFDCCathode*> >FDCcathodes)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<FDCHits.size();i++)
        {
            double wire_strip_midx=0;
            double wire_strip_midy=0;
            double wire_strip_midz=0;
            float xdispl=0;

            //if cathode
            if(FDCHits[i]->type != 0)
            {
               // 2*((*peak)->gLayer-1)+(1-(*peak)->plane/2
                //std::cout<<"finding strip "<< (2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2)) << ","<<FDCcathodes.size()<<std::endl;
                //std::cout<<FDCHits[i]->gLayer<<","<<FDCHits[i]->layer<<","<<FDCHits[i]->plane<<std::endl;
                xdispl=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->u;

               // wire_strip_midx=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->origin.x();
               // wire_strip_midy=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->origin.y();
               // wire_strip_midz=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->origin.z();

                //std::cout<<wire_strip_midx<<","<<wire_strip_midy<<","<<wire_strip_midz<<endl;
            }
            else //if anode
            {
                wire_strip_midx=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.x();
                wire_strip_midy=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.y();
                wire_strip_midz=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.z();

            }


            arr.emplace_back(WriteHitJSON(i, FDCHits[i]->layer, FDCHits[i]->module, FDCHits[i]->element, FDCHits[i]->plane, FDCHits[i]->gPlane, FDCHits[i]->gLayer, FDCHits[i]->q, FDCHits[i]->pulse_height, FDCHits[i]->pulse_height_raw, FDCHits[i]->t, FDCHits[i]->r, FDCHits[i]->d, FDCHits[i]->type,wire_strip_midx,wire_strip_midy,wire_strip_midz,xdispl));
        }

        return arr;
    }

    static tao::json::value Add_FDCPseudos(vector<const DFDCPseudo*> FDCPseudos)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<FDCPseudos.size();i++)
        {
            arr.emplace_back(WritePseudoJSON(i,FDCPseudos[i]->time,FDCPseudos[i]->xy.X(),FDCPseudos[i]->xy.Y(),FDCPseudos[i]->wire->origin.Z()));
        }

        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int layer, int module, int element, int plane, int gPlane, int gLayer, float q, float pulse_height, float pulse_height_raw, float t, float r, float d, int type,double wire_strip_midx, double wire_strip_midy, double wire_strip_midz, float xdispl)
    {
        if(layer==2) {
            std::cout << wire_strip_midz << std::endl;
        }
        tao::json::value FDCHit({
                                        {"id", id},
                                        {"layer", layer},
                                        {"module", module},
                                        {"element", element},
                                        {"plane", plane},
                                        {"gPlane", gPlane},
                                        {"gLayer", gLayer},
                                        {"q", q},
                                        {"pulse_height", pulse_height},
                                        {"pulse_height_raw", pulse_height_raw},
                                        {"t", t},
                                        {"r", r},
                                        {"d", d},
                                        {"type", type},
                                        {"midx", wire_strip_midx},
                                        {"midy", wire_strip_midy},
                                        {"midz", wire_strip_midz},
                                        {"u", xdispl}

                                });

        return FDCHit;//event_out << tao::json::to_string(FDCHit, 4);
    }

    static tao::json::value WritePseudoJSON(int id,double time,double x,double y,double z)
    {
        tao::json::value FDCPseudo({
                                        {"id", id},
                                        {"time", time},
                                        {"x", x},
                                        {"y", y},
                                        {"z", z}

                                });

        return FDCPseudo;//event_out << tao::json::to_string(FDCHit, 4);
    }


};

#endif //EVESTANDALONE_FDC_H
