#ifndef EMISSIVEMATERIAL_H
#define EMISSIVEMATERIAL_H

#include "material.h"
#include "color.h"
#include "intersection.h"
#include "defines.h"

class EmissiveMaterial : public Material{
public:
	EmissiveMaterial(const Color& c) : Material(0,0,0), color(c) { }
	~EmissiveMaterial() { }

	/// Returns the BRDF at the intersection is for the light direction L.
	virtual Color evalBRDF(const Intersection& is, const Vector3D& L){ return color; };

protected:
	Color color;			///< Optional name of EmissiveMaterial.
};

#endif
