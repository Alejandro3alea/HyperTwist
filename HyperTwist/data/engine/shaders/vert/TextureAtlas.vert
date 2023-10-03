#version 400 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexCoord;

// ------------------- UNIFORM -------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 uViewPos;
uniform float uZoom;

uniform vec2 uTextureScale;
uniform vec2 uTextureOffset;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
} vs_out;

void main()
{
    mat4 newModel = model;
    newModel[3][1] *= uZoom;
    vs_out.TexUV = vTexCoord * uTextureScale + uTextureOffset;
    gl_Position = proj * view * newModel * vec4(vPos, 1.0, 1.0);
}