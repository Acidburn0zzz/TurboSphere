#pragma once

#include <string>
#include <vector>
#include <map>
#include <color.h>
#include "Shape.hpp"

#ifdef OS_X
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

namespace Sapphire {
namespace Galileo{

class ShaderParamChange : public Sapphire::Galileo::GL::Operation {
public:
    typedef void (*callback_t)(GLint, GLsizei, const void *);
protected:
    int mLocation;
    size_t mCount;
    void *mData;
    callback_t mCallback;

public:
    ShaderParamChange();
    ShaderParamChange(int aLocation, size_t aCount, const void *aData,
                      callback_t aCallback, size_t s);
    ~ShaderParamChange();

    virtual int Draw(){
        mCallback(mLocation, mCount, mData);
        return 0;
    }

    virtual bool IsPersistent(void){
        return false;
    }


};

class Shader : public Sapphire::Galileo::GL::Operation {

public:
  static const std::string ShaderPositionName;
  static const std::string ShaderTextureUVName;
  static const std::string ShaderColorName;
  static const std::string ShaderOffsetUniformName;
  static const std::string ShaderRotOffsetUniformName;
  static const std::string ShaderAngleUniformName;
  static const std::string ShaderScreenSizeUniformName;
  typedef std::map<std::string, int> GLSLValueMap;
protected:
    int mProgram;
    std::vector<int> mAttributeNumbers;
public:

    static Shader *GetDefaultShader(void);

    Shader(int aProgram);
    ~Shader();

    // Map of Vertex Attribute names and their locations.
    GLSLValueMap mAttributes;
    // Map of Uniform names and their locations.
    GLSLValueMap mUniforms;

    // Checks for the specified name in the shader program.
    // Returns whether or not the name exists.
    // If it exists, it will be added to the appropriate map.
    bool AddUniform(const std::string &aName);
    bool AddAttribute(const std::string &aName);

    void Bind(void);

    virtual int Draw(){
        Bind();
        return 0;
    }

    int GetName(){ return mProgram; }

    static __thread std::vector<int> *EnabledAttributes;
    static __thread Shader *BoundShader;

};


}
}