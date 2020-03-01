#include "main.h"
VOID LoadOpenGLFunctions() {
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
//            if (sprites[0].active) {
//                if (filled == 0 || filled == 4) {
//                    sprites[0].points[0] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
//                    sprites[0].points[1] = 1.0f - (yTemp / (float)windowSize.y) * 2;
//                    sprites[0].points[9] = sprites[0].points[0];
//                    sprites[0].points[10] = sprites[0].points[1];
//                    filled = 1;
//                } else if (filled == 1) {
//                    sprites[0].points[3] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
//                    sprites[0].points[4] = 1.0f - (yTemp / (float)windowSize.y) * 2;
//                    filled = 2;
//                } else if (filled == 2) {
//                    sprites[0].points[6] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
//                    sprites[0].points[7] = 1.0f - (yTemp / (float)windowSize.y) * 2;
//                    sprites[0].points[12] = sprites[0].points[6];
//                    sprites[0].points[13] = sprites[0].points[7];
//                    filled = 3;
//                } else if (filled == 3) {
//                    sprites[0].points[15] = (xTemp / (float)windowSize.x) * 2 - 1.0f;
//                    sprites[0].points[16] = 1.0f - (yTemp / (float)windowSize.y) * 2;
//                    filled = 4;
//                }
//            }
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

VOID SetupLoggingConsoleWindow() {
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, nullptr, _IONBF, 1);
    *stdout = *hf_out;
    printf("Hello, party people!\r\n");
}

// TODO: Now that textures work with stb_image, would this work instead?
VOID SetupGDIPlusImageLoader() {
    // Need GDI+ to parse image files
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
}

void ExitWithMessage(const char * message) {
    MessageBox(nullptr, message, "Error", MB_ICONERROR);
    ExitProcess(1);
}

VOID HandleFileError(LPCTSTR message, LPCTSTR fileName) {
    CHAR buffer[256];
    snprintf(buffer, 256, "Error opening %s: %s!", message, fileName);
    ExitWithMessage(buffer);
}

FileReadResult ReadLocalFile(LPCTSTR fileName) {
    HANDLE file;
    LARGE_INTEGER fileSizeInBytes = {};
    CHAR * fileBytes = nullptr;
    file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        HandleFileError("Couldn't open file", fileName);
    } else {
        if (!GetFileSizeEx(file, (PLARGE_INTEGER)&fileSizeInBytes)) {
            HandleFileError("Couldn't get file size", fileName);
        } else {
            // Allocate memory for the file data.
            fileBytes = (CHAR *)malloc((size_t)(sizeof(CHAR) * fileSizeInBytes.QuadPart));
            // Read the file.
            int bytesRead;
            BOOL success = ReadFile(
                    file,
                    fileBytes,
                    fileSizeInBytes.LowPart,
                    (PDWORD)&bytesRead,
                    nullptr);
            if (!success) {
                HandleFileError("Couldn't read file", fileName);
            } else {
                fileBytes[fileSizeInBytes.LowPart] = 0x00;
            }
        }
    }
    FileReadResult res = { fileBytes, (size_t)(sizeof(CHAR) * fileSizeInBytes.QuadPart) };
    return res;
}

// TODO: This is the verily dumbest texture atlas "packer" I ever dun did perchance to lay mine own two eyes upon. Fix.
// TODO: Maybe pad all textures out to the nearest power of 2 and use a quadtree? (record padding amount of course)
VOID RebuildOverlayTextureAtlas() {
    INT largestWidth = 0;
    INT largestHeight = 0;
    for (INT i = 0; i < numSprites; i++) {
        if (sprites[i].width > largestWidth) {
            largestWidth = sprites[i].width;
        }
        if (sprites[i].height > largestHeight) {
            largestHeight = sprites[i].height;
        }
    }
    auto spritesPerRow = (INT)ceil(sqrt(numSprites));
    auto spritesPerCol = (INT)ceil(numSprites / (float)spritesPerRow);
    INT width = largestWidth * spritesPerRow;
    INT height = spritesPerCol * largestHeight;
    if (width > 0 && height > 0) {
        auto buffer = (PBYTE)HeapAlloc(GetProcessHeap(), 0, sizeof(BYTE) * 4 * height * width);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int bufferIndex = x * 4 + y * width * 4;
                int spriteX = x % largestWidth;
                int spriteY = y % largestHeight;
                int spriteNum = x / largestWidth + (y / largestHeight) * spritesPerRow;
                int spriteWidth = sprites[spriteNum].width;
                int spriteHeight = sprites[spriteNum].height;
                int spriteBufferIndex = spriteX * 4 + spriteY * sprites[spriteNum].width * 4;
                if (spriteBufferIndex < sprites[spriteNum].bufferSize && spriteX < spriteWidth && spriteY < spriteHeight) {
                    buffer[bufferIndex] = sprites[spriteNum].buffer[spriteBufferIndex];
                    buffer[bufferIndex + 1] = sprites[spriteNum].buffer[spriteBufferIndex + 1];
                    buffer[bufferIndex + 2] = sprites[spriteNum].buffer[spriteBufferIndex + 2];
                    buffer[bufferIndex + 3] = sprites[spriteNum].buffer[spriteBufferIndex + 3];
                }
            }
        }
        for (int spriteNum = 0; spriteNum < numSprites; spriteNum++) {
            sprites[spriteNum].atlasX = ((spriteNum % spritesPerRow) * largestWidth) / (float)width;
            sprites[spriteNum].atlasY = ((spriteNum / spritesPerRow) * largestHeight) / (float)height;
            sprites[spriteNum].atlasW = sprites[spriteNum].width / (float)width;
            sprites[spriteNum].atlasH = sprites[spriteNum].height / (float)height;
        }
        stbi_write_png("atlas.png", width, height, 4, buffer, width * 4);
        glCreateTextures(GL_TEXTURE_2D, 1, &overlayTexture);
        glTextureStorage2D(overlayTexture, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(overlayTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        glTextureParameteri(overlayTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(overlayTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(overlayTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(overlayTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glActiveTexture(GL_TEXTURE0);
        glBindTextureUnit(0, overlayTexture);
    }
}

VOID SetupJRE() {
    HINSTANCE jre = LoadLibrary(JRE_PATH);
    if (!jre) {
        printf("jre DLL load error=%d\r\n", (int)GetLastError());
    }
    MyJNI_GetDefaultJavaVMInitArgs = DLLFUNC(void, (void *), jre, "JNI_GetDefaultJavaVMInitArgs");
    MyJNI_CreateJavaVM = DLLFUNC(jint, (JavaVM **, void **, void *), jre, "JNI_CreateJavaVM");
    JavaVMInitArgs vm_args = {};
    vm_args.version = 0x00010001;
    MyJNI_GetDefaultJavaVMInitArgs(&vm_args);
    MyJNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);
    FileReadResult gameClassReadResult = ReadLocalFile("Game.class");
    auto classBytes = (jbyte const *)gameClassReadResult.data;
    auto classSize = (jsize)gameClassReadResult.size;
    jclass gameClass = env->DefineClass("Game", nullptr, classBytes, classSize);
    gameMain = env->GetStaticMethodID(gameClass, "main", "()V");
    JNINativeMethod loadSpriteFile {
            (char *)"loadSpriteFile",
            (char *)"(Ljava/lang/String;Ljava/lang/String;)V",
            (void *) *[](JNIEnv *env, jobject objectOrClass, jstring name, jstring fileName) {
                // Load sprite file from disk
                strcpy(sprites[numSprites].name, env->GetStringUTFChars(name, nullptr));
                sprites[numSprites].loadImageFile(env->GetStringUTFChars(fileName, nullptr));
                numSprites++;
                RebuildOverlayTextureAtlas();
                printf("JNI loadSpriteFile\r\n");
            }
    };
    env->RegisterNatives(gameClass, &loadSpriteFile, 1);
    JNINativeMethod loadSpriteFiles {
            (char *)"loadSpriteFiles",
            (char *)"([Ljava/lang/String;[Ljava/lang/String;LLoadSpriteFilesCallbackFunc;)V",
            (void *) *[](JNIEnv *env, jobject objectOrClass, jobjectArray spriteNames, jobjectArray spriteFilePaths, jobject cb) {
                // Load sprite files from disk
                for (int i = 0; i < env->GetArrayLength(spriteNames); i++) {
                    auto name = env->GetStringUTFChars((jstring)env->GetObjectArrayElement(spriteNames, i), nullptr);
                    auto filePath = env->GetStringUTFChars((jstring)env->GetObjectArrayElement(spriteFilePaths, i), nullptr);
                    strcpy(sprites[numSprites].name, name);
                    sprites[numSprites].loadImageFile(filePath);
                    numSprites++;
                    RebuildOverlayTextureAtlas();
                    printf("JNI loadSpriteFile\r\n");
                }
//                jclass objclass = env->GetObjectClass(cb);
//                jmethodID method = env->GetMethodID(objclass, "cb", "()V");
//                env->CallStaticVoidMethod(nullptr, method);
            }
    };
    env->RegisterNatives(gameClass, &loadSpriteFiles, 1);
    JNINativeMethod print {
            (char *)"print",
            (char *)"(Ljava/lang/String)V",
            (void *) *[](JNIEnv *env, jobject objectOrClass, jstring str) {
                printf("%s", env->GetStringUTFChars(str, nullptr));
            }
    };
    env->RegisterNatives(gameClass, &print, 1);
    JNINativeMethod createWidget {
            (char *)"createWidget",
            (char *)"(Ljava/lang/String;[Ljava/lang/String;[[FFFFF)V",
            (void *) *[](JNIEnv *env, jclass objectOrClass, jstring name, jobjectArray spriteNames,
                         jobjectArray spriteLocations, jfloat x, jfloat y, jfloat w, jfloat h) {
                strcpy(widgets[numWidgets].name, env->GetStringUTFChars(name, nullptr));
                widgets[numWidgets].x = x;
                widgets[numWidgets].y = y;
                widgets[numWidgets].width = w;
                widgets[numWidgets].height = h;
                INT numSpritesInWidget = env->GetArrayLength(spriteNames);
                widgets[numWidgets].numSpritesInWidget = numSpritesInWidget;
                for (INT i = 0; i < numSpritesInWidget; i++) {
                    // Get name
                    const CHAR * spriteName = env->GetStringUTFChars((jstring)env->GetObjectArrayElement(spriteNames, i), nullptr);
                    // Set sprite name
                    strcpy(widgets[numWidgets].spriteNames[i], spriteName);
                    // Set sprite index
                    INT spriteIndex = 0;
                    for (INT j = 0; j < numSprites; j++) {
                        if (strcmp(sprites[j].name, spriteName) == 0) {
                            break;
                        }
                        spriteIndex++;
                    }
                    widgets[numWidgets].spriteIndexes[i] = spriteIndex;
                    // Set sprite triangle points
                    auto floats = (jfloatArray)env->GetObjectArrayElement(spriteLocations, i);
                    float * spriteLocation = env->GetFloatArrayElements(floats, JNI_FALSE);
                    FLOAT spriteX = spriteLocation[0];
                    FLOAT spriteY = spriteLocation[1];
                    FLOAT spriteW = spriteLocation[2];
                    FLOAT spriteH = spriteLocation[3];
                    FLOAT widgetX = widgets[numWidgets].x;
                    FLOAT widgetY = widgets[numWidgets].y;
                    FLOAT widgetW = widgets[numWidgets].width;
                    FLOAT widgetH = widgets[numWidgets].height;
                    printf("%f\r\n", spriteX);
                    printf("%f\r\n", spriteY);
                    printf("%f\r\n", spriteW);
                    printf("%f\r\n", spriteH);
                    // TODO: Factor in location of widget itself
                    GLfloat pointsTemp[] = {
                            spriteX * widgetW - 0.5f + widgetX, (spriteY - spriteH) * widgetH + 0.5f - widgetY, 0.0f,
                            spriteX * widgetW - 0.5f + widgetX, spriteY * widgetH + 0.5f - widgetY, 0.0f,
                            (spriteX + spriteW) * widgetW - 0.5f + widgetX, spriteY * widgetH + 0.5f - widgetY, 0.0f,
                            spriteX * widgetW - 0.5f + widgetX, (spriteY - spriteH) * widgetH + 0.5f - widgetY, 0.0f,
                            (spriteX + spriteW) * widgetW - 0.5f + widgetX, spriteY * widgetH + 0.5f - widgetY, 0.0f,
                            (spriteX + spriteW) * widgetW - 0.5f + widgetX, (spriteY - spriteH) * widgetH + 0.5f - widgetY, 0.0f
                    };
//                    GLfloat pointsTemp[] = {
//                            spriteX, spriteY - spriteH, 0.0f,
//                            spriteX, spriteY, 0.0f,
//                            spriteX + spriteW, spriteY, 0.0f,
//                            spriteX, spriteY - spriteH, 0.0f,
//                            spriteX + spriteW, spriteY, 0.0f,
//                            spriteX + spriteW, spriteY - spriteH, 0.0f
//                    };
                    for (INT j = 0; j < 18; j++) {
                        widgets[numWidgets].spritesPoints[i][j] = pointsTemp[j];
                    }
                    // Set sprite texture coordinates
                    FLOAT atlasX = sprites[spriteIndex].atlasX;
                    FLOAT atlasY = sprites[spriteIndex].atlasY;
                    FLOAT atlasW = sprites[spriteIndex].atlasW;
                    FLOAT atlasH = sprites[spriteIndex].atlasH;
                    GLfloat textureCoordinatesTemp[] = {
                        atlasX, atlasY + atlasH,
                        atlasX, atlasY,
                        atlasX + atlasW, atlasY,
                        atlasX, atlasY + atlasH,
                        atlasX + atlasW, atlasY,
                        atlasX + atlasW, atlasY + atlasH
                    };
                    for (INT j = 0; j < 12; j++) {
                        widgets[numWidgets].spritesTextureCoordinates[i][j] = textureCoordinatesTemp[j];
                    }
                }
                numWidgets++;
                printf("JNI createWidget numWidgets=%d\r\n", numWidgets);
            }
    };
    env->RegisterNatives(gameClass, &createWidget, 1);
}

VOID RunGameLoop() {
    env->CallStaticVoidMethod(nullptr, gameMain);
}

VOID SetupWin32Stuff() {
    HINSTANCE h;
    WNDCLASS wc = {};
    int pf;
    PIXELFORMATDESCRIPTOR pfd = {};
    h = GetModuleHandle(nullptr);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = h;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "Test123";
    RegisterClass(&wc);
    window = CreateWindow("Test123", "Hi there", WM_WINDOWED, 0, 0, 800, 600, nullptr, nullptr, h, nullptr);
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
    ShowWindow(window, SW_SHOWDEFAULT);
}

VOID SetupTimers() {
    SetTimer(window, 0, 10000, (TIMERPROC) nullptr);
}

VOID Init() {
    // Setup a console for logging
    SetupLoggingConsoleWindow();
    // Setup GDI+ for image loading purposes
    SetupGDIPlusImageLoader();
    // Load JRE
    SetupJRE();
    // win32 windows setup stuff
    SetupWin32Stuff();
    // Setup timers for game events
    SetupTimers();
}

VOID Sprite::loadImageFile(const CHAR * fileName) {
    active = TRUE;
    buffer = stbi_load(fileName, &width, &height, &bpp, 0);
    bufferSize = width * height * 4;
    if (bpp != 4) {
        ExitWithMessage("Not 4bpp!\r\n");
    }
    printf("Sprite::loadImageFile width=%d, height=%d, bpp=%d\r\n", width, height, bpp);
    if (!buffer) {
        CHAR fullMessage[256];
        snprintf(fullMessage, 256, "FAILED TO LOAD THE IMAGE %s", fileName);
        MessageBox(nullptr, fullMessage, "", 0);
    }
}

VOID Widget::draw() {
    glBindTextureUnit(0, overlayTexture);
    glUseProgram(overlayShaderProgram);
    int textureLocation = glGetUniformLocation(overlayShaderProgram, "overlay_texture");
    glUniform1i(textureLocation, 0);
    for (INT i = 0; i < numSpritesInWidget; i++) {
        glNamedBufferData(overlayPointsVBO, numOverlayPoints * 3 * sizeof(GLfloat), spritesPoints[i], GL_STATIC_DRAW);
        glNamedBufferData(overlayTextureVBO, numOverlayPoints * 2 * sizeof(GLfloat), spritesTextureCoordinates[i],
                          GL_STATIC_DRAW);
        glBindVertexArray(overlayVAO);
        glDrawArrays(GL_TRIANGLES, 0, numOverlayPoints);
    }
}

VOID SetupOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Create Vertex Position VBO
    glCreateBuffers(1, &overlayPointsVBO);
//    glNamedBufferData(overlayPointsVBO, numOverlayPoints * 3 * sizeof(GLfloat), testPoints, GL_STATIC_DRAW);
    // Create Texture VBO
    glCreateBuffers(1, &overlayTextureVBO);
//    glNamedBufferData(overlayTextureVBO, numOverlayPoints * 2 * sizeof(GLfloat), testTextureCoordinates, GL_STATIC_DRAW);
    // Create VAO
    glCreateVertexArrays(1, &overlayVAO);
    glEnableVertexArrayAttrib(overlayVAO, 0);
    glEnableVertexArrayAttrib(overlayVAO, 1);
    glVertexArrayVertexBuffer(overlayVAO, 0, overlayPointsVBO, 0, 12);
    glVertexArrayVertexBuffer(overlayVAO, 1, overlayTextureVBO, 0, 8);
    glVertexArrayAttribBinding(overlayVAO, 0, 0);
    glVertexArrayAttribBinding(overlayVAO, 1, 1);
    glVertexArrayAttribFormat(overlayVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(overlayVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glBindVertexArray(overlayVAO);
    // Create vertex shader
    overlayVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(overlayVertexShader, 1, &vertex_shader, nullptr);
    glCompileShader(overlayVertexShader);
    // Create fragment shader
    overlayFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(overlayFragmentShader, 1, &fragment_shader, nullptr);
    glCompileShader(overlayFragmentShader);
    // Create shader program
    overlayShaderProgram = glCreateProgram();
    glAttachShader(overlayShaderProgram, overlayFragmentShader);
    glAttachShader(overlayShaderProgram, overlayVertexShader);
    glLinkProgram(overlayShaderProgram);
}

VOID GameLoop() {
    while (!done && message.message != WM_QUIT) {
        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
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
            // Run Java code
            RunGameLoop();
            // Make OpenGL calls
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (int i = 0; i < numWidgets; i++) {
                widgets[i].draw();
            }
            glFlush();
            SwapBuffers(deviceContext);
            // TODO: Calculate framerate
            // This must be smaller than the input delay to register key presses.
            // TODO: Should use timers so not all game updates happen at this high speed
            Sleep(10);
        }
    }
}

int Shutdown() {
    wglMakeCurrent(nullptr, nullptr);
    ReleaseDC(window, deviceContext);
    wglDeleteContext(openGLRenderingContext);
    DestroyWindow(window);
    return (int)message.wParam;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    Init();
    LoadOpenGLFunctions();
    SetupOpenGL();
    GameLoop();
    return Shutdown();
}
