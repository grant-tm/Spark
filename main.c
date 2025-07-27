#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <math.h>

// Include Clay and the Win32 GDI renderer
#define CLAY_IMPLEMENTATION
#include "external/clay/clay.h"
#include "external/clay/renderers/win32_gdi/clay_renderer_gdi.c"

// Color Definitions
const Clay_Color COLOR_PRIMARY = (Clay_Color) {52, 58, 59, 255};
const Clay_Color COLOR_SIDEBAR = (Clay_Color) {31, 34, 34, 255};
const Clay_Color COLOR_HEADER = (Clay_Color) {39, 42, 43, 255};
const Clay_Color COLOR_OUTLINE = (Clay_Color) {52, 58, 59, 255};

// Global variables for Clay context
Clay_Context* clayContext = NULL;
HFONT fonts[1];

// Error handler for Clay
void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Clay Error: %s\n", errorData.errorText.chars);
    switch(errorData.errorType) {
        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED:
            printf("Text measurement function not provided\n");
            break;
        case CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED:
            printf("Arena capacity exceeded\n");
            break;
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED:
            printf("Elements capacity exceeded\n");
            break;
        case CLAY_ERROR_TYPE_DUPLICATE_ID:
            printf("Duplicate element ID\n");
            break;
        default:
            printf("Unknown error type\n");
            break;
    }
}

// Text measurement function for Win32 GDI
static inline Clay_Dimensions MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    if (userData != NULL) {
        HFONT* fonts = (HFONT*) userData;
        HFONT hFont = fonts[config->fontId];
        
        if (hFont != NULL) {
            HDC hScreenDC = GetDC(NULL);
            HDC hTempDC = CreateCompatibleDC(hScreenDC);
            
            if (hTempDC != NULL) {
                HFONT hPrevFont = SelectObject(hTempDC, hFont);
                
                SIZE size;
                GetTextExtentPoint32(hTempDC, text.chars, text.length, &size);
                
                SelectObject(hTempDC, hPrevFont);
                DeleteDC(hTempDC);
                ReleaseDC(HWND_DESKTOP, hScreenDC);
                
                return (Clay_Dimensions) {
                    .width = (float)size.cx,
                    .height = (float)size.cy
                };
            }
            
            ReleaseDC(HWND_DESKTOP, hScreenDC);
        }
    }
    
    // Fallback for system bitmap font
    return (Clay_Dimensions) {
        .width = text.length * 8.0f, // Approximate character width
        .height = 16.0f // Approximate character height
    };
}

#define TITLE_BAR_HEIGHT 32
#define BUTTON_WIDTH 46

// Create the main layout
Clay_RenderCommandArray CreateMainLayout()
{
    Clay_BeginLayout();
    
    // Main container with sidebar and content
    CLAY({ 
        .id = CLAY_ID("OuterContainer"), 
        .layout = { 
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, 
            .padding = CLAY_PADDING_ALL(0), 
            .childGap = 0 
        }, 
        .backgroundColor = COLOR_PRIMARY
    }) {
        
        // Top Bar
        CLAY({
            .id = CLAY_ID("Header"),
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(30) },
                .padding = CLAY_PADDING_ALL(0),
                .childGap = 0
            },
            .backgroundColor = COLOR_HEADER
        }) {}

        // Main content area
        CLAY({ 
            .id = CLAY_ID("MainContent"), 
            .layout = { 
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } 
            }, 
            .backgroundColor = COLOR_PRIMARY
        }) {
            // Side Bar
            CLAY({
                .id = CLAY_ID("SideBar"),
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_GROW(0) }, 
                    .padding = CLAY_PADDING_ALL(0), 
                    .childGap = 0 
                },
                .backgroundColor = COLOR_SIDEBAR
            }) {}

            // Main Interactive Area
            CLAY({
                .id = CLAY_ID("FileArea"),
                .layout = { 
                    //.layoutDirection = CLAY_TOP_TO_BOTTOM, 
                    .sizing = { .width = CLAY_SIZING_FIXED(340), .height = CLAY_SIZING_GROW(0) }, 
                    .padding = CLAY_PADDING_ALL(16), 
                    .childGap = 16 
                },
                .backgroundColor = COLOR_PRIMARY
            }) {}
        }
    }
    
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    return renderCommands;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    
    //-----------------------------------------------------
    // WINDOW SETUP
    //-----------------------------------------------------
    case WM_CREATE:
        {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            int xPos = (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2;
            int yPos = (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2;
            SetWindowPos(hwnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
        break;
    
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    //-----------------------------------------------------
    // HANDLE CONTROL INPUTS
    //-----------------------------------------------------
    case WM_MOUSEWHEEL:
        {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            Clay_UpdateScrollContainers(true, (Clay_Vector2){0, zDelta}, 0.016f); // ~60fps
            InvalidateRect(hwnd, NULL, false);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
        {
            short mouseX = GET_X_LPARAM(lParam);
            short mouseY = GET_Y_LPARAM(lParam);
            bool isMouseDown = (wParam & MK_LBUTTON) != 0;
            
            Clay_SetPointerState((Clay_Vector2){mouseX, mouseY}, isMouseDown);
            InvalidateRect(hwnd, NULL, false);
        }
        break;

    //-----------------------------------------------------
    // PAINT
    //-----------------------------------------------------
    case WM_PAINT:
        {
            Clay_RenderCommandArray renderCommands = CreateMainLayout();
            Clay_Win32_Render(hwnd, renderCommands, fonts);
        }
        break;
    
    //-----------------------------------------------------
    // CUSTOMIZE NON-CLIENT AREA
    //-----------------------------------------------------
    case WM_NCCALCSIZE:
        if (wParam) return 0;
        break;

    case WM_NCHITTEST: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &pt);
        RECT rect;
        GetClientRect(hwnd, &rect);

        if (pt.y >= 0 && pt.y < 5) return HTTOP;
        if (pt.y < 5 && pt.x < 5) return HTTOPLEFT;
        if (pt.y < 5 && pt.x > rect.right - 5) return HTTOPRIGHT;
        if (pt.y > rect.bottom - 5 && pt.x < 5) return HTBOTTOMLEFT;
        if (pt.y > rect.bottom - 5 && pt.x > rect.right - 5) return HTBOTTOMRIGHT;
        if (pt.x < 5) return HTLEFT;
        if (pt.x > rect.right - 5) return HTRIGHT;
        if (pt.y > rect.bottom - 5) return HTBOTTOM;

        if (pt.y < TITLE_BAR_HEIGHT) {
            if (pt.x >= rect.right - 3 * BUTTON_WIDTH && pt.x < rect.right - 2 * BUTTON_WIDTH) return HTMINBUTTON;
            if (pt.x >= rect.right - 2 * BUTTON_WIDTH && pt.x < rect.right - BUTTON_WIDTH) return HTMAXBUTTON;
            if (pt.x >= rect.right - BUTTON_WIDTH) return HTCLOSE;
            return HTCAPTION;
        }
        return HTCLIENT;
    }

    case WM_NCLBUTTONDOWN:
        if (wParam == HTMINBUTTON) ShowWindow(hwnd, SW_MINIMIZE);
        else if (wParam == HTMAXBUTTON) {
            WINDOWPLACEMENT wp = { .length = sizeof(wp) };
            GetWindowPlacement(hwnd, &wp);
            ShowWindow(hwnd, (wp.showCmd == SW_MAXIMIZE) ? SW_RESTORE : SW_MAXIMIZE);
        }
        else if (wParam == HTCLOSE) PostMessage(hwnd, WM_CLOSE, 0, 0);
        else return DefWindowProc(hwnd, msg, wParam, lParam);
        return 0;

    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow) {
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"Spark";
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassExW(&wc)) return -1;

    DWORD style = WS_OVERLAPPEDWINDOW & ~WS_CAPTION & WS_THICKFRAME;
    DWORD exStyle = WS_EX_APPWINDOW;
    HWND hwnd = CreateWindowExW(
        exStyle,
        L"Spark",
        L"Sample Explorer",
        style | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInst, NULL);

    if (!hwnd) return -1;

    // Initialize Clay
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    clayContext = Clay_Initialize(clayMemory, (Clay_Dimensions){800, 600}, (Clay_ErrorHandler){HandleClayErrors});
    if (clayContext == NULL) {
        MessageBox(NULL, "Clay initialization failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    Clay_SetMeasureTextFunction(MeasureText, fonts);

    // Force update to remove default non-client area
    SetWindowPos(
        hwnd, 
        NULL, 
        0, 0, 0, 0, 
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
