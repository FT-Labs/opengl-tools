#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

typedef struct Shader Shader;

struct Shader {
    GLuint vertex;
    GLuint fragment;
    GLuint program;
    char *vertex_code;
    char *fragment_code;
    void(*load)(Shader *, const char *, const char *);
    void(*destroy)(Shader *);
};

void shader_load(Shader *shader, const char *vertex_fp, const char *fragment_fp);
void shader_destroy(Shader *shader);

long file_size(FILE *fp)
{
    fseek(fp, 0, SEEK_END); /* Set pointer to end of file */
    long fsize = ftell(fp); /* get file size */
    rewind(fp); /* rewind pointer */
    return fsize;
}

Shader shader_init()
{
    Shader shader;
    shader.load = shader_load;
    shader.destroy = shader_destroy;
    return shader;
}


void shader_load(Shader *shader, const char *vertex_fp, const char *fragment_fp)
{
    /* load vertex file */
    FILE *vertex_stream;
    if (!(vertex_stream = fopen(vertex_fp, "r"))) {
        printf("Can't open vertex file");
        exit(EXIT_FAILURE);
    }

    long fsize = file_size(vertex_stream);
    int vlength = fsize;

    shader->vertex_code = malloc(fsize + 1); /* Set length */
    shader->vertex_code[fsize] = 0; /* Set size+1 to null */

    int i = 0;
    printf("Reading shader...\n");
    while ((shader->vertex_code[i] = fgetc(vertex_stream)) != EOF)
        i++;
    shader->vertex_code[i] = 0;
    fclose(vertex_stream);

    FILE *fragment_stream;
    if (!(fragment_stream = fopen(fragment_fp, "r"))) {
        printf("Can't open fragment file");
        exit(EXIT_FAILURE);
    }

    fsize = file_size(fragment_stream);
    int flength = fsize;

    shader->fragment_code = malloc(fsize + 1); /* Set length */
    shader->fragment_code[fsize] = 0; /* Set size+1 to null */

    i = 0;
    printf("Reading fragment...\n");
    while ((shader->fragment_code[i] = fgetc(fragment_stream)) != EOF)
        i++;
    shader->fragment_code[i] = 0;
    fclose(fragment_stream);

	/* Creating shader objects */
	if(!(shader->vertex = glCreateShader(GL_VERTEX_SHADER))){
		printf("Shaders: Error creating vertex shader object!\n");
	}

	if(!(shader->fragment = glCreateShader(GL_FRAGMENT_SHADER))){
		printf("Shaders: Error creating fragment shader object!\n");
	}

    const char *ptr_vertex = shader->vertex_code;
    const char *ptr_fragment = shader->fragment_code;

    /* Attach shaders */
    glShaderSource(shader->vertex, 1, &ptr_vertex, &vlength);
    glShaderSource(shader->fragment, 1, &ptr_fragment, &flength);

    /* Compile */
    printf("Compiling...\n");
    glCompileShader(shader->vertex);
    glCompileShader(shader->fragment);

    /* Link it */
    if (!(shader->program = glCreateProgram())) {
        printf("Creating program object failed.\n");
    }

    glAttachShader(shader->program, shader->vertex);
    glAttachShader(shader->program, shader->fragment);

    glLinkProgram(shader->program);

    free(shader->vertex_code);
    free(shader->fragment_code);
}

void shader_destroy(Shader *shader)
{
	glDetachShader(shader->vertex, shader->program);
	glDetachShader(shader->fragment, shader->program);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
	glDeleteProgram(shader->program);

	shader->load = NULL;
	shader->destroy = NULL;
}
