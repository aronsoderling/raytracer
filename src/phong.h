/*
*
*  Created by Aron Söderling on 2014-03-28.
*
*/
#ifndef PHONG_H
#define PHONG_H

#include "material.h"

class Phong : public Material
{
public:
	Phong(const Color& c, float r = 0.0f, float t = 0.0f, float n = 1.0f);
	Color evalBRDF(const Intersection& is, const Vector3D& L);

protected:
	Color mDiffColor;	
	float specular;
};

#endif
