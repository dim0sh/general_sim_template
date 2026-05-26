#ifndef SIM_H
#define SIM_H

#include "../lib/raylib/raylib.h"
#include "../lib/raylib/rlgl.h"
#include "../lib/microui/microui.h"

#include <stdlib.h>

typedef struct Base_SIM_Model {
    int fps;
    int window_width;
    int window_height;
    const char *title;
    Camera camera;
    int camera_mode;
    mu_Context *ui_ctx;
    Font font;
} base_sim_model;

base_sim_model *base_sim_model_init(
    int fps,
    int window_width,
    int window_height,
    const char *title,
    int camera_mode
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
    int camera_mode,
    data_model *data_model
);

// void sim_init(sim_model *model, bool cursor);
// void sim_update(sim_model *model);
// void sim_draw(sim_model *model);

#ifdef SIMULATION_IMPLEMENTATION
base_sim_model *base_sim_model_init(
    int fps,
    int window_width,
    int window_height,
    const char *title,
    int camera_mode
) {
    base_sim_model *model = (base_sim_model*)malloc(sizeof(*model));
    model->fps = fps;
    model->window_width = window_width;
    model->window_height = window_height;
    model->title = title;
    model->camera_mode = camera_mode;
    Camera camera = { 0 };
    camera.position = (Vector3){ 20.0f, 20.0f, 20.0f };    
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          
    camera.fovy = 104.0f;                                
    camera.projection = CAMERA_PERSPECTIVE; 
    model->camera = camera;
    model->ui_ctx = (mu_Context*)malloc(sizeof(mu_Context));
    return model;
}

sim_model *sim_model_init(
    int fps, 
    int window_width, 
    int window_height, 
    const char *title, 
    int camera_mode,
    data_model *data_model
) {
    sim_model *model = (sim_model*)malloc(sizeof(*model));
    model->base_model = base_sim_model_init(fps, window_width, window_height, title, camera_mode);
    model->data_model = data_model;
    return model;
}
#endif

void sim_loop();


#endif