#version 400 core

layout (location = 0) in vec2 vPos;

// ------------------- UNIFORM -------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

// --------------------- OUT ---------------------
out OUT_IN_VARIABLES {
	float AlphaVal;
} vs_out;

void main()
{
    
    vs_out.AlphaVal = mod(vPos.y, 4) != 0 ? 0.25 : 1.0;
    gl_Position = proj * view * vec4(vPos, 0.25, 1.0);
}