#ifndef D
#define D

#include <iostream>
#include <string>

using namespace std;

class Animal {
public:

	string name;

	Animal(string _name) {
		name = _name;
	}

	virtual void voice() {
		cout << "I'm an animal. My name is " << name << "." << endl;
	}
	
};

#endif
