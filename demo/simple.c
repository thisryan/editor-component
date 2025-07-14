#include <raylib.h>

#include <stdio.h>

#include <string.h>

#define VECTOR_PATH "base/ds/vector.h"
#define EDITOR_IMPLEMENTATION
#include <comp_editor.h>

typedef enum {
    CUSRTOM_HINT_START = HINT_END,

    CUSTOM_HINT_KEYWORD,
    CUSTOM_HINT_HEADER,
    CUSTOM_HINT_LIST,
    CUSTOM_HINT_EMPHASIZE,
    CUSTOM_HINT_STAR,
    CUSTOM_HINT_UNDERLINE,
    CUSTOM_HINT_INLINE_CODE
} custom_render_hints_t;

char *load_file_to_string(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    // Seek to end to get file size
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(fp);
        return NULL;
    }

    long length = ftell(fp);
    if (length < 0) {
        perror("ftell");
        fclose(fp);
        return NULL;
    }

    rewind(fp); // Go back to start

    // Allocate buffer (+1 for null terminator)
    char *buffer = malloc(length + 1);
    if (!buffer) {
        perror("malloc");
        fclose(fp);
        return NULL;
    }

    // Read file into buffer
    size_t read = fread(buffer, 1, length, fp);
    if (read != (size_t)length) {
        perror("fread");
        free(buffer);
        fclose(fp);
        return NULL;
    }

    buffer[length] = '\0'; // Null terminate

    fclose(fp);
    return buffer;
}


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


bool word_seperator(uint32_t c) {
    return c == ' ' || c == '\n';
}

line_coloring_t calculate_color(char *text, int size, line_coloring_t old_color) {
    char *comp = "int";
    if(text == NULL || strlen(comp) > size) {
        return (line_coloring_t){0};
    }
    word_coloring_t *word_colorings = NULL;


    for(int i = 0;i <= size - strlen(comp);i++) {
        if(strncmp(comp, text + i, strlen(comp)) == 0) {
            word_coloring_t word = {.start = i, .end = i + strlen(comp), .rendering_hint = CUSTOM_HINT_KEYWORD};
            vec_add(word_colorings, word);
        }
    }

    return (line_coloring_t) {.word_colorings = word_colorings};
}

bool is_header(char*text, int size) {
    int index = 0;
    while(index < size && text[index] == ' ') {
        index++;
    }

    if(index >= size) {
        return false;
    }

    if(text[index] == '#') {
        while(index < size && text[index] == '#') {
            index++;
        }
        if(text[index] == ' ') {
            return true;
        }
    }

    return false;
}

bool check_for_list(char *text, int size, int *position) {
    int index = 0;
    while(index < size && text[index] == ' ') {
        index++;
    }

    if(index >= size) {
        return false;
    }

    if(text[index] == '-') {
        if(index < size-1 && text[index+1] == ' ') {
            *position = index;
            return true;
        }
    }

    return false;
}

bool check_for_surrounded(char* text, int size, char symbol, int *end_position){
    int index = 0;
    // printf("Chelc %c \n", symbol);
    if(text[index] == symbol) {
        // printf("start equal %c \n", symbol);
        index++;
        if(text[index] != ' ') {
            // printf("nOT space %c \n", symbol);
            while(index < size && text[index] != symbol) {
                index++;
            }
            *end_position = index+1;
            return true;
        }
    }
    return false;
}

line_coloring_t markdown_coloring(char *text, int size, line_coloring_t old_color) {
    if(is_header(text, size)) {
        word_coloring_t wc = {.start = 0, .end = size, .rendering_hint = CUSTOM_HINT_HEADER};
        word_coloring_t *lc = NULL;
        vec_add(lc, wc);
        return (line_coloring_t){.word_colorings = lc};
    }

    word_coloring_t *lc = NULL;
    int index = 0;

    int list_position;
    if(check_for_list(text, size, &list_position)) {
        word_coloring_t wc = {.start = list_position, .end= list_position+1, .rendering_hint = CUSTOM_HINT_LIST};
        vec_add(lc, wc);
        text = text + list_position;
        index = list_position;
    }

    for(int i = index;i < size;i++) {
        if(text[i] == '*') {
            int offset;
            if(check_for_surrounded(text + i, size - i, '*', &offset)) {
                word_coloring_t wc = {.start = i, .end = i + offset, .rendering_hint = CUSTOM_HINT_EMPHASIZE};
                vec_add(lc, wc);
                i+=offset;
            }
        }
        if(text[i] == '_'){
            int offset;
            if(check_for_surrounded(text + i, size - i, '_', &offset)) {
                word_coloring_t wc = {.start = i, .end = i + offset, .rendering_hint = CUSTOM_HINT_UNDERLINE};
                vec_add(lc, wc);
                i+=offset;
            }
        }
        if(text[i] == '`'){
            int offset;
            if(check_for_surrounded(text + i, size - i, '`', &offset)) {
                word_coloring_t wc = {.start = i, .end = i + offset, .rendering_hint = CUSTOM_HINT_INLINE_CODE};
                vec_add(lc, wc);
                i+=offset;
            }
        }
    }

    return (line_coloring_t){lc};
}

bool mouse_in_rect(int mx, int my, int rx, int ry, int rw, int rh) {
    return mx >= rx && mx <= rx + rw && my >= ry && my <= ry + rh;
}

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(0,0, "Editor component");

    Image icon = LoadImage("res/frying-pan.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    char *text =  "Das ist ein text\nMit 2 Zeilen";
    if(argc > 1) {
        text = load_file_to_string(argv[1]);
    }

    init_options_t options = {
        .text = text
    };

    Font roboto = LoadFontEx("res/FiraCode-Regular.ttf", 30, NULL, 0);

    editor_t editor = create(options, markdown_coloring);
    editor.font = &roboto;
    editor.word_seperator = &word_seperator;
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
            insert_at_cursor(&editor, c);
        }
        
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            stop_select(&editor);
        }

        if(IsKeyPressed(KEY_RIGHT)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)){
                jump_word(&editor, 0);
            }else {
                move_cursor_right(&editor);
            }
        }

        if(IsKeyPressed(KEY_LEFT)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                jump_word(&editor, -1);
            }else{
                move_cursor_left(&editor);
            }
        }

        if(IsKeyPressed(KEY_DOWN)) {
            if(IsKeyDown(KEY_LEFT_ALT)) {
                move_line_at_cursor(&editor, 1);
            } else {
                move_cursor_down(&editor);
            }
        }

        if(IsKeyPressed(KEY_UP)) {
            if(IsKeyDown(KEY_LEFT_ALT)) {
                move_line_at_cursor(&editor, -1);
            } else {
                move_cursor_up(&editor);
            }
        }

        if(IsKeyPressed(KEY_BACKSPACE)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                delete_word_at_cursor(&editor,-1);
            }else {
                delete_at_cursor(&editor, -1);
            }
        }

        if(IsKeyPressed(KEY_DELETE)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                delete_word_at_cursor(&editor,0);
            }else {
                delete_at_cursor(&editor, 0);
            }
        }

        if(IsKeyPressed(KEY_ENTER)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                if(IsKeyDown(KEY_LEFT_SHIFT)) {
                    newline_at_cursor(&editor, false, 0);
                } else {
                    newline_at_cursor(&editor, false, 1);
                }
            } else {
                newline_at_cursor(&editor, true, 1);
            }
        }

        if(IsKeyPressed(KEY_A) && IsKeyDown(KEY_LEFT_CONTROL)) {
            select_all(&editor);
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_RIGHT_BRACKET)) {
            render_options.font_size *= 1.2;
            UnloadFont(roboto);
            render_options.line_margin = render_options.font_size/6;
            roboto = LoadFontEx("res/FiraCode-Regular.ttf", render_options.font_size, NULL, 0);
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_SLASH)) {
            render_options.font_size /= 1.2;
            UnloadFont(roboto);
            render_options.line_margin = render_options.font_size/6;
            roboto = LoadFontEx("res/FiraCode-Regular.ttf", render_options.font_size, NULL, 0);
        }

        if(IsKeyPressed(KEY_LEFT_SHIFT)) {
            start_select(&editor);
        }

        if(IsKeyReleased(KEY_LEFT_SHIFT)) {
            stop_select(&editor);
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
            const char *text = GetClipboardText();
            insert_block_at_cursor(&editor, text, false, true);
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
            const char *text = get_selection(&editor, 0);
            SetClipboardText(text);
        }

        if(IsKeyPressed(KEY_END)) {
            jump_end_of_line(&editor);
        }

        if(IsKeyPressed(KEY_HOME)) {
            jump_start_of_line(&editor);
        }

        if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) {
            dublicate_line_at_cursor(&editor, 1);
        }
        if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) {
            dublicate_line_at_cursor(&editor, 0);
        }
        if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_K)) {
            delete_line_at_cursor(&editor);
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
                jump_cursor_to(&editor, out_x, out_y);
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
                unindent(&editor);
            } else {
                indent(&editor);
            }
        }

        if(temp_cursor != cursor) {
            cursor = temp_cursor;
            SetMouseCursor(cursor);
        }
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            start_select(&editor);
        }

        render_command_t *render_commands = start_render(&editor, &render_options);

        vec_for_each_ptr(const render_command_t *command, render_commands) {
            if(command->type == TEXT) {
                const render_text_t *text_command = &command->as.text;
                Font font = *(Font*)text_command->font;
                Vector2 pos = { text_command->x, text_command->y};
                Color c = WHITE;
                if(command->render_hint == HINT_LINE_NUMBER) {
                    c = GRAY;
                }
                if(command->render_hint == CUSTOM_HINT_KEYWORD) {
                    c = RED;
                }
                if(command->render_hint == CUSTOM_HINT_HEADER) {
                    c = GREEN;
                }
                if(command->render_hint == CUSTOM_HINT_LIST) {
                    c = ORANGE;
                }
                if(command->render_hint == CUSTOM_HINT_EMPHASIZE) {
                    c = RED;
                }
                if(command->render_hint == CUSTOM_HINT_UNDERLINE) {
                    c = BLUE;
                }
                if(command->render_hint == CUSTOM_HINT_INLINE_CODE) {
                    c = BEIGE;
                }
                DrawTextEx(font, text_command->text,pos, text_command->font_size, 2, c);
            }

            if(command->type == RECT) {
                const render_rect_t *rect_command = &command->as.rect;
                // Color c = {.r = rect_command->color[0], .g = rect_command->color[1], .b = rect_command->color[2], .a = rect_command->color[3]};
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


