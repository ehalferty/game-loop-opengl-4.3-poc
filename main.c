#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <gdiplus.h>
#include <cstdio>
#include <stdio.h>
#include <vector>
#include <io.h>
#include <fcntl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define GLFUNC(RETTYP, ARGTYPES, NAME) (RETTYP (*)ARGTYPES)wglGetProcAddress(NAME)
#define KEYPRESSED(scancode) (keyboardState[scancode] >> 7) == 0 && (previousKeyboardState[scancode] >> 7) != 0
#define WM_WINDOWED WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
#define WM_BORDERLESS_WINDOWED WS_POPUP | WS_VISIBLE
#define WM_CHANGE_WINDOW_MODE WM_USER
#define WINDOW_MODE_WINDOWED 1
#define WINDOW_MODE_BORDERLESS_WINDOWED 2
#define WINDOW_MODE_FULLSCREEN 3
const int numOverlayPoints = 6;
GLfloat overlayTextureCoordinates[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f};
HWND window;
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
GLuint vertexArrayObject;
GLuint vertexBufferObject;
GLuint vertexBufferObjectTexture;
GLfloat points[] = {
    0.0f, -0.5f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.5f, 0.0f, 0.0f,
    0.0f, -0.5f, 0.0f,
    0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f
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
    #version 410
    in vec2 texture_coordinates;
    uniform sampler2D overlay_texture;
    out vec4 frag_color;
    void main() {
        frag_color = texture(overlay_texture, texture_coordinates);
    }
)""";
GLuint vertexShader, fragmentShader;
GLuint shaderProgram;
void (*glGenBuffers)(GLsizei n, GLuint * buffers);
void (*glBindBuffer)(GLenum target, GLuint buffer);
void (*glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
void (*glGenVertexArrays)(GLsizei n, GLuint *arrays);
void (*glBindVertexArray)(GLuint array);
void (*glEnableVertexAttribArray)(GLuint index);
void (*glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                              const GLvoid * pointer);
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
void LoadOpenGLFunctions() {
    glGenBuffers = GLFUNC(void, (GLsizei, GLuint *), "glGenBuffers");
    glBindBuffer = GLFUNC(void, (GLenum, GLuint), "glBindBuffer");
    glBufferData = GLFUNC(void, (GLenum, GLsizeiptr, const GLvoid *, GLenum), "glBufferData");
    glGenVertexArrays = GLFUNC(void, (GLsizei, GLuint *), "glGenVertexArrays");
    glBindVertexArray = GLFUNC(void, (GLuint), "glBindVertexArray");
    glEnableVertexAttribArray = GLFUNC(void, (GLuint), "glEnableVertexAttribArray");
    glVertexAttribPointer = GLFUNC(void, (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *),
                                   "glVertexAttribPointer");
    glCreateShader = GLFUNC(GLuint, (GLenum), "glCreateShader");
    glShaderSource = GLFUNC(void, (GLuint, GLsizei, const GLchar **, const GLint *), "glShaderSource");
    glCompileShader = GLFUNC(void, (GLuint), "glCompileShader");
    glCreateProgram = GLFUNC(GLuint, (), "glCreateProgram");
    glAttachShader = GLFUNC(void, (GLuint, GLuint), "glAttachShader");
    glLinkProgram = GLFUNC(void, (GLuint), "glLinkProgram");
    glUseProgram = GLFUNC(void, (GLuint), "glUseProgram");
    glActiveTexture = GLFUNC(void, (GLenum), "glActiveTexture");
    glGetUniformLocation = GLFUNC(GLint, (GLuint, const GLchar *), "glGetUniformLocation");
    glUniform1i = GLFUNC(void, (GLint, GLint), "glUniform1i");
}
LONG WINAPI WindowProc(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static PAINTSTRUCT paintStruct;
    INT i = 0;
    BOOL found = FALSE;
    switch (uMsg) {
    case WM_PAINT:
        // Do this to mark window as not dirty, so we don't get WM_PAINT messages all the time.
        // TODO: Try removing this explicit WM_PAINT handler.
        BeginPaint(window, &paintStruct);
        EndPaint(window, &paintStruct);
        return 0;
    case WM_TIMER:
        switch (wParam) {
            case 0:
                printf("Timer 0\r\n");
                return 0;
        }
    case WM_SIZE:
        windowSize.x = LOWORD(lParam);
        windowSize.y = HIWORD(lParam);
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        PostMessage(window, WM_PAINT, 0, 0);
        perspectiveChanged = TRUE;
        return 0;
    case WM_CHANGE_WINDOW_MODE:
        oldWindowMode = windowMode;
        windowMode = wParam;
        currentWindowStyle = GetWindowLongPtr(window, GWL_STYLE);
        if (windowMode == WINDOW_MODE_WINDOWED) {
            if (currentWindowStyle != windowStyleWindowed) {
                SetWindowLongPtr(window, GWL_STYLE, windowStyleWindowed);
                SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0,  SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
            }
            SetWindowPos(window, 0, 0, 0, 800, 600, SWP_SHOWWINDOW);
            if (oldWindowMode == WINDOW_MODE_FULLSCREEN) {
                ChangeDisplaySettings(NULL, 0);
            }
        } else if (windowMode == WINDOW_MODE_BORDERLESS_WINDOWED) {
            if (currentWindowStyle != WM_BORDERLESS_WINDOWED) {
                SetWindowLongPtr(window, GWL_STYLE, WM_BORDERLESS_WINDOWED);
                SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0,  SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
            }
            if (oldWindowMode == WINDOW_MODE_FULLSCREEN) {
                ChangeDisplaySettings(NULL, 0);
            }
            fullScreen.x = (int)GetSystemMetrics(SM_CXSCREEN);
            fullScreen.y = (int)GetSystemMetrics(SM_CYSCREEN);
            if (windowSize.x != fullScreen.x && windowSize.y != fullScreen.y) {
                SetWindowPos(window, 0, 0, 0, fullScreen.x, fullScreen.y, SWP_SHOWWINDOW);
            }
        } else if (windowMode == WINDOW_MODE_FULLSCREEN) {
            // TODO: Is this really exclusive fullscreen?
            // TODO: See if there's input latency.
            // TODO: Figure out how to get the removed resolution-changing code to work (driver issue?!)
            if (currentWindowStyle != WM_BORDERLESS_WINDOWED) {
                SetWindowLongPtr(window, GWL_STYLE, WM_BORDERLESS_WINDOWED);
            }
            GetMonitorInfo(MonitorFromWindow(window ,MONITOR_DEFAULTTOPRIMARY), &monitorInfo);
            SetWindowLongPtr(window, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
            SetWindowPos(window, HWND_TOPMOST, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                         monitorInfo.rcMonitor.right  - monitorInfo.rcMonitor.left,
                         monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            InvalidateRect(window, NULL, TRUE);
            PostMessage(window, WM_PAINT, 0, 0);
        }
        PostMessage(window, WM_PAINT, 0, 0);
        return 0;
    case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER) {
            return 0;
        }
        break;
    case WM_ACTIVATE:
        active = !HIWORD(wParam);
        return 0;
    case WM_SETFOCUS:
        if (windowMode == WINDOW_MODE_FULLSCREEN) {
            SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
        }
        return 0;
    case WM_KILLFOCUS:
        if (windowMode == WINDOW_MODE_FULLSCREEN) {
            SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
        }
        return 0;
    // TODO: Keyboard input mode (when menus open)
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        return 0;
    case WM_LBUTTONDOWN:
        xTemp = GET_X_LPARAM(lParam);
        yTemp = GET_Y_LPARAM(lParam);
        if (filled == 0 || filled == 4) {
            points[0] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
            points[1] = 1.0f - (yTemp / (float)windowSize.y) * 2;
            points[9] = points[0];
            points[10] = points[1];
            filled = 1;
        } else if (filled == 1) {
            points[3] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
            points[4] = 1.0f - (yTemp / (float)windowSize.y) * 2;
            filled = 2;
        } else if (filled == 2) {
            points[6] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
            points[7] = 1.0f - (yTemp / (float)windowSize.y) * 2;
            points[12] = points[6];
            points[13] = points[7];
            filled = 3;
        } else if (filled == 3) {
            points[15] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
            points[16] = 1.0f - (yTemp / (float)windowSize.y) * 2;
            filled = 4;
        }
        return 0;
    case WM_CLOSE:
        if (windowMode == WINDOW_MODE_FULLSCREEN) {
            ChangeDisplaySettings(NULL, 0);
        }
        PostQuitMessage(0);
        done = TRUE;
        return 0;
    default:
        break;
    }
    return (LONG)DefWindowProc(window, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    // Need GDI+ to parse image files
    // TODO: Now that textures work with stb_image, would this work instead?
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    Gdiplus::Bitmap* image = new Gdiplus::Bitmap(L"C:\\Users\\phaz\\game-loop-opengl-4.3-poc\\kitten.jpg");
    int w = (int)image->GetWidth();
    int h = (int)image->GetHeight();
    Gdiplus::Rect rc(0, 0, w, h);
    Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
    image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);
    std::vector<uint32_t> col;
    col.resize(w * h);
    for (int y = 0; y < h; y++) {
        memcpy(&col[y * w], (char*)bitmapData->Scan0 + bitmapData->Stride * y, w * 4);
        for (int x = 0; x < w; x++) {
            uint32_t& c = col[y * w + x];
            c = (c & 0xff00ff00) | ((c & 0xff) << 16) | ((c & 0xff0000) >> 16);
        }
    }
    image->UnlockBits(bitmapData);
    delete bitmapData;
    delete image;
    Gdiplus::GdiplusShutdown(gdiplusToken);
    // Setup a console for logging
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
    printf("Hello, party people!\r\n");
    MSG   message;
    WNDCLASS wc;
    int pf;
    PIXELFORMATDESCRIPTOR pfd;
    hInstance = GetModuleHandle(NULL);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Test123";
    RegisterClass(&wc);
    window = CreateWindow("Test123", "Hi there", WM_WINDOWED, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
    windowStyleWindowed = GetWindowLongPtr(window, GWL_STYLE);
    deviceContext = GetDC(window);
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pf = ChoosePixelFormat(deviceContext, &pfd);
    SetPixelFormat(deviceContext, pf, &pfd);
    DescribePixelFormat(deviceContext, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    ReleaseDC(window, deviceContext);
    deviceContext = GetDC(window);
    openGLRenderingContext = wglCreateContext(deviceContext);
    wglMakeCurrent(deviceContext, openGLRenderingContext);
    ShowWindow(window, nCmdShow);
    LoadOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Vertex Position VBO
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, numOverlayPoints * 3 * sizeof (GLfloat), points, GL_STATIC_DRAW);
    // Texture VBO
    glGenBuffers(1, &vertexBufferObjectTexture);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectTexture);
    glBufferData(GL_ARRAY_BUFFER, numOverlayPoints * 2 * sizeof(GLfloat), overlayTextureCoordinates, GL_STATIC_DRAW);
    // VAO
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectTexture);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    GLuint overlayTexture;
    glGenTextures(1, &overlayTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, overlayTexture);

    unsigned char *buffer;
    int bpp, width, height;
    buffer = stbi_load("kitten.png", &width, &height, &bpp, 0);
    printf("width=%d, height=%d, bpp=%d\r\n", width, height, bpp);
    if (!buffer) {
        MessageBox(0, "Uh... failed", "", 0);
    }
    unsigned char *buffer2 = (unsigned char *)malloc(100 * 100 * 4);
    for (int ii = 0; ii < 100 * 100; ii++) {
        buffer2[ii * 4 + 0] = 0x00;
        buffer2[ii * 4 + 1] = ii * 10;
        buffer2[ii * 4 + 2] = ii * 10;
        buffer2[ii * 4 + 3] = 0x00;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    glLinkProgram(shaderProgram);
    SetTimer(window, 0, 10000, (TIMERPROC) NULL);
    while (!done && message.message != WM_QUIT) {
        if(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } else {
            for (INT i = 0; i < 256; i++) {
                previousKeyboardState[i] = keyboardState[i];
            }
            GetKeyboardState((PBYTE)keyboardState);
            // TODO: Game engine stuff here
            // Check if a key was pressed which changes the window mode
            if (KEYPRESSED(0x31) && windowMode != 1) {
                SendMessage(window, WM_CHANGE_WINDOW_MODE, WINDOW_MODE_WINDOWED, 0);
            } else if (KEYPRESSED(0x32) && windowMode != 2) {
                SendMessage(window, WM_CHANGE_WINDOW_MODE, WINDOW_MODE_BORDERLESS_WINDOWED, 0);
            } else if (KEYPRESSED(0x33) && windowMode != 3) {
                SendMessage(window, WM_CHANGE_WINDOW_MODE, WINDOW_MODE_FULLSCREEN, 0);
            } else if (KEYPRESSED(0x51) && windowMode != 3) {
                SendMessage(window, WM_CLOSE, 0, 0);
            }
            // Make OpenGL calls
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
            glBufferData(GL_ARRAY_BUFFER, numOverlayPoints * 3 * sizeof (GLfloat), points, GL_STATIC_DRAW);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glUseProgram(shaderProgram);
            int textureLocation = glGetUniformLocation(shaderProgram, "overlay_texture");
            glUniform1i(textureLocation, 0);

            // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 100, 100, GL_RGBA, GL_UNSIGNED_BYTE, buffer2);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            glBindVertexArray(vertexArrayObject);
            glDrawArrays(GL_TRIANGLES, 0, numOverlayPoints);
            glFlush();
            SwapBuffers(deviceContext);
            // TODO: Calculate framerate
            // This must be smaller than the input delay to register key presses.
            // TODO: Should use timers so not all game updates happen at this high speed
            Sleep(10);
        }
    }
    wglMakeCurrent(NULL, NULL);
    ReleaseDC(window, deviceContext);
    wglDeleteContext(openGLRenderingContext);
    DestroyWindow(window);
    return (int)message.wParam;
}
