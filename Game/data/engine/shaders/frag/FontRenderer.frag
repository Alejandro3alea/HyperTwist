#version 400 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
    vec2 TexCoords;
    flat int index;
} fs_in;

// ------------------- UNIFORM ------------------
uniform sampler2DArray uText;
uniform int uLetterMap[255];
uniform vec3 uTextColor;

// --------------------- OUT --------------------
out vec4 FragColor;

// ----------------------------------------------------------------------------
void main()
{    
    vec4 letterSample = vec4(1.0, 1.0, 1.0, texture(uText, vec3(fs_in.TexCoords.xy, uLetterMap[fs_in.index])).r);
    FragColor = vec4(uTextColor, 1.0) * letterSample;
}