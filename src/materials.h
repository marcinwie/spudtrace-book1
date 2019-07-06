#pragma once

#include <algorithm>
#include "math_utils.h"

class Material
{
public:
	virtual bool scatter(const ray& in, const HitRecord& rec, vector3& attenuation, ray& scattered) const = 0;
};


class Lambertian : public Material
{
public:
	Lambertian(const vector3& a) : _albedo(a) {}
	virtual bool scatter(const ray& in, const HitRecord& rec, vector3& attenuation, ray& scattered) const {
		vector3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = _albedo;
		return true;
	}
private:
	vector3 _albedo;
};

class Metal : public Material
{
public:
	Metal(const vector3& a, float f) : _albedo(a), _fuzz(std::min(f, 1.0f)) {
	}

	virtual bool scatter(const ray& in, const HitRecord& rec, vector3& attenuation, ray& scattered) const {
		vector3 reflected = reflect(unit_vector(in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + _fuzz * random_in_unit_sphere());
		attenuation = _albedo;
		return dot(scattered.direction(), rec.normal) > 0.0f;
	}
private:
	vector3 _albedo;
	float _fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(float ri) : _ref_idx(ri) {}
	virtual bool scatter(const ray& in, const HitRecord& rec, vector3& attenuation, ray& scattered) const
	{
		vector3 outward_normal;
		vector3 reflected = reflect(in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vector3::ONE;
		vector3 refracted;
		float reflect_prob;
		float cosine;

		if (dot(in.direction(), rec.normal) > 0.0f) {
			outward_normal = -rec.normal;
			ni_over_nt = _ref_idx;
			cosine = _ref_idx * dot(in.direction(), rec.normal) / in.direction().length();
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0f / _ref_idx;
			cosine = -dot(in.direction(), rec.normal) / in.direction().length();
		}

		if (refract(in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, _ref_idx);
		}
		else {
			scattered = ray(rec.p, reflected);
			reflect_prob = 1.0f;
		}

		if (drand48() < reflect_prob) {
			scattered = ray(rec.p, reflected);
		}
		else {
			scattered = ray(rec.p, refracted);
		}

		return true;
	}
private:
	float _ref_idx;
};