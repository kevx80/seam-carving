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

double min(double x, double y, double z)
{
    if (x < y){
        if (z < x){
            return (double)(z);
        } else {
            return (double)(x);
        }
    } else {
        if (z < y){
            return (double)(z);
        } else {
            return (double)(y);
        }
    }
}

double get_cost(double *arr, int width, int y, int x)
{
    return arr[(y * width) + x];
}

int lowest_energy_above(double *arr, int width, int y, int x)
{
    double lowest = get_cost(arr, width, y - 1, x);
    int i_lowest = x;
    double left;
    if (x != 0){
        left = get_cost(arr, width, y - 1, x - 1);
        if (left < lowest){
            lowest = left;
            i_lowest = x - 1;
        }
    }
    double right;
    if (x != (width - 1)){
        right = get_cost(arr, width, y - 1, x + 1);
        if (right < lowest){
            i_lowest = x + 1;
        }
    }
    return i_lowest;
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
    *best_arr = malloc(sizeof(double) * (int)(grad->height) * (int)(grad->width));
    int y, x;
    for (x = 0; x < grad->width; x++){
        int energy = get_pixel(grad, 0, x, 0);
        (*best_arr)[x] = (double)energy;
    }
    for (y = 1; y < grad->height; y++){
        for (x = 0; x < grad->width; x++){
            double l, m, r;
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

void recover_path(double *best, int height, int width, int **path)
{
    int x;
    int y = height - 1;
    int *list = malloc(sizeof(int) * height);
    
    // finding lowest value in bottom of list
    double min_x = get_cost(best, width, y, 0);
    int pos_min = 0;
    for (x = 1; x < width; x++){
        if (get_cost(best, width, y, x) < min_x){
            min_x = get_cost(best, width, y, x);
            pos_min = x;
        }
    }

    list[height - 1] = pos_min;

    // looking above
    for (y = height - 1; y > 0; y--){
        list[y - 1] = lowest_energy_above(best, width, y, list[y]);
    }

    *path = list;
}