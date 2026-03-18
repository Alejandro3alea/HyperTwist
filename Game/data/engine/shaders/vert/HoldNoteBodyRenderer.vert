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
uniform float uSongOffset;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES 
{
	vec2 TexUV;
    float HoldNoteType;
} vs_out;

void main()
{
    const float noteDefaultScale = 60.0;
    float HoldScale = (vNoteSize + 0.0) * uZoom;
    vs_out.TexUV =vec2(vTexCoord.x, (1.0 - vTexCoord.y) * HoldScale / 4.0);
    vs_out.HoldNoteType = vHoldNoteType;

    // @TODO: Change from matrix
    mat4 newModel = mat4(1.0);
    newModel[3][0] += vNoteOffsetX * noteDefaultScale;
    newModel[3][1] += vNoteOffsetY - uSongOffset;
    newModel[3][1] *= uZoom * 2.0 * noteDefaultScale;
    newModel[1][1] *= HoldScale;

    // @TODO: Change from matrix
    float xPos = (vPos.x + vNoteOffsetX) * noteDefaultScale;
    float yPos = (vPos.y  * HoldScale + (vNoteOffsetY - uSongOffset) * uZoom * 2.0 * noteDefaultScale) * noteDefaultScale;

    gl_Position = proj * view * newModel * vec4(vPos * noteDefaultScale, 1.001, 1.0);
}