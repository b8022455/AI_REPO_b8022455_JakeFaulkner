#include "WinState.h"
#include "GameApp.h"

WinState::WinState(const Text& t1, const Text& t2, const Button& btn)
{
	mTitle = t1;
	mDescription = t2;
	mBtn = btn;

	DirectX::XMFLOAT2 centre = GameApp::Get().GetClientSize();
	centre.x *= 0.5f;
	centre.y *= 0.6f;

	mBtn.SetPos({ centre.x, centre.y });
}

void WinState::Initialize()
{

}

void WinState::Update(const GameTimer& gt)
{
	if (Input::Get().AnyMenuButtonPressed())
		GameApp::Get().PlayClickDownAudio();

	if (Input::Get().AnyMenuButtonPressed())
		mBtn.SetColor(GC::BUTTON_DOWN_COLOR);

	if (Input::Get().MenuInputUp())		//Go back to start menu
	{
		GameApp::Get().PlayClickUpAudio(true);
		mBtn.SetColor(GC::BUTTON_UP_COLOR);
		mBtn.Activate();
	}

}

void WinState::Draw(const GameTimer& gt)
{
	mTitle.Draw();
	mDescription.Draw();
	DrawLeaderboard();
}

void WinState::OnResume()
{

}

void WinState::DrawLeaderboard()
{
	std::vector<int> scores(5);
	mLeaderboard.scale = 1.5f;
	mLeaderboard.position = DirectX::SimpleMath::Vector2(320.f, 140.f);
	std::stringstream txt;

	//Get leaderboard from txt file
	std::ifstream fin;
	fin.open("Data/Leaderboard.txt", std::ios::out);
	if (fin.fail())
		assert(fin.fail());
	else
		for (int i = 0; i < scores.size(); i++)		//gets scores from txt file
			fin >> scores.at(i);

	fin.close();

	txt << "1.    " << scores.at(0) << "\n\n"
		<< "2.    " << scores.at(1) << "\n\n"
		<< "3.    " << scores.at(2) << "\n\n"
		<< "4.    " << scores.at(3) << "\n\n"
		<< "5.    " << scores.at(4) << "\n\n";

	mLeaderboard.string = txt.str().c_str();
	mLeaderboard.Draw();
}
