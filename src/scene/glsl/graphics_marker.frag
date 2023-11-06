#version 450
#include "antialias.glsl"
#include "common.glsl"
#include "markers.glsl"

// NOTE: the values below must correspond to _enums.h

// Marker mode.
#define DVZ_MARKER_MODE_NONE     0
#define DVZ_MARKER_MODE_CODE     1
#define DVZ_MARKER_MODE_BITMAP   2
#define DVZ_MARKER_MODE_MSDF     3
#define DVZ_MARKER_MODE_MTSDF    4
#define DVZ_MARKER_MODE_SDF_MONO 5
#define DVZ_MARKER_MODE_SDF_RGBA 6

// Marker aspect.
#define DVZ_MARKER_ASPECT_FILLED  0
#define DVZ_MARKER_ASPECT_STROKE  1
#define DVZ_MARKER_ASPECT_OUTLINE 2

// Marker shape.
#define DVZ_MARKER_SHAPE_DISC     0
#define DVZ_MARKER_SHAPE_ASTERISK 1
#define DVZ_MARKER_SHAPE_CHEVRON  2
#define DVZ_MARKER_SHAPE_CLOVER   3
#define DVZ_MARKER_SHAPE_CLUB     4
#define DVZ_MARKER_SHAPE_CROSS    5
#define DVZ_MARKER_SHAPE_DIAMOND  6
#define DVZ_MARKER_SHAPE_ARROW    7
#define DVZ_MARKER_SHAPE_ELLIPSE  8
#define DVZ_MARKER_SHAPE_HBAR     9
#define DVZ_MARKER_SHAPE_HEART    10
#define DVZ_MARKER_SHAPE_INFINITY 11
#define DVZ_MARKER_SHAPE_PIN      12
#define DVZ_MARKER_SHAPE_RING     13
#define DVZ_MARKER_SHAPE_SPADE    14
#define DVZ_MARKER_SHAPE_SQUARE   15
#define DVZ_MARKER_SHAPE_TAG      16
#define DVZ_MARKER_SHAPE_TRIANGLE 17
#define DVZ_MARKER_SHAPE_VBAR     18

layout(constant_id = 0) const int MARKER_MODE = 0;   // code, sdf, bitmap...
layout(constant_id = 1) const int MARKER_ASPECT = 0; // filled, outline, stroke
layout(constant_id = 2) const int MARKER_SHAPE = 0;  // when using CODE mode, which shape to use

layout(binding = USER_BINDING) uniform MarkersParams
{
    vec4 edge_color;
    float edge_width;
}
params;

layout(location = 0) in vec4 color;
layout(location = 1) in float size;
layout(location = 2) in float angle;

layout(location = 0) out vec4 out_color;


float select_marker(vec2 P, float size)
{
    switch (MARKER_SHAPE)
    {

    case DVZ_MARKER_SHAPE_DISC:
        return marker_disc(P, size);
        break;

    case DVZ_MARKER_SHAPE_ASTERISK:
        return marker_asterisk(P, size);
        break;

    case DVZ_MARKER_SHAPE_CHEVRON:
        return marker_chevron(P, size);
        break;

    case DVZ_MARKER_SHAPE_CLOVER:
        return marker_clover(P, size);
        break;

    case DVZ_MARKER_SHAPE_CLUB:
        return marker_club(P, size);
        break;

    case DVZ_MARKER_SHAPE_CROSS:
        return marker_cross(P, size);
        break;

    case DVZ_MARKER_SHAPE_DIAMOND:
        return marker_diamond(P, size);
        break;

    case DVZ_MARKER_SHAPE_ARROW:
        return marker_arrow(P, size);
        break;

    case DVZ_MARKER_SHAPE_ELLIPSE:
        return marker_ellipse(P, size);
        break;

    case DVZ_MARKER_SHAPE_HBAR:
        return marker_hbar(P, size);
        break;

    case DVZ_MARKER_SHAPE_HEART:
        return marker_heart(P, size);
        break;

    case DVZ_MARKER_SHAPE_INFINITY:
        return marker_infinity(P, size);
        break;

    case DVZ_MARKER_SHAPE_PIN:
        return marker_pin(P, size);
        break;

    case DVZ_MARKER_SHAPE_RING:
        return marker_ring(P, size);
        break;

    case DVZ_MARKER_SHAPE_SPADE:
        return marker_spade(P, size);
        break;

    case DVZ_MARKER_SHAPE_SQUARE:
        return marker_square(P, size);
        break;

    case DVZ_MARKER_SHAPE_TAG:
        return marker_tag(P, size);
        break;

    case DVZ_MARKER_SHAPE_TRIANGLE:
        return marker_triangle(P, size);
        break;

    case DVZ_MARKER_SHAPE_VBAR:
        return marker_vbar(P, size);
        break;

    default:
        return 0.0;
    }
}


void main()
{
    CLIP;

    float a = angle;
    float c = cos(angle);
    float s = sin(angle);

    // Pixel coordinates in [-0.5, -0.5, +0.5, +0.5].
    vec2 P = gl_PointCoord.xy - vec2(0.5, 0.5);

    // NOTE: rescale according to the rotation, to keep the marker size fixed while
    // the underlying square marker container is bigger to account for the rotation
    // of the marker.
    P *= (abs(c) + abs(s));

    // Marker rotation.
    mat2 rot = mat2(c, s, -s, c);
    P = rot * P;

    // Marker SDF.
    float distance = select_marker(P * (size + 2 * params.edge_width + antialias), size);

    // Outline/filled.
    if (params.edge_width > 0)
    {
        out_color = outline(distance, params.edge_width, params.edge_color, color);
    }
    else
    {
        out_color = filled(distance, params.edge_width, color);
    }

    if (out_color.a < .01)
    {
        discard;
    }

    // DEBUG
    // out_color.a = max(out_color.a, .5);
    // out_color.b = .75;
}
