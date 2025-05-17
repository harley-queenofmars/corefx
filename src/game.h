#pragma once
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "../corefw/corefw.h"   // IWYU pragma: keep

extern CFClassRef CFXGame;

typedef struct __CFXGame* CFXGameRef;
typedef struct __CFXGameVtbl* CFXGameVtblRef;

extern CFXGameRef CFXGame_instance;

typedef void (*CFXGameProc)(void* self);
typedef struct __CFXGameVtbl {
    void (*Initialize)  (void* self);
    void (*LoadContent) (void* self);
    void (*Update)      (void* self);
    void (*Draw)        (void* self);
} __CFXGameVtbl;

typedef struct __CFXGame {
    __CFObject obj;
    void* subclass;
    CFXGameVtblRef virtual;
    GLFWwindow* window;
    char* title;
    int len;
    bool* keys;
    double delta;
    double factor;
    uint64_t targetElapsedTime;
    uint64_t accumulatedElapsedTime;
    uint64_t maxElapsedTime;
    uint64_t totalGameTime;
    uint64_t elapsedGameTime;
    uint64_t currentTime;
    long previousTicks;
    int x;
    int y;
    int width;
    int height;
    uint32_t flags;
    int mouseX;
    int mouseY;
    bool mouseDown;
    int sdlVersion;
    int frameSkip;
    int gl_major_version;
    int gl_minor_version;
    bool isRunning;
    int ticks;
    int fps;
    bool isFixedTimeStep;
    bool isRunningSlowly;
    int updateFrameLag;
    bool shouldExit;
    bool suppressDraw;
} __CFXGame;


extern method void* Ctor(
    CFXGameRef this, 
    char* cstr, 
    int width, 
    int height, 
    void* subclass, 
    CFXGameVtblRef vptr);

extern method void HandleEvents(
    CFXGameRef const this);

extern method char* ToString(
    CFXGameRef this);

extern method void Start(
    CFXGameRef const this);

extern method void Tick(
    CFXGameRef const this);

extern method void RunLoop(
    CFXGameRef const this);

extern method void Run(
    CFXGameRef const this);

static inline CFXGameRef NewCFXGame(char* cstr, int width, int height, void* subclass, CFXGameVtblRef vptr)
{
    return Ctor((CFXGameRef)CFCreate(CFXGame), cstr, width, height, subclass, vptr);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///// virtual methods
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/**
 * CFXGame::Draw
 */
static inline method void Draw(CFXGameRef const this)
{
    this->virtual->Draw(this->subclass);
}

/**
 * CFXGame::LoadContent
 */
static inline method void LoadContent(CFXGameRef const this)
{
    this->virtual->LoadContent(this->subclass);
}

/**
 * CFXGame::Initialize
 */
static inline method void Initialize(CFXGameRef const this)
{
    this->virtual->Initialize(this->subclass);
}

/**
 * CFXGame::Update
 */
static inline method void Update(CFXGameRef const this)
{
    this->virtual->Update(this->subclass);
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///// gamepad events
////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// 
bool onclick_handler_dpad_up(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
bool onclick_handler_dpad_down(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
bool onclick_handler_dpad_left(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
bool onclick_handler_dpad_right(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
bool onclick_handler_button_a(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);

bool touchstart_handler_dpad_up(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchstart_handler_dpad_down(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchstart_handler_dpad_left(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchstart_handler_dpad_right(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchstart_handler_button_a(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);

bool touchend_handler_dpad_up(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchend_handler_dpad_down(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchend_handler_dpad_left(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchend_handler_dpad_right(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchend_handler_button_a(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);

bool touchcancel_handler_dpad_up(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchcancel_handler_dpad_down(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchcancel_handler_dpad_left(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchcancel_handler_dpad_right(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
bool touchcancel_handler_button_a(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData);
