#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include "vec3.h"
#include "light.h"
#include "sphere.h"
#include <vector>
#include "ray.h"
#include "camera.h"
#include <fstream>
#include "Plane.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#define PI 3.14159265358979323846
using namespace std;

struct RGBType {
	double r;
	double g;
	double b;
};

void savebmp(const char* filename, int w, int h, int dpi, RGBType* data) {
	FILE* f;
	int k = w * h;
	int s = 4 * k;
	int filesize = 54 + s;

	double factor = 39.375;
	int m = static_cast<int>(factor);

	int ppm = dpi * m;

	unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w >> 8);
	bmpinfoheader[6] = (unsigned char)(w >> 16);
	bmpinfoheader[7] = (unsigned char)(w >> 24);

	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s >> 8);
	bmpinfoheader[23] = (unsigned char)(s >> 16);
	bmpinfoheader[24] = (unsigned char)(s >> 24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm >> 8);
	bmpinfoheader[27] = (unsigned char)(ppm >> 16);
	bmpinfoheader[28] = (unsigned char)(ppm >> 24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm >> 8);
	bmpinfoheader[31] = (unsigned char)(ppm >> 16);
	bmpinfoheader[32] = (unsigned char)(ppm >> 24);

	f = fopen(filename, "wb");

	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);

	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];

		double red = (data[i].r) * 255;
		double green = (data[i].g) * 255;
		double blue = (data[i].b) * 255;

		unsigned char color[3] = { (int)floor(blue),(int)floor(green),(int)floor(red) };

		fwrite(color, 1, 3, f);
	}

	fclose(f);
}

int main() {
	int width = 640, height = 480;
	vec3* image = new vec3[width * height];
	camera camera(vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(), (10 * PI / 180), (float)width/(float)height);
	Plane plane;


	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			double new_x = (2.0 * i) / width - 1.0;
			double new_y = (2.0 * j) / height - 1.0;
			ray *ray = camera.create_camera_ray(new_x, new_y);
			if (plane.is_intersected(*ray)) {
				image[i * j] = vec3(0.0,255.0,0.0);
			}
			else {
				image[i * j] = vec3();
			}

		}

	
	}   
	
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(double(1), image[i].get_x()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_y()) * 255) <<
			(unsigned char)(std::min(double(1), image[i].get_z()) * 255);
	}
	ofs.close();

	return 0;
}