#version 400

in vec4 baseColor;

out vec4 p_color;

void main()
{
    //vec3 color = vec3(1.0, 1.0, 1.0);
    
    vec3 color = baseColor.rgb;
    
	p_color = vec4(color,1);
}