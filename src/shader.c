#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <corefw.h>       // IWYU pragma: keep
#include "corefx.h"                 // IWYU pragma: keep
#include <GLFW/glfw3.h>

class(CFXShader);

/**
 * @brief Constructor function for CFXShaderRef objects.
 *
 * Initializes a shader object by compiling the provided vertex and fragment shader source code.
 *
 * @param this Pointer to the CFXShaderRef object to initialize.
 * @param vShader Reference to the vertex shader source code as a CFStringRef.
 * @param fShader Reference to the fragment shader source code as a CFStringRef.
 * @return Pointer to the initialized CFXShaderRef object.
 */
proc void* Ctor(CFXShaderRef this, CFStringRef vShader, CFStringRef fShader)
{
    Compile(this, CFStringC(vShader), CFStringC(fShader));
    return this;
}

/**
 * Activates the specified shader program for subsequent rendering operations.
 *
 * This function calls glUseProgram with the shader program's ID, making it the current
 * active shader program in the OpenGL context. It returns the same shader reference for
 * possible chaining or further use.
 *
 * @param this A reference to the shader program to activate.
 * @return The same shader reference passed as input.
 */
proc CFXShaderRef Use(CFXShaderRef this)
{
    glUseProgram(this->Id);
    return this;
}

/**
 * @brief Checks and reports shader compilation or program linking errors.
 *
 * This function inspects the compilation status of a shader or the linking status of a shader program.
 * If an error is detected, it retrieves and prints the error log to the standard output.
 *
 * @param this   Reference to the shader object (unused in this function).
 * @param object The OpenGL shader or program object to check.
 * @param type   A string indicating the type of object ("PROGRAM" for program objects, otherwise treated as shader).
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
 * @brief Compiles and links vertex and fragment shaders into a shader program.
 *
 * This function creates, compiles, and attaches vertex and fragment shaders from the provided source code,
 * links them into a shader program, and stores the resulting program ID in the given CFXShaderRef object.
 * It also checks for compilation and linking errors, and deletes the individual shader objects after linking.
 *
 * @param this Pointer to the CFXShaderRef object where the program ID will be stored.
 * @param vShaderSrc Source code for the vertex shader.
 * @param fShaderSrc Source code for the fragment shader.
 */
proc void Compile(
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

/**
 * @brief Sets a float uniform variable in the shader program.
 *
 * This function sets the value of a float uniform variable specified by its name
 * in the given shader program. Optionally, it can activate the shader before setting
 * the uniform.
 *
 * @param this        Reference to the shader object.
 * @param name        Name of the uniform variable in the shader.
 * @param value       Float value to set for the uniform variable.
 * @param useShader   If GL_TRUE, the shader will be activated before setting the uniform.
 */
proc void SetFloat(
    CFXShaderRef this,
    const GLchar* name,
    const GLfloat value,
    const GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform1f(glGetUniformLocation(this->Id, name), value);
}

/**
 * @brief Sets a float uniform variable in the shader.
 *
 * This function sets the value of a float uniform variable identified by its name
 * in the specified shader object.
 *
 * @param this   Reference to the shader object.
 * @param name   Name of the uniform variable to set.
 * @param value  Float value to assign to the uniform variable.
 */
proc void SetFloat(
    CFXShaderRef this,
    const GLchar* name,
    const GLfloat value)
{
    SetFloat(this, name, value, true);
}

/**
 * @brief Sets an integer uniform variable in the shader program.
 *
 * This function sets the value of an integer uniform variable specified by its name
 * in the given shader program. Optionally, it can activate the shader before setting
 * the uniform.
 *
 * @param this       Reference to the shader object.
 * @param name       Name of the uniform variable to set.
 * @param value      Integer value to assign to the uniform variable.
 * @param useShader  If GL_TRUE, the shader program is activated before setting the uniform.
 */
proc void SetInteger(
    CFXShaderRef this,
    const GLchar* name,
    GLint value,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform1i(glGetUniformLocation(this->Id, name), value);
}

/**
 * @brief Sets an integer uniform variable in the shader.
 *
 * This function sets the value of an integer uniform variable identified by its name
 * in the specified shader object. It calls an overloaded version of SetInteger with
 * an additional parameter set to true.
 *
 * @param this   Reference to the shader object.
 * @param name   Name of the uniform variable to set.
 * @param value  Integer value to assign to the uniform variable.
 */
proc void SetInteger(
    CFXShaderRef this,
    const GLchar* name,
    GLint value)
{
    SetInteger(this, name, value, true);
}

/**
 * @brief Sets a vec2 uniform variable in the shader.
 *
 * This function sets the value of a 2-component vector (vec2) uniform variable in the specified shader program.
 * Optionally, it can activate the shader before setting the uniform.
 *
 * @param this        Reference to the shader object.
 * @param name        Name of the uniform variable in the shader.
 * @param x           First component (x) of the vector.
 * @param y           Second component (y) of the vector.
 * @param useShader   If GL_TRUE, the shader will be activated before setting the uniform.
 */
proc void SetVector2(
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

/**
 * @brief Sets a 2-component vector uniform variable in the shader.
 *
 * This function sets the value of a vec2 uniform variable identified by its name
 * in the specified shader. The vector components are provided as x and y.
 *
 * @param this  Reference to the shader object.
 * @param name  Name of the uniform variable in the shader.
 * @param x     The x component of the vector.
 * @param y     The y component of the vector.
 */
proc void SetVector2(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y)
{
    SetVector2(this, name, x, y, true);
}

proc void SetVector2v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec2* vector,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform2fv(glGetUniformLocation(this->Id, name), 1, (GLfloat*)vector);
}

proc void SetVector2v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec2* vector)
{
    SetVector2v(this, name, vector, true);
}

proc void SetVector3(
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

proc void SetVector3(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z)
{
    SetVector3(this, name, x, y, z, true);
}

proc void SetVector3v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec3* vector,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform3fv(glGetUniformLocation(this->Id, name), 1, (GLfloat*)vector);
}

proc void SetVector3v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec3* vector)
{
    SetVector3v(this, name, vector, true);
}

proc void SetVector4(
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

proc void SetVector4(
    CFXShaderRef this,
    const GLchar* name,
    GLfloat x,
    GLfloat y,
    GLfloat z,
    GLfloat w)
{
    SetVector4(this, name, x, y, z, w, true);
}

proc void SetVector4v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec4* vector,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniform4fv(glGetUniformLocation(this->Id, name), 1, (GLfloat*)vector);
}

proc void SetVector4v(
    CFXShaderRef this,
    const GLchar* name,
    const Vec4* vector)
{
    SetVector4v(this, name, vector, true);
}

/**
 * @brief Sets a 4x4 matrix uniform variable in the shader program.
 *
 * This function uploads the provided 4x4 matrix to the shader uniform variable specified by name.
 * Optionally, it can activate the shader program before setting the uniform.
 *
 * @param this        Reference to the shader object.
 * @param name        Name of the uniform variable in the shader.
 * @param matrix      Pointer to the 4x4 matrix to be set.
 * @param useShader   If GL_TRUE, the shader program is activated before setting the uniform.
 */
proc void SetMatrix(
    CFXShaderRef this,
    const GLchar* name,
    const Mat* matrix,
    GLboolean useShader)
{
    if (useShader)
        Use(this);
    glUniformMatrix4fv(glGetUniformLocation(this->Id, name), 1, GL_FALSE, (GLfloat*)matrix);
}

/**
 * @brief Sets a matrix uniform variable in the shader.
 *
 * This function sets the value of a matrix uniform variable identified by its name
 * in the specified shader. It uses the default behavior for transposing the matrix.
 *
 * @param this   Reference to the shader object.
 * @param name   Name of the matrix uniform variable in the shader.
 * @param matrix Pointer to the matrix to be set.
 */
proc void SetMatrix(
    CFXShaderRef this,
    const GLchar* name,
    const Mat* matrix)
{
    SetMatrix(this, name, matrix, true);
}