#include <raylib.h>

#include <stdio.h>

#define EDITOR_IMPLEMENTATION
#include <comp_editor.h>

int text_width(char* text, int index, int font_size) {
    char *copy = NULL;

    for(int i = 0;i < index;i++){
        vec_add(copy, text[i]);
    }
    vec_add(copy, '\0');

    int width = MeasureText(copy, font_size);

    vec_free(copy);

    return width;
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(0,0, "Editor component");

    init_options_t options = {
        .text = "Das ist ein text\nMit 2 Zeilen",
    };
    editor_t editor = create(options);
    render_options_t render_options = {
        .area_height = 500,
        .font_size = 30,
        .line_margin = 5,
        .text_width = &text_width,
    };

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        int c;
        while((c = GetCharPressed()) != 0) {
            insert_at_cursor(&editor, c);
        }

        if(IsKeyPressed(KEY_RIGHT)) {
            move_cursor_right(&editor);
        }

        if(IsKeyPressed(KEY_LEFT)) {
            move_cursor_left(&editor);
        }

        if(IsKeyPressed(KEY_DOWN)) {
            move_cursor_down(&editor);
        }

        if(IsKeyPressed(KEY_UP)) {
            move_cursor_up(&editor);
        }

        render_command_t *render_commands = start_render(&editor, &render_options);

        vec_for_each_ptr(const render_command_t *command, render_commands) {
            if(command->type == TEXT) {
                const render_text_t *text_command = &command->as.text;
                DrawText(text_command->text, text_command->x, text_command->y, 30, BLACK);
            }

            if(command->type == RECT) {
                const render_rect_t *rect_command = &command->as.rect;
                Color c = {.r = rect_command->color[0], .g = rect_command->color[1], .b = rect_command->color[2], .a = rect_command->color[3]};
                DrawRectangle(rect_command->x, rect_command->y, rect_command->width, rect_command->height, c);
            }
        }

        editor_stop_render(&editor, render_commands);

        EndDrawing();
    }

    CloseWindow();
}
