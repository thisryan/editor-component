#ifndef COMP_EDITOR_HEADER_
#define COMP_EDITOR_HEADER_

#ifndef NAMESPACE
#define NAMESPACE
#endif

#ifndef TYPENAME
#define TYPENAME editor_t
#endif

#define CONCAT(a,b) a##b
#define EXPAND_AND_CONCAT(a,b) CONCAT(a,b)
#define NAME(name) EXPAND_AND_CONCAT(NAMESPACE, name)

#include <stdlib.h>
#include <stdbool.h>

#ifndef cursor_t
#define cursor_t NAME(cursor_t)
#endif
typedef struct {
    int x_start,y_start;
    int x_end, y_end;
} cursor_t;

#ifndef line_buffer_t
#define line_buffer_t NAME(line_buffer_t)
#endif
typedef struct {
    char* text;
} line_buffer_t;

#ifndef editor_t
#define editor_t TYPENAME
#endif
typedef struct {
    line_buffer_t *lines;
    cursor_t *cursors;
    char** printable_lines;
    void *font;
} editor_t;

#ifndef render_type_t
#define render_type_t NAME(render_type_t)
#endif
typedef enum {
    TEXT,
    RECT,
} render_type_t;

#ifndef render_text_t
#define render_text_t NAME(render_text_t)
#endif
typedef struct {
    int x, y;
    char* text;
    void* font;
    int font_size;
} render_text_t;

#ifndef render_rect_t
#define render_rect_t NAME(render_rect_t)
#endif
typedef struct {
    int x, y;
    int width,  height;
    int color[4];
} render_rect_t;

#ifndef render_data_t
#define render_data_t NAME(render_data_t)
#endif
typedef union {
    render_text_t text;
    render_rect_t rect;
} render_data_t;

#ifndef render_command_t
#define render_command_t NAME(render_command_t)
#endif
typedef struct {
    render_type_t type;
    render_data_t as;
} render_command_t;

#ifndef init_options_t
#define init_options_t NAME(init_options_t)
#endif
typedef struct {
    char* text;
} init_options_t;

#ifndef render_options_t
#define render_options_t NAME(render_options_t)
#endif
typedef struct {
    int area_x, area_y;
    int area_width, area_height;
    int font_size;
    int line_margin;
    int (*text_width)(char* text, int index, void* font, int font_size);
    bool line_numbers;
    int line_number_margin;
} render_options_t;

#ifndef editor_create
#define editor_create NAME(create)
#endif
editor_t editor_create(init_options_t options);

#ifndef editor_insert_at_cursor
#define editor_insert_at_cursor NAME(insert_at_cursor)
#endif
void editor_insert_at_cursor(editor_t *editor, int c);

#ifndef editor_delete_at_cursor
#define editor_delete_at_cursor NAME(delete_at_cursor)
#endif
void editor_delete_at_cursor(editor_t *editor);

#ifndef editor_newline_at_cursor
#define editor_newline_at_cursor NAME(newline_at_cursor)
#endif
void editor_newline_at_cursor(editor_t *editor);

#ifndef editor_start_render
#define editor_start_render NAME(start_render)
#endif
render_command_t* editor_start_render(editor_t *editor, const render_options_t* options);

#ifdef EDITOR_IMPLEMENTATION

#ifndef VECTOR_PATH
#define VECTOR_PATH "base/ds/vector.h"
#endif
#include VECTOR_PATH

#include <string.h>

char** edutil_split_lines(char* text) {
    if(text == NULL) return NULL;

    size_t len = strlen(text);

    char **lines = NULL;
    char *cur_line = NULL;
    for(int i = 0;i < len;i++) {
        if(text[i] == '\n') {
            vec_add(lines, cur_line);
            cur_line = NULL;
            continue;
        }

        vec_add(cur_line, text[i]);
    }

    vec_add(lines, cur_line);

    return lines;
}

void edutil_insert_in_line(line_buffer_t *line, int index, int c) {
    vec_insert(line->text, index, (char)c);
}

void edutil_remove_in_line(line_buffer_t *line, int index) {
    vec_remove(line->text, index);
}

void edutil_move_cursor_right(editor_t *editor, cursor_t *cursor) {
    if(cursor->y_start >= vec_length(editor->lines)-1 && cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) return;

    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = 0;
        cursor->y_start++;
        return;
    }

    cursor->x_start++;
}

void edutil_move_cursor_left(editor_t *editor, cursor_t *cursor) {
    if(cursor->x_start <= 0 && cursor->y_start <= 0) return;

    if(cursor->x_start <= 0) {
        cursor->y_start--;
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
        return;
    }

    cursor->x_start--;
}

void edutil_move_cursor_down(editor_t *editor, cursor_t *cursor) {
    if(cursor->y_start >= vec_length(editor->lines)-1) return;

    cursor->y_start++;
    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    }
}

void edutil_move_cursor_up(editor_t *editor, cursor_t *cursor) {
    if(cursor->y_start <= 0) return;

    cursor->y_start--;
    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    }
}

void edutil_concat_lines(line_buffer_t *line1, line_buffer_t *line2) {
    vec_for_each_cpy(char c, line2->text) {
        vec_add(line1->text, c);
    }
}

line_buffer_t edutil_split_line_at(line_buffer_t *line1, int index) {
    char *text = NULL;
    size_t length = vec_length(line1->text);
    for(int j = index;j < length;j++) {
        vec_add(text, line1->text[index]);
        vec_remove(line1->text, index);
    }

    return (line_buffer_t) { text };
}

void move_cursor_right(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_right(editor, cursor);
    }
}

void move_cursor_left(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_left(editor, cursor);
    }
}

void move_cursor_down(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_down(editor, cursor);
    }
}

void move_cursor_up(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_up(editor, cursor);
    }
}

editor_t editor_create(init_options_t options) {
    editor_t editor = {0};

    char **lines = edutil_split_lines(options.text);

    line_buffer_t *line_buffers = NULL;

    vec_for_each_cpy(char *line, lines) {
        line_buffer_t buffer = {.text = line};
        vec_add(line_buffers, buffer);
    }

    editor.lines = line_buffers;

    cursor_t cursor = {0};
    vec_add(editor.cursors, cursor);
    return editor;
}

void editor_insert_at_cursor(editor_t *editor, int c) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        line_buffer_t *line = &editor->lines[cursor->y_start];
        edutil_insert_in_line(line, cursor->x_start, c);
        edutil_move_cursor_right(editor, cursor);
    }
}

void editor_delete_at_cursor(editor_t *editor) {
    int *delete_lines = NULL;
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        if(cursor->x_start == 0 && cursor->y_start == 0) continue;
        line_buffer_t *line = &editor->lines[cursor->y_start];
        line_buffer_t *prev_line = &editor->lines[cursor->y_start-1];

        if(cursor->x_start == 0) {
            vec_add(delete_lines, cursor->y_start);
            edutil_move_cursor_left(editor,cursor);
            edutil_concat_lines(prev_line, line);
        } else {
            edutil_remove_in_line(line, cursor->x_start-1);
            edutil_move_cursor_left(editor,cursor);
        }
    }


    for(int j = vec_length(delete_lines)-1;j >= 0;j--){
        vec_remove(editor->lines, delete_lines[j]);
    }
}

void editor_newline_at_cursor(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];
    line_buffer_t newline = edutil_split_line_at(&editor->lines[cursor->y_start], cursor->x_start);

    vec_insert(editor->lines, cursor->y_start+1, newline);
    edutil_move_cursor_right(editor, cursor);
}

render_command_t* editor_start_render(editor_t *editor, const render_options_t* options) {
    render_command_t *render_commands = {0};
    int y = options->area_y;
    int line_number_width = 0;

    if(options->line_numbers) {
        int x = options->area_x;
        for(int i = 0;i < vec_length(editor->lines);i++) {
            char *line_number = NULL;
            vec_init_size(line_number, 21);
            snprintf(line_number, 21, "%d", i+1);

            vec_add(render_commands, ((render_command_t){.type = TEXT, .as.text = {.x = x, .y = y, .text = line_number, .font = editor->font, .font_size = options->font_size}}));

            int width = options->text_width(line_number, strlen(line_number), editor->font, options->font_size);

            if(width > line_number_width) {
                line_number_width = width;
            }

            y += options->font_size + options->line_margin;
            if(y > options->area_height) {
                break;
            }
        }
    }

    y = options->area_y;
    for(int index = 0;index < vec_length(editor->lines);index++) {
        line_buffer_t line_buffer = editor->lines[index];

        int x = options->area_x;
        if(options->line_numbers) {
            x += line_number_width + options->line_number_margin;
        }

        char* printable_line = NULL;
        int copy_index = 0;
        while(copy_index < vec_length(line_buffer.text) && x + options->text_width(printable_line, copy_index, editor->font, options->font_size) < options->area_width) {
            char c = line_buffer.text[copy_index++];
            vec_add(printable_line, c);
        }
        if(x + options->text_width(printable_line, copy_index, editor->font, options->font_size) > options->area_width) {
            vec_pop(printable_line);
        }
        vec_add(printable_line, '\0');

        vec_add(render_commands, ((render_command_t){.type = TEXT, .as.text = {.x = x, .y = y, .text = printable_line, .font = editor->font, .font_size = options->font_size}}));

        vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
            int width = options->text_width(line_buffer.text, cursor->x_start, editor->font, options->font_size);
            if(x + width - options->area_x > options->area_width) continue;
            if(cursor->y_start == index) {
                render_command_t command = {
                    .type = RECT,
                    .as.rect = {
                        .x = x + width,
                        .y = y,
                        .width = 2,
                        .height = options->font_size,
                        .color = {0,0,0, 255}
                    }
                };
                vec_add(render_commands, command);
            }
        }

        y += options->font_size + options->line_margin;
        if(y > options->area_height) {
            break;
        }
    }

    return render_commands;
}

void editor_stop_render(editor_t *editor, render_command_t* render_commands) {
    vec_for_each_cpy(render_command_t command, render_commands) {
        if(command.type == TEXT) {
            vec_free(command.as.text.text);
        }
    }
    vec_free(render_commands);

}

#undef VECTOR_PATH

#endif // EDITOR_IMPLEMENTATION

#undef NAMESPACE
#undef TYPENAME
#undef NAME
#undef cursor_t
#undef line_buffer_t
#undef editor_t
#undef render_type_t
#undef render_text_t
#undef render_rect_t
#undef render_data_t
#undef render_data_t
#undef render_options_t
#undef render_command_t
#undef init_options_t
#undef EDITOR_IMPLEMENTATION
#undef editor_create
#undef editor_start_render
#undef editor_insert_at_cursor

#endif //COMP_EDITOR_HEADER_
