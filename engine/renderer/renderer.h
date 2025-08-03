#ifndef RENDERER_H
#define RENDERER_H

typedef struct renderer_t renderer_t;
struct renderer_t;

internal void renderer_init(platform_handle_t window_handle);
internal void renderer_shutdown();

#endif // RENDERER_H
