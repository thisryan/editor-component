CFLAGS := '-std=c99 -Wall -Werror -Wpedantic'

main:
    clang-19 demo/simple.c -o build/simple -I.  -lraylib -lm {{CFLAGS}}