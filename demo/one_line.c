#include <raylib.h>

#include <stdio.h>

#define VECTOR_PATH "base/ds/vector.h"
#define EDITOR_IMPLEMENTATION
#include <comp_editor.h>

Font roboto;
Font roboto_label;

void update_one_line_editor(editor_t *editor) {
    int c;
    while((c = GetCharPressed()) != 0) {
        insert_at_cursor(editor, c);
    }

    if(IsKeyPressed(KEY_RIGHT)) {
        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            jump_word(editor, 0);
        } else {
            move_cursor_right(editor);
        }
    }

    if(IsKeyPressed(KEY_LEFT)) {
        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            jump_word(editor, -1);
        }else{
            move_cursor_left(editor);
        }
    }

    if(IsKeyPressed(KEY_BACKSPACE)) {
        delete_at_cursor(editor);
    }

    if(IsKeyPressed(KEY_LEFT_SHIFT)) {
        start_select(editor);
    }

    if(IsKeyReleased(KEY_LEFT_SHIFT)) {
        stop_select(editor);
    }

    if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
        const char *text = get_selection(editor, 0);
        SetClipboardText(text);
    }

    if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
        const char *text = GetClipboardText();
        insert_block_at_cursor(editor, text, true, true);
    }

    if(IsKeyPressed(KEY_END)) {
        jump_end_of_line(editor);
    }

     if(IsKeyPressed(KEY_HOME)) {
        jump_start_of_line(editor);
    }
}

void render_editor(editor_t *editor, render_options_t *options, bool active) {
    render_command_t *render_commands = start_render(editor, options);

    vec_for_each_ptr(const render_command_t *command, render_commands) {
        if(command->type == TEXT) {
            const render_text_t *text_command = &command->as.text;
            Font font = *(Font*)text_command->font;
            Vector2 pos = { text_command->x, text_command->y};
            DrawTextEx(font, text_command->text,pos, text_command->font_size, 2, BLACK);
        }

        if(command->type == RECT) {
            const render_rect_t *rect_command = &command->as.rect;
            // Color c = {.r = rect_command->color[0], .g = rect_command->color[1], .b = rect_command->color[2], .a = rect_command->color[3]};
            Color c = {.r = 13, .g = 224, .b = 206, .a = 100};
            if(command->render_hint == HINT_CURSOR) {
                c.a = active ? 255 : 0;
            }
            DrawRectangle(rect_command->x, rect_command->y, rect_command->width, rect_command->height, c);
        }
    }

    editor_stop_render(editor, render_commands);
}

void render_one_line_editor(editor_t *editor, render_options_t *options, char* label, bool active) {
    render_editor(editor, options, active);

    DrawRectangle(options->area_x, options->area_y + options->font_size + 2, options->area_width, 3, BLACK);
    DrawTextEx(roboto_label, label, (Vector2){options->area_x, options->area_y-15}, 15 ,2, BLACK);
}

void update_textarea_editor(editor_t *editor) {
    int c;
    while((c = GetCharPressed()) != 0) {
        insert_at_cursor(editor, c);
    }

    if(IsKeyPressed(KEY_RIGHT)) {
        if(IsKeyDown(KEY_LEFT_CONTROL)){
            jump_word(editor, 0);
        }else {
            move_cursor_right(editor);
        }
    }

    if(IsKeyPressed(KEY_LEFT)) {
        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            jump_word(editor, -1);
        }else{
            move_cursor_left(editor);
        }
    }

    if(IsKeyPressed(KEY_DOWN)) {
        if(IsKeyDown(KEY_LEFT_ALT)) {
            move_line_at_cursor(editor, 1);
        } else {
            move_cursor_down(editor);
        }
    }

    if(IsKeyPressed(KEY_UP)) {
        if(IsKeyDown(KEY_LEFT_ALT)) {
            move_line_at_cursor(editor, -1);
        } else {
            move_cursor_up(editor);
        }
    }

    if(IsKeyPressed(KEY_BACKSPACE)) {
        delete_at_cursor(editor);
    }

    if(IsKeyPressed(KEY_ENTER)) {
        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            if(IsKeyDown(KEY_LEFT_SHIFT)) {
                newline_at_cursor(editor, false, 0);
            } else {
                newline_at_cursor(editor, false, 1);
            }
        } else {
            newline_at_cursor(editor, true, 1);
        }
    }

    if(IsKeyPressed(KEY_LEFT_SHIFT)) {
        start_select(editor);
    }

    if(IsKeyReleased(KEY_LEFT_SHIFT)) {
        stop_select(editor);
    }

    if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
        const char *text = GetClipboardText();
        insert_block_at_cursor(editor, text, false, true);
    }

    if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
        const char *text = get_selection(editor, 0);
        SetClipboardText(text);
    }

    if(IsKeyPressed(KEY_END)) {
        jump_end_of_line(editor);
    }

    if(IsKeyPressed(KEY_HOME)) {
        jump_start_of_line(editor);
    }

    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
        dublicate_line_at_cursor(editor, 1);
    }
    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
        dublicate_line_at_cursor(editor, 0);
    }
    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_K)) {
        delete_line_at_cursor(editor);
    }
}

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

bool word_seperator(uint32_t c) {
    return c == ' ';
}

int main(int argc, char **argv) {
    InitWindow(600,800, "Editor component");

    init_options_t options = {
        .text = ""
    };

    roboto = LoadFontEx("res/FiraCode-Regular.ttf", 30, NULL, 0);
    roboto_label = LoadFontEx("res/FiraCode-Regular.ttf", 15, NULL, 0);

    editor_t editor_name = create(options);
    editor_name.font = &roboto;
    editor_name.word_seperator = &word_seperator;
    render_options_t render_options = {
        .area_width = 400,
        .area_height = 40,
        .area_x = 100,
        .area_y = 100,
        .font_size = 30,
        .text_width = &text_width,
    };

    editor_t editor_email = create(options);
    editor_email.font = &roboto;
    editor_email.word_seperator = &word_seperator;
    render_options_t render_options_email = {
        .area_width = 400,
        .area_height = 40,
        .area_x = 100,
        .area_y = 200,
        .font_size = 30,
        .text_width = &text_width,
    };

    editor_t editor_message = create(options);
    editor_message.font = &roboto;
    editor_message.word_seperator = &word_seperator;
    render_options_t render_options_message = {
        .area_width = 400,
        .area_height = 150,
        .area_x = 100,
        .area_y = 300,
        .font_size = 30,
        .text_width = &text_width,
        .line_margin = 0
    };


    int selected_editor = 0;

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        if(IsKeyPressed(KEY_TAB)) {
            selected_editor = (selected_editor + 1) % 3;
        }

        if(selected_editor == 0){
            update_one_line_editor(&editor_name);
        } else if(selected_editor == 1) {
            update_one_line_editor(&editor_email);
        } else {
            update_textarea_editor(&editor_message);
        }

        render_one_line_editor(&editor_name, &render_options, "Name", selected_editor == 0);
        render_one_line_editor(&editor_email, &render_options_email, "Email", selected_editor == 1);
        render_editor(&editor_message, &render_options_message, selected_editor == 2);
        char* label = "Message";
        DrawTextEx(roboto_label, label, (Vector2){render_options_message.area_x, render_options_message.area_y-15}, 15 ,2, BLACK);
        Rectangle rec = {render_options_message.area_x-1, render_options_message.area_y-1, render_options_message.area_width+2, render_options_message.area_height+2};
        DrawRectangleLinesEx(rec, 1, BLACK);

        EndDrawing();
    }
}
