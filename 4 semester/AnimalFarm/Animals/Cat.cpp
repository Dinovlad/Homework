#include <iostream>
#include "AnimalFactory.h"
#include "Animal.h"

class Cat : public Animal {
public:	

	int useless = AnimalFactory::getAnimalFactory().registerAnimal("cat", new AnimalCreator<Cat>());

	Cat(string _name) : Animal(_name) {}

	void voice() {
		cout << "I'm a cat. My name is " << name << "." << endl;
	}

};


