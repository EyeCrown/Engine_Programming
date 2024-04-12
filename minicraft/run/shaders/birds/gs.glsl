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
         
    //vec3 five =  vec3(direction[0].xyz);
    
    vec3 up = normalize(vec3(0, 0, 1));
    vec3 right = normalize(cross(direction[0].xyz, up));
    up = normalize(cross( right, direction[0].xyz));
    float scale = 0.2;
    float length = 1;
    
    vec3 one   = normalize(-right - up) * scale;    // botLeft
    vec3 two   = normalize( right - up) * scale;    // botRight
    vec3 three = normalize(-right + up) * scale;    // topLeft
    vec3 four  = normalize( right + up) * scale;    // topRight
    vec3 five  = normalize(direction[0].xyz);    // direction
    
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);
    vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
    
    CreateTriangle(two, three, four, black, black, orange);         // 2,3,4
    
    CreateTriangle(three, four, five, black, orange, orange);       // 3,4,5
    CreateTriangle(four, five, two, orange, yellow, black);         // 4,5,2
    CreateTriangle(five, two, one, yellow, black, black);           // 5,2,1
    
    CreateTriangle(two, one, three, black, black, black);           // 2,1,3
    
    CreateTriangle(one, three, five, black, black, yellow);         // 1,3,5
    
    EndPrimitive();
    
}