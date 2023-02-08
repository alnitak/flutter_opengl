#include "uniformQueue.h"
#include "Shader.h"
#include "ffi.h"

#include <iterator>
#include <iomanip>      // for setw
#include <any>
#include <iostream>

// #ifdef _IS_WIN_
// #define std::any_cast<T,X>(reinterpret_cast<T>(X)
// #else
// // if doesn't compile, update visual studio
// // https://developercommunity.visualstudio.com/t/stdany-doesnt-link-when-exceptions-are-disabled/376072
// #define CAST<T, X>( any_cast<T>(X)
// #endif

GLuint UniformQueue::programObject = 0;

UniformQueue::UniformQueue() {
}

UniformQueue::~UniformQueue() {
    // delete all Sampler2D textures
    for (auto &[name, uniform]: uniforms) {
        const std::type_info &t = uniforms[name].type();

        if (t == typeid(UNIFORM_SAMPLER2D_t)) {
            Sampler2D &sampler = std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val;
            glDeleteTextures(1, &sampler.texture_index);
        }
    }
}

void UniformQueue::debug(const std::string &name) {
    auto uniform = uniforms[name];
    const std::type_info &t = uniforms[name].type();
    
    if (t == typeid(UNIFORM_BOOL_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "bool: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_BOOL_t &>(uniform).val <<
             std::endl;
    } else
    if (t == typeid(UNIFORM_INT_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "int: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_INT_t &>(uniform).val <<
             std::endl;
    } else
    if (t == typeid(UNIFORM_FLOAT_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "float: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_FLOAT_t &>(uniform).val <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_VEC2_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "vec2: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC2_t &>(uniform).val.x << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC2_t &>(uniform).val.y <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_VEC3_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "vec3: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC3_t &>(uniform).val.x << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC3_t &>(uniform).val.y << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC3_t &>(uniform).val.z <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_VEC4_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "vec4: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC4_t &>(uniform).val.x << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC4_t &>(uniform).val.y << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC4_t &>(uniform).val.z << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_VEC4_t &>(uniform).val.w <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_MAT2_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "mat2: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT2_t &>(uniform).val[0][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT2_t &>(uniform).val[0][1] << " " << std::endl <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT2_t &>(uniform).val[1][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT2_t &>(uniform).val[1][1] <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_MAT3_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "mat3: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[0][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[0][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[0][2] << " " << std::endl <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[1][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[1][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[1][2] << " " << std::endl <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[2][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[2][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT3_t &>(uniform).val[2][2] <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_MAT4_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "mat4: " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[0][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[0][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[0][2] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[0][3] << " " << std::endl <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[1][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[1][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[1][2] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[1][3] << " " << std::endl <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[2][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[2][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[2][2] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[2][3] << " " << std::endl <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[3][0] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[3][1] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[3][2] << " " <<
             std::left << std::setw(10) << std::any_cast<UNIFORM_MAT4_t &>(uniform).val[3][3] <<
             std::endl;
    } else 
    if (t == typeid(UNIFORM_SAMPLER2D_t)) {
        std::cout << std::left << std::setw(15) << name <<
             std::left << std::setw(7) << "Sampler2D: " << 
             std::left << std::setw(4) << std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val.width << " x " <<
             std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val.height << 
             "  texture index: " <<
             std::left << std::setw(6) << std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val.texture_index << 
             "  N texture: " <<
             std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val.nTexture << 
             std::endl;
    }
}


// Add a uniform to the shader
bool UniformQueue::addUniform(std::string name, UniformType type, void *val) {
    // check if the uniform already exists
    if (/*type != UNIFORM_SAMPLER2D && */uniforms.find(name) != uniforms.end()) {
        std::cout << "Uniform \"" << name << "\"  already exists!" << std::endl;
        return false;
    }

    std::cout << "ADD " << name << std::endl;
    switch (type) {
        case UNIFORM_BOOL: {
            bool f = *(bool *) val;
            uniforms.emplace(name, UNIFORM_BOOL_t(UNIFORM_BOOL, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_INT: {
            int f = *(int *) val;
            uniforms.emplace(name, UNIFORM_INT_t(UNIFORM_INT, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_FLOAT: {
            float f = *(float *) val;
            uniforms.emplace(name, UNIFORM_FLOAT_t(UNIFORM_FLOAT, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_VEC2: {
            glm::vec2 f = *(glm::vec2 *) val;
            uniforms.emplace(name, UNIFORM_VEC2_t(UNIFORM_VEC2, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_VEC3: {
            glm::vec3 f = *(glm::vec3 *) val;
            uniforms.emplace(name, UNIFORM_VEC3_t(UNIFORM_VEC3, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_VEC4: {
            glm::vec4 f = *(glm::vec4 *) val;
            uniforms.emplace(name, UNIFORM_VEC4_t(UNIFORM_VEC4, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_MAT2: {
            glm::mat2 f = *(glm::mat2 *) val;
            uniforms.emplace(name, UNIFORM_MAT2_t(UNIFORM_MAT2, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_MAT3: {
            glm::mat3 f = *(glm::mat3 *) val;
            uniforms.emplace(name, UNIFORM_MAT3_t(UNIFORM_MAT3, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_MAT4: {
            glm::mat4 f = *(glm::mat4 *) val;
            uniforms.emplace(name, UNIFORM_MAT4_t(UNIFORM_MAT4, f));
            setUniformValue(name, val);
            break;
        }
        case UNIFORM_SAMPLER2D: {
            const Sampler2D f = *(Sampler2D*)(val);
            uniforms.emplace(name, UNIFORM_SAMPLER2D_t(UNIFORM_SAMPLER2D, f));
            break;
        }
    }
    return true;
}

// Remove the uniform with name [name]
bool UniformQueue::removeUniform(const std::string &name)
{
    // check if the uniform already exists
    if (uniforms.find(name) == uniforms.end()) {
        std::cout << "Uniform \"" << name << "\"  doesn't exists!" << std::endl;
        return false;
    }

    if (uniforms[name].type() == typeid(UNIFORM_SAMPLER2D_t)) {
        Sampler2D &f = std::any_cast<UNIFORM_SAMPLER2D_t &>(uniforms[name]).val;
        if (getRenderer() != nullptr && getRenderer()->isLooping())
            getRenderer()->deleteTextureMsg(f.texture_index);
        else
            glDeleteTextures(1, &f.texture_index);
    }
    uniforms.erase(uniforms.find(name));
    return true;
}

// Set new value to an existing uniform
bool UniformQueue::setUniformValue(const std::string &name, void *val) {
    bool found = false;
    const std::type_info &t = uniforms[name].type();

    if (t == typeid(UNIFORM_BOOL_t)) {
        float f = *(float *) val;
        std::any_cast<UNIFORM_BOOL_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_INT_t)) {
        float f = *(float *) val;
        std::any_cast<UNIFORM_INT_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_FLOAT_t)) {
        float f = *(float *) val;
        std::any_cast<UNIFORM_FLOAT_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_VEC2_t)) {
        glm::vec2 f = *(glm::vec2 *) val;
        std::any_cast<UNIFORM_VEC2_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_VEC3_t)) {
        glm::vec3 f = *(glm::vec3 *) val;
        std::any_cast<UNIFORM_VEC3_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_VEC4_t)) {
        glm::vec4 f = *(glm::vec4 *) val;
        std::any_cast<UNIFORM_VEC4_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_MAT2_t)) {
        glm::mat2 f = *(glm::mat2 *) val;
        std::any_cast<UNIFORM_MAT2_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_MAT3_t)) {
        glm::mat3 f = *(glm::mat3 *) val;
        std::any_cast<UNIFORM_MAT3_t &>(uniforms[name]).val = f;
        found = true;
    } else 
    if (t == typeid(UNIFORM_MAT4_t)) {
        glm::mat4 f = *(glm::mat4 *) val;
        std::any_cast<UNIFORM_MAT4_t &>(uniforms[name]).val = f;
        found = true;
    } else  
    if (t == typeid(UNIFORM_SAMPLER2D_t)) {
        std::cout << "UPDATE Sampler2D " << name << std::endl;
        Sampler2D &f = std::any_cast<UNIFORM_SAMPLER2D_t &>(uniforms[name]).val;
        f.add_RGBA32(f.width, f.height, (unsigned char *) val);
        if (getRenderer() != nullptr && getRenderer()->isLooping())
            getRenderer()->setNewTextureMsg();
        found = true;
    } else 
    {
        std::cout << "Uniform \"" << name << "\"  not found!" << std::endl;
    }

    return found;
}

void UniformQueue::sendAllUniforms() {
    for (auto &[name, uniform]: uniforms) {
        const std::type_info &t = uniforms[name].type();

        if (t == typeid(UNIFORM_BOOL_t)) {
            setBool(name, programObject, std::any_cast<UNIFORM_BOOL_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_INT_t)) {
            setInt(name, programObject, std::any_cast<UNIFORM_INT_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_FLOAT_t)) {
            setFloat(name, programObject, std::any_cast<UNIFORM_FLOAT_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_VEC2_t)) {
            setVec2(name, programObject, std::any_cast<UNIFORM_VEC2_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_VEC3_t)) {
            setVec3(name, programObject, std::any_cast<UNIFORM_VEC3_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_VEC4_t)) {
            setVec4(name, programObject, std::any_cast<UNIFORM_VEC4_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_MAT2_t)) {
            setMat2(name, programObject, std::any_cast<UNIFORM_MAT2_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_MAT3_t)) {
            setMat3(name, programObject, std::any_cast<UNIFORM_MAT3_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_MAT4_t)) {
            setMat4(name, programObject, std::any_cast<UNIFORM_MAT4_t &>(uniform).val);
        } else 
        if (t == typeid(UNIFORM_SAMPLER2D_t)) {
            setSampler2D(name, programObject, std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val);
            // debug(name);
        }
    }
}

// Called once when a new texture is set. Could be from Shader::initShader()
// or from Renderer loop with MSG_NEW_TEXTURE message
void UniformQueue::setAllSampler2D()
{
    // make a vector to store all th nTexture number already used
    std::vector<int> nTextures;
    for (auto &[name, uniform]: uniforms) {
        const std::type_info &t = uniforms[name].type();
        if (t == typeid(UNIFORM_SAMPLER2D_t)) {
            Sampler2D sampler = std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val;
            nTextures.push_back(sampler.nTexture);
        }
    }

    int n = 0;
    for (auto &[name, uniform]: uniforms) {
        const std::type_info &t = uniforms[name].type();

        if (t == typeid(UNIFORM_SAMPLER2D_t)) {
            Sampler2D &sampler = std::any_cast<UNIFORM_SAMPLER2D_t &>(uniform).val;
            if (!sampler.data.empty())
            {
                // Find a new [nTexture] if not already set
                // If it is already set the new sampler.data is given to replace the existing
                if (sampler.nTexture == -1)
                {
                    while ( n < 32 && std::find(nTextures.begin(), nTextures.end(), n) != nTextures.end() )
                        n++;
                } else
                    n = sampler.nTexture;

                if (n < 32) {
                    // std::cout << "setAllSampler2D " << name << "  nTexture: " << n << "  size: " << sampler.width << "x" << sampler.height << std::endl;
                    sampler.genTexture(n);
                    setSampler2D(name, programObject, sampler);
                    nTextures.push_back(n);
                    // debug(name);
                }
            }
        }
    }
}

bool UniformQueue::replaceSampler2D(const std::string &name, int w, int h, unsigned char *rawData)
{
    // check if the uniform already exists
    if (uniforms.find(name) == uniforms.end()) {
        std::cout << "Uniform \"" << name << "\"  doesn't exists!" << std::endl;
        return false;
    }
    const std::type_info &t = uniforms[name].type();

    if (t == typeid(UNIFORM_SAMPLER2D_t)) {
        Sampler2D &sampler = std::any_cast<UNIFORM_SAMPLER2D_t &>(uniforms[name]).val;
        std::cout << "replaceSampler2D \"" << name << std::endl;
        sampler.replaceTexture(w, h, rawData);
        return true;
    }
    return false;
}

Sampler2D *UniformQueue::getSampler2D(const std::string &name)
{
    if (uniforms.find(name) == uniforms.end()) {
        std::cout << "Uniform \"" << name << "\"  doesn't exists!" << std::endl;
        return nullptr;
    }

    const std::type_info &t = uniforms[name].type();
    if (t == typeid(UNIFORM_SAMPLER2D_t)) {
        return &std::any_cast<UNIFORM_SAMPLER2D_t &>(uniforms[name]).val;
    }
    std::cout << "Uniform \"" << name << "\"  is not a Sampler2D!" << std::endl;
    return nullptr;
}


// ------------------------------------------------------------------------
void UniformQueue::setBool(const std::string &name, GLuint po, bool value) {
    glUniform1i(glGetUniformLocation(po, name.c_str()), (int) value);
}

// ------------------------------------------------------------------------
void UniformQueue::setInt(const std::string &name, GLuint po, int value) {
    glUniform1i(glGetUniformLocation(po, name.c_str()), value);
}

// ------------------------------------------------------------------------
void UniformQueue::setFloat(const std::string &name, GLuint po, float value) {
    glUniform1f(glGetUniformLocation(po, name.c_str()), value);
}

// ------------------------------------------------------------------------
void UniformQueue::setVec2(const std::string &name, GLuint po, const glm::vec2 &value) {
    glUniform2fv(glGetUniformLocation(po, name.c_str()), 1, &value[0]);
}

void UniformQueue::setVec2(const std::string &name, GLuint po, float x, float y) {
    glUniform2f(glGetUniformLocation(po, name.c_str()), x, y);
}

// ------------------------------------------------------------------------
void UniformQueue::setVec3(const std::string &name, GLuint po, const glm::vec3 &value) {
    glUniform3fv(glGetUniformLocation(po, name.c_str()), 1, &value[0]);
}

void UniformQueue::setVec3(const std::string &name, GLuint po, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(po, name.c_str()), x, y, z);
}

// ------------------------------------------------------------------------
void UniformQueue::setVec4(const std::string &name, GLuint po, const glm::vec4 &value) {
    glUniform4fv(glGetUniformLocation(po, name.c_str()), 1, &value[0]);
}

void UniformQueue::setVec4(const std::string &name, GLuint po, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(po, name.c_str()), x, y, z, w);
}

// ------------------------------------------------------------------------
void UniformQueue::setMat2(const std::string &name, GLuint po, const glm::mat2 &mat) {
    glUniformMatrix2fv(glGetUniformLocation(po, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void UniformQueue::setMat3(const std::string &name, GLuint po, const glm::mat3 &mat) {
    glUniformMatrix3fv(glGetUniformLocation(po, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void UniformQueue::setMat4(const std::string &name, GLuint po, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(po, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void UniformQueue::setSampler2D(const std::string &name, GLuint po, Sampler2D &data) {

    glUniform1i(
        glGetUniformLocation(po, name.c_str()),
        data.nTexture
    );

    glActiveTexture(GL_TEXTURE0 + data.nTexture);
    glBindTexture(GL_TEXTURE_2D, data.texture_index);
}