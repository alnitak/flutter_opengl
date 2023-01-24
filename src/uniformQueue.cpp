#include "uniformQueue.h"

#include <iterator>
#include <iomanip>      // for setw
#include <any>
#include <iostream>

#ifdef _IS_WIN_
#define CAST(T,X) reinterpret_cast<T>(X)
#else
#define CAST(T, X) any_cast<T>(X)
#endif

GLuint UniformQueue::programObject = 0;

UniformQueue::UniformQueue() {
}

UniformQueue::~UniformQueue() {
    // uniforms.clear();
}

void UniformQueue::debug(const string &name) {
    auto uniform = uniforms[name];
    const type_info &t = uniforms[name].type();
    cout;
    if (t == typeid(UNIFORM_BOOL_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "bool: " <<
             left << setw(10) << CAST(UNIFORM_BOOL_t &, uniform).val <<
             endl;
    } else
    if (t == typeid(UNIFORM_INT_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "int: " <<
             left << setw(10) << CAST(UNIFORM_INT_t &, uniform).val <<
             endl;
    } else
    if (t == typeid(UNIFORM_FLOAT_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "float: " <<
             left << setw(10) << CAST(UNIFORM_FLOAT_t &, uniform).val <<
             endl;
    } else 
    if (t == typeid(UNIFORM_VEC2_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "vec2: " <<
             left << setw(10) << CAST(UNIFORM_VEC2_t &, uniform).val.x << " " <<
             left << setw(10) << CAST(UNIFORM_VEC2_t &, uniform).val.y <<
             endl;
    } else 
    if (t == typeid(UNIFORM_VEC3_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "vec3: " <<
             left << setw(10) << CAST(UNIFORM_VEC3_t &, uniform).val.x << " " <<
             left << setw(10) << CAST(UNIFORM_VEC3_t &, uniform).val.y << " " <<
             left << setw(10) << CAST(UNIFORM_VEC3_t &, uniform).val.z <<
             endl;
    } else 
    if (t == typeid(UNIFORM_VEC4_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "vec4: " <<
             left << setw(10) << CAST(UNIFORM_VEC4_t &, uniform).val.x << " " <<
             left << setw(10) << CAST(UNIFORM_VEC4_t &, uniform).val.y << " " <<
             left << setw(10) << CAST(UNIFORM_VEC4_t &, uniform).val.z << " " <<
             left << setw(10) << CAST(UNIFORM_VEC4_t &, uniform).val.w <<
             endl;
    } else 
    if (t == typeid(UNIFORM_MAT2_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "mat2: " <<
             left << setw(10) << CAST(UNIFORM_MAT2_t &, uniform).val[0][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT2_t &, uniform).val[0][1] << " " << endl <<
             left << setw(10) << CAST(UNIFORM_MAT2_t &, uniform).val[1][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT2_t &, uniform).val[1][1] <<
             endl;
    } else 
    if (t == typeid(UNIFORM_MAT3_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "mat3: " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[0][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[0][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[0][2] << " " << endl <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[1][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[1][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[1][2] << " " << endl <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[2][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[2][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT3_t &, uniform).val[2][2] <<
             endl;
    } else 
    if (t == typeid(UNIFORM_MAT4_t)) {
        cout << left << setw(15) << name <<
             left << setw(7) << "mat4: " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[0][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[0][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[0][2] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[0][3] << " " << endl <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[1][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[1][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[1][2] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[1][3] << " " << endl <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[2][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[2][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[2][2] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[2][3] << " " << endl <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[3][0] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[3][1] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[3][2] << " " <<
             left << setw(10) << CAST(UNIFORM_MAT4_t &, uniform).val[3][3] <<
             endl;
    }
}


// Add a uniform to the shader
void UniformQueue::addUniform(string name, UniformType type, void *val) {
    // check if the uniform already exists
    if (uniforms.find(name) != uniforms.end()) {
        cout << "Uniform \"" << name << "\"  already exists!" << endl;
        return;
    }

    // cout << "ADD " << name << endl;
    switch (type) {
        case UNIFORM_BOOL: {
            bool f = *(bool *) val;
            uniforms.emplace(name, UniformStruct<bool>(UNIFORM_BOOL, f));
            break;
        }
        case UNIFORM_INT: {
            int f = *(int *) val;
            uniforms.emplace(name, UniformStruct<int>(UNIFORM_INT, f));
            break;
        }
        case UNIFORM_FLOAT: {
            float f = *(float *) val;
            uniforms.emplace(name, UniformStruct<float>(UNIFORM_FLOAT, f));
            break;
        }
        case UNIFORM_VEC2: {
            glm::vec2 f = *(glm::vec2 *) val;
            uniforms.emplace(name, UNIFORM_VEC2_t(UNIFORM_VEC2, f));
            break;
        }
        case UNIFORM_VEC3: {
            glm::vec3 f = *(glm::vec3 *) val;
            uniforms.emplace(name, UNIFORM_VEC3_t(UNIFORM_VEC3, f));
            break;
        }
        case UNIFORM_VEC4: {
            glm::vec4 f = *(glm::vec4 *) val;
            uniforms.emplace(name, UNIFORM_VEC4_t(UNIFORM_VEC4, f));
            break;
        }
        case UNIFORM_MAT2: {
            glm::mat2 f = *(glm::mat2 *) val;
            uniforms.emplace(name, UNIFORM_MAT2_t(UNIFORM_MAT2, f));
            break;
        }
        case UNIFORM_MAT3: {
            glm::mat3 f = *(glm::mat3 *) val;
            uniforms.emplace(name, UNIFORM_MAT3_t(UNIFORM_MAT3, f));
            break;
        }
        case UNIFORM_MAT4: {
            glm::mat4 f = *(glm::mat4 *) val;
            uniforms.emplace(name, UNIFORM_MAT4_t(UNIFORM_MAT4, f));
            break;
        }
    }
    setUniformValue(name, val);
}

// Set new value to an existing uniform
bool UniformQueue::setUniformValue(const string &name, void *val) {
    bool found = false;
    const type_info &t = uniforms[name].type();

    if (t == typeid(UNIFORM_BOOL_t)) {
        float f = *(float *) val;
        CAST(UNIFORM_BOOL_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_INT_t)) {
        float f = *(float *) val;
        CAST(UNIFORM_INT_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_FLOAT_t)) {
        float f = *(float *) val;
        CAST(UNIFORM_FLOAT_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_VEC2_t)) {
        glm::vec2 f = *(glm::vec2 *) val;
        CAST(UNIFORM_VEC2_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_VEC3_t)) {
        glm::vec3 f = *(glm::vec3 *) val;
        CAST(UNIFORM_VEC3_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_VEC4_t)) {
        glm::vec4 f = *(glm::vec4 *) val;
        CAST(UNIFORM_VEC4_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_MAT2_t)) {
        glm::mat2 f = *(glm::mat2 *) val;
        CAST(UNIFORM_MAT2_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_MAT3_t)) {
        glm::mat3 f = *(glm::mat3 *) val;
        CAST(UNIFORM_MAT3_t &, uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_MAT4_t)) {
        glm::mat4 f = *(glm::mat4 *) val;
        CAST(UNIFORM_MAT4_t &, uniforms[name]).val = f;
        found = true;
    } else 
    {
        cout << "Uniform \"" << name << "\"  not found!" << endl;
    }

    return found;
}

void UniformQueue::sendAllUniforms() {
    for (auto &[name, uniform]: uniforms) {
        const type_info &t = uniforms[name].type();

        if (t == typeid(UNIFORM_BOOL_t)) {
            setBool(name, programObject, CAST(UNIFORM_BOOL_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_INT_t)) {
            setInt(name, programObject, CAST(UNIFORM_INT_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_FLOAT_t)) {
            setFloat(name, programObject, CAST(UNIFORM_FLOAT_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_VEC2_t)) {
            setVec2(name, programObject, CAST(UNIFORM_VEC2_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_VEC3_t)) {
            setVec3(name, programObject, CAST(UNIFORM_VEC3_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_VEC4_t)) {
            setVec4(name, programObject, CAST(UNIFORM_VEC4_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_MAT2_t)) {
            setMat2(name, programObject, CAST(UNIFORM_MAT2_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_MAT3_t)) {
            setMat3(name, programObject, CAST(UNIFORM_MAT3_t &, uniform).val);
        } else 
        if (t == typeid(UNIFORM_MAT4_t)) {
            setMat4(name, programObject, CAST(UNIFORM_MAT4_t &, uniform).val);
        }
        // debug(name);
    }
}

// ------------------------------------------------------------------------
void setBool(const string &name, GLuint po, bool value) {
    glUniform1i(glGetUniformLocation(po, name.c_str()), (int) value);
}

// ------------------------------------------------------------------------
void setInt(const string &name, GLuint po, int value) {
    glUniform1i(glGetUniformLocation(po, name.c_str()), value);
}

// ------------------------------------------------------------------------
void setFloat(const string &name, GLuint po, float value) {
    glUniform1f(glGetUniformLocation(po, name.c_str()), value);
}

// ------------------------------------------------------------------------
void setVec2(const string &name, GLuint po, const glm::vec2 &value) {
    glUniform2fv(glGetUniformLocation(po, name.c_str()), 1, &value[0]);
}

void setVec2(const string &name, GLuint po, float x, float y) {
    glUniform2f(glGetUniformLocation(po, name.c_str()), x, y);
}

// ------------------------------------------------------------------------
void setVec3(const string &name, GLuint po, const glm::vec3 &value) {
    glUniform3fv(glGetUniformLocation(po, name.c_str()), 1, &value[0]);
}

void setVec3(const string &name, GLuint po, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(po, name.c_str()), x, y, z);
}

// ------------------------------------------------------------------------
void setVec4(const string &name, GLuint po, const glm::vec4 &value) {
    glUniform4fv(glGetUniformLocation(po, name.c_str()), 1, &value[0]);
}

void setVec4(const string &name, GLuint po, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(po, name.c_str()), x, y, z, w);
}

// ------------------------------------------------------------------------
void setMat2(const string &name, GLuint po, const glm::mat2 &mat) {
    glUniformMatrix2fv(glGetUniformLocation(po, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void setMat3(const string &name, GLuint po, const glm::mat3 &mat) {
    glUniformMatrix3fv(glGetUniformLocation(po, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void setMat4(const string &name, GLuint po, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(po, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}