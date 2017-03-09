#include <iostream>
#include <stdlib.h>
#include <vector>
#include <stdexcept>

using namespace std;

template <typename Key, typename Value>
class Hashtable {

private:

        struct KeyValue {

        	Key key;

        	Value value;

        };

        vector<KeyValue> *data;

        int capacity;

        int _size;

	int increment;

        int hash(Key key) {
        	return ((int) key) % capacity;
        }

public:

        Hashtable(int capacity, int increment) {

		if (capacity <= 0) {
			throw std::invalid_argument("Non-positive capacity.");
		}

		if (increment < 0) {
			throw std::invalid_argument("Negative increment.");
		}


        	this->capacity = capacity;
		this->increment = increment;

        	data = new vector<KeyValue>[capacity];

        	_size = 0;

        }

        void add(Key key, Value value) {
        	
		KeyValue item;

		item.key = key;
		item.value = value;

		//expand the hashtable if it is overfilled
		if ((increment != 0) && (capacity == _size)) {
			setCapacity(capacity + increment);
		}
	
		add(item);

	}

        void add(KeyValue item) {

		vector<KeyValue> &v = data[hash(item.key)];

		for (auto it = v.begin(); it < v.end(); it++) {
        		if (it->key == item.key) {

        			v.insert(it, item);

				_size++;

				return;

        		}
		}

        	v.push_back(item);

        	_size++;

        }

	void setCapacity(int newCapacity) {

		capacity = newCapacity;

		vector<KeyValue> *oldTable = data;
		data = new vector<KeyValue>[capacity];
		
		_size = 0;

		for(int i = 0; i < capacity; i++) {

			for(auto &item : oldTable[i]) {
				add(item);
			}

			delete &oldTable[i];

		}

	}

        Value getValue(Key key) {

        	for (auto &item : data[hash(key)]) {
        		if (item.key == key) {
        			return item.value;
        		}
		}

        	throw std::invalid_argument("No such key.");

        }

        Value remove(Key key) {

        	vector<KeyValue> &v = data[hash(key)];

        	for(auto it = v.begin(); it < v.end(); it++) {
        		if (it->key == key) {

        			Value val = it->value;

        			v.erase(it);

        			_size--;

        			return val;

        		}
        	}

        	throw std::invalid_argument("No such key.");

        }

        int size() {
        	return _size;
        }

	int collisions() {
		
		int sum = 0;

		for(int i = 0; i < capacity; i++) {

			int l = data[i].size();

			if (l == 0) {
				continue;
			}

			sum += l - 1;

		}
	
		return sum;

	}

};

int main() {

	Hashtable<int, long> ht(20, 20);

	ht.add(1208, 2553243248756L);
	ht.add(108, 8234244346758756L);
	ht.add(34348, 7092313090156L);
	ht.add(134348, 291087240263489756L);
	ht.add(138, 8238947L);
	ht.add(308, 897878758756L);
	ht.add(898, 897878786758756L);
	ht.add(19908, 8978786758756L);
	ht.add(125, 89788756L);
	ht.add(1208, 89756L);
	ht.add(12348, 898786758756L);
	ht.add(1, 8787868756L);
	ht.add(228, 78786758756L);

	int k = 134348;

	cout << "Value at key " << k << ": " << ht.remove(k) << endl;
	cout << "Number of collisions : " << ht.collisions() << endl;	

	return 0;

}
