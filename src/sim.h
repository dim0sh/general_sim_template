#ifndef SIM_H
#define SIM_H

#include "../lib/raylib/raylib.h"
#include "../lib/raylib/rlgl.h"
#include "../lib/microui/microui.h"

#include <stdlib.h>

#if defined(DA_ST_REALLOC) && !defined(DA_ST_FREE) || !defined(DA_ST_REALLOC) && defined(DA_ST_FREE)
#error "The custom allocation flag (DA_ST_CUSTOM_ALLOC) must be defined and both realloc and free must be defined, one is not sufficient."
#endif
#if !defined(DA_ST_REALLOC) && !defined(DA_ST_FREE)
#include <stdlib.h>
#define DA_ST_REALLOC(allocator,pointer,size) realloc(pointer,size)
#define DA_ST_FREE(allocator,pointer) free(pointer)
#endif

#if defined SIM_MODE_3D && defined SIM_MODE_2D
#error "only one simulation mode permitted at one time"
#endif

#if !defined SIM_MODE_3D && !defined SIM_MODE_2D
#define SIM_MODE_3D
#endif

#if defined SIM_MODE_3D
#define SimulationDrawingModeBegin(camera) BeginMode3D(camera)
#define SimulationDrawingModeEnd() EndMode3D()
#elif defined SIM_MODE_2D
#define SimulationDrawingModeBegin(camera) BeginMode2D(camera)
#define SimulationDrawingModeEnd() EndMode2D()
#endif

typedef struct Base_SIM_Model {
    int fps;
    int window_width;
    int window_height;
    const char *title;
    #if defined SIM_MODE_3D
    Camera camera;
    int camera_mode;
    #elif defined SIM_MODE_2D
    Camera2D camera;
    float camera_zoom;
    #endif
    mu_Context *ui_ctx;
    Font font;
} base_sim_model;

base_sim_model *base_sim_model_init(
    int fps,
    int window_width,
    int window_height,
    const char *title,
    void * allocator,
    #if defined SIM_MODE_3D
    int camera_mode
    #elif defined SIM_MODE_2D
    float camera_zoom
    #endif
);

typedef struct DataModel data_model;

typedef struct SimModel {
    base_sim_model *base_model;
    data_model *data_model;
} sim_model;

sim_model *sim_model_init(
    int fps, 
    int window_width, 
    int window_height, 
    const char *title, 
    #if defined SIM_MODE_3D
    int camera_mode,
    #elif defined SIM_MODE_2D
    float camera_zoom,
    #endif
    #if defined DA_ST_CUSTOM_ALLOC
    void * allocator,
    #endif
    data_model *data_model
);

void sim_init(sim_model *model, bool cursor);
void sim_update(sim_model *model);
void sim_draw(sim_model *model);
void sim_destroy(sim_model *model);
void sim_window(sim_model *model);

#ifdef SIMULATION_IMPLEMENTATION

base_sim_model *base_sim_model_init(
    int fps,
    int window_width,
    int window_height,
    const char *title,
    void * allocator,
    #if defined SIM_MODE_3D
    int camera_mode
    #elif defined SIM_MODE_2D
    float camera_zoom
    #endif
) {
    // base_sim_model *model = (base_sim_model*)malloc(sizeof(*model));
    base_sim_model *model = (base_sim_model*)DA_ST_REALLOC(allocator,NULL,sizeof(*model));
    model->fps = fps;
    model->window_width = window_width;
    model->window_height = window_height;
    model->title = title;
    #if defined SIM_MODE_3D
    model->camera_mode = camera_mode;
    Camera camera = { 0 };
    camera.position = (Vector3){ 20.0f, 20.0f, 20.0f };    
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          
    camera.fovy = 104.0f;                                
    camera.projection = CAMERA_PERSPECTIVE; 
    model->camera = camera;
    #elif defined SIM_MODE_2D
    Camera2D camera = {0};
    camera.zoom = 1.0f;
    model->camera = camera;
    #endif
    model->ui_ctx = (mu_Context*)DA_ST_REALLOC(allocator,NULL,sizeof(mu_Context));
    return model;
}

sim_model *sim_model_init(
    int fps, 
    int window_width, 
    int window_height, 
    const char *title, 
    #if defined SIM_MODE_3D
    int camera_mode,
    #elif defined SIM_MODE_2D
    float camera_zoom,
    #endif
    #if defined DA_ST_CUSTOM_ALLOC
    void * allocator,
    #endif
    data_model *data_model
) {
    sim_model *model = (sim_model*)malloc(sizeof(*model));
    #if defined DA_ST_CUSTOM_ALLOC
    #if defined SIM_MODE_3D
    model->base_model = base_sim_model_init(fps, window_width, window_height, title, allocator, camera_mode);
    #elif defined SIM_MODE_2D
    model->base_model = base_sim_model_init(fps, window_width, window_height, title, allocator, camera_zoom);
    #endif
    #else
    #if defined SIM_MODE_3D
    model->base_model = base_sim_model_init(fps, window_width, window_height, title, NULL, camera_mode);
    #elif defined SIM_MODE_2D
    model->base_model = base_sim_model_init(fps, window_width, window_height, title, NULL, camera_zoom);
    #endif
    #endif
    model->data_model = data_model;
    return model;
}

void internal_sim_destroy(sim_model *model) {
    sim_destroy(model);

    free(model->base_model->ui_ctx);
    free(model->base_model);
}

void simulate(sim_model *model, bool cursor) {
    sim_init(model, true);           

    sim_window(model);

    internal_sim_destroy(model);
}

#endif

#define DrawingModeScope(camera,content) do{\
    SimulationDrawingModeBegin(camera);\
    {content};\
    SimulationDrawingModeEnd();\
}while(0)

void sim_loop();

#endif