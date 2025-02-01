#version 400 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec2 vNoteOffset;
layout (location = 3) in vec2 vTexScale;
layout (location = 4) in vec2 vTexOffset;

// ------------------- UNIFORM -------------------
uniform mat4 view;
uniform mat4 proj;

uniform vec3 uViewPos;
uniform float uZoom;

uniform mat4 uRotations[4];
uniform float uXPositions[4];

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
    flat int IsValid;
} vs_out;

void main()
{
    int direction = int(vNoteOffset.x);
    mat4 newModel = uRotations[direction];
    newModel[3][0] += uXPositions[direction] * uZoom;
    newModel[3][1] += vNoteOffset.y;
    newModel[3][1] *= uZoom * 2.0;

    vs_out.TexUV = vTexCoord * vTexScale + vTexOffset;
    vs_out.IsValid = direction;

    gl_Position = proj * view * newModel * vec4(vPos*uZoom, 1.75, 1.0);
}