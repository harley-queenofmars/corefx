#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <corefw.h>   // IWYU pragma: keep
#include "corefx.h"             // IWYU pragma: keep

class(CFXGame);

constexpr float TicksPerMillisecond = 10000.0f;
constexpr float MillisecondsPerTick = 1.0f / (TicksPerMillisecond);
constexpr float TicksPerSecond = TicksPerMillisecond * 1000.0f; // 10,000,000
constexpr float SecondsPerTick = 1.0f / (TicksPerSecond); // 0.0001

CFXGameRef CFXGame_instance;

/**
 * @brief Destructor function for the CFXGame object.
 *
 * This function releases resources associated with the CFXGame instance.
 * It frees dynamically allocated memory for the title and keys fields,
 * and terminates the GLFW library.
 *
 * @param self Pointer to the CFXGame instance to be destroyed.
 */
static void dtor(void* self)
{
    CFXGameRef this = self;

    free(this->title);
    free(this->keys);
    glfwTerminate();
}

/**
 * @brief GLFW key callback function for handling keyboard input in the game.
 *
 * This function is called whenever a key event occurs in the GLFW window.
 * - If the Escape key is pressed, it sets the window's "should close" flag to true, signaling the application to close.
 * - For all valid key codes (0 to 1023), it updates the corresponding entry in the game's key state array:
 *   - Sets the key state to true on key press.
 *   - Sets the key state to false on key release.
 *
 * @param window    Pointer to the GLFW window receiving the event.
 * @param key       The keyboard key that was pressed or released.
 * @param scancode  The system-specific scancode of the key.
 * @param action    GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT.
 * @param mode      Bit field describing which modifier keys were held down.
 */
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



/**
 * @brief Callback function to handle changes in the framebuffer size.
 *
 * This function is called whenever the window's framebuffer is resized,
 * such as when the window is resized by the user or when moving between
 * displays with different pixel densities (e.g., Retina displays).
 * It updates the OpenGL viewport to match the new framebuffer dimensions,
 * ensuring that rendering covers the entire window.
 *
 * @param window Pointer to the GLFW window whose framebuffer was resized.
 * @param width  The new width, in pixels, of the framebuffer.
 * @param height The new height, in pixels, of the framebuffer.
 */
void CFXGame_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * @brief Retrieves the current time in 100-nanosecond ticks since the Unix epoch.
 *
 * This function uses gettimeofday to obtain the current time with microsecond precision,
 * then converts the result to 100-nanosecond intervals (ticks).
 *
 * @return The current time as a 64-bit unsigned integer representing the number of 100-nanosecond ticks.
 */
static uint64_t GetTicks()
{
    struct timeval t;
    gettimeofday(&t, nullptr);

    uint64_t ts = t.tv_sec;
    uint64_t us = t.tv_usec;
    return ((ts * 1000000L) + us) * 10;
}

/**
 * @brief Constructor for the CFXGame object.
 *
 * Initializes the CFXGame structure with the provided parameters, sets up the GLFW window,
 * configures OpenGL context, and registers input callbacks for both keyboard and touch controls.
 * Also initializes various game state fields and random seed.
 *
 * @param this         Pointer to the CFXGame object to initialize.
 * @param cstr         Title string for the game window.
 * @param width        Width of the game window.
 * @param height       Height of the game window.
 * @param subclass     Pointer to a subclass or user data.
 * @param vptr         Pointer to the virtual function table for the game.
 * @return             Pointer to the initialized CFXGame object.
 */
proc void* Ctor(CFXGameRef this, char* cstr, int width, int height, void* subclass, CFXGameVtblRef vptr)
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

/**
 * Converts the given CFXGameRef object to its string representation.
 *
 * @param this A reference to the CFXGame object.
 * @return The title of the game as a string.
 */
proc char* ToString(CFXGameRef this)
{
    return this->title;
}

/**
 * @brief Starts the game by setting the isRunning flag to true.
 *
 * @param this A reference to the CFXGame instance to start.
 */
proc void Start(CFXGameRef const this)
{
    this->isRunning = true;
}

/**
 * @brief Handles input events for the game, specifically checking for the Escape key.
 *
 * This function checks if the Escape key is pressed. If it is, it sets the window
 * to close and updates the game's exit flag accordingly.
 *
 * @param this A reference to the game instance containing the window and state.
 */
proc void HandleEvents(CFXGameRef const this)
{
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(this->window, true);
        this->shouldExit = true;
        return;
    }
}

/**
 * @brief Advances the game simulation by one tick, handling both fixed and variable timestep updates.
 *
 * This function manages the main game loop tick, updating the game's timing state and invoking
 * update and draw routines as appropriate. It supports both fixed timestep and variable timestep
 * modes, ensuring consistent game logic updates and smooth rendering.
 *
 * The function performs the following tasks:
 * - Calculates the elapsed time since the last tick and accumulates it.
 * - In fixed timestep mode, sleeps if not enough time has passed to perform an update, to save CPU.
 * - Limits the maximum accumulated elapsed time to prevent spiral of death.
 * - In fixed timestep mode, performs as many fixed-length updates as possible, tracking lag and
 *   adjusting the isRunningSlowly flag if the game falls behind.
 * - In variable timestep mode, performs a single update with the accumulated elapsed time.
 * - Calls the Update() function to advance game logic.
 * - Calls the Draw() function unless drawing is suppressed.
 * - Checks for exit conditions and updates the running state accordingly.
 *
 * @param this Pointer to the game object (CFXGameRef) whose state is to be updated.
 */
proc void Tick(CFXGameRef const this)
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
 * @brief Runs the main game loop for the specified game instance.
 *
 * This function initializes the game, loads necessary content, and starts the game.
 * It then sets up the main loop using Emscripten, repeatedly calling the RunLoop
 * function with the game instance as its argument.
 *
 * @param this A reference to the game instance (CFXGameRef) to run.
 */
proc void Run(CFXGameRef const this)
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

/**
 * @brief Runs the main game loop for the specified game instance.
 *
 * This function processes input events and updates the game state
 * by calling HandleEvents() and Tick() in sequence.
 *
 * @param this A constant reference to the game instance (CFXGameRef).
 */
proc void RunLoop(CFXGameRef const this)
{
    HandleEvents(this);
    Tick(this);
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///// gamepad events
////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// 



/**
 * @brief Handles the "up" direction press event from a D-pad input.
 *
 * This function is intended to be used as an event handler for mouse or touch events
 * that simulate pressing the "up" direction on a D-pad. When triggered, it sets the
 * "up" key state to true and the "down" key state to false in the game's key state array.
 *
 * @param eventType The type of the event (e.g., mouse down, mouse up).
 * @param mouseEvent Pointer to the EmscriptenMouseEvent structure containing event data.
 * @param userData Pointer to the CFXGameRef game state structure.
 * @return EM_TRUE to indicate the event was handled.
 */
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


/**
 * @brief Handles touch start events for the D-pad up control.
 *
 * This function is called when a touch start event is detected on the D-pad up control.
 * It sets the "up" key state to true and the "down" key state to false in the game's key array.
 *
 * @param eventType The type of the touch event.
 * @param touchEvent Pointer to the EmscriptenTouchEvent structure containing event data.
 * @param userData Pointer to user data, expected to be a CFXGameRef instance.
 * @return EM_TRUE to indicate the event was handled.
 */
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


/**
 * @brief Handles the end of a touch event for the D-pad up control.
 *
 * This function is called when a touch event ends (e.g., the user lifts their finger)
 * on the D-pad up control. It updates the game's key state by setting both the
 * GLFW_KEY_UP and GLFW_KEY_DOWN keys to false, indicating that neither the up nor down
 * direction is being pressed.
 *
 * @param eventType The type of the touch event.
 * @param touchEvent Pointer to the EmscriptenTouchEvent structure containing event data.
 * @param userData Pointer to user data, expected to be a CFXGameRef instance.
 * @return EM_TRUE to indicate the event was handled.
 */
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


/**
 * @brief Handles the touch cancel event for the D-pad up control.
 *
 * This function is called when a touch cancel event occurs on the D-pad up control.
 * It resets the state of the up and down keys in the game's key array to false,
 * indicating that neither the up nor down direction is being pressed.
 *
 * @param eventType The type of the touch event.
 * @param touchEvent Pointer to the EmscriptenTouchEvent structure containing event data.
 * @param userData Pointer to user data, expected to be a CFXGameRef instance.
 * @return EM_TRUE to indicate the event was handled.
 */
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
