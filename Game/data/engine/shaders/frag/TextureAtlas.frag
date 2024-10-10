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

// ----------------------------------------------------------------------------
void main()
{
	vec4 texColor = texture(uTexture, fs_in.TexUV).rgba;
	
	if (texColor.a < 0.5)
		discard;

    FragColor = vec4(texColor.rgb, texColor.a);
}