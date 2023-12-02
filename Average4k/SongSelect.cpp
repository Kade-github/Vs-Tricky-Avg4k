#include "SongSelect.h"
#include "Gameplay.h"

int SongSelect::selectedDiffIndex = 0;
Chart* SongSelect::currentChart = NULL;
song* SongSelect::selectedSong = NULL;
int SongSelect::lastChecked = 0;

void SongSelect::updateList() {

	listOfCharts.clear();

	for (const auto& entry : std::filesystem::directory_iterator("assets/charts/"))
	{
		for (auto& e : std::filesystem::directory_iterator(entry.path()))
		{
			std::string bruh = e.path().string();

			std::transform(bruh.begin(), bruh.end(), bruh.begin(),
				[](unsigned char c) { return std::tolower(c); });
			if (ends_with(bruh, ".sm"))
			{
				song s;
				s.type = StepMania;
				s.steam = false;
				strcpy_s(s.folder, entry.path().string().c_str());
				strcpy_s(s.path, bruh.c_str());
				listOfCharts.push_back(s);
			}

		}
	}

	selectedIndex = 0;
	selectedDiffIndex = 0;

	infoText->setText("F5 to refresh - " + std::to_string(listOfCharts.size()) + " Charts loaded");
	infoText->centerX();


	if (listOfCharts.size() != 0)
	{

		switchChart(listOfCharts[0]);

		song& selectedChart = listOfCharts[selectedIndex];
		std::string name = currentChart->meta.songName;
		std::string diff = currentChart->meta.difficulties[selectedDiffIndex].name;

		songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
		songName->centerX();
	}

}

void SongSelect::update(Events::updateEvent event)
{

	if (!currentChart)
		return;

}

void SongSelect::keyDown(SDL_KeyboardEvent event)
{
	if (listOfCharts.size() == 0)
	{
		currentChart = NULL;

		updateList();
	}
	else
	{
		Color c;
		song selectedChart = listOfCharts[selectedIndex];
		std::string name = currentChart->meta.songName;
		std::string diff = currentChart->meta.difficulties[selectedDiffIndex].name;
		switch (event.keysym.sym)
		{
		case SDLK_DOWN:
			if (!allowMove)
				return;
			selectedIndex++;
			selectedDiffIndex = 0;

			if (selectedIndex > listOfCharts.size() - 1)
				selectedIndex = 0;

			if (currentChart)
				currentChart->destroy();

			selectedChart = listOfCharts[selectedIndex];

			switchChart(selectedChart);

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();

			if (listOfCharts[selectedIndex].steam)
			{
				c.r = 128;
				c.g = 128;
				c.b = 255;
			}
			else
			{
				c.r = 128;
				c.g = 255;
				c.b = 128;
			}

			break;
		case SDLK_UP:
			if (!allowMove)
				return;
			selectedIndex--;
			selectedDiffIndex = 0;

			if (selectedIndex < 0)
				selectedIndex = listOfCharts.size() - 1;

			selectedChart = listOfCharts[selectedIndex];


			if (currentChart)
				currentChart->destroy();

			switchChart(selectedChart);

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();


			break;
		case SDLK_LEFT:
			if (!currentChart || !allowMove)
				return;
			selectedDiffIndex--;
			if (selectedDiffIndex < 0)
				selectedDiffIndex = currentChart->meta.difficulties.size() - 1;

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();


			break;
		case SDLK_RIGHT:
			if (!currentChart || !allowMove)
				return;
			selectedDiffIndex++;
			if (selectedDiffIndex > currentChart->meta.difficulties.size() - 1)
				selectedDiffIndex = 0;

			diff = currentChart->meta.difficulties[selectedDiffIndex].name;

			songName->setText("> " + currentChart->meta.songName + " (" + diff + ") ");
			songName->centerX();

			break;
		case SDLK_F5:
			if (!allowMove)
				return;
			currentChart = NULL;
			updateList();
			break;
		case SDLK_RETURN:
			if (!allowMove)
				return;

			Game::instance->transitionToMenu(new Gameplay());
			free(selectedSong);
			break;
		case SDLK_ESCAPE:

			Game::instance->transitionToMenu(new MainMenu());
			free(selectedSong);
			break;
		}
	}
}

SongSelect::SongSelect()
{

}

void SongSelect::create() {
	addCamera(Game::mainCamera);
	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/mm/bg.png");
	sprite->create();
	add(sprite);

	AvgRect* rect = new AvgRect(0, 0, 1280, 720);
	rect->alpha = 0.3;
	add(rect);

	songName = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) - 200, "No songs loaded", 24, "NotoSans-Regular");
	songName->centerX();
	songName->centerY();

	songName->create();
	add(songName);

	if (!currentChart)
		delete currentChart;

	currentChart = NULL;

	infoText = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) - 275, "", 24, "NotoSans-Regular");
	infoText->create();
	add(infoText);

	selectedSong = (song*)malloc(sizeof(song)); // REMEMBER TO FREE THIS KADE PLEASE (- kade from the past)

	created = true;

	updateList();
}

void SongSelect::switchChart(song s)
{
	// cannot do switch due to "YOU CANNOT DEFINE VARIABLES IN SWITCHES OR SOMETHING"

	int time = SDL_GetTicks();


	SMFile* filee = new SMFile(s.path, s.folder, false);
	currentChart = new Chart(filee->meta);
	delete filee;


	if (selectedSong)
	{
		free(selectedSong);
		selectedSong = (song*)malloc(sizeof(song));
	}

	strcpy_s(selectedSong->path, listOfCharts[selectedIndex].path);
	selectedSong->steam = s.steam;
	selectedSong->type = listOfCharts[selectedIndex].type;
	std::cout << "took " << (SDL_GetTicks() - time) << "ms for " << s.type << std::endl;
	std::string path = currentChart->meta.folder + "/" + currentChart->meta.audio;
	std::cout << "playing " << path << std::endl;
	if (songPrev != NULL)
	{
		if (songPrev->path == path)
			return;
		songPrev->stop();
		songPrev->free();
		SoundManager::removeChannel("prevSong");
	}
	if (SoundManager::getChannelByName("prevSong") == NULL)
		songPrev = SoundManager::createChannel(path, "prevSong");
	else
		songPrev = SoundManager::getChannelByName("prevSong");
	songPrev->play();

}
