#define SIMULATION_IMPLEMENTATION
#include "sim.h"
#define DYNARRAY_IMPLEMENTATION
#include "../lib/dynarray/dynarray.h"
#include "../lib/raylib/raylib.h"
#include "../lib/raylib/raymath.h"
#include "../lib/microui_renderer/ui_renderer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dataset.h"

void sim_init(sim_model *model, bool cursor) {
    InitWindow(model->base_model->window_width, model->base_model->window_height, model->base_model->title);
    
    SetTargetFPS(model->base_model->fps);

    if (!cursor) {
        DisableCursor(); 
    }

    mu_init(model->base_model->ui_ctx);
    /* try to load a larger custom font from the repo fonts folder; fallback to default */
    Font loaded = LoadFontEx("fonts/Science_Gothic/static/ScienceGothic-Regular.ttf", 32, NULL, 0);
    if (loaded.texture.id != 0) {
        model->base_model->font = loaded;
    } else {
        model->base_model->font = GetFontDefault();
    }

    murl_setup_font_ex(model->base_model->ui_ctx, &model->base_model->font);
    model->base_model->ui_ctx->style->size.x = 300;
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
    mu_Context *ctx = model->base_model->ui_ctx;
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
int draw_filter_condition(Vector3 * item) {
    return item->y<20;
}

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
            __attribute__((unused))
            Vector3 *vec = NULL;
            arr_foreach(Vector3, vec, model->data_model->arr) {
                DrawCube(*vec, 10, 10, 10, BLACK);
            }
            arr_filter_each(Vector3, vec, model->data_model->arr, draw_filter_condition) {
                DrawCube(*vec, 10, 10, 10, RED);
            }
        EndMode3D();
        murl_render(model->base_model->ui_ctx);
    EndDrawing();
}
// ---
// update loop
// ---
void sim_update(sim_model *model) {
    data_model *data = model->data_model;
    __attribute__((unused))
    Vector3 *vec = NULL;
    arr_foreach(Vector3, vec, data->arr) {
        *vec = Vector3Add(*vec, Vector3Scale((Vector3){0,1,0},GetFrameTime()));
    }
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
    free(model->base_model->ui_ctx);
    free(model->base_model);
}
// ---
// main loop
// ---
void sim_loop() {
    data_model sim_data = (data_model){
        .test = 10,
        .time = 0.0,
        .bg = { 90, 95, 100 },
        .size = {2.0,2.0,2.0},
        .arr = NULL,
    };
    // Vector3 vec = {10,10,10};
    // arr_push(Vector3, sim_data.arr, &vec);
    Vector3 vec = {10,10,10};
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