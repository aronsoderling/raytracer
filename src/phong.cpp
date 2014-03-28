/*
*
*  Created by Aron Söderling on 2014-03-28.
*
*/

#include "defines.h"
#include "intersection.h"
#include "phong.h"

using namespace std;

Phong::Phong(const Color& c, float r, float t, float n) : Material(r, t, n), mDiffColor(c)
{
}

/**
* Returns the BRDF at the intersection is for the light direction L.
*/
Color Phong::evalBRDF(const Intersection& is, const Vector3D& L)
{
	Vector3D H = (L + is.mView).normalize();
	float ndoth = max((H * is.mNormal), 1.0f);
	Color diffuse = mDiffColor / M_PI;
	//max(((specular + 4) / 8 * M_PI)
	float spec = pow(ndoth, 100.0f);
	Color s = is.mMaterial->getReflectivity(is) * Color(spec, spec, spec);
	return diffuse + s;
}


