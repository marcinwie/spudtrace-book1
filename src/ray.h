#pragma once

#include "vector3.h"

class ray
{
public: 
	ray() = default; 
	ray(const vector3& a, const vector3& b) : _a(a), _b(b) {};

	const vector3& origin() const { return _a; }
	const vector3& direction() const { return _b; }
	vector3 point_at_parameter(float t) const {
		return _a + t * _b;
	}

private: 
	vector3 _a; 
	vector3 _b; 
};
