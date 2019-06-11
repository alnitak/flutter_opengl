#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <memory.h>
#include <math.h>

#include "common.hpp"
#include "shaders.h"

ShadersCommonParams shadersCommonParams;

// shader variables indexes
GLuint indexPosition;
GLint indexColor;
GLint indexMVPMatrix;
GLint indexResolution;
GLint indexTime;
GLint indexChannel0;
GLuint indexChannel0_texture;
GLint indexTextureCoords;


void eglPrintError(std::string note, EGLint error)
{
    std::string ret;
    switch(error)
    {
        case EGL_SUCCESS: ret = "No error"; break;
        case EGL_NOT_INITIALIZED: ret = "EGL not initialized or failed to initialize"; break;
        case EGL_BAD_ACCESS: ret = "Resource inaccessible"; break;
        case EGL_BAD_ALLOC: ret = "Cannot allocate resources"; break;
        case EGL_BAD_ATTRIBUTE: ret = "Unrecognized attribute or attribute value"; break;
        case EGL_BAD_CONTEXT: ret = "Invalid EGL context"; break;
        case EGL_BAD_CONFIG: ret = "Invalid EGL frame buffer configuration"; break;
        case EGL_BAD_CURRENT_SURFACE: ret = "Current surface is no longer valid"; break;
        case EGL_BAD_DISPLAY: ret = "Invalid EGL display"; break;
        case EGL_BAD_SURFACE: ret = "Invalid surface"; break;
        case EGL_BAD_MATCH: ret = "Inconsistent arguments"; break;
        case EGL_BAD_PARAMETER: ret = "Invalid argument"; break;
        case EGL_BAD_NATIVE_PIXMAP: ret = "Invalid native pixmap"; break;
        case EGL_BAD_NATIVE_WINDOW: ret = "Invalid native window"; break;
        case EGL_CONTEXT_LOST: ret = "Context lost"; break;
        default: ret = "Unknown error";
    }
    if (error == EGL_SUCCESS)
        LOGD("%s  %s", note.c_str(), ret.c_str());
    else
        LOGE("%s  error: %d  0x%X  %s", note.c_str(), error, error, ret.c_str());
}

void setShadersCommonParamsWidth(GLsizei width) {
    shadersCommonParams.width = width;
}
void setShadersCommonParamsHeight(GLsizei height) {
    shadersCommonParams.height = height;
}

ShadersCommonParams getShadersCommonParams() {
    return shadersCommonParams;
}


///////////////////////////////////
/// INITIALIZE SHADER FUNCTIONS
///////////////////////////////////

///
/// Create a shader object, load the shader source, and
/// compile the shader.
///
GLuint LoadShader( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader( type );

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource( shader, 1, &shaderSrc, NULL );

    // Compile the shader
    glCompileShader( shader );

    // Check the compile status
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

    if ( !compiled )
    {
        GLint infoLen = 0;

        glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 )
        {
            char *infoLog = (char*)malloc( sizeof(char) * infoLen );

            glGetShaderInfoLog( shader, infoLen, NULL, infoLog );
            LOGE( "Error compiling shader:\n%s\n", infoLog );

            free( infoLog );
        }

        glDeleteShader ( shader );
        return 0;
    }

    return shader;

}

///
/// Initialize the shader and program object
///
bool initShaderDefaults(void *args)
{
    EGLint err = eglGetError();
    if (err != 0x3000) {
        fprintf(stderr, "Unable to query EGL surface dimensions\n");
        return EXIT_FAILURE;
    }

    glClearDepthf(1.0F);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    //set clear color to black
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

    glViewport(0, 0, shadersCommonParams.width, shadersCommonParams.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float nearClip = -2.0F;
    float farClip  = 2.0F;
    float yFOV  = 75.0F;
    float yMax = nearClip * tan(yFOV*M_PI/360.0F);
    float aspect = shadersCommonParams.width/shadersCommonParams.height;
    float xMin = -yMax * aspect;
    float xMax = yMax *aspect;


    if (shadersCommonParams.width > shadersCommonParams.height)
        glScalef((float)shadersCommonParams.height/(float)shadersCommonParams.width, 1.0F, 1.0F);
    else
        glScalef(1.0, (float)shadersCommonParams.width/(float)shadersCommonParams.height, 1.0F);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return true;
}

bool initShader1(void *args) {
//    https://www.shadertoy.com/view/3l23Rh
    char vShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "attribute vec4 a_Position;         \n"     // Per-vertex position information we will pass in.

            "uniform vec3  u_resolution;       \n"
            "uniform float u_time;             \n"

            "varying vec3       v_resolution;  \n"     // viewport resolution (in pixels)
            "varying float      v_time;        \n"

            "void main()                       \n"     // The entry point for our vertex shader.
            "{                                 \n"
            "   gl_Position = a_Position;      \n"
            "   v_resolution = u_resolution;   \n"
            "   v_time = u_time;               \n"
            "}                                 \n";
    char fShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}\n"
            "const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;\n"
            "float mag2(vec2 p){return dot(p,p);}\n"
            "float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }\n"
            "float prm1 = 0.;\n"
            "vec2 bsMo = vec2(0);\n"
            ""
            "varying vec3      v_resolution; \n"     // viewport resolution (in pixels)
            "varying float     v_time;       \n"     // shader playback time (in seconds)
            ""
            "vec2 disp(float t){ return vec2(sin(t*0.22)*1., cos(t*0.175)*1.)*2.; }\n"
            ""
            "vec2 map(vec3 p)\n"
            "{\n"
            "    vec3 p2 = p;\n"
            "    p2.xy -= disp(p.z).xy;\n"
            "    p.xy *= rot(sin(p.z+v_time)*(0.1 + prm1*0.05) + v_time*0.09);\n"
            "    float cl = mag2(p2.xy);\n"
            "    float d = 0.;\n"
            "    p *= .61;\n"
            "    float z = 1.;\n"
            "    float trk = 1.;\n"
            "    float dspAmp = 0.1 + prm1*0.2;\n"
            "    for(int i = 0; i < 5; i++)\n"
            "    {\n"
            "        p += sin(p.zxy*0.75*trk + v_time*trk*.8)*dspAmp;\n"
            "        d -= abs(dot(cos(p), sin(p.yzx))*z);\n"
            "        z *= 0.57;\n"
            "        trk *= 1.4;\n"
            "        p = p*m3;\n"
            "    }\n"
            "    d = abs(d + prm1*3.)+ prm1*.3 - 2.5 + bsMo.y;\n"
            "    return vec2(d + cl*.2 + 0.25, cl);\n"
            "}\n"
            ""
            "vec4 render( in vec3 ro, in vec3 rd, float time )\n"
            "{\n"
            "   vec4 rez = vec4(0);\n"
            "   const float ldst = 8.;\n"
            "   vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst);\n"
            "   float t = 1.5;\n"
            "   float fogT = 0.;\n"
            "   for(int i=0; i<130; i++)\n"
            "   {\n"
            "       if(rez.a > 0.99)break;\n"
            ""
            "       vec3 pos = ro + t*rd;\n"
            "       vec2 mpv = map(pos);\n"
            "       float den = clamp(mpv.x-0.3,0.,1.)*1.12;\n"
            "       float dn = clamp((mpv.x + 2.),0.,3.);\n"
            ""
            "       vec4 col = vec4(0);\n"
            "        if (mpv.x > 0.6)\n"
            "        {\n"
            ""
            "            col = vec4(sin(vec3(5.,0.4,0.2) + mpv.y*0.1 +sin(pos.z*0.4)*0.5 + 1.8)*0.5 + 0.5,0.08);\n"
            "            col *= den*den*den;\n"
            "            col.rgb *= linstep(4.,-2.5, mpv.x)*2.3;\n"
            "            float dif =  clamp((den - map(pos+.8).x)/9., 0.001, 1. );\n"
            "            dif += clamp((den - map(pos+.35).x)/2.5, 0.001, 1. );\n"
            "            col.xyz *= den*(vec3(0.005,.045,.075) + 1.5*vec3(0.033,0.07,0.03)*dif);\n"
            "        }\n"
            ""
            "       float fogC = exp(t*0.2 - 2.2);\n"
            "       col.rgba += vec4(0.06,0.11,0.11, 0.1)*clamp(fogC-fogT, 0., 1.);\n"
            "       fogT = fogC;\n"
            "       rez = rez + col*(1. - rez.a);\n"
            "       t += clamp(0.5 - dn*dn*.05, 0.09, 0.3);\n"
            "   }\n"
            "   return clamp(rez, 0.0, 1.0);\n"
            "}\n"
            ""
            "float getsat(vec3 c)\n"
            "{\n"
            "    float mi = min(min(c.x, c.y), c.z);\n"
            "    float ma = max(max(c.x, c.y), c.z);\n"
            "    return (ma - mi)/(ma+ 1e-7);\n"
            "}\n"
            "\n"
            "vec3 iLerp(in vec3 a, in vec3 b, in float x)\n"
            "{\n"
            "    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);\n"
            "    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));\n"
            "    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));\n"
            "    float lgt = dot(vec3(1.0), ic);\n"
            "    float ff = dot(dir, normalize(ic));\n"
            "    ic += 1.5*dir*sd*ff*lgt;\n"
            "    return clamp(ic,0.,1.);\n"
            "}\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec2 q = gl_FragCoord.xy/v_resolution.xy;\n"
            "    vec2 p = (gl_FragCoord.xy - 0.5*v_resolution.xy)/v_resolution.y;\n"
            "    bsMo = (0.5*v_resolution.xy)/v_resolution.y;\n"
            ""
            "    float time = v_time*3.;\n"
            "    vec3 ro = vec3(0,0,time);\n"
            ""
            "    ro += vec3(sin(v_time)*0.5,sin(v_time*1.)*0.,0);\n"
            ""
            "    float dspAmp = .85;\n"
            "    ro.xy += disp(ro.z)*dspAmp;\n"
            "    float tgtDst = 3.5;\n"
            ""
            "    vec3 target = normalize(ro - vec3(disp(time + tgtDst)*dspAmp, time + tgtDst));\n"
            "    ro.x -= bsMo.x*2.;\n"
            "    vec3 rightdir = normalize(cross(target, vec3(0,1,0)));\n"
            "    vec3 updir = normalize(cross(rightdir, target));\n"
            "    rightdir = normalize(cross(updir, target));\n"
            "    vec3 rd=normalize((p.x*rightdir + p.y*updir)*1. - target);\n"
            "    rd.xy *= rot(-disp(time + 3.5).x*0.2 + bsMo.x);\n"
            "    prm1 = smoothstep(-0.4, 0.4,sin(v_time*0.3));\n"
            "    vec4 scn = render(ro, rd, time);\n"
            ""
            "    vec3 col = scn.rgb;\n"
            "    col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));\n"
            "    \n"
            "    col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);\n"
            ""
            "    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3;\n"
            ""
            "    gl_FragColor = vec4( col, 1.0 );\n"
            "}";


    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader   = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    if (shadersCommonParams.programObject != 0)
        glDeleteProgram(shadersCommonParams.programObject);
    shadersCommonParams.programObject  = glCreateProgram();

    if(shadersCommonParams.programObject == 0) {
        eglPrintError("initShader1() shadersCommonParams.shadersCommonParams.programObject == 0", eglGetError());
        return 0;
    }

    glAttachShader(shadersCommonParams.programObject, vertexShader);
    glAttachShader(shadersCommonParams.programObject, fragmentShader);


    // Bind to attributes
    glBindAttribLocation(shadersCommonParams.programObject, 0, "u_iChannel0");
    glBindAttribLocation(shadersCommonParams.programObject, 1, "u_resolution");
    glBindAttribLocation(shadersCommonParams.programObject, 2, "u_time");

    glGenTextures(0, &indexChannel0_texture);
    glBindTexture(GL_TEXTURE_2D, indexChannel0_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 31, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
// turn off filtering and wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Link the program
    glLinkProgram(shadersCommonParams.programObject);

    // shaders are no longer needed, even if they are deleted when calling glDeleteProgram()
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Check the link status
    glGetProgramiv(shadersCommonParams.programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;

        glGetProgramiv(shadersCommonParams.programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(shadersCommonParams.programObject, infoLen, NULL, infoLog);
            LOGE("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(shadersCommonParams.programObject);
        return false;
    }

    // get indexes to pass to glEnableVertexAttribArray()
    indexChannel0   = glGetUniformLocation(shadersCommonParams.programObject, "u_iChannel0");
    glUniform1i(indexChannel0, 0);
    indexResolution = glGetUniformLocation(shadersCommonParams.programObject, "u_resolution");
    indexTime       = glGetUniformLocation(shadersCommonParams.programObject, "u_time");

    // Use the program object
    glUseProgram ( shadersCommonParams.programObject );

    return true;
}

bool initShader2(void *args) {
//    https://www.shadertoy.com/view/XsXXDn

    char vShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "attribute vec4 a_Position;         \n"     // Per-vertex position information we will pass in.

            "uniform vec3  u_resolution;       \n"
            "uniform float u_time;             \n"

            "varying vec3       v_resolution;  \n"     // viewport resolution (in pixels)
            "varying float      v_time;        \n"

            "void main()                       \n"     // The entry point for our vertex shader.
            "{                                 \n"
            "   gl_Position = a_Position;      \n"
            "   v_resolution = u_resolution;   \n"
            "   v_time = u_time;               \n"
            "}                                 \n";
    char fShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "varying vec3      v_resolution; \n"     // viewport resolution (in pixels)
            "varying float     v_time;       \n"     // shader playback time (in seconds)
            "#define t v_time\n"
            "#define r v_resolution.xy\n"
            "\n"
            "void main(){\n"
            "   vec3 c;\n"
            "   float l,z=t;\n"
            "   for(int i=0;i<3;i++) {\n"
            "       vec2 uv,p=gl_FragCoord.xy/r;\n"
            "       uv=p;\n"
            "       p-=.5;\n"
            "       p.x*=r.x/r.y;\n"
            "       z+=.07;\n"
            "       l=length(p);\n"
            "       uv+=p/l*(sin(z)+1.)*abs(sin(l*9.-z*2.));\n"
            "       c[i]=30.0/length(abs(mod(uv,1.)-.5));\n"
            "   }\n"
            "   gl_FragColor=vec4(c/l,t);\n"
            "}";



    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader   = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    if (shadersCommonParams.programObject != 0)
        glDeleteProgram(shadersCommonParams.programObject);
    shadersCommonParams.programObject  = glCreateProgram();

    if(shadersCommonParams.programObject == 0) {
        eglPrintError("initShader2() shadersCommonParams.programObject == 0", eglGetError());
        return 0;
    }

    glAttachShader(shadersCommonParams.programObject, vertexShader);
    glAttachShader(shadersCommonParams.programObject, fragmentShader);

    // Bind to attributes
    glBindAttribLocation(shadersCommonParams.programObject, 0, "u_iChannel0");
    glBindAttribLocation(shadersCommonParams.programObject, 1, "u_resolution");
    glBindAttribLocation(shadersCommonParams.programObject, 2, "u_time");

    glGenTextures(0, &indexChannel0_texture);
    glBindTexture(GL_TEXTURE_2D, indexChannel0_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 31, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
// turn off filtering and wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Link the program
    glLinkProgram(shadersCommonParams.programObject);

    // shaders are no loger needed, even if they are deleted when calling glDeleteProgram()
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Check the link status
    glGetProgramiv(shadersCommonParams.programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;

        glGetProgramiv(shadersCommonParams.programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(shadersCommonParams.programObject, infoLen, NULL, infoLog);
            LOGE("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(shadersCommonParams.programObject);
        return false;
    }

    // get indexes to pass to glEnableVertexAttribArray()
    indexChannel0   = glGetUniformLocation(shadersCommonParams.programObject, "u_iChannel0");
    glUniform1i(indexChannel0, 0);
    indexResolution = glGetUniformLocation(shadersCommonParams.programObject, "u_resolution");
    indexTime       = glGetUniformLocation(shadersCommonParams.programObject, "u_time");

    // Use the program object
    glUseProgram ( shadersCommonParams.programObject );

    return true;
}


bool initShader3(void *args) {
//    https://www.shadertoy.com/view/ttlGDf

    char vShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "attribute vec4 a_Position;         \n"     // Per-vertex position information we will pass in.

            "uniform vec3  u_resolution;       \n"
            "uniform float u_time;             \n"

            "varying vec3       v_resolution;  \n"     // viewport resolution (in pixels)
            "varying float      v_time;        \n"

            "void main()                       \n"     // The entry point for our vertex shader.
            "{                                 \n"
            "   gl_Position = a_Position;      \n"
            "   v_resolution = u_resolution;   \n"
            "   v_time = u_time;               \n"
            "}                                 \n";
    char fShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "varying vec3      v_resolution; \n"     // viewport resolution (in pixels)
            "varying float     v_time;       \n"     // shader playback time (in seconds)
            ""
            "void main()\n"
            "{\n"
            //"    //Normalized pixel coordinates (from 0 to 1)\n"
            "    vec2 uv = gl_FragCoord.xy/v_resolution.xy;\n"
            "    float t = (v_time)/3.0;\n"
            "    \n"
            "    vec2 pos = uv;\n"
            "    pos.y /= v_resolution.x/v_resolution.y;\n"
            "    pos = 4.0*(vec2(0.5,0.5) - pos);\n"
            "    \n"
            "    float strength = 0.4;\n"
            "    for(float i = 1.0; i < 7.0; i+=1.0){ \n"
            "    \tpos.x += strength * sin(2.0*t+i*1.5 * pos.y)+t*0.1;\n"
            "    \tpos.y += strength * cos(2.0*t+i*1.5 * pos.x);\n"
            "    }\n"
            "\n"
            //"    //Time varying pixel colour\n"
            "    vec3 col = 0.5 + 0.5*cos(v_time+pos.xyx+vec3(0,2,4));\n"
            "\n"
            //"    //Fragment colour\n"
            "    gl_FragColor = vec4(col,1.0);\n"
            "}";



    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader   = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    if (shadersCommonParams.programObject != 0)
        glDeleteProgram(shadersCommonParams.programObject);
    shadersCommonParams.programObject  = glCreateProgram();

    if(shadersCommonParams.programObject == 0) {
        eglPrintError("initShader3() shadersCommonParams.programObject == 0", eglGetError());
        return 0;
    }

    glAttachShader(shadersCommonParams.programObject, vertexShader);
    glAttachShader(shadersCommonParams.programObject, fragmentShader);

    // Bind to attributes
    glBindAttribLocation(shadersCommonParams.programObject, 0, "u_iChannel0");
    glBindAttribLocation(shadersCommonParams.programObject, 1, "u_resolution");
    glBindAttribLocation(shadersCommonParams.programObject, 2, "u_time");

    glGenTextures(0, &indexChannel0_texture);
    glBindTexture(GL_TEXTURE_2D, indexChannel0_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 31, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
// turn off filtering and wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Link the program
    glLinkProgram(shadersCommonParams.programObject);

    // shaders are no loger needed, even if they are deleted when calling glDeleteProgram()
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Check the link status
    glGetProgramiv(shadersCommonParams.programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;

        glGetProgramiv(shadersCommonParams.programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(shadersCommonParams.programObject, infoLen, NULL, infoLog);
            LOGE("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(shadersCommonParams.programObject);
        return false;
    }

    // get indexes to pass to glEnableVertexAttribArray()
    indexChannel0   = glGetUniformLocation(shadersCommonParams.programObject, "u_iChannel0");
    glUniform1i(indexChannel0, 0);
    indexResolution = glGetUniformLocation(shadersCommonParams.programObject, "u_resolution");
    indexTime       = glGetUniformLocation(shadersCommonParams.programObject, "u_time");

    // Use the program object
    glUseProgram ( shadersCommonParams.programObject );

    return true;
}




///////////////////////////////////
/// DRAWING FUNCTIONS
///////////////////////////////////
static const GLfloat vertices[] =
        {
                // front
                -0.5F, 0.5F, 0.5F,
                -0.5F, -0.5F, 0.5F,
                0.5F, 0.5F, 0.5F,
                0.5F, 0.5F, 0.5F,
                -0.5F, -0.5F, 0.5F,
                0.5F, -0.5F, 0.5F,

                // right
                0.5F, 0.5F, 0.5F,
                0.5F, -0.5F, 0.5F,
                0.5F, 0.5F, -0.5F,
                0.5F, 0.5F, -0.5F,
                0.5F, -0.5F, 0.5F,
                0.5F, -0.5F, -0.5F,

                // back
                0.5F, 0.5F, -0.5F,
                0.5F, -0.5F, -0.5F,
                -0.5F, 0.5F, -0.5F,
                -0.5F, 0.5F, -0.5F,
                0.5F, -0.5F, -0.5F,
                -0.5F, -0.5F, -0.5F,

                // left
                -0.5F, 0.5F, -0.5F,
                -0.5F, -0.5F, -0.5F,
                -0.5F, 0.5F, 0.5F,
                -0.5F, 0.5F, 0.5F,
                -0.5F, -0.5F, -0.5F,
                -0.5F, -0.5F, 0.5F,

                // top
                -0.5F, 0.5F, -0.5F,
                -0.5F, 0.5F, 0.5F,
                0.5F, 0.5F, -0.5F,
                0.5F, 0.5F, -0.5F,
                -0.5F, 0.5F, 0.5F,
                0.5F, 0.5F, 0.5F,

                // bottom
                -0.5F, -0.5F, 0.5F,
                -0.5F, -0.5F, -0.5F,
                0.5F, -0.5F, 0.5F,
                0.5F, -0.5F, 0.5F,
                -0.5F, -0.5F, -0.5F,
                0.5F, -0.5F, -0.5F
        };
static const GLfloat colors[] =
        {
                // front
                1.0F, 0.5F, 0.0F,1.0F,
                1.0F, 0.5F, 0.0F,1.0F,
                1.0F, 0.5F, 0.0F,1.0F,
                1.0F, 0.5F, 0.0F,1.0F,
                1.0F, 0.5F, 0.0F,1.0F,
                1.0F, 0.5F, 0.0F,1.0F,

                // right
                0.3F, 1.0F, 0.5F ,1.0F,
                0.3F, 1.0F, 0.5F ,1.0F,
                0.3F, 1.0F, 0.5F ,1.0F,
                0.3F, 1.0F, 0.5F ,1.0F,
                0.3F, 1.0F, 0.5F ,1.0F,
                0.3F, 1.0F, 0.5F ,1.0F,

                // back
                0.5F, 0.0F, 1.0F ,1.0F,
                0.5F, 0.0F, 1.0F ,1.0F,
                0.5F, 0.0F, 1.0F ,1.0F,
                0.5F, 0.0F, 1.0F ,1.0F,
                0.5F, 0.0F, 1.0F ,1.0F,
                0.5F, 0.0F, 1.0F ,1.0F,

                // left
                0.8F, 0.5F, 0.2F,1.0F,
                0.8F, 0.5F, 0.2F,1.0F,
                0.8F, 0.5F, 0.2F,1.0F,
                0.8F, 0.5F, 0.2F,1.0F,
                0.8F, 0.5F, 0.2F,1.0F,
                0.8F, 0.5F, 0.2F,1.0F,

                // top
                0.3F, 0.8F, 0.5F ,1.0F,
                0.3F, 0.8F, 0.5F ,1.0F,
                0.3F, 0.8F, 0.5F ,1.0F,
                0.3F, 0.8F, 0.5F ,1.0F,
                0.3F, 0.8F, 0.5F ,1.0F,
                0.3F, 0.8F, 0.5F ,1.0F,

                // bottom
                0.3F, 0.2F, 0.8F ,1.0F,
                0.3F, 0.2F, 0.8F ,1.0F,
                0.3F, 0.2F, 0.8F ,1.0F,
                0.3F, 0.2F, 0.8F ,1.0F,
                0.3F, 0.2F, 0.8F ,1.0F,
                0.3F, 0.2F, 0.8F ,1.0F,
        };


void defaultDrawFrame(void *args)
{
    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;

    //    RendererDataTime *b = (RendererDataTime *) args;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(4, GL_FLOAT, 0, colors);

    glRotatef(.1F, sin(time), 0.2F, sin(time));

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

//    delete b;
}



void drawFrame1(void *args) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    struct timespec time;
//    clock_gettime(CLOCK_REALTIME, &time);
    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;


    float shaderVertices[] = {
            -1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F
    };


    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices);
    glEnableVertexAttribArray(indexPosition);

    glUniform1f(indexTime, time );
    glUniform3f(indexResolution, (GLfloat) shadersCommonParams.width, (GLfloat) shadersCommonParams.height, 0.0F);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}



void drawFrame2(void *args) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;


    float shaderVertices[] = {
            -1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F
    };

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices);
    glEnableVertexAttribArray(indexPosition);

    float t = time - ((int)(time / 30.0F)) * 30.0F; // pass value from 0 to 30 range
    glUniform1f(indexTime, t);
    glUniform3f(indexResolution, (GLfloat) shadersCommonParams.width, (GLfloat) shadersCommonParams.height, 0.0F);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}



void drawFrame3(void *args) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;


    float shaderVertices[] = {
            -1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F
    };

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices);
    glEnableVertexAttribArray(indexPosition);

    float t = time - ((int)(time / 30.0F)) * 30.0F; // pass value from 0 to 30 range
    glUniform1f(indexTime, t);

    glUniform3f(indexResolution, (GLfloat) shadersCommonParams.width, (GLfloat) shadersCommonParams.height, 0.0F);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}