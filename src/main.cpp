/*
 *  main.cpp
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-03-20.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#include <iostream>
#include "defines.h"
#include "scene.h"
#include "image.h"
#include "camera.h"
#include "mesh.h"
#include "sphere.h"
#include "diffuse.h"
#include "whittedtracer.h"
#include "pointlight.h"
#include "lightprobe.h"
#include "listaccelerator.h"
#include "texture.h"
#include "phong.h"

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

/**
 * Builds the "Spheres" scene.
 */
static void buildSpheres(Scene& scene)
{
	// Add lights
	PointLight *pointLight1 = new PointLight(Point3D(20.0f, 240.0f, -7.0f), Color(1.5f, 1.5f, 1.5f), 10.0f);
	scene.add(pointLight1);

	// Setup materials
	Phong *material[4];

	material[0] = new Phong(Color(0.0f,0.2f,0.9f), 5);
	material[1] = new Phong(Color(0.2f, 0.2f, 0.6f), 100);
	material[2] = new Phong(Color(0.0f, 0.7f, 0.1f), 25);
	material[3] = new Phong(Color(0.6f, 0.6f, 0.6f), 25);

	material[0]->setReflectivity(0.1f);
	material[1]->setReflectivity(0.9f);
	material[2]->setReflectivity(0.3f);
	material[3]->setReflectivity(0.5f);

	material[0]->setTransparency(0.6f);
	material[1]->setTransparency(0.0f);
	material[2]->setTransparency(0.3f);
	material[3]->setTransparency(0.0f);

	material[0]->setIndexOfRefraction(1.01f);
	material[1]->setIndexOfRefraction(1.00f);
	material[2]->setIndexOfRefraction(1.20f);
	material[3]->setIndexOfRefraction(1.00f);

	// Add spheres
	for (int i = 0; i < 2 * 2 * 2; i++) {
		float x = (float) (-6 + 12 * (i & 1));
		float y = (float) (-6 + 12 * ((i >> 1) & 1));
		float z = (float) (-6 + 12 * ((i >> 2) & 1));
		Sphere* sphere = new Sphere(3.5, material[i % 3]);
		sphere->setTranslation(Vector3D(x, y, z));
		scene.add(sphere);
	}

	// NOTE: Remove this sphere from the scene when it's time to draw the plane!
	//Sphere* sphere = new Sphere(200, material[3]);
	//sphere->setTranslation(Vector3D(0, -210, 0));
	//scene.add(sphere); // To be replaced by plane!

	// Add plane
	Phong *planeMaterial = new Phong(Color(1.0f,1.0f,1.0f), 1000);
	planeMaterial->setReflectivity(0.75f);
	Mesh* plane = new Mesh("data/plane.obj", planeMaterial);

	plane->setScale(20.0f);
	plane->setTranslation(Vector3D(0, -10, 0));
	scene.add(plane);

	// TODO: Uncomment the following lines for the cliff-hanger assignment

	/*
	Diffuse *triSphereMaterial = new Diffuse(Color(1.0f,0.0f,1.0f));
	Mesh* triSphere = new Mesh("data/sphere.obj", triSphereMaterial);
	triSphere->setScale(5.0f);
	triSphere->setTranslation(Vector3D(0, 0, 0));
	scene.add(triSphere);
	*/

}


/**
 * Changes the current working directory.
 */
static bool changeDirectory(const char* directory)
{
#ifdef WIN32
	return SetCurrentDirectory(directory) == TRUE;
#else
	return chdir(directory) == 0;
#endif
}

/**
 * Goes up the directory hierarchy until a folder named "data" is found.
 */
static void changeToRootDirectory()
{
	for (int i = 0; i < 64; i++) {
		bool found = changeDirectory("data");
		
		if (!changeDirectory(".."))
			break;
		
		if (found)
			return;
	}
	
	throw runtime_error("unable to find data directory");
}

/**
 * The entry point of the program.
 */
int main(int argc, char* const argv[])
{
	try {
		// Set working directory.
		changeToRootDirectory();

		// Build scene.
		ListAccelerator accelerator;

		Scene scene(&accelerator);
		buildSpheres(scene);
		
		// Create output image.
		Image output(512, 512);

		// Create camera.
		Camera* camera = new Camera(&output);

		Point3D pos(27.0f, 17.0f, 21.0f);
		Point3D target(-1.0f, -3.0f, 0.0f);
		Vector3D up(0.0f, 1.0f, 0.0f);
		
		camera->setLookAt(pos, target, up, 52.0f);
		scene.add(camera);

		// Prepare the scene for rendering.
		scene.prepare();
		
		// Create a raytracer and start raytracing.
		std::cout << "creating raytracer" << std::endl;

		WhittedTracer rt(&scene, &output);
		rt.computeImage();

		// Save image.
		output.save("output.png");
	}
	catch (const std::exception& e) {
		// Print the error and exit.
		std::cerr << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
}
