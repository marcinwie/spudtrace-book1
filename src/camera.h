#pragma once

#include "math_utils.h"

class Camera {
public:
	Camera(const vector3& lookfrom, const vector3& lookat, const vector3& vup, float vfov, float aspect)
	{
		float theta = vfov * static_cast<float>(M_PI) / 180.0f; 
		float half_height = tan(theta / 2.0f); 
		float half_width = aspect * half_height;
		_w = unit_vector(lookfrom-lookat);
		_u = unit_vector(cross(vup, _w));
		_v = cross(_w, _u);

		
		//_lower_left_corner = vector3(-half_width, -half_height, -1.0f);
		_origin = lookfrom;
		_lower_left_corner = _origin - half_width * _u - half_height * _v - _w;
		_horizontal = 2.0f * half_width * _u;
		_vertical = 2.0f * half_height * _v;
	}

	Camera(const vector3& lookfrom, const vector3& lookat, const vector3& vup, float vfov, float aspect, float aperture, float focus_dist)
	{
		_lens_radius = aperture / 2.f;
		float theta = vfov * static_cast<float>(M_PI) / 180.0f;
		float half_height = tan(theta / 2.0f);
		float half_width = aspect * half_height;
		_origin = lookfrom;
		_w = unit_vector(lookfrom - lookat);
		_u = unit_vector(cross(vup, _w));
		_v = cross(_w, _u);


		//_lower_left_corner = vector3(-half_width, -half_height, -1.0f);
		_lower_left_corner = _origin - half_width * focus_dist * _u - half_height * focus_dist * _v - focus_dist * _w;
		_horizontal = 2.0f * half_width * focus_dist * _u;
		_vertical = 2.0f * half_height * focus_dist  * _v;
	}

	ray getRay(float s, float t) const {
		vector3 rd = _lens_radius * random_in_unit_disk();
		vector3 offset = _u * rd.x() + _v * rd.y();
		return ray(_origin + offset, _lower_left_corner + s * _horizontal + t * _vertical - _origin - offset);
	}

private:

	vector3 _lower_left_corner;
	vector3 _horizontal;
	vector3 _vertical;
	vector3 _origin;
	vector3 _w, _u, _v;
	float _lens_radius;
};
