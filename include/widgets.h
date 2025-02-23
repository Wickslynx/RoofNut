#ifndef WIDGETS_H
#define WIDGETS_H

#ifdef ROOFNUT_USE_OPENGL

void RoofNutTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
//Shaders and stuff.
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
"}\n\0"
#endif

#endif
