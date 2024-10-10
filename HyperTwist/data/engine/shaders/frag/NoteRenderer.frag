#version 400 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	vec2 TexUV;
	flat int IsValid;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform sampler2D uTexture;

// ----------------------------------------------------------------------------
void main()
{
	if (fs_in.IsValid == -1)
		discard;

	vec4 texColor = texture(uTexture, fs_in.TexUV).rgba;
	
	float alphaVal = texColor.a * texColor.a;
	if (alphaVal < 0.5)
		discard;

    FragColor = vec4(texColor.rgb, alphaVal);
}