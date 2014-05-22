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
#include "pathtracer.h"
#include "photonmapper.h"
#include "pointlight.h"
#include "lightprobe.h"
#include "listaccelerator.h"
#include "texture.h"
#include "phong.h"
#include "bvhaccelerator.h"
#include "cornellscene.h"
#include <omp.h>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;


static void buildDOFScene(Scene& scene){
	PointLight* light = new PointLight(Point3D(0.0f, 108.0f, 0.0f), Color(1.0f, 1.0f, 1.0f), 1.0f);
	scene.add(light);

	Diffuse* red;  
	Diffuse* green;
	Diffuse* blue;
	Diffuse* white;

	red = new Diffuse(Color(0.7f, 0.2f, 0.2f));
	green = new Diffuse(Color(0.2f,0.7f,0.2f));
	blue = new Diffuse(Color(0.2f, 0.2f, 0.7f));
	white = new Diffuse(Color(0.7f,0.7f, 0.7f));
	
	Sphere* ball1 = new Sphere(16.5f, red);
	ball1->setTranslation(Vector3D(22.0f, 16.5f, -200.0f));
	scene.add(ball1);

	Sphere* ball2 = new Sphere(16.5f, green);
	ball2->setTranslation(Vector3D(-10.0f, 16.5f, 0.0f));
	scene.add(ball2);

	Sphere* ball3 = new Sphere(16.5f, blue);
	ball3->setTranslation(Vector3D(-35.0f, 16.5f, 100.0f));
	scene.add(ball3);

	Mesh* ground = new Mesh("data/plane.obj", white);
	ground->setScale(150.0f);
	scene.add(ground);

	/*
	Mesh* side1 = new Mesh("data/plane.obj", red);
	side1->setScale(150.0f);
	side1->setRotation(180.0f, 0.0f, 90.0f);
	side1->setTranslation(-60, 60, 0.0f);
	scene.add(side1);

	Mesh* side2 = new Mesh("data/plane.obj", blue);
	side2->setScale(150.0f);
	side2->setRotation(0.0f, 0.0f, 90.0f);
	side2->setTranslation(60, 60, 0.0f);
	scene.add(side2);

	Mesh* side3 = new Mesh("data/plane.obj", white);
	side3->setScale(150.0f);
	side3->setRotation(90.0f, 0.0f, 0.0f);
	side3->setTranslation(0.0f, 60, -60);
	scene.add(side3);

	Mesh* roof = new Mesh("data/plane.obj", white);
	roof->setScale(150.0f);
	roof->setRotation(180.0f, 0.0f, 0.0f);
	roof->setTranslation(0.0f, 120, 0.0f);
	scene.add(roof);
	*/
}
/**
 * Builds the "Spheres" scene.
 */
static void buildSpheres(Scene& scene)
{
	// Add lights
	PointLight *pointLight1 = new PointLight(Point3D(20.0f, 240.0f, -7.0f), Color(1.5f, 1.5f, 1.5f));
	scene.add(pointLight1);

	// Setup materials
	Phong *material[4];

	material[0] = new Phong(Color(0.0f, 0.2f, 0.9f), 10);
	material[1] = new Phong(Color(1.0f, 0.3f, 0.2f), 100);
	material[2] = new Phong(Color(0.0f, 0.7f, 0.1f), 10);
	material[3] = new Phong(Color(0.6f, 0.6f, 0.6f), 35);

	material[0]->setReflectivity(0.0f);
	material[1]->setReflectivity(0.7f);
	material[2]->setReflectivity(0.0f);
	material[3]->setReflectivity(0.5f);

	material[0]->setTransparency(0.7f);
	material[1]->setTransparency(0.0f);
	material[2]->setTransparency(0.3f);
	material[3]->setTransparency(0.0f);

	material[0]->setIndexOfRefraction(1.01f);
	material[1]->setIndexOfRefraction(1.00f);
	material[2]->setIndexOfRefraction(1.20f);
	material[3]->setIndexOfRefraction(1.00f);

	// Add spheres
	for (int i = 0; i < 2 * 2 * 2; i++) {
		float x = (float)(-6 + 12 * (i & 1));
		float y = (float)(-6 + 12 * ((i >> 1) & 1));
		float z = (float)(-6 + 12 * ((i >> 2) & 1));
		Sphere* sphere = new Sphere(3.5, material[i % 3]);
		sphere->setTranslation(Vector3D(x, y, z));
		scene.add(sphere);
	}

	// NOTE: Remove this sphere from the scene when it's time to draw the plane!
	/*Sphere* sphere = new Sphere(200, material[3]);
	sphere->setTranslation(Vector3D(0, -210, 0));
	scene.add(sphere); // To be replaced by plane!
	*/
	// Add plane
	Phong *planeMaterial = new Phong(Color(1.0f, 1.0f, 1.0f), 10);
	planeMaterial->setReflectivity(0.75f);
	Mesh* plane = new Mesh("data/plane.obj", planeMaterial);

	plane->setScale(20.0f);
	plane->setTranslation(Vector3D(0, -10, 0));
	scene.add(plane);

	// TODO: Uncomment the following lines for the cliff-hanger assignment

	/*
	Phong *triSphereMaterial = new Phong(Color(1.0f,0.0f,1.0f));
	Mesh* triSphere = new Mesh("data/sphere.obj", triSphereMaterial);
	triSphere->setScale(5.0f);
	triSphere->setTranslation(Vector3D(0, 0, 0));
	scene.add(triSphere);
	*/


}

static void buildSimple(Scene& scene, bool scrambled)
{
	PointLight *pointLight0 = new PointLight(Point3D(-20.0f, 20.0f, 60.0f), Color(1.5f, 1.5f, 1.5f));
	scene.add(pointLight0);
	// Setup material
	Diffuse *material = new Diffuse(Color(0.0f, 0.2f, 1.5f));
	material->setReflectivity(0.4f);
	// Add spheres
	for (int i = 0; i < 80; i++) {
		float x, y, z;
		if (scrambled) {
			x = -39.5f + (float)((i * 13) % 80);
			y = -39.5f + (float)((i * 7) % 80);
			z = -39.5f + (float)((i * 29) % 80);
		}
		else {
			x = -39.5f + (float)(i % 80);
			y = -39.5f + (float)(i % 80);
			z = -39.5f + (float)(i % 80);
		}
		Sphere* sphere = new Sphere(1.0, material);
		sphere->setTranslation(Vector3D(x, y, z));
		scene.add(sphere);
	}
}

static void buildElephant(Scene& scene)
{
	PointLight *pointLight0 = new PointLight(Point3D(-50.0f, 60.0f, 20.0f), Color(0.4f, 0.7f, 0.7f));
	scene.add(pointLight0);
	PointLight *pointLight1 = new PointLight(Point3D(70.0f, 140.0f, -7.0f), Color(2.5f, 2.5f, 2.5f));
	scene.add(pointLight1);
	Phong *planeMaterial = new Phong(Color(1.0f, 1.0f, 1.0f), 15);
	planeMaterial->setReflectivity(0.75f);
	Mesh* plane = new Mesh("data/plane.obj", planeMaterial);
	plane->setScale(20.0f);
	plane->setTranslation(Vector3D(0, -10, 0));
	scene.add(plane);
	Phong *elephantMaterial0 = new Phong(Color(0.4f, 0.7f, 1.0f), 25);
	elephantMaterial0->setReflectivity(0.55f);
	Mesh* elephant0 = new Mesh("data/elephant.obj", elephantMaterial0);
	elephant0->setScale(1.1f);
	elephant0->setRotation(0.0f, 220.0f, 0.0f);
	elephant0->setTranslation(Vector3D(-12, -10, 1));
	scene.add(elephant0);
	Phong *elephantMaterial1 = new Phong(Color(0.4f, 1.0f, 0.7f), 25);
	elephantMaterial1->setReflectivity(0.20f);
	elephantMaterial1->setTransparency(0.50f);
	elephantMaterial1->setIndexOfRefraction(1.1f);
	Mesh* elephant1 = new Mesh("data/elephant.obj", elephantMaterial1);
	elephant1->setScale(1.35f);
	elephant1->setRotation(0.0f, 190.0f, 0.0f);
	elephant1->setTranslation(Vector3D(8.0f, -10, -5));
	scene.add(elephant1);
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
		BVHAccelerator accelerator;
		Scene scene(&accelerator);
		Image output(512, 512);
		Camera* camera = new Camera(&output);

		
		//buildCornellScene(&scene);
		
		setupCornellCamera(camera);
		

		/*
		buildSimple(scene, false);
		Point3D pos(0.0f, 0.0f, 116.0f);
		Point3D target(0.0f, 0.0f, 0.0f);
		Vector3D up(0.0f, 1.0f, 0.0f);
		camera->setLookAt(pos, target, up, 58.0f);
		*/
		
		/*
		buildElephant(scene);
		Point3D pos(27.0f, 13.0f, 21.0f);
		Point3D target(0.0f, -4.0f, 0.0f);
		Vector3D up(0.0f, 1.0f, 0.0f);
		camera->setLookAt(pos, target, up, 52.0f);
		*/
		
		
		//buildSpheres(scene);
		/*
		Point3D pos(27.0f, 17.0f, 21.0f);
		Point3D target(-1.0f, -3.0f, 0.0f);
		Vector3D up(0.0f, 1.0f, 0.0f);
		camera->setLookAt(pos, target, up, 52.0f);
		*/

		buildDOFScene(scene);
		/*
		Point3D pos(27.0f, 17.0f, 21.0f);
		Point3D target(-1.0f, -3.0f, 0.0f);
		Vector3D up(0.0f, 1.0f, 0.0f);
		camera->setLookAt(pos, target, up, 52.0f);
		*/
		scene.add(camera);

		// Prepare the scene for rendering.
		scene.prepare();
		
		// Create a raytracer and start raytracing.
		std::cout << "creating raytracer" << std::endl;

		//PathTracer rt(&scene, &output);
		//PhotonMapper rt(&scene, &output);
		WhittedTracer rt(&scene, &output);
		rt.computeImage();

		// Save image.
		output.save("output.png");//comment this line when running photonmapper
	}
	catch (const std::exception& e) {
		// Print the error and exit.
		std::cerr << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
}

