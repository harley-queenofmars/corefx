#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "../corefw/corefw.h"   // IWYU pragma: keep
#include "corefx.h"             // IWYU pragma: keep

class(CFXGame);

static void dtor(void* self)
{
    CFXGameRef this = self;

    free(this->title);
    free(this->keys);
    glfwTerminate();
}

constexpr float TicksPerMillisecond = 10000.0f;
constexpr float MillisecondsPerTick = 1.0f / (TicksPerMillisecond);
constexpr float TicksPerSecond = TicksPerMillisecond * 1000.0f; // 10,000,000
constexpr float SecondsPerTick = 1.0f / (TicksPerSecond); // 0.0001

CFXGameRef CFXGame_instance;

void CFXGame_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key,
    // we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            CFXGame_instance->keys[key] = true;
        else if (action == GLFW_RELEASE)
            CFXGame_instance->keys[key] = false;
    }
}



void CFXGame_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

static uint64_t GetTicks()
{
    struct timeval t;
    gettimeofday(&t, nullptr);

    uint64_t ts = t.tv_sec;
    uint64_t us = t.tv_usec;
    return ((ts * 1000000L) + us) * 10;
}

method void* Ctor(CFXGameRef this, char* cstr, int width, int height, void* subclass, CFXGameVtblRef vptr)
{
    this->subclass = subclass;
    this->virtual = vptr;
    srand(time(nullptr));
    this->title = CFStrDup(cstr);
    this->len = strlen(cstr);
    this->keys = calloc(1024, 1);
    this->width = width;
    this->height = height;
    this->frameSkip = 0;
    this->isRunning = false;
    this->previousTicks = 0;
    this->isFixedTimeStep = true;
    this->shouldExit = false;
    this->suppressDraw = false;
    this->maxElapsedTime = 500 * TicksPerMillisecond;
    this->targetElapsedTime = 166667;
    this->accumulatedElapsedTime = 0;
    this->currentTime = GetTicks();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    // glfw window creation
    // --------------------
    this->window = glfwCreateWindow(this->width, this->height, "LearnOpenGL", nullptr, nullptr);
    if (this->window == nullptr) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, CFXGame_framebuffer_size_callback);

    CFXGame_instance = this;
    glfwSetKeyCallback(this->window, CFXGame_key_callback);
    glfwSwapInterval(1);

    glViewport(0, 0, this->width, this->height);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    emscripten_set_click_callback("#dpad-up", this, EM_TRUE, onclick_handler_dpad_up);
    emscripten_set_click_callback("#dpad-down", this, EM_TRUE, onclick_handler_dpad_down);
    emscripten_set_click_callback("#dpad-left", this, EM_TRUE, onclick_handler_dpad_left);
    emscripten_set_click_callback("#dpad-right", this, EM_TRUE, onclick_handler_dpad_right);
    emscripten_set_click_callback("#button-a", this, EM_TRUE, onclick_handler_button_a);

    emscripten_set_touchstart_callback("#dpad-up", this, EM_TRUE, touchstart_handler_dpad_up);
    emscripten_set_touchstart_callback("#dpad-down", this, EM_TRUE, touchstart_handler_dpad_down);
    emscripten_set_touchstart_callback("#dpad-left", this, EM_TRUE, touchstart_handler_dpad_left);
    emscripten_set_touchstart_callback("#dpad-right", this, EM_TRUE, touchstart_handler_dpad_right);
    emscripten_set_touchstart_callback("#buttn-a", this, EM_TRUE, touchstart_handler_button_a);

    emscripten_set_touchend_callback("#dpad-up", this, EM_TRUE, touchend_handler_dpad_up);
    emscripten_set_touchend_callback("#dpad-down", this, EM_TRUE, touchend_handler_dpad_down);
    emscripten_set_touchend_callback("#dpad-left", this, EM_TRUE, touchend_handler_dpad_left);
    emscripten_set_touchend_callback("#dpad-right", this, EM_TRUE, touchend_handler_dpad_right);
    emscripten_set_touchend_callback("#buttn-a", this, EM_TRUE, touchend_handler_button_a);

    emscripten_set_touchcancel_callback("#dpad-up", this, EM_TRUE, touchcancel_handler_dpad_up);
    emscripten_set_touchcancel_callback("#dpad-down", this, EM_TRUE, touchcancel_handler_dpad_down);
    emscripten_set_touchcancel_callback("#dpad-left", this, EM_TRUE, touchcancel_handler_dpad_left);
    emscripten_set_touchcancel_callback("#dpad-right", this, EM_TRUE, touchcancel_handler_dpad_right);
    emscripten_set_touchcancel_callback("#buttn-a", this, EM_TRUE, touchcancel_handler_button_a);

    return this;
}

method char* ToString(CFXGameRef this)
{
    return this->title;
}
/**
 * CFXGame::Start
 */
method void Start(CFXGameRef const this)
{
    this->isRunning = true;
}

/**
 * CFXGame::HandleEvents
 */
method void HandleEvents(CFXGameRef const this)
{
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(this->window, true);
        this->shouldExit = true;
        return;
    }
}

/**
 * CFXGame::Tick
 */
method void Tick(CFXGameRef const this)
{
    while (true) {
        // Advance the accumulated elapsed time.
        long currentTicks = (GetTicks() - this->currentTime); //*10;
        this->accumulatedElapsedTime = this->accumulatedElapsedTime + currentTicks - this->previousTicks;
        this->previousTicks = (long)currentTicks;

        // If we're in the fixed timestep mode and not enough time has elapsed
        // to perform an update we sleep off the the remaining time to save battery
        // life and/or release CPU time to other threads and processes.
        if (this->isFixedTimeStep && this->accumulatedElapsedTime < this->targetElapsedTime) {
            int sleepTime = (int)((this->targetElapsedTime - this->accumulatedElapsedTime) * MillisecondsPerTick); //).TotalMilliseconds();
            if (sleepTime < 1) {
                break;
            }
            // NOTE: While sleep can be inaccurate in general it is
            // accurate enough for frame limiting purposes if some
            // fluctuation is an acceptable result.

            // #ifndef usleep
            // SDL_Delay(sleepTime);
            // #else
// #ifdef __EMSCRIPTEN__
// #else
//             usleep(sleepTime * 1000);
// #endif
            // #endif
            // goto RetryTick;
        } else
            break;
    }
    // Do not allow any update to take longer than our maximum.
    if (this->accumulatedElapsedTime > this->maxElapsedTime)
        this->accumulatedElapsedTime = this->maxElapsedTime;

    if (this->isFixedTimeStep) {
        this->elapsedGameTime = this->targetElapsedTime;
        int stepCount = 0;

        // Perform as many full fixed length time steps as we can.
        while (this->accumulatedElapsedTime >= this->targetElapsedTime && !this->shouldExit) {
            this->totalGameTime += this->targetElapsedTime;
            this->accumulatedElapsedTime -= this->targetElapsedTime;
            ++stepCount;
            this->delta = (double)this->elapsedGameTime * SecondsPerTick;
            Update(this);
            // this->virtual->Update(self);
        }
        //Every update after the first accumulates lag
        this->updateFrameLag += Max(0, stepCount - 1);
        //If we think we are isRunning slowly, wait until the lag clears before resetting it
        if (this->isRunningSlowly) {
            if (this->updateFrameLag == 0)

                this->isRunningSlowly = false;
        } else if (this->updateFrameLag >= 5) {
            //If we lag more than 5 frames, start thinking we are isRunning slowly
            this->isRunningSlowly = true;
        }
        //Every time we just do one update and one draw, then we are not isRunning slowly, so decrease the lag
        if (stepCount == 1 && this->updateFrameLag > 0)
            this->updateFrameLag--;

        // Draw needs to know the total elapsed time
        // that occured for the fixed length updates.
        this->elapsedGameTime = this->targetElapsedTime * stepCount;
    } else {
        // Perform a single variable length update.
        this->elapsedGameTime = this->accumulatedElapsedTime;
        this->totalGameTime += this->accumulatedElapsedTime;
        this->accumulatedElapsedTime = 0;

        this->delta = (double)this->elapsedGameTime * SecondsPerTick;
        Update(this);
        // this->virtual->Update(self);
    }
    // Draw unless the update suppressed it.
    if (this->suppressDraw)
        this->suppressDraw = false;
    else {
        Draw(this);
    }

    if (this->shouldExit || glfwWindowShouldClose(this->window))
        this->isRunning = false;
    // Platform.Exit();
}

/**
 * CFXGame::RunLoop
 */
method void RunLoop(CFXGameRef const this)
{
    HandleEvents(this);
    // if (this->keys[SDLK_ESCAPE]) {
    //     this->shouldExit = true;
    // }
    Tick(this);
}

/**
 * CFXGame::Run
 */
method void Run(CFXGameRef const this)
{
    Initialize(this);
    LoadContent(this);
    Start(this);
#if __EMSCRIPTEN__
    emscripten_set_main_loop_arg((em_arg_callback_func)RunLoop, (void*)this, -1, 1);
#else
    while (this->isRunning) {
        RunLoop(this);
    }
#endif
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///// gamepad events
////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// 

bool onclick_handler_dpad_up(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = true;
    this->keys[GLFW_KEY_DOWN] = false;
    return EM_TRUE;
}

bool onclick_handler_dpad_down(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = false;
    this->keys[GLFW_KEY_DOWN] = true;
    return EM_TRUE;
}

bool onclick_handler_dpad_left(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = true;
    this->keys[GLFW_KEY_RIGHT] = false;
    return EM_TRUE;
}

bool onclick_handler_dpad_right(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = false;
    this->keys[GLFW_KEY_RIGHT] = true;
    return EM_TRUE;
}

bool onclick_handler_button_a(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_SPACE] = true;
    return EM_TRUE;
}


bool touchstart_handler_dpad_up(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = true;
    this->keys[GLFW_KEY_DOWN] = false;
    return EM_TRUE;
}

bool touchstart_handler_dpad_down(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = false;
    this->keys[GLFW_KEY_DOWN] = true;
    return EM_TRUE;
}

bool touchstart_handler_dpad_left(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = true;
    this->keys[GLFW_KEY_RIGHT] = false;
    return EM_TRUE;
}

bool touchstart_handler_dpad_right(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = false;
    this->keys[GLFW_KEY_RIGHT] = true;
    return EM_TRUE;
}

bool touchstart_handler_button_a(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_SPACE] = true;
    return EM_TRUE;
}


bool touchend_handler_dpad_up(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = false;
    this->keys[GLFW_KEY_DOWN] = false;
    return EM_TRUE;
}

bool touchend_handler_dpad_down(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = false;
    this->keys[GLFW_KEY_DOWN] = false;
    return EM_TRUE;
}

bool touchend_handler_dpad_left(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = false;
    this->keys[GLFW_KEY_RIGHT] = false;
    return EM_TRUE;
}

bool touchend_handler_dpad_right(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = false;
    this->keys[GLFW_KEY_RIGHT] = false;
    return EM_TRUE;
}

bool touchend_handler_button_a(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_SPACE] = false;
    return EM_TRUE;
}


bool touchcancel_handler_dpad_up(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = false;
    this->keys[GLFW_KEY_DOWN] = false;
    return EM_TRUE;
}

bool touchcancel_handler_dpad_down(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_UP] = false;
    this->keys[GLFW_KEY_DOWN] = false;
    return EM_TRUE;
}

bool touchcancel_handler_dpad_left(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = false;
    this->keys[GLFW_KEY_RIGHT] = false;
    return EM_TRUE;
}

bool touchcancel_handler_dpad_right(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_LEFT] = false;
    this->keys[GLFW_KEY_RIGHT] = false;
    return EM_TRUE;
}

bool touchcancel_handler_button_a(int eventType, const EmscriptenTouchEvent *touchEvent, void *userData)
{
    CFXGameRef this = userData;
    this->keys[GLFW_KEY_SPACE] = false;
    return EM_TRUE;
}
