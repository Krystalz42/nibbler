#include <utility>
#include <iostream>
#include <random>
#include <fstream>
#include <gui/Gui.hpp>
#include <boost/program_options.hpp>
#include <ia/KStar.hpp>
#include "cores/Test.hpp"

void nibbler(Univers &univers) {
	while (!univers.isExit()) {
		if (univers.getGui_() == nullptr)
			univers.createGui();
		univers.getGui_()->mainLoop();

		if (univers.isOpenGame_()) {
			univers.startNewGame();
		}

	}
}

void option_dependency(boost::program_options::variables_map const &vm,
					   std::string const &for_what, std::string const &required_option) {
	if (vm.count(for_what) && !vm[for_what].defaulted())
		if (vm.count(required_option) == 0 || vm[required_option].defaulted())
			throw std::logic_error(std::string("Option '") + for_what
								   + "' requires option '" + required_option + "'.");
}

template<typename ... Args>
void option_dependency(boost::program_options::variables_map const &vm,
					   std::string const &for_what,
					   std::string const &required_option,
					   Args ... args) {
	option_dependency(vm, for_what, required_option);
	option_dependency(vm, for_what, args...);
}

int main(int argc, char **argv) {

	if (!NIBBLER_ROOT_PROJECT_PATH) {
		std::cerr << "NIBBLER_ROOT_PROJECT_PATH is not defined" << std::endl;
		return 0;
	}

	srand(time(NULL));
	try {
		Univers univers;

		boost::program_options::options_description desc("Options");
		desc.add_options()
				("help,h", "Print help messages")
				("fileInput", boost::program_options::value<std::string>(), "File to store input")
				("fileLog", boost::program_options::value<std::string>(), "File to store log")
				("id", boost::program_options::value<int>(), "Id of input")
				("pidTestProcess", boost::program_options::value<int>(), "Pid of shell tester process")
				("test,t", "Boolean for test mode")
				("input,i", "Boolean for input mode")
				("logger,l", boost::program_options::value<std::string>(), "Set file for outpout log")
				("sound,s", "enable the sound");

		boost::program_options::variables_map vm;
		try {
			boost::program_options::store(
					boost::program_options::parse_command_line(
							argc, argv, desc), vm);

			option_dependency(vm, "test", "id", "fileInput", "pidTestProcess", "fileLog");
			option_dependency(vm, "input", "id", "fileInput");

			if (vm.count("help")) {
				std::cout << "Basic Command Line Parameter App" << std::endl
						  << desc << std::endl;
				return (0);
			}
			if (vm.count("sound"))
				univers.loadSound();
			if (vm.count("test") && vm.count("id") && vm.count("fileInput") && vm.count("pidTestProcess") && vm.count("fileLog")) {
				Test::getInstance().setTest(true);
				Test::getInstance().setId(vm["id"].as<int>());
				Test::getInstance().setPidTestProcess(vm["pidTestProcess"].as<int>());
				Test::getInstance().setInputFile(vm["fileInput"].as<std::string>());
				Test::getInstance().setLogFile(vm["fileLog"].as<std::string>());
			}
			else if (vm.count("input") && vm.count("id") && vm.count("fileInput")) {
				Test::getInstance().setInput(true);
				Test::getInstance().setId(vm["id"].as<int>());
				Test::getInstance().setInputFile(vm["fileInput"].as<std::string>());
				if (vm.count("fileLog"))
					Test::getInstance().setLogFile(vm["fileLog"].as<std::string>());
				else
					Test::getInstance().setLogFile(vm["fileInput"].as<std::string>());
			}

			boost::program_options::notify(vm);
		}
		catch (const boost::program_options::error &e) {
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return (0);
		}
		nibbler(univers);
	}
	catch (const std::exception &e) {
		std::cerr << "Unhandled Exception reached the top of main: "
				  << e.what() << ", application will now exit" << std::endl;
	}
	return (0);
}