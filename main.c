/**
 * @file: main.c
 * @description: Main function of the BF interpreter
 * @author: WAHAHA
 * @Date: 2024/8/7 下午12:40
 * @FilePath: bf_runner//main.c
 * @category: 
 */

#include <stdio.h>
#include "bf.h"
#include "command.h"
#include "debug_sig.h"

int main(int argc, char *argv[]) {
    /* Check parameters */
    if (argc < 2) {
#if DEBUG_SIG >= DEBUG_SIG_RUN
        printf("[ERROR] Usage: %s <BF code file> [input file] [output file]\n", argv[0]);
#endif
        return 1;
    }

    /* Initialize BF environment */
    files_t files = parse_params(argc, argv);
    bf_ctx_t *ctx = new_ctx(files.input_file, files.output_file);
    ctx_bind_code(ctx, files.code_file);

    /* Run BF code */
#if DEBUG_SIG >= DEBUG_SIG_INFO
    printf("[INFO] Running BF code...\n");
#endif
    if (files.output_file == NULL) {
        printf("========================================\n");
    }
    int result = run_bf(ctx);
    if (files.output_file == NULL) {
        printf("\n========================================\n");
    }
#if DEBUG_SIG >= DEBUG_SIG_INFO
    printf("[INFO] BF code exits with code %d\n", result);
#endif

    /* Free resources */
    fclose(files.code_file);
    fclose(files.input_file);
    fclose(files.output_file);
    free_ctx(ctx);

    return 0;
}