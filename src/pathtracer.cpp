/*
 *  Pathtracer.cpp
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-03-10.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#include "defines.h"
#include "scene.h"
#include "camera.h"
#include "ray.h"
#include "intersection.h"
#include "material.h"
#include "pathtracer.h"
#include "timer.h"
#include "image.h"
#include <omp.h>

const float nbrSamples = 100.0;
const float samplesPerAxis = 10.0;
const int iSamplesPerAxis = 10;
const bool sampling = true;
const int maxDepth = 4;
const float p_abs = 0.1f;
const float abs_factor = 1.0f / (1.0f - p_abs);

/**
 * Creates a Path raytracer. The parameters are passed on to the base class constructor.
 */
PathTracer::PathTracer(Scene* scene, Image* img) : Raytracer(scene,img)
{
}


PathTracer::~PathTracer()
{
}

/**
 * Raytraces the scene by calling tracePixel() for each pixel in the output image.
 * The function tracePixel() is responsible for computing the output color of the
 * pixel. This should be done in a sub class derived from the Raytracer base class.
 */
void PathTracer::computeImage()
{
	std::cout << "Raytracing..." << std::endl;
	Timer timer;
	
	Color c;
	int width = mImage->getWidth();
	int height = mImage->getHeight();
	
	// Loop over all pixels in the image
	/*for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Raytrace the pixel at (x,y).
			
			c = tracePixel(x,y);
			// Store the result in the image.
			mImage->setPixel(x,y,c);
		}
		
		// Print progress approximately every 5%.
		if ((y + 1) % (height / 20) == 0 || (y + 1) == height){
			std::cout << (100 * (y + 1) / height) << "% done" << std::endl;
		}
	}*/

	int lines = 0;
	#pragma omp parallel for 
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Color c = tracePixel(x, y);
				mImage->setPixel(x, y, c);
			}
	#pragma omp critical 
			{
				lines++;

				if (lines % (height / 20) == 0 || lines == height)
					std::cout << (100 * lines / height) << "%" << std::endl; 
			}
		}

	//std::cout << "Total number of rays: " << nbrRays << std::endl;
	std::cout << "Done in: " << timer.stop() << " seconds" << std::endl;
}

/**
 * Compute the color of the pixel at (x,y) by raytracing. 
 * The default implementation here just traces through the center of
 * the pixel.
 */
Color PathTracer::tracePixel(int x, int y)
{
	Color pixelColor = Color(0.0f, 0.0f, 0.0f);

	//super sampling, samples / pixel
	for (int i = 0; i < iSamplesPerAxis; ++i){
		for (int j = 0; j < iSamplesPerAxis; ++j){
			float cx = (float)x + j / samplesPerAxis + uniform() / samplesPerAxis;
			float cy = (float)y + i / samplesPerAxis + uniform() / samplesPerAxis;
			Ray ray = mCamera->getRay(cx, cy);
			pixelColor += trace(ray, 0);
		}
	}
	return pixelColor / nbrSamples;
}

/**
 * Computes the radiance returned by tracing the ray r.
 */
Color PathTracer::trace(const Ray& ray, int depth)
{
	Color colorOut = Color(0.0f,0.0f,0.0f);
	Intersection is;
	if (mScene->intersect(ray, is)){
		Color reflectedC, refractedC, lDirect, lIndirect;
		float type = uniform();
		
		float reflectivity = is.mMaterial->getReflectivity(is);
		float transparency = is.mMaterial->getTransparency(is);
	
		if (type <= reflectivity){
			colorOut = trace(is.getReflectedRay(), depth + 1);
		}
		else if (type - reflectivity <= transparency){
			colorOut = trace(is.getRefractedRay(), depth + 1);
		}
		else{
			
			for (int i = 0; i < mScene->getNumberOfLights(); ++i){
				PointLight* l = mScene->getLight(i);
				if (!mScene->intersect(is.getShadowRay(l))){
					Vector3D lightVec = l->getWorldPosition() - is.mPosition;
					float d2 = lightVec.length2();
					lightVec.normalize();
					Color radiance = l->getRadiance();
					Color brdf = is.mMaterial->evalBRDF(is, lightVec);
					float angle = max(lightVec * is.mNormal, 0.0f);
					lDirect += radiance * brdf * angle / d2;
				}
			}

			if (depth < maxDepth || uniform() > p_abs){
				float theta = acos(sqrt(1 - uniform()));
				float phi = 2 * M_PI * uniform();
				float x = sin(theta) * cos(phi);
				float y = sin(theta) * sin(phi);
				float z = cos(theta);

				Vector3D nvec(1.0f, 0.0f, 0.0f);
				Vector3D mvec(0.0f, 1.0f, 0.0f);

				Vector3D W = is.mNormal;
				W.normalize();
				Vector3D U = nvec % W;
				if (U.length() < 0.01f)
					U = mvec % W;
				Vector3D V = W % U;

				Vector3D dir = x * U + y * V + z * W;

				Ray ray2;
				ray2.orig = is.mPosition;
				ray2.dir = dir;
				if (is.mMaterial->isEmissive()){
					lDirect = is.mMaterial->evalBRDF(is, dir);
				}
				else{
					lIndirect = M_PI * trace(ray2, depth + 1)* is.mMaterial->evalBRDF(is, dir);
				}
				if (depth > maxDepth)
					lIndirect *= abs_factor;
			}
			colorOut = lDirect + lIndirect;
		}			
	}
return colorOut;	
}

