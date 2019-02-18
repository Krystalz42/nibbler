//
// Created by Noe TONIOLO on 2019-02-18.
//

#include "Test.hpp"
#include <iostream>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include <vector>


Test &Test::getInstance() {
	return (Test::m_instance);
}

void Test::sigHandler_(int signo) {
	if (signo == SIGUSR1) {
		std::cout << ": received SIGUSR1" << std::endl;
		Test::getInstance().next_ = true;
	} else
		exit(signo);
}

void Test::writeInput(std::string const &s) {
	if (input_)
		oAction_ << id_ << " " << s << std::endl;
}

void Test::update() {
	if (!test_ || !next_) {
		return ;
	}
	next_ = false;
	std::vector<std::string> token;

	boost::split(token, buffer_, boost::is_any_of(" "));

	assert(token.size() >= 2);

	if (stoi(token[INPUT_ID]) == id_) {
		std::cout << id_ << ": Is my action [" << token[INPUT_ACTION] << "]" << std::endl;
		callback_(ptr_, token[INPUT_ACTION]);
		std::cout << "Continue" << std::endl;
		usleep(INPUT_WAITING_TIME);
		kill(pidTestProcess_, SIGUSR1);
	}
	if (!std::getline(iAction_, buffer_)) {
		std::cout << id_ << "[END]" << std::endl;
		exit(EXIT_SUCCESS);
	}
}

void Test::setTest(bool b) {
	test_ = b;
}
void Test::setInput(bool b) {
	input_ = b;
}
void Test::setInputCallback(std::function<void(void *, std::string const &)> const &callback, void *ptr) {
	ptr_ = ptr;
	callback_ = callback;
}
void Test::setId(int id) {
	id_ = id;
}
void Test::setPidTestProcess(int pidTestProcess) {
	pidTestProcess_ = pidTestProcess;
}
void Test::setInputFile(std::string const &inputFile) {
	if (test_) {
		iAction_.open(inputFile);
		std::getline(iAction_, buffer_);
	}
	if (input_)
		oAction_.open(inputFile, std::ios_base::app);
}


Test::Test() :
test_(0),
input_(0),
id_(0),
pidTestProcess_(0),
next_(false),
ptr_(nullptr) {
	if (signal(SIGUSR1, Test::sigHandler_) == SIG_ERR)
		printf("\ncan't catch SIGUSR1\n");
}

Test::~Test() {
	iAction_.close();
	oAction_.close();
}

Test Test::m_instance=Test();