#define WIDTH   get_global_size(0)
#define HEIGHT  get_global_size(1)

//--------------------------------------------------------------
unsigned char convolve(const unsigned char *matrix, __global int *template) {
    unsigned char val = 0;
    for(int i=0;i<25;i++) {
        val += matrix[i]*template[i];
    }
    return val;
}

//--------------------------------------------------------------
int getIndex(__constant const int i, __constant const int j) {
    return (i%WIDTH)*WIDTH+(j%HEIGHT);
}

//--------------------------------------------------------------
__kernel void distanceTransform(__global unsigned char *src, __global unsigned char *dst, __global int *L0, __global int *L45, __global int *L90, __global int *L135, __global int threshold) {
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    if(coord.x<=2 || coord.x>=WIDTH-2 || coord.y<=2 || coord.y>=HEIGHT-2) {
        dst[getIndex(coord.x, coord.y)] = 0;
        return;
    }
    unsigned char matrix[25];
    int m = 0;
    for(int i=-2;i<=2;i++)
        for(int j=-2;j<=2;j++)
            matrix[m++] = src[getIndex(coord.x+i, coord.y+j)];
    unsigned char v1 = convolve(matrix, L0);
    unsigned char v2 = convolve(matrix, L45);
    unsigned char v3 = convolve(matrix, L90);
    unsigned char v4 = convolve(matrix, L135);
    unsigned char max1 = v1 > v2 ? v1 : v2;
	unsigned char max2 = v3 > v4 ? v3 : v4;
    dst[getIndex(coord.x, coord.y)] = (max1>max2?max1:max2)>threshold ? 255 : 0;
}

//--------------------------------------------------------------
__kernel void depthThreshold(__global unsigned char *pixels, __global int near, __global int far) {
    int i = get_global_id(0);
    if(pixels[i]<far || pixels[i]>near) {
        pixels[i] = 0;
    }
    else {
        pixels[i] = 255;
    }
}