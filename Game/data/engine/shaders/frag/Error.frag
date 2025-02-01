#version 400 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	vec2 TexUV;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform sampler2D uTexture;
uniform vec4 uColor;

// ----------------------------------------------------------------------------
void main()
{
    FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}