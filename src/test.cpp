#include "defines.h"
#include "scene.h"
#include "camera.h"
#include "ray.h"
#include "intersection.h"
#include "material.h"
#include "pathtracer.h"
#include "timer.h"
#include "image.h"

using namespace std;

int main(){
	Vector3D v(0.5f, 0.5f, 0.5f);
	Vector3D n(0.f, 0.f, -1.f);
	v = v - 2 * (v * n) * n;
	cout << v.x << "," << v.y << "," << v.z << endl;
	cout << endl;
}