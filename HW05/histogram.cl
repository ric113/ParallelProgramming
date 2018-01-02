__kernel void histogram(__global const unsigned int* image_rgb, __global unsigned int* histogram_res, unsigned int size)
{
        int idx = get_global_id(0);
        int y = idx % 256;
        int x = idx / 256;

        int i;
        for(i = 0 ; i < size ; i ++) {

                if( i % 3 == x && y == image_rgb[i]) {
                        histogram_res[idx] ++;
                }
        }
}