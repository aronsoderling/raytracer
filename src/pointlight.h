/*
 *  pointlight.h
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-02-28.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "node.h"

/**
 * Class representing a simple point light source.
 * The light source has a position, color and intensity.
 * The getRadiance() function returns the color multiplied
 * by the intensity, and the getWorldPosition() function
 * returns the position of the light source in world coordinates.
 */
class PointLight : public Node
{
public:
	PointLight();
	PointLight(const Point3D& p, const Color& c, float i=1.0f);
	virtual ~PointLight();

	void setPosition(const Point3D& p) { mPosition = p; }
	void setColor(const Color& c) { mColor = c; }
	void setIntensity(float i) { mIntensity = i; }
	Color getRadiance() const { return mRadiance; }
	Point3D getWorldPosition() const { return mWorldPos; }

protected:
	void prepare();

protected:
	float mIntensity;		///< Intensity (multiplication factor).
	Color mColor;			///< Color of light.
	Point3D mPosition;		///< Position of light (local space).
	Color mRadiance;		///< Cached intensity*color.
	Point3D mWorldPos;		///< Cached world position.
};

#endif
