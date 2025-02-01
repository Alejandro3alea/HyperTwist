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
	vec4 texColor = texture(uTexture, fs_in.TexUV).rgba;
	
	if (texColor.a < 0.1)
		discard;

    FragColor = vec4(texColor.r * uColor.r, texColor.g * uColor.g, texColor.b * uColor.b, texColor.a * uColor.a);
}