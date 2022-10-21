#include <Arduino.h>

#ifndef Splitter_h
#define Splitter_h


class Splitter {
public:

	Splitter();
	String split(String data, char separator, int index);

private:

};

#endif
