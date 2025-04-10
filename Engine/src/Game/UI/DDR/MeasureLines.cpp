#include "MeasureLines.h"

void MeasureLines::OnSongChange(Song* InSong)
{
	mNotesBackground = std::unique_ptr<Renderable>(new Renderable());
	mNotesBackground->SetShader("engine/shaders/NotesUI.shader");
	mNotesBackground.get()->mColor = { 0.0f, 0.0f, 0.0f, 0.75f };
	mNotesBackground.get()->transform.pos = { 0.0f, 0.0f, 0.1f };
	mNotesBackground.get()->transform.scale = { 5.5f, 100000.0f, 0.05f };

	mBeatsBackground = std::unique_ptr<Renderable>(new Renderable());
	mBeatsBackground->SetShader("engine/shaders/NotesUI.shader");
	mBeatsBackground.get()->mColor = { 0.0f, 0.0f, 0.0f, 0.75f };
	mBeatsBackground.get()->transform.pos = { 6.75f, 0.0f, 0.1f };
	mBeatsBackground.get()->transform.scale = { 1.0f, 100000.0f, 0.05f };

	if (mLineList.get())
	{
		mLineList.release();
	}
	
	std::vector<glm::vec2> Positions;
	for (unsigned i = 0; i < 1000; i++)
	{
		int repetitions = (i % 4 == 0) ? 4 : 1;

		for (unsigned j = 0; j < repetitions; j++)
		{
			Positions.push_back({ -5.0f, i * 2 });
			Positions.push_back({ 5.0f, i * 2 });

			Positions.push_back({ 6.0f, i * 2 });
			Positions.push_back({ 7.5f, i * 2 });
		}
	}

	mLineList = std::unique_ptr<LineList>(new LineList(Positions));
}
