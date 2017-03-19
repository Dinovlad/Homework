#include <iostream>
#include "AnimalFactory.h"
#include "Animal.h"

class Dog : public Animal {
public:	

	Dog(string _name) : Animal(_name) {}

	void voice() {
		cout << "I'm a dog. My name is " << name << "." << endl;
	}

};

int dogInit = AnimalFactory::getAnimalFactory().registerAnimal("dog", new AnimalCreator<Dog>());
