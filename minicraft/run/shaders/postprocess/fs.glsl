#version 400

in vec2 uv;

uniform float elapsed;
uniform sampler2D TexColor;
uniform sampler2D TexDepth;
uniform float screen_width;
uniform float screen_height;
uniform vec2 near_far;
uniform float type;
uniform vec3 skyColor;

#define CUBE_EAU 4.0

out vec4 color_out;

float LinearizeDepth(float z)
{
	float n = near_far.x; // camera z near
  	float f = near_far.y; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main (void)
{
	float xstep = 2.0/screen_width;
    float ystep = 2.0/screen_height;
    float ratio = screen_width / screen_height;

    vec2 realUv = uv;
    
    //vec4 color = texture2D( TexColor , uv );
    float depth = texture2D( TexDepth , realUv ).r;
    float depthr = texture2D( TexDepth , realUv + vec2(xstep, 0)).r;
    float depthl = texture2D( TexDepth , realUv - vec2(xstep, 0)).r;
    float depthu = texture2D( TexDepth , realUv + vec2(0, ystep)).r;
    float depthd = texture2D( TexDepth , realUv - vec2(0, ystep)).r;
    
    //Permet de scaler la profondeur
    depth = LinearizeDepth(depth);
    depthr = LinearizeDepth(depthr);
    depthl = LinearizeDepth(depthl);
    depthu = LinearizeDepth(depthu);
    depthd = LinearizeDepth(depthd);
    
    float edge = (depth * 4) - (depthr+depthl+depthu+depthd);
    edge = clamp(20*edge, 0, 1);

    //Gamma correction
    //color.r = pow(color.r,1.0/2.2);
    //color.g = pow(color.g,1.0/2.2);
    //color.b = pow(color.b,1.0/2.2);
    
    if (type == CUBE_EAU)
    {
        //baseColor += vec4(0.0, 0.0, 1.0, 1.0);
        realUv.y = (uv.y + sin(elapsed + uv.x * 5) / 15);
    }
    
    vec4 color = texture2D( TexColor , realUv );

    if (type == CUBE_EAU)
    {
        color += vec4(0.0, 0.0, 1.0, 1.0);
    }
    //color += depth * 1;

    //if(depth < 0.9f)
        //color.rgb = mix(color.rgb, skyColor, clamp(pow(depth, 4.0f)*5, 0, 1));
    color.rgb += edge * vec3(1, 1, 1);

	color_out = vec4(color.rgb,1.0);
}