#pragma once

#include <vector>
#include "glm/vec3.hpp"

using namespace std;
using namespace glm;

class catmull_rom
{
	static vec3 return_catmull_rom_vec3(float dt, float tension, vec3 p0, vec3 p1, vec3 p2, vec3 p3);
	static vector<vec3> calculate_spline(float tension, int n, vec3 p0, vec3 p1, vec3 p2, vec3 p3);
	static vector<vec3> build_vectors(const vector<vec3>& points, bool need_closed_curve);

public:
	static vector<vec3> calculate(const vector<vec3>& points, float tension = 0.5f, int n = 5, bool need_closed_curve = true);
};

