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


            double mass=ChargedTracks[i]->Get_BestTrackingFOM()->mass();
            rt->SetMass(mass);
            //rt.SetMass(TrackCandidates[i]->mass());
            auto PID=ChargedTracks[i]->Get_BestTrackingFOM()->PID();
            auto position = ChargedTracks[i]->Get_BestTrackingFOM()->position();
            auto momentum = ChargedTracks[i]->Get_BestTrackingFOM()->momentum();
            auto charge = ChargedTracks[i]->Get_BestTrackingFOM()->charge();
            auto start_time = ChargedTracks[i]->Get_BestTrackingFOM()->t0();

            double TrackChiSq_NDF=ChargedTracks[i]->Get_Hypothesis(PID)->dChiSq_Track/double(ChargedTracks[i]->Get_Hypothesis(PID)->dNDF_Track);

            rt->Swim(position, momentum, charge);
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt->swim_steps;

            vector<DVector3> track_points;
            vector<double> track_point_times;

            for(int j=0; j<rt->Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;

                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>625 )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                track_points.push_back(step_loc);
                track_point_times.push_back(steps[j].t);//flight time since start
            }

           // event_out<<WriteTrackJSON2(name, momentum,charge,TrackChiSq_NDF, track_points);
            jsonTracks.emplace_back(WriteTrackJSON(name, momentum,charge,TrackChiSq_NDF,start_time, track_points, track_point_times));
            track_points.clear();
            track_point_times.clear();
            //delete track_points;

        }


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
            auto start_time = NeutralTracks[i]->Get_BestFOM()->t0();
            auto momentum=NeutralTracks[i]->Get_BestFOM()->momentum();
            rt->Swim(NeutralTracks[i]->Get_BestFOM()->position(), momentum, NeutralTracks[i]->Get_BestFOM()->charge());
            //rt.Swim(TrackCandidates[i]->position(), TrackCandidates[i]->momentum(), TrackCandidates[i]->charge());
            DReferenceTrajectory::swim_step_t* steps =rt->swim_steps;

            vector<DVector3> track_points;
            vector<double> track_point_times;

            for(int j=0; j<rt->Nswim_steps; j++)
            {
                DVector3 step_loc=steps[j].origin;
                //cout<<i<<"|"<<step_loc.X()<<","<<step_loc.Y()<<","<<step_loc.Z()<<endl;
                if(step_loc.Z()>645/*625*/ )
                    break;

                if(step_loc.Z()<=0)
                    continue;

                track_points.push_back(step_loc);
                track_point_times.push_back(steps[j].t);
            }
            //event_out<<tao::json::to_string(WriteTrackJSON2(name,momentum, 0,-1, track_points), 4);

            jsonTracks.emplace_back(WriteTrackJSON(name,momentum, 0,-1, start_time,track_points,track_point_times));
            track_points.clear();
            track_point_times.clear();
        }


        return jsonTracks;

    }
    tao::json::value WriteTrackJSON( string id, TVector3 momentum, double charge, double TrackChiSq_NDF, double start_time, vector<DVector3> track_points, vector<double> track_point_times)
    {
        tao::json::value track(
                {
                        {"id", id},
                        {"charge", charge},
                        {"TrackChiSq_NDF", TrackChiSq_NDF},
                        {"momentum", momentum.Mag()},
                        {"mass", momentum.Mag()},
                        {"start_time", start_time}

                });

        std::ostringstream ss;
        auto jsonArray = tao::json::value::array({});

        vector<double> vals;

        for (int j = 0; j<track_points.size(); j++) {
            vals.push_back(track_points[j].X());
            jsonArray.emplace_back(tao::json::value::array({track_points[j].X() , track_points[j].Y() , track_points[j].Z(), track_point_times[j]}));

        }

        track["points"] = jsonArray;

        return track;
    }
private:
    DReferenceTrajectory* rt;
};

#endif //EVESTANDALONE_TRACKING_H