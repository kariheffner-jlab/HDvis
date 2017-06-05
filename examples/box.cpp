/// \file
/// \ingroup tutorial_eve
/// Demonstrates usage of TEveBox class.
///
/// \image html eve_box.png
/// \macro_code
///
/// \author Matevz Tadel

#include <TEveManager.h>
#include <TApplication.h>
#include <TEveBox.h>
#include <TRandom.h>

TEveBox* box(Float_t a=10, Float_t d=5, Float_t x=0, Float_t y=0, Float_t z=0)
{
    TEveManager::Create();

    TRandom& r = * gRandom;

    TEveBox* b = new TEveBox;
    b->SetMainColor(kCyan);
    b->SetMainTransparency(0);

#define RND_BOX(x) r.Uniform(-(x), (x))
    b->SetVertex(0, x - a + RND_BOX(d), y - a + RND_BOX(d), z - a + RND_BOX(d));
    b->SetVertex(1, x - a + RND_BOX(d), y + a + RND_BOX(d), z - a + RND_BOX(d));
    b->SetVertex(2, x + a + RND_BOX(d), y + a + RND_BOX(d), z - a + RND_BOX(d));
    b->SetVertex(3, x + a + RND_BOX(d), y - a + RND_BOX(d), z - a + RND_BOX(d));
    b->SetVertex(4, x - a + RND_BOX(d), y - a + RND_BOX(d), z + a + RND_BOX(d));
    b->SetVertex(5, x - a + RND_BOX(d), y + a + RND_BOX(d), z + a + RND_BOX(d));
    b->SetVertex(6, x + a + RND_BOX(d), y + a + RND_BOX(d), z + a + RND_BOX(d));
    b->SetVertex(7, x + a + RND_BOX(d), y - a + RND_BOX(d), z + a + RND_BOX(d));
#undef RND_BOX

    gEve->AddElement(b);
    gEve->Redraw3D(kTRUE);

    return b;
}


int main(int narg, char* argv[])
{
    using namespace std;
    auto gApp = new TApplication("Hahaha it works!", &narg, argv);

    box();

    gApp->Run();
    return 0;
}