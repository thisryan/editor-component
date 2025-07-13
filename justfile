CFLAGS := '-std=c99 -Wall -Werror -Wpedantic'

main:
    clang-19 demo/simple.c -o build/simple -I.  -lraylib -lm {{CFLAGS}}

one_line:
    clang-19 demo/one_line.c -o build/one_line -I.  -lraylib -lm {{CFLAGS}}

one_lineDebug:
    clang-19 demo/one_line.c -o build/one_line -I.  -lraylib -lm {{CFLAGS}} -g -fsanitize=address

mainDebug:
    clang-19 demo/simple.c -o build/simple -I.  -lraylib -lm {{CFLAGS}} -g -fsanitize=address