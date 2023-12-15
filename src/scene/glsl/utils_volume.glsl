
#define VOLUME_TYPE_SCALAR 0
#define VOLUME_TYPE_RGBA   1

#define VOLUME_COLOR_DIRECT   0
#define VOLUME_COLOR_COLORMAP 1

#define VOLUME_DIR_FRONT_BACK 0
#define VOLUME_DIR_BACK_FRONT 1


vec4 fetch_color(ivec2 modes, sampler3D tex_density, vec3 uvw, float transfer)
{
    // modes is (VOLUME_TYPE, VOLUME_COLOR)
    vec4 color = vec4(0);
    if (modes.x == VOLUME_TYPE_SCALAR)
    {
        float v = texture(tex_density, uvw).r;
        v = clamp(v, 0, .9999);

        if (modes.y == VOLUME_COLOR_DIRECT)
        {
            color = vec4(1, 1, 1, v);
        }
        else if (modes.y == VOLUME_COLOR_COLORMAP)
        {
            color = colormap(DVZ_CMAP_HSV, v);
            color.a = v;
        }
    }
    else if (modes.x == VOLUME_TYPE_RGBA)
    {
        color = texture(tex_density, uvw);
    }

    // Transfer function.
    color.a = clamp(color.a * transfer, 0, 1);

    return color;
}
