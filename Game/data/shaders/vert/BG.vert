#version 400 core

layout (location = 0) in vec3 vPos;


// ------------------- UNIFORM -------------------

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
} vs_out;

void main()
{
    vs_out.TexUV = vPos.xy;
    gl_Position = vec4(vPos, 1.0);
}