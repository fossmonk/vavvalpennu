#include <shader.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

Shader shader_load_custom(const char *vs_fname, const char* fs_fname) {
    char *vs_header = "#version 330\n\n";
    char *fs_header = "#version 330\n\n";

    size_t vsh_size = strlen(vs_header);
    size_t fsh_size = strlen(fs_header);

    char *vs_text = NULL;
    char *fs_text = NULL;
    char *vs_buf = NULL;
    char *fs_buf = NULL;

    if(vs_fname != NULL) {
        vs_text = LoadFileText(vs_fname);
        size_t vst_size = strlen(vs_text);
        size_t alloc_size = vsh_size + vst_size + 8;
        vs_buf = calloc(alloc_size, sizeof(char));
        snprintf(vs_buf, alloc_size, "%s%s\n", vs_header, vs_text);
        UnloadFileText(vs_text);
    }

    if(fs_fname != NULL) {
        fs_text = LoadFileText(fs_fname);
        size_t fst_size = strlen(fs_text);
        size_t alloc_size = fsh_size + fst_size + 8;
        fs_buf = calloc(alloc_size, sizeof(char));
        snprintf(fs_buf, alloc_size, "%s%s\n", fs_header, fs_text);
        UnloadFileText(fs_text);
    }

    Shader shader = LoadShaderFromMemory(vs_buf, fs_buf);

    if(vs_buf != NULL) free(vs_buf);
    if(fs_buf != NULL) free(fs_buf);

    return shader;
}