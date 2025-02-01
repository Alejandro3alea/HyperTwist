#version 400 core

// --------------------- IN ---------------------
in OUT_IN_VARIABLES 
{
	vec2 TexUV;
    float HoldNoteType;
} fs_in;

// --------------------- OUT --------------------
out vec4 FragColor;

// ------------------- UNIFORM ------------------
uniform sampler2D uHoldNote;
uniform sampler2D uHoldBottomCap;
uniform sampler2D uRollNote;
uniform sampler2D uRollBottomCap;

// ----------------------------------------------------------------------------
void main()
{
	if (fs_in.HoldNoteType == -1.0)
		discard;

	vec4 texColor;
	
	if (fs_in.TexUV.y <= 0.1475)
	{
		vec2 newUV = vec2(fs_in.TexUV.x, fs_in.TexUV.y / 0.15);
		if (fs_in.HoldNoteType == 0.0)
			texColor = texture(uHoldBottomCap, newUV);
		else
			texColor = texture(uRollBottomCap, newUV);
	}
	else
	{
		vec2 newUV = vec2(fs_in.TexUV.x, fs_in.TexUV.y - 0.15);
		if (fs_in.HoldNoteType == 0.0)
			texColor = texture(uHoldNote, newUV);
		else
			texColor = texture(uRollNote, newUV);
	}

	float alphaVal = texColor.a * texColor.a;
	if (alphaVal < 0.5)
		discard;

    FragColor = vec4(texColor.rgb, alphaVal);
}