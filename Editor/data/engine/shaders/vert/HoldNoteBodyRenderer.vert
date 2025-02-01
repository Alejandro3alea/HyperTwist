#version 400 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in float vHoldNoteType;
layout (location = 3) in float vNoteOffsetX;
layout (location = 4) in float vNoteOffsetY;
layout (location = 5) in float vNoteSize;

// ------------------- UNIFORM -------------------
uniform mat4 view;
uniform mat4 proj;

uniform float uZoom;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	vec2 TexUV;
    float HoldNoteType;
} vs_out;

void main()
{
    float HoldScale = (vNoteSize + 0.0) * uZoom;
    vs_out.TexUV =vec2(vTexCoord.x, (1.0 - vTexCoord.y) * HoldScale / 4.0);
    vs_out.HoldNoteType = vHoldNoteType;

    mat4 newModel = mat4(1.0);
    newModel[3][0] += vNoteOffsetX;
    newModel[3][1] += vNoteOffsetY + 0.0;
    newModel[3][1] *= uZoom * 2.0;
    newModel[1][1] *= HoldScale;

    gl_Position = proj * view * newModel * vec4(vPos, 1.001, 1.0);
}