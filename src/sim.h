#ifndef SIM_H
#define SIM_H

#include "../lib/raylib/raylib.h"
#include "../lib/raylib/rlgl.h"
#include "../lib/microui/microui.h"

typedef struct Base_SIM_Model {
    int fps;
    int window_width;
    int window_height;
    const char *title;
    Camera camera;
    int camera_mode;
    mu_Context *ctx;
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
void sim_loop();


#endif