#include "framework/engine.h"
#include "framework/utils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/spline.hpp"

using namespace std;
using namespace glm;

/*
* Coordinate system:
* x - right
* y - up
* z - backward
*/

vec3 get_spline_point(const std::vector<vec3>& points, float t)
{
	// indices of the relevant control points
	int i0 = clamp<int>(t - 1, 0, points.size() - 1);
	int i1 = clamp<int>(t, 0, points.size() - 1);
	int i2 = clamp<int>(t + 1, 0, points.size() - 1);
	int i3 = clamp<int>(t + 2, 0, points.size() - 1);

	// parameter on the local curve interval
	float local_t = fract(t);

	return catmullRom(points[i0], points[i1], points[i2], points[i3], local_t);
}

int main()
{
	// initialization
	Engine *engine = Engine::get();
	engine->init(1600, 900, "UNIGINE Test Task");

	// set up camera
	Camera &cam = engine->getCamera();
	cam.Position = vec3(0.0f, 12.0f, 17.0f);
	cam.Yaw = -90.0f;
	cam.Pitch = -45.0f;
	cam.UpdateCameraVectors();

	// create shared meshes
	Mesh plane_mesh = createPlane();
	Mesh sphere_mesh = createSphere();

	// create background objects
	Object *plane = engine->createObject(&plane_mesh);
	plane->setColor(0.2f, 0.37f, 0.2f); // green
	plane->setPosition(0, -0.5f, 0);
	plane->setRotation(-90.0f, 0.0f, 0.0f);
	plane->setScale(20.0f);

	// path
	const float path[] = {
		 0.0f, -0.375f,  7.0f, // 1
		-6.0f, -0.375f,  5.0f, // 2
		-8.0f, -0.375f,  1.0f, // 3
		-4.0f, -0.375f, -6.0f, // 4
		 0.0f, -0.375f, -7.0f, // 5
		 1.0f, -0.375f, -4.0f, // 6
		 4.0f, -0.375f, -3.0f, // 7
		 8.0f, -0.375f,  7.0f  // 8
	};
	vector<Object *> points;
	for (int i = 0; i < 8; i++)
	{
		Object *sphere = engine->createObject(&sphere_mesh);
		sphere->setColor(1, 0, 0);
		sphere->setPosition(path[i*3], path[i*3+1], path[i*3+2]);
		sphere->setScale(0.25f);
		points.push_back(sphere);
	}
	LineDrawer path_drawer(path, points.size(), true);

	vector<vec3> path_v(path, path + std::size(path));
	vector<vec3> spline_points;
	for (auto i = 0.f; i < 100; i++)
	{
		spline_points.emplace_back(get_spline_point(path_v, i));
	}

	// test cube
	auto cube_mesh = createCube();
	Object* cube = engine->createObject(&cube_mesh);
	cube->setColor(0.5f, 0.2f, 0.2f);
	cube->setPosition(0, 0.5f, 0);
	cube->setRotation(-90.0f, 0.0f, 0.0f);
	cube->setScale(1.f);

	// main loop
	while (!engine->isDone())
	{
		engine->update();
		engine->render();

		path_drawer.draw();
		
		engine->swap();
	}

	engine->shutdown();
	return 0;
}
