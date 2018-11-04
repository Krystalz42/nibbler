#pragma once


#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/array.hpp>
#include <Univers.hpp>
#include "ServerTCP.hpp"
#include "chat_message.hpp"

using boost::asio::ip::tcp;

class ClientTCP : public boost::enable_shared_from_this<ClientTCP> {
public:
	typedef boost::shared_ptr<ClientTCP> pointer_client;

	ClientTCP(Univers &univers, boost::asio::io_service &io,
			  std::string &hostname);

	void connect();

	void read_socket_header();

	void read_socket_chat();

	void read_socket_data(eHeader);

	void write_socket(std::string message);

	void write_socket(void const *data, size_t len);

	void handle_read_data(const boost::system::error_code &, size_t);

	void handle_read_header(const boost::system::error_code &, size_t);

	void handle_read_chat(const boost::system::error_code &, size_t);

	void handle_write(const boost::system::error_code &, size_t);

	void parse_input(eHeader header, void const *, size_t);

	void refreshMySnake(void);
	void change_name(char const *name);
	void change_sprite(eSnakeSprite snakeSprite);
	void change_state_ready(void);

	template<typename T>
	static void add_prefix(eHeader, std::string &, T *element, size_t bytes);

	static pointer_client
	create(Univers &univers, boost::asio::io_service &io, std::string hostname);


public:
	const Snake *getSnakes() const;

private:
	Snake snakes[MAX_SNAKE];
	tcp::socket socket;
	boost::asio::streambuf buffer_chat;
	boost::array<char, 512> buffer_data;

	int16_t id_;
	boost::asio::deadline_timer timer;
	tcp::resolver resolver;
	tcp::resolver::query query;
	Univers &univers;
};

template<typename T>
void ClientTCP::add_prefix(eHeader header, std::string &message, T *element,
						   size_t bytes) {

	message.append(reinterpret_cast<char *>(&header), sizeof(eHeader));
	message.append(reinterpret_cast<char *>(element), bytes);
	if (header == CHAT)
		message.append("\n", 1);
}
