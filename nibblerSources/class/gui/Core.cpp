#include <Univers.hpp>
#include "Core.hpp"
#include "widget/WidgetExit.hpp"
#include "widget/WidgetSnake.hpp"
#include "widget/WidgetLobby.hpp"
#include "gui/widget/WidgetOption.hpp"
#include "gui/widget/WidgetConnect.hpp"
#include "gui/widget/WidgetMassiveButton.hpp"

Core::Core(Univers &univers) :
univers(univers),
pathRessources_(boost::filesystem::path(NIBBLER_ROOT_PROJECT_PATH) / "ressources/"),
_winSize(sf::Vector2<unsigned int>(1000, 900)),
_win(sf::VideoMode(_winSize.x, _winSize.y), "Project Sanke"),
_io(_createContext()),
_chat(*this),
_mapSize(sf::Vector2<int>(20, 20))
{
	if (!_imageTitleScreen.loadFromFile((pathRessources_ / "ecran_titre.png").generic_string()))
		(throw(Core::CoreConstructorException("Cannot load background")));
	_io.IniFilename = NULL;
}

ImGuiIO			&Core::_createContext(void)
{
	_win.setFramerateLimit(60);
	ImGui::SFML::Init(_win);

	// Give Focus on Widow
	sf::Event event;
	event.type = sf::Event::GainedFocus;
	ImGui::SFML::ProcessEvent(event);

	return (ImGui::GetIO());
}

Core::~Core(void)
{
	ImGui::SFML::Shutdown();
}

boost::filesystem::path const	&Core::getPathRessources() const {
	return (pathRessources_);
}

void			Core::_updateGenCoreEvent() {
	sf::Event event;
	while (_win.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
			_win.close();
	}
	ImGui::SFML::Update(_win, _deltaClock.restart());
}

void			Core::titleScreen() {
	sf::Event	event;
	bool		titleScreen = true;

	while (_win.isOpen() && titleScreen)
	{
		while (_win.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				_win.close();
			else if (event.type == sf::Event::KeyPressed)
				titleScreen = false;
		}
		ImGui::SFML::Update(_win, _deltaClock.restart());
		ImGui::SetNextWindowPosCenter();
		ImGui::Begin("Titlescreen", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		ImGui::Image(_imageTitleScreen);
		ImGui::End();
		_render();
	}
}

void			callbackExit(void *ptr)
{
	static_cast<Core *>(ptr)->exit();
}

void			Core::aState(void)
{
	WidgetExit wexit(*this, &callbackExit, this);
	WidgetLobby lobby(*this);
	WidgetOption *optionSnake = nullptr;
	WidgetConnect optionConnect(*this);
	WidgetMassiveButton massiveButton(*this);


	while (_win.isOpen() && !univers.isOpenGame_())
	{
		sf::Event event;
		while (_win.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed) {

				switch (event.key.code) {
					case sf::Keyboard::A:
						univers.callbackAction(kCreateIA);
						break;
					case sf::Keyboard::B:
						univers.callbackAction(kBorderless);
						break;
					case sf::Keyboard::C:
						univers.callbackAction(kCreateClient);
						break;
					case sf::Keyboard::D:
						univers.callbackAction(kDeleteClient);
						break;
					case sf::Keyboard::E:
						univers.callbackAction(kConnect);
						break;
					case sf::Keyboard::R:
						univers.callbackAction(kReady);
						break;
					case sf::Keyboard::S:
						univers.callbackAction(kCreateServer);
						break;
					case sf::Keyboard::W:
						univers.callbackAction(kDeleteServer);
						break;
					case sf::Keyboard::X:
						univers.callbackAction(kStartGame);
						break;
					default:
						break;
				}
			}
			_processEvent(event);
			ImGui::SFML::ProcessEvent(event);
		}

		ImGui::SFML::Update(_win, _deltaClock.restart());

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(0, 50)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(50, 50)));
		_chat.render();

		lobby.render();

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(95, 0)), 0, sf::Vector2f(0.5f, 0.5f));
		wexit.render();

		if (univers.getSnakeClient() && univers.getSnakeClient()->isConnect()) {
			if (!optionSnake)
				optionSnake = new WidgetOption(*this);
			ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(70, 50)));
			ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(30, 25)));
			optionSnake->render();
			ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(70, 75)));
			ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(30, 25)));
			optionConnect.render();
		}
		else {
			ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(70, 50)));
			ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(30, 50)));
			optionConnect.render();
		}

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(50, 50)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(20, 50)));
		massiveButton.render();

		_render();

	}
	if (optionSnake)
		delete optionSnake;
}

void				Core::_render(void) {
	_win.clear();
	ImGui::SFML::Render(_win);
	_win.display();
}

void 				Core::exit(void) {
	_win.close();
}

sf::Vector2<unsigned int>	Core::positionByPercent(sf::Vector2<unsigned int> const &percent) const {
	return (sf::Vector2<unsigned int>(_winSize.x * percent.x / 100,
										_winSize.y * percent.y / 100));
}

void					Core::_processEvent(sf::Event const &event) {
	if (event.type == sf::Event::Resized)
		_winSize = sf::Vector2<unsigned int>(event.size.width, event.size.height);
	else if (event.type == sf::Event::Closed)
		_win.close();
}


void					Core::beginColor(float const color) {
	assert(!Core::_useColor);
	Core::_useColor = true;
	ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(color, 0.7f, 0.7f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(color, 0.8f, 0.8f)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(color, 0.9f, 0.9f)));
}
void					Core::endColor() {
	ImGui::PopStyleColor(3);
	Core::_useColor = false;
}


float const 				Core::HUE_GREEN = 0.33f;
float const 				Core::HUE_RED = 0.f;

bool 						Core::_useColor = false;

Core::CoreConstructorException::~CoreConstructorException(void) noexcept{}
Core::CoreConstructorException::CoreConstructorException(void) noexcept :
	_error("Error on Core constructor") {}
Core::CoreConstructorException::CoreConstructorException(std::string s) noexcept :
	_error(s) { }
Core::CoreConstructorException::CoreConstructorException(Core::CoreConstructorException const &src) noexcept :
	_error(src._error)
	{ _error = src._error; }
const char	*Core::CoreConstructorException::what() const noexcept
	{ return (_error.c_str()); }

