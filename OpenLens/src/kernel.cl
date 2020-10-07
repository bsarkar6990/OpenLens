
constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
#define read(i,j) read_imagef(imageA, sampler, (int2)(i, j))
#define SOBEL_RADIUS 1
__kernel void Transfer(read_only image2d_t imageA, write_only image2d_t imageC, const uint width, const uint height)
{
    const int gidx  = get_global_id(0);
    const int gidy  = get_global_id(1);
	int horizontal=1;
    int vertical=1;
    int keep_sign=1;
    int has_alpha=1;

	float4 hor_grad = 0.0f;
    float4 ver_grad = 0.0f;
    float4 gradient = 0.0f;

    int dst_width = get_global_size(0);
    int src_width = dst_width + SOBEL_RADIUS * 2;
    int i = gidx + SOBEL_RADIUS, j = gidy + SOBEL_RADIUS;

	float4 pix_fl = read((i - 1) , (j - 1));
    float4 pix_fm = read((i - 0) , (j - 1));
    float4 pix_fr = read((i + 1) , (j - 1));
    float4 pix_ml = read((i - 1) , (j - 0));
    float4 pix_mm = read((i - 0) , (j - 0));
    float4 pix_mr = read((i + 1) , (j - 0));
    float4 pix_bl = read((i - 1) , (j + 1));
    float4 pix_bm = read((i - 0) , (j + 1));
    float4 pix_br = read((i + 1) , (j + 1));

	if (horizontal)
    {
        hor_grad +=
            (-1.0f * pix_fl + 1.0f * pix_fr) +
            (-2.0f * pix_ml + 2.0f * pix_mr) +
            (-1.0f * pix_bl + 1.0f * pix_br);
    }
    if (vertical)
    {
        ver_grad +=
            ( 1.0f * pix_fl) + ( 2.0f * pix_fm) + ( 1.0f * pix_fr) +
            (-1.0f * pix_bl) + (-2.0f * pix_bm) + (-1.0f * pix_br);
    }

    if (horizontal && vertical)
    {
        /* sqrt(32.0) = 5.656854249492381 */
        gradient = sqrt(hor_grad * hor_grad +
                        ver_grad * ver_grad) / 5.656854249492381f;
    }
    else
    {
        if (keep_sign)
            gradient = hor_grad + ver_grad;
        else
            gradient = fabs(hor_grad + ver_grad);
    }

    if (has_alpha)
    {
      gradient.w = pix_mm.w;
    }
    else
    {
      gradient.w = 1.0f;
    }

     write_imagef(imageC, (int2)(width-gidx -1,gidy),(float4)(gradient.xyzw));
}
__kernel void TransferE(read_only image2d_t imageA, write_only image2d_t imageC, const uint width, const uint height)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);
	uint4 pixel = read_imageui(imageA, sampler, (int2)(x, y));
	
     write_imageui(imageC, (int2)(width-x-1,y),(uint4)(pixel.xyzw));
}