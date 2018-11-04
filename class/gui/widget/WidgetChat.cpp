#include "WidgetChat.hpp"
#include "Univers.hpp"
#include "../../../imgui-sfml/imgui.h"

WidgetChat::WidgetChat(Univers &univers) :
AWidget(),
_univers(univers)
{
	bzero(this->_bufferMessage, IM_ARRAYSIZE(this->_bufferMessage));
}

WidgetChat::~WidgetChat(void)
{}

void			WidgetChat::addLog(const char* str, ...)
{
	va_list		args;

	va_start(args, str);
	this->_bufferChat.appendfv(str, args);
	this->_bufferChat.appendfv("\n", args);
	va_end(args);
	this->_scrollChat = true;
}
void			WidgetChat::clear(void)
{
	this->_bufferChat.clear();
}

void			WidgetChat::render(void)
{
	ImGui::Begin("Chat", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	if (ImGui::Button("Clear"))
		this->clear();
	ImGui::BeginChild("scrolling", ImVec2(0,ImGui::GetWindowHeight() - 4 * ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::TextUnformatted(this->_bufferChat.begin());
	if (this->_scrollChat)
		ImGui::SetScrollHereY(1.0f);
	this->_scrollChat = false;
	ImGui::EndChild();
	if (ImGui::InputText("Tap", this->_bufferMessage, IM_ARRAYSIZE(this->_bufferMessage), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (!this->_chatCommand())
		{
			std::string bufferMessage;
			ClientTCP::add_prefix(CHAT, bufferMessage, this->_bufferMessage,
								  sizeof(_bufferMessage));
			std::cout << "WidgetChat::render.size() " << bufferMessage.size() << std::endl;
			this->_univers.getClientTCP_().write_socket(bufferMessage);
		}
		bzero(this->_bufferMessage, IM_ARRAYSIZE(this->_bufferMessage));
	}
	ImGui::End();

}

bool			WidgetChat::_chatCommand(void)
{
	if (!(this->_bufferMessage[0] == '/'))
		return (false);
	if (strstr(this->_bufferMessage, "/help"))
		this->addLog("/help\n/name Aname\n", this->_bufferMessage);
	else if (strstr(this->_bufferMessage, "/name "))
		this->_univers.getClientTCP_().change_name(this->_bufferMessage + sizeof("/name ") - 1);
	else
		this->addLog("{%s} n'est pas une commande valide\n", this->_bufferMessage);
	return (true);
}
