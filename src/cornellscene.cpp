/*
 *  cornellscene.cpp
 *  prTracer
 *
 *  Created by Rasmus Barringer on 2011-04-01.
 *  Copyright 2011 Lund University. All rights reserved.
 *
 */

#include "cornellscene.h"
#include "diffuse.h"
#include "sphere.h"
#include "mesh.h"

void buildCornellScene(Scene* scene)
{
	Diffuse* red = new Diffuse(Color(0.7f, 0.1f, 0.1f));
	Diffuse* blue = new Diffuse(Color(0.1f, 0.1f, 0.7f));
	Diffuse* white = new Diffuse(Color(0.7f, 0.7f, 0.7f));

	Mesh* ground = new Mesh("data/plane.obj", white);
	ground->setScale(150.0f);
	
	Mesh* side1 = new Mesh("data/plane.obj", red);
	side1->setScale(150.0f);
	side1->setRotation(180.0f, 0.0f, 90.0f);
	side1->setTranslation(-60, 60, 0.0f);
	
	Mesh* side2 = new Mesh("data/plane.obj", blue);
	side2->setScale(150.0f);
	side2->setRotation(0.0f, 0.0f, 90.0f);
	side2->setTranslation(60, 60, 0.0f);
	
	Mesh* side3 = new Mesh("data/plane.obj", white);
	side3->setScale(150.0f);
	side3->setRotation(90.0f, 0.0f, 0.0f);
	side3->setTranslation(0.0f, 60, -60);
	
	Mesh* roof = new Mesh("data/plane.obj", white);
	roof->setScale(150.0f);
	roof->setRotation(180.0f, 0.0f, 0.0f);
	roof->setTranslation(0.0f, 120, 0.0f);

	Sphere* ball1 = new Sphere(16.5f, white);
	ball1->setTranslation(Vector3D(23.5f, 16.5f, 31.0f));
	
	Sphere* ball2 = new Sphere(16.5f, white);
	ball2->setTranslation(Vector3D(-22.5f, 16.5f, 0.0f));
	
	PointLight* light = new PointLight(Point3D(0.0f, 108.0f, 0.0f), Color(1.0f, 1.0f, 1.0f), 6000.0f);
	
	scene->add(light);
	scene->add(ground);
	scene->add(side1);
	scene->add(side2);
	scene->add(side3);
	scene->add(roof);
	scene->add(ball1);
	scene->add(ball2);
}

void setupCornellCamera(Camera* camera)
{
	Vector3D up(0.0f, 1.0f, 0.0f);
	Point3D pos(0.0f, 60.0f, 180.0f);
	Point3D target(0.0f, 60.0f, 0.0f);
	camera->setLookAt(pos, target, up, 52.0f);
}