#ifndef H
#define H

#include <vector>
#include <map>
#include <string>
#include "Animals/Animal.h"

using namespace std;

class AbstractAnimalCreator {
	public:
	virtual Animal* create(string _name) = 0;
};

template <class Type>
class AnimalCreator : public AbstractAnimalCreator {
	public:
	Animal* create(string _name) {
		return new Type(_name);
	}
};

class AnimalFactory {

	private:

	map<string, AbstractAnimalCreator*> reg;

	AnimalFactory() {}
	AnimalFactory(const AnimalFactory&);

	public:

	static AnimalFactory& getAnimalFactory() {
		static AnimalFactory af;		
		return af;
	}

	int registerAnimal(string id, AbstractAnimalCreator *creator) {
		reg[id] = creator;
		return 0;
	}

	Animal* produce(string *id, string *name) {
		return reg[*id]->create(*name);
	}

};

#endif
