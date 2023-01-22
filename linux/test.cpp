#include "include/test.h"

#include "include/flutter_opengl/flutter_opengl_plugin.h"
// #include <flutter_linux/flutter_linux.h>
// #include <flutter_linux/fl_view.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <thread>

#include "include/fl_my_texture_gl.h"
#include "../src/Shader.h"


const char* nullSafe(const char* str)
{
	return str ? str : "null";
}

void eglTest(EGLDisplay egl_display)
{
  std::cout << "*********************************START" << std::endl;
  std::cout << "OPENGL GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "OPENGL GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OPENGL GL_VERSION: " << glGetString(GL_VERSION) << std::endl;

	std::cout << "EGL Vendor    : " << nullSafe(eglQueryString(egl_display, EGL_VENDOR)) << std::endl;
	std::cout << "EGL Version   : " << nullSafe(eglQueryString(egl_display, EGL_VERSION)) << std::endl;
	std::cout << "EGL ClientAPIs: " << nullSafe(eglQueryString(egl_display, EGL_CLIENT_APIS)) << std::endl;
	std::cout << "EGL Extensions: " << nullSafe(eglQueryString(egl_display, EGL_EXTENSIONS)) << std::endl;
  std::cout << "*********************************FINE" << std::endl;
}


static EGLDisplay egl_display;
static EGLSurface egl_surface;
static EGLContext egl_context;

static const GLfloat vertices[] = {
    0.0f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
};
static GLfloat shaderVertices[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f,
        1.0f, -1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f,
        1.0f, 1.0f,   0.0f,
        1.0f, -1.0f,  0.0f
};
std::vector<float> shaderVertices2 {
    -0.5f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f
};

const GLchar* vertex_shader000 = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 color;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(position, 1.0);\n"
    "ourColor = color;\n"
    "}\0";
const GLchar* fragment_shader000 = 
    "#version 330 core\n"
    "out vec4 color;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "color = ourColor;\n"
    "}\n\0";

static const GLchar* vertex_shader00 =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main(){\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";
static const GLchar* fragment_shader00 =
    "#version 330 core\n"
    "uniform float time;\n"
    "out vec4 color;\n"
    "void main() {\n"
    // "  color=vec4(gl_Position.x, gl_Position.y, time/10.);\n"
    "  color=vec4((1.0, 0.0, 1.0, 1.0);\n"
    "}\0";


static const GLchar* vertex_shader0 =
    "#version 330 core\n"
    "out vec2 tc;\n"
    "void main(){\n"
    "  tc=vec2( (gl_VertexID & 1)<<1, (gl_VertexID & 2));\n"
    "  gl_Position=vec4(tc*2.0-1.0,0,1);\n"
    "}\0";
static const GLchar* fragment_shader0 =
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "uniform int useTex;\n"
    "uniform float time;\n"
    "in vec2 tc;\n"
    "out vec4 color;\n"
    "void main() {\n"
    "  if (useTex>0) color=texture(tex,vec2(tc.x,tc.y+0.01*sin(gl_FragCoord.x*0.1)));\n"
    "  else color=vec4(tc,0.0f,1.0f / time);\n"
    "}\0";


static const GLchar* vertex_shader1 =
    "#version 150\n"
    "attribute vec3 position;\n"
    "void main() {\n"
    "}\n\0";
static const GLchar* fragment_shader1 =
    "#version 150\n"
    "void main() {\n"
    "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n\0";

static const GLchar* vertex_shader2 =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"    TexCoords = aTexCoords;\n"
"}\n\0";
static const GLchar* fragment_shader2 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(screenTexture, TexCoords);\n"
"}\n\0";

static const GLchar* vertex_shader3 =
"#version 150\n"
"void main() \n"
"{ \n"
"} \n\0";
static const GLchar* fragment_shader3 =
"#version 150\n"
"precision mediump float; \n"
"void main() \n"
"{ \n"
" gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
"} \n\0";


// Supported versions are: 1.10, 1.20, 1.30, 1.40, 1.50, 3.30, 4.00, 
//          4.10, 4.20, 4.30, 4.40, 4.50, 4.60, 1.00 ES, 3.00 ES, 3.10 ES, and 3.20 ES
static const GLchar* vertex_shader4 =
      "#version 330 core\n"
      "precision mediump float;\n"
      "precision mediump int;\n"
      "attribute vec4 a_Position;         \n" // Per-vertex position information we will pass in.

      "uniform vec3    u_resolution;     \n"
      "uniform float   u_time;           \n"
      "varying vec3    v_resolution;     \n" // viewport resolution (in pixels)
      "varying float   v_time;           \n"
      "void main()                       \n" // The entry point for our vertex shader.
      "{                                 \n"
      "   gl_Position = a_Position;      \n"
      "   v_resolution = u_resolution;   \n"
      "   v_time = u_time;               \n"
      "}                                 \n\0";

  static const GLchar* fragment_shader4 =
      "#version 330 core\n"
      "precision mediump float;\n"
      "precision mediump int;\n"
      "mat2 rot(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}\n"
      "const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;\n"
      "float mag2(vec2 p){return dot(p,p);}\n"
      "float linstep(in float mn, in float mx, in float x){ return clamp((x - mn)/(mx - mn), 0., 1.); }\n"
      "float prm1 = 0.;\n"
      "vec2 bsMo = vec2(0);\n"
      ""
      "varying vec3      v_resolution; \n" // viewport resolution (in pixels)
      "varying float     v_time;       \n" // shader playback time (in seconds)
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
      "}\0";



void drawFrame(FlutterOpenglPlugin *self) {

    // Load the vertex data
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices );
    glEnableVertexAttribArray ( 0 );

    // glBindTexture(GL_TEXTURE_2D, self->texture_name);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}


// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
// https://cgit.freedesktop.org/mesa/demos/tree/src/egl/opengles2/es2tri.c
// https://discourse.gnome.org/t/gtk-opengl-widget-drawing-result-broken-when-maximize-restore-window/4165
void main2(FlutterOpenglPlugin *self, int width, int height) {
    GtkWidget *widget = GTK_WIDGET(self->fl_view);
  //   GdkDisplay *display = gtk_widget_get_display(widget);
  //   int www = gtk_widget_get_allocated_width(widget);
  //   int hhh = gtk_widget_get_allocated_height(widget);
  //   Window xidApp = gdk_x11_window_get_xid(gtk_widget_get_window(widget));

  

  // GtkAllocation allocation;
  // gtk_widget_get_allocation(widget, &allocation);
  // GdkWindowAttr window_attributes;
  // window_attributes.window_type = GDK_WINDOW_CHILD;
  // window_attributes.x = 100; //allocation.x;
  // window_attributes.y = 100; //allocation.y;
  // window_attributes.width = 200; //allocation.width;
  // window_attributes.height = 400; //allocation.height;
  // window_attributes.wclass = GDK_INPUT_OUTPUT;
  // window_attributes.event_mask =
  //     gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK |
  //     GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK |
  //     GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK | GDK_SMOOTH_SCROLL_MASK |
  //     GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK;
  // gint window_attributes_mask = GDK_WA_X | GDK_WA_Y;

  // GdkWindow *viewWindow =
  //     gdk_window_new(gtk_widget_get_window(widget), &window_attributes,
  //                    window_attributes_mask);
  // gtk_widget_register_window(widget, viewWindow);
  // gtk_widget_set_window(widget, viewWindow);
  // gdk_window_show(viewWindow);

  EGLNativeWindowType _window = gdk_x11_window_get_xid(gtk_widget_get_window(widget));

  EGLint majorVersion, minorVersion;
  EGLConfig egl_config;
  EGLint n_config;
  EGLint attributes[] = {
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_BUFFER_SIZE,    16,
      EGL_DEPTH_SIZE,     16,
      EGL_STENCIL_SIZE,   0,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_NONE
  };

  egl_display =
      eglGetDisplay((EGLNativeDisplayType)gdk_x11_display_get_xdisplay(
          gtk_widget_get_display(widget)));
  egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  eglPrintError("eglGetDisplay()");

  eglInitialize(egl_display, &majorVersion, &minorVersion);
  eglPrintError("eglInitialize()");
  
  eglChooseConfig(egl_display, attributes, &egl_config, 1, &n_config);
  eglPrintError("eglChooseConfig()");
  
  eglBindAPI(EGL_OPENGL_ES_API);
  eglPrintError("eglBindAPI()");
  
  // egl_surface = eglCreateWindowSurface(
  //     egl_display, egl_config, gdk_x11_window_get_xid(viewWindow), NULL);  
  egl_surface = eglCreateWindowSurface(
      egl_display, egl_config, _window, NULL);
  eglPrintError("eglCreateWindowSurface()");
  
  egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, 0);
  // egl_context = self->context;
  // eglPrintError("eglCreateContext()");
  
  eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
  eglPrintError("eglMakeCurrent()");
  
  int w = 0;
  int h = 0;
  eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &w);
  eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &h);

  eglTest(egl_display);
  printf("********** QUERY: %d  %d\n", w,h);




//   std::string vShader = 
// "#version 150\n"
// "uniform mat4 modelviewProjection;"
// "attribute vec4 pos;\n"
// "attribute vec4 color;\n"
// "varying vec4 v_color;\n"
// "void main() {\n"
// "   gl_Position = modelviewProjection * pos;\n"
// "   v_color = color;\n"
// "};\n";

//   std::string fShader = 
// "#version 150\n"
// "precision mediump float\n;"
// "varying vec4 v_color;\n"
// "void main() {\n"
// "   gl_FragColor = v_color;\n"
// "};\n";


//   Shader *shader = new Shader(width, height);
//   shader->setShadersText(vShader, fShader);
  
//   shader->initShader();

//   drawFrame(self);
//   if (!eglSwapBuffers(egl_display, egl_surface)) {
//       eglPrintError("eglSwapBuffers()");
//   }
//   fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
//                                                         self->texture);

//   free(shader);

    GLuint shader_program, vbo;
    GLint pos;

    shader_program = common_get_shader_program(vertex_shader1, fragment_shader1);
    pos = glGetAttribLocation(shader_program, "position");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, w, h);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader_program);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(egl_display, egl_surface);
    eglPrintError("eglSwapBuffers()");
}





/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;


GLint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
    enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;

    /* Vertex shader */
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

// GLFW
// void main3(FlutterOpenglPlugin *self, int width, int height) {
//     GLuint shader_program, vbo;
//     GLint pos;
//     GLFWwindow* window;

//     glfwInit();
//     glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//     window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);

//     // https://www.glfw.org/docs/3.3/group__native.html
//     // glfwGetX11Window()

//     glfwMakeContextCurrent(window);

//     printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
//     printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );

    

//     shader_program = common_get_shader_program(vertex_shader2, fragment_shader2);
//     pos = glGetAttribLocation(shader_program, "position");

//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     glViewport(0, 0, WIDTH, HEIGHT);

//     glGenBuffers(1, &vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//     glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
//     glEnableVertexAttribArray(pos);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);

//     while (!glfwWindowShouldClose(window)) {
//         glfwPollEvents();
//         glClear(GL_COLOR_BUFFER_BIT);
//         glUseProgram(shader_program);
//         glDrawArrays(GL_TRIANGLES, 0, 3);
//         glfwSwapBuffers(window);
//     }
//     glDeleteBuffers(1, &vbo);
//     glfwTerminate();
// }







/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/
GLuint createRenderTarget(uint16_t const width, uint16_t const height)
{
    GLuint frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // Color buffer
    GLuint colorBuffer = 0;
    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);
    
    // Depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    GLuint test = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( test != GL_FRAMEBUFFER_COMPLETE )
    {
        std::cout << "OpenGL Failed to create multi-sampled render target" << std::endl;
        return 0;
    }
    return frameBuffer;
}

GLuint createRenderTargetTexture(FlutterOpenglPlugin *self, uint16_t const width, uint16_t const height)
{
    GLuint frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    
    // The texture we're going to render to
    // GLuint renderedTexture;
    // glGenTextures(1, &renderedTexture);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, self->texture_name/*renderedTexture*/);
    
    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    // The depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    
    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->texture_name/*renderedTexture*/, 0);
    
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    
    GLuint test = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( test != GL_FRAMEBUFFER_COMPLETE )
    {
        std::cout << "OpenGL Failed to create render target" << std::endl;
        return 0;
    }
    return frameBuffer;
}

static void cs(GLuint p, GLenum type, const char *src)
{
	GLuint sh=glCreateShader(type);
	glShaderSource(sh, 1, (const GLchar**)&src, NULL);
	glCompileShader(sh);
	glAttachShader(p, sh);
}
// https://github.com/derhass/miscstuff/blob/2b06b6b39ba77b82488a8c803784d60cedc8dc49/OpenGL/code/HelloFBO_minimal.c
void mainFBO3(FlutterOpenglPlugin *self, int width, int height) {

    gdk_gl_context_make_current(self->context);
    GLuint vs, p, vao, tex, fbo, lUseTex;
    p=glCreateProgram();
	cs(p,GL_VERTEX_SHADER,vertex_shader0);
	cs(p,GL_FRAGMENT_SHADER,fragment_shader0);
	glLinkProgram(p);
	glUseProgram(p);
	lUseTex=glGetUniformLocation(p,"useTex");

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	// glGenTextures(1,&self->texture_name);
	glBindTexture(GL_TEXTURE_2D, self->texture_name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, self->texture_name, 0);


    // while (!glfwWindowShouldClose(win)) {
	// 	int w,h;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glViewport(0,0,width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1i(lUseTex,0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
        
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// glfwGetFramebufferSize(win,&w,&h);
		glViewport(0,0,width, height);
		glUniform1i(lUseTex,1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	// 	glfwSwapBuffers(win);
	// 	glfwPollEvents();
	// }


    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                        self->texture);
    gdk_gl_context_clear_current();
}

void mainFBO4(FlutterOpenglPlugin *self, int width, int height) {

    Shader *shader = new Shader(width, height);
    shader->setShadersText(vertex_shader4, fragment_shader4);
    shader->initShader(self);

    std::thread gl_thread([](FlutterOpenglPlugin *self, Shader *shader, int width, int height) {

        GLfloat startTime = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;

        while(1) {
            gdk_gl_context_make_current(self->context);
// If you want to use a clockwise definition, you can simply call
// glFrontFace(GL_CW);

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
   
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shaderVertices), shaderVertices, GL_STATIC_DRAW);

    // Position attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    // glEnableVertexAttribArray(0);
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices );
    glEnableVertexAttribArray ( 0 );

    glBindVertexArray(0); // Unbind VAO


            GLuint fbo;
            
            glBindTexture(GL_TEXTURE_2D, self->texture_name);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glGenFramebuffers(1,&fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, self->texture_name, 0);

            GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC - startTime;
            // printf("%f\n", time);
            

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
                glViewport(0,0,width, height);
                glClearColor(.2f,.3f,.3f,1.f);
                glClear(GL_COLOR_BUFFER_BIT);
                    shader->use();
                    glUniform1f(glGetUniformLocation(shader->getProgramObject(), 
                        "u_time"), time);
                    glUniform3f(glGetUniformLocation(shader->getProgramObject(), 
                        "u_resolution"), (GLfloat)width, (GLfloat)height, 0.0f);
                
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);

            fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                                self->texture);
            gdk_gl_context_clear_current();
        }
    },
        self, shader, width, height
    );
    gl_thread.detach();

}

void mainFBO5(FlutterOpenglPlugin *self, int width, int height) {

    std::thread gl_thread([](FlutterOpenglPlugin *self, int width, int height) {

    gdk_gl_context_make_current(self->context);
        Shader *shader = new Shader(width, height);
        shader->setShadersText(vertex_shader4, fragment_shader4);
        shader->initShader(self);
    gdk_gl_context_clear_current();
        while (1) {
            shader->drawFrame(self);
        }
    },
        self, width, height
    );
    gl_thread.detach();
}

// https://metashapes.com/blog/opengl-frame-buffer-objects-blitting-multi-sampling/
void mainFBO2(FlutterOpenglPlugin *self, int width, int height) {

    
    gdk_gl_context_make_current(self->context);

    GLuint framebuffer = createRenderTarget(width, height);

    GLuint renderbuffer = createRenderTargetTexture(self, width, height);
    
    // GLuint shader_program = common_get_shader_program(vertex_shader2, fragment_shader2);
    Shader *shader = new Shader(width, height);
    shader->setShadersText(vertex_shader4, fragment_shader4);
    shader->initShader(self);

    

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    shader->use();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    // draw your scene here... 
    drawFrame(self);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind your FBO to set the default framebuffer
    glClearColor(0.0f, 0.7f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    shader->use(); // shader program for rendering the quad  
    // glUseProgram(shader_program);    eglPrintError("glUseProgram()");

    glBindTexture(GL_TEXTURE_2D, self->texture_name); // color attachment texture

    GLuint vbo;
    GLint pos;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, width, height);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shaderVertices), shaderVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can also use VAO or attribute pointers instead of only VBO...
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);




    

    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                        self->texture);
    gdk_gl_context_clear_current();
}




// https://learnopengl.com/Advanced-OpenGL/Framebuffers @Rendering to a texture
void mainFBO(FlutterOpenglPlugin *self, int width, int height) {

    gdk_gl_context_make_current(self->context);

    GLuint framebuffer;
    // Creating a framebuffer
    glGenFramebuffers(1, &framebuffer);                 eglPrintError("glGenFramebuffers()");
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);     eglPrintError("glBindFramebuffer()");

    if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating framebuffer!" << std::endl;
        return;
    }

    // texture is already created in "createSurface"
    // glGenTextures(1, &self->texture_name);
    glBindTexture(GL_TEXTURE_2D, self->texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0, 0, width, height);

    // attach texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->texture_name, 0);

    // Creating a renderbuffer object
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo );                    eglPrintError("glGenRenderbuffers()");
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);       eglPrintError("glBindRenderbuffer()");
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  eglPrintError("glRenderbufferStorage()");
    glBindRenderbuffer(GL_RENDERBUFFER, 0);         eglPrintError("glBindRenderbuffer()");

    // attach the renderbuffer object to the depth and stencil attachment of the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);   eglPrintError("glFramebufferRenderbuffer()");

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        eglPrintError("glCheckFramebufferStatus()");
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        // return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);       eglPrintError("glBindFramebuffer()");

    // load shader
    GLuint shader_program = common_get_shader_program(vertex_shader1, fragment_shader1);
    // Shader *shader = new Shader(width, height);
    // shader->setShadersText(vertex_shader3, fragment_shader3);
    // shader->initShader();


    // first pass
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);     eglPrintError("glBindFramebuffer()");
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);
    
    // DrawScene();
    drawFrame(self);
    // float shaderVertices[] = {
    //         -1.0f, -1.0f, 0.0f,
    //         -1.0f, 1.0f,  0.0f,
    //         1.0f, -1.0f,  0.0f,
    //         -1.0f, 1.0f,  0.0f,
    //         1.0f, 1.0f,   0.0f,
    //         1.0f, -1.0f,  0.0f
    // };
    // // Load the vertex data
    // glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices );
    // glEnableVertexAttribArray ( 0 );
    // glBindTexture(GL_TEXTURE_2D, self->texture_name);
    // eglPrintError("glBindTexture()");
    // glDrawArrays(GL_TRIANGLES, 0, 6);



    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   eglPrintError("glBindFramebuffer()");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    // glUseProgram(shader->programObject);    eglPrintError("glUseProgram()");
    glUseProgram(shader_program);    eglPrintError("glUseProgram()");

    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, self->texture_name);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                        self->texture);
    gdk_gl_context_clear_current();
    // free(shader);
    glDeleteFramebuffers(1, &framebuffer); 
}


void testRandomTexture(FlutterOpenglPlugin *self, int width, int height) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 255);
    std::vector<uint8_t> buffer;
    for (int i = 0; i < height * width * 4; ++i) {
        buffer.push_back(
            distr(gen)
            );
    }

    gdk_gl_context_make_current(self->context);

    glBindTexture(GL_TEXTURE_2D, self->texture_name); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 
            0, GL_BGR, GL_UNSIGNED_BYTE, buffer.data());
              
    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                        self->texture);
    gdk_gl_context_clear_current();
    std::cout << "************ FINE" << std::endl;
}
