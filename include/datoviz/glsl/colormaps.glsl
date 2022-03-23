/*************************************************************************************************/
/*  Colormaps                                                                                    */
/*************************************************************************************************/

#include "random.glsl"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

#define CLAMP x = clamp(x, 0, 1);



/*************************************************************************************************/
/*  Colormap constants                                                                           */
/*************************************************************************************************/

#define DVZ_CMAP_BINARY 0
#define DVZ_CMAP_HSV 1
#define DVZ_CMAP_CIVIDIS 2
#define DVZ_CMAP_INFERNO 3
#define DVZ_CMAP_MAGMA 4
#define DVZ_CMAP_PLASMA 5
#define DVZ_CMAP_VIRIDIS 6
#define DVZ_CMAP_AUTUMN 25
#define DVZ_CMAP_BONE 26
#define DVZ_CMAP_COOL 27
#define DVZ_CMAP_COPPER 28
#define DVZ_CMAP_HOT 31
#define DVZ_CMAP_SPRING 33
#define DVZ_CMAP_SUMMER 34
#define DVZ_CMAP_WINTER 35
#define DVZ_CMAP_JET 61

// Custom colormap constants.
#define DVZ_CMAP_QUALMAP 124 // custom red-green gradient with fixed perceived luminance
#define DVZ_CMAP_RANDOM 239



/*************************************************************************************************/
/*  Utils                                                                                        */
/*************************************************************************************************/

vec4 rgb2hsv(vec4 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec4(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x, c.a);
}


vec4 hsv2rgb(vec4 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return vec4(c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y), c.a);
}



/*************************************************************************************************/
/*  Built-in colormaps                                                                           */
/*************************************************************************************************/

vec4 binary(float x) {
    float u = 1 - x;
    return vec4(u, u, u, 1);
}


// The following colormaps come from (quintic approximation):
// https://www.shadertoy.com/view/XtGGzG
vec4 viridis(float x)
{
    CLAMP
	vec4 x1 = vec4(1.0, x, x * x, x * x * x); // 1 x x2 x3
	vec4 x2 = x1 * x1.w * x; // x4 x5 x6 x7
	return vec4(
		dot(x1.xyzw, vec4(+0.280268003, -0.143510503, +2.225793877, -14.815088879)) + dot(x2.xy, vec2(+25.212752309, -11.772589584)),
		dot(x1.xyzw, vec4(-0.002117546, +1.617109353, -1.909305070, +2.701152864)) + dot(x2.xy, vec2(-1.685288385, +0.178738871)),
		dot(x1.xyzw, vec4(+0.300805501, +2.614650302, -12.019139090, +28.933559110)) + dot(x2.xy, vec2(-33.491294770, +13.762053843)), 1);
}


vec4 inferno(float x)
{
    CLAMP
	vec4 x1 = vec4( 1.0, x, x * x, x * x * x ); // 1 x x2 x3
	vec4 x2 = x1 * x1.w * x; // x4 x5 x6 x7
	return vec4(
		dot(x1.xyzw, vec4(-0.027780558, +1.228188385, +0.278906882, +3.892783760 )) + dot(x2.xy, vec2(-8.490712758, +4.069046086)),
		dot(x1.xyzw, vec4(+0.014065206, +0.015360518, +1.605395918, -4.821108251 )) + dot(x2.xy, vec2(+8.389314011, -4.193858954)),
		dot(x1.xyzw, vec4(-0.019628385, +3.122510347, -5.893222355, +2.798380308 )) + dot(x2.xy, vec2(-3.608884658, +4.324996022)), 1);
}


vec4 magma(float x)
{
    CLAMP
	vec4 x1 = vec4(1.0, x, x * x, x * x * x); // 1 x x2 x3
	vec4 x2 = x1 * x1.w * x; // x4 x5 x6 x7
	return vec4(
		dot(x1.xyzw, vec4(-0.023226960, +1.087154378, -0.109964741, +6.333665763)) + dot(x2.xy, vec2(-11.640596589, +5.33762535)),
		dot(x1.xyzw, vec4(+0.010680993, +0.176613780, +1.638227448, -6.743522237)) + dot(x2.xy, vec2(+11.426396979, -5.52323637)),
		dot(x1.xyzw, vec4(-0.008260782, +2.244286052, +3.005587601, -24.279769818)) + dot(x2.xy, vec2(+32.484310068, -12.68825973)), 1);
}


vec4 plasma(float x)
{
    CLAMP
	vec4 x1 = vec4(1.0, x, x * x, x * x * x); // 1 x x2 x3
	vec4 x2 = x1 * x1.w * x; // x4 x5 x6 x7
	return vec4(
		dot(x1.xyzw, vec4(+0.063861086, +1.992659096, -1.023901152, -0.490832805)) + dot(x2.xy, vec2(+1.308442123, -0.914547012)),
		dot(x1.xyzw, vec4(+0.049718590, -0.791144343, +2.892305078, +0.811726816)) + dot(x2.xy, vec2(-4.686502417, +2.717794514)),
		dot(x1.xyzw, vec4(+0.513275779, +1.580255060, -5.164414457, +4.559573646)) + dot(x2.xy, vec2(-1.916810682, +0.570638854)), 1);
}


// The following colormaps come from:
// https://github.com/kbinani/colormap-shaders
vec4 autumn(float x) {
    CLAMP
    return vec4(1.0, x, 0.0, 1.0);
}


vec4 winter(float x) {
    CLAMP
    return vec4(0.0, x, clamp(-0.5 * x + 1.0, 0.0, 1.0), 1.0);
}


vec4 summer(float x) {
    CLAMP
    return vec4(x, clamp(0.5 * x + 0.5, 0.0, 1.0), 0.4, 1.0);
}


vec4 spring(float x) {
    CLAMP
    return vec4(1.0, x, clamp(1.0 - x, 0.0, 1.0), 1.0);
}


vec4 copper(float x) {
    float r = clamp(80.0 / 63.0 * x + 5.0 / 252.0, 0.0, 1.0);
    float g = clamp(0.7936 * x - 0.0124, 0.0, 1.0);
    float b = clamp(796.0 / 1575.0 * x + 199.0 / 25200.0, 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}


vec4 hot(float x) {
    float r = clamp(8.0 / 3.0 * x, 0.0, 1.0);
    float g = clamp(8.0 / 3.0 * x - 1.0, 0.0, 1.0);
    float b = clamp(4.0 * x - 3.0, 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}


vec4 cool(float x) {
    float r = clamp((1.0 + 1.0 / 63.0) * x - 1.0 / 63.0, 0.0, 1.0);
    float g = clamp(-(1.0 + 1.0 / 63.0) * x + (1.0 + 1.0 / 63.0), 0.0, 1.0);
    float b = 1.0;
    return vec4(r, g, b, 1.0);
}



float _hsv_red(float x) {
    if (x < 0.5) return -6.0 * x + 67.0 / 32.0;
    else return 6.0 * x - 79.0 / 16.0;
}
float _hsv_green(float x) {
    if (x < 0.4) return 6.0 * x - 3.0 / 32.0;
    else return -6.0 * x + 79.0 / 16.0;
}
float _hsv_blue(float x) {
    if (x < 0.7) return 6.0 * x - 67.0 / 32.0;
    else return -6.0 * x + 195.0 / 32.0;
}
vec4 hsv(float x) {
    CLAMP
    float r = clamp(_hsv_red(x), 0.0, 1.0);
    float g = clamp(_hsv_green(x), 0.0, 1.0);
    float b = clamp(_hsv_blue(x), 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}



float _bone_red(float x) {
    if (x < 0.75) return 8.0 / 9.0 * x - (13.0 + 8.0 / 9.0) / 1000.0;
    else return (13.0 + 8.0 / 9.0) / 10.0 * x - (3.0 + 8.0 / 9.0) / 10.0;
}
float _bone_green(float x) {
    if (x <= 0.375) return 8.0 / 9.0 * x - (13.0 + 8.0 / 9.0) / 1000.0;
    else if (x <= 0.75) return (1.0 + 2.0 / 9.0) * x - (13.0 + 8.0 / 9.0) / 100.0;
    else return 8.0 / 9.0 * x + 1.0 / 9.0;
}
float _bone_blue(float x) {
    if (x <= 0.375) return (1.0 + 2.0 / 9.0) * x - (13.0 + 8.0 / 9.0) / 1000.0;
    else return 8.0 / 9.0 * x + 1.0 / 9.0;
}
vec4 bone(float x) {
    CLAMP
    float r = clamp(_bone_red(x), 0.0, 1.0);
    float g = clamp(_bone_green(x), 0.0, 1.0);
    float b = clamp(_bone_blue(x), 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}



float _jet_red(float x) {
    if (x < 0.7) return 4.0 * x - 1.5;
    else return -4.0 * x + 4.5;
}
float _jet_green(float x) {
    if (x < 0.5) return 4.0 * x - 0.5;
    else return -4.0 * x + 3.5;
}
float _jet_blue(float x) {
    if (x < 0.3) return 4.0 * x + 0.5;
    else return -4.0 * x + 2.5;
}
vec4 jet(float x) {
    CLAMP
    float r = clamp(_jet_red(x), 0.0, 1.0);
    float g = clamp(_jet_green(x), 0.0, 1.0);
    float b = clamp(_jet_blue(x), 0.0, 1.0);
    return vec4(r, g, b, 1.0);
}



/*************************************************************************************************/
/*  Custom colormaps                                                                             */
/*************************************************************************************************/

vec4 qualmap(float x) {
   if (x < 0.008) return vec4(1.000, 0.209, 0.283, 1);
    else if (x < 0.016) return vec4(1.000, 0.210, 0.272, 1);
    else if (x < 0.023) return vec4(1.000, 0.210, 0.262, 1);
    else if (x < 0.031) return vec4(1.000, 0.211, 0.252, 1);
    else if (x < 0.039) return vec4(1.000, 0.212, 0.243, 1);
    else if (x < 0.047) return vec4(1.000, 0.213, 0.233, 1);
    else if (x < 0.055) return vec4(1.000, 0.214, 0.224, 1);
    else if (x < 0.062) return vec4(1.000, 0.215, 0.214, 1);
    else if (x < 0.070) return vec4(1.000, 0.216, 0.205, 1);
    else if (x < 0.078) return vec4(1.000, 0.217, 0.196, 1);
    else if (x < 0.086) return vec4(1.000, 0.219, 0.188, 1);
    else if (x < 0.094) return vec4(1.000, 0.220, 0.179, 1);
    else if (x < 0.102) return vec4(1.000, 0.221, 0.171, 1);
    else if (x < 0.109) return vec4(1.000, 0.223, 0.162, 1);
    else if (x < 0.117) return vec4(1.000, 0.224, 0.154, 1);
    else if (x < 0.125) return vec4(1.000, 0.226, 0.146, 1);
    else if (x < 0.133) return vec4(1.000, 0.228, 0.139, 1);
    else if (x < 0.141) return vec4(1.000, 0.229, 0.131, 1);
    else if (x < 0.148) return vec4(1.000, 0.231, 0.124, 1);
    else if (x < 0.156) return vec4(1.000, 0.233, 0.117, 1);
    else if (x < 0.164) return vec4(1.000, 0.235, 0.109, 1);
    else if (x < 0.172) return vec4(1.000, 0.237, 0.103, 1);
    else if (x < 0.180) return vec4(1.000, 0.240, 0.096, 1);
    else if (x < 0.188) return vec4(1.000, 0.242, 0.089, 1);
    else if (x < 0.195) return vec4(1.000, 0.244, 0.083, 1);
    else if (x < 0.203) return vec4(1.000, 0.246, 0.076, 1);
    else if (x < 0.211) return vec4(1.000, 0.249, 0.070, 1);
    else if (x < 0.219) return vec4(1.000, 0.251, 0.064, 1);
    else if (x < 0.227) return vec4(1.000, 0.254, 0.058, 1);
    else if (x < 0.234) return vec4(1.000, 0.257, 0.053, 1);
    else if (x < 0.242) return vec4(1.000, 0.259, 0.047, 1);
    else if (x < 0.250) return vec4(1.000, 0.262, 0.042, 1);
    else if (x < 0.258) return vec4(1.000, 0.265, 0.037, 1);
    else if (x < 0.266) return vec4(1.000, 0.268, 0.031, 1);
    else if (x < 0.273) return vec4(1.000, 0.271, 0.026, 1);
    else if (x < 0.281) return vec4(1.000, 0.274, 0.022, 1);
    else if (x < 0.289) return vec4(1.000, 0.277, 0.017, 1);
    else if (x < 0.297) return vec4(1.000, 0.281, 0.012, 1);
    else if (x < 0.305) return vec4(1.000, 0.284, 0.008, 1);
    else if (x < 0.312) return vec4(1.000, 0.287, 0.004, 1);
    else if (x < 0.320) return vec4(1.000, 0.291, 0.000, 1);
    else if (x < 0.328) return vec4(1.000, 0.294, 0.000, 1);
    else if (x < 0.336) return vec4(1.000, 0.298, 0.000, 1);
    else if (x < 0.344) return vec4(1.000, 0.302, 0.000, 1);
    else if (x < 0.352) return vec4(1.000, 0.305, 0.000, 1);
    else if (x < 0.359) return vec4(1.000, 0.309, 0.000, 1);
    else if (x < 0.367) return vec4(1.000, 0.313, 0.000, 1);
    else if (x < 0.375) return vec4(1.000, 0.317, 0.000, 1);
    else if (x < 0.383) return vec4(1.000, 0.321, 0.000, 1);
    else if (x < 0.391) return vec4(1.000, 0.325, 0.000, 1);
    else if (x < 0.398) return vec4(1.000, 0.329, 0.000, 1);
    else if (x < 0.406) return vec4(1.000, 0.333, 0.000, 1);
    else if (x < 0.414) return vec4(1.000, 0.337, 0.000, 1);
    else if (x < 0.422) return vec4(1.000, 0.341, 0.000, 1);
    else if (x < 0.430) return vec4(1.000, 0.346, 0.000, 1);
    else if (x < 0.438) return vec4(1.000, 0.350, 0.000, 1);
    else if (x < 0.445) return vec4(1.000, 0.355, 0.000, 1);
    else if (x < 0.453) return vec4(1.000, 0.359, 0.000, 1);
    else if (x < 0.461) return vec4(1.000, 0.364, 0.000, 1);
    else if (x < 0.469) return vec4(1.000, 0.368, 0.000, 1);
    else if (x < 0.477) return vec4(1.000, 0.373, 0.000, 1);
    else if (x < 0.484) return vec4(1.000, 0.377, 0.000, 1);
    else if (x < 0.492) return vec4(1.000, 0.382, 0.000, 1);
    else if (x < 0.500) return vec4(1.000, 0.387, 0.000, 1);
    else if (x < 0.508) return vec4(1.000, 0.392, 0.000, 1);
    else if (x < 0.516) return vec4(1.000, 0.396, 0.000, 1);
    else if (x < 0.523) return vec4(1.000, 0.401, 0.000, 1);
    else if (x < 0.531) return vec4(1.000, 0.406, 0.000, 1);
    else if (x < 0.539) return vec4(0.990, 0.411, 0.000, 1);
    else if (x < 0.547) return vec4(0.977, 0.416, 0.000, 1);
    else if (x < 0.555) return vec4(0.964, 0.421, 0.000, 1);
    else if (x < 0.562) return vec4(0.951, 0.426, 0.000, 1);
    else if (x < 0.570) return vec4(0.938, 0.431, 0.000, 1);
    else if (x < 0.578) return vec4(0.925, 0.436, 0.000, 1);
    else if (x < 0.586) return vec4(0.912, 0.441, 0.000, 1);
    else if (x < 0.594) return vec4(0.898, 0.447, 0.000, 1);
    else if (x < 0.602) return vec4(0.885, 0.452, 0.000, 1);
    else if (x < 0.609) return vec4(0.871, 0.457, 0.000, 1);
    else if (x < 0.617) return vec4(0.858, 0.462, 0.000, 1);
    else if (x < 0.625) return vec4(0.844, 0.467, 0.000, 1);
    else if (x < 0.633) return vec4(0.830, 0.473, 0.000, 1);
    else if (x < 0.641) return vec4(0.816, 0.478, 0.000, 1);
    else if (x < 0.648) return vec4(0.802, 0.483, 0.000, 1);
    else if (x < 0.656) return vec4(0.788, 0.488, 0.000, 1);
    else if (x < 0.664) return vec4(0.774, 0.494, 0.000, 1);
    else if (x < 0.672) return vec4(0.760, 0.499, 0.000, 1);
    else if (x < 0.680) return vec4(0.746, 0.504, 0.000, 1);
    else if (x < 0.688) return vec4(0.732, 0.509, 0.000, 1);
    else if (x < 0.695) return vec4(0.718, 0.515, 0.000, 1);
    else if (x < 0.703) return vec4(0.704, 0.520, 0.000, 1);
    else if (x < 0.711) return vec4(0.690, 0.525, 0.000, 1);
    else if (x < 0.719) return vec4(0.675, 0.531, 0.000, 1);
    else if (x < 0.727) return vec4(0.661, 0.536, 0.000, 1);
    else if (x < 0.734) return vec4(0.647, 0.541, 0.000, 1);
    else if (x < 0.742) return vec4(0.633, 0.546, 0.000, 1);
    else if (x < 0.750) return vec4(0.618, 0.552, 0.000, 1);
    else if (x < 0.758) return vec4(0.604, 0.557, 0.000, 1);
    else if (x < 0.766) return vec4(0.590, 0.562, 0.000, 1);
    else if (x < 0.773) return vec4(0.576, 0.567, 0.000, 1);
    else if (x < 0.781) return vec4(0.562, 0.572, 0.000, 1);
    else if (x < 0.789) return vec4(0.547, 0.578, 0.000, 1);
    else if (x < 0.797) return vec4(0.533, 0.583, 0.000, 1);
    else if (x < 0.805) return vec4(0.519, 0.588, 0.000, 1);
    else if (x < 0.812) return vec4(0.505, 0.593, 0.000, 1);
    else if (x < 0.820) return vec4(0.491, 0.598, 0.000, 1);
    else if (x < 0.828) return vec4(0.477, 0.603, 0.000, 1);
    else if (x < 0.836) return vec4(0.463, 0.608, 0.000, 1);
    else if (x < 0.844) return vec4(0.449, 0.613, 0.000, 1);
    else if (x < 0.852) return vec4(0.435, 0.618, 0.000, 1);
    else if (x < 0.859) return vec4(0.422, 0.623, 0.000, 1);
    else if (x < 0.867) return vec4(0.408, 0.628, 0.000, 1);
    else if (x < 0.875) return vec4(0.394, 0.632, 0.000, 1);
    else if (x < 0.883) return vec4(0.381, 0.637, 0.000, 1);
    else if (x < 0.891) return vec4(0.367, 0.642, 0.000, 1);
    else if (x < 0.898) return vec4(0.354, 0.646, 0.002, 1);
    else if (x < 0.906) return vec4(0.340, 0.651, 0.007, 1);
    else if (x < 0.914) return vec4(0.327, 0.656, 0.011, 1);
    else if (x < 0.922) return vec4(0.314, 0.660, 0.016, 1);
    else if (x < 0.930) return vec4(0.301, 0.664, 0.021, 1);
    else if (x < 0.938) return vec4(0.288, 0.669, 0.026, 1);
    else if (x < 0.945) return vec4(0.275, 0.673, 0.032, 1);
    else if (x < 0.953) return vec4(0.263, 0.677, 0.037, 1);
    else if (x < 0.961) return vec4(0.250, 0.682, 0.043, 1);
    else if (x < 0.969) return vec4(0.237, 0.686, 0.048, 1);
    else if (x < 0.977) return vec4(0.225, 0.690, 0.054, 1);
    else if (x < 0.984) return vec4(0.213, 0.694, 0.061, 1);
    else if (x < 0.992) return vec4(0.201, 0.698, 0.067, 1);
    else return vec4(0.189, 0.702, 0.073, 1);
}



/*************************************************************************************************/
/*  Random colormap                                                                              */
/*************************************************************************************************/

vec4 random_colormap(float x) {
    vec3 rgb = vec3(random(x), random(x*2), random(x*3));
    return vec4(.25 + .75 * rgb, 1);
}



vec4 colormap(int cmap, float x) {
    CLAMP
    if (cmap == DVZ_CMAP_BINARY) return binary(x);
    else if (cmap == DVZ_CMAP_HSV) return hsv(x);
    else if (cmap == DVZ_CMAP_INFERNO) return inferno(x);
    else if (cmap == DVZ_CMAP_MAGMA) return magma(x);
    else if (cmap == DVZ_CMAP_PLASMA) return plasma(x);
    else if (cmap == DVZ_CMAP_VIRIDIS) return viridis(x);
    else if (cmap == DVZ_CMAP_AUTUMN) return autumn(x);
    else if (cmap == DVZ_CMAP_BONE) return bone(x);
    else if (cmap == DVZ_CMAP_COOL) return cool(x);
    else if (cmap == DVZ_CMAP_COPPER) return copper(x);
    else if (cmap == DVZ_CMAP_HOT) return hot(x);
    else if (cmap == DVZ_CMAP_SPRING) return spring(x);
    else if (cmap == DVZ_CMAP_SUMMER) return summer(x);
    else if (cmap == DVZ_CMAP_WINTER) return winter(x);
    else if (cmap == DVZ_CMAP_JET) return jet(x);

    // Custom colormaps.
    else if (cmap == DVZ_CMAP_QUALMAP) return qualmap(x);
    else if (cmap == DVZ_CMAP_RANDOM) return random_colormap(x);
    else return vec4(x, x, x, 1);
}
