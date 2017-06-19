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
        event_out.open("../js/eventw.json", ios::app);//JSON
        event_out<<"\"FCAL_hits\": "<<"[\n";//JSON

        auto FCAL_bs = new TEveBoxSet("FCAL_hits");
        TEveRGBAPalette* pal = new TEveRGBAPalette(-150,150);

        FCAL_bs->SetPalette(pal);
        //FCAL_bs->SetFrame(frm);
        FCAL_bs->Reset(TEveBoxSet::kBT_AABox, kTRUE, 64);

        //h2->Reset();
        for (uint i = 0; i < FCALHits.size(); i++) {

            WriteHitJSON(event_out,i,FCALHits[i]->row,FCALHits[i]->column,FCALHits[i]->x,FCALHits[i]->y,FCALHits[i]->E,FCALHits[i]->t,FCALHits[i]->intOverPeak);
            if(i!=FCALHits.size()-1)
                event_out<<","<<endl;



            FCAL_bs->AddBox(FCALHits[i]->x - 1, FCALHits[i]->y - 1, float_t(500 + 173.9), 2, 2,
                            FCALHits[i]->E * 100/*20*log(FCALHits[i]->E*1000)*/);

            int redness = 255;
            if (abs(FCALHits[i]->t) * 10 < 255)
                redness = int(abs(FCALHits[i]->t) * 10);

            if (FCALHits[i]->t >= 0.0)
                FCAL_bs->DigitColor(UChar_t(redness), 0, 0, 50);
                //FCAL_bs->DigitValue(FCALHits[i]->t);
            else
                FCAL_bs->DigitColor(0, UChar_t(redness), 0, 50);
            //FCAL_bs->DigitColor(0,abs(FCALHits[i]->t),0);


        }
        event_out<<"]"<<endl;
        event_out.close();
        gEve->AddElement(FCAL_bs);
    }
    void Add_FCALShowers(vector<const DFCALShower *> FCALShowers)
    {

        vector<TEveArrow*> FCAL_showers;

        for(uint i=0;i<FCALShowers.size();i++)
        {
            //auto FCAL_shower=new TEveJetCone(Form("FCAL Shower %d", i), "");
            TEveVector show_pos;
            show_pos.fX=float(FCALShowers[i]->getPosition().X());
            show_pos.fY=float(FCALShowers[i]->getPosition().Y());
            show_pos.fZ=float(FCALShowers[i]->getPosition().Z());//float(500 + 173.9);

            double z_offset=395;  //This balances the arrow properly...touch at your own risk

            auto FCAL_shower=new TEveArrow(0,0,show_pos.fZ-1400+z_offset,show_pos.fX,show_pos.fY,show_pos.fZ+z_offset);
            FCAL_shower->SetElementName(Form("FCAL Shower %i", i));
            FCAL_shower->SetTubeR(0);

            double redness;
            if(FCALShowers[i]->getEnergy()/2.0>1.0)
                redness=0;
            else
                redness=(5.0-(FCALShowers[i]->getEnergy()/2.0));

            //std::cout<<redness<<endl;
            //FCAL_shower->SetMainColorRGB(UChar_t(250),0,0);
            FCAL_shower->SetMainColorRGB(5.0, float(redness), 0);
            FCAL_shower->SetMainAlpha(.5);

            FCAL_showers.push_back(FCAL_shower);
        }

        for(uint i=0;i<FCAL_showers.size();i++)
        {
            gEve->AddElement(FCAL_showers[i]);
        }
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
private:

};
#endif //EVESTANDALONE_FCAL_H
