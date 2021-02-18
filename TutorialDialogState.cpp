#include "TutorialDialogState.h"
#include "GameApp.h"

void TutorialDialogState::Initialize()
{
	const DirectX::XMINT2 s = GameApp::Get().GetClientSizeU2(); //todo on resize
	const RECT panelSrc{ GC::PANEL_SRC[0],	GC::PANEL_SRC[1],	GC::PANEL_SRC[2],	GC::PANEL_SRC[3], }; // sprite coords

	// viewport padding
	RECT panelDst
	{
		s.x / 4,
		s.y / 4,
		s.x - (s.x / 4),
		s.y - (s.y / 4)
	};

	mPanel.Initialize("uiTex",panelSrc,panelDst);

	// offset Text padding
	panelDst.left += 32;
	panelDst.top += 32;

	mText.position = { (float)panelDst.left ,(float)panelDst.top };
	mText.string = "Shouldn't see this!";
}

void TutorialDialogState::Update(const GameTimer & gt)
{
	if (Input::Get().KeyReleased(GC::KEY_PAUSE))
	{
		GameApp::Get().ChangeState(GC::STATE_PLAY);
	}
}

void TutorialDialogState::Draw(const GameTimer & gt)
{
	mPanel.Draw();
	mText.Draw();
}

void TutorialDialogState::OnResume()
{
	mText.string = mDialog;
}

void TutorialDialogState::OnPause()
{
}

void TutorialDialogState::Reset()
{
}
