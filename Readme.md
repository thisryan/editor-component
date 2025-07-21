# A reusable editor component

A reusable editor component as single header library which is agnostic of the platform it is used on.

Demos can be found in the `demos` folder

## Functionality

Currently supported function:
```c
// ++Cursor

editor_cursor_mright+
editor_cursor_mleft+
editor_cursor_mdown+
editor_cursor_mup+
editor_cursor_mto+
editor_cursor_mendline+
editor_cursor_mstartline+
editor_cursor_mword+
editor_select_all+

// ++Organization

editor_create+
editor_cleanup+
editor_update_timerss

// ++Mutation

editor_acursor_insert+#
editor_acursor_delete+#
editor_acursor_newline+#
editor_acursor_insert_block+#
editor_acursor_line_dublicate+#
editor_acursor_line_move+
editor_acursor_line_delete+#
editor_acursor_word_delete+#
editor_start_select+#
editor_stop_select+#
editor_set_select+#
editor_acursor_indent+#
editor_acurosr_unindent+#

// ++Information

editor_mouse_to_cursor+
editor_get_selection+

// ++Rendering

edutil_render_command_from_line+
edutil_render_line_numbers+
edutil_next_coloring+
edutil_split_highlight+
edutil_render_lines+
editor_start_render+
editor_stop_render+

// ++Rollback

rollback_insert
rollback_compound
rollback_delete
rollback_delete_lines
rollback_action
editor_rollback
```
