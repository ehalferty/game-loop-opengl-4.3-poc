//
// Created by phaz on 2/13/2020.
//

#ifndef GAME_LOOP_OPENGL_4_3_POC_MAIN_H
#define GAME_LOOP_OPENGL_4_3_POC_MAIN_H

#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <cstdio>
#include <io.h>
#include <cmath>
#include <fcntl.h>
#include "openjdk_8_jdk_includes/jni.h"
#include "unzip.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define GLFUNC(RETTYP, ARGTYPES, NAME) (RETTYP (*)ARGTYPES)wglGetProcAddress(NAME)
#define DLLFUNC(RETTYP, ARGTYPES, DLL, NAME) (RETTYP (*)ARGTYPES)GetProcAddress(DLL, NAME)
#define KEYPRESSED(scancode) (keyboardState[scancode] >> 7) == 0 && (previousKeyboardState[scancode] >> 7) != 0
#define WM_WINDOWED (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define WM_BORDERLESS_WINDOWED (WS_POPUP | WS_VISIBLE)
#define WM_CHANGE_WINDOW_MODE WM_USER
#define WINDOW_MODE_WINDOWED 1
#define WINDOW_MODE_BORDERLESS_WINDOWED 2
#define WINDOW_MODE_FULLSCREEN 3
#define JRE_PATH ".\\OpenJDK8U-jre_x64_windows_hotspot_8u242b08\\jdk8u242-b08-jre\\bin\\server\\jvm.dll"
#define LIBZIP_PATH ".\\libzip\\zip.dll"
const int numOverlayPoints = 6;
GLfloat testTextureCoordinates[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f};
GLfloat testPoints[] = {
        -1.0f, -0.5f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        -1.0f, -0.5f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f
};
HWND window;
MSG message = {};
JavaVM *jvm = nullptr;
JNIEnv *env = nullptr;
jmethodID gameMain = nullptr;
HDC deviceContext;
HGLRC openGLRenderingContext;
POINT windowSize = { 4, 3 };
POINT fullScreen = { 4, 3 };
BOOL windowVisible = FALSE;
INT currentWindowStyle = WINDOW_MODE_WINDOWED;
DEVMODE screenSettings;
MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
BOOL perspectiveChanged = FALSE;
BOOL active = FALSE;
BOOL done = FALSE;
INT xTemp = 0, yTemp = 0, filled = 0;
INT windowStyleWindowed = 0;


GLuint overlayVAO;
GLuint overlayPointsVBO;
GLuint overlayTextureVBO;
GLuint overlayTexture;
GLuint overlayVertexShader;
GLuint overlayFragmentShader;
GLuint overlayShaderProgram;

struct FileReadResult {
    LPCTSTR data;
    size_t size;
};
INT windowMode = WINDOW_MODE_WINDOWED, oldWindowMode = WINDOW_MODE_WINDOWED;
CHAR previousKeyboardState[256];
CHAR keyboardState[256];
const char * vertex_shader = R"""(
    #version 410
    layout(location = 0) in vec3 vertex_position;
    layout(location = 1) in vec2 vertex_texture;
    out vec2 texture_coordinates;
    void main() {
        texture_coordinates = vertex_texture;
        gl_Position = vec4(vertex_position, 1.0);
    }
)""";
const char * fragment_shader = R"""(
    #version 450
    in vec2 texture_coordinates;
    layout (binding = 0) uniform sampler2D overlay_texture;
    out vec4 frag_color;
    void main() {
        frag_color = texture(overlay_texture, texture_coordinates);
    }
)""";

class Sprite {
public:
    CHAR name[100];
    INT width;
    INT height;
    FLOAT atlasX;
    FLOAT atlasY;
    FLOAT atlasW;
    FLOAT atlasH;
    INT bpp;
    INT bufferSize;
    PBYTE buffer;
    VOID loadImageFile(const char *fileName);
};
int numSprites = 0;
Sprite sprites[100];

class Widget {
public:
    CHAR name[100];
    FLOAT x;
    FLOAT y;
    FLOAT width;
    FLOAT height;
    INT numSpritesInWidget;
    CHAR spriteNames[100][100];
    INT spriteIndexes[100];
    GLfloat spritesPoints[100][18];
    GLfloat spritesTextureCoordinates[100][12];
    VOID draw();
};
int numWidgets = 0;
Widget widgets[100];

// OpenGL functions
void (*glBindBuffer)(GLenum target, GLuint buffer);
void (*glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
void (*glBindVertexArray)(GLuint array);
GLuint (*glCreateShader)(GLenum shaderType);
void (*glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
void (*glCompileShader)(GLuint shader);
GLuint (*glCreateProgram)();
void (*glAttachShader)(GLuint program, GLuint shader);
void (*glLinkProgram)(GLuint program);
void (*glUseProgram)(GLuint program);
void (*glActiveTexture)(GLenum texture);
GLint (*glGetUniformLocation)(GLuint program, const GLchar *name);
void (*glUniform1i)(GLint location, GLint v0);
void (*glCreateBuffers)(GLsizei n, GLuint *buffers);
void (*glNamedBufferData)(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
void (*glCreateVertexArrays)(GLsizei n, GLuint *arrays);
void (*glEnableVertexArrayAttrib)(GLuint vaobj, GLuint index);
void (*glVertexArrayVertexBuffer)(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
void (*glVertexArrayAttribFormat)(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized,
                                  GLuint relativeoffset);
void (*glVertexArrayAttribBinding)(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
void (*glCreateTextures)(GLenum target, GLsizei n, GLuint *textures);
void (*glTextureStorage2D)(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void (*glTextureSubImage2D)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                            GLenum format,GLenum type, const void *pixels);
void (*glTextureParameteri)(GLuint texture, GLenum pname, GLint param);
void (*glBindTextureUnit)(GLuint unit, GLuint texture);

// JRE functions
void (*MyJNI_GetDefaultJavaVMInitArgs)(void *args);
jint (*MyJNI_CreateJavaVM)(JavaVM **pvm, void **penv, void *args);

// LibZip functions
//void *(*zip_source_buffer_create)(const void *data, uint64_t len, int freep, zip_error_t *error)
//zip_source_t *
//zip_source_buffer_create(const void *data, zip_uint64_t len, int freep, zip_error_t *error);


#endif //GAME_LOOP_OPENGL_4_3_POC_MAIN_H
