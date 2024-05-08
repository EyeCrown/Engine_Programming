#version 400

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0
#define CUBE_BRANCHES 38.0

uniform sampler2D myTexture;
uniform float elapsed;
uniform vec3 sunPos;
uniform vec3 sunColor;
uniform vec3 camPos;

//Variables en entree
in vec4 wPos;
in vec3 normal;
in vec4 color;
in vec2 uv;
in float type;

out vec4 color_out;

//Globales
const float ambientLevel = 0.4;

float noise(vec4 position, float variator, float offset)
{
    return 0.5f * sin(position.x / 3 + variator) 
    + 0.05f * sin((position.y+position.x) + variator) 
    + offset;
}

void main()
{
    vec3 toLight = normalize(sunPos - wPos.xyz);
    vec3 newNorm = normal;
    vec2 realUv = vec2((uv.x + type) / 32, uv.y);

    float specular = 0;
    
	if(type == CUBE_EAU)
    {
        vec4 A = wPos;
        vec4 B = wPos + vec4(0.2f,0,0,0);
        vec4 C = wPos + vec4(0,0.2f,0,0);

        A.z += noise(A, elapsed, -0.5);
        B.z += noise(B, elapsed, -0.5);
        C.z += noise(C, elapsed, -0.5);

        newNorm = normalize(cross(vec3(B-A),vec3(C-A)));

        vec3 toCam = normalize(camPos - A.xyz);
        vec3 halfVec = normalize(toLight + toCam);
        float angle = dot(newNorm,halfVec);
        specular  = pow(abs(angle),300)*50;
    }

    float diffuse = max(0, dot(newNorm, toLight));
    
    
    //color_out =  texture(myTexture, realUv) * vec4(1.0);
    color_out = texture(myTexture, realUv) * vec4(sqrt(color.rgb * diffuse + sunColor.xyz * specular), color.a);
}