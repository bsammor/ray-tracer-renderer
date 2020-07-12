#ifndef UTILITIES_H
#define UTILITIES_H
#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader.h>
#include <triangle.h>
#include <experimental/filesystem>
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
    for (int i = 0; i < len; ++i) 
    {
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

std::map<std::string, Texture> load_textures(std::string path) 
{
    std::map<std::string, Texture> textures_map;
    for (const auto & entry : fs::directory_iterator(path)) 
	{
		std::string name = entry.path().string();

		std::string last_element(name.substr(name.rfind("/") + 2));

		Texture texture(name.c_str());
		textures_map.insert(std::pair<std::string, Texture>(last_element, texture));
	}

    return textures_map;
}

void load_mesh(std::string inputfile, std::vector<std::shared_ptr<Object>> &scene) 
{
    std::string mat_dir = "models";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), mat_dir.c_str());

	if (!warn.empty()) 
    {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) 
    {
		std::cerr << err << std::endl;
	}

	if (!ret) 
    {
		exit(1);
	}

    for (auto& shape : shapes) 
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
        {
            Vec3 vt0, vt1, vt2;
            int fv = shape.mesh.num_face_vertices[f];
            tinyobj::index_t& idx0 = shape.mesh.indices[index_offset + 0];  // v0
            tinyobj::index_t& idx1 = shape.mesh.indices[index_offset + 1];  // v1
            tinyobj::index_t& idx2 = shape.mesh.indices[index_offset + 2];  // v2

            Vec3 v0 = Vec3(attrib.vertices[3 * idx0.vertex_index + 0],
                attrib.vertices[3 * idx0.vertex_index + 1],
                attrib.vertices[3 * idx0.vertex_index + 2]);
            Vec3 v1 = Vec3(attrib.vertices[3 * idx1.vertex_index + 0],
                attrib.vertices[3 * idx1.vertex_index + 1],
                attrib.vertices[3 * idx1.vertex_index + 2]);
            Vec3 v2 = Vec3(attrib.vertices[3 * idx2.vertex_index + 0],
                attrib.vertices[3 * idx2.vertex_index + 1],
                attrib.vertices[3 * idx2.vertex_index + 2]);

            if (attrib.texcoords.size() > 0) 
            {
                vt0 = Vec3(attrib.texcoords[2 * idx0.texcoord_index + 0],
                    attrib.texcoords[2 * idx0.texcoord_index + 1],
                    0.0);
                vt1 = Vec3(attrib.texcoords[2 * idx1.texcoord_index + 0],
                    attrib.texcoords[2 * idx1.texcoord_index + 1],
                    0.0);
                vt2 = Vec3(attrib.texcoords[2 * idx2.texcoord_index + 0],
                    attrib.texcoords[2 * idx2.texcoord_index + 1],
                    0.0);
            }

            int id = shape.mesh.material_ids[f];

            Color tri_color = Color(materials[id].diffuse[0], materials[id].diffuse[1], materials[id].diffuse[2]);
            std::shared_ptr<Triangle> tri = std::shared_ptr<Triangle>(new Triangle(v0, v1, v2, vt0, vt1, vt2, tri_color, diffuse));

            std::string tex = materials[id].diffuse_texname;
            std::string last_token = "";
            if (!tex.empty()) last_token = tex.substr(tex.rfind("/") + 2);

            tri->tex = last_token;
            scene.push_back(tri);
            index_offset += fv;
        }
    }
}

#endif