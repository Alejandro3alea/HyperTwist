#version 400 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	float AlphaVal;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;


// ----------------------------------------------------------------------------
void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, fs_in.AlphaVal);
}