#include <iostream>
#include "AnimalFactory.h"

int main() {

	vector<Animal*> animals;

	string buf;

	while(cin >> buf) {
	
		if ((buf == "print") || (buf == "pr")) {

			for (auto &an : animals) {
				an->voice();
			}
			
		} else if ((buf == "terminate") || (buf == "exit")) {
				
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
