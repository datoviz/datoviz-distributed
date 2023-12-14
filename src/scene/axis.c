/*************************************************************************************************/
/*  Axis                                                                                         */
/*************************************************************************************************/


/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/axis.h"
#include "_macros.h"
#include "scene/atlas.h"
#include "scene/colormaps.h"
#include "scene/font.h"
#include "scene/visuals/glyph.h"
#include "scene/visuals/segment.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

#define MINOR_TICKS_PER_INTERVAL 4



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct AtlasFont AtlasFont;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct AtlasFont
{
    unsigned long ttf_size;
    unsigned char* ttf_bytes;
    DvzAtlas* atlas;
    DvzFont* font;
};



/*************************************************************************************************/
/*  Util functions                                                                               */
/*************************************************************************************************/

static AtlasFont _load_font(void)
{
    // Load the font ttf bytes.
    unsigned long ttf_size = 0;
    unsigned char* ttf_bytes = dvz_resource_font("Roboto_Medium", &ttf_size);
    ASSERT(ttf_size > 0);
    ANN(ttf_bytes);

    // Create the font.
    DvzFont* font = dvz_font(ttf_size, ttf_bytes);

    // Create the atlas.
    DvzAtlas* atlas = dvz_atlas(ttf_size, ttf_bytes);

    // Generate the atlas.
    dvz_atlas_generate(atlas);

    AtlasFont af = {.ttf_size = ttf_size, .ttf_bytes = ttf_bytes, .atlas = atlas, .font = font};
    return af;
}



/*************************************************************************************************/
/*  Allocation functions                                                                         */
/*************************************************************************************************/

static inline uint32_t _tick_count(DvzAxis* axis)
{
    ANN(axis);
    ANN(axis->segment);
    return axis->segment->item_count;
}



static inline uint32_t _glyph_count(DvzAxis* axis)
{
    ANN(axis);
    ANN(axis->glyph);
    return axis->glyph->item_count;
}



static inline uint32_t _minor_tick_count(uint32_t tick_count)
{
    return (tick_count - 1) * MINOR_TICKS_PER_INTERVAL;
}



// NOTE: the caller must FREE the output
static inline void* _repeat(uint32_t item_count, DvzSize item_size, void* value)
{
    void* out = (vec3*)calloc(item_count, item_size);
    for (uint32_t i = 0; i < item_count; i++)
    {
        memcpy((void*)((uint64_t)out + i * item_size), value, item_size);
    }
    return out;
}



// NOTE: the caller must FREE the output
static inline void* _repeat_group(
    DvzSize item_size, uint32_t item_count, uint32_t group_count, uint32_t* group_size,
    void* group_values, bool uniform)
{
    void* out = (vec3*)calloc(item_count, item_size);
    uint32_t k = 0;
    DvzSize item_size_src = uniform ? 0 : item_size;
    for (uint32_t i = 0; i < group_count; i++)
    {
        for (uint32_t j = 0; j < group_size[i]; j++)
        {
            ASSERT(k < item_count);
            memcpy(
                (void*)((uint64_t)out + (k++) * item_size),
                (void*)((uint64_t)group_values + i * item_size_src), //
                item_size);
        }
    }
    ASSERT(k == item_count);
    return out;
}



static inline void
set_groups(DvzAxis* axis, uint32_t glyph_count, uint32_t tick_count, uint32_t* group_size)
{
    ANN(axis);
    ANN(group_size);
    ASSERT(glyph_count > 0);
    ASSERT(tick_count > 0);

    axis->glyph_count = glyph_count;
    axis->tick_count = tick_count;
    axis->group_size = group_size;

    uint32_t n_major = axis->tick_count;
    uint32_t n_minor = _minor_tick_count(n_major);
    uint32_t n_total = n_major + n_minor;

    dvz_segment_alloc(axis->segment, n_total);
    dvz_glyph_alloc(axis->glyph, glyph_count);
}



/*************************************************************************************************/
/*  Tick computation                                                                             */
/*************************************************************************************************/

static inline vec3* make_tick_positions(DvzAxis* axis, double* values)
{
    ANN(axis);
    ANN(values);
    uint32_t tick_count = axis->tick_count;

    // axis->p0 corresponds to axis->dmin
    // axis->p1 corresponds to axis->dmax
    double dmin = axis->dmin;
    double dmax = axis->dmax;
    ASSERT(dmin < dmax);
    double denom = 1. / (dmax - dmin);
    ASSERT(denom > 0);
    double d = 0;
    double a = 0; // rescaled value between 0 and 1

    float px = axis->p1[0] - axis->p0[0];
    float py = axis->p1[1] - axis->p0[1];
    float pz = axis->p1[2] - axis->p0[2];

    vec3* tick_positions = (vec3*)calloc(tick_count, sizeof(vec3));
    for (uint32_t i = 0; i < tick_count; i++)
    {
        d = values[i];
        a = (d - dmin) * denom;

        tick_positions[i][0] = axis->p0[0] + px * a;
        tick_positions[i][1] = axis->p0[1] + py * a;
        tick_positions[i][2] = axis->p0[2] + pz * a;
    }

    return tick_positions;
}



/*************************************************************************************************/
/*  Tick functions                                                                               */
/*************************************************************************************************/

static inline void set_segment_pos(DvzAxis* axis, vec3* positions)
{
    ANN(axis);

    DvzVisual* segment = axis->segment;
    ANN(segment);

    uint32_t n_major = axis->tick_count;
    uint32_t n_minor = _minor_tick_count(n_major);
    uint32_t n_total = n_major + n_minor;

    // Concatenation of major and minor ticks.
    vec3* pos = (vec3*)calloc(n_total, sizeof(vec3));
    memcpy(pos, positions, n_major * sizeof(vec3));

    // Generate the minor ticks.
    uint32_t major = 0;
    uint32_t minor = 0;
    vec3* target = &pos[n_major];
    float dx = (positions[1][0] - positions[0][0]) / (MINOR_TICKS_PER_INTERVAL + 1);
    float dy = (positions[1][1] - positions[0][1]) / (MINOR_TICKS_PER_INTERVAL + 1);
    float dz = (positions[1][2] - positions[0][2]) / (MINOR_TICKS_PER_INTERVAL + 1);
    for (uint32_t i = 0; i < n_minor; i++)
    {
        major = i / MINOR_TICKS_PER_INTERVAL;
        minor = i % MINOR_TICKS_PER_INTERVAL;
        target[i][0] = positions[major][0] + (minor + 1) * dx;
        target[i][1] = positions[major][1] + (minor + 1) * dy;
        target[i][2] = positions[major][2] + (minor + 1) * dz;
    }

    dvz_segment_position(segment, 0, n_total, pos, pos, 0);
    FREE(pos);
}



static inline void set_segment_color(DvzAxis* axis)
{
    ANN(axis);

    DvzVisual* segment = axis->segment;
    ANN(segment);

    uint32_t n_major = axis->tick_count;
    uint32_t n_minor = _minor_tick_count(n_major);
    uint32_t n_total = n_major + n_minor;

    // Colors of the major and minor ticks.
    cvec4* colors = (cvec4*)calloc(n_total, sizeof(cvec4));
    for (uint32_t i = 0; i < n_major; i++)
    {
        memcpy(colors[i], axis->color_major, sizeof(cvec4));
    }
    for (uint32_t i = 0; i < n_minor; i++)
    {
        memcpy(colors[n_major + i], axis->color_minor, sizeof(cvec4));
    }

    dvz_segment_color(segment, 0, n_total, colors, 0);
    FREE(colors);
}



static inline void set_segment_shift(DvzAxis* axis)
{
    ANN(axis);

    DvzVisual* segment = axis->segment;
    ANN(segment);

    uint32_t n_major = axis->tick_count;
    uint32_t n_minor = _minor_tick_count(n_major);
    uint32_t n_total = n_major + n_minor;

    // Vector pointing from p0 to p1.
    vec3 u = {0};
    glm_vec3_sub(axis->p1, axis->p0, u);
    glm_vec3_normalize(u);
    // NOTE: this only works in 2D.

    // Tick length.
    float major_length = axis->tick_length[2];
    float minor_length = axis->tick_length[3];

    // Major and minor ticks.
    vec4* shift = (vec4*)calloc(n_total, sizeof(vec4));
    for (uint32_t i = 0; i < n_major; i++)
    {
        shift[i][2] = -u[1] * major_length;
        shift[i][3] = +u[0] * major_length;
    }
    for (uint32_t i = 0; i < n_minor; i++)
    {
        shift[n_major + i][2] = -u[1] * minor_length;
        shift[n_major + i][3] = +u[0] * minor_length;
    }
    // NOTE: this only works in 2D. In 3D, need to use end positions and shift=0.
    dvz_segment_shift(segment, 0, n_total, shift, 0);
    FREE(shift);
}



static inline void set_segment_width(DvzAxis* axis)
{
    ANN(axis);

    DvzVisual* segment = axis->segment;
    ANN(segment);

    uint32_t n_major = axis->tick_count;
    uint32_t n_minor = _minor_tick_count(n_major);
    uint32_t n_total = n_major + n_minor;

    // Widths of the major and minor ticks.
    float* width = (float*)calloc(n_total, sizeof(float));
    for (uint32_t i = 0; i < n_major; i++)
    {
        width[i] = axis->tick_width[2]; // major
    }
    for (uint32_t i = 0; i < n_minor; i++)
    {
        width[n_major + i] = axis->tick_width[3]; // minor
    }

    dvz_segment_linewidth(segment, 0, n_total, width, 0);
    FREE(width);
}



/*************************************************************************************************/
/*  Glyph functions                                                                              */
/*************************************************************************************************/

// NOTE: size of positions array is group_count=tick_count
static inline void set_glyph_pos(DvzAxis* axis, vec3* positions)
{
    ANN(axis);

    uint32_t glyph_count = axis->glyph_count;
    uint32_t group_count = axis->tick_count;
    uint32_t* group_size = axis->group_size;

    ASSERT(glyph_count > 0);
    ASSERT(group_count > 0);
    ANN(group_size);
    ANN(positions);
    DvzVisual* glyph = axis->glyph;

    vec3* pos =
        _repeat_group(sizeof(vec3), glyph_count, group_count, group_size, (void*)positions, false);
    dvz_glyph_position(glyph, 0, glyph_count, pos, 0);
    FREE(pos);

    vec2* anchor = (vec2*)_repeat(glyph_count, sizeof(vec2), (vec2){-.5, -1.5});
    dvz_glyph_anchor(glyph, 0, glyph_count, anchor, 0);
    FREE(anchor)
}



static inline void set_glyph_color(DvzAxis* axis)
{
    ANN(axis);

    uint32_t glyph_count = axis->glyph_count;
    uint32_t group_count = axis->tick_count;
    uint32_t* group_size = axis->group_size;

    ASSERT(glyph_count > 0);
    ASSERT(group_count > 0);
    ANN(group_size);
    DvzVisual* glyph = axis->glyph;
    cvec4* colors = _repeat_group(
        sizeof(cvec4), glyph_count, group_count, group_size, (void*)axis->color_glyph, true);
    dvz_glyph_color(glyph, 0, glyph_count, colors, 0);
    FREE(colors);
}



static inline void set_text(DvzAxis* axis, const char* glyphs)
{
    // NOTE: text is the concatenation of all group strings, without trailing zeros
    ANN(axis);

    // Set the size and shift properties of the glyph vsual by using the font to compute the
    // layout.
    uint32_t n = strnlen(glyphs, 65536); // NOTE: hard-coded maximal text size
    vec4* xywh = dvz_font_ascii(axis->font, glyphs);

    // NOTE: remove the x0 offset for each group.
    uint32_t glyph_count = axis->glyph_count;
    uint32_t group_count = axis->tick_count;
    uint32_t* group_size = axis->group_size;
    ASSERT(glyph_count > 0);
    ASSERT(group_count > 0);
    ANN(group_size);
    float x0 = 0.0;
    uint32_t k = 0;
    for (uint32_t i = 0; i < group_count; i++)
    {
        x0 = xywh[k][0];
        for (uint32_t j = 0; j < group_size[i]; j++)
        {
            ASSERT(k < n);
            xywh[k++][0] -= x0;
        }
    }
    ASSERT(k == glyph_count);

    dvz_glyph_xywh(axis->glyph, 0, n, xywh, (vec2){0, 0}, 0); // TODO: offset
    FREE(xywh);

    dvz_glyph_ascii(axis->glyph, glyphs);
}



/*************************************************************************************************/
/*  General functions                                                                            */
/*************************************************************************************************/

DvzAxis* dvz_axis(DvzBatch* batch, int flags)
{
    DvzAxis* axis = (DvzAxis*)calloc(1, sizeof(DvzAxis));
    axis->flags = flags;

    axis->segment = dvz_segment(batch, 0);
    axis->glyph = dvz_glyph(batch, 0);

    // Load the font and generate the atlas.
    AtlasFont af = _load_font();
    axis->atlas = af.atlas;
    axis->font = af.font;

    // Upload the atlas texture to the glyph visual.
    dvz_glyph_atlas(axis->glyph, axis->atlas);

    return axis;
}



DvzVisual* dvz_axis_segment(DvzAxis* axis)
{
    ANN(axis);
    return axis->segment;
}



DvzVisual* dvz_axis_glyph(DvzAxis* axis)
{
    ANN(axis);
    return axis->glyph;
}



void dvz_axis_destroy(DvzAxis* axis)
{
    ANN(axis);

    dvz_visual_destroy(axis->segment);
    dvz_visual_destroy(axis->glyph);

    dvz_atlas_destroy(axis->atlas);
    dvz_font_destroy(axis->font);

    FREE(axis);
}



/*************************************************************************************************/
/*  Global parameters                                                                            */
/*************************************************************************************************/

void dvz_axis_size(DvzAxis* axis, float font_size)
{
    ANN(axis);
    DvzVisual* glyph = axis->glyph;
    ANN(glyph);

    dvz_font_size(axis->font, font_size);
}



void dvz_axis_width(DvzAxis* axis, float lim, float grid, float major, float minor)
{
    ANN(axis);
    axis->tick_width[0] = lim;
    axis->tick_width[1] = grid;
    axis->tick_width[2] = major;
    axis->tick_width[3] = minor;
}



void dvz_axis_length(DvzAxis* axis, float lim, float grid, float major, float minor)
{
    ANN(axis);
    axis->tick_length[0] = lim;
    axis->tick_length[1] = grid;
    axis->tick_length[2] = major;
    axis->tick_length[3] = minor;
}



void dvz_axis_color(DvzAxis* axis, cvec4 glyph, cvec4 lim, cvec4 grid, cvec4 major, cvec4 minor)
{
    ANN(axis);
    memcpy(axis->color_glyph, glyph, sizeof(cvec4));
    memcpy(axis->color_lim, lim, sizeof(cvec4));
    memcpy(axis->color_grid, grid, sizeof(cvec4));
    memcpy(axis->color_major, major, sizeof(cvec4));
    memcpy(axis->color_minor, minor, sizeof(cvec4));
}



void dvz_axis_pos(DvzAxis* axis, double dmin, double dmax, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
    ANN(axis);
    ASSERT(dmin < dmax);

    axis->dmin = dmin;
    axis->dmax = dmax;

    _vec3_copy(p0, axis->p0);
    _vec3_copy(p1, axis->p1);
    _vec3_copy(p2, axis->p2);
    _vec3_copy(p3, axis->p3);
}



/*************************************************************************************************/
/*  Ticks and glyphs                                                                             */
/*************************************************************************************************/

void dvz_axis_set(
    DvzAxis* axis, uint32_t tick_count, double* values, //
    uint32_t glyph_count, char* glyphs, uint32_t* index, uint32_t* length)
{
    ANN(axis);

    // Allocation.
    set_groups(axis, glyph_count, tick_count, length);

    // Segment.
    set_segment_width(axis);

    // Tick positions.
    vec3* tick_positions = make_tick_positions(axis, values);
    set_glyph_pos(axis, tick_positions);
    set_segment_pos(axis, tick_positions);
    FREE(tick_positions);

    // Tick width and length.
    set_segment_width(axis);
    set_segment_shift(axis);

    // Colors.
    set_segment_color(axis);
    set_glyph_color(axis);

    set_text(axis, glyphs);
}



void dvz_axis_get(DvzAxis* axis, DvzMVP* mvp, vec2 out_d)
{
    ANN(axis);
    // TODO
    // compute dmin, dmax of the visible viewbox
}



int dvz_axis_direction(DvzAxis* axis, DvzMVP* mvp)
{
    ANN(axis);
    // TODO
    // returns 0 for horizontal, 1 for vertical. depends on the intersection or not
    // of two projected boxes with maximal label length
    return 0;
}
