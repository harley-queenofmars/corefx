#pragma once
#include "../corefw/corefw.h"       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include "shader.h"
#include "texture2d.h"

extern CFClassRef CFXResourceManager;

typedef struct __CFXResourceManager* CFXResourceManagerRef;
/**
 *  class CFXResourceManager
 */
typedef struct __CFXResourceManager {
    __CFObject obj;
    CFMapRef Shaders;
    CFMapRef Textures;
    CFMapRef Fonts;
} __CFXResourceManager;

extern method void* Ctor(
    CFXResourceManagerRef this);

extern method CFXShaderRef LoadShader(
    const CFXResourceManagerRef this,
    const GLchar* vShaderFile,
    const GLchar* fShaderFile,
    const char* name);

extern method CFXShaderRef GetShader(
    const CFXResourceManagerRef this,
    const char* name);

extern method CFXTexture2DRef LoadTexture(
    const CFXResourceManagerRef this,
    const GLchar* file,
    GLboolean alpha,
    const char* name);

extern method CFXTexture2DRef GetTexture(
    const CFXResourceManagerRef this,
    const char* name);

static inline CFXResourceManagerRef NewCFXResourceManager()
{
    return Ctor((CFXResourceManagerRef)CFCreate(CFXResourceManager));
}
