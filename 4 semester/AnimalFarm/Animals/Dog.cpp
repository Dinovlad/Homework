#include <iostream>
#include "AnimalFactory.h"
#include "Animal.h"

class Dog : public Animal {
public:	

	int useless = AnimalFactory::getAnimalFactory().registerAnimal("dog", new AnimalCreator<Dog>());

	Dog(string _name) : Animal(_name) {}

	void voice() {
		cout << "I'm a dog. My name is " << name << "." << endl;
	}

};
