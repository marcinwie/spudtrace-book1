#pragma once

#include "math_utils.h"
#include "materials.h"

#include <vector>
#include <memory.h>

class Object {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};

class Sphere : public Object
{
public:
	Sphere() = default;
	Sphere(const vector3& c, float r, Material* m) : _center(c), _radius(r), _mat_ptr(m) {}
	virtual bool hit(const ray& r, float t_min, float t_max, HitRecord& rec) const;
private:
	vector3 _center;
	float _radius;
	Material* _mat_ptr;
};

bool Sphere::hit(const ray& r, float t_min, float t_max, HitRecord& rec) const
{
	vector3 oc = r.origin() - _center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - _radius * _radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0.0f) {
		float temp = (-b - sqrt(b * b - a * c)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - _center) / _radius;
			rec.mat_ptr = _mat_ptr;
			return true;
		}
		temp = (-b + sqrt(b * b - a * c)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - _center) / _radius;
			rec.mat_ptr = _mat_ptr;
			return true;
		}
	}
	return false;
}

class World : public Object
{
public:
	using object_ptr = std::unique_ptr<Object>;

	World() = default;

	virtual bool hit(const ray& r, float t_min, float t_max, HitRecord& rec) const;

	std::vector<object_ptr> _objects;
};


bool World::hit(const ray& r, float t_min, float t_max, HitRecord& rec) const
{
	HitRecord temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;
	for (auto& obj : _objects) {
		if (obj->hit(r, t_min, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}
