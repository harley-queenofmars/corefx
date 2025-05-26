#pragma once
#include <emscripten.h>
#include <emscripten/html5.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <corefw.h>   // IWYU pragma: keep

/**
 * @brief A reference to a CoreFX class object.
 *
 * CFClassRef is typically used to refer to class definitions or types
 * within the CoreFX framework, enabling dynamic type handling and
 * reflection-like operations.
 */
extern CFClassRef CFXGame;

typedef struct __CFXGame* CFXGameRef;
typedef struct __CFXGameVtbl* CFXGameVtblRef;

extern CFXGameRef CFXGame_instance;

/**
 * @typedef CFXGameProc
 * @brief Function pointer type for game procedures.
 *
 * Represents a pointer to a function that takes a single void pointer argument (typically used to pass a context or "self" pointer)
 * and returns void. This is commonly used for callback mechanisms or to define game loop or event handler functions.
 *
 * @param self Pointer to the context or object instance associated with the game procedure.
 */
typedef void (*CFXGameProc)(void* self);
/**
 * @brief Virtual table structure for the CFXGame interface.
 *
 * This struct defines a set of function pointers representing the core
 * lifecycle methods of a game object. Each function takes a pointer to
 * the game instance (`self`) as its parameter.
 *
 * Members:
 * - Initialize:   Initializes the game instance.
 * - LoadContent:  Loads game assets and resources.
 * - Update:       Updates the game logic and state.
 * - Draw:         Renders the game to the screen.
 */
typedef struct __CFXGameVtbl {
    void (*Initialize)  (void* self);
    void (*LoadContent) (void* self);
    void (*Update)      (void* self);
    void (*Draw)        (void* self);
} __CFXGameVtbl;

/**
 * @struct __CFXGame
 * @brief Represents the main game object containing state and configuration for the game loop.
 *
 * This structure encapsulates all the necessary data for managing the game state, timing,
 * windowing, input, and rendering context. It is designed to be used with a virtual function
 * table for extensibility and supports both fixed and variable time step updates.
 *
 * Members:
 * - obj: Base object for inheritance or reference counting.
 * - subclass: Pointer to subclass-specific data.
 * - virtual: Pointer to the virtual function table for game methods.
 * - window: Pointer to the GLFW window associated with the game.
 * - title: Title of the game window.
 * - len: Length of the title string.
 * - keys: Array representing the state of keyboard keys.
 * - delta: Time elapsed since the last frame (in seconds).
 * - factor: Time scaling factor for updates.
 * - targetElapsedTime: Target time per frame (in ticks).
 * - accumulatedElapsedTime: Accumulated time not yet processed (in ticks).
 * - maxElapsedTime: Maximum time to process per frame (in ticks).
 * - totalGameTime: Total time since the game started (in ticks).
 * - elapsedGameTime: Time elapsed in the current frame (in ticks).
 * - currentTime: Current time (in ticks).
 * - previousTicks: Time at the previous frame (in ticks).
 * - x, y: Position of the window.
 * - width, height: Dimensions of the window.
 * - flags: Bitmask of game state flags.
 * - mouseX, mouseY: Current mouse position.
 * - mouseDown: Indicates if the mouse button is pressed.
 * - sdlVersion: SDL version used (if applicable).
 * - frameSkip: Number of frames to skip for performance.
 * - gl_major_version, gl_minor_version: OpenGL context version.
 * - isRunning: Indicates if the game loop is active.
 * - ticks: Current tick count.
 * - fps: Frames per second.
 * - isFixedTimeStep: Whether the game uses a fixed time step.
 * - isRunningSlowly: Indicates if the game is running slower than the target frame rate.
 * - updateFrameLag: Number of frames the update is lagging behind.
 * - shouldExit: Flag to signal the game should exit.
 * - suppressDraw: Flag to suppress rendering for the current frame.
 */
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


extern proc void* Ctor(
    CFXGameRef this, 
    char* cstr, 
    int width, 
    int height, 
    void* subclass, 
    CFXGameVtblRef vptr);

extern proc void HandleEvents(
    CFXGameRef const this);

extern proc char* ToString(
    CFXGameRef this);

extern proc void Start(
    CFXGameRef const this);

extern proc void Tick(
    CFXGameRef const this);

extern proc void RunLoop(
    CFXGameRef const this);

extern proc void Run(
    CFXGameRef const this);

static inline CFXGameRef NewCFXGame(char* cstr, int width, int height, void* subclass, CFXGameVtblRef vptr)
{
    return Ctor((CFXGameRef)CFCreate(CFXGame), cstr, width, height, subclass, vptr);
}

/**
 * @brief Calls the virtual Draw method for the given game instance.
 *
 * This function invokes the Draw method defined in the virtual table of the provided
 * CFXGameRef object, passing its subclass pointer. It is intended to dispatch the
 * drawing operation to the appropriate subclass implementation.
 *
 * @param this Reference to the CFXGameRef game object whose Draw method should be called.
 */
static inline proc void Draw(CFXGameRef const this)
{
    this->virtual->Draw(this->subclass);
}

/**
 * @brief Loads the game content for the specified game instance.
 *
 * This function calls the virtual LoadContent method of the game's virtual table,
 * passing the subclass instance. It is intended to be used for initializing or
 * loading resources required by the game.
 *
 * @param this Reference to the game instance whose content should be loaded.
 */
static inline proc void LoadContent(CFXGameRef const this)
{
    this->virtual->LoadContent(this->subclass);
}

/**
 * @brief Initializes the game instance.
 *
 * This function calls the virtual Initialize method of the game's virtual table,
 * passing the subclass instance. It is intended to set up any necessary state or
 * resources required for the game to run.
 *
 * @param this Reference to the CFXGame instance to initialize.
 */
static inline proc void Initialize(CFXGameRef const this)
{
    this->virtual->Initialize(this->subclass);
}

/**
 * @brief Calls the virtual Update method for the given game instance.
 *
 * This function invokes the Update method defined in the virtual table of the
 * provided CFXGameRef instance, passing its subclass as an argument. It is
 * intended to update the state of the game or its components as defined by the
 * specific subclass implementation.
 *
 * @param this Reference to the CFXGameRef game instance whose Update method should be called.
 */
static inline proc void Update(CFXGameRef const this)
{
    this->virtual->Update(this->subclass);
}

/**
 * @brief Handle the direction input from a gamepad when a mouse or touch event occurs.
 *
 * These functions are intended to be used as an event handler for mouse or touch events related to the gamepad control.
 *
 * @param eventType The type of the mouse event (e.g., click, mouse down, mouse up).
 * @param mouseEvent Pointer to the EmscriptenMouseEvent structure containing details about the mouse event.
 * @param userData Pointer to user-defined data passed to the handler.
 * @return true if the event was handled successfully; false otherwise.
 */
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
