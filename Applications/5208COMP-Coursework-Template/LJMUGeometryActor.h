#pragma once
#include "Application.h"
#include <GeometryActor.h>
#include <SamplerStateConfigDX11.h>
#include <RasterizerStateConfigDX11.h>

using namespace Glyph3;

class LJMUGeometryActor : public GeometryActor
{
public:
    //Constructors
    LJMUGeometryActor(); //standard empty constructor
    void UseLitTextureMaterial(ResourcePtr texture);
    void UseSkyBoxMaterial(ResourcePtr texture);
    MaterialPtr m_pLitTextureMaterial;

    MaterialPtr m_pSkyBoxMaterial;
protected:
    MaterialPtr LJMUGeometryActor::createLitTexturedMaterial();

    MaterialPtr LJMUGeometryActor::SkyBoxmaterial();


};