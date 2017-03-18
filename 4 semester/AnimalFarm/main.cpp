#include <iostream>
#include "AnimalFactory.h"

using namespace std;

int main() {

	vector<Animal*> animals;

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

			Animal *a = AnimalFactory::getAnimalFactory().produce(&buf, &nm);

			animals.push_back(a);

		}
			

	}

	return 0;

}
