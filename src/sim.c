#define ARENA_IMPLEMENTATION
#include "../lib/arena/arena.h"
#define CUSTOM_REALLOC(allocator,pointer,size) arena_realloc(allocator,pointer,size)
#define CUSTOM_FREE(allocator,pointer) arena_free(pointer)

#define DA_ST_CUSTOM_ALLOC
#define DA_ST_REALLOC   CUSTOM_REALLOC
#define DA_ST_FREE      CUSTOM_FREE

#define DA_ARR_CUSTOM_ALLOC
#define DA_REALLOC      CUSTOM_REALLOC
#define DA_FREE         CUSTOM_FREE

#define SIMULATION_IMPLEMENTATION
// #define SIM_MODE_2D
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
// #include <time.h>
// #include <stdlib.h>

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
void sim_draw(sim_model *model) {
    BeginDrawing();
        ClearBackground(WHITE);
        DrawingModeScope(model->base_model->camera,{
            DrawGrid(100, 10.0);
            DrawCube(
                (Vector3){0.0,0.0,0.0},
                model->data_model->size[0], model->data_model->size[1], model->data_model->size[2], 
                (Color){model->data_model->bg[0],model->data_model->bg[1],model->data_model->bg[2],255}
            );
            arr_map(Vector3, model->data_model->arr, vec, {
                DrawCube(*vec, 2, 2, 2, BLACK);
            });
            arr_filter_each(Vector3, model->data_model->arr, vec, (vec->y<20), {
                DrawCube(*vec, 2, 2, 2, RED);
            });
        });
        murl_render(model->base_model->ui_ctx);
    EndDrawing();
}
// ---
// update loop
// ---
void sim_update(sim_model *model) {
    data_model *data = model->data_model;
    float cond_val = 15;
    float x = -1;
    float z = 1;
    arr_filter_each(Vector3, data->arr, vec, (vec->y<cond_val), {
        *vec = Vector3Add(*vec, Vector3Scale((Vector3){x,1,z},GetFrameTime()));
        x += 2./(float)arr_len(data->arr);
        z -= 2./(float)arr_len(data->arr);
    });
    x=1;
    z=-1;
    arr_filter_each(Vector3, data->arr, vec, (vec->y>cond_val), {
        *vec = Vector3Add(*vec, Vector3Scale((Vector3){x,1,z},GetFrameTime()));
        x -= 2./(float)arr_len(data->arr);
        z += 2./(float)arr_len(data->arr);
    });
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
    arena_uninit(model->base_model->allocator);
    /* unload font if it was loaded from file (GetFontDefault() shares system font texture) */
    if (model->base_model->font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(model->base_model->font);
    }
}
// ---
// main loop
// ---
void sim_loop() {
    // srand(time(NULL));
    arena_t * core_arena = arena_init(40e8);

    data_model sim_data = (data_model){
        .test = 10,
        .time = 0.0,
        .bg = { 90, 95, 100 },
        .size = {2.0,2.0,2.0},
        .arr = arr_with(Vector3, core_arena, 20, &((Vector3){10,10,10})),
    };
    // Vector3 vec = {10,10,10};
    // arr_push(Vector3, sim_data.arr, &vec);

    sim_model *model = sim_model_init(
        5000, 
        1920, 
        1080, 
        "General sim template", 
        CAMERA_ORTHOGRAPHIC,
        core_arena,
        &sim_data
    );

    simulate(model, true);           
}