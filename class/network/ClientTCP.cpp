#include <logger.h>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <nibbler.hpp>
#include "asio.hpp"
#include <Univers.hpp>
#include "ClientTCP.hpp"
#include <gui/Core.hpp>

int const ClientTCP::size_header[] = {
		[CHAT] = SIZEOF_CHAT_PCKT,
		[FOOD] = sizeof(PositionComponent),
		[ID] = sizeof(int16_t),
		[OPEN_GAME] = sizeof(bool),
		[START_GAME] = sizeof(StartInfo),
		[SNAKE] = sizeof(Snake),
		[SNAKE_ARRAY] = sizeof(Snake) * MAX_SNAKE,
		[HEADER] = sizeof(eHeader),
		[INPUT] = sizeof(InputInfo),
		[RESIZE_MAP] = sizeof(unsigned int),
		[REMOVE_SNAKE] = sizeof(int16_t)
};

ClientTCP::ClientTCP(::Univers &univers, boost::asio::io_service &io)
		: isConnect_(false),
		  openGame_(false),
		  id_(0),
		  univers(univers),
		  resolver(io),
		  socket(io),
		  factory(univers),
		  io(io){

}

void ClientTCP::send_host_open_game(void) {
	bool data;
	data = true;
	write_socket(add_prefix(OPEN_GAME, &data));
}

void ClientTCP::change_map_size(unsigned int size) {
	unsigned int size2D[2];
	size2D[0] = size;

	write_socket(add_prefix(RESIZE_MAP, &size2D));
}

void ClientTCP::change_name(char const *name) {
	bzero(snake_array[id_].name, NAME_BUFFER);
	if (strlen(name) > NAME_BUFFER)
		strncpy(snake_array[id_].name, name, NAME_BUFFER - 1);
	else
		strcpy(snake_array[id_].name, name);
	this->refreshMySnake();
}

void ClientTCP::change_sprite(eSprite snakeSprite) {
	snake_array[id_].sprite = snakeSprite;
	this->refreshMySnake();
}

void ClientTCP::change_state_ready(void) {
	snake_array[id_].isReady = !snake_array[id_].isReady;
	this->refreshMySnake();
}

void ClientTCP::refreshMySnake(void) {
	write_socket(add_prefix(SNAKE, &snake_array[id_]));
}

void ClientTCP::connect(std::string dns, std::string port) {
	try {
		tcp::resolver::query query(dns, port);
		tcp::resolver::iterator it = resolver.resolve(query);
		boost::asio::connect(socket, it);
		read_socket_header();
		thread = boost::thread(boost::bind(&boost::asio::io_service::run, &io));
		thread.detach();
		isConnect_ = true;
	} catch (std::exception &exception) {
		std::cout << exception.what() << std::endl;
	}

}

void ClientTCP::read_socket_header() {
	boost::asio::async_read(socket, boost::asio::buffer(buffer_data,
														ClientTCP::size_header[HEADER]),
							boost::bind(&ClientTCP::handle_read_header,
										shared_from_this(),
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void ClientTCP::read_socket_data(eHeader header) {
	boost::asio::async_read(socket, boost::asio::buffer(buffer_data,
														ClientTCP::size_header[header]),
							boost::bind(&ClientTCP::handle_read_data,
										shared_from_this(),
										header,
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void ClientTCP::handle_read_header(const boost::system::error_code &error_code,
								   size_t len) {
//	std::cout << "ClientTCP::handle_read_header > len : " << len << std::endl;
	id = std::this_thread::get_id();
	if (error_code.value() == 0) {
		assert(len == sizeof(eHeader));
		eHeader header;
		std::memcpy(&header, buffer_data.data(), sizeof(eHeader));
//		std::cout << "Header : " << header << std::endl;
		read_socket_data(header);
	}
}

int16_t ClientTCP::getId() const {
	return id_;
}
bool	ClientTCP::isConnect() const {
	return isConnect_;
}
bool	ClientTCP::isOpenGame() const {
	return openGame_;
}


void ClientTCP::write_socket(std::string message) {
	boost::asio::async_write(socket, boost::asio::buffer(message),
							 boost::bind(&ClientTCP::handle_write,
										 shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred));
}

void ClientTCP::parse_input(eHeader header, void const *input, size_t len) {



	switch (header) {
		case CHAT: {
			log_info("eHeader::CHAT");
			char *data_deserialize = new char[len];
			std::memcpy(data_deserialize, input, len);
			univers.getCore_().addMessageChat(std::string(data_deserialize, len));
			delete [] data_deserialize;
			break;
		}
		case SNAKE_ARRAY: {
			log_info("eHeader::SNAKE_ARRAY");
			std::memcpy(snake_array, input, sizeof(Snake) * MAX_SNAKE);
			break;
		}
		case SNAKE: {
			log_info("eHeader::SNAKE");

			Snake snake_temp;

			std::memcpy(&snake_temp, input, len);

			snake_array[snake_temp.id] = snake_temp;

			univers.playNoise(eSound::READY);
			break;
		}
		case REMOVE_SNAKE: {
			log_info("eHeader::REMOVE_SNAKE");

			snake_array[*(reinterpret_cast< const int16_t *>(input))].id = -1;

			univers.playNoise(eSound::DEATH);
			break;
		}
		case FOOD: {
			log_info("eHeader::FOOD");

			PositionComponent positionComponent;
			std::memcpy(&positionComponent, input, len);
			foodCreations.push_back(FoodCreation(positionComponent));
			break;
		}

		case ID:
			log_info("eHeader::ID");

			std::memcpy(&id_, input, len);
			break;
		case OPEN_GAME: {
			log_info("eHeader::OPEN_GAME");

			ClientTCP::StartInfo startInfo;
			bool data;
			std::memcpy(&data, input, ClientTCP::size_header[OPEN_GAME]);
			openGame_ = data;
		}
			break;
		case START_GAME: {
			log_info("eHeader::START_GAME");

			StartInfo st;
			std::memcpy(&st, input, ClientTCP::size_header[START_GAME]);
			factory.create_all_snake(snake_array, st.nu);
			univers.getWorld_().getEventManager().emitEvent<StartEvent>(st.time_duration);
		}
			break;
		case INPUT: {

			InputInfo ii;
			std::memcpy(&ii, input, len);
			joystickEvents.push_back(JoystickEvent(ii.id, ii.dir));
		}
			break;
		case RESIZE_MAP: {
			log_info("eHeader::RESIZE_MAP");

			unsigned int buffer;
			std::memcpy(&buffer, input, ClientTCP::size_header[RESIZE_MAP]);
			univers.setMapSize(buffer);
			univers.playNoise(eSound::RESIZE_MAP);
		}
			break;
		default:
			break;
	}
}


void ClientTCP::deliverEvents() {
	mu.lock();
	for (auto foodCreation : foodCreations) {
		univers.getWorld_().getEventManager().emitEvent(foodCreation);
	}
	for (auto joystickEvent : joystickEvents) {
		univers.getWorld_().getEventManager().emitEvent(joystickEvent);
	}
	joystickEvents.clear();
	foodCreations.clear();
	mu.unlock();
}


void ClientTCP::handle_read_data(eHeader header, boost::system::error_code const &error_code,
								 size_t len) {
	if (error_code.value() == 0 && len > 0) {
		parse_input(header, buffer_data.data(), len);
	}
	read_socket_header();
}

void ClientTCP::handle_write(const boost::system::error_code &error_code,
							 size_t len) {
}

ClientTCP::pointer_client
ClientTCP::create(Univers &univers, boost::asio::io_service &io) {
	return pointer_client(new ClientTCP(univers, io));
}
Snake const *ClientTCP::getSnakes() const {
	return snake_array;
}
Snake	const &ClientTCP::getSnake(void) const {
	std::cout << this->snake_array[this->id_] << std::endl;
	return this->snake_array[this->id_];
}