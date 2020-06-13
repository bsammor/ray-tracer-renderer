#include <color.h>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <texture.h>
#include <experimental/filesystem>
#include <sys/time.h>
#include <map>
namespace fs = std::experimental::filesystem;

void save_image(double WIDTH, double HEIGHT, Color* image) 
{
	std::string s;
	int len = 5;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

	srand(time(NULL));
    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

	s[len] = 0;

	const char *temp = s.c_str();
	char name[25];
	sprintf(name, "./pictures/%s.ppm", temp);

	std::ofstream ofs(name, std::ios::out | std::ios::binary);
	ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
	for (int i = 0; i < WIDTH * HEIGHT; ++i) 
	{
		ofs << (unsigned char)(std::min(double(1), image[i].r) * 255) <<
			(unsigned char)(std::min(double(1), image[i].g) * 255) <<
			(unsigned char)(std::min(double(1), image[i].b) * 255);
	}
	ofs.close();
}

double clamp(const double& lo, const double& hi, const double& v)
{
	return std::max(lo, std::min(hi, v));
}

void load_textures(std::string path, std::map<std::string, Texture> &textures_map) 
{
    for (const auto & entry : fs::directory_iterator(path)) 
	{
		std::string name = entry.path().string();

		std::string last_element(name.substr(name.rfind("/") + 2));

		Texture texture(name.c_str());
		textures_map.insert(std::pair<std::string, Texture>(last_element, texture));
	}
}