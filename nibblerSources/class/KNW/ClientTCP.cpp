#include "ClientTCP.hpp"


namespace KNW {

	ClientTCP::ClientTCP(IOManager &io_manager)
			:
			dataTCP_(DataTCP::create()),
			io_manager_(io_manager) {

	}

	ClientTCP::b_sptr ClientTCP::create(IOManager &io_manager,std::function<void()> c) {
		auto ptr = boost::shared_ptr<ClientTCP>(new ClientTCP(io_manager));
		ptr->callbackDeadConnection_ = std::move(c);
		return ptr;
	}

	void ClientTCP::connect(const std::string host, const std::string port) {
		boost::asio::ip::tcp::resolver resolver(io_manager_.getIo());
		boost::shared_ptr<boost::asio::ip::tcp::socket> socket_(new boost::asio::ip::tcp::socket(io_manager_.getIo()));

		boost::asio::ip::tcp::resolver::query query(
				host,
				boost::lexical_cast<std::string>(port));

		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(
				query);
		ClientTCP::b_wptr wptr(shared_from_this());
		socket_->connect(*iterator);
		iotcp = IOTCP::create(dataTCP_, socket_, callbackDeadConnection_);
	}

	bool ClientTCP::isConnect() const {
		return iotcp != nullptr && iotcp->isConnect();
	}

	DataTCP &ClientTCP::getDataTCP_() {
		return *dataTCP_;
	}


	void ClientTCP::disconnect() {
		if (iotcp && iotcp->getSocket_() && iotcp->getSocket_()->is_open()) {
			iotcp->writeSyncSocket(dataTCP_->serializeData(0, 0));
		}
		iotcp = nullptr;
	}

	ClientTCP::~ClientTCP() {
		disconnect();
	}

}























