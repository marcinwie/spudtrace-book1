#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "vector3.h"
#include "ray.h"

class Material;
struct HitRecord
{
	float t;
	vector3 p;
	vector3 normal;
	Material* mat_ptr;
};

float drand48()
{
	return (float)rand() / (RAND_MAX + 1.0f);
}

vector3 random_in_unit_sphere() {
	vector3 p;
	do {
		p = 2.0f * vector3(drand48(), drand48(), drand48()) - vector3::ONE;
	} while (p.squared_length() >= 1.0f);
	return p;
}

vector3 random_in_unit_disk() {
	vector3 p;
	static const vector3 c = vector3(1.0f, 1.0f, 0.0f);
	do {
		p = 2.0f * vector3(drand48(), drand48(), 0.0f) - c;
	} while (dot(p,p) >= 1.0f);
	return p;
}

vector3 reflect(const vector3& v, const vector3& n) {
	return v - 2 * dot(v, n) * n;
}

bool refract(const vector3& v, const vector3& n, float ni_over_nt, vector3& refracted) {
	vector3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	return false;
}

float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}
