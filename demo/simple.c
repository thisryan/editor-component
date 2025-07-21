#include <raylib.h>

#include <stdio.h>

#include <string.h>

#define VECTOR_PATH "base/ds/vector.h"
#define EDITOR_IMPLEMENTATION
#include <comp_editor.h>

int text_width(char* text, int index, void* font, int font_size) {
    char *copy = NULL;

    for(int i = 0;i < index;i++){
        vec_add(copy, text[i]);
    }
    vec_add(copy, '\0');
    Font *raylib_font = (Font*)font;
    int width = MeasureTextEx(*raylib_font, copy, font_size, 2).x;

    vec_free(copy);

    return width;
}

bool word_seperator(uint32_t c) {}

line_coloring_t calculate_color(char *text, int size, line_coloring_t old_color) {
    return (line_coloring_t) {.word_colorings = NULL};
}

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_ERROR);

    InitWindow(0,0, "Editor component");

    char *text =  "Das ist ein text\nMit 2 Zeilen";
    if(argc > 1) {
        text = load_file_to_string(argv[1]);
    }

    init_options_t options = {
        .text = text
    };

    Font roboto = GetDefaultFont();

    editor_t editor = create(options, &calculate_color, &word_seperator);

    editor.font = &roboto;
    render_options_t render_options = {
        .area_width = 500,
        .area_height = 500,
        .font_size = 30,
        .line_margin = 5,
        .text_width = &text_width,
        .area_x = 20,
        .line_numbers = true,
        .line_number_margin = 10
    };

    Color back = {.r = 24, .g = 24, .b = 24, .a = 255};

    int cursor = MOUSE_CURSOR_DEFAULT;
    while(!WindowShouldClose()) {
        double delta = GetFrameTime();
        update_timers(&editor, delta);

        BeginDrawing();
        ClearBackground(back);
        int temp_cursor = MOUSE_CURSOR_DEFAULT;

        render_options.area_width = GetScreenWidth();
        render_options.area_height = GetScreenHeight();

        int c;
        while((c = GetCharPressed()) != 0) {
            acursor_insert(&editor, c);
        }

        if(IsKeyPressed(KEY_RIGHT)) {
            cursor_mright(&editor);
        }

        if(IsKeyPressed(KEY_LEFT)) {
            cursor_mword(&editor, -1);
        }

        if(IsKeyPressed(KEY_DOWN)) {
            cursor_mdown(&editor);
        }

        if(IsKeyPressed(KEY_UP)) {
            cursor_mup(&editor);
        }

        if(IsKeyPressed(KEY_BACKSPACE)) {
            acursor_delete(&editor, -1, true);
        }

        if(IsKeyPressed(KEY_DELETE)) {
            acursor_word_delete(&editor,0);
        }

        if(IsKeyPressed(KEY_ENTER)) {
            acursor_newline(&editor, true, 1);
        }

        if(GetMouseWheelMove() == 1) {
            editor.scroll_start = editor.scroll_start == 0 ? 0 : editor.scroll_start-1;
        }

        if(GetMouseWheelMove() == -1) {
            editor.scroll_start = editor.scroll_start == vec_length(editor.lines)-2 ? vec_length(editor.lines)-2 : editor.scroll_start+1;
        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            int out_x, out_y;
            mouse_to_cursor(&editor, &render_options, GetMouseX(), GetMouseY(), &out_x, &out_y);
            if(!(out_x == -1 && out_y == -1)) {
                editor_cursor_mto(&editor, out_x, out_y);
            }
        }

        if(IsKeyPressed(KEY_Y) && IsKeyDown(KEY_LEFT_CONTROL)) {
            rollback(&editor);
        }

        if(mouse_in_rect(GetMouseX(), GetMouseY(), render_options.area_x, render_options.area_y,render_options.area_width,render_options.area_height)) {
            temp_cursor = MOUSE_CURSOR_IBEAM;
        }

        if(IsKeyPressed(KEY_TAB)) {
            if(IsKeyDown(KEY_LEFT_SHIFT)) {
                acursor_unindent(&editor);
            } else {
                acursor_indent(&editor);
            }
        }

        if(temp_cursor != cursor) {
            cursor = temp_cursor;
            SetMouseCursor(cursor);
        }

        bool selecting = IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_LEFT_SHIFT);
        editor_set_select(&editor, selecting);

        render_command_t *render_commands = start_render(&editor, &render_options);

        for(int i = 0;i < vec_length(render_commands);i++){
            const render_command_t *command = &render_commands[i];
            if(command->type == TEXT) {
                const render_text_t *text_command = &command->as.text;
                Font font = *(Font*)text_command->font;
                Vector2 pos = { text_command->x, text_command->y};
                Color c = WHITE;

                if(command->render_hint == HINT_LINE_NUMBER) {
                    c = GRAY;
                }

                DrawTextEx(font, text_command->text,pos, text_command->font_size, 2, c);
            }

            if(command->type == RECT) {
                const render_rect_t *rect_command = &command->as.rect;
                Color c = {.r = 13, .g = 224, .b = 206, .a = 45};

                if(command->render_hint == HINT_CURSOR) {
                    c.a = 255;
                }
                if(command->render_hint == HINT_HIGHLIGHT_LINE) {
                    c = (Color){.r = 255, .g = 255, .b = 255, .a = 25};
                }
                DrawRectangle(rect_command->x, rect_command->y, rect_command->width, rect_command->height, c);
            }
        }

        editor_stop_render(&editor, render_commands);

        EndDrawing();
    }

    editor_cleanup(&editor);
    UnloadFont(roboto);

    CloseWindow();
}