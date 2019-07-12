// spudray.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "math.h"
#include "objects.h"
#include "materials.h"
#include "camera.h"

#include <iostream>
#include <fstream>
#include <thread>

#include "threadqueue.h"

vector3 color(const ray& r, const Object* obj, int depth) {
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

struct SceneInfo
{
	int32_t	_width; 
	int32_t	_height;
	int32_t	_samples; 
	const Camera*	_camera; 
	const World*	_world;
};


struct JobInfo
{
	float _nx; 
	float _ny;
	float _ns;
	int32_t _j; 
	int32_t _i;
	vector3* _output; 
};

void process_ray(const SceneInfo& scene, float nx, float ny, float ns, int32_t j, int32_t i, vector3* output)
{
	vector3 col(0.f, 0.f, 0.f);
	for (int s = 0; s < (int)scene._samples; s++)
	{
		float u = (i + drand48()) / nx;
		float v = (j + drand48()) / ny;
		ray r = scene._camera->getRay(u, v);
		vector3 p = r.point_at_parameter(2.0);
		col += color(r, scene._world, 0);
	}
	col /= ns;
	col = vector3(sqrt(col.x()), sqrt(col.y()), sqrt(col.z()));

	// store the result in our output buffer
	*output = col;
}

void thread_process(const SceneInfo& scene, vector3* output)
{
	ThreadPool pool(8);
	pool.init();

	std::vector<std::future<void>> results; 
	results.reserve(scene._height * scene._width); 

	float nx = scene._width * 1.0f;
	float ny = scene._height * 1.0f;
	float ns = scene._samples * 1.0f;

	for (int32_t j = scene._height - 1; j >= 0; j--)
	{
		for (int32_t i = 0; i < scene._width; i++)
		{
			results.emplace_back(pool.submit(process_ray, std::ref(scene), nx, ny, ns, j, i, output));
			//process_ray(scene, job);
			output++;
		}
	}

	for (auto& result : results) {
		result.get();
	}

	pool.shutdown();

}

bool write_ppm(const char* filename, int32_t width, int32_t height, const std::vector<vector3>& data)
{
	std::ofstream stream;
	stream.open(filename);
	if (!stream.is_open())
		return false; 

	auto start = std::chrono::high_resolution_clock::now();

	stream << "P3\n" << width << " " << height << "\n255\n";
	for (auto itr = data.begin(); itr != data.end(); itr++)
	{
		auto& col = *itr;

		int ir = (int)(255.99 * col.r());
		int ig = (int)(255.99 * col.g());
		int ib = (int)(255.99 * col.b());

		stream << ir << " " << ig << " " << ib << "\n";
	}

	stream.close();

	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Finished writing file in  " << std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() << " second(s)\n";

	return true;
}

int main()
{
	const int32_t width = 1920;
	const int32_t height = 1080;
	const int32_t samples = 10;

	// keep 2 to 1 aspect ratio to keep the rest of the math match the article
	const float nx = width * 1.0f;
	const float ny = height * 1.0f;

	World world; 

	// load scene data to the world
	sample_scene(world);
	book_cover_scene(world);

	// setup camera
	vector3 lookFrom = vector3(13.0f, 2.0f, 3.0f);
	vector3 lookAt = vector3(0.0f, 0.0f, 0.0f);
	//float dist_to_focus = (lookFrom - lookAt).length();
	float dist_to_focus = 10.f;
	float aperture = 0.1f;
	float vfov = 20.0f;
	Camera cam(lookFrom, lookAt, vector3::UP, vfov, nx/ny, aperture, dist_to_focus );

	auto start = std::chrono::high_resolution_clock::now();

	// create output buffer and pre-allocate all memory so we are not doing bunch of allocations
	std::vector<vector3> frame_buffer;
	frame_buffer.resize(width * height);
	
	// process all ray-tracing and generate a color buffer
	SceneInfo scene = { width, height, samples, &cam, &world };
	thread_process(scene, frame_buffer.data());

	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Finished image processing in  " << std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() << " second(s)\n";

	// dump image data to ppm file
	write_ppm("output.ppm", width, height, frame_buffer); 

	//std::thread t(&thread_process, params);   // t starts running
	//t.join();

	
}