/*
 *  PhotonMapper.cpp
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
#include "photonmapper.h"
#include "timer.h"
#include "image.h"
#include "lightprobe.h"
#include "bvhhitpointaccelerator.h"
#include <omp.h>

const float nbrSamples = 4.0;
const float samplesPerAxis = 2.0;
const int iSamplesPerAxis = 2;
const bool sampling = true;
const int maxDepth = 4;
const float p_abs = 0.1f;
const float abs_factor = 1.0f / (1.0f - p_abs);
const float startRadius = 0.9f;
const int numberPhotons = 100000;

/**
 * Creates a Path raytracer. The parameters are passed on to the base class constructor.
 */

using namespace std;

PhotonMapper::PhotonMapper(Scene* scene, Image* img) : Raytracer(scene,img)
{
}


PhotonMapper::~PhotonMapper()
{
}

/**
 * Raytraces the scene by calling tracePixel() for each pixel in the output image.
 * The function tracePixel() is responsible for computing the output color of the
 * pixel. This should be done in a sub class derived from the Raytracer base class.
 */
void PhotonMapper::computeImage()
{
	std::cout << "Raytracing..." << std::endl;
	Timer timer;
	
	Color c;
	
	cout << "Starting forward pass" << endl;
	forwardPass();
	cout << "Forward pass done" << endl;
	for (int i = 0; i < 10; ++i){
		photonTracingPass();
		output(i);
	}
	

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

	//std::cout << "Total number of rays: " << nbrRays << std::endl;
	std::cout << "Done in: " << timer.stop() << " seconds" << std::endl;
}

void PhotonMapper::forwardPass(){
	int width = mImage->getWidth();
	int height = mImage->getHeight();

	int lines = 0;
//	#pragma omp parallel for 
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				forwardPassPixel(x, y);
				//mImage->setPixel(x, y, c);
			}
//	#pragma omp critical 
//			{
				lines++;

				//if (lines % (height / 20) == 0 || lines == height)
				//	std::cout << (100 * lines / height) << "%" << std::endl;
//			}
		}

	cout << "Building BVH" << endl;
	hitpointBVH.build(vec);
	//hitpointBVH.objs = vec;
	cout << "Building BVH done" << endl;
	//bvh.print();
}

void PhotonMapper::forwardPassPixel(int x, int y){
	for (int i = 0; i < iSamplesPerAxis; ++i){
		for (int j = 0; j < iSamplesPerAxis; ++j){
			float cx = (float)x + j / samplesPerAxis + uniform() / samplesPerAxis;
			float cy = (float)y + i / samplesPerAxis + uniform() / samplesPerAxis;
			Ray ray = mCamera->getRay(cx, cy);
			
			Intersection is;
			if (mScene->intersect(ray, is)){
				Hitpoint* hp = new Hitpoint();
				hp->pixelX = x;
				hp->pixelY = y;
				hp->is = is;
				hp->radius = startRadius;
				hp->pixelWeight = 1 / nbrSamples;
				
				for (int i = 0; i < mScene->getNumberOfLights(); ++i){
					PointLight* l = mScene->getLight(i);
					if (!mScene->intersect(is.getShadowRay(l))){
						Vector3D lightVec = l->getWorldPosition() - is.mPosition;
						float d2 = lightVec.length2();
						lightVec.normalize();
						Color radiance = l->getRadiance();
						Color brdf = is.mMaterial->evalBRDF(is, lightVec);
						float angle = max(lightVec * is.mNormal, 0.0f);
						hp->directIllumination += radiance * brdf * angle / d2;
					}
				}
				vec.push_back(hp);
			}
		}
	}
}

void PhotonMapper::photonTracingPass(){
	for (int i = 0; i < mScene->getNumberOfLights(); ++i){
		PointLight* l = mScene->getLight(i);
		for (int i = 0; i < numberPhotons; ++i){
			
			Vector3D dir(uniform() * 2 - 1, uniform() * 2 - 1, uniform() * 2 - 1);
			while(dir.length2() > 1.0f){
				dir = Vector3D(uniform() * 2 - 1, uniform() * 2 - 1, uniform() * 2 - 1);
			}
			dir.normalize();

			Ray ray;
			ray.orig = l->getWorldPosition();
			ray.dir = dir;

			Color startFlux = l->getRadiance() * 4.0f * M_PI;

			trace(ray, 0, startFlux);
		}
	}
}


/**
 * Computes the radiance returned by tracing the ray r.
 */
void PhotonMapper::trace(const Ray& ray, int depth, const Color& flux)
{
	Intersection is;
	Color lIndirect = Color(0.0f, 0.0f, 0.0f);

	if (mScene->intersect(ray, is)){
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
			Color addFlux = flux;
			if (depth != 1){
				Hitpoint hp;
				addFlux = flux * is.mMaterial->evalBRDF(is, -dir);
				if (depth > maxDepth)
					addFlux *= abs_factor;
				hitpointBVH.intersect(is.mPosition, addFlux);
			}
			trace(ray2, depth + 1, addFlux);
		}
	}
}

void PhotonMapper::output(int i){
	for (Hitpoint* hp : hitpointBVH.objs){
		Color flux = hp->totalFlux;
		float rad = hp->radius;
		Color out = hp->directIllumination + hp->totalFlux / (numberPhotons * (i+1) * M_PI * hp->radius * hp->radius);
		mImage->setPixel(hp->pixelX, hp->pixelY, out);
	}

	stringstream ss;
	ss << "output_photon_" << i << ".png";
	mImage->save(ss.str());
}