#include "sim.h"
#include "../lib/dynarray/dynarray.h"
#include "ui_renderer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

base_sim_model *base_sim_model_init(
    int fps,
    int window_width,
    int window_height,
    const char *title,
    int camera_mode
) {
    base_sim_model *model = malloc(sizeof(*model));
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
    model->ctx = malloc(sizeof(mu_Context));
    mu_init(model->ctx);
    murl_setup_font(model->ctx);
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
    sim_model *model = malloc(sizeof(*model));
    model->base_model = base_sim_model_init(fps, window_width, window_height, title, camera_mode);
    model->data_model = data_model;
    return model;
}

void sim_init(sim_model *model, bool cursor) {
    InitWindow(model->base_model->window_width, model->base_model->window_height, model->base_model->title);
    
    SetTargetFPS(model->base_model->fps);

    if (!cursor) {
        DisableCursor(); 
    }
}
// ---
// ui declaration
// ---
void sim_ui(sim_model *model) {
    mu_Context *ctx = model->base_model->ctx;
    murl_handle_input(ctx);
    mu_begin(ctx);
    if (mu_begin_window(ctx, "My Window", mu_rect(20, 20, 200, 150))) {
        /* process ui here... */
        mu_label(ctx, "Hello, raylib");
        
        char label[20];
        int j = snprintf(label, 20, "FPS:\t%d\n", GetFPS());
        mu_label(ctx, label);

        if (mu_button(ctx, "The button")) {
            mu_open_popup(ctx, "popup");
        }

        if (mu_begin_popup(ctx, "popup")) {
            mu_label(ctx, "This is a popup");
            mu_end_popup(ctx);
        }
        
        mu_end_window(ctx);
    }
    mu_end(ctx);
}
// ---
// draw loop
// ---
void sim_draw(sim_model *model) {
    BeginDrawing();
        ClearBackground(WHITE);
        BeginMode3D(model->base_model->camera);
            DrawGrid(100, 10.0);
        
        EndMode3D();
        murl_render(model->base_model->ctx);
    EndDrawing();
}
// ---
// update loop
// ---
void sim_update(sim_model *model) {
    // data_model *data = model->data_model;
    // // printf("test:\t%d\n",data.test);
    // data->time += GetFrameTime();
    // if (data->time > 1) {
    //     printf("test:\t%d\n",data->test);
    //     data->test += 10;
    //     data->time = 0;
    // }
}

void sim_window(sim_model *model) {
    while (!WindowShouldClose()) {
        sim_update(model);
        sim_ui(model);
        UpdateCamera(&model->base_model->camera, model->base_model->camera_mode);
        sim_draw(model);
    }
    CloseWindow();
}

void sim_destroy(sim_model *model) {
    free(model->base_model->ctx);
    free(model->base_model);
}
// ---
// main loop
// ---
void sim_loop() {
    data_model sim_data = (data_model){
        .test = 10,
        .time = 0.0,
    };

    sim_model *model = sim_model_init(
        1000, 
        1920, 
        1080, 
        "General sim template", 
        CAMERA_ORTHOGRAPHIC,
        &sim_data
    );

    sim_init(model, true);           

    sim_window(model);

    sim_destroy(model);
}