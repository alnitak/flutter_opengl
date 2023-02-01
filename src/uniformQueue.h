#ifndef UNIFORM_QUEUE_H
#define UNIFORM_QUEUE_H

#include "common.h"
#include "Sampler2D.h"

#include <iostream>
#include <vector>
#include <variant>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <cstdlib>
#include <new>
#include <any>
#include <map>
#include <utility>

#ifdef _IS_ANDROID_

#include <EGL/egl.h>
#include "GLES3/gl3.h"
#include "glm.hpp"

#endif
#ifdef _IS_LINUX_
#include <EGL/egl.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#endif
#ifdef _IS_WIN_
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#endif


typedef enum {
    UNIFORM_BOOL,
    UNIFORM_INT,
    UNIFORM_FLOAT,
    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,
    UNIFORM_MAT2,
    UNIFORM_MAT3,
    UNIFORM_MAT4,
    UNIFORM_SAMPLER2D
} UniformType;

void setBool(const std::string &name, GLuint po, bool value);

void setInt(const std::string &name, GLuint po, int value);

void setFloat(const std::string &name, GLuint po, float value);

void setVec2(const std::string &name, GLuint po, const glm::vec2 &value);

void setVec2(const std::string &name, GLuint po, float x, float y);

void setVec3(const std::string &name, GLuint po, const glm::vec3 &value);

void setVec3(const std::string &name, GLuint po, float x, float y, float z);

void setVec4(const std::string &name, GLuint po, const glm::vec4 &value);

void setVec4(const std::string &name, GLuint po, float x, float y, float z, float w);

void setMat2(const std::string &name, GLuint po, const glm::mat2 &mat);

void setMat3(const std::string &name, GLuint po, const glm::mat3 &mat);

void setMat4(const std::string &name, GLuint po, const glm::mat4 &mat);

void setSampler2D(const std::string &name, GLuint po, Sampler2D &data);

class UniformQueue {
public:
    UniformQueue();

    ~UniformQueue();

    void debug(const std::string &name);

    void setProgram(GLuint po) { this->programObject = po; }

    void addUniform(std::string name, UniformType type, void *val);

    bool setUniformValue(const std::string &, void *val);

    void sendAllUniforms();

    void setAllSampler2D();

    template<typename T>
    struct UniformStruct {
        UniformType type;
        T val;

        UniformStruct(UniformType type, const T &data)
                : type(type), val(data) {};
    };

    typedef UniformStruct<bool> UNIFORM_BOOL_t;
    typedef UniformStruct<int> UNIFORM_INT_t;
    typedef UniformStruct<float> UNIFORM_FLOAT_t;
    typedef UniformStruct<glm::vec2> UNIFORM_VEC2_t;
    typedef UniformStruct<glm::vec3> UNIFORM_VEC3_t;
    typedef UniformStruct<glm::vec4> UNIFORM_VEC4_t;
    typedef UniformStruct<glm::mat2> UNIFORM_MAT2_t;
    typedef UniformStruct<glm::mat3> UNIFORM_MAT3_t;
    typedef UniformStruct<glm::mat4> UNIFORM_MAT4_t;
    typedef UniformStruct<Sampler2D> UNIFORM_SAMPLER2D_t;

    std::map<std::string, std::any> uniforms;

private:
    static GLuint programObject;
};

#endif // UNIFORM_QUEUE_H