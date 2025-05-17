#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include "../corefw/corefw.h"       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include <GLFW/glfw3.h>

class(CFXShader);

method void* Ctor(CFXShaderRef this, CFStringRef vShader, CFStringRef fShader)
{
    Compile(this, CFStringC(vShader), CFStringC(fShader));
    return this;
}
/**
 * Use shader
 */
method CFXShaderRef Use(CFXShaderRef this)
{
    glUseProgram(this->Id);
    return this;
}

/**
 * CheckCompileErrors
 * 
 * Checks if compilation or linking failed and if so, print the error logs
 */
void CheckCompileErrors(
    CFXShaderRef this,
    GLuint object,
    char* type)
{
    GLint success;
    GLchar infoLog[1024];
    if (strncmp(type, "PROGRAM", 7) != 0) {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, 1024, nullptr, infoLog);
            printf("| ERROR::SHADER: Compile-time error: type: %s\n%s\n", type, infoLog);
            printf(" -- --------------------------------------------------- --\n");
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, 1024, nullptr, infoLog);
            printf("| ERROR::SHADER: Link-time error: type: %s\n%s\n", type, infoLog);
            printf(" -- --------------------------------------------------- --\n");
        }
    }
}

/**
 * Compile
 * 
 * @param vertexSource vertex shader source code
 * @param fragmentSource fragment shader source code
 * 
 */
method void Compile(
    CFXShaderRef this,
    const GLchar* vShaderSrc,
    const GLchar* fShaderSrc)
{

    GLuint sVertex, sFragment;
    // Vertex CFXShader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vShaderSrc, nullptr);
    glCompileShader(sVertex);
    CheckCompileErrors(this, sVertex, "VERTEX");
    // Fragment CFXShader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fShaderSrc, nullptr);
    glCompileShader(sFragment);
    CheckCompileErrors(this, sFragment, "FRAGMENT");

    this->Id = glCreateProgram();
    glAttachShader(this->Id, sVertex);
    glAttachShader(this->Id, sFragment);
    glLinkProgram(this->Id);
    CheckCompileErrors(this, this->Id, "PROGRAM");
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
}

method void SetFloat(
    CFXShaderRef this,
    const GLchar* name,
    const GLfloat value,
    const GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform1f(glGetUniformLocation(this->Id, name), value);
}

method void SetFloat(
    CFXShaderRef this,
    const GLchar* name,
    const GLfloat value)
{
    SetFloat(this, name, value, true);
}

method void SetInteger(
    CFXShaderRef this,
    const GLchar* name,
    GLint value,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform1i(glGetUniformLocation(this->Id, name), value);
}

method void SetInteger(
    CFXShaderRef this,
    const GLchar* name,
    GLint value)
{
    SetInteger(this, name, value, true);
}

method void SetVector2(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform2f(glGetUniformLocation(this->Id, name), x, y);
}

method void SetVector2(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y)
{
    SetVector2(this, name, x, y, true);
}

method void SetVector2v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec2* vector,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform2fv(glGetUniformLocation(this->Id, name), 1, (GLfloat*)vector);
}

method void SetVector2v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec2* vector)
{
    SetVector2v(this, name, vector, true);
}

method void SetVector3(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform3f(glGetUniformLocation(this->Id, name), x, y, z);
}

method void SetVector3(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z)
{
    SetVector3(this, name, x, y, z, true);
}

method void SetVector3v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec3* vector,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform3fv(glGetUniformLocation(this->Id, name), 1, (GLfloat*)vector);
}

method void SetVector3v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec3* vector)
{
    SetVector3v(this, name, vector, true);
}

method void SetVector4(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat w,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform4f(glGetUniformLocation(this->Id, name), x, y, z, w);
}

method void SetVector4(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat w)
{
    SetVector4(this, name, x, y, z, w, true);
}

method void SetVector4v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec4* vector,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform4fv(glGetUniformLocation(this->Id, name), 1, (GLfloat*)vector);
}

method void SetVector4v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec4* vector)
{
    SetVector4v(this, name, vector, true);
}

method void SetMatrix(
    CFXShaderRef this,
    const GLchar* name,
    const Mat* matrix,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniformMatrix4fv(glGetUniformLocation(this->Id, name), 1, GL_FALSE, (GLfloat*)matrix);
}

method void SetMatrix(
    CFXShaderRef this,
    const GLchar* name,
    const Mat* matrix)
{
    SetMatrix(this, name, matrix, true);
}