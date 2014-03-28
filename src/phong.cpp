/*
*
*  Created by Aron Söderling on 2014-03-28.
*
*/

#include "defines.h"
#include "intersection.h"
#include "phong.h"

using namespace std;

Phong::Phong(const Color& c, int s, float r, float t, float n) : Material(r, t, n), mDiffColor(c), specular(s)
{
}

/**
* Returns the BRDF at the intersection is for the light direction L.
*/
Color Phong::evalBRDF(const Intersection& is, const Vector3D& L)
{
	Vector3D H = (L + is.mView).normalize();
	Color diffuseColor = mDiffColor / M_PI;
	Color specularColor = Color(1.0f, 1.0f, 1.0f);
	//max(((specular + 4) / 8 * M_PI)
	//float spec = pow((H * is.mNormal), 100.0f);
	//Color s = is.mMaterial->getReflectivity(is) * Color(spec, spec, spec);
	return (1 - is.mMaterial->getReflectivity(is) - is.mMaterial->getTransparency(is)) * diffuseColor + is.mMaterial->getReflectivity(is) *specularColor * ((specular + 4) / (M_PI * 8)) *pow((H * is.mNormal), specular);
}


