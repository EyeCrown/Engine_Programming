#version 400

uniform mat4 mvp;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

layout (points) in;
layout (triangle_strip, max_vertices = 18) out;

in vec4[] wPos;
in vec4[] direction;


out vec4 baseColor;

float size = 1.0;

void CreateVertex(vec3 offset){
    vec4 actualOffset = vec4(offset * size, 0.0);
    vec4 worldPos = gl_in[0].gl_Position + actualOffset;
    gl_Position = p * v * worldPos;
    EmitVertex();
}

void CreateTriangle(vec3 a, vec3 b, vec3 c){
    baseColor = vec4(a, 1.0);
    CreateVertex(a);
    baseColor = vec4(b, 1.0);
    CreateVertex(b);
    baseColor = vec4(c, 1.0);
    CreateVertex(c);
}


void main(){
    // gl_in[0].gl_Position
    // I need vec direction, right, up
    vec3 one =   vec3( 0,-0.5,-0.5);
    vec3 two =   vec3( 0, 0.5,-0.5);
    vec3 three = vec3( 0,-0.5, 0.5);
    vec3 four =  vec3( 0, 0.5, 0.5);
    vec3 five =  vec3( 1, 0, 0);
    
    CreateTriangle(two, three, four);
    CreateTriangle(three, four, five);
    CreateTriangle(four, five, two);
    CreateTriangle(five, two, one);
    CreateTriangle(two, one, three);
    CreateTriangle(one, three, five);
    
    EndPrimitive();
    
}