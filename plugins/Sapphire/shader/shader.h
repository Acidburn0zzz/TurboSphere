#ifndef SAPPHIRE_SHADER_HEAD
#define SAPPHIRE_SHADER_HEAD
#include "../frontend.h"
#include "../loadfunc/gl_extra.h"
#include "../../common/plugin.h"



typedef GLuint TS_Shader;
typedef GLuint TS_ShaderProg;


extern GLint CurrentColorAttrib;
extern GLint CurrentVertexAttrib;
extern GLint CurrentTexcoordAttrib;

struct TS_ScriptShader {
    GLuint name;
    const char *source;
    int used_in_prog;
    GLenum type;
};

struct TS_ShaderS {
    GLuint name;
    TS_ScriptShader* fragment;
    TS_ScriptShader* vertex;
    GLint vertexAttrib;
    GLint colorAttrib;
    GLint texcoordAttrib;
};

extern TS_Shader    TS_CurrentShader;
extern TS_Shader    TS_CurrentCompositeShader;

//void TS_ScriptShaderFinalizer(V8FINALIZERARGS(TS_ScriptShader));
//void TS_ScriptShaderProgramFinalizer(V8FINALIZERARGS(TS_ShaderS));

TS_Shader TS_LoadSystemShader(const char *file);
TS_Shader TS_LoadShader(const char *file, GLuint *frag = NULL, GLuint *vert = NULL);
TS_ShaderProg TS_CreateProgram(TS_Shader frag, TS_Shader vert);
TS_Shader TS_CreateShader(const char *text, GLenum type, char **error_text=NULL);

#endif
