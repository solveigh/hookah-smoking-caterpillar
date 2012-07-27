/*
 * Useful.h
 *
 *  Created on: Feb 20, 2012
 *      Author: matthies
 */

#ifndef USEFUL_H_
#define USEFUL_H_

#include <stdlib.h>
#include <stdio.h>

#include <sstream>

#include <iostream>
#include <string>
#include <vector>

#ifdef __WINDOWS__
#include <cstring>
#include <string.h>
#endif

using namespace std;

namespace qlib {

#ifdef WIN32
#define log(src,...) td::cout << __FUNCTION__ << "() at " << __FILE__ << ":" << __LINE__ << ": " << src << __VA_ARGS__ << std::endl;
#elif __linux__
//#define log(src,args...) std::cout << __FUNCTION__ << "() at " << __FILE__ << ":" << __LINE__ << ": " << src << ##args << std::endl;
//OS::slog(LOG_LOCAL0 | sev, "{%d} in %s() at %s:%d: " fmt , syscall(SYS_gettid), __FUNCTION__, __FILE__, __LINE__, ##args)
#endif

#define log(src, args...) std::cout << __FUNCTION__ << "() at " << __FILE__ << ":" << __LINE__ << ": " << src << std::endl;
//##args <<

// simple helper class
class PacketDescription {
public:
	PacketDescription(int priority, int size) {
		_priority = priority;
		_size = size;
	};

	int getPriority(){ return _priority; };
	int getSize(){ return _size; };

private:
	int _priority;
	int _size;
};

class Useful {

public:
	Useful();
	virtual ~Useful();

	static Useful *getInstance();

	double getTime(string name);
	int getPriority(string name);

	int generateRandomPriority();
	int generateRandomPriority(int lower, int upper);
	int generateRandomSize();

	std::vector<PacketDescription> readDataList(std::string filename);

	void writeRandomDataToList(std::string filename, int priority, int size);

	void appendToFile(std::string filename, std::string value);
	void appendToFileTab(std::string filename, std::string value);
	void appendToFile(std::string filename, double value);
	void appendToFile(std::string filename, int value);

	bool testFirstLineOfFile(std::string filename, std::string value);

    // helper function to create input data
    void createInputData(int number);

private:
	static Useful *_instance;

};

} /* namespace queueing */
#endif /* USEFUL_H_ */
