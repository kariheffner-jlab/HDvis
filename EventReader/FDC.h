//
// Created by tbritton on 6/20/17.
//

#ifndef EVESTANDALONE_FDC_H
#define EVESTANDALONE_FDC_H

#include <FDC/DFDCHit.h>
#include <FDC/DFDCPseudo.h>
#include <FDC/DFDCWire.h>
#include <FDC/DFDCGeometry.h>



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
            float ucoord=0;
            double angle=0;
            //double uaxis_x=0;
            //double uaxis_y=0;
            //double uaxis_z=0;

            //if cathode
            if(FDCHits[i]->type != 0)
            {
               // 2*((*peak)->gLayer-1)+(1-(*peak)->plane/2
                //std::cout<<"finding strip "<< (2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2)) << ","<<FDCcathodes.size()<<std::endl;
                //std::cout<<FDCHits[i]->gLayer<<","<<FDCHits[i]->layer<<","<<FDCHits[i]->plane<<std::endl;
                ucoord=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->u;
                angle=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->angle;

                /*uaxis_x=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->udir.X();
                uaxis_y=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->udir.Y();
                uaxis_z=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->udir.Z();*/

                //wire_strip_midx=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->origin.x();
                //wire_strip_midy=FDCcathodes[(2*(FDCHits[i]->gLayer-1)+(FDCHits[i]->plane/2))][FDCHits[i]->element-1]->origin.y();


                if(FDCHits[i]->plane==1)
                {
                    wire_strip_midz=FDCwires[FDCHits[i]->gLayer-1][0]->origin.z()-.5;
                   // std::cout<<FDCHits[i]->element-1<<","<<FDCwires[FDCHits[i]->gLayer-1].size()<<endl;
                   // wire_strip_midx=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.x();
                   // std::cout<<"done"<<std::endl;
                    //wire_strip_midy=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.y();
                }
                else
                {
                    wire_strip_midz=FDCwires[FDCHits[i]->gLayer-1][0]->origin.z()+.5;
                    //wire_strip_midx=FDCwires[FDCHits[i]->gLayer-1][FDCwires[FDCHits[i]->gLayer-1].size()-FDCHits[i]->element]->origin.x();
                    //wire_strip_midy=FDCwires[FDCHits[i]->gLayer-1][FDCwires[FDCHits[i]->gLayer-1].size()-FDCHits[i]->element]->origin.y();
                }


                //std::cout<<wire_strip_midx<<","<<wire_strip_midy<<","<<wire_strip_midz<<endl;
            }
            else //if anode
            {
                wire_strip_midx=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.x();
                wire_strip_midy=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.y();
                wire_strip_midz=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->origin.z();
                //ucoord=FDCwires[FDCHits[i]->gLayer-1][FDCHits[i]->element-1]->u;

            }
            auto pulse_height_r = 0.0;
            if(!isnan((FDCHits[i]->pulse_height_raw)))
            {
                pulse_height_r = FDCHits[i]->pulse_height_raw;
            }

            //std::cout<<i<<" , "<< FDCHits[i]->layer<<" , "<< FDCHits[i]->module<<" , "<< FDCHits[i]->element<<" , "<< FDCHits[i]->plane<<" , "<< FDCHits[i]->gPlane<<" , "<< FDCHits[i]->gLayer<<" , "<< FDCHits[i]->q<<" , "<< FDCHits[i]->pulse_height<<" , "<< FDCHits[i]->pulse_height_raw<<" , "<< FDCHits[i]->t<<" , "<< FDCHits[i]->r<<" , "<< FDCHits[i]->d<<" , "<< FDCHits[i]->type<<" , "<<wire_strip_midx<<" , "<<wire_strip_midy<<" , "<<wire_strip_midz<<" , "<<ucoord<<" , "<<angle<<std::endl;
            arr.emplace_back(WriteHitJSON(i, FDCHits[i]->layer, FDCHits[i]->module, FDCHits[i]->element, FDCHits[i]->plane, FDCHits[i]->gPlane, FDCHits[i]->gLayer, FDCHits[i]->q, FDCHits[i]->pulse_height, pulse_height_r, FDCHits[i]->t, FDCHits[i]->r, FDCHits[i]->d, FDCHits[i]->type,wire_strip_midx,wire_strip_midy,wire_strip_midz,ucoord,angle));


        }

      /*  for(int j=0;j<192;j++)
        {
            double wire_strip_midz=FDCwires[0][0]->origin.z()-.5;
            float ucoord=FDCcathodes[0][j]->u;
            arr.emplace_back(WriteHitJSON(j, 1, 0, j+1,1, 1, 1, 0, 0, 0, 0, 0, 0, 1,0,0,wire_strip_midz,ucoord/*,uaxis_x,uaxis_y,uaxis_z));
        }*/
        return arr;
    }

    static tao::json::value Add_FDCPseudos(vector<const DFDCPseudo*> FDCPseudos)
    {
        auto arr = tao::json::value::array({});

        for(uint i=0;i<FDCPseudos.size();i++)
        {
            arr.emplace_back(WritePseudoJSON(i,FDCPseudos[i]->time,FDCPseudos[i]->xy.X(),FDCPseudos[i]->xy.Y(),FDCPseudos[i]->wire->origin.Z(),FDCPseudos[i]->u,FDCPseudos[i]->v,FDCPseudos[i]->t_u, FDCPseudos[i]->t_v, FDCPseudos[i]->dE, FDCPseudos[i]->q, FDCPseudos[i]->wire->layer, FDCPseudos[i]->w, FDCPseudos[i]->s));
        }

        return arr;
    }

    static tao::json::value WriteHitJSON(int id, int layer, int module, int element, int plane, int gPlane, int gLayer, float q, float pulse_height, float pulse_height_raw, float t, float r, float d, int type,double wire_strip_midx, double wire_strip_midy, double wire_strip_midz, float xdispl,double angle)
    {
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
                                        {"u", xdispl},
                                        {"angle", angle},
                                        {"package", (gLayer-1/6)+1},
                                        {"chamber", (gLayer%6)+1}

                                });

        return FDCHit;//event_out << tao::json::to_string(FDCHit, 4);
    }

    static tao::json::value WritePseudoJSON(int id,double time,double x,double y,double z, double u, double v, double u_t, double v_t, double dE, double q, int gLayer, double w, double s)
    {
        tao::json::value FDCPseudo({
                                        {"id", id},
                                        {"time", time},
                                        {"u", u},
                                        {"v", v},
                                        {"u_t", u_t},
                                        {"v_t", v_t},
                                        {"dE", dE},
                                        {"q", q},
                                        {"package", (gLayer-1/6)+1},
                                        {"chamber", (gLayer%6)+1},
                                        {"w", w},
                                        {"s", s}

                                });


        auto jsonposArray = tao::json::value::array({});

        //std::cout<<track_points[j].X()<<" , "<< track_points[j].Y()<<" , "<<track_points[j].Z()<<" , "<<track_point_times[j]<<std::endl;

        jsonposArray.emplace_back(tao::json::value::array({x,y, z}));

        FDCPseudo["position"] = jsonposArray;

        return FDCPseudo;//event_out << tao::json::to_string(FDCHit, 4);
    }


};

#endif //EVESTANDALONE_FDC_H
