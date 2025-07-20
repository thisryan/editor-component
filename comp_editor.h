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

#ifndef word_coloring_t
#define word_coloring_t NAME(word_coloring_t)
#endif
typedef struct {
    int start, end;
    int rendering_hint;
} word_coloring_t;

#ifndef line_coloring_t
#define line_coloring_t NAME(line_coloring_t)
#endif
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

#ifndef editor_timer_t
#define timer_t NAME(timer_t)
#endif
typedef struct {
    double length;
    double current;
    bool running;
    bool effect;
} editor_timer_t;

typedef enum {
    ACTION_INSERT,
    ACTION_MOVE,
    ACTION_DELETE,
    ACTION_COMPOUND,
    ACTION_DELETE_LINE,
    ACTION_NEWLINE,
    ACTION_MOVE_LINE
} editor_action_type_t;

typedef struct {
    int x_start, y_start;
    int x_end, y_end;
} action_insert_t;

typedef enum {
    DELETE_MULTIPLE,
    DELETE_SINGLE,
} delete_type_t;

typedef struct {
    delete_type_t type;
    char* data;
    char c;
} action_delete_t;

typedef struct {
    int index;
    line_buffer_t* lines;
} action_delete_line_t;

typedef struct editor_action_t editor_action_t;

typedef struct {
    editor_action_type_t compound_type;
    editor_action_t *actions;
} action_compound_t;

typedef struct {
    int line;
    bool splitline;
} action_newline_t;

typedef struct {
    int direction;
    int amount;
} action_move_line_t;

typedef union {
    action_insert_t insert;
    action_compound_t compound;
    action_delete_t delete;
    action_delete_line_t delete_line;
    action_newline_t newline;
    action_move_line_t move_line;
} action_data_t;

struct editor_action_t {
    editor_action_type_t type;
    action_data_t as;
    cursor_t before;
    cursor_t after;
};

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
    editor_action_t *actions;
    editor_timer_t action_timer;
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

#ifndef editor_cursor_mright
#define editor_cursor_mright NAME(cursor_mright)
#endif
void editor_cursor_mright(editor_t *editor);

#ifndef editor_cursor_mleft
#define editor_cursor_mleft NAME(cursor_mleft)
#endif
void editor_cursor_mleft(editor_t *editor);

#ifndef editor_cursor_mdown
#define editor_cursor_mdown NAME(cursor_mdown)
#endif
void editor_cursor_mdown(editor_t *editor);

#ifndef editor_cursor_mup
#define editor_cursor_mup NAME(cursor_mup)
#endif
void editor_cursor_mup(editor_t *editor);

#ifndef editor_create
#define editor_create NAME(create)
#endif
editor_t editor_create(
    init_options_t options,
    line_coloring_t (*calculate_color)(char *, int, line_coloring_t),
    bool (*word_seperator)(uint32_t c)
);

#ifndef editor_acursor_insert
#define editor_acursor_insert NAME(acursor_insert)
#endif
void editor_acursor_insert(editor_t *editor, int c);

#ifndef editor_acursor_delete
#define editor_acursor_delete NAME(acursor_delete)
#endif
void editor_acursor_delete(editor_t *editor, int direction, bool unindent);

#ifndef editor_acursor_word_delete
#define editor_acursor_word_delete NAME(acursor_word_delete)
#endif
void editor_acursor_word_delete(editor_t *editor, int direction);

#ifndef editor_acursor_newline
#define editor_acursor_newline NAME(acursor_newline)
#endif
void editor_acursor_newline(editor_t *editor, bool splitline, int direction);

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

#ifndef editor_acursor_insert_block
#define editor_acursor_insert_block NAME(acursor_insert_block)
#endif
void editor_acursor_insert_block(editor_t *editor, const char* text, bool ignore_newlines, bool select);

#ifndef editor_get_selection
#define editor_get_selection NAME(get_selection)
#endif
char *editor_get_selection(editor_t *editor, int cursor_index);

#ifndef editor_cursor_mendline
#define editor_cursor_mendline NAME(cursor_mendline)
#endif
void editor_cursor_mendline(editor_t *editor);

#ifndef editor_cursor_mstartline
#define editor_cursor_mstartline NAME(cursor_mstartline)
#endif
void editor_cursor_mstartline(editor_t *editor);

#ifndef editor_acursor_line_dublicate
#define editor_acursor_line_dublicate NAME(acursor_line_dublicate)
#endif
void editor_acursor_line_dublicate(editor_t *editor, int direction);

#ifndef editor_acursor_line_move
#define editor_acursor_line_move NAME(acursor_line_move)
#endif
void editor_acursor_line_move(editor_t *editor, int direction);

#ifndef editor_acursor_line_delete
#define editor_acursor_line_delete NAME(acursor_line_delete)
#endif
void editor_acursor_line_delete(editor_t *editor);

#ifndef editor_cursor_mword
#define editor_cursor_mword NAME(cursor_mword)
#endif
void editor_cursor_mword(editor_t *editor, int direction);

#ifndef editor_mouse_to_cursor
#define editor_mouse_to_cursor NAME(mouse_to_cursor)
#endif
void editor_mouse_to_cursor(editor_t *editor, render_options_t *options, float x, float y, int *out_x, int* out_y);

#ifndef editor_rollback
#define editor_rollback NAME(rollback)
#endif
void editor_rollback(editor_t *editor);

#ifndef editor_update_timers
#define editor_update_timers NAME(update_timers)
#endif
void editor_update_timers(editor_t *editor, double delta);

#ifndef editor_select_all
#define editor_select_all NAME(select_all)
#endif
void editor_select_all(editor_t *editor);

#ifndef editor_acursor_indent
#define editor_acursor_indent NAME(acursor_indent)
#endif
void editor_acursor_indent(editor_t *editor);

#ifndef editor_acursor_unindent
#define editor_acursor_unindent NAME(acursor_unindent)
#endif
void editor_acursor_unindent(editor_t *editor);

#ifdef EDITOR_IMPLEMENTATION

#ifndef VECTOR_PATH
#error "Please define VECTOR_PATH"
#endif
#include VECTOR_PATH

#include <string.h>

// --Utility

// --Misc

int edutil_min(int a, int b){
    return a < b ? a : b;
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

line_buffer_t *edutil_line_get(editor_t *editor, int index) {
    return &editor->lines[index];
}

int edutil_find_first_char(line_buffer_t *line) {
    int first_char = -1;
    for(int i = 0;i < vec_length(line->text);i++){
        if(line->text[i] != ' ') {
            first_char = i;
            break;
        }
    }

    return first_char;
}

int coloring_sort(const word_coloring_t *a, const word_coloring_t *b) {
    if(a->start == b->start) {
        return 0;
    }

    return a->start < b->start ? -1 : 1;
}

void edutil_update_coloring(editor_t* editor, line_buffer_t *line) {
    line_coloring_t color = editor->calculate_color(line->text, vec_length(line->text), (line_coloring_t){0});
    qsort(color.word_colorings, vec_length(color.word_colorings), sizeof(*color.word_colorings), (int (*)(const void *, const void *))coloring_sort);
    line->coloring = color;
}

void edutil_check_view(editor_t *editor, cursor_t *cursor) {
    if(cursor->y_start < editor->scroll_start) {
        editor->scroll_start = cursor->y_start;
    }

    if(cursor->y_start >= editor->scroll_start + editor->last_size) {
        editor->scroll_start = cursor->y_start - editor->last_size+1;
    }
}

char *edutil_copy_text(char* text, int start, int end) {
    char* new_text = NULL;
    for(int i = start;i < edutil_min(end, vec_length(text));i++){
        vec_add(new_text, text[i]);
    }
    return new_text;
}

int edutil_line_number_width(editor_t *editor, const render_options_t *options) {
    int line_number_width = 0;

    if(options->line_numbers) {
        int biggest_number = vec_length(editor->lines);
        char line_number[21];

        snprintf(line_number, 21, "%d", biggest_number);
        line_number_width = options->text_width(line_number, strlen(line_number), editor->font, options->font_size);
        line_number_width += options->line_number_margin;
    }

    return line_number_width;
}

int edutil_line_index_from_width(editor_t *editor, render_options_t *options, line_buffer_t *line, int width) {
    if(options->text_width(line->text, vec_length(line->text), editor->font, options->font_size) < width) {
        return vec_length(line->text);
    }

    int index = 0;
    while(index < vec_length(line->text)
        && options->text_width(line->text, index, editor->font, options->font_size) < width){
        index++;
    }
    return index-1;
}

// --Timer

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

void edutil_timer_advance(editor_timer_t *timer, double delta) {
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

// --Action

void edutil_add_action_compound(editor_t *editor, editor_action_t *actions) {
    editor_action_t compound = {
        .type = ACTION_COMPOUND,
        .as.compound = {
            .compound_type = actions[0].type,
            .actions = actions
        }
    };
    vec_add(editor->actions, compound);
}

void edutil_add_action_impl(editor_t *editor, editor_action_t action) {
    editor_action_t *last_action = &vec_peek(editor->actions);

    printf("Adding action: running: %d \n", editor->action_timer.running);
    if(editor->action_timer.running
        && last_action->type == ACTION_COMPOUND
        && last_action->as.compound.compound_type == action.type) {
        vec_add(last_action->as.compound.actions, action);
        return;
    }

    if(editor->action_timer.running
    && last_action->type == action.type) {
        editor_action_t *actions = NULL;
        vec_add(actions, *last_action);
        vec_pop(editor->actions);

        vec_add(actions, action);
        edutil_add_action_compound(editor, actions);
        return;
    }

    vec_add(editor->actions, action);
}

void edutil_add_action(editor_t *editor, editor_action_t action) {
    if(vec_length(editor->actions) > 0) {
        edutil_add_action_impl(editor, action);
    } else {
        vec_add(editor->actions, action);
    }

    edutil_timer_start(&editor->action_timer, 0.5);
}

void edutil_add_action_insert(editor_t *editor, cursor_t before, cursor_t after) {
    editor_action_t action = {
        .type = ACTION_INSERT,
        .as.insert = {.x_start = before.x_start, .y_start = before.y_start, .y_end = after.y_start, .x_end = after.x_start},
        .before = before,
        .after = after
    };
    edutil_add_action(editor, action);
}

void edutil_add_action_delete_single(editor_t *editor, cursor_t before, cursor_t after, uint32_t c) {
    editor_action_t delete = {.type = ACTION_DELETE, .before = before, .after = after, .as.delete = {.type = DELETE_SINGLE, .c = c}};
    edutil_add_action(editor, delete);
}

void edutil_add_action_delete_multiple(editor_t *editor, cursor_t before, cursor_t after, char *cs) {
    editor_action_t delete = {.type = ACTION_DELETE, .before = before, .after = after, .as.delete = {.type = DELETE_MULTIPLE, .data = cs}};
    edutil_add_action(editor, delete);
}

void edutil_add_action_delete_lines(editor_t *editor, cursor_t before, cursor_t after, int index, line_buffer_t *lines) {
    editor_action_t action = {
        .type = ACTION_DELETE_LINE,
        .before = before,
        .after = after,
        .as.delete_line = {
            .index = index,
            .lines = lines
        }
    };
    edutil_add_action(editor, action);
}

void edutil_add_action_newline(editor_t *editor, cursor_t before, cursor_t after, int line, bool splitline) {
    editor_action_t action = {
        .type = ACTION_NEWLINE,
        .before = before,
        .after = after,
        .as.newline = {
            .line = line,
            .splitline = splitline
        }
    };
    edutil_add_action(editor, action);
}

void edutil_add_action_move_line(editor_t *editor, cursor_t before, cursor_t after, int direction, int amount) {
    editor_action_t action = {
        .type = ACTION_MOVE_LINE,
        .before = before,
        .after = after,
        .as.move_line = {
            .direction = direction,
            .amount = amount
        }
    };
    edutil_add_action(editor, action);
}

// --Cursor

void edutil_pmove(editor_t * editor, cursor_t *cursor, bool selecting) {
    if(!selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }

    edutil_check_view(editor, cursor);
}

bool edutil_cursor_aedge(editor_t *editor, cursor_t* cursor, int direction) {
    return (direction == -1 && cursor->x_start == 0 && cursor->y_start == 0)
        || (direction == 0 && cursor->y_start >= vec_length(editor->lines)-1 && cursor->x_start == vec_length(editor->lines[cursor->y_start].text));
}

void edutil_cursor_mright(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(edutil_cursor_aedge(editor, cursor, 0)) return;

    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = 0;
        cursor->y_start++;
    } else {
        cursor->x_start++;
    }

    edutil_pmove(editor, cursor, selecting);
}

void edutil_cursor_mleft(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(edutil_cursor_aedge(editor, cursor, -1)) return;

    if(cursor->x_start <= 0) {
        cursor->y_start--;
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    } else {
        cursor->x_start--;
    }

    edutil_pmove(editor, cursor, selecting);
}

void edutil_cursor_mdown(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(cursor->y_start >= vec_length(editor->lines)-1) return;

    cursor->y_start++;
    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    }

    edutil_pmove(editor, cursor, selecting);
}

void edutil_cursor_mup(editor_t *editor, cursor_t *cursor, bool selecting) {
    if(cursor->y_start <= 0) return;

    cursor->y_start--;
    if(cursor->x_start >= vec_length(editor->lines[cursor->y_start].text)) {
        cursor->x_start = vec_length(editor->lines[cursor->y_start].text);
    }

    edutil_pmove(editor, cursor, selecting);
}

bool edutil_cursor_hselection(cursor_t *cursor) {
    return cursor->x_start != cursor->x_end || cursor->y_start != cursor->y_end;
}

void edutil_cursor_set(cursor_t *cursor, int x_start, int y_start) {
    cursor->x_start = x_start;
    cursor->x_end = x_start;
    cursor->y_start = y_start;
    cursor->y_end = y_start;
}

void edutil_cursor_sorted(cursor_t *cursor, int *out_x_start, int *out_y_start, int *out_x_end, int *out_y_end) {
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

    *out_x_start = x_start;
    *out_y_start = y_start;
    *out_x_end = x_end;
    *out_y_end = y_end;
}

bool edutil_cursor_in_intendation(cursor_t *cursor, line_buffer_t *line) {
    int first_char = edutil_find_first_char(line);
    return cursor->x_start != 0 && (first_char == -1 || first_char >= cursor->x_start);
}

// --Lines

void edutil_insert_line(editor_t *editor, line_buffer_t line, int index) {
    edutil_update_coloring(editor, &line);
    vec_insert(editor->lines, index, line);
}

void edutil_insert_in_line(line_buffer_t *line, int index, int c) {
    vec_insert(line->text, index, (char)c);
}

char edutil_remove_in_line(line_buffer_t *line, int index) {
    char c = line->text[index];
    vec_remove(line->text, index);
    return c;
}

char* edutil_remove_in_lineEx(editor_t *editor, line_buffer_t *line, int start, int end) {
    char *cs = NULL;
    for(int i = start;i < end;i++) {
        int c = edutil_remove_in_line(line, start);
        vec_add(cs, c);
    }
    edutil_update_coloring(editor, line);

    return cs;
}

void edutil_insert_line_in_line(line_buffer_t *line1, line_buffer_t *line2, int index) {
    for(int i = 0;i < vec_length(line2->text);i++) {
        vec_insert(line1->text, index+i, line2->text[i]);
    }
}

void edutil_concat_lines(editor_t *editor, line_buffer_t *line1, line_buffer_t *line2) {
    vec_for_each_cpy(char c, line2->text) {
        vec_add(line1->text, c);
    }
    edutil_update_coloring(editor, line1);
}

line_buffer_t edutil_split_line_at(editor_t *editor, line_buffer_t *line1, int index) {
    char *text = NULL;
    size_t length = vec_length(line1->text);

    for(int j = index;j < length;j++) {
        vec_add(text, line1->text[index]);
        vec_remove(line1->text, index);
    }

    line_buffer_t newline =  { text };

    edutil_update_coloring(editor, line1);
    edutil_update_coloring(editor, &newline);

   return newline;
}

char* edutil_get_block(editor_t *editor, int x_from, int y_from, int x_to, int y_to) {
    char *text = NULL;

    if(y_from == y_to) {
        for(int i = x_from;i < x_to;i++) {
            vec_add(text, editor->lines[y_from].text[i]);
        }
    } else {
        for(int i = x_from;i < vec_length(editor->lines[y_from].text);i++) {
            vec_add(text, editor->lines[y_from].text[i]);
        }
        vec_add(text,'\n');

        for(int i = y_from+1;i < y_to;i++) {
            for(int j = 0;j < vec_length(editor->lines[i].text);j++) {
                vec_add(text, editor->lines[i].text[j]);
            }
            vec_add(text, '\n');
        }

        for(int i = 0;i < x_to;i++) {
            vec_add(text, editor->lines[y_to].text[i]);
        }
    }
    vec_add(text, '\0');

    return text;
}

char* edutil_remove_block(editor_t *editor, int x_from, int y_from, int x_to, int y_to) {
    char *cs = NULL;
    if(y_from == y_to) {
        line_buffer_t *line = edutil_line_get(editor, y_from);
        cs = edutil_remove_in_lineEx(editor, line, x_from, x_to);
        vec_add(cs, '\0');
    } else {
        cs = edutil_get_block(editor, x_from, y_from, x_to, y_to);

        line_buffer_t *start = edutil_line_get(editor, y_from);
        line_buffer_t other = edutil_split_line_at(editor, start, x_from);
        // TODO(Adrian): Line free
        vec_free(other.text);

        line_buffer_t *end = edutil_line_get(editor, y_to);
        other = edutil_split_line_at(editor, end, x_to);

        edutil_concat_lines(editor, start, &other);
        for(int i = y_from+1;i <= y_to;i++){
            vec_remove(editor->lines, y_from+1);
        }
    }

    return cs;
}

void edutil_remove_selection(editor_t *editor, cursor_t *cursor) {
    int x_start, y_start, x_end, y_end;
    edutil_cursor_sorted(cursor, &x_start, &y_start, &x_end, &y_end);

    cursor_t before = *cursor;

    char *cs = edutil_remove_block(editor, x_start, y_start, x_end, y_end);
    edutil_cursor_set(cursor, x_start, y_start);

    cursor_t after = *cursor;

    edutil_add_action_delete_multiple(editor, before, after, cs);
}

void edutil_insert_block(editor_t *editor, cursor_t *cursor, const char* text, int x_from, int y_from, int x_to, int y_to, bool ignore_newlines, bool select) {
    if(ignore_newlines) {
        line_buffer_t *line_buffer = &editor->lines[y_from];
        char* text_copy = NULL;
        for(int i = 0;i < strlen(text);i++){
            vec_add(text_copy, text[i]);
        }
        line_buffer_t new_line = (line_buffer_t){text_copy};

        edutil_insert_line_in_line(line_buffer, &new_line, x_from);
        edutil_update_coloring(editor, line_buffer);
        if(select) {
            cursor->x_start += vec_length(text_copy);
        }
        return;
    }

    char **lines = edutil_split_lines(text);
    line_buffer_t rest = edutil_split_line_at(editor, &editor->lines[y_from], x_from);
    line_buffer_t first = {lines[0]};
    edutil_concat_lines(editor, &editor->lines[y_from], &first);

    if(vec_length(lines) == 1) {
        edutil_concat_lines(editor, &editor->lines[y_from], &rest);
        x_from += vec_length(first.text);
    } else {
        for(int i = 1;i < vec_length(lines)-1;i++) {
            line_buffer_t line = {lines[i]};
            edutil_update_coloring(editor, &line);
            y_from++;
            vec_insert(editor->lines, y_from, line);
        }

        line_buffer_t line = {lines[vec_length(lines)-1]};
        x_from = vec_length(line.text);
        edutil_concat_lines(editor, &line, &rest);
        y_from++;
        vec_insert(editor->lines, y_from, line);
    }

    if(select) {
        cursor->x_start = x_from;
        cursor->y_start = y_from;
    }
}

void edutil_move_block(editor_t *editor, int y_from, int y_to, int direction, int amount) {
    int abs_amount = amount < 0 ? -amount : amount;
    int move_start = direction < 0 ? 0: (y_to - y_from);
    for(int i = 0;i < abs_amount;i++){
        line_buffer_t line = editor->lines[direction < 0 ? y_from-1 : y_to+1];

        for(int j = 0;j <= (y_to - y_from);j++){
            editor->lines[y_from + abs(move_start - j) + direction] = editor->lines[y_from + abs(move_start - j)];
        }

        editor->lines[direction < 0 ? y_to : y_from] = line;

        y_from += direction;
        y_to += direction;
    }
}

// ++Editor functions

// ++Cursor

void editor_cursor_mright(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_cursor_mright(editor, cursor, editor->selecting);
    }
}

void editor_cursor_mleft(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_cursor_mleft(editor, cursor, editor->selecting);
    }
}

void editor_cursor_mdown(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_cursor_mdown(editor, cursor, editor->selecting);
    }
}

void editor_cursor_mup(editor_t *editor) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        edutil_cursor_mup(editor, cursor, editor->selecting);
    }
}

void editor_cursor_mto(editor_t* editor, int x, int y) {
    cursor_t *cursor = &editor->cursors[0];

    cursor->x_start = x;
    cursor->y_start = y;

    if(!editor->selecting) {
        cursor->x_end = cursor->x_start;
        cursor->y_end = cursor->y_start;
    }
}

void editor_cursor_mendline(editor_t *editor) {
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

void editor_cursor_mstartline(editor_t *editor) {
    for(int i = 0;i < vec_length(editor->cursors);i++) {
        cursor_t *cursor = &editor->cursors[i];
        cursor->x_start = 0;

        if(!editor->selecting) {
            cursor->x_end = cursor->x_start;
            cursor->y_end = cursor->y_start;
        }
    }
}

// TODO(Adrian): When multiple white space only jump whitespace?
#define dir_cond(DIR) ((cursor->x_start == vec_length(editor->lines[cursor->y_start].text) && DIR == 0) ||  (cursor->x_start == 0 && DIR == -1))
#define sep_cond(DIR) (editor->word_seperator(dir_cond(DIR) ? '\n' : editor->lines[cursor->y_start].text[cursor->x_start + DIR]))
void editor_cursor_mword(editor_t *editor, int direction) {
    cursor_t *cursor = &editor->cursors[0];

    bool whitespace_before = sep_cond(direction);

    if(edutil_cursor_aedge(editor, cursor, direction)) return;

    if(whitespace_before) {
        while(!edutil_cursor_aedge(editor, cursor, direction) && sep_cond(direction)) {
            if(direction == -1) {
                edutil_cursor_mleft(editor,cursor, editor->selecting);
            } else {
                edutil_cursor_mright(editor,cursor, editor->selecting);
            }
        }
    }

    while(!edutil_cursor_aedge(editor, cursor, direction) && !sep_cond(direction)) {
        if(direction == -1) {
            edutil_cursor_mleft(editor,cursor, editor->selecting);
        } else {
            edutil_cursor_mright(editor,cursor, editor->selecting);
        }
    }
}

void editor_select_all(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];

    int last_line = vec_length(editor->lines)-1;
    int last_line_end = vec_length(editor->lines[last_line].text);

    cursor->x_start = last_line_end;
    cursor->y_start = last_line;
    cursor->x_end = 0;
    cursor->y_end = 0;
}

// ++Orginization

editor_t editor_create(
    init_options_t options,
    line_coloring_t (*calculate_color)(char *, int, line_coloring_t),
    bool (*word_seperator)(uint32_t c)
) {
    // Setup basics
    editor_t editor = {0};
    editor.calculate_color = calculate_color;
    editor.word_seperator = word_seperator;

    // Setup lines
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

    // Setup cursors
    cursor_t cursor = {0};
    vec_add(editor.cursors, cursor);

    return editor;
}

void editor_cleanup(editor_t *editor) {
    // Cleanup lines
    for(int i = 0;i < vec_length(editor->lines);i++) {
        vec_free(editor->lines[i].text);
        vec_free(editor->lines[i].coloring.word_colorings);
    }
    vec_free(editor->lines);

    // Cleanup cursors
    vec_free(editor->cursors);

    // Cleanup actions
    for(int i = 0;i < vec_length(editor->actions);i++){
        if(editor->actions[i].type == ACTION_COMPOUND) {
            vec_free(editor->actions[i].as.compound.actions);
        }

        if(editor->actions[i].type == ACTION_DELETE){
            vec_free(editor->actions[i].as.delete.data);
        }
    }
    vec_free(editor->actions);
}

void editor_update_timers(editor_t *editor, double delta) {
    edutil_timer_advance(&editor->action_timer, delta);
}

// ++Mutation

void editor_acursor_insert(editor_t *editor, int c) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        if(edutil_cursor_hselection(cursor)) {
            edutil_remove_selection(editor, cursor);
        }

        line_buffer_t *line = &editor->lines[cursor->y_start];
        edutil_insert_in_line(line, cursor->x_start, c);

        cursor_t before = *cursor;
        edutil_cursor_mright(editor, cursor, false);
        cursor_t after = *cursor;

        edutil_update_coloring(editor, line);
        edutil_add_action_insert(editor, before, after);
    }
}

void editor_delete_one(editor_t *editor, cursor_t *cursor, int direction, bool unindent) {
    if(edutil_cursor_hselection(cursor)) {
        edutil_remove_selection(editor, cursor);
        return;
    }

    if(edutil_cursor_aedge(editor, cursor, direction)) return;

    line_buffer_t *indent_check = &editor->lines[cursor->y_start];
    if(direction == -1 && edutil_cursor_in_intendation(cursor, indent_check)){
        do {
            vec_remove(indent_check->text, cursor->x_start-1);
            edutil_cursor_mleft(editor, cursor, false);
        } while(cursor->x_start % 4 != 0);
        edutil_update_coloring(editor, indent_check);

        return;
    }

    cursor_t before = *cursor, after = *cursor;
    if(direction == -1) {
        edutil_cursor_mleft(editor, cursor, false);
        after = *cursor;
    }

    line_buffer_t *line = &editor->lines[cursor->y_start];
    line_buffer_t *other_line = &editor->lines[cursor->y_start + 1];

    int c;
    if(cursor->x_start == vec_length(line->text)) {

        edutil_concat_lines(editor, line, other_line);
        vec_remove(editor->lines, cursor->y_start+1);

        c = '\n';
    } else {
        c = edutil_remove_in_line(line, cursor->x_start);
        // TODO(Adrian): unindent on backspace
    }
    edutil_add_action_delete_single(editor, before, after, c);
    edutil_update_coloring(editor, line);
}

void editor_acursor_delete(editor_t *editor, int direction, bool unindent) {
    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        editor_delete_one(editor, cursor, direction, unindent);
    }
}

void editor_acursor_newline(editor_t *editor, bool splitline, int direction) {
    cursor_t *cursor = &editor->cursors[0];
    if(splitline && edutil_cursor_hselection(cursor)) {
        edutil_remove_selection(editor, cursor);
    }

    line_buffer_t newline = {NULL};
    if(splitline) {
        newline = edutil_split_line_at(editor, &editor->lines[cursor->y_start], cursor->x_start);
        edutil_update_coloring(editor, &newline);
    }

    edutil_insert_line(editor, newline, cursor->y_start+direction);

    cursor_t before = *cursor;
    if(splitline) {
        edutil_cursor_mright(editor, cursor, false);
    } else if(direction == 1) {
        edutil_cursor_mdown(editor, cursor, false);
    } else if(direction == 0) {
        edutil_cursor_mdown(editor, cursor, false);
        edutil_cursor_mup(editor, cursor, false);
    }
    cursor_t after = *cursor;

    edutil_add_action_newline(editor, before, after, cursor->y_start, splitline);
}

void editor_acursor_insert_block(editor_t *editor, const char* text, bool ignore_newlines, bool select) {
    cursor_t *cursor = &editor->cursors[0];

    int x_from, y_from, x_to, y_to;
    edutil_cursor_sorted(cursor, &x_from, &y_from, &x_to, &y_to);

    cursor_t before = *cursor;
    edutil_insert_block(editor, cursor, text, x_from, y_from, x_to, y_to, ignore_newlines, select);
    cursor_t after = *cursor;

    edutil_add_action_insert(editor, before, after);
}

void editor_acursor_line_dublicate(editor_t *editor, int direction) {
    cursor_t *cursor = &editor->cursors[0];
    int x_from, y_from, x_to, y_to;
    edutil_cursor_sorted(cursor, &x_from, &y_from, &x_to, &y_to);

    cursor_t before = *cursor;

    for(int i = y_to;i >= y_from;i--){
        line_buffer_t *line_buffer = &editor->lines[i];

        char* copy = edutil_copy_text(line_buffer->text, 0, vec_length(line_buffer->text));
        line_buffer_t new_buffer = {copy};

        edutil_insert_line(editor, new_buffer, y_to+1);
    }

    cursor->y_start += (y_to - y_from)+1;
    cursor->y_end += (y_to - y_from)+1;

    cursor_t after = *cursor;

    for(int i = y_from;i <= y_to;i++){
        edutil_add_action_newline(editor, before, after, i, false);
    }
}


void editor_acursor_line_move(editor_t *editor, int amount) {
    cursor_t *cursor = &editor->cursors[0];

    cursor_t before = *cursor;

    int _x_from, y_from, _x_to, y_to;
    edutil_cursor_sorted(cursor, &_x_from, &y_from, &_x_to, &y_to);

    if(y_from + amount < 0 || y_to + amount >= vec_length(editor->lines)) {
        return;
    }

    int direction = amount < 0 ? -1 : 1;
    edutil_move_block(editor, y_from, y_to, direction, amount);

    cursor->y_start += amount;
    cursor->y_end += amount;

    cursor_t after = *cursor;

    edutil_add_action_move_line(editor, before, after, direction, amount);
}

void editor_acursor_line_delete(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];

    cursor_t before = *cursor;
    int y_start = cursor->y_start < cursor->y_end ? cursor->y_start : cursor->y_end;
    int y_end = cursor->y_start < cursor->y_end ? cursor->y_end : cursor->y_start;

    line_buffer_t *lines = NULL;
    for(int i = y_start;i <= y_end;i++) {
        line_buffer_t *line = edutil_line_get(editor, i);
        char *text = edutil_copy_text(line->text, 0, vec_length(line->text));
        line_buffer_t save_line = {.text = text};

        vec_add(lines, save_line);

        if(y_start == vec_length(editor->lines)-1) {
            vec_free(editor->lines[y_start].text);
            editor->lines[y_start].text = NULL;
            cursor->x_start = 0;
        } else {
            vec_remove(editor->lines, y_start);
        }
    }

    edutil_cursor_set(cursor, 0, y_start);

    cursor_t after = *cursor;
    edutil_add_action_delete_lines(editor, before, after, y_start, lines);
}

void editor_acursor_word_delete(editor_t *editor, int direction) {
    cursor_t *cursor = &editor->cursors[0];

    bool whitespace_before = sep_cond(direction);

    if(edutil_cursor_aedge(editor, cursor, direction)) return;

    if(whitespace_before) {
        while(!edutil_cursor_aedge(editor, cursor, direction) && sep_cond(direction)) {
            editor_delete_one(editor, cursor, direction, false);
        }
    }

    while(!edutil_cursor_aedge(editor, cursor, direction) && !sep_cond(direction)) {
        editor_delete_one(editor, cursor, direction,  false);
    }
}

void editor_start_select(editor_t *editor) {
    editor->selecting = true;
}

void editor_stop_select(editor_t *editor) {
    editor->selecting = false;
}

void editor_set_select(editor_t *editor, bool selecting) {
    editor->selecting = selecting;
}

void editor_acursor_indent(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];
    cursor_t before = *cursor;

    if(!edutil_cursor_hselection(cursor)) {
        line_buffer_t *current = &editor->lines[cursor->y_start];
        edutil_insert_in_line(current, cursor->x_start, ' ');
        edutil_cursor_mright(editor, cursor, false);


        while(cursor->x_start % 4 != 0){
            edutil_insert_in_line(current, cursor->x_start, ' ');
            edutil_cursor_mright(editor, cursor, false);
        }

        cursor_t after = *cursor;
        edutil_add_action_insert(editor, before, after);

        edutil_update_coloring(editor, current);
        return;
    }

    int y_start = cursor->y_start < cursor->y_end ? cursor->y_start : cursor->y_end;
    int y_end = cursor->y_start < cursor->y_end ? cursor->y_end : cursor->y_start;

    for(int i = y_start;i <= y_end;i++) {
        line_buffer_t *line = &editor->lines[i];

        for(int j = 0;j < 4;j++){
            if(i == cursor->y_start) {
                cursor->x_start++;
            }
            if(i == cursor->y_end) {
                cursor->x_end++;
            }
            vec_insert(line->text, 0, ' ');
        }

        edutil_update_coloring(editor, line);
    }
    cursor_t after = *cursor;

    edutil_add_action_insert(editor, before, after);
}

void editor_acursor_unindent(editor_t *editor) {
    cursor_t *cursor = &editor->cursors[0];

    cursor_t before = *cursor;

    int y_start = cursor->y_start < cursor->y_end ? cursor->y_start : cursor->y_end;
    int y_end = cursor->y_start < cursor->y_end ? cursor->y_end : cursor->y_start;

    char *indents = NULL;
    for(int i = y_start;i <= y_end;i++) {
        line_buffer_t *line = &editor->lines[i];

        int first_char = edutil_find_first_char(line);

        int stop = edutil_min(4, first_char);
        if(first_char == -1) {
            stop = edutil_min(4, vec_length(line->text));
        }

        for(int j = 0;j < stop;j++){
            if(i == cursor->y_start) {
                cursor->x_start--;
            }
            if(i == cursor->y_end) {
                cursor->x_end--;
            }

            vec_add(indents, line->text[0]);
            vec_remove(line->text, 0);
        }

        edutil_update_coloring(editor, line);
    }

    cursor_t after = *cursor;

    vec_add(indents, '\0');
    edutil_add_action_delete_multiple(editor, before, after, indents);
}

// ++Information

void editor_mouse_to_cursor(editor_t *editor, render_options_t *options, float x, float y, int *out_x, int* out_y) {
    if(y < options->area_y || y > options->area_y + options->area_height || x < options->area_x || x > options->area_x + options->area_width) {
        *out_x = -1;
        *out_y = -1;
        return;
    }

    float normalized_y = y - options->area_y;
    int relative_line = normalized_y / (options->font_size + options->line_margin);
    int final_line = relative_line + editor->scroll_start;

    if(final_line >= vec_length(editor->lines)) {
        *out_y = vec_length(editor->lines)-1;
        *out_x = vec_length(editor->lines[*out_y].text);
        return;
    }

    int line_number_width = edutil_line_number_width(editor, options);
    int normalized_x = x - options->area_x - line_number_width;

    line_buffer_t line = editor->lines[final_line];
    int final_x = edutil_line_index_from_width(editor, options, &line, normalized_x);

    *out_y = final_line;
    *out_x = final_x;
}

char *editor_get_selection(editor_t *editor, int cursor_index) {
    cursor_t *cursor = &editor->cursors[cursor_index];

    if(!edutil_cursor_hselection(cursor)) {
        return NULL;
    }

    int x_from, y_from, x_to, y_to;
    edutil_cursor_sorted(cursor, &x_from, &y_from, &x_to, &y_to);
    char *text = edutil_get_block(editor, x_from, y_from, x_to, y_to);

    return text;
}

// ++Rendering

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

void edutil_render_line_numbers(editor_t *editor, const render_options_t *options, render_command_t **render_commands) {
    int x = options->area_x;
    int y = options->area_y;
    for(int i = editor->scroll_start;i < vec_length(editor->lines);i++) {
        char *line_number = NULL;
        vec_init_size(line_number, 21);
        snprintf(line_number, 21, "%d", i+1);

        vec_add(*render_commands, ((render_command_t){.type = TEXT, .render_hint = HINT_LINE_NUMBER, .as.text = {.x = x, .y = y, .text = line_number, .font = editor->font, .font_size = options->font_size}}));

        y += options->font_size + options->line_margin;
        if(y > options->area_height) {
            break;
        }
    }
}

typedef struct {
    char* text;
    int hint;
    int start;
} text_with_hint_t;

int edutil_next_coloring(const word_coloring_t* words, int current) {
    for(int i = 0;i < vec_length(words);i++) {
        if(words[i].start >= current) {
            return i;
        }
    }

    return -1;
}

text_with_hint_t *edutil_split_highlight(line_buffer_t *buffer) {
    text_with_hint_t *hints = NULL;

    int last_end = 0;
    const word_coloring_t* words = buffer->coloring.word_colorings;
    while(last_end < vec_length(buffer->text)) {
        int next = edutil_next_coloring(words, last_end);

        int start = last_end;
        int end = vec_length(buffer->text);
        int hint = HINT_TEXT;
        if(next != -1){
            if(last_end >= words[next].start) {
                end = words[next].end;
                hint = words[next].rendering_hint;
            } else {
                end = words[next].start;
            }
        }

        char *text = edutil_copy_text(buffer->text, start, end);
        vec_add(text, '\0');
        text_with_hint_t text_hint = {.text = text, .hint = hint, .start = start};
        vec_add(hints, text_hint);

        last_end = end;
    }

    return hints;
}

int edutil_render_lines(editor_t *editor, const render_options_t *options, render_command_t **render_commands, int line_number_width) {
    int y = options->area_y;
    for(int index = editor->scroll_start;index < vec_length(editor->lines);index++) {
        line_buffer_t line_buffer = editor->lines[index];

        int x = options->area_x + line_number_width;

        text_with_hint_t *hints = edutil_split_highlight(&line_buffer);
        for(int i = 0;i < vec_length(hints);i++){
            int x_shift = x + options->text_width(line_buffer.text, hints[i].start+1, editor->font, options->font_size);
            x_shift -= options->text_width(line_buffer.text+hints[i].start, 1, editor->font, options->font_size);

            render_command_t command = {
                .type = TEXT,
                .render_hint = hints[i].hint,
                .as.text = {
                    .x = x_shift,
                    .y = y,
                    .text = hints[i].text,
                    .font = editor->font,
                    .font_size = options->font_size
                }
            };
            vec_add(*render_commands, command);

        }

        y += options->font_size + options->line_margin;
        if(y > options->area_y+options->area_height) {
            break;
        }
    }

    return y;
}

render_command_t* editor_start_render(editor_t *editor, const render_options_t* options) {
    render_command_t *render_commands = {0};

    int line_number_width = edutil_line_number_width(editor, options);
    if(options->line_numbers) {
        edutil_render_line_numbers(editor, options, &render_commands);
    }

    int y = edutil_render_lines(editor, options, &render_commands, line_number_width);

    editor->last_size = options->area_height / (options->font_size + options->line_margin);

    vec_for_each_ptr(cursor_t *cursor, editor->cursors) {
        if(cursor->y_start-editor->scroll_start > y && cursor->y_end-editor->scroll_start > y) continue;
        int x = options->area_x + line_number_width;

        int x_start, y_start, x_end, y_end;
        edutil_cursor_sorted(cursor, &x_start, &y_start, &x_end, &y_end);

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
        if(!edutil_cursor_hselection(cursor)) {
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
        }

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

void editor_stop_render(editor_t *editor, render_command_t* render_commands) {
    vec_for_each_cpy(render_command_t command, render_commands) {
        if(command.type == TEXT) {
            vec_free(command.as.text.text);
        }
    }
    vec_free(render_commands);
}

// ++Rollback

void rollback_insert(editor_t *editor, action_insert_t action);
cursor_t rollback_compound(editor_t *editor, action_compound_t action);
void rollback_delete(editor_t* editor, action_delete_t delete, cursor_t after);
void rollback_delete_lines(editor_t* editor, action_delete_line_t delete);
void rollback_newline(editor_t* editor, action_newline_t newline);
void rollback_move_line(editor_t *editor, action_move_line_t move_line, cursor_t after);

cursor_t rollback_action(editor_t *editor, editor_action_t action) {
    cursor_t cursor = action.before;
    switch(action.type){
        case ACTION_INSERT: {
            rollback_insert(editor, action.as.insert);
            break;
        }
        case ACTION_COMPOUND: {
            cursor = rollback_compound(editor, action.as.compound);
            break;
        }
        case ACTION_DELETE: {
            rollback_delete(editor, action.as.delete, action.after);
            break;
        }
        case ACTION_DELETE_LINE: {
            rollback_delete_lines(editor, action.as.delete_line);
            break;
        }
        case ACTION_NEWLINE: {
            rollback_newline(editor, action.as.newline);
        }
        case ACTION_MOVE_LINE: {
            rollback_move_line(editor, action.as.move_line, action.after);
        }
        default: {
            break;
        }
    }

    return cursor;
}

void rollback_insert(editor_t *editor, action_insert_t action){
    cursor_t cursor = {.x_start = action.x_start, .y_start = action.y_start, .x_end = action.x_end, .y_end = action.y_end};

    int x_from, y_from, x_to, y_to;
    edutil_cursor_sorted(&cursor, &x_from, &y_from, &x_to, &y_to);

    printf("%d:%d:%d:%d\n", x_from, y_from, x_to, y_to);
    char* cs = edutil_remove_block(editor, x_from, y_from, x_to, y_to);
    vec_free(cs);
}

cursor_t rollback_compound(editor_t *editor, action_compound_t action) {
    printf("Compound %zu \n", vec_length(action.actions));
    for(int i = vec_length(action.actions)-1;i >= 0;i--){
        rollback_action(editor, action.actions[i]);
    }
    return action.actions[0].before;
}

void rollback_delete(editor_t* editor, action_delete_t delete, cursor_t after) {
    if(delete.type == DELETE_SINGLE) {
        line_buffer_t *line = &editor->lines[after.y_start];
        if(delete.c == '\n') {
            line_buffer_t newline = edutil_split_line_at(editor, line, after.x_start);
            edutil_update_coloring(editor, &newline);
            vec_insert(editor->lines, after.y_start+1, newline);
        } else {
            vec_insert(line->text, after.x_start, delete.c);
        }
        edutil_update_coloring(editor, line);
    }else {
        int x_from, y_from, x_to, y_to;
        edutil_cursor_sorted(&after, &x_from, &y_from, &x_to, &y_to);
        edutil_insert_block(editor, &after, delete.data, x_from, y_from, x_to, y_to, false, false);
    }
}

void rollback_delete_lines(editor_t* editor, action_delete_line_t delete) {
    for(int j = vec_length(delete.lines)-1;j >= 0;j--){
        if(delete.index == vec_length(editor->lines)-1 && j == vec_length(delete.lines)-1) {
            editor->lines[delete.index] = delete.lines[j];
        } else {
            vec_insert(editor->lines, delete.index, delete.lines[j]);
        }
        edutil_update_coloring(editor, &editor->lines[delete.index]);
    }
}

void rollback_newline(editor_t* editor, action_newline_t newline) {
    line_buffer_t *line = edutil_line_get(editor, newline.line);

    if(newline.splitline) {
        line_buffer_t *before = edutil_line_get(editor, newline.line-1);
        edutil_concat_lines(editor, before, line);
    }

    vec_remove(editor->lines, newline.line);
}

void rollback_move_line(editor_t *editor, action_move_line_t move_line, cursor_t after) {
    int _x_from, y_from, _x_to, y_to;
    edutil_cursor_sorted(&after, &_x_from, &y_from, &_x_to, &y_to);

    edutil_move_block(editor, y_from, y_to, -move_line.direction, move_line.amount);
}

void editor_rollback(editor_t *editor) {
    if(vec_empty(editor->actions)) {
        return;
    }

    editor_action_t action = vec_pop(editor->actions);

    editor->cursors[0] = rollback_action(editor, action);
}


#undef dir_cond
#undef sep_cond

#endif // EDITOR_IMPLEMENTATION

#undef CONCAT
#undef EXPAND_AND_CONCAT
#undef NAME
#undef cursor_t
#undef word_coloring_t
#undef line_coloring_t
#undef line_buffer_t
#undef editor_timer_t
#undef editor_t
#undef render_type_t
#undef render_text_t
#undef render_rect_t
#undef render_data_t
#undef render_data_t
#undef render_hint_t
#undef render_command_t
#undef init_options_t
#undef render_options_t
#undef editor_cursor_mright
#undef editor_cursor_mleft
#undef editor_cursor_mdown
#undef editor_cursor_mup
#undef editor_create
#undef editor_acursor_insert
#undef editor_acursor_delete
#undef editor_acursor_word_delete
#undef editor_acursor_newline
#undef editor_start_select
#undef editor_stop_select
#undef editor_start_render
#undef editor_acursor_insert_block
#undef editor_get_selection
#undef editor_cursor_mendline
#undef editor_cursor_mstartline
#undef editor_acursor_line_dublicate
#undef editor_acursor_line_move
#undef editor_acursor_line_delete
#undef editor_cursor_mword
#undef editor_mouse_to_cursor
#undef editor_rollback
#undef editor_update_timers
#undef editor_select_all
#undef editor_acursor_indent
#undef editor_acursor_unindent

#endif //COMP_EDITOR_HEADER_
