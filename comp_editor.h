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
#include <stdint.h>

#ifndef cursor_t
#define cursor_t NAME(cursor_t)
#endif
typedef struct {
    int x_start,y_start;
    int x_end, y_end;
} cursor_t;

typedef struct {
    int start, end;
    int rendering_hint;
} word_coloring_t;

typedef struct {
    word_coloring_t *word_colorings;
} line_coloring_t;

#ifndef line_buffer_t
#define line_buffer_t NAME(line_buffer_t)
#endif
typedef struct {
    char* text;
    line_coloring_t coloring;
} line_buffer_t;

typedef struct {
    double length;
    double current;
    bool running;
    bool effect;
} editor_timer_t;
#ifndef editor_t
#define editor_t TYPENAME
#endif
typedef struct {
    line_buffer_t *lines;
    cursor_t *cursors;
    char** printable_lines;
    void *font;
    bool selecting;
    bool (*word_seperator)(uint32_t);
    int last_size;
    int scroll_start;
    line_coloring_t (*calculate_color)(char *, int, line_coloring_t);
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

#ifndef render_hint_t
#define render_hint_t NAME(render_hint_t)
#endif
typedef enum {
    HINT_CURSOR,
    HINT_TEXT,
    HINT_LINE_NUMBER,
    HINT_SELECTION,
    HINT_HIGHLIGHT_LINE,

    HINT_END
} render_hint_t;

#ifndef render_command_t
#define render_command_t NAME(render_command_t)
#endif
typedef struct {
    render_type_t type;
    render_data_t as;
    int render_hint;
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
editor_t editor_create(init_options_t options, line_coloring_t (*calculate_color)(char *, int, line_coloring_t));

#ifndef editor_insert_at_cursor
#define editor_insert_at_cursor NAME(insert_at_cursor)
#endif
void editor_insert_at_cursor(editor_t *editor, int c);

#ifndef editor_delete_at_cursor
#define editor_delete_at_cursor NAME(delete_at_cursor)
#endif
void editor_delete_at_cursor(editor_t *editor);

#ifndef editor_delete_word_at_cursor
#define editor_delete_word_at_cursor NAME(delete_word_at_cursor)
#endif
void editor_delete_word_at_cursor(editor_t *editor);

#ifndef editor_newline_at_cursor
#define editor_newline_at_cursor NAME(newline_at_cursor)
#endif
void editor_newline_at_cursor(editor_t *editor, bool splitline, int direction);

#ifndef editor_start_select
#define editor_start_select NAME(start_select)
#endif
void editor_start_select(editor_t *editor);

#ifndef editor_stop_select
#define editor_stop_select NAME(stop_select)
#endif
void editor_stop_select(editor_t *editor);

#ifndef editor_start_render
#define editor_start_render NAME(start_render)
#endif
render_command_t* editor_start_render(editor_t *editor, const render_options_t* options);

#ifndef editor_insert_block_at_cursor
#define editor_insert_block_at_cursor NAME(insert_block_at_cursor)
#endif
void editor_insert_block_at_cursor(editor_t *editor, const char* text, bool ignore_newlines, bool select);

#ifndef editor_get_selection
#define editor_get_selection NAME(get_selection)
#endif
char *editor_get_selection(editor_t *editor, int cursor_index);

#ifndef editor_jump_end_of_line
#define editor_jump_end_of_line NAME(jump_end_of_line)
#endif
void editor_jump_end_of_line(editor_t *editor);

#ifndef editor_jump_start_of_line
#define editor_jump_start_of_line NAME(jump_start_of_line)
#endif
void editor_jump_start_of_line(editor_t *editor);

#ifndef editor_dublicate_line_at_cursor
#define editor_dublicate_line_at_cursor NAME(dublicate_line_at_cursor)
#endif
void editor_dublicate_line_at_cursor(editor_t *editor, int direction);

#ifndef editor_move_line_at_cursor
#define editor_move_line_at_cursor NAME(move_line_at_cursor)
#endif
void editor_move_line_at_cursor(editor_t *editor, int direction);

#ifndef editor_delete_line_at_cursor
#define editor_delete_line_at_cursor NAME(delete_line_at_cursor)
#endif
void editor_delete_line_at_cursor(editor_t *editor);

#ifndef editor_jump_word
#define editor_jump_word NAME(jump_word)
#endif
void editor_jump_word(editor_t *editor, int direction);

#ifndef editor_mouse_to_cursor
#define editor_mouse_to_cursor NAME(mouse_to_cursor)
#endif
void editor_mouse_to_cursor(editor_t *editor, render_options_t *options, float x, float y, int *out_x, int* out_y);

#ifdef EDITOR_IMPLEMENTATION

#ifndef VECTOR_PATH
#define VECTOR_PATH "base/ds/vector.h"
#endif
#include VECTOR_PATH

#include <string.h>

void edutil_advance_timer(editor_timer_t *timer, double delta) {
    if(!timer->running) return;

    timer->current += delta;
    if(timer->current > timer->length) {
        timer->running = false;
        timer->effect = true;
    }
}

bool edutil_timer_poll(editor_timer_t *timer) {
    if(timer->effect) {
        timer->effect = false;
        return true;
    }

    return false;
}

void edutil_timer_start(editor_timer_t *timer, double length) {
    timer->running = true;
    timer->effect = false;
    timer->length = length;
    timer->current = 0;
}

void edutil_timer_stop(editor_timer_t *timer) {
    timer->running = false;
    timer->effect = false;
    timer->current = 0;
}
char** edutil_split_lines(const char* text) {
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

void check_editor_view(editor_t *editor, cursor_t *cursor) {
    if(cursor->y_start < editor->scroll_start) {
        editor->scroll_start = cursor->y_start;
    }

    if(cursor->y_start >= editor->scroll_start + editor->last_size) {
        editor->scroll_start = cursor->y_start - editor->last_size+1;
    }
}

void edutil_move_cursor_right(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(cursor->y_start >= vec_length(editor->lines)-1 && cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) return;

    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = 0;
        cursor->y_start++;
    } else {
        cursor->x_start++;
    }

    if(!selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }

    check_editor_view(editor, cursor);
}

void edutil_move_cursor_left(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(cursor->x_start <= 0 && cursor->y_start <= 0) return;

    if(cursor->x_start <= 0) {
        cursor->y_start--;
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    } else {
        cursor->x_start--;
    }


    if(!selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }
    check_editor_view(editor, cursor);
}

void edutil_move_cursor_down(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(cursor->y_start >= vec_length(editor->lines)-1) return;

    cursor->y_start++;
    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    }

    if(!selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }
    check_editor_view(editor, cursor);
}

void edutil_move_cursor_up(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(cursor->y_start <= 0) return;

    cursor->y_start--;
    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    }

    if(!selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }
    check_editor_view(editor, cursor);
}

void edutil_concat_lines(line_buffer_t *line1, line_buffer_t *line2) {
    vec_for_each_cpy(char c, line2->text) {
        vec_add(line1->text, c);
    }
}

void edutil_insert_line_in_line(line_buffer_t *line1, line_buffer_t *line2, int index) {
    for(int i = 0;i < vec_length(line2->text);i++) {
        vec_insert(line1->text, index+i, line2->text[i]);
    }
}

bool edutil_cursor_has_selection(cursor_t *cursor) {
    return cursor->x_start != cursor->x_end || cursor->y_start != cursor->y_end;
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

void edutil_remove_selection(editor_t *editor, cursor_t *cursor) {
    int x_start = cursor->x_start;
    int x_end = cursor->x_end;
    int y_start = cursor->y_start;
    int y_end = cursor->y_end;

    if(y_start == y_end && x_start > x_end) {
        x_start = cursor->x_end;
        x_end = cursor->x_start;
    } else if(y_start > y_end) {
        x_start = cursor->x_end;
        y_start = cursor->y_end;
        x_end = cursor->x_start;
        y_end = cursor->y_start;
    }

    if(y_start == y_end) {
        for(int i = x_start;i < x_end;i++){
            vec_remove(editor->lines[y_start].text, x_start);
        }
    } else {
        line_buffer_t removed = edutil_split_line_at(&editor->lines[y_start], x_start);
        vec_free(removed.text);
        line_buffer_t rest = edutil_split_line_at(&editor->lines[y_end], x_end);
        edutil_concat_lines(&editor->lines[y_start], &rest);

        for(int i = y_start+1;i <= y_end;i++){
            vec_free(editor->lines[y_start+1].text);
            vec_remove(editor->lines, y_start+1);
        }
    }

    cursor->x_start = x_start;
    cursor->x_end = x_start;
    cursor->y_start = y_start;
    cursor->y_end = y_start;
}

void move_cursor_right(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_right(editor, cursor, editor->selecting);
    }
}

void move_cursor_left(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_left(editor, cursor, editor->selecting);
    }
}

void jump_cursor_to(editor_t* editor, int x, int y) {
    cursor_t *cursor = &editor->cursors[0];

    cursor->x_start = x;
    cursor->y_start = y;

    if(!editor->selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }
}

void move_cursor_down(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_down(editor, cursor, editor->selecting);
    }
}

void move_cursor_up(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_move_cursor_up(editor, cursor, editor->selecting);
    }
}

editor_t editor_create(init_options_t options, line_coloring_t (*calculate_color)(char *, int, line_coloring_t)) {
    editor_t editor = {0};
    editor.calculate_color = calculate_color;

    char **lines = edutil_split_lines(options.text);

    line_buffer_t *line_buffers = NULL;

    vec_for_each_cpy(char *line, lines) {
        line_buffer_t buffer = {.text = line};
        line_coloring_t color = editor.calculate_color(line, vec_length(line), (line_coloring_t){0});
        buffer.coloring = color;
        vec_add(line_buffers, buffer);
    }
    vec_free(lines);

    editor.lines = line_buffers;

    cursor_t cursor = {0};
    vec_add(editor.cursors, cursor);
    return editor;
}

void editor_cleanup(editor_t *editor) {
    vec_free(editor->cursors);
    for(int i = 0;i < vec_length(editor->lines);i++) {
        vec_free(editor->lines[i].text);
        vec_free(editor->lines[i].coloring.word_colorings);
    }
    vec_free(editor->lines);
}

void editor_insert_at_cursor(editor_t *editor, int c) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        if(edutil_cursor_has_selection(cursor)) {
            edutil_remove_selection(editor, cursor);
        }

        line_buffer_t *line = &editor->lines[cursor->y_start];
        edutil_insert_in_line(line, cursor->x_start, c);
        line_coloring_t coloring = editor->calculate_color(line->text, vec_length(line->text), line->coloring);
        line->coloring = coloring;
        edutil_move_cursor_right(editor, cursor, false);
    }
}

void editor_delete_at_cursor(editor_t *editor) {
    int *delete_lines = NULL;
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        if(edutil_cursor_has_selection(cursor)) {
            edutil_remove_selection(editor, cursor);
            continue;
        }

        if(cursor->x_start == 0 && cursor->y_start == 0) continue;

        line_buffer_t *line = &editor->lines[cursor->y_start];
        line_buffer_t *prev_line = &editor->lines[cursor->y_start-1];

        if(cursor->x_start == 0) {
            vec_add(delete_lines, cursor->y_start);
            edutil_move_cursor_left(editor,cursor, false);
            printf("%d:%d \n", cursor->x_start, cursor->x_end);
            edutil_concat_lines(prev_line, line);
            line_coloring_t coloring = editor->calculate_color(prev_line->text, vec_length(prev_line->text), prev_line->coloring);
            prev_line->coloring = coloring;
        } else {
            edutil_remove_in_line(line, cursor->x_start-1);
            edutil_move_cursor_left(editor,cursor, false);
            line_coloring_t coloring = editor->calculate_color(line->text, vec_length(line->text), line->coloring);
            line->coloring = coloring;
        }

    }

    for(int j = vec_length(delete_lines)-1;j >= 0;j--){
        vec_remove(editor->lines, delete_lines[j]);
    }
}

void editor_newline_at_cursor(editor_t *editor, bool splitline, int direction) {
    cursor_t *cursor = &editor->cursors[0];
    if(splitline && edutil_cursor_has_selection(cursor)) {
        edutil_remove_selection(editor, cursor);
    }

    line_buffer_t newline = {NULL};
    if(splitline) {
        newline = edutil_split_line_at(&editor->lines[cursor->y_start], cursor->x_start);
    }

    vec_insert(editor->lines, cursor->y_start+direction, newline);
    if(splitline) {
        edutil_move_cursor_right(editor, cursor, false);
    } else if(direction == 1) {
        edutil_move_cursor_down(editor, cursor, false);
    } else if(direction == 0) {
        edutil_move_cursor_down(editor, cursor, false);
        edutil_move_cursor_up(editor, cursor, false);
    }
}

void editor_insert_block_at_cursor(editor_t *editor, const char* text, bool ignore_newlines, bool select) {
    cursor_t *cursor = &editor->cursors[0];

    if(edutil_cursor_has_selection(cursor)) {
        edutil_remove_selection(editor, cursor);
    }

    if(ignore_newlines) {
        line_buffer_t *line_buffer = &editor->lines[cursor->y_start];
        char* text_copy = NULL;
        for(int i = 0;i < strlen(text);i++){
            vec_add(text_copy, text[i]);
        }
        line_buffer_t new_line = (line_buffer_t){text_copy};

        edutil_insert_line_in_line(line_buffer, &new_line, cursor->x_start);
        cursor->x_start += vec_length(text_copy);
    } else {
        char **lines = edutil_split_lines(text);
        line_buffer_t rest = edutil_split_line_at(&editor->lines[cursor->y_start], cursor->x_start);
        line_buffer_t first = {lines[0]};
        edutil_concat_lines(&editor->lines[cursor->y_start], &first);

        if(vec_length(lines) == 1) {
            edutil_concat_lines(&editor->lines[cursor->y_start], &rest);
            cursor->x_start += vec_length(first.text);
        } else {
            for(int i = 1;i < vec_length(lines)-1;i++) {
                line_buffer_t line = {lines[i]};
                cursor->y_start++;
                vec_insert(editor->lines, cursor->y_start, line);
            }

            line_buffer_t line = {lines[vec_length(lines)-1]};
            cursor->x_start = vec_length(line.text);
            edutil_concat_lines(&line, &rest);
            cursor->y_start++;
            vec_insert(editor->lines, cursor->y_start, line);
        }
    }
}

char *editor_get_selection(editor_t *editor, int cursor_index) {
    cursor_t *cursor = &editor->cursors[cursor_index];

    if(!edutil_cursor_has_selection(cursor)) {
        return NULL;
    }

    char* text = NULL;
    int x_start = cursor->x_start;
    int x_end = cursor->x_end;
    int y_start = cursor->y_start;
    int y_end = cursor->y_end;

    if(y_start == y_end && x_start > x_end) {
        x_start = cursor->x_end;
        x_end = cursor->x_start;
    } else if(y_start > y_end) {
        x_start = cursor->x_end;
        y_start = cursor->y_end;
        x_end = cursor->x_start;
        y_end = cursor->y_start;
    }

    if(y_start == y_end) {
        for(int i = x_start;i < x_end;i++) {
            vec_add(text, editor->lines[y_start].text[i]);
        }
    } else {
        for(int i = x_start;i < vec_length(editor->lines[y_start].text);i++) {
            vec_add(text, editor->lines[y_start].text[i]);
        }
        vec_add(text,'\n');
        for(int i = y_start+1;i < y_end;i++) {
            for(int j = x_start;j < vec_length(editor->lines[i].text);j++) {
                vec_add(text, editor->lines[i].text[j]);
            }
            vec_add(text, '\n');
        }

        for(int i = 0;i < x_end;i++) {
            vec_add(text, editor->lines[y_end].text[i]);
        }
    }
    vec_add(text, '\0');

    return text;

}

render_command_t edutil_render_command_from_line(int x, int cur_y, int min_width, int height, int x_start, int x_end, line_buffer_t line_buffer, const render_options_t* options, editor_t *editor) {
    int width_to_start = options->text_width(line_buffer.text, x_start, editor->font, options->font_size);
    int rect_width = options->text_width(line_buffer.text, x_end, editor->font, options->font_size) - width_to_start;
    rect_width = min_width > rect_width ? min_width : rect_width;
    render_command_t command = {
        .type = RECT,
        .as.rect = {
            .x = x + width_to_start,
            .y = cur_y,
            .width = rect_width,
            .height = height,
            .color = {120, 120, 120, 100}
        },
        .render_hint = HINT_SELECTION
    };
    return command;
}

render_command_t* editor_start_render(editor_t *editor, const render_options_t* options) {
    render_command_t *render_commands = {0};
    int y = options->area_y;

    int line_number_width = 0;
    if(options->line_numbers) {
        int biggest_number = vec_length(editor->lines)-1;
        char *line_number = NULL;
        vec_init_size(line_number, 21);
        snprintf(line_number, 21, "%d", biggest_number);
        line_number_width = options->text_width(line_number, strlen(line_number), editor->font, options->font_size);
        vec_free(line_number);
        int x = options->area_x;
        for(int i = editor->scroll_start;i < vec_length(editor->lines);i++) {
            char *line_number = NULL;
            vec_init_size(line_number, 21);
            snprintf(line_number, 21, "%d", i+1);

            vec_add(render_commands, ((render_command_t){.type = TEXT, .render_hint = HINT_LINE_NUMBER, .as.text = {.x = x, .y = y, .text = line_number, .font = editor->font, .font_size = options->font_size}}));

            y += options->font_size + options->line_margin;
            if(y > options->area_height) {
                break;
            }
        }
    }

    int lines = 0;
    y = options->area_y;
    // printf("------ \n");
    for(int index = editor->scroll_start;index < vec_length(editor->lines);index++) {
        line_buffer_t line_buffer = editor->lines[index];

        int x = options->area_x;
        if(options->line_numbers) {
            x += line_number_width + options->line_number_margin;
        }

        char* printable_line = NULL;
        int copy_index = 0;
        bool in_highlight = false;
        int highlight_index = 0;
        line_coloring_t coloring = line_buffer.coloring;
        while(copy_index < vec_length(line_buffer.text) && options->text_width(printable_line, vec_length(printable_line), editor->font, options->font_size) < options->area_width) {
            if(in_highlight && highlight_index < vec_length(coloring.word_colorings) && copy_index == coloring.word_colorings[highlight_index].end) {
                if(printable_line != NULL) {
                    int x_shift = 0;
                    if(copy_index < vec_length(line_buffer.text)-1) {
                        char c = line_buffer.text[copy_index+1];
                        vec_add(printable_line, c);
                        x_shift += options->text_width(printable_line, vec_length(printable_line), editor->font, options->font_size);
                        x_shift -= options->text_width(&c, 1, editor->font, options->font_size);
                        vec_pop(printable_line);
                    } else {
                        x_shift += options->text_width(printable_line, vec_length(printable_line), editor->font, options->font_size);
                    }
                    vec_add(printable_line, '\0');
                    vec_add(render_commands, ((render_command_t){.type = TEXT, .render_hint = coloring.word_colorings[highlight_index].rendering_hint, .as.text = {.x = x, .y = y, .text = printable_line, .font = editor->font, .font_size = options->font_size}}));
                    x += x_shift;
                    printable_line = NULL;
                }
                in_highlight = false;
                highlight_index++;
            }

            if(!in_highlight && highlight_index < vec_length(coloring.word_colorings) && copy_index == coloring.word_colorings[highlight_index].start) {
                if(printable_line != NULL) {
                    int x_shift = 0;
                    if(copy_index < vec_length(line_buffer.text)-1) {
                        char c = line_buffer.text[copy_index+1];
                        vec_add(printable_line, c);
                        x_shift += options->text_width(printable_line, vec_length(printable_line), editor->font, options->font_size);
                        x_shift -= options->text_width(&c, 1, editor->font, options->font_size);
                        vec_pop(printable_line);
                    } else {
                        x_shift += options->text_width(printable_line, vec_length(printable_line), editor->font, options->font_size);
                    }
                    vec_add(printable_line, '\0');
                    vec_add(render_commands, ((render_command_t){.type = TEXT, .render_hint = HINT_TEXT, .as.text = {.x = x, .y = y, .text = printable_line, .font = editor->font, .font_size = options->font_size}}));
                    x += x_shift;
                    printable_line = NULL;
                }
                in_highlight = true;
            }

            if(index == 4) {
                // printf("%d:%d:%d:%d \n",copy_index, in_highlight, highlight_index, coloring.word_colorings[highlight_index].start);
            }

            char c = line_buffer.text[copy_index++];
            vec_add(printable_line, c);
        }
        if(options->text_width(printable_line, vec_length(printable_line), editor->font, options->font_size) > options->area_width) {
            vec_pop(printable_line);
        }
        vec_add(printable_line, '\0');

        vec_add(render_commands, ((render_command_t){.type = TEXT, .render_hint = in_highlight ? coloring.word_colorings[highlight_index].rendering_hint : HINT_TEXT, .as.text = {.x = x, .y = y, .text = printable_line, .font = editor->font, .font_size = options->font_size}}));

        lines++;
        y += options->font_size + options->line_margin;
        if(y > options->area_y+options->area_height) {
            break;
        }
    }

    editor->last_size = lines;

    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        if(cursor->y_start-editor->scroll_start > y && cursor->y_end-editor->scroll_start > y) continue;
        int x = options->area_x;
        if(options->line_numbers) {
            x += line_number_width + options->line_number_margin;
        }

        int x_start = cursor->x_start;
        int x_end = cursor->x_end;
        int y_start = cursor->y_start;
        int y_end = cursor->y_end;

        if(y_start == y_end && x_start > x_end) {
            x_start = cursor->x_end;
            x_end = cursor->x_start;
        } else if(y_start > y_end) {
            x_start = cursor->x_end;
            y_start = cursor->y_end;
            x_end = cursor->x_start;
            y_end = cursor->y_start;
        }

        int cur_y = options->area_y + options->font_size * (cursor->y_start - editor->scroll_start) + options->line_margin * (cursor->y_start - editor->scroll_start);
        line_buffer_t line_buffer = editor->lines[cursor->y_start];
        int width = options->text_width(line_buffer.text, cursor->x_start, editor->font, options->font_size);
        if(x + width - options->area_x > options->area_width) continue;
        render_command_t command = {
            .type = RECT,
            .as.rect = {
                .x = x + width,
                .y = cur_y,
                .width = 2,
                .height = options->font_size,
                .color = {0,0,0, 255}
            },
            .render_hint = HINT_CURSOR,
        };
        vec_add(render_commands, command);
        render_command_t highlight = {
            .type = RECT,
            .as.rect = {
                .x = options->area_x,
                .y = cur_y,
                .width = options->area_width,
                .height = options->font_size,
                .color = {0,0,0, 255}
            },
            .render_hint = HINT_HIGHLIGHT_LINE,
        };
        vec_add(render_commands, highlight);

        if(y_start == y_end && x_start != x_end) {
            render_command_t command = edutil_render_command_from_line(x, cur_y, 5, options->font_size, x_start, x_end, line_buffer, options, editor);
            vec_add(render_commands, command);
        } else if(y_start != y_end) {
            int cur_y = options->area_y + options->font_size * (y_start - editor->scroll_start) + options->line_margin * (y_start - editor->scroll_start);
            line_buffer_t line_buffer = editor->lines[y_start];
            render_command_t command = edutil_render_command_from_line(x, cur_y, 0, options->font_size + options->line_margin, x_start, vec_length(line_buffer.text), line_buffer, options, editor);
            vec_add(render_commands, command);
            for(int j = y_start +1; j < y_end;j++) {
                int cur_y = options->area_y + options->font_size * (j - editor->scroll_start) + options->line_margin * (j - editor->scroll_start);
                line_buffer = editor->lines[j];
                command = edutil_render_command_from_line(x, cur_y, 5, options->font_size + options->line_margin, 0, vec_length(line_buffer.text), line_buffer, options, editor);
                vec_add(render_commands, command);
            }

            cur_y = options->area_y + options->font_size * (y_end - editor->scroll_start) + options->line_margin * (y_end - editor->scroll_start);
            line_buffer = editor->lines[y_end];
            command = edutil_render_command_from_line(x, cur_y, 0, options->font_size, 0, x_end, line_buffer, options, editor);
            vec_add(render_commands, command);
        }
    }

    return render_commands;
}

void editor_start_select(editor_t *editor) {
    editor->selecting = true;
}

void editor_stop_select(editor_t *editor) {
    editor->selecting = false;
}

void editor_stop_render(editor_t *editor, render_command_t* render_commands) {
    vec_for_each_cpy(render_command_t command, render_commands) {
        if(command.type == TEXT) {
            vec_free(command.as.text.text);
        }
    }
    vec_free(render_commands);
}

void editor_jump_end_of_line(editor_t *editor) {
    for(int i = 0;i < vec_length(editor->cursors);i++) {
        cursor_t *cursor = &editor->cursors[i];
        line_buffer_t *line = &editor->lines[cursor->y_start];
        cursor->x_start = vec_length(line->text);

        if(!editor->selecting) {
            cursor->x_end = cursor->x_start;
            cursor->y_end = cursor->y_start;
        }
    }
}

void editor_jump_start_of_line(editor_t *editor) {
    for(int i = 0;i < vec_length(editor->cursors);i++) {
        cursor_t *cursor = &editor->cursors[i];
        cursor->x_start = 0;

        if(!editor->selecting) {
            cursor->x_end = cursor->x_start;
            cursor->y_end = cursor->y_start;
        }
    }
}

void editor_dublicate_line_at_cursor(editor_t *editor, int direction) {
    for(int i = 0;i < vec_length(editor->cursors);i++){
        cursor_t *cursor = &editor->cursors[i];
        line_buffer_t *line_buffer = &editor->lines[cursor->y_start];
        char* copy = NULL;
        for(int j = 0;j < vec_length(line_buffer->text);j++){
            vec_add(copy, line_buffer->text[j]);
        }

        line_buffer_t new_buffer = {copy};
        if(direction == 0) {
            vec_insert(editor->lines, cursor->y_start, new_buffer);
        } else {
            vec_insert(editor->lines, cursor->y_start+1, new_buffer);
            edutil_move_cursor_down(editor, cursor, false);
        }
    }
}

void editor_move_line_at_cursor(editor_t *editor, int amount) {
    for(int i = 0;i < vec_length(editor->cursors);i++){
        cursor_t *cursor = &editor->cursors[i];

        if(cursor->y_start + amount < 0 || cursor->y_start + amount >= vec_length(editor->lines)) {
            continue;
        }

        line_buffer_t line = editor->lines[cursor->y_start];
        printf("Cursor y stat: %d \n", cursor->y_start);

        int direction = amount < 0 ? -1 : 1;
        int abs_amount = amount < 0 ? -amount : amount;
        for(int j = 0;j < abs_amount;j++) {
            editor->lines[cursor->y_start] = editor->lines[cursor->y_start+j+direction];
        }
        editor->lines[cursor->y_start + amount] = line;
        cursor->y_start += amount;
        cursor->y_end += amount;
    }
}

void editor_delete_line_at_cursor(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];

    int y_start = cursor->y_start < cursor->y_end ? cursor->y_start : cursor->y_end;
    int y_end = cursor->y_start < cursor->y_end ? cursor->y_end : cursor->y_start;

    for(int i = y_start;i <= y_end;i++) {
        if(y_start == vec_length(editor->lines)-1) {
            vec_free(editor->lines[y_start].text);
            editor->lines[y_start].text = NULL;
            cursor->x_start = 0;
        } else {
            vec_remove(editor->lines, y_start);
        }
    }

    cursor->y_end = y_start;
    cursor->y_start = y_start;
    cursor->x_end = cursor->x_start;
}

int cursor_at_edge(editor_t *editor, cursor_t* cursor) {
    if(cursor->y_start == 0 && cursor->x_start == 0){
        return -1;
    }

    if(cursor->y_start == vec_length(editor->lines)-1 && cursor->x_start == vec_length(editor->lines[cursor->y_start].text)) {
        return 1;
    }

    return 0;
}



#define edge_cond ((direction == -1 && cursor_at_edge(editor, cursor) >= 0) || (direction == 0 && cursor_at_edge(editor, cursor) <= 0))
#define dir_cond(DIR) ((cursor->x_start == vec_length(editor->lines[cursor->y_start].text) && DIR == 0) ||  (cursor->x_start == 0 && DIR == -1))
#define sep_cond(DIR) (editor->word_seperator(dir_cond(DIR) ? '\n' : editor->lines[cursor->y_start].text[cursor->x_start + DIR]))
void editor_jump_word(editor_t *editor, int direction) {
    cursor_t *cursor = &editor->cursors[0];

    bool whitespace_before = sep_cond(direction);

    if(direction == -1 && cursor_at_edge(editor, cursor) < 0) return;
    if(direction == 0 && cursor_at_edge(editor, cursor) > 0) return;

    if(whitespace_before) {
        while(edge_cond && sep_cond(direction)) {
            if(direction == -1) {
                edutil_move_cursor_left(editor,cursor, editor->selecting);
            } else {
                edutil_move_cursor_right(editor,cursor, editor->selecting);
            }
        }
    }

    while(edge_cond && !sep_cond(direction)) {
        if(direction == -1) {
            edutil_move_cursor_left(editor,cursor, editor->selecting);
        } else {
            edutil_move_cursor_right(editor,cursor, editor->selecting);
        }
    }
}

void editor_delete_word_at_cursor(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];

    bool whitespace_before = sep_cond(-1);

    if(cursor_at_edge(editor, cursor) < 0) return;

    if(whitespace_before) {
        while(cursor_at_edge(editor, cursor) >= 0 && sep_cond(-1)) {
            editor_delete_at_cursor(editor);
        }
    }

    while(cursor_at_edge(editor, cursor) >= 0 && !sep_cond(-1)) {
        editor_delete_at_cursor(editor);
    }
}

void editor_mouse_to_cursor(editor_t *editor, render_options_t *options, float x, float y, int *out_x, int* out_y) {
    if(y < options->area_y || y > options->area_y + options->area_height || x < options->area_x || x > options->area_x + options->area_width) {
        *out_x = -1;
        *out_y = -1;
        return;
    }
    int line_number_width = 0;
    if(options->line_numbers) {
        int biggest_number = vec_length(editor->lines)-1;
        char *line_number = NULL;
        vec_init_size(line_number, 21);
        snprintf(line_number, 21, "%d", biggest_number);
        line_number_width = options->text_width(line_number, strlen(line_number), editor->font, options->font_size);
        vec_free(line_number);
    }

    float normalized_y = y - options->area_y;
    int relative_line = normalized_y / (options->font_size + options->line_margin);
    int final_line = relative_line + editor->scroll_start;

    if(final_line >= vec_length(editor->lines)) {
        *out_y = vec_length(editor->lines)-1;
        *out_x = vec_length(editor->lines[*out_y].text);
        return;

    }

    int normalized_x = x - options->area_x - line_number_width;

    line_buffer_t line_buffer = editor->lines[final_line];
    int copy_index = 0;
    while(copy_index < vec_length(line_buffer.text) && options->text_width(line_buffer.text, copy_index, editor->font, options->font_size) < normalized_x) {
        copy_index++;
    }
    int final_x = options->text_width(line_buffer.text, copy_index, editor->font, options->font_size) < normalized_x ? vec_length(line_buffer.text) : copy_index-1;

    *out_y = final_line;
    *out_x = final_x;
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
