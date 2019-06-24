#include "Shader1.h"
#include "../ndk/common.hpp"


Shader1::Shader1(int width, int height)
{
    this->width = width;
    this->height = height;
    this->programObject = 0;
}

Shader1::~Shader1()
{
    glDeleteProgram(programObject);
}

bool Shader1::initShader(void *args) {
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


    programObject = createProgram(vShaderStr, fShaderStr);
    // Bind to attributes
    glBindAttribLocation(programObject, 0, "u_resolution");
    glBindAttribLocation(programObject, 1, "u_time");

    // get indexes to pass to glEnableVertexAttribArray()
    indexResolution = glGetUniformLocation(programObject, "u_resolution");
    indexTime       = glGetUniformLocation(programObject, "u_time");

    // Use the program object
    glUseProgram ( programObject );

    return true;
}

void Shader1::drawFrame(void *args) {
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
    glEnableVertexAttribArray(0);

    glUniform1f(indexTime, time );
    glUniform3f(indexResolution, (GLfloat) width, (GLfloat) height, 0.0F);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

