#include "catmull_rom.h"

vec3 catmull_rom::return_catmull_rom_vec3(const float dt, const float tension, const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3)
{
	const vec3 a = p1;
	const vec3 b = tension * (p2 - p0);
	const vec3 c = 3.f * (p2 - p1) - tension * (p3 - p1) - 2.f * tension * (p2 - p0);
	const vec3 d = -2.f * (p2 - p1) + tension * (p3 - p1) + tension * (p2 - p0);
	const vec3 result = a + (b * dt) + (c * dt * dt) + (d * dt * dt * dt);

	return result;
}

vector<vec3> catmull_rom::calculate_spline(const float tension, const int n, const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3)
{
	auto result = vector<vec3>(n + 1);
	auto index = 0;
	const float step = 1.f / n;
	for (auto t = 0; t < n; t++)
	{
		const auto vector = return_catmull_rom_vec3(t * step, tension, p0, p1, p2, p3);
		result[index++] = vector;
	}
	result[index] = p2;

	return result;
}

vector<vec3> catmull_rom::build_vectors(const vector<vec3>& points, const bool need_closed_curve)
{
	const auto n = points.size();
	vector<vec3> vectors;
	if (need_closed_curve)
	{
		vectors = vector<vec3>(n + 3);
		vectors[0] = points[n - 1];
		vectors[n + 1] = points[0];
		vectors[n + 2] = points[1];
	}
	else
	{
		vectors = vector<vec3>(n + 2);
		vectors[0] = points[0];
		vectors[n + 1] = points[n - 1];
	}
	for (auto i = 0; i < n; ++i)
	{
		vectors[i + 1] = points[i];
	}

	return vectors;
}

vector<vec3> catmull_rom::calculate(const vector<vec3>& points, const float tension, const int n, const bool need_closed_curve)
{
	if (points.size() <= 2) {
		throw std::invalid_argument("The nodes count must be > 2.");
	}

	if (n < 1) {
		throw std::invalid_argument("The subnodes count between the nodes of the spline must be >= 1.");
	}

	const auto vectors = build_vectors(points, need_closed_curve);
	vector<vec3> result;

	for (size_t i = 1; i < vectors.size() - 2; ++i)
	{
		auto spline = calculate_spline(tension, n, vectors[i - 1], vectors[i], vectors[i + 1], vectors[i + 2]);
		result.insert(result.end(), spline.begin(), spline.end());
	}

	return result;
}