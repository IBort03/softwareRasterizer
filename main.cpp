#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;

    // If line is steep, we transpose the image, this prevents gaps when calulating how to draw line using Bresenhams Line algorithm
    if (std::abs(x0-x1) < std::abs(y0-y1)) { 
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    // Fixes problem so that lines drawn have symmetry (a,b) = (b,a). Always draws from left to right
    if (x0 > x1) { 
        std::swap(x0, x1);
        std::swap(y0, y1);
    }   
    
    // Sets up variables for line drawing
    int dx = x1-x0;
    int dy = y1-y0;
    int derror = std::abs(dy) * 2;
    int error = 0;
    int y = y0;

    // Possible speed up option is to check for steep before entering for loop

    for (int x = x0; x <= x1; x++) { // Draws the line using Bresenham's line algorithm
        if (steep) {
            image.set(y, x, color); // Un-transposes now that calulations are done 
        } else {
            image.set(x, y, color);
        }

        // Error handling, this allows us to find the best straight line
        // from our current pixel to the end
        error += derror;
        // If error is greater than .5 then the line has moved up or down,
        // so y must be incremented or decremented accordingly for the best line
        if (error > dx) {
            y += (y1 > y0? 1:-1);
            error -= dx * 2;
        }
    }
}

int main(int argc, char** argv) {

    if (argc == 2) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/head.obj");
    }

	TGAImage image(width, height, TGAImage::RGB);
	
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1) % 3]);
            int x0 = (v0.x + 1.) * width/2;
            int y0 = (v0.y + 1.) * height/2;
            int x1 = (v1.x + 1.) * width/2;
            int y1 = (v1.y + 1.) * height/2;
            line(x0, y0, x1, y1, image, white);
        }
    }


	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
    delete model;
	return 0;
}
