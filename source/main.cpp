#include "framework/engine.h"
#include "framework/utils.h"
#include "catmull_rom.h"

using namespace std;
using namespace glm;

/*
* Coordinate system:
* x - right
* y - up
* z - backward
*/

// ============================================================

class points_holder
{
	vector<vec3> spline_points;

	static float get_distance(const vec3& v1, const vec3& v2) {
		const auto dx = v1.x - v2.x;
		const auto dy = v1.y - v2.y;
		const auto dz = v1.z - v2.z;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}

public:
	vector<vec3> create_spline_points(const float path[], const size_t path_count, float min_distance = 0.1f)
	{
		vector<vec3> path_v;
		for (size_t i = 0; i < path_count; i += 3)
		{
			path_v.emplace_back(vec3 (path[i], path[i + 1], path[i + 2]));
		}

		const vector<vec3> all_spline_points = catmull_rom::calculate(path_v, 0.5f, 500);

		spline_points.clear();
		for(auto point : all_spline_points)
		{
			if (!spline_points.empty() && get_distance(point, spline_points[spline_points.size() - 1]) < min_distance)
			{
				continue;
			}
			spline_points.emplace_back(point);
		}

		return spline_points;
	}

	///	Get a path point in the range from 0 to 1
	vec3 get_point_at(const float alpha) const
	{
		const auto index = clamp<size_t>(alpha * spline_points.size(), 0, spline_points.size() - 1);
		return spline_points[index];
	}
};

inline float get_angle(const vec3 v1, const vec3 v2)
{
	return atan2(v2.z - v1.z, v2.x - v1.x) * 180.f / pi<float>();
}

void update_cube(Object* cube, const vec3 new_point)
{
	const auto old_point = cube->getPosition();
	const auto old_angle = cube->getRotation();
	const auto angle = get_angle(old_point, new_point);
	cube->setPosition(new_point);
	cube->setRotation(old_angle.x, 180 - angle, old_angle.z);
}

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

	auto holder = points_holder();
	auto spline_points = holder.create_spline_points(path, std::size(path), 0.01f);
	// init

	// test cube
	auto cube_mesh = createCube();
	Object* cube = engine->createObject(&cube_mesh);
	cube->setColor(0.5f, 0.2f, 0.2f);
	cube->setPosition(0, 0.5f, 0);
	cube->setRotation(-90.0f, 0.0f, 0.0f);
	cube->setScale(1.f);

	// path drawing
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

	auto path_t = 0.f;
	const auto cube_speed = 0.05f;

	// main loop
	while (!engine->isDone())
	{
		auto delta_time = engine->getDeltaTime();
		path_t += delta_time * cube_speed;
		if (path_t > 1)
		{
			path_t -= 1;
		}

		auto new_point = holder.get_point_at(path_t);
		update_cube(cube, new_point);

		engine->update();
		engine->render();

		path_drawer.draw();
		path_drawer2.draw();
		
		engine->swap();
	}

	engine->shutdown();
	return 0;
}
