#version 400 core
layout (location = 0) in vec2 vPos;

out OUT_IN_VARIABLES {
    vec2 TexCoords;
    flat int index;
} vs_out;

uniform mat4 uTransforms[255];
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * uTransforms[gl_InstanceID] * vec4(vPos.xy, 0.0, 1.0);
    vs_out.index = gl_InstanceID;
    vs_out.TexCoords = vPos.xy;
    vs_out.TexCoords.y = 1.0f - vs_out.TexCoords.y;
}