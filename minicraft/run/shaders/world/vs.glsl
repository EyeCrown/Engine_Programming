#version 400

uniform float elapsed;
uniform mat4 mvp;
uniform mat4 nmat;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in float vs_type_in;

//Variables en sortie
out vec4 wPos;
out vec3 normal;
out vec4 color;
out vec2 uv;
out float type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0
#define CUBE_BRANCHES 38.0


float noise(vec4 position, float variator, float offset)
{
    return 0.5f * sin(position.x / 3 + variator) 
    + 0.05f * sin((position.y+position.x) + variator) 
    + offset;
}

void main()
{
	vec4 vecIn = vec4(vs_position_in,1.0);
		
	normal = (nmat * vec4(vs_normal_in,1.0)).xyz; 

    wPos = m * vecIn;
    
	uv = vs_uv_in;
    type = vs_type_in;
	//Couleur par défaut violet
	color = vec4(1.0,1.0,1.0,1.0);

	//Couleur fonction du type
	if(vs_type_in == CUBE_HERBE)
    {
		//color = vec4(0,1,0,1);
    }
	if(vs_type_in == CUBE_TERRE)
	{
		//color = vec4(0.2,0.1,0,1);
	}
	if(vs_type_in == CUBE_EAU)
	{
		color = vec4(0.0,0.0,1.0,1.0);	
		//color.a = 0.8;	
		wPos.z += noise(wPos, elapsed, -0.5);
		//type = 22;
    }
    if(vs_type_in == CUBE_BRANCHES)
    {
        wPos.x += noise(wPos, elapsed * 10, 0.0);
        wPos.y -= noise(wPos, elapsed * 10, 0.0);
    }
    
    gl_Position = p * v * wPos;
}