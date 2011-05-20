//--------------------------------------------------------------
__kernel void depthThreshold(__global unsigned char *pixels,
                             __global int near, __global int far) {
    int i = get_global_id(0);
    if(pixels[i]<far || pixels[i]>near) {
        pixels[i] = 0;
    }
}