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

void CreateTriangle(vec3 a, vec3 b, vec3 c, vec4 aColor, vec4 bColor, vec4 cColor){
    baseColor = aColor;
    CreateVertex(a);
    baseColor = bColor;
    CreateVertex(b);
    baseColor = cColor;
    CreateVertex(c);
}


void main(){
    // gl_in[0].gl_Position
    // I need vec direction, right, up
    vec3 one =   vec3( 0,-0.5,-0.5);    // botLeft
    vec3 two =   vec3( 0, 0.5,-0.5);    // botRight
    vec3 three = vec3( 0,-0.5, 0.5);    // topLeft
    vec3 four =  vec3( 0, 0.5, 0.5);    // topRight
    vec3 five =  vec3( 1, 0, 0);        // direction
    //vec3 five =  vec3(direction[0].xyz);
    
    vec3 up = normalize(vec3(0, 0, 1));
    vec3 right = normalize(cross(direction[0].xyz, up));
    
    one   = -right - up;
    two   =  right - up;
    three = -right + up;
    four  =  right + up;
    five  = normalize(direction[0].xyz); 
    
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);
    vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
    
    CreateTriangle(two, three, four, black, black, orange);
    
    CreateTriangle(three, four, five, black, orange, orange);
    CreateTriangle(four, five, two, orange, yellow, black);
    CreateTriangle(five, two, one, yellow, black, black);
    
    CreateTriangle(two, one, three, black, black, black);
    
    CreateTriangle(one, three, five, black, black, yellow);
    
    EndPrimitive();
    
}