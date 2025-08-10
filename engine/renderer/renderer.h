#ifndef RENDERER_H
#define RENDERER_H

typedef struct renderer_t renderer_t;
struct renderer_t;

typedef struct renderer_pipeline_t renderer_pipeline_t;
struct renderer_pipeline_t;

typedef enum vertex_attr_type_t vertex_attr_type_t;
enum vertex_attr_type_t
{
    VERTEX_ATTR_TYPE_position,
    VERTEX_ATTR_TYPE_normal,
    VERTEX_ATTR_TYPE_color,
    VERTEX_ATTR_TYPE_uv,
    VERTEX_ATTR_TYPE_count
};

typedef struct vertex_t vertex_t;
struct vertex_t
{
    vec3_t position;
    vec3_t normal;
    vec3_t color;
    vec2_t uv;
};

internal void renderer_init(platform_handle_t window_handle);
internal void renderer_destroy();

internal void renderer_pipeline_init(renderer_pipeline_t* state);
internal void renderer_pipeline_destroy(renderer_pipeline_t* state);

#endif // RENDERER_H
