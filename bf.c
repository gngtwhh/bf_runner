/**
 * @file: load.c
 * @description: This is a source file that contains the implementation of xxx
 * @author: WAHAHA
 * @Date: 2024/8/7 下午12:45
 * @FilePath: bf_runner//load.c
 * @category: 
 */

#include <string.h>
#include <conio.h>
#include "bf.h"
#include "debug_sig.h"

bf_ctx_t *new_ctx(FILE *input_file, FILE *output_file) {
    // fill with 0
    bf_ctx_t *ctx = (bf_ctx_t *) calloc(1, sizeof(bf_ctx_t));
    ctx->array = (byte *) calloc(1, sizeof(char) * MAX_MEM_SIZE);
    ctx->arr_size = MAX_MEM_SIZE;
//    ctx->ptr = 0;
//    ctx->code_len = 0;
//    ctx->code =NULL;
//    ctx->input = ctx->output = NULL;

    if (input_file != NULL) {
        ctx->input = input_file;
    } else {
#if DEBUG_SIG >= DEBUG_SIG_INFO
        printf("[INFO] Using stdin\n");
#endif
    }
    if (output_file != NULL) {
        ctx->output = output_file;
    } else {
#if DEBUG_SIG >= DEBUG_SIG_INFO
        printf("[INFO] Using stdout\n");
#endif
    }
    return ctx;
}

bf_ctx_t *copy_ctx(bf_ctx_t *old_ctx) {
    bf_ctx_t *new_one = (bf_ctx_t *) malloc(sizeof(bf_ctx_t));
    *new_one = *old_ctx;
    return new_one;
}

void free_ctx(bf_ctx_t *ctx) {
    if (ctx->input != NULL) {
        fclose(ctx->input);
    }
    if (ctx->output != NULL) {
        fclose(ctx->output);
    }
    free(ctx->array);
    free(ctx);
}

void ctx_bind_code(bf_ctx_t *ctx, FILE *code_file) {
    // alloc code array
    size_t code_size = 1024;
    ctx->code = (char *) malloc(sizeof(char) * code_size);
    // read code
    ctx->code_len = fread(ctx->code, sizeof(char), code_size, code_file);
    while (!feof(code_file)) {
        // expand code array to 2 times
        ctx->code = (char *) realloc(ctx->code, sizeof(char) * (code_size * 2));
        code_size *= 2;
        if (ctx->code == NULL) {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
            printf("[ERROR] Unable to allocate memory for code array\n");
#endif
            return;
        }
        // continue reading
        ctx->code_len += fread(ctx->code + ctx->code_len, sizeof(char), code_size - ctx->code_len, code_file);
    }
    // null terminate code array
    ctx->code[ctx->code_len] = '\0';
#if DEBUG_SIG >= DEBUG_SIG_DEBUG
    printf("[DEBUG] func: ctx_bind_code, info: code_len: %zu\n", ctx->code_len);
#endif
}

int run_bf(bf_ctx_t *ctx) {
    char *code = ctx->code;
    char command;
    while (ctx->code_ptr < ctx->code_len) {
        command = code[ctx->code_ptr];
#if DEBUG_SIG >= DEBUG_SIG_DEBUG
        printf("[DEBUG] func: run_bf, info: command: [%c] ascii:%d\n", command, command);
#endif
        interpret_command(ctx, command);
        if (ctx->status == BF_STATUS_ERROR) {
            return -1;
        }
        ctx->code_ptr++;
    }
    return (int) (ctx->array[ctx->ptr]);
}

void interpret_command(bf_ctx_t *ctx, char command) {
    switch (command) {
    case '>':
        vm_add_ptr(ctx);
        break;
    case '<':
        vm_sub_ptr(ctx);
        break;
    case '+':
        vm_add_val(ctx);
        break;
    case '-':
        vm_sub_val(ctx);
        break;
    case '.':
        vm_output(ctx);
        break;
    case ',':
        vm_input(ctx);
        break;
    case '[':
        vm_loop_start(ctx);
        break;
    case ']':
#if DEBUG_SIG >= DEBUG_SIG_ERROR
        printf("[ERROR] Unmatched \"]\" found at code position %zu\n", ctx->code_ptr);
#endif
        ctx->status = BF_STATUS_ERROR;
        break;
    case '\n': // ignore newlines
        break;
    default:
#if DEBUG_SIG >= DEBUG_SIG_ERROR
        printf("[ERROR] Invalid BF command: %c\n", command);
#endif
        ctx->status = BF_STATUS_ERROR;
    }
}

void vm_add_ptr(bf_ctx_t *ctx) {
    if (ctx->ptr < ctx->arr_size - 1) {
        ctx->ptr++;
    } else {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
        printf("[ERROR] Memory overflow, cannot increment pointer\n");
#endif
        ctx->status = BF_STATUS_ERROR;
    }
}

void vm_sub_ptr(bf_ctx_t *ctx) {
    if (ctx->ptr > 0) {
        ctx->ptr--;
    } else {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
        printf("[ERROR] Memory underflow, cannot decrement pointer\n");
#endif
        ctx->status = BF_STATUS_ERROR;
    }
}

void vm_add_val(bf_ctx_t *ctx) {
    if (ctx->array[ctx->ptr] < 255) {
        ctx->array[ctx->ptr]++;
    } else {
//        printf("[ERROR] Value overflow, cannot increment value\n");
//        ctx->status = BF_STATUS_ERROR;
        ctx->array[ctx->ptr] = 0; // wrap around
    }
}

void vm_sub_val(bf_ctx_t *ctx) {
    if (ctx->array[ctx->ptr] > 0) {
        ctx->array[ctx->ptr]--;
    } else {
//        printf("[ERROR] Value underflow, cannot decrement value\n");
//        ctx->status = BF_STATUS_ERROR;
        ctx->array[ctx->ptr] = 255; // wrap around
    }
}

void vm_output(bf_ctx_t *ctx) {
    if (ctx->output != NULL) {
        fputc(ctx->array[ctx->ptr], ctx->output);
    } else {
        putchar((char) (ctx->array[ctx->ptr]));
        fflush(stdout);
    }
}

void vm_input(bf_ctx_t *ctx) {
    int ch;
    if (ctx->input != NULL) {
        ch = fgetc(ctx->input);
    } else {
        ch = getch(); // get character from stdin without buffering
    }
    if (ch == EOF) {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
        printf("[ERROR] End of input file reached\n");
#endif
        ctx->status = BF_STATUS_ERROR;
    } else {
        ctx->array[ctx->ptr] = (byte) ch;
    }
}

void vm_loop_start(bf_ctx_t *ctx) {
    int cnt;
    /* find the matching ']' */
    size_t right_side = ctx->code_ptr;
    for (cnt = 1, right_side++; cnt != 0;) {
        if (right_side >= ctx->code_len) {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
            printf("[ERROR] Unmatched \"[\" found at code position %zu\n", ctx->code_ptr);
#endif
            ctx->status = BF_STATUS_ERROR;
            return;
        }
        if (ctx->code[right_side] == '[') {
            cnt++;
        } else if (ctx->code[right_side] == ']') {
            cnt--;
        }
        right_side++;
    }
    right_side--; // Locate the matching ']'

    /* check the condition */
    if (ctx->array[ctx->ptr] == 0) { // skip loop
        ctx->code_ptr = right_side;
    } else { // execute loop
        do {
            /* copy the current context */
            bf_ctx_t *cur_ctx = copy_ctx(ctx);
            cur_ctx->code_ptr = ctx->code_ptr + 1;
            cur_ctx->code = cur_ctx->code + cur_ctx->code_ptr;
            cur_ctx->code_len = right_side - cur_ctx->code_ptr;
            cur_ctx->code_ptr = 0; // reset the code pointer to 0 --- offset relative to the start of the loop
            /* run the loop */
            run_bf(cur_ctx); // run the loop
            /* copy the result back to the original context */
            ctx->ptr = cur_ctx->ptr;
            if (cur_ctx->status == BF_STATUS_ERROR) {
                ctx->status = BF_STATUS_ERROR;
            }
            /* free the copied context */
            free(cur_ctx); // just free the copied context obj, but don't free the context data(ctx still owns the data)
        } while (ctx->array[ctx->ptr] != 0);
        /* move the code pointer to the end of the loop */
        ctx->code_ptr = right_side;
    }
}