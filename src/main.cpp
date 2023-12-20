#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"
#include "Model.h"

void configScene();
void renderScene();
void setLights (glm::mat4 P, glm::mat4 V);
void drawObject(Model &model, Material &material, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void funFramebufferSize(GLFWwindow* window, int width, int height);
void funKey            (GLFWwindow* window, int key  , int scancode, int action, int mods);
void funScroll         (GLFWwindow* window, double xoffset, double yoffset);
void funCursorPos      (GLFWwindow* window, double xpos, double ypos);

// Shaders
Shaders shaders;

// Modelos
Model cube;
Model plane;
Model square;

// Luces y materiales
#define   NLD 1
#define   NLP 1
#define   NLF 2
Light     lightG;
Light     lightD[NLD];
Light     lightP[NLP];
Light     lightF[NLF];
Material  mluz;
Material  ruby;
Material  pearl;
Material  brass;
Material  emerald;
Material  tin;
Material  copper;
Material  black;

// Viewport
int w = 500;
int h = 500;

// Animaciones
float rotX = 0.0;
float rotY = 0.0;
float rotZ = 0.0;
float desZ = 0.0;

// Movimiento de camara
float fovy   = 90.0;
float alphaX =  0.0;
float alphaY =  0.0;

// Posiciones cubos
// Capa de arriba
glm::vec3 UpC         = glm::vec3(0.0, 2.1, 0.0);
glm::vec3 UpEast      = glm::vec3(0.0, 0.0, 2.1);
glm::vec3 UpWest      = glm::vec3(0.0, 0.0, -2.1);
glm::vec3 UpNorth     = glm::vec3(2.1, 0.0, 0.0);
glm::vec3 UpSouth     = glm::vec3(-2.1, 0.0, 0.0);
glm::vec3 UpNortheast = glm::vec3(2.1, 0.0, 2.1);
glm::vec3 UpNorthwest = glm::vec3(2.1, 0.0, -2.1);
glm::vec3 UpSoutheast = glm::vec3(-2.1, 0.0, 2.1);
glm::vec3 UpSouthwest = glm::vec3(-2.1, 0.0, -2.1);
glm::vec3 WestC       = glm::vec3(2.1, 0.0, 0.0);

// Posiciones cuadrados

// Rotaciones cubos
float rotUp[3]     = {0.0, 0.0, 0.0};
float rotMiddle[3] = {0.0, 0.0, 0.0};
float rotDown[3]   = {0.0, 0.0, 0.0};
float rotNorth[3]  = {0.0, 0.0, 0.0};
float rotSouth[3]  = {0.0, 0.0, 0.0};
float rotEst[3]    = {0.0, 0.0, 0.0};
float rotWest[3]   = {0.0, 0.0, 0.0};

// Rotaciones cuadrados
float rotV[3] = {0.0, 0.0, 90.0};
float rotL[3] = {0.0, 0.0, 90.0};
float rotR[3] = {90.0, 0.0, 0.0};
float rotN[3] = {90.0, 0.0, 0.0};


int main() {

    // Inicializamos GLFW
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Creamos la ventana
    GLFWwindow* window;
    window = glfwCreateWindow(w, h, "PracticaIG", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Inicializamos GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    const GLubyte *oglVersion = glGetString(GL_VERSION);
    std::cout <<"This system supports OpenGL Version: " << oglVersion << std::endl;

    // Configuramos los CallBacks
    glfwSetFramebufferSizeCallback(window, funFramebufferSize);
    glfwSetKeyCallback      (window, funKey);
    glfwSetScrollCallback   (window, funScroll);
    glfwSetCursorPosCallback(window, funCursorPos);

    // Entramos en el bucle de renderizado
    configScene();
    while(!glfwWindowShouldClose(window)) {
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void configScene() {

    // Test de profundidad
    glEnable(GL_DEPTH_TEST);

    // Shaders
    shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader.glsl");
    //shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader_cartoon.glsl");
    //shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader_procedural.glsl");
    //shaders.initShaders("resources/shaders/vshader.glsl","resources/shaders/fshader_discard.glsl");

    // Modelos
    cube.initModel("resources/models/cube.obj");
    plane.initModel("resources/models/plane.obj");

    // Luz ambiental global
    lightG.ambient = glm::vec3(2.0, 2.0, 2.0);

    // Luces direccionales
    lightD[0].direction = glm::vec3(-1.0, 0.0, 0.0);
    lightD[0].ambient   = glm::vec3( 0.1, 0.1, 0.1);
    lightD[0].diffuse   = glm::vec3( 0.7, 0.7, 0.7);
    lightD[0].specular  = glm::vec3( 0.7, 0.7, 0.7);

    // Luces posicionales
    lightP[0].position    = glm::vec3(6.0, 3.0, 3.0);
    lightP[0].ambient     = glm::vec3(0.2, 0.2, 0.2);
    lightP[0].diffuse     = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].specular    = glm::vec3(0.9, 0.9, 0.9);
    lightP[0].c0          = 1.00;
    lightP[0].c1          = 0.22;
    lightP[0].c2          = 0.20;

    // Luces focales
    lightF[0].position    = glm::vec3(-6.0,  2.0,  5.0);
    lightF[0].direction   = glm::vec3( 2.0, -2.0, -5.0);
    lightF[0].ambient     = glm::vec3( 0.2,  0.2,  0.2);
    lightF[0].diffuse     = glm::vec3( 0.9,  0.9,  0.9);
    lightF[0].specular    = glm::vec3( 0.9,  0.9,  0.9);
    lightF[0].innerCutOff = 10.0;
    lightF[0].outerCutOff = lightF[0].innerCutOff + 5.0;
    lightF[0].c0          = 1.000;
    lightF[0].c1          = 0.090;
    lightF[0].c2          = 0.032;
    lightF[1].position    = glm::vec3( 6.0,  2.0,  5.0);
    lightF[1].direction   = glm::vec3(-2.0, -2.0, -5.0);
    lightF[1].ambient     = glm::vec3( 0.2,  0.2,  0.2);
    lightF[1].diffuse     = glm::vec3( 0.9,  0.9,  0.9);
    lightF[1].specular    = glm::vec3( 0.9,  0.9,  0.9);
    lightF[1].innerCutOff = 5.0;
    lightF[1].outerCutOff = lightF[1].innerCutOff + 1.0;
    lightF[1].c0          = 1.000;
    lightF[1].c1          = 0.090;
    lightF[1].c2          = 0.032;

    // Materiales
    mluz.ambient      = glm::vec4(0.0, 0.0, 0.0, 1.0);
    mluz.diffuse      = glm::vec4(0.0, 0.0, 0.0, 1.0);
    mluz.specular     = glm::vec4(0.0, 0.0, 0.0, 1.0);
    mluz.emissive     = glm::vec4(1.0, 1.0, 1.0, 1.0);
    mluz.shininess    = 1.0;
    // Ruby
    ruby.ambient      = glm::vec4(0.174500, 0.011750, 0.011750, 0.55);
    ruby.diffuse      = glm::vec4(0.614240, 0.041360, 0.041360, 0.55);
    ruby.specular     = glm::vec4(0.727811, 0.626959, 0.626959, 0.55);
    ruby.emissive     = glm::vec4(0.000000, 0.000000, 0.000000, 1.00);
    ruby.shininess    = 76.8;
    // Pearl
    pearl.ambient     = glm::vec4(0.25, 0.20725, 0.20725, 0.922);
    pearl.diffuse     = glm::vec4(1.0, 0.829, 0.829, 0.922);
    pearl.specular    = glm::vec4(0.296648, 0.296648, 0.296648, 0.922);
    pearl.shininess   = 11.264;
    // Brass
    brass.ambient     = glm::vec4(0.329412f, 0.223529f, 0.027451f,1.0f);
    brass.diffuse     = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
    brass.specular    = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
    brass.shininess   = 27.8974f;
    // Emerald
    emerald.ambient   = glm::vec4(0.0215f, 0.1745f, 0.0215f, 0.55f);
    emerald.diffuse   = glm::vec4(0.07568f, 0.61424f, 0.07568f, 0.55f);
    emerald.specular  = glm::vec4(0.633f, 0.727811f, 0.633f, 0.55f);
    emerald.shininess = 76.8f;
    // Tin
    tin.ambient       = glm::vec4(0.105882f, 0.058824f, 0.113725f, 1.0f);
    tin.diffuse       = glm::vec4(0.427451f, 0.470588f, 0.541176f, 1.0f);
    tin.specular      = glm::vec4(0.333333f, 0.333333f, 0.521569f, 1.0f);
    tin.shininess     = 9.84615f;
    // Copper
    copper.ambient    = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
    copper.diffuse    = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
    copper.specular   = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
    copper.shininess  = 12.8f;
    // Black plastic
    black.ambient     = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    black.diffuse     = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
    black.specular    = glm::vec4(0.50f, 0.50f, 0.50f, 1.0f);
    black.shininess   = 32.0f;

}

void renderScene() {

    // Borramos el buffer de color
    glClearColor(255.0, 255.0, 255.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Indicamos los shaders a utilizar
    shaders.useShaders();

    // Matriz P
    float nplane =  0.1;
    float fplane = 25.0;
    float aspect = (float)w/(float)h;
    glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

    // Matriz V
    float x = 10.0f*glm::cos(glm::radians(alphaY))*glm::sin(glm::radians(alphaX));
    float y = 10.0f*glm::sin(glm::radians(alphaY));
    float z = 10.0f*glm::cos(glm::radians(alphaY))*glm::cos(glm::radians(alphaX));
    glm::vec3 eye   (  x,   y,   z);
    glm::vec3 center(0.0, 0.0,  0.0);
    glm::vec3 up    (0.0, 2.0,  0.0);
    glm::mat4 V = glm::lookAt(eye, center, up);
    shaders.setVec3("ueye",eye);

    // Fijamos las luces
    setLights(P,V);

    glm::mat4 S  =  glm::scale    (glm::mat4(1.0f), glm::vec3(0.9f));
    glm::mat4 Ry;
    glm::mat4 Rx;
    glm::mat4 Rz;
    glm::mat4 T;
    glm::mat4 MUp;
    glm::mat4 MWest;
    /*// Centro
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate (I, glm::vec3(2.1, 2.1, 2.1));
        drawObject(cube, black, P, V, T * Rx * Ry);*/
    // Centro derecha
    Ry    = glm::rotate    (I, glm::radians(rotWest[1]), glm::vec3(0,1,0));
    Rx    = glm::rotate    (I, glm::radians(rotWest[0]), glm::vec3(1,0,0));
    Rz    = glm::rotate    (I, glm::radians(rotWest[2]), glm::vec3(0,0,1));
    T     = glm::translate (I, glm::vec3(0.0, 2.1, 2.1));
    MWest = T * Rx * Ry * Rz;
    drawObject             (cube, black, P, V, MWest);
    /*// Color verde
    T = glm::translate     (I, glm::vec3(-1.1, 2.1, 2.1));
    drawObject             (plane, emerald, P, V, MWest * T * S);*/
    /*// Centro izquierda
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 2.1, 2.1));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color azul
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(5.21, 2.1, 2.1));
        drawObject(plane, tin, P, V, T * Rx * Ry * Rz * S);*/
    // Centro arriba
    Ry = glm::rotate    (I, glm::radians(rotUp[1]), glm::vec3(0,1,0));
    Rx = glm::rotate    (I, glm::radians(rotUp[0]), glm::vec3(1,0,0));
    Rz = glm::rotate    (I, glm::radians(rotUp[2]), glm::vec3(0,0,1));
    T  = glm::translate(I, UpC);
    MUp  = T * Rx * Ry * Rz;
    drawObject(cube, black, P, V, MUp);
    /*// Color amarillo
    T = glm::translate(I, UpC);
    drawObject(plane, brass, P, V, MUp * T * S);*/
    /*// Centro abajo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 0.0, 2.1));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color blanco
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, -1.01, 2.1));
        drawObject(plane, pearl, P, V, T * Rx * Ry * Rz * S);*/
    // Centro abajo derecha
    T = glm::translate(I, glm::vec3(0.0, 0.0, 2.1));
    drawObject(cube, black, P, V, MWest * T);
    /*// Color blanco
    T = glm::translate(I, glm::vec3(0.0, -1.01, 2.1));
    drawObject(plane, pearl, P, V, MWest * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.1, 0.0, 2.1));
    drawObject(plane, emerald, P, V, MWest * T * S);*/
    /*// Centro abajo izquierda
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 0.0, 2.1));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color blanco
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, -1.01, 2.1));
        drawObject(plane, pearl, P, V, T * Rx * Ry * Rz * S);
        // Color azul
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(5.21, 0.0, 2.1));
        drawObject(plane, tin, P, V, T * Rx * Ry * Rz * S);*/
    // Centro arriba derecha
    T = glm::translate(I, UpEast);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color amarillo
    T = glm::translate(I, UpEast);
    drawObject(plane, brass, P, V, MUp * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 4.2, 2.1));
    drawObject(plane, emerald, P, V, MUp * T * S);*/
    // Centro arriba izquierda
    T = glm::translate(I, UpWest);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color amarillo
    T = glm::translate(I, UpEast);
    drawObject(plane, brass, P, V, MUp * T * S);
    // Color azul
    T = glm::translate(I, glm::vec3(5.21, 4.2, 2.1));
    drawObject(plane, tin, P, V, MUp * T * S);*/
    /*// Derecha centro
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 2.1, 0.0));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color rojo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 2.1, -1.01));
        drawObject(plane, ruby, P, V, T * Rx * Ry * S);
    // Derecha centro derecha
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 2.1, 0.0));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color rojo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 2.1, -1.01));
        drawObject(plane, ruby, P, V, T * Rx * Ry * S);
        // Color azul
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(5.21, 2.1, 0.0));
        drawObject(plane, tin, P, V, T * Rx * Ry * Rz * S);*/
    // Derecha centro izquierda
    T = glm::translate(I, glm::vec3(0.0, 2.1, 0.0));
    drawObject(cube, black, P, V, MWest * T * S);
    /*// Color rojo
    T = glm::translate(I, glm::vec3(0.0, 2.1, -1.01));
    drawObject(plane, ruby, P, V, MWest * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 2.1, 0.0));
    drawObject(plane, emerald, P, V, MWest * T * S);*/
    // Derecha centro arriba
    T = glm::translate(I, UpNorth);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color rojo
    T = glm::translate(I, glm::vec3(2.1, 4.2, -1.01));
    drawObject(plane, ruby, P, V, MUp * T * S);
    // Color amarillo
    T = glm::translate(I, glm::vec3(2.1, 5.21, 0.0));
    drawObject(plane, brass, P, V, MUp * T * S);*/
    /*// Derecha centro abajo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 0.0, 0.0));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color rojo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 0.0, -1.01));
        drawObject(plane, ruby, P, V, T * Rx * Ry * S);
        // Color blanco
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, -1.01, 0.0));
        drawObject(plane, pearl, P, V, T * Rx * Ry * Rz * S);*/
    // Derecha arriba izquierda
    T = glm::translate(I, UpNortheast);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color rojo
    T = glm::translate(I, glm::vec3(4.2, 4.2, -1.01));
    drawObject(plane, ruby, P, V, MUp * T * S);
    // Color amarillo
    T = glm::translate(I, glm::vec3(4.2, 5.21, -0.01));
    drawObject(plane, brass, P, V, MUp * T * S);
    // Color azul
    T = glm::translate(I, glm::vec3(5.21, 4.2, 0.0));
    drawObject(plane, tin, P, V, MUp * T * S);*/
    // Derecha arriba derecha
    T = glm::translate(I, UpSouth);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color rojo
    T = glm::translate(I, glm::vec3(0.0, 4.2, -1.01));
    drawObject(plane, ruby, P, V, MUp * T * S);
    // Color amarillo
    T = glm::translate(I, glm::vec3(0.0, 5.21, -0.01));
    drawObject(plane, brass, P, V, MUp * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 4.2, 0.0));
    drawObject(plane, emerald, P, V, MUp * T * S);*/
    /*// Derecha abajo izquierda
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 0.0, 0.0));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color rojo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 0.0, -1.01));
        drawObject(plane, ruby, P, V, T * Rx * Ry * S);
        // Color blanco
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, -1.01, 0.0));
        drawObject(plane, pearl, P, V, T * Rx * Ry * Rz * S);
        // Color azul
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(5.21, 0.0, 0.0));
        drawObject(plane, tin, P, V, T * Rx * Ry * Rz * S);*/
    // Derecha abajo derecha
    T = glm::translate(I, glm::vec3(0.0, 0.0, 0.0));
    drawObject(cube, black, P, V, MWest * T);
    /*// Color rojo
    T = glm::translate(I, glm::vec3(0.0, 0.0, -1.01));
    drawObject(plane, ruby, P, V, MWest * T * S);
    // Color blanco
    T = glm::translate(I, glm::vec3(0.0, -1.01, 0.0));
    drawObject(plane, pearl, P, V, MWest * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 0.0, 0.0));
    drawObject(plane, emerald, P, V, MWest * T * S);*/
    /*// Izquierda centro
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 2.1, 4.2));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color naranja
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 2.1, 5.21));
        drawObject(plane, copper, P, V, T * Rx * Ry * S);
    // Izquierda centro derecha
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 2.1, 4.2));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color naranja
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 2.1, 5.21));
        drawObject(plane, copper, P, V, T * Rx * Ry * S);
        // Color azul
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(5.21, 2.1, 4.2));
        drawObject(plane, tin, P, V, T * Rx * Ry * Rz * S);*/
    // Izquierda centro izquierda
    T = glm::translate(I, glm::vec3(0.0, 2.1, 4.2));
    drawObject(cube, black, P, V, MWest * T);
    /*// Color naranja
    T = glm::translate(I, glm::vec3(0.0, 2.1, 5.21));
    drawObject(plane, copper, P, V, MWest * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 2.1, 4.2));
    drawObject(plane, emerald, P, V, MWest * T * S);*/
    // Izquierda centro arriba
    T = glm::translate(I, UpNorthwest);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color naranja
    T = glm::translate(I, glm::vec3(2.1, 4.2, 5.21));
    drawObject(plane, copper, P, V, MUp * T * S);
    // Color amarillo
    T = glm::translate(I, glm::vec3(2.1, 5.21, 4.2));
    drawObject(plane, brass, P, V, MUp * T * S);*/
    /*// Izquierda centro abajo
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 0.0, 4.2));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color naranja
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, 0.0, 5.21));
        drawObject(plane, copper, P, V, T * Rx * Ry * S);
        // Color blanco
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(2.1, -1.01, 4.2));
        drawObject(plane, pearl, P, V, T * Rx * Ry * Rz * S);*/
    // Izquierda arriba izquierda
    T = glm::translate(I, UpSouthwest);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color naranja
    T = glm::translate(I, glm::vec3(4.2, 4.2, 5.21));
    drawObject(plane, copper, P, V, MUp * T * S);
    // Color amarillo
    T = glm::translate(I, glm::vec3(4.2, 5.21, 4.2));
    drawObject(plane, brass, P, V, MUp * T * S);
    // Color azul
    T = glm::translate(I, glm::vec3(5.21, 4.2, 4.2));
    drawObject(plane, tin, P, V, MUp * T * S);*/
    // Izquierda arriba derecha
    T = glm::translate(I, UpSoutheast);
    drawObject(cube, black, P, V, MUp * T);
    /*// Color naranja
    T = glm::translate(I, glm::vec3(0.0, 4.2, 5.21));
    drawObject(plane, copper, P, V, MUp * T * S);
    // Color amarillo
    T = glm::translate(I, glm::vec3(0.0, 5.21, 4.2));
    drawObject(plane, brass, P, V, MUp * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 4.2, 4.2));
    drawObject(plane, emerald, P, V, MUp * T * S);*/
    /*// Izquierda abajo izquierda
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 0.0, 4.2));
        drawObject(cube, black, P, V, T * Rx * Ry);
        // Color naranja
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, 0.0, 5.21));
        drawObject(plane, copper, P, V, T * Rx * Ry * S);
        // Color blanco
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(4.2, -1.01, 4.2));
        drawObject(plane, pearl, P, V, T * Rx * Ry * Rz * S);
        // Color azul
        Ry = glm::rotate    (I, glm::radians(rotY), glm::vec3(0,1,0));
        Rx = glm::rotate    (I, glm::radians(rotX), glm::vec3(1,0,0));
        Rz = glm::rotate    (I, glm::radians(rotZ), glm::vec3(0,0,1));
        T = glm::translate(I, glm::vec3(5.21, 0.0, 4.2));
        drawObject(plane, tin, P, V, T * Rx * Ry * Rz * S);*/
    // Izquierda abajo derecha
    T = glm::translate(I, glm::vec3(0.0, 0.0, 4.2));
    drawObject(cube, black, P, V, MWest * T);
    /*// Color naranja
    T = glm::translate(I, glm::vec3(0.0, 0.0, 5.21));
    drawObject(plane, copper, P, V, MWest * T * S);
    // Color blanco
    T = glm::translate(I, glm::vec3(0.0, -1.01, 4.2));
    drawObject(plane, pearl, P, V, MWest * T * S);
    // Color verde
    T = glm::translate(I, glm::vec3(-1.01, 0.0, 4.2));
    drawObject(plane, emerald, P, V, MWest * T * S);*/
    /*// Plano
        Ry = glm::rotate   (I, glm::radians(0.0f), glm::vec3(0,1,0));
        Rx = glm::rotate   (I, glm::radians(0.0f), glm::vec3(1,0,0));
        T = glm::translate(I, glm::vec3(0.0, -5.0, 0.0));
        S = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));
        drawObject(plane, pearl, P, V, T * Rx * Ry * S);*/

}

void setLights(glm::mat4 P, glm::mat4 V) {

    shaders.setLight("ulightG",lightG);
    for(int i=0; i<NLD; i++) shaders.setLight("ulightD["+toString(i)+"]",lightD[i]);
    for(int i=0; i<NLP; i++) shaders.setLight("ulightP["+toString(i)+"]",lightP[i]);
    for(int i=0; i<NLF; i++) shaders.setLight("ulightF["+toString(i)+"]",lightF[i]);

    for(int i=0; i<NLP; i++) {
        glm::mat4 M = glm::translate(I,lightP[i].position) * glm::scale(I,glm::vec3(0.1));
        drawObject(cube, mluz, P, V, M);
        drawObject(plane, mluz, P, V, M);
        drawObject(square, mluz, P, V, M);
    }

    for(int i=0; i<NLF; i++) {
        glm::mat4 M = glm::translate(I,lightF[i].position) * glm::scale(I,glm::vec3(0.025));
        drawObject(cube, mluz, P, V, M);
        drawObject(plane, mluz, P, V, M);
        drawObject(square, mluz, P, V, M);
    }

}

void drawObject(Model &model, Material &material, glm::mat4 P, glm::mat4 V, glm::mat4 M) {

    shaders.setMat4("uN"  ,glm::transpose(glm::inverse(M)));
    shaders.setMat4("uM"  ,M);
    shaders.setMat4("uPVM",P*V*M);
    shaders.setMaterial("umaterial",material);
    model.renderModel(GL_FILL);

}

void funFramebufferSize(GLFWwindow* window, int width, int height) {

    // Configuracion del Viewport
    glViewport(0, 0, width, height);

    // Actualizacion de w y h
    w = width;
    h = height;

}

void funKey(GLFWwindow* window, int key  , int scancode, int action, int mods) {

    switch(key) {
        case GLFW_KEY_UP:    rotX -= 5.0f;   break;
        case GLFW_KEY_DOWN:  rotX += 5.0f;   break;
        case GLFW_KEY_LEFT:  rotY -= 5.0f;   break;
        case GLFW_KEY_RIGHT: rotY += 5.0f;   break;
        case GLFW_KEY_Q:     rotUp[1] += 5.0f;  break;
        case GLFW_KEY_E:     rotUp[1] -= 5.0f;  break;
        case GLFW_KEY_Z:
            if(mods==GLFW_MOD_SHIFT) desZ -= desZ > -24.0f ? 0.1f : 0.0f;
            else                     desZ += desZ <   5.0f ? 0.1f : 0.0f;
            break;
        default:
            rotX = 0.0f;
            rotY = 0.0f;
            break;
    }

}

void funScroll(GLFWwindow* window, double xoffset, double yoffset) {

    if(yoffset>0) fovy -= fovy>10.0f ? 5.0f : 0.0f;
    if(yoffset<0) fovy += fovy<90.0f ? 5.0f : 0.0f;

}

void funCursorPos(GLFWwindow* window, double xpos, double ypos) {

    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_RELEASE) return;

    float limY = 89.0;
    alphaX = 90.0*(2.0*xpos/(float)w - 1.0);
    alphaY = 90.0*(1.0 - 2.0*ypos/(float)h);
    if(alphaY<-limY) alphaY = -limY;
    if(alphaY> limY) alphaY =  limY;

}