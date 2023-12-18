#include <nan.h>
#include <windows.h>

using namespace v8;

typedef struct RoboticMousePoint
{
    int32_t x;
    int32_t y;
} RoboticMousePoint;

// The width of the virtual screen, in pixels.
static int vscreenWidth = -1; // not initialized

// The height of the virtual screen, in pixels.
static int vscreenHeight = -1; // not initialized

// The coordinates for the left side of the virtual screen.
static int vscreenMinX = 0;

// The coordinates for the top of the virtual screen.
static int vscreenMinY = 0;

#define MOUSE_COORD_TO_ABS(coord, width_or_height) ((65536 * (coord) / width_or_height) + ((coord) < 0 ? -1 : 1))

void UpdateScreenMetrics()
{
    vscreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    vscreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    vscreenMinX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    vscreenMinY = GetSystemMetrics(SM_YVIRTUALSCREEN);
}

NAN_METHOD(MoveMouse)
{
    if (info.Length() != 2)
    {
        return Nan::ThrowError("Invalid number of argument");
    }

    int32_t x = Nan::To<int32_t>(info[0]).FromJust();
    int32_t y = Nan::To<int32_t>(info[1]).FromJust();
    size_t real_x = MOUSE_COORD_TO_ABS(x - vscreenMinX, vscreenWidth);
    size_t real_y = MOUSE_COORD_TO_ABS(y - vscreenMinY, vscreenHeight);

    INPUT input[1] = {};
    ZeroMemory(input, sizeof(input));

    input[0].type = INPUT_MOUSE;
    input[0].mi.dx = real_x;
    input[0].mi.dy = real_y;
    input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
    input[0].mi.time = 0;

    SendInput(ARRAYSIZE(input), input, sizeof(input));

    info.GetReturnValue().Set(Nan::New(true));
}

NAN_METHOD(PressLeftClick)
{
    INPUT input[1] = {};
    ZeroMemory(input, sizeof(input));

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    input[0].mi.time = 0;

    SendInput(ARRAYSIZE(input), input, sizeof(input));

    info.GetReturnValue().Set(Nan::New(true));
}

NAN_METHOD(ReleaseLeftClick)
{
    INPUT input[1] = {};
    ZeroMemory(input, sizeof(input));

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    input[0].mi.time = 0;

    SendInput(ARRAYSIZE(input), input, sizeof(input));

    info.GetReturnValue().Set(Nan::New(true));
}

NAN_MODULE_INIT(Init)
{
    UpdateScreenMetrics();

    Nan::Set(
        target,
        Nan::New("moveMouse").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(MoveMouse)).ToLocalChecked());
    Nan::Set(
        target,
        Nan::New("pressLeftClick").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(PressLeftClick)).ToLocalChecked());
    Nan::Set(
        target,
        Nan::New("releaseLeftClick").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(ReleaseLeftClick)).ToLocalChecked());
}

NODE_MODULE(roboticMouse, Init)