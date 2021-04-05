#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>

// code to print dynamic array
void print_dyn(double *arr, int height, int width)
{
    int y, x;
    for (y = 0; y < height; y++){
        for (x = 0; x < width; x++){
            printf("%lf ", arr[(y * width) + x]);
        }
        printf("\n");
    }
    printf("\n");
}

void main()
{
    // read in 3x4.bin as tbf
    struct rgb_img **tbf = malloc(sizeof(struct rgb_img **));
    read_in_img(tbf, "3x4.bin");

    // testing calc_energy
    struct rgb_img **tbf_energy = malloc(sizeof(struct rgb_img **));
    create_img(tbf_energy, (*tbf)->height, (*tbf)->width);
    calc_energy(*tbf, tbf_energy);
    print_grad(*tbf_energy);

    // read in 6x5.bin as sbf
    struct rgb_img **sbf = malloc(sizeof(struct rgb_img **));
    read_in_img(sbf, "6x5.bin");

    // testing dynamic_seam
    struct rgb_img **sbf_energy = malloc(sizeof(struct rgb_img **));
    create_img(sbf_energy, (*sbf)->height, (*sbf)->width);
    calc_energy(*sbf, sbf_energy);
    double **sbf_dyn = malloc(sizeof(double **));
    dynamic_seam(*sbf_energy, sbf_dyn);
    printf("\n");
    print_dyn(*sbf_dyn, (*sbf)->height, (*sbf)->width);

    // testing recover_path
    int **sbf_path = malloc(sizeof(int **));
    recover_path(*sbf_dyn, (*sbf)->height, (*sbf)->width, sbf_path);
    for (int i = 0; i < (*sbf)->height; i++){
        printf("%d, ", (*sbf_path)[i]);
    }

    // testing remove_seam
    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 5; i++){
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);
}