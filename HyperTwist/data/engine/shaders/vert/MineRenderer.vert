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

uniform float uXPositions[4];

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
    int IsValid;
} vs_out;

void main()
{
    int direction = int(vNoteOffset.x);

    vs_out.TexUV = vTexCoord * vTexScale + vTexOffset;
    vs_out.IsValid = direction;

    float xPos = vPos.x + uXPositions[direction];
    float yPos = vPos.y + vNoteOffset.y * uZoom * 2.0;
    gl_Position = proj * view * vec4(xPos, yPos, 1.75, 1.0);
}