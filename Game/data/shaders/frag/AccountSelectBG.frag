#version 430 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
    vec2 TexUV;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform float uTime;
uniform float uActivatedPlayersValues[2];

#define RANGE 2

// Edited function from: https://www.shadertoy.com/view/XsjGDt
float circle(vec2 uv, vec2 pos, float rad) 
{
	float d = length(pos - uv) - rad;
	float t = clamp(d, 0.0, 1.0);
	return pow(1.0 - t, 50);
}

// Edited function from: https://www.shadertoy.com/view/XsjGDt
float circlenormal(vec2 uv, vec2 pos, float rad) 
{
	float d = length(pos - uv) - rad;
	float t = clamp(d, 0.0, 1.0);
	return pow(1.0 - t, 5);
}

// Edited function from: https://www.shadertoy.com/view/XsjGDt
float pulse(vec2 uv, vec2 pos, float rad1, float rad2) 
{
    return clamp(circle(uv, pos, rad1) - circle(uv, pos, rad2), 0.0, 1.0) * circlenormal(uv, pos, 0.5);
}

// Edited function from: https://www.shadertoy.com/view/7lXGzN
vec2 inflate(vec2 uv, float radius, float strength) 
{
    const vec2 center = vec2(0.0);
    float dist = distance(uv , center);
    vec2 dir = normalize(uv - center);
    float scale = 1.-strength + strength *smoothstep(0.,1.,dist/radius);
    float newDist = dist * scale;
    return center + newDist * dir;
}

// Edited function from: https://www.shadertoy.com/view/ltXBz7#
float grid(vec2 uv, float e) 
{
    const float w = 2.;
    vec2 g = smoothstep(0.0, w*e, abs(fract(uv * 8 + 0.5) - 0.5)); 
    return g.x * g.y;
}

vec3 interlace(vec2 co, vec3 col) 
{
    if (int(co.y) % 3 == 0) 
    {
        return col * ((clamp(sin(uTime * 4.), 0.5, 1.0) * 0.1) + 0.75);
    }
    return col;
}

void main() 
{
    vec2 UV = fs_in.TexUV;
    vec2 PlayerZone = vec2(fract(UV.x), UV.y);
    vec2 InflatedUV = inflate(UV, RANGE, 0.15);
    //vec2 InflatedUV = UV;

    // Idea: Cool diagonal effect
    //float gridValue = mod(6 * (UV.x + UV.y), 6) / 6;
    const vec3 BGColor = vec3(0.05, 0.2, 0.25);
    const vec3 gridNormalColor = vec3(0.0, 0.6, 0.5); 
    const vec3 gridPulseColor = vec3(0.1, 1.0, 0.99);
    float gridValue = 1.0 - grid(InflatedUV, 0.01);

    float rad1 = 1.25 * fract(0.25 + 0.5 * uTime);
    float rad2 = 1*fract(0.25 + 0.5 * uTime);
    float pulseValue = pulse(PlayerZone * vec2(1, 0.5), vec2(0.5, 0), rad1, rad2);
    
    int playerIdx = (UV.x < 0.0) ? 0 : 1;
    vec3 pulseColor = mix(gridNormalColor, gridPulseColor, pulseValue * uActivatedPlayersValues[playerIdx]);
    
    vec3 Result = mix(BGColor, pulseColor, gridValue);
    //vec3 Result = vec3(gridValue * pulse(vec2(fract(InflatedUV.x), InflatedUV.y) * vec2(1, 0.5), vec2(0.5, 0), rad1, rad2));

    vec3 InterlacedColor = interlace(InflatedUV * 225, Result);
    FragColor = vec4(InterlacedColor, 1);
}