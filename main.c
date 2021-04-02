#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>

void main()
{
    // read in 3x4.png as tbf
    struct rgb_img **tbf = malloc(sizeof(struct rgb_img **));
    read_in_img(tbf, "3x4.bin");

    // testing calc_energy
    struct rgb_img **tbf_energy = malloc(sizeof(struct rgb_img **));
    create_img(tbf_energy, (*tbf)->height, (*tbf)->width);
    calc_energy(*tbf, tbf_energy);
    print_grad(*tbf_energy);

}