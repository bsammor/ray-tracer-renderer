#include <cstdlib>
#include <cstdio>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture {
    public:
    unsigned char *data = NULL;
    int width = 0;
    int height = 0;
    int channels = 0;
    
    Texture() {}

    Texture(const char *filename)
    {
        stbi_set_flip_vertically_on_load(true); 
        data = stbi_load(filename, &width, &height, &channels, 0); 
        if (!data)
            std::cout << "Failed to load texture" << std::endl;

    }
};

