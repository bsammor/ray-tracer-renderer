#include "triangle_mesh.h"

TriangleMesh::TriangleMesh(const char* file)
{
	std::string inputfile = file;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}
}

bool TriangleMesh::intersected(Ray* ray, int index)
{
    bool polygon_hit = false;
    for (auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size();
            ++f)  // faces (triangles)
        {
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

            Triangle face = Triangle(v0, v1, v2, Color(1, 1, 1), diffuse);

            if (face.intersected(ray, index)) {
                polygon_hit = true;
            }
            index_offset += fv;
        }
    }
    return polygon_hit;
}

Vec3 TriangleMesh::get_normal(Vec3 point)
{
	return Vec3();
}


