#include <raylib.h>

#include <stdio.h>

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
    // int width = MeasureText(copy, font_size);

    vec_free(copy);

    return width;
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(0,0, "Editor component");
    init_options_t options = {
        .text = "Das ist ein text\nMit 2 Zeilen",
    };

    Font roboto = LoadFontEx("res/Roboto-Black.ttf", 30, NULL, 0);

    editor_t editor = create(options);
    editor.font = &roboto;
    render_options_t render_options = {
        .area_height = 500,
        .area_width = 500,
        .font_size = 30,
        .line_margin = 5,
        .text_width = &text_width,
        .area_x = 20,
        .line_numbers = true,
        .line_number_margin = 10
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

        if(IsKeyPressed(KEY_BACKSPACE)) {
            delete_at_cursor(&editor);
        }

        if(IsKeyPressed(KEY_ENTER)) {
            newline_at_cursor(&editor);
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
            render_options.font_size *= 1.2;
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_B)) {
            render_options.font_size /= 1.2;
        }

        render_command_t *render_commands = start_render(&editor, &render_options);

        vec_for_each_ptr(const render_command_t *command, render_commands) {
            if(command->type == TEXT) {
                const render_text_t *text_command = &command->as.text;
                Font font = *(Font*)text_command->font;
                Vector2 pos = { text_command->x, text_command->y};
                DrawTextEx(font, text_command->text,pos, text_command->font_size, 2, BLACK);
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


