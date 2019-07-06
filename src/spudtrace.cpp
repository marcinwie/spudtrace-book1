// spudray.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "math.h"
#include "objects.h"
#include "materials.h"
#include "camera.h"

vector3 color(const ray& r, Object* obj, int depth) {
	HitRecord rec;
	if (obj->hit(r, 0.001f, FLT_MAX, rec)) {
		ray scattered;
		vector3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, obj, depth + 1);
		}
		else {
			return vector3::ZERO;
		}
	}
	else
	{
		vector3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vector3::ONE + t * vector3(0.5f, 0.7f, 1.0f);
	}
}

void sample_scene(World& world)
{
	world._objects.push_back(std::make_unique<Sphere>(vector3(0.0f, 0.0f, -1.0f), 0.5f,
		new Lambertian(vector3(0.1f, 0.2f, 0.5f))));
	world._objects.push_back(std::make_unique<Sphere>(vector3(0.0f, -100.5f, -1.0f), 100.f,
		new Lambertian(vector3(.8f, 0.8f, 0.0f))));
	world._objects.push_back(std::make_unique<Sphere>(vector3(1.0f, 0.0f, -1.0f), 0.5f,
		new Metal(vector3(.8f, 0.6f, 0.2f), 0.2f)));
	world._objects.push_back(std::make_unique<Sphere>(vector3(-1.0f, 0.0f, -1.0f), 0.5f,
		new Dielectric(1.5f)));
	world._objects.push_back(std::make_unique<Sphere>(vector3(-1.0f, 0.0f, -1.0f), -0.45f,
		new Dielectric(1.5f)));

}

void book_cover_scene(World& world) {
	
	int32_t x_max = 22; 
	int32_t y_max = 22;
	int32_t x_half = 11; 
	int32_t y_half = 11;

	world._objects.reserve(world._objects.size() + (x_max * y_max) + 4);

	world._objects.push_back(std::make_unique<Sphere>(vector3(0.f, -1000.f, 0.f), 1000.f,
		new Lambertian(vector3(0.5f, 0.5f, 0.5f))));

	for (int32_t x = 0; x < x_max; x++) {
		for (int32_t y = 0; y < y_max; y++) {
			int32_t a = x - x_half;
			int32_t b = y - y_half;

			float choose_mat = drand48();
			vector3 center(a + 0.9f * drand48(), 0.2f, b + 0.9f * drand48());
			if ((center - vector3(4.f, 0.2f, 0.f)).length() > 0.9f) {
				if (choose_mat < 0.8f) {  // diffuse
					world._objects.push_back(std::make_unique<Sphere>(center, 0.2f,
						new Lambertian(vector3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()))));
				}
				else if (choose_mat < 0.95f) { // metal
					world._objects.push_back(std::make_unique<Sphere>(center, 0.2f,
						new Metal(vector3(0.5f * (1 + drand48()), 0.5f * (1.f + drand48()), 0.5f * (1.f + drand48())), 0.5f * drand48())));
				}
				else {  // glass
					world._objects.push_back(std::make_unique<Sphere>(center, 0.2f, new Dielectric(1.5f)));
				}
			}
		}
	}

	world._objects.push_back(std::make_unique<Sphere>(vector3(0.f, 1.f, 0.f), 1.f, new Dielectric(1.5f)));
	world._objects.push_back(std::make_unique<Sphere>(vector3(-4.f, 1.f, 0.f), 1.f, new Lambertian(vector3(0.4f, 0.2f, 0.1f))));
	world._objects.push_back(std::make_unique<Sphere>(vector3(4.f, 1.f, 0.f), 1.f, new Metal(vector3(0.7f, 0.6f, 0.5f), 0.0f)));
}

int main()
{
	int32_t width = 1920;
	int32_t height = 1080;
	// keep 2 to 1 aspect ratio to keep the rest of the math match the article
	float nx = width * 1.0f;
	float ny = height * 1.0f;
	float ns = 100.f;

	//float nx = 320.f;
	//float ny = 240.f;
	//float ns = 40.f;

	World w; 

	//float R = cos(M_PI / 4);
	//w._objects.push_back(std::make_unique<Sphere>(vector3(R, 0.0f, -1.0f), R,
	//	new Lambertian(vector3(0.0f, 0.0f, 1.0f))));
	//w._objects.push_back(std::make_unique<Sphere>(vector3(-R, 0.0f, -1.0f), R,
	//	new Lambertian(vector3(1.0f, 0.0f, 0.0f))));

	sample_scene(w);
	book_cover_scene(w);

	vector3 lookFrom = vector3(13.0f, 2.0f, 3.0f);
	vector3 lookAt = vector3(0.0f, 0.0f, 0.0f);
	//float dist_to_focus = (lookFrom - lookAt).length();
	float dist_to_focus = 10.f;
	float aperture = 0.1f;
	float vfov = 20.0f;
	Camera cam(lookFrom, lookAt, vector3::UP, vfov, nx/ny, aperture, dist_to_focus );


	std::vector<vector3> frame_buffer; 
	frame_buffer.reserve(width* height);
	for (int32_t j = height-1; j >=0; j--)
	{
		for (int32_t i = 0; i < width; i++)
		{
			vector3 col(0.f, 0.f, 0.f);
			for (int s = 0; s < (int)ns; s++)
			{
				float u = (i+ drand48()) / nx;
				float v = (j + drand48()) / ny;
				ray r = cam.getRay(u, v);
				vector3 p = r.point_at_parameter(2.0);
				col += color(r, &w, 0);
			}
			col /= ns; 
			col = vector3(sqrt(col.x()), sqrt(col.y()), sqrt(col.z()));
			frame_buffer.push_back(col);
		}
	}

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for (auto& col : frame_buffer)
	{
		int ir = (int)(255.99 * col.r());
		int ig = (int)(255.99 * col.g());
		int ib = (int)(255.99 * col.b());

		std::cout << ir << " " << ig << " " << ib << "\n";

	}
}