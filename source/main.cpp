#include "framework/engine.h"
#include "framework/utils.h"
#include "../catmull_rom.h"

using namespace std;
using namespace glm;

/*
* Coordinate system:
* x - right
* y - up
* z - backward
*/



// ============================================================

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

	// ========================================================

	vector<vec3> path_v;
	for (size_t i = 0; i < std::size(path); i += 3)
	{
		vec3 v(path[i], path[i + 1], path[i + 2]);
		path_v.emplace_back(v);
	}

	vector<vec3> spline_points = catmull_rom::calculate(path_v, 0.5f, 10);
	size_t spline_points_limit = spline_points.size();
	vector<Object*> points2;
	for (auto& spline_point : spline_points)
	{
		Object* sphere = engine->createObject(&sphere_mesh);
		sphere->setColor(0, 0, 1);
		sphere->setPosition(spline_point);
		sphere->setScale(0.1f);
		points2.push_back(sphere);
	}
	LineDrawer path_drawer2(spline_points, true);

	// test cube
	auto cube_mesh = createCube();
	Object* cube = engine->createObject(&cube_mesh);
	cube->setColor(0.5f, 0.2f, 0.2f);
	cube->setPosition(0, 0.5f, 0);
	cube->setRotation(-90.0f, 0.0f, 0.0f);
	cube->setScale(1.f);

	/// class points holder
	///	get point between 0 to 1
	
	/// class time map

	/// new cube rotation
	// get current position
	// get new position by deltatime
	// calc angle between positions
	// set new position
	// set new rotation

	auto path_t = 0.f;
	auto speed = 0.1f;

	// main loop
	while (!engine->isDone())
	{
		auto delta_time = engine->getDeltaTime();
		path_t += delta_time * speed;
		if (path_t > 1)
		{
			path_t -= 1;
		}
		auto index2 = clamp<size_t>(path_t * spline_points_limit, 0, spline_points_limit - 1);
		auto current_point = spline_points[index2];
		cube->setPosition(current_point);
		cout << path_t << " : " << index2 << " : " << current_point.x << " " << current_point.y << " " << current_point.z << endl;

		engine->update();
		engine->render();

		path_drawer.draw();
		path_drawer2.draw();
		
		engine->swap();
	}

	engine->shutdown();
	return 0;
}
