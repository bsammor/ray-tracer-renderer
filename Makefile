OBJS	= main.o vec3.o light.o camera.o color.o plane.o triangle.o triangle_mesh.o sphere.o ray.o
SOURCE	= main.cpp vec3.cpp light.cpp camera.cpp color.cpp plane.cpp triangle.cpp triangle_mesh.cpp sphere.cpp ray.cpp 
HEADER	= vec3.h vec2.h light.h camera.h color.h plane.h object.h triangle.h triangle_mesh.h sphere.h ray.h tiny_obj_loader.h texture.h stb_image.h
OUT	= tracer
CC	 = g++
FLAGS	 = -g -c -Wall 
LFLAGS	 = -std=c++17 -lstdc++fs -ljpeg

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 

vec3.o: vec3.cpp
	$(CC) $(FLAGS) vec3.cpp 

light.o: light.cpp
	$(CC) $(FLAGS) light.cpp 

camera.o: camera.cpp
	$(CC) $(FLAGS) camera.cpp 

color.o: color.cpp
	$(CC) $(FLAGS) color.cpp 

plane.o: plane.cpp
	$(CC) $(FLAGS) plane.cpp 

triangle.o: triangle.cpp
	$(CC) $(FLAGS) triangle.cpp 

triangle_mesh.o: triangle_mesh.cpp
	$(CC) $(FLAGS) triangle_mesh.cpp 

sphere.o: sphere.cpp
	$(CC) $(FLAGS) sphere.cpp 

ray.o: ray.cpp
	$(CC) $(FLAGS) ray.cpp 

clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)