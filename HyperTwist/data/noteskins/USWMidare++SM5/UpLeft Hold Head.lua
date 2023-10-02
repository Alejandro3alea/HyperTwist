if string.find(Var "Element", "Active") then
	return Def.Sprite {
		Texture=NOTESKIN:GetPath( '_UpLeft', 'holda' );
		Frames = Sprite.LinearFrames( 1, 1 );
	};
else
	return Def.Sprite {
		Texture=NOTESKIN:GetPath( '_UpLeft', 'holdi' );
		Frames = Sprite.LinearFrames( 1, 1 );
	};
end
