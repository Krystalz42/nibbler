#include "ServerTCP.hpp"
#include "ClientTCP.hpp"
#include <factory/Factory.hpp>
#include <logger.h>

ServerTCP::ServerTCP(boost::asio::io_service &io_service, unsigned int port)
		: nu_(0),
		  acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		  mapSize(30) {
	//std::cout << "Server created" << std::endl;
	start_accept();
}

void ServerTCP::start_accept() {
	acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {

		if (!ec) {
			TCPConnection::pointer new_connection =
					TCPConnection::create(snake_array[nu_], acceptor_.get_io_service(), *this);
			new_connection->socket() = std::move(socket);

			int16_t first_id = 0;

			for (int i = 0; i < MAX_SNAKE; i++) {
				if (snake_array[i].id == -1) {
					first_id = i;
					break ;
				}
			}

			snake_array[first_id].id = first_id;
			snake_array[first_id] = Snake::randomSnake(first_id);
			new_connection->read_socket_header();
			pointers.push_back(new_connection);
			refresh_data_snake_array(new_connection, first_id);
			refresh_data_map_size(new_connection);
			++nu_;
		}
		start_accept();
	});
}

void ServerTCP::erase_snake(Snake const &snake) {

	/*for (int i = 0; i < MAX_SNAKE; i++) {
		if (snake_array[i].id == snake.id) {
			for (; i < MAX_SNAKE; i++) {
				if (i == MAX_SNAKE || i + 1 == MAX_SNAKE) {
					snake_array[i].id = -1;
					break ;
				}
				else {
					snake_array[i] = snake_array[i + 1];
				}
			}
			break ;
		}
	}
	nu_--;
	async_write(ClientTCP::add_prefix(SNAKE_ARRAY, snake_array));*/

	nu_--;
	int16_t id = snake.id;
	snake_array[id].id = -1;
	async_write(ClientTCP::add_prefix(REMOVE_SNAKE, &id));
}


void ServerTCP::refresh_data_snake_array(
		TCPConnection::pointer &connection,
		int16_t id) {
	{
//		std::cout << "ServerTCP::refresh_data_snake_array : " << id << std::endl;
		connection->write_socket(ClientTCP::add_prefix(ID, &id));
	}
	{
//		std::cout << "ServerTCP::refresh_data_snake_array : " << std::endl;
		connection->write_socket(ClientTCP::add_prefix(SNAKE_ARRAY, snake_array));
	}
	{
//		std::cout << "ServerTCP::refresh_data_snake_array : " << std::endl;
		async_write(ClientTCP::add_prefix(SNAKE, &snake_array[id]));
	}
}

void ServerTCP::refresh_data_map_size(TCPConnection::pointer &connection) {
	async_write(ClientTCP::add_prefix(RESIZE_MAP, &mapSize));
}

void ServerTCP::start_game() {
	for (int index = 0; index < MAX_SNAKE; ++index) {
		if (!snake_array[index].isReady && snake_array[index].id != -1) {
			std::cerr << "Error " << index << std::endl;
			return;
		}
	}
	ClientTCP::StartInfo startInfo;
	startInfo.nu = nu_;
	startInfo.time_duration = boost::posix_time::microsec_clock::universal_time();
	std::cout << "Start timer in server " << std::endl;
	int max_food = (nu_ > 1 ? nu_ - 1 : nu_);
	for (int index = 0; index < max_food; ++index) {
		std::cout << max_food << std::endl;
		PositionComponent positionComponent((rand() % (30 - 2)) + 1, (rand() % (30 - 2)) + 1);
		async_write(ClientTCP::add_prefix(FOOD, &positionComponent));
	}
	async_write(ClientTCP::add_prefix(START_GAME, &startInfo));
}

void ServerTCP::async_write(std::string message) {
	for (auto &pointer : pointers) {
		pointer->write_socket(message);
	}
}

void ServerTCP::async_write(void const *input, size_t len) {
	for (auto &pointer : pointers) {
		pointer->write_socket(input, len);
	}
}

void ServerTCP::parse_input(eHeader header, void const *input, size_t len) {

	std::cout << std::endl;

	char *data_deserialize = new char[len];

	std::memcpy(data_deserialize, reinterpret_cast<char const *>(input), len);

	switch (header) {
		case SNAKE_ARRAY: {
			std::memcpy(snake_array, data_deserialize, sizeof(Snake) * MAX_SNAKE);
			break;
		}
		case SNAKE: {
			Snake snake_temp;
			std::memcpy(&snake_temp, data_deserialize, sizeof(Snake));
			std::cout << snake_temp.id << std::endl;
			assert(snake_temp.id >= 0 && snake_temp.id < MAX_SNAKE);
			snake_array[snake_temp.id] = snake_temp;
			break;
		}
		case START_GAME:
			log_info("StartGame from server");
			start_game();
			return;
		case FOOD: {
			PositionComponent positionComponent;
			std::memcpy(&positionComponent, input, len);
			if (positionComponent.y != 0 && positionComponent.x != 0)
				async_write(ClientTCP::add_prefix(FOOD, &positionComponent));
			return;
		}
		case RESIZE_MAP: {
				std::memcpy(&mapSize, input, len);
				break ;
			}

		default:
			break;
	}
	//std::cout << "ServerTCP::parse_input.size() " << buffer.size() << std::endl;
	async_write(ClientTCP::add_prefix(header, data_deserialize));
	delete [] data_deserialize;
}

void ServerTCP::remove(TCPConnection::pointer remove) {
	std::cout << "Romve a Snake + Connection" << std::endl;

	pointers.erase(std::remove_if(pointers.begin(), pointers.end(),
								  [remove](TCPConnection::pointer pointer) {
									  return remove == pointer;
								  }));
}

/** ---------------------- TCPConnection ---------------------- **/

TCPConnection::TCPConnection(
		Snake const &snake,
		boost::asio::io_service &io_service,
		ServerTCP &serverTCP)
		: snake_(snake),
		  socket_(io_service),
		  serverTCP_(serverTCP) {
}

void TCPConnection::checkError_(boost::system::error_code const &error_code) {

	if ((boost::asio::error::eof == error_code) ||
		(boost::asio::error::connection_reset == error_code))
	{
		//Need to reset the Snake;
		std::cout << "Disconnected" << std::endl;
		serverTCP_.erase_snake(snake_);
		serverTCP_.remove(shared_from_this());
	}
}

void
TCPConnection::handle_read_data(eHeader header, boost::system::error_code const &error_code,
								size_t len) {
	//std::cout << "TCPConnection::handle_read_data [" << len << "]" << std::endl;
	checkError_(error_code);

	if (error_code.value() == 0 && len > 0) {
		serverTCP_.parse_input(header, buffer_data.data(), len);
	}
	read_socket_header();
}

void TCPConnection::handle_write(const boost::system::error_code &error_code,
								 size_t len) {
	checkError_(error_code);
}
void
TCPConnection::handle_read_header(const boost::system::error_code &error_code,
								  size_t len) {
	checkError_(error_code);
	if (error_code.value() == 0) {
		assert(len == sizeof(eHeader));
		eHeader header;
		std::memcpy(&header, buffer_data.data(), sizeof(eHeader));
		read_socket_data(header);
	}
}


void TCPConnection::read_socket_header() {
	//std::cout << "TCPConnection::read_socket_header" << std::endl;
	boost::asio::async_read(socket_,
							boost::asio::buffer(buffer_data, ClientTCP::size_header[HEADER]),
							boost::bind(&TCPConnection::handle_read_header,
										shared_from_this(),
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void TCPConnection::read_socket_data(eHeader header) {
	//std::cout << "TCPConnection::read_socket_data" << std::endl;
	boost::asio::async_read(socket_, boost::asio::buffer(buffer_data, ClientTCP::size_header[header]),
							boost::bind(&TCPConnection::handle_read_data,
										shared_from_this(),
										header,
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred));
}

void TCPConnection::write_socket(void const *data, size_t len) {

	boost::asio::async_write(socket_, boost::asio::buffer(data, len),
							 boost::bind(&TCPConnection::handle_write,
										 shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred));
}

void TCPConnection::write_socket(std::string message) {
	boost::asio::async_write(socket_, boost::asio::buffer(message),
							 boost::bind(&TCPConnection::handle_write,
										 shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred));
}


TCPConnection::pointer
TCPConnection::create(Snake const &snake, boost::asio::io_service &io_service,
					  ServerTCP &serverTCP) {
	return pointer(new TCPConnection(snake, io_service, serverTCP));
}


tcp::socket &TCPConnection::socket() {
	return socket_;
}
