#include <thread>

#include <TFile.h>
#include <TApplication.h>

#include <TEveCalo.h>
#include <TEveWindow.h>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TEveScene.h>
#include <TEveViewer.h>
#include <TGeoManager.h>
#include <TEveGeoNode.h>
#include <TEveGedEditor.h>
#include <TGTab.h>
#include <iostream>
#include <TRint.h>
#include "TGDMLWrite.h"


using namespace std;

//TGeoManager * hddsroot();
TGeoManager * getGeom();

string OUTPUT_FILENAME = "hd_root.root";
string COMMAND_LINE_OUTPUT_FILENAME = "";
bool filename_from_command_line = false;

void ParseCommandLineArguments(int &narg, char *argv[]);
void DecideOutputFilename(void);
void Usage(void);

TApplication *gApp;

void RunRootApp()
{
    gApp->Run();
}


/*
////////////////////////////////////////////////////////////////////////////////
/// Chooses the object and method that should be used for processing object

XMLNodePointer_t TGDMLWrite::ChooseObject(TGeoShape *geoShape)
{
    const char * clsname = geoShape->ClassName();
    XMLNodePointer_t solidN;

    if (CanProcess((TObject *)geoShape) == kFALSE) {
        return NULL;
    }

    //process different sh apes
    if (strcmp(clsname, "TGeoBBox") == 0) {
        solidN = CreateBoxN((TGeoBBox*) geoShape);
    } else if (strcmp(clsname, "TGeoParaboloid") == 0) {
        solidN = CreateParaboloidN((TGeoParaboloid*) geoShape);
    } else if (strcmp(clsname, "TGeoSphere") == 0) {
        solidN = CreateSphereN((TGeoSphere*) geoShape);
    } else if (strcmp(clsname, "TGeoArb8") == 0) {
        solidN = CreateArb8N((TGeoArb8*) geoShape);
    } else if (strcmp(clsname, "TGeoConeSeg") == 0) {
        solidN = CreateConeN((TGeoConeSeg*) geoShape);
    } else if (strcmp(clsname, "TGeoCone") == 0) {
        solidN = CreateConeN((TGeoCone*) geoShape);
    } else if (strcmp(clsname, "TGeoPara") == 0) {
        solidN = CreateParaN((TGeoPara*) geoShape);
    } else if (strcmp(clsname, "TGeoTrap") == 0) {
        solidN = CreateTrapN((TGeoTrap*) geoShape);
    } else if (strcmp(clsname, "TGeoGtra") == 0) {
        solidN = CreateTwistedTrapN((TGeoGtra*) geoShape);
    } else if (strcmp(clsname, "TGeoTrd1") == 0) {
        solidN = CreateTrdN((TGeoTrd1*) geoShape);
    } else if (strcmp(clsname, "TGeoTrd2") == 0) {
        solidN = CreateTrdN((TGeoTrd2*) geoShape);
    } else if (strcmp(clsname, "TGeoTubeSeg") == 0) {
        solidN = CreateTubeN((TGeoTubeSeg*) geoShape);
    } else if (strcmp(clsname, "TGeoCtub") == 0) {
        solidN = CreateCutTubeN((TGeoCtub*) geoShape);
    } else if (strcmp(clsname, "TGeoTube") == 0) {
        solidN = CreateTubeN((TGeoTube*) geoShape);
    } else if (strcmp(clsname, "TGeoPcon") == 0) {
        solidN = CreatePolyconeN((TGeoPcon*) geoShape);
    } else if (strcmp(clsname, "TGeoTorus") == 0) {
        solidN = CreateTorusN((TGeoTorus*) geoShape);
    } else if (strcmp(clsname, "TGeoPgon") == 0) {
        solidN = CreatePolyhedraN((TGeoPgon*) geoShape);
    } else if (strcmp(clsname, "TGeoEltu") == 0) {
        solidN = CreateEltubeN((TGeoEltu*) geoShape);
    } else if (strcmp(clsname, "TGeoHype") == 0) {
        solidN = CreateHypeN((TGeoHype*) geoShape);
    } else if (strcmp(clsname, "TGeoXtru") == 0) {
        solidN = CreateXtrusionN((TGeoXtru*) geoShape);
    } else if (strcmp(clsname, "TGeoScaledShape") == 0) {
        TGeoScaledShape * geoscale = (TGeoScaledShape *) geoShape;
        TString scaleObjClsName = geoscale->GetShape()->ClassName();
        if (scaleObjClsName == "TGeoCone") {
            solidN = CreateElConeN((TGeoScaledShape*) geoShape);
        } else {
            Info("ChooseObject",
                 "ERROR! TGeoScaledShape object is not possible to process correctly. %s object is processed without scale",
                 scaleObjClsName.Data());
            solidN = ChooseObject(geoscale->GetShape());
            //Name has to be propagated to geoscale level pointer
            fNameList->fLst[TString::Format("%p", geoscale)] =
                    fNameList->fLst[TString::Format("%p", geoscale->GetShape())];
        }
    } else if (strcmp(clsname, "TGeoCompositeShape") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (strcmp(clsname, "TGeoUnion") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (strcmp(clsname, "TGeoIntersection") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else if (strcmp(clsname, "TGeoSubtraction") == 0) {
        solidN = CreateCommonBoolN((TGeoCompositeShape*) geoShape);
    } else {
        Info("ChooseObject", "ERROR! %s Solid CANNOT be processed, solid is NOT supported",
             clsname);
        solidN = NULL;
    }
    if (solidN == NULL) {
        if (fNameList->fLst[TString::Format("%p", geoShape)] == "") {
            TString missingName = geoShape->GetName();
            GenName("missing_" + missingName, TString::Format("%p", geoShape));
        } else {
            fNameList->fLst[TString::Format("%p", geoShape)] = "missing_" + fNameList->fLst[TString::Format("%p", geoShape)];
        }
    }

    return solidN;
}
*/


void replaceAll( string &s, const string &search, const string &replace ) {
    for( size_t pos = 0; ; pos += replace.length() ) {
        // Locate the substring to replace
        pos = s.find( search, pos );
        if( pos == string::npos ) break;
        // Replace by erasing and inserting
        s.erase( pos, search.length() );
        s.insert( pos, replace );
    }
}

////////////////////////////////////////////////////////////////////////////////
///NCNAME basic restrictions
///Replace "$" character with empty character etc.

string GenName(string& oldname)
{
    string newname(oldname);
    replaceAll(newname, "$", "");
    replaceAll(newname, " ", "_");
    replaceAll(newname, ":", "");
    replaceAll(newname, "@", "");
    replaceAll(newname, "%", "");
    replaceAll(newname, "&", "");
    replaceAll(newname, "/", "");
    replaceAll(newname, "+", "");
    replaceAll(newname, ";", "");
    replaceAll(newname, "{", "");
    replaceAll(newname, "}", "");
    replaceAll(newname, "(", "");
    replaceAll(newname, ")", "");
    replaceAll(newname, "[", "");
    replaceAll(newname, "]", "");
    replaceAll(newname, "_refl", "");
    //workaround if first letter is digit than replace it to "O" (ou character)
    return newname;
}


////////////////////////////////////////////////////////////////////////////////
/// Check for null parameter to skip the NULL objects

Bool_t IsNullParam(Double_t parValue, TString parName, TString objName)
{
    if (parValue == 0.) {
        Info("IsNullParam", "ERROR! %s is NULL due to %s = %.12g, Volume based on this shape will be skipped",
             objName.Data(),
             parName.Data(),
             parValue);
        return kTRUE;
    }
    return kFALSE;
}


////////////////////////////////////////////////////////////////////////////////
/// Creates "box" node for GDML

void CreateBoxN(TGeoBBox * geoShape)
{
    //XMLNodePointer_t mainN = fGdmlE->NewChild(0, 0, "box", 0);

    string lname = geoShape->GetName();
    lname = GenName(lname);


    if (IsNullParam(geoShape->GetDX(), "DX", lname) ||
        IsNullParam(geoShape->GetDY(), "DY", lname) ||
        IsNullParam(geoShape->GetDZ(), "DZ", lname)) {
        return;
    }

    cout<<"x = "<<geoShape->GetDX()<<" y = "<<geoShape->GetDY()<<" z = "<<geoShape->GetDZ()<<endl;

}

void ProcessObject(TGeoShape *geoShape) {
    string clsname = geoShape->ClassName();
    XMLNodePointer_t solidN;



    //process different sh apes
    if ( clsname == "TGeoBBox") {
        CreateBoxN((TGeoBBox *) geoShape);
    } //else if (strcmp(clsname, "TGeoParaboloid") == 0) {
        //solidN = CreateParaboloidN((TGeoParaboloid *) geoShape);
    //}
}
static const UInt_t fgkProcBit    = BIT(14);    //14th bit is set when solid is processed
static const UInt_t fgkProcBitVol = BIT(19);    //19th bit is set when volume is processed
static const UInt_t fgkMaxNameErr = 5;          //maximum number of errors for naming

vector<string> fNameList;

struct Xyz {
    Double_t x;
    Double_t y;
    Double_t z;
};


////////////////////////////////////////////////////////////////////////////////
/// Retrieves X Y Z angles from rotation matrix

Xyz GetXYZangles(const Double_t * rotationMatrix)
{
    Xyz lxyz;
    Double_t a, b, c;
    Double_t rad = 180.0 / TMath::ACos(-1.0);
    const Double_t *r = rotationMatrix;
    Double_t cosb = TMath::Sqrt(r[0] * r[0] + r[1] * r[1]);
    if (cosb > 0.00001) {
        a = TMath::ATan2(r[5], r[8]) * rad;
        b = TMath::ATan2(-r[2], cosb) * rad;
        c = TMath::ATan2(r[1], r[0]) * rad;
    } else {
        a = TMath::ATan2(-r[7], r[4]) * rad;
        b = TMath::ATan2(-r[2], cosb) * rad;
        c = 0;
    }
    lxyz.x = a;
    lxyz.y = b;
    lxyz.z = c;
    return lxyz;
}
////////////////////////////////////////////////////////////////////////////////
/// Method extracting geometry structure recursively

void ExtractVolumes(TGeoVolume* volume)
{

    string volname, matname, solname, pattClsName, nodeVolNameBak;
    TGeoPatternFinder *pattFinder = 0;
    Bool_t isPattern = kFALSE;

    //create the name for volume/assebmly
    if (volume->IsTopVolume()) {
        //not needed a special function for generating name
        volname = volume->GetName();
        cout<<"fTopVolumeName ="<< volname<<endl;
        //register name to the pointer
        fNameList.push_back(volname);
    } else {
        volname = volume->GetName();
        volname = GenName(volname);
    }
    cout<<"volname ="<< volname<<endl;

    //start to create main volume/assembly node
    if (volume->IsAssembly()) {
        cout<<"   volumeN = StartAssemblyN(volname);"<<endl;
    } else {
        //get reference material and add solid to <solids> + get name
        matname = volume->GetMaterial()->GetName();
        cout<<"   matname = "<<matname<<endl;

        string clsname(volume->GetShape()->ClassName());
        //ProcessValues(volume->GetShape())
        cout<<"   clsname = "<<clsname<<endl;

        /*
        volumeN = StartVolumeN(volname, solname, matname);

        //divisionvol can't be in assembly
        pattFinder = volume->GetFinder();
        //if found pattern
        if (pattFinder) {
            pattClsName = TString::Format("%s", pattFinder->ClassName());
            TString shapeCls = TString::Format("%s", volume->GetShape()->ClassName());
            //if pattern in accepted pattern list and not in shape rejected list
            if ((fAccPatt->fLst[pattClsName] == kTRUE) &&
                (fRejShape->fLst[shapeCls] != kTRUE)) {
                isPattern = kTRUE;
            }
        }*/
    }
    //get all nodes in volume
    TObjArray *nodeLst = volume->GetNodes();
    TIter next(nodeLst);
    TGeoNode *geoNode;
    Int_t nCnt = 0;
    //loop through all nodes
    while ((geoNode = (TGeoNode *) next())) {
        //get volume of current node and if not processed then process it
        TGeoVolume * subvol = geoNode->GetVolume();
        if (subvol->TestAttBit(fgkProcBitVol) == kFALSE) {
            subvol->SetAttBit(fgkProcBitVol);
            ExtractVolumes(subvol);
        }

        //volume of this node has to exist because it was processed recursively
        string nodevolname = geoNode->GetVolume()->GetName();
        if (nodevolname.find("missing_")!= -1) {
            continue;
        }
        if (nCnt == 0) { //save name of the first node for divisionvol
            nodeVolNameBak = nodevolname;
        }

        if (isPattern == kFALSE) {
            //create name for node
            string nodename, posname, rotname;
            nodename = geoNode->GetName();
            nodename = GenName(nodename);
            nodename = nodename + "in" + volname;
            cout<<"  nodename = "<<nodename<<endl;

            //create name for position and clear rotation
            posname = nodename + "pos";
            rotname = "";

            //position
            const Double_t * pos = geoNode->GetMatrix()->GetTranslation();
            Xyz nodPos;
            nodPos.x = pos[0];
            nodPos.y = pos[1];
            nodPos.z = pos[2];
            //childN = CreatePositionN(posname.Data(), nodPos);
            //fGdmlE->AddChild(fDefineNode, childN); //adding node to <define> node

            //Deal with reflection

            Double_t lx, ly, lz;
            Double_t xangle = 0;
            Double_t zangle = 0;
            lx = geoNode->GetMatrix()->GetRotationMatrix()[0];
            ly = geoNode->GetMatrix()->GetRotationMatrix()[4];
            lz = geoNode->GetMatrix()->GetRotationMatrix()[8];
            if (geoNode->GetMatrix()->IsReflection()
                && TMath::Abs(lx) == 1 &&  TMath::Abs(ly) == 1 && TMath::Abs(lz) == 1) {
                /* TODO scaleN = fGdmlE->NewChild(0, 0, "scale", 0);
                fGdmlE->NewAttr(scaleN, 0, "name", (nodename + "scl").Data());
                fGdmlE->NewAttr(scaleN, 0, "x", TString::Format("%.12g", lx));
                fGdmlE->NewAttr(scaleN, 0, "y", TString::Format("%.12g", ly));
                fGdmlE->NewAttr(scaleN, 0, "z", TString::Format("%.12g", lz));
                 */
                cout << "   geoNode->GetMatrix()->IsReflection() &&..."<<endl;
                //experimentally found out, that rotation should be updated like this
                if (lx == -1) {
                    zangle = 180;
                }
                if (lz == -1) {
                    xangle = 180;
                }
            }

            //rotation
            Xyz lxyz = GetXYZangles(geoNode->GetMatrix()->GetRotationMatrix());
            lxyz.x -= xangle;
            lxyz.z -= zangle;
            if ((lxyz.x != 0.0) || (lxyz.y != 0.0) || (lxyz.z != 0.0)) {
                rotname = nodename + "rot";
                cout<<"   rotation "<<rotname<<" lxyz.x="<<lxyz.x<<" lxyz.y"<<lxyz.y<<" lxyz.z"<<lxyz.z<<endl;
            }

            cout<<"   Add child?"<<endl;
        }
        nCnt++;
    }
    //create only one divisionvol node
    if (isPattern && pattFinder) {
        //retrieve attributes of division
        cout<<"isPattern && pattFinder"<<endl;
//        Int_t ndiv, divaxis;
//        Double_t offset, width, xlo, xhi;
//        TString axis, unit;
//
//        ndiv = pattFinder->GetNdiv();
//        width = pattFinder->GetStep();
//
//        divaxis = pattFinder->GetDivAxis();
//        volume->GetShape()->GetAxisRange(divaxis, xlo, xhi);
//
//        //compute relative start (not positional)
//        offset = pattFinder->GetStart() - xlo;
//        axis = GetPattAxis(divaxis, pattClsName, unit);
//
//        //create division node
//        childN = CreateDivisionN(offset, width, ndiv, axis.Data(), unit.Data(), nodeVolNameBak.Data());
//        fGdmlE->AddChild(volumeN, childN);
    }

    //fVolCnt++;
    //add volume/assembly node into the <structure> node
    //fGdmlE->AddChild(fStructureNode, volumeN);

}
//-----------
// main
//-----------
int main(int narg, char *argv[])
{
	// Create ROOT application 
	// Instantiate an event loop object

	gApp = new TApplication("Hahaha it works!", &narg, argv);

    new TGeoManager("GLUEX", "GlueX Geometry");

    gGeoManager=getGeom();

    gGeoManager->Print();
    //gGeoManager->Export("/home/romanov/gluex/hdvis/github/halld.gdml");
    //TGDMLWrite::StartGDMLWriting(gGeoManager,"", "vg");
    //ExtractVolumes(gGeoManager->GetTopVolume());

    gGeoManager->SetVisLevel(40);
    gGeoManager->Export("ha.gdml");


    //gGeoManager->GetTopVolume()->Draw("ogle");

    //gApp->Run(false);
}
