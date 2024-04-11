#version 400

uniform float elapsed;
uniform mat4 mvp;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

uniform vec3 birdPosition;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_direction_in;

out vec4 wPos;
out vec4 direction;

void main()
{
	vec4 vecIn = vec4(vs_position_in,1);
	//vec4 vecIn = vec4(birdPosition,1);
	
	wPos = m * vecIn;
	direction = vec4(vs_direction_in, 1);
	
	//wPos.x += sin(elapsed);
	
	gl_Position = wPos;
}