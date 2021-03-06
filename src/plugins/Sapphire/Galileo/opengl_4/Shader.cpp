#include "Shader.hpp"
#include <cstdio>

#include <cassert>

#include <opengame.h>

namespace Sapphire {

  namespace embed {
    static const char * const FragShaderSource = "\
    #version 410\n\
    \
    in vec4 color_v;\n\
    in vec2 tex_v;\n\
    out vec4 fragColour;\n\
    \
    uniform sampler2D texDat;\n\
    \
    void main(void)\n\
    {\n\
        fragColour = color_v * texture(texDat, tex_v);\n\
    }\
    ";

    static const char * const VertexShaderSource = "\
    #version 410\n\
    \
    uniform vec2 TS_Offset;\n\
    uniform vec2 TS_RotOffset;\n\
    uniform float TS_RotAngle;\n\
    \
    in vec2 TS_TextureUV;\n\
    in vec4 TS_Position;\n\
    in vec4 TS_Color;\n\
    \
    out vec4 color_v;\n\
    out vec2 tex_v;\n\
    \
    uniform vec2 TS_ScreenSize;\n\
    \
    vec2 TS_ApplyRotation(vec2 A_position, vec2 A_offset, float A_angle){\n\
        vec2 L_offset = (A_position - A_offset);\n\
        if(L_offset.x==0){\n\
            L_offset.x=-0.1;\n\
        }\n\
        \
        float L_modangle = atan(L_offset.y, L_offset.x)+A_angle;\n\
        L_offset *= L_offset;\n\
        float L_distance = sqrt(L_offset.x+L_offset.y);\n\
        \
        return (A_offset+(vec2(cos(L_modangle), sin(L_modangle))*L_distance));\n\
        \
    }\n\
    \n\
    void main (void){\n\
        // Passthroughs.\n\
        tex_v = TS_TextureUV;\n\
        color_v = TS_Color;\n\
        \n\
        // Calculate the rotation.\n\
        vec4 TS_NewPosition = TS_Position;\n\
        TS_NewPosition.xy = TS_ApplyRotation(TS_Position.xy, TS_RotOffset, TS_RotAngle);\
        \n\
        // Apply translation and normalize.\n\
        vec4 TS_NewPos = TS_NewPosition*2.0;\n\
        gl_Position = (vec4(TS_Offset.x*2.0, TS_Offset.y*(-2.0), 0.0, 0.0) +(vec4(TS_NewPos.x - TS_ScreenSize.x, -TS_NewPos.y + (TS_ScreenSize.y), TS_NewPosition.ba)))/vec4(TS_ScreenSize, 1.0, 1.0);\n\
    }\
    ";
  }

namespace SDL_GL {


GLuint CreateShader(const char *text, GLenum type){
    if(text==NULL){
        printf("[Shader] Error: Emtpy string given.\n");
        return 0;
    }
    GLuint shader = glCreateShader(type);
    if(shader==0){
        fprintf(stderr, "[Shader] Error: Something went terribly wrong, the shader index was 0.\n");
    }
    GLsizei tsize = (GLsizei)strlen(text)+2;

    glShaderSource(shader, 1, &text, &tsize);

    glCompileShader(shader);

    GLint shader_status;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);

    if (shader_status==GL_FALSE) {
        printf("[Shader] Error: Failed to compile shader.\n");

        GLint log_size;
        GLint written = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
        char *log_text = new char[log_size];
        glGetShaderInfoLog(shader, log_size, &written, log_text);
        if(written==0)
            printf("[Shader] Error: No log was written.\n");
        if(written>log_size)
            printf("[Shader] Error: Your OpenGL driver just wrote past the end of my buffer. I told it not to, really!\n");
        printf("%s\n", log_text);
        delete[] log_text;
        glDeleteShader(shader);

        return 0;
    }
    printf("[Shader] Info: Shader compiled ok. ID number %i.\n", shader);
    return shader;

}

GLuint CreateProgram(GLuint frag, GLuint vert){
    if((glIsShader(frag)==GL_FALSE)||(glIsShader(vert)==GL_FALSE)){
        fprintf(stderr, "[Shader] Error: One or more shader was invalid\n\tFrag %s\tVert %s\n",
        (glIsShader(frag)==GL_TRUE)?"good":"bad", (glIsShader(vert)==GL_TRUE)?"good":"bad");
        return 0;
    }

    GLint program_status;

    GLint fragstat, vertstat;

    glGetShaderiv(frag, GL_SHADER_TYPE, &fragstat);

    if(fragstat!=GL_FRAGMENT_SHADER)
        fprintf(stderr, "[Shader] Error: Invalid fragment shader.\n");

    glGetShaderiv(vert, GL_SHADER_TYPE, &vertstat);

    if(vertstat!=GL_VERTEX_SHADER)
        fprintf(stderr, "[Shader] Error: Invalid vertex shader.\n");

    if((fragstat!=GL_FRAGMENT_SHADER)||(vertstat!=GL_VERTEX_SHADER)){
        fprintf(stderr, "[Shader] Error: Bad shader(s). Exiting.\n");
        return 0;

    }

    GLuint prog = glCreateProgram();

    glAttachShader(prog, frag);
    glAttachShader(prog, vert);
    fprintf(stderr, "[Shader] Info: Linking Program.\n");
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &program_status);

    if(!program_status){
        fprintf(stderr, "[Shader] Error: Could not link program.\n");
        GLint log_size;
        char *log_text;

        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);
        log_text = new char[log_size+1];
        glGetProgramInfoLog(prog, log_size, NULL, log_text);
        fprintf(stderr, "%s\n", log_text);
        delete[] log_text;
        glDeleteProgram(prog);

        return 0;

    }
    fprintf(stderr, "[Shader] Info: Program linked ok.\n");
    return prog;

}

GLuint DefaultFragShader(void){
    return CreateShader(embed::FragShaderSource, GL_FRAGMENT_SHADER);
}

GLuint DefaultVertShader(void){
    return CreateShader(embed::VertexShaderSource, GL_VERTEX_SHADER);
}

GLuint CreateDefaultProgram(void){
    return CreateProgram(DefaultFragShader(), DefaultVertShader());
}

}

namespace Galileo{

Shader::~Shader(){
    glDeleteShader(mHandle);
}

bool Shader::Compile(std::string &err){
    
    if(mSource.empty()){
        err = " emtpy shader source";
        return false;
    }
    mHandle = glCreateShader(mType);
    if(mHandle==0){
        err = "shader index was 0";
        return false;
    }
    
    GLsizei tsize = (GLsizei)mSource.size()+1;
    
    const GLchar * text = static_cast<const GLchar *>(mSource.c_str());
    glShaderSource(mHandle, 1, &text, &tsize);

    glCompileShader(mHandle);

    GLint shader_status;

    glGetShaderiv(mHandle, GL_COMPILE_STATUS, &shader_status);

    if(shader_status==GL_FALSE) {
        err =  "failed to compile shader: ";

        GLint log_size;
        GLint written = 0;

        glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &log_size);
        char *log_text = new char[log_size];
        glGetShaderInfoLog(mHandle, log_size, &written, log_text);
        if(written==0)
            err += "[No log was written]";
        else
            err += log_text;
        delete[] log_text;
        glDeleteShader(mHandle);
        return false;
    }
    return true;
}

ShaderParamChange::ShaderParamChange()
  : mLocation(-1)
  , mCount(0)
  , mData(nullptr)
  , mCallback(nullptr)
{

}

ShaderParamChange::ShaderParamChange(int aLocation, size_t aCount, const void *aData,
                      callback_t aCallback, size_t s)
  : mLocation(aLocation)
  , mCount(aCount)
  , mData(malloc(mCount))
  , mCallback(aCallback)
{
    memcpy(mData, aData, mCount*s);
}

ShaderParamChange::~ShaderParamChange(){
    free(mData);
}

const std::string ShaderProgram::ShaderPositionName = "TS_Position";
const std::string ShaderProgram::ShaderTextureUVName= "TS_TextureUV";
const std::string ShaderProgram::ShaderColorName    = "TS_Color";
const std::string ShaderProgram::ShaderOffsetUniformName    = "TS_Offset";
const std::string ShaderProgram::ShaderRotOffsetUniformName    = "TS_RotOffset";
const std::string ShaderProgram::ShaderAngleUniformName    = "TS_RotAngle";
const std::string ShaderProgram::ShaderScreenSizeUniformName    = "TS_ScreenSize";

#ifdef _MSC_VER
	__declspec( thread ) ShaderProgram *ShaderProgram::BoundShader = nullptr;
#else
    __thread ShaderProgram *ShaderProgram::BoundShader = nullptr;
#endif

ShaderProgram *ShaderProgram::GetDefault(void *ctx){

    GLuint lDefaultProg = SDL_GL::CreateDefaultProgram();
    if((lDefaultProg)!=0){

        ShaderProgram *lShader = new ShaderProgram(TS_GetContextEnvironment(ctx)->config, lDefaultProg);

        return lShader;
    }
    else
      return nullptr;
}

ShaderProgram::ShaderProgram(TS_GameConfig *config, int aProgram)
  : mProgram(aProgram){
    mAttributes = GLSLValueMap();
    mUniforms   = GLSLValueMap();

    glUseProgram(mProgram);
    // Add the default uniforms and attribs, if the shader defines them.
    AddUniform(ShaderOffsetUniformName);
    AddUniform(ShaderRotOffsetUniformName);
    AddUniform(ShaderAngleUniformName);
    AddUniform(ShaderScreenSizeUniformName);
    AddAttribute(ShaderPositionName);
    AddAttribute(ShaderTextureUVName);
    AddAttribute(ShaderColorName);

    glUniform2f(FindUniform(ShaderScreenSizeUniformName), config->screenwidth, config->screenheight);

}

ShaderProgram::ShaderProgram(TS_GameConfig *config, unsigned shaderc, std::shared_ptr<Shader> shaderv[]){
    shaders.resize(shaderc);
    mProgram = glCreateProgram();
    for(unsigned i = 0; i<shaderc; i++){
        shaders[i] = shaderv[i];
        glAttachShader(mProgram, shaderv[i]->GetHandle());
    }
}

ShaderProgram::~ShaderProgram() {}

bool ShaderProgram::Link(std::string &err){
    glLinkProgram(mProgram);
    
    GLint program_status;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &program_status);

    if(!program_status){
        err = "could not link program";
        GLint log_size;
        char *log_text;

        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &log_size);
        log_text = new char[log_size+1];
        glGetProgramInfoLog(mProgram, log_size, NULL, log_text);
        err += log_text;
        delete[] log_text;
        glDeleteProgram(mProgram);
        
        mProgram = -1;
        
        return false;

    }
    return true;
}

/*
// Map of Vertex Attribute names and their locations.
GLSLValueMap mAttributes;
// Map of Uniform names and their locations.
GLSLValueMap mUniforms;
*/
// Checks for the specified name in the shader program.
// Returns whether or not the name exists.
// If it exists, it will be added to the appropriate map.
bool ShaderProgram::AddUniform(const std::string &aName){
    int lLocation = glGetUniformLocation(mProgram, aName.c_str());
    assert(lLocation>=0);
    if(lLocation>=0){
        mUniforms.push_back({aName, lLocation});
        return true;
    }
    else
        return false;
}

bool ShaderProgram::AddAttribute(const std::string &aName){
    int lLocation = glGetAttribLocation(mProgram, aName.c_str());
    assert(lLocation>=0);
    if(lLocation>=0){
        mAttributes.push_back({aName, lLocation});
        mAttributeNumbers.push_back(lLocation);
        return true;
    }
    else
        return false;
}

void ShaderProgram::Bind(void){
    
    if(BoundShader==this) return;
    BoundShader = this;
    
    assert(mProgram);
    glUseProgram(mProgram);

    for(GLSLValueMap::const_iterator i = mAttributes.cbegin(); i!=mAttributes.cend(); i++){
        assert(i->a>=0);
        glEnableVertexAttribArray(i->a);
    }
}

}
}
