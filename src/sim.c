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
    mu_init(model->base_model->ctx);
    /* try to load a larger custom font from the repo fonts folder; fallback to default */
    Font loaded = LoadFontEx("fonts/Science_Gothic/static/ScienceGothic-Regular.ttf", 32, NULL, 0);
    if (loaded.texture.id != 0) {
        model->base_model->font = loaded;
    } else {
        model->base_model->font = GetFontDefault();
    }

    murl_setup_font_ex(model->base_model->ctx, &model->base_model->font);
}
// ---
// sim data
// ---
struct DataModel {
    int test;
    float time;
    float bg[3];
    float size[3];
    dynarray_t * arr;
};
// ---
// ui declaration
// ---

// static float bg[3] = { 90, 95, 100 };
void sim_ui(sim_model *model) {
    mu_Context *ctx = model->base_model->ctx;
    float *bg = model->data_model->bg;
    float *size = model->data_model->size;
    murl_handle_input(ctx);
    mu_begin(ctx);
    if (mu_begin_window(ctx, "My Window", mu_rect(20, 20, 400, 450))) {
        // mu_Container *win = mu_get_current_container(ctx);
        // win->rect.w = mu_max(win->rect.w, 240);
        // win->rect.h = mu_max(win->rect.h, 300);
        // mu_label(ctx, "Hello, raylib");
        
        if (mu_header_ex(ctx, "Color", MU_OPT_EXPANDED)) {
            mu_layout_row(ctx, 2, (int[]) { -78, -1 }, 74);
            /* sliders */
            mu_layout_begin_column(ctx);
            mu_layout_row(ctx, 2, (int[]) { 100, -1 }, 0);
            mu_label(ctx, "Red:");   mu_slider(ctx, &bg[0], 0, 255);
            mu_label(ctx, "Green:"); mu_slider(ctx, &bg[1], 0, 255);
            mu_label(ctx, "Blue:");  mu_slider(ctx, &bg[2], 0, 255);
            mu_layout_end_column(ctx);
            /* color preview */
            mu_Rect r = mu_layout_next(ctx);
            mu_draw_rect(ctx, r, mu_color(bg[0], bg[1], bg[2], 255));
            char buf[32];
            sprintf(buf, "#%02X%02X%02X", (int) bg[0], (int) bg[1], (int) bg[2]);
            mu_draw_control_text(ctx, buf, r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
        }
        if (mu_header_ex(ctx, "Size", MU_OPT_EXPANDED)) {
            mu_layout_begin_column(ctx);
            mu_layout_row(ctx, 2, (int[]) { 100, -1 }, 0);
            mu_label(ctx, "width:");   mu_slider(ctx, &size[0], 2, 255);
            mu_label(ctx, "height:"); mu_slider(ctx, &size[1], 2, 255);
            mu_label(ctx, "length:");  mu_slider(ctx, &size[2], 2, 255);
            mu_layout_end_column(ctx);
        }
        char label[10];
        int fps_format_success = snprintf(label, 10, "FPS:\t%d\n", GetFPS());
        if (fps_format_success) {
            mu_label(ctx, label);
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
            DrawCube(
                (Vector3){0.0,0.0,0.0},
                model->data_model->size[0], model->data_model->size[1], model->data_model->size[2], 
                (Color){model->data_model->bg[0],model->data_model->bg[1],model->data_model->bg[2],255}
            );
            Vector3 *vec;
            float x = 0;
            arr_foreach(Vector3, vec, model->data_model->arr) {
                DrawCube(*vec, x++, 10, 10, BLACK);
            }
            arr_filter(Vector3, vec, model->data_model->arr, vec->x==10) {
                DrawCube(*vec, 10, x++, 10, RED);
            }
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
        // UpdateCamera(&model->base_model->camera, model->base_model->camera_mode);
        sim_draw(model);
    }
    CloseWindow();
}

void sim_destroy(sim_model *model) {
    arr_free(model->data_model->arr);
    /* unload font if it was loaded from file (GetFontDefault() shares system font texture) */
    if (model->base_model->font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(model->base_model->font);
    }
    free(model->base_model->ctx);
    free(model->base_model);
}
// ---
// main loop
// ---
void sim_loop() {

    Vector3 vec = {10,10,10};

    data_model sim_data = (data_model){
        .test = 10,
        .time = 0.0,
        .bg = { 90, 95, 100 },
        .size = {2.0,2.0,2.0},
        .arr = NULL,
    };
    // Vector3 vec = {10,10,10};
    // arr_push(Vector3, sim_data.arr, &vec);
    sim_data.arr = arr_with(Vector3, 20, &vec);

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