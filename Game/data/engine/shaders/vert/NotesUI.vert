#version 400 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexCoord;

// ------------------- UNIFORM -------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float uZoom;
uniform bool uFlipX;
uniform bool uFlipY;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
} vs_out;

void main()
{
    vs_out.TexUV = vec2(uFlipX ? 1.0 - vTexCoord.x : vTexCoord.x, uFlipY ? 1.0 - vTexCoord.y : vTexCoord.y);
    gl_Position = proj * view * model * vec4(vPos, 0.0, 1.0);
}