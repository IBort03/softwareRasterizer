#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
Model *model = NULL;
const int width = 200;
const int height = 200;

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

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    // Sorts the vertices in ascending order, this lets us find height of triangle easier
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t2, t0);
    if (t1.y > t2.y) std::swap(t2, t1);

    // Calculate how much y needs to change per x of the line and the errors to check
    int dyL1 = t1.y - t0.y;
    int dyL2 = t2.y - t0.y;
    // The change of error is the same as the absolute value of the change of x
    int derrorL1 =  std::abs(t1.x - t0.x);
    int errorL1 = 0;
    int derrorL2 = std::abs(t2.x - t0.x);
    int errorL2 = 0;

    // Start both lines at the same point (the bottom vertex of the triangle)
    int xL1 = t0.x;
    int xL2 = xL1;

    int x1 = xL1;
    int x2 = xL2;;

    // For loop that draws the "first half" of the triangle using inverse of Bresenham's algorithm (y added by 1 and change x accordingly)
    for (int y = t0.y; y <= t1.y; y++) {

        // Draws the horizontl line bewteen the two points of the triangle
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; x++) {
            image.set(x, y, color);
        }

        // Add the change of x to the error of both lines
        errorL1 += derrorL1;
        errorL2 += derrorL2;

        // If the error goes above the change in y, that means it is x's turn to increase (either left or right)
        // While loops used since x may need to increment more often then y
        while (errorL1 >= dyL1) {
            xL1 += (t1.x > t0.x? 1:-1);
            errorL1 -= dyL1;
        }
        while (errorL2 >= dyL2) {
            xL2 += (t2.x > t0.x? 1:-1);
            errorL2 -= dyL2;
        }

        // Updates the x values
        x1 = xL1;
        x2 = xL2;
    }

    /*for (int i = halfTriangleHeight; i < triangleHeight; i++) {

        if (xLeft > xRight) std::swap(xLeft, xRight);
        for (int x = xLeft; x < xRight; x++) {
            image.set(x, y, color);
        }
    } */


    /*line(t0.x, t0.y, t1.x, t1.y, image, color);
    line(t1.x, t1.y, t2.x, t2.y, image, color);
    line(t2.x, t2.y, t0.x, t0.y, image, color);*/
}

int main(int argc, char** argv) {

    /* if (argc == 2) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/head.obj");
    }
    */

	TGAImage image(width, height, TGAImage::RGB);
	
    /*
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
    } */

    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);    


	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("triangles.tga");
    // delete model;
	return 0;
}
