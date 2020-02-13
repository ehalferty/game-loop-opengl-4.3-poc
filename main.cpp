#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>
//#include <gdiplus.h>
//#include <vector>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define GLFUNC(RETTYP, ARGTYPES, NAME) (RETTYP (*)ARGTYPES)wglGetProcAddress(NAME)
#define KEYPRESSED(scancode) (keyboardState[scancode] >> 7) == 0 && (previousKeyboardState[scancode] >> 7) != 0
#define WM_WINDOWED (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define WM_BORDERLESS_WINDOWED (WS_POPUP | WS_VISIBLE)
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
    #version 450
    in vec2 texture_coordinates;
    layout (binding = 0) uniform sampler2D overlay_texture;
    out vec4 frag_color;
    void main() {
        frag_color = texture(overlay_texture, texture_coordinates);
    }
)""";
GLuint vertexShader, fragmentShader;
GLuint shaderProgram;
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
void LoadOpenGLFunctions() {
    glBindBuffer = GLFUNC(void, (GLenum, GLuint), "glBindBuffer");
    glBufferData = GLFUNC(void, (GLenum, GLsizeiptr, const GLvoid *, GLenum), "glBufferData");
    glBindVertexArray = GLFUNC(void, (GLuint), "glBindVertexArray");
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
    glCreateBuffers = GLFUNC(void, (GLsizei, GLuint *), "glCreateBuffers");
    glNamedBufferData = GLFUNC(void, (GLuint, GLsizeiptr, const void *, GLenum), "glNamedBufferData");
    glCreateVertexArrays = GLFUNC(void, (GLsizei, GLuint *), "glCreateVertexArrays");
    glEnableVertexArrayAttrib = GLFUNC(void, (GLuint, GLuint), "glEnableVertexArrayAttrib");
    glVertexArrayVertexBuffer = GLFUNC(void, (GLuint, GLuint, GLuint, GLintptr, GLsizei), "glVertexArrayVertexBuffer");
    glVertexArrayAttribFormat = GLFUNC(void, (GLuint, GLuint, GLint, GLenum, GLboolean, GLuint),
                                       "glVertexArrayAttribFormat");
    glVertexArrayAttribBinding = GLFUNC(void, (GLuint, GLuint, GLuint), "glVertexArrayAttribBinding");
    glCreateTextures = GLFUNC(void, (GLenum, GLsizei, GLuint *), "glCreateTextures");
    glTextureStorage2D = GLFUNC(void, (GLuint, GLsizei, GLenum, GLsizei, GLsizei), "glTextureStorage2D");
    glTextureSubImage2D = GLFUNC(void, (GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum,
                                 const void *), "glTextureSubImage2D");
    glTextureParameteri = GLFUNC(void, (GLuint, GLenum, GLint), "glTextureParameteri");
    glBindTextureUnit = GLFUNC(void, (GLuint, GLuint), "glBindTextureUnit");
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
            default:
                return 0;
        }
    case WM_SIZE:
        windowSize.x = LOWORD(lParam);
        windowSize.y = HIWORD(lParam);
        glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(window, nullptr, TRUE);
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
            SetWindowPos(window, nullptr, 0, 0, 800, 600, SWP_SHOWWINDOW);
            if (oldWindowMode == WINDOW_MODE_FULLSCREEN) {
                ChangeDisplaySettings(nullptr, 0);
            }
        } else if (windowMode == WINDOW_MODE_BORDERLESS_WINDOWED) {
            if (currentWindowStyle != WM_BORDERLESS_WINDOWED) {
                SetWindowLongPtr(window, GWL_STYLE, WM_BORDERLESS_WINDOWED);
                SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0,  SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
            }
            if (oldWindowMode == WINDOW_MODE_FULLSCREEN) {
                ChangeDisplaySettings(nullptr, 0);
            }
            fullScreen.x = (int)GetSystemMetrics(SM_CXSCREEN);
            fullScreen.y = (int)GetSystemMetrics(SM_CYSCREEN);
            if (windowSize.x != fullScreen.x && windowSize.y != fullScreen.y) {
                SetWindowPos(window, nullptr, 0, 0, fullScreen.x, fullScreen.y, SWP_SHOWWINDOW);
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
            InvalidateRect(window, nullptr, TRUE);
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
            ChangeDisplaySettings(nullptr, 0);
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
//    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
//    ULONG_PTR gdiplusToken;
//    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
//    Gdiplus::Bitmap* image = new Gdiplus::Bitmap(L"C:\\Users\\phaz\\game-loop-opengl-4.3-poc\\kitten.jpg");
//    int w = (int)image->GetWidth();
//    int h = (int)image->GetHeight();
//    Gdiplus::Rect rc(0, 0, w, h);
//    Gdiplus::BitmapData* bitmapData = new Gdiplus::BitmapData;
//    image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, bitmapData);
//    std::vector<uint32_t> col;
//    col.resize(w * h);
//    for (int y = 0; y < h; y++) {
//        memcpy(&col[y * w], (char*)bitmapData->Scan0 + bitmapData->Stride * y, w * 4);
//        for (int x = 0; x < w; x++) {
//            uint32_t& c = col[y * w + x];
//            c = (c & 0xff00ff00) | ((c & 0xff) << 16) | ((c & 0xff0000) >> 16);
//        }
//    }
//    image->UnlockBits(bitmapData);
//    delete bitmapData;
//    delete image;
//    Gdiplus::GdiplusShutdown(gdiplusToken);
    // Setup a console for logging
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, nullptr, _IONBF, 1);
    *stdout = *hf_out;
    printf("Hello, party people!\r\n");



    // Load JRE
    // SetCurrentDirectory(R"(C:\Program Files\Java\jre1.8.0_241\bin\server)");
    // SetCurrentDirectory(R"(C:\Users\phaz\Documents\OpenJDK8U-jre_x64_windows_hotspot_8u242b08\jdk8u242-b08-jre\bin\server)");
    // // HINSTANCE jre = LoadLibrary("jvm.dll");
    // HINSTANCE jre = LoadLibrary("jvm.dll");

    // C:\Users\phaz\game-loop-opengl-4.3-poc\OpenJDK8U-jre_x64_windows_hotspot_8u242b08\jdk8u242-b08-jre\bin\server

    // SetCurrentDirectory(R"(C:\Users\phaz\game-loop-opengl-4.3-poc\OpenJDK8U-jre_x64_windows_hotspot_8u242b08\jdk8u242-b08-jre\bin\server)");
    HINSTANCE jre = LoadLibrary(".\\OpenJDK8U-jre_x64_windows_hotspot_8u242b08\\jdk8u242-b08-jre\\bin\\server\\jvm.dll");
    // HINSTANCE jre = LoadLibrary(R"(".\OpenJDK8U-jre_x64_windows_hotspot_8u242b08\jdk8u242-b08-jre\bin\server\jvm.dll")");
    // C:\Program Files (x86)\Java\jre1.8.0_241\bin\client
    if (!jre) {
        printf("jre error=%d\r\n", GetLastError());
    }
//    HINSTANCE jre = LoadLibrary(R"(C:\Users\phaz\game-loop-opengl-4.3-poc\java8_32bit.dll)");
//    FARPROC JNI_CreateJavaVM = GetProcAddress(jre, "JNI_CreateJavaVM");
//    printf("JNI_CreateJavaVM=%X\r\n", (size_t)JNI_CreateJavaVM);
//    HKEY hKey;
//    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\JavaSoft\\Java Runtime Environment", 0, KEY_READ, &hKey);

//    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Perl", 0, KEY_READ, &hKey);
//    HINSTANCE jre = LoadLibrary("C:\\Documents and Settings\\User\\Desktop  \\fgfdg\\dgdg\\test.dll");


    // win32 windows setup stuff
    MSG message = {};
    WNDCLASS wc = {};
    int pf;
    PIXELFORMATDESCRIPTOR pfd = {};
    hInstance = GetModuleHandle(nullptr);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "Test123";
    RegisterClass(&wc);
    window = CreateWindow("Test123", "Hi there", WM_WINDOWED, 0, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);
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
    glCreateBuffers(1, &vertexBufferObject);
    glNamedBufferData(vertexBufferObject, numOverlayPoints * 3 * sizeof (GLfloat), points, GL_STATIC_DRAW);
    // Texture VBO
    glCreateBuffers(1, &vertexBufferObjectTexture);
    glNamedBufferData(vertexBufferObjectTexture, numOverlayPoints * 2 * sizeof (GLfloat), overlayTextureCoordinates, GL_STATIC_DRAW);
    // VAO
    glCreateVertexArrays(1, &vertexArrayObject);
    glEnableVertexArrayAttrib(vertexArrayObject, 0);
    glEnableVertexArrayAttrib(vertexArrayObject, 1);
    glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBufferObject, 0, 12);
    glVertexArrayVertexBuffer(vertexArrayObject, 1, vertexBufferObjectTexture, 0, 8);
    glVertexArrayAttribBinding(vertexArrayObject, 0, 0);
    glVertexArrayAttribBinding(vertexArrayObject, 1, 1);
    glVertexArrayAttribFormat(vertexArrayObject, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vertexArrayObject, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glBindVertexArray(vertexArrayObject);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    unsigned char *buffer;
    int bpp, width, height;
    buffer = stbi_load(R"(C:\Users\phaz\game-loop-opengl-4.3-poc\kitten.png)", &width, &height, &bpp, 0);
    printf("width=%d, height=%d, bpp=%d\r\n", width, height, bpp);
    if (!buffer) {
        MessageBox(nullptr, "FAILED TO LOAD THE KITTEN", "", 0);
    }
    GLuint overlayTexture;
    glCreateTextures(GL_TEXTURE_2D, 1, &overlayTexture);
    glTextureStorage2D(overlayTexture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(overlayTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glTextureParameteri(overlayTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(overlayTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(overlayTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(overlayTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glActiveTexture(GL_TEXTURE0);
    glBindTextureUnit(0, overlayTexture);

    glShaderSource(vertexShader, 1, &vertex_shader, nullptr);
    glCompileShader(vertexShader);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, nullptr);
    glCompileShader(fragmentShader);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    glLinkProgram(shaderProgram);
    SetTimer(window, 0, 10000, (TIMERPROC) nullptr);
    while (!done && message.message != WM_QUIT) {
        if(PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
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
    wglMakeCurrent(nullptr, nullptr);
    ReleaseDC(window, deviceContext);
    wglDeleteContext(openGLRenderingContext);
    DestroyWindow(window);
    return (int)message.wParam;
}
