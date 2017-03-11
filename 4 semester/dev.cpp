#include <iostream>
#include <string>
#include <vector>
#include <map>

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

	virtual Animal* produce(string _name) {}

};

class Cat : public Animal {
public:

	Cat(string _name) : Animal(_name) {}

	void voice() {
		cout << "I'm a cat. My name is " << name << "." << endl;
	}

	Animal* produce(string _name) {
		return new Cat(_name);
	}

};

class Dog : public Animal{
public:

	Dog(string _name) : Animal(_name) {}

	void voice() {
		cout << "I'm a dog. My name is " << name << "." << endl;
	}

	Animal* produce(string _name) {
		return new Dog(_name);
	}

};

class AbstractAnimalFactory {

	virtual Animal* create(string _name) = 0;
	
};

template <class Type>
class AnimalFactory {

	Animal* create(string _name) {
		return new Type(_name);
	}

};

int main() {

	vector<Animal*> animals;

	map<string, Animal*> table = {{"cat", new Cat("X")}, {"dog", new Dog("X")}};

	string buf;

	while(cin >> buf) {
	
		if (buf == "print") {

			for (auto &an : animals) {
				an->voice();
			}
			
		} else if (buf == "terminate") {
				
			return 0;

		} else {

			string nm;

			cin >> nm;

			Animal *a = table[buf]->produce(nm);

			animals.push_back(a);

		}
			

	}

	return 0;

}
