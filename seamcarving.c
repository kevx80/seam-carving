#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>
#include <math.h>

#define RED 0
#define GRN 1
#define BLU 2

// helper functions
int find_x_component(struct rgb_img *im, int y, int x, int color)
{
    int pl, pr;
    if (x == 0){
        pl= get_pixel(im, y, im->width - 1, color);
    } else {
        pl= get_pixel(im, y, x - 1, color);
    }
    if (x + 1 == im->width) {
        pr = get_pixel(im, y, 0, color);
    } else {
        pr = get_pixel(im, y, x + 1, color);
    }
    return (pr - pl);
}

int find_y_component(struct rgb_img *im, int y, int x, int color)
{
    int pu, pd;
    if (y == 0){
        pu= get_pixel(im, im->height - 1, x, color);
    } else {
        pu= get_pixel(im, y - 1, x, color);
    }
    if (y + 1 == im->height) {
        pd = get_pixel(im, 0, x, color);
    } else {
        pd = get_pixel(im, y + 1, x, color);
    }
    return (pd - pu);
}

int find_energy(struct rgb_img *im, int y, int x)
{
   // calculate delta x
   int Rx = find_x_component(im, y, x, RED);
   int Gx = find_x_component(im, y, x, GRN);
   int Bx = find_x_component(im, y, x, BLU);
   int Dx = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);

   // calculate delta y
   int Ry = find_y_component(im, y, x, RED);
   int Gy = find_y_component(im, y, x, GRN);
   int By = find_y_component(im, y, x, BLU);
   int Dy = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);

   // calculate energy
   int energy = (int)(sqrt(Dx + Dy));
   return energy;
}

double min(int8_t x, int8_t y, int8_t z)
{
    int a = (int) x;
    int b = (int) y;
    int c = (int) z;
    if (a < b){
        if (c < a){
            return (double)(c);
        } else {
            return (double)(a);
        }
    } else {
        if (c < b){
            return (double)(c);
        } else {
            return (double)(b);
        }
    }
}

double get_cost(double *arr, int width, int y, int x)
{
    return arr[(y * width) + x];
}

// main functions
void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{
    create_img(grad, im->height, im->width);
    int x, y;
    for (y = 0; y < im->height; y++){
        for (x = 0; x < im->width; x++){
            int energy = find_energy(im, y ,x);
            int to_ins = (int)(energy / 10); 
            // ^ should technically be casted to uint8_t per instructions but set_pixel takes ints as arguments
            set_pixel(*grad, y, x, to_ins, to_ins, to_ins); 
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    *best_arr = (double *)malloc((int)(grad->height) * (int)(grad->width));
    int y, x;
    for (x = 0; x < grad->width; x++){
        int energy = get_pixel(grad, 0, x, 0);
        (*best_arr)[x] = energy;
    }
    for (y = 1; y < grad->height; y++){
        for (x = 0; x < grad->width; x++){
            int8_t l, m, r;
            m = get_cost(*best_arr, grad->width, y - 1, x);
            if (x != 0){
                l = get_cost(*best_arr, grad->width, y - 1, x - 1);
            } else {
                l = m;
            }
            if (x != (grad->width - 1)){
                r = get_cost(*best_arr, grad->width, y - 1, x + 1);
            } else {
                r = m;
            };
            double smallest = min(l, m, r);
            int energy = get_pixel(grad, y, x, 0);
            (*best_arr)[(y * grad->width) + x] = smallest + energy;
        }
    }
}