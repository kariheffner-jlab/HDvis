//
// Created by tbritton on 5/31/17.
//

#include <TRACKING/DReferenceTrajectory.h>
#include <PID/DChargedTrack.h>
#include <PID/DNeutralParticle.h>

#include <fstream>
#include <tao/json.hpp>
#include <tao/json/from_string.hpp>
#include <tao/json/value.hpp>

#ifndef EVESTANDALONE_TRACKING_H
#define EVESTANDALONE_TRACKING_H



class Tracking
{
public:

    Tracking(DMagneticFieldMap* Bfield, DGeometry* Geom)//DGeometry
    {
        int RMAX_INTERIOR=65;
        int RMAX_EXTERIOR=89;
        rt=new DReferenceTrajectory(Bfield);
        rt->Rsqmax_interior = RMAX_INTERIOR*RMAX_INTERIOR;//innerBCAL radius
        rt->Rsqmax_exterior = RMAX_EXTERIOR*RMAX_EXTERIOR;

        rt->SetDRootGeom(NULL);
        rt->SetDGeometry(Geom);
    };


    tao::json::value Add_DChargedTracks(vector<const DChargedTrack*> ChargedTracks)
    {
        auto jsonTracks = tao::json::value::array({});

        for(int i=0;i<ChargedTracks.size()/*TrackCandidates.size()*/;i++)
        {

            string PID_name=ParticleType(ChargedTracks[i]->Get_BestTrackingFOM()->PID());
            string name=PID_name + Form(" Track Points %i", i);
            //cout<<name<<endl;
            rt->Reset();
            //auto Track_ps = new TEvePointSet();

            rt->SetMass(ChargedTracks[i]->Get_BestTrackingFOM()->mass());
            //rt.SetMass(TrackCandidates[i]->mass());
            auto PID=ChargedTracks[i]->Get_BestTrackingFOM()->PID();
            auto position = ChargedTracks[i]->Get_BestTrackingFOM()->position();
            auto momentum = ChargedTracks[i]->Get_BestTrackingFOM()->momentum();
            auto charge = ChargedTracks[i]->Get_BestTrackingFOM()->charge();

            double TrackChiSq_NDF=ChargedTracks[i]->Get_Hypothesis(PID)->dChiSq_Track/double(ChargedTracks[i]->Get_Hypothesis(PID)->dNDF_Track);

            rt->Swim(position, momentum, charge);
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt->swim_steps;

            vector<DVector3> track_points;

            for(int j=0; j<rt->Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;

                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>625 )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                track_points.push_back(step_loc);

            }

           // event_out<<WriteTrackJSON2(name, momentum,charge,TrackChiSq_NDF, track_points);
            jsonTracks.emplace_back(WriteTrackJSON2(name, momentum,charge,TrackChiSq_NDF, track_points));
            track_points.clear();
            //delete track_points;

           /* if(i!=ChargedTracks.size()-1)
                event_out<<"},"<<endl;
            else
                event_out<<"}"<<endl;*/


        }

        //event_out<<"]"<<endl;
        //string outstr = event_out.str();
        //return tao::json::to_string(tao::json::value( {{ "charged_tracks", jsonTracks }}), 4);

        return jsonTracks;


    }

    tao::json::value Add_DNeutralParticles(vector<const DNeutralParticle*> NeutralTracks)
    {
        //ostringstream event_out;
       // event_out<<"\"neutral_tracks\": "<<"[\n";//JSON
        auto jsonTracks = tao::json::value::array({});

        for(int i=0;i<NeutralTracks.size()/*TrackCandidates.size()*/;i++)
        {

            string PID_name=ParticleType(NeutralTracks[i]->Get_BestFOM()->PID());
            string name=PID_name + Form(" Track Points %i", i);
            //cout<<name<<endl;
            rt->Reset();

            rt->SetMass(NeutralTracks[i]->Get_BestFOM()->mass());
            //rt.SetMass(TrackCandidates[i]->mass());
            auto momentum=NeutralTracks[i]->Get_BestFOM()->momentum();
            rt->Swim(NeutralTracks[i]->Get_BestFOM()->position(), momentum, NeutralTracks[i]->Get_BestFOM()->charge());
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt->swim_steps;

            vector<DVector3> track_points;

            for(int j=0; j<rt->Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;
                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>625 )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                track_points.push_back(step_loc);

            }
            //event_out<<tao::json::to_string(WriteTrackJSON2(name,momentum, 0,-1, track_points), 4);

            jsonTracks.emplace_back(WriteTrackJSON2(name,momentum, 0,-1, track_points));
            track_points.clear();

            //if(i!=NeutralTracks.size()-1)
             //   event_out<<"},"<<endl;
            //else
             //   event_out<<"}"<<endl;

        }

        //event_out<<"]"<<endl;
        //string outstr = event_out.str();
        //return tao::json::to_string(tao::json::value( { {"neutral_tracks", jsonTracks} }), 4);
        return jsonTracks;

    }
    void WriteTrackJSON(ofstream& event_out, string id, TVector3 momentum, double charge, double TrackChiSq_NDF, vector<DVector3> track_points)
    {

        if(track_points.size()!=0) {
            event_out << "{" << endl;
            event_out << "\"id\": " <<"\""<<id<<"\"" << "," << endl; //JSON
            event_out << "\"charge\": " << charge << "," << endl; //JSON
            event_out << "\"TrackChiSq_NDF\": " << TrackChiSq_NDF << "," << endl; //JSON
            event_out << "\"momentum\": " <<momentum.Mag()<<","<<endl;//<< "[" << momentum.X() << "," << momentum.Y() << "," << momentum.Z() << "],"<<endl; //JSON
            event_out << "\"points\": " << "[" << endl; //JSON

            for (int j = 0; j<track_points.size(); j++) {

                if (j != track_points.size()-1) {
                    event_out << "[" << track_points[j].X() << "," << track_points[j].Y() << "," << track_points[j].Z()
                              << "],"
                              << endl; //JSON
                } else {
                    event_out << "[" << track_points[j].X() << "," << track_points[j].Y() << "," << track_points[j].Z()
                              << "]" << endl; //JSON
                }
            }

            event_out<<"]"<<endl; //JSON
        }
    }
    tao::json::value WriteTrackJSON2( string id, TVector3 momentum, double charge, double TrackChiSq_NDF, vector<DVector3> track_points)
    {
        tao::json::value track(
                {
                        {"id", id},
                        {"charge", charge},
                        {"TrackChiSq_NDF", TrackChiSq_NDF},
                        {"momentum", momentum.Mag()}

                });

        std::ostringstream ss;
        auto jsonArray = tao::json::value::array({});


        vector<double> vals;

        for (int j = 0; j<track_points.size(); j++) {
            vals.push_back(track_points[j].X());
            jsonArray.emplace_back(tao::json::value::array({track_points[j].X() , track_points[j].Y() , track_points[j].Z()}));

//
//            if (j != track_points.size()-1) {
//                ss << "[" << track_points[j].X() << "," << track_points[j].Y() << "," << track_points[j].Z() << "],";
//            } else {
//                ss << "[" << track_points[j].X() << "," << track_points[j].Y() << "," << track_points[j].Z() << "]"; //JSON
//            }
        }



        track["points"] = jsonArray;

        return track;
    }
private:
    DReferenceTrajectory* rt;
};

#endif //EVESTANDALONE_TRACKING_H