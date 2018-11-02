#include "WidgetSnake.hpp"
#include <iostream>
WidgetSnake::WidgetSnake(std::string const &str,
							std::vector<sf::Texture> &texture,
							std::vector<std::string> &color,
							bool isYourSnake = false) :
AWidget(),
_texture(texture),
_color(color),
_name(str),
_indexColor(0),
_isReady(false),
_isYourSnake(isYourSnake)
{
}

WidgetSnake::~WidgetSnake(void)
{}

void			WidgetSnake::render(void)
{
	if (!this->_isYourSnake)
		this->_renderYourSnake();
	unsigned int		sizeTexture;

	ImGui::Begin(this->_name.c_str(), NULL, ImGuiWindowFlags_NoDecoration);

	if (ImGui::GetWindowSize().x < ImGui::GetWindowSize().y - ImGui::GetFrameHeightWithSpacing() * 3)
		sizeTexture = ImGui::GetWindowSize().x * 0.8;
	else
		sizeTexture = ImGui::GetWindowSize().y * 0.8 - ImGui::GetFrameHeightWithSpacing() * 3;

	ImGui::PushItemWidth(sizeTexture);
	ImGui::SetCursorPosX(4);
	ImGui::LabelText(this->_name.c_str(), "Name : ");


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::Image(this->_texture[this->_indexColor], sf::Vector2f(sizeTexture, sizeTexture));


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::PushID(0);
	if (!this->_isReady)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.9f, 0.9f));
		if (ImGui::Button("Ready ?", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_isReady = true;
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.33f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.33f, 0.9f, 0.9f));
		if (ImGui::Button("Ready !", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_isReady = false;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	if (ImGui::BeginCombo("", this->_color[this->_indexColor].c_str(), ImGuiComboFlags_None))
	{
		unsigned int i = 0;
		for (auto const &e : this->_color)
		{
			if (ImGui::Selectable(e.c_str(), i == this->_indexColor))
				this->_indexColor = i;
			i++;
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}

void			WidgetSnake::_renderYourSnake(void)
{
	unsigned int		sizeTexture;

	ImGui::Begin(this->_name.c_str(), NULL, ImGuiWindowFlags_NoDecoration);

	if (ImGui::GetWindowSize().x < ImGui::GetWindowSize().y - ImGui::GetFrameHeightWithSpacing() * 3)
		sizeTexture = ImGui::GetWindowSize().x * 0.8;
	else
		sizeTexture = ImGui::GetWindowSize().y * 0.8 - ImGui::GetFrameHeightWithSpacing() * 3;



	ImGui::PushItemWidth(sizeTexture);
	ImGui::SetCursorPosX(4);
	ImGui::LabelText(this->_name.c_str(), "Name : ");


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::Image(this->_texture[this->_indexColor], sf::Vector2f(sizeTexture, sizeTexture));


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	ImGui::PushID(0);
	if (!this->_isReady)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.9f, 0.9f));
		if (ImGui::Button("Ready ?", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_isReady = true;
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.33f, 0.8f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.33f, 0.9f, 0.9f));
		if (ImGui::Button("Ready !", sf::Vector2f(sizeTexture, ImGui::GetFrameHeight())))
			this->_isReady = false;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - sizeTexture) / 2);
	if (ImGui::BeginCombo("", this->_color[this->_indexColor].c_str(), ImGuiComboFlags_NoArrowButton))
	{
		unsigned int i = 0;
		for (auto const &e : this->_color)
		{
			if (ImGui::Selectable(e.c_str(), i == this->_indexColor))
				this->_indexColor = i;
			i++;
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}