#include <iostream>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <string>

using namespace std;

template <typename Key>
class Hasher {
public:
	virtual int hashcode(const Key& key) = 0;
};

template <typename Key>
class PrimitiveHasher : public Hasher<Key> {
public:
	
	int hashcode(const Key& key) {
		return (int) key;
	}

};

template <typename Key, typename Value>
class Hashtable {

private:

        struct KeyValue {

        	Key key;

        	Value value;

        };

	Hasher<Key> *hasher;

        vector<KeyValue> *data;

        int capacity;

        int _size;

	int increment;

        int hash(const Key& key) {
        	return hasher->hashcode(key) % capacity;
        }

public:

        Hashtable(int capacity, int increment, Hasher<Key> *_hasher) {

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
	
		hasher = _hasher;

        }

	Hashtable(int capacity, int increment) : 
		Hashtable(capacity, increment, new PrimitiveHasher<Key>()) {}
	
	~Hashtable() {

		delete[] data;

		delete hasher;
		
	}

        void put(const Key& key, const Value& value) {
       	
		KeyValue item;

		item.key = key;
		item.value = value;

		//expand the hashtable if it is overfilled
		if ((increment != 0) && (capacity == _size)) {
			setCapacity(capacity + increment);
		}
	
		put(item);

	}

        void put(const KeyValue& item) {

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

		int oldCapacity = capacity;
		capacity = newCapacity;

		vector<KeyValue> *oldTable = data;
		data = new vector<KeyValue>[capacity];
		
		_size = 0;

		for(int i = 0; i < oldCapacity; i++) {
			for(auto &item : oldTable[i]) {
				put(item);
			}
		}

		delete[] oldTable;

	}

        Value get(const Key& key) {

        	for (auto &item : data[hash(key)]) {
        		if (item.key == key) {
        			return item.value;
        		}
		}

        	throw std::invalid_argument("No such key.");

        }

        Value remove(const Key& key) {

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

class StringHasher : public Hasher<string> {

	int hashcode(const string& s) {
		
		int code = 0;

		for (char c : s) {
			code += c;
		}

		return code;

	}

};

int main() {

	Hashtable<int, long> ht(20, 20);

	ht.put(1208, 2553243248756L);
	ht.put(108, 8234244346758756L);
	ht.put(34348, 7092313090156L);
	ht.put(134348, 291087240263489756L);
	ht.put(138, 8238947L);
	ht.put(308, 897878758756L);
	ht.put(898, 897878786758756L);
	ht.put(19908, 8978786758756L);
	ht.put(125, 89788756L);
	ht.put(1208, 89756L);
	ht.put(12348, 898786758756L);
	ht.put(1, 8787868756L);
	ht.put(228, 78786758756L);

	int k = 134348;

	cout << "Value at key " << k << ": " << ht.remove(k) << endl;
	cout << "Number of collisions : " << ht.collisions() << endl;	

	Hashtable<string, int> accounts(2, 20, new StringHasher());

	cout << "Something." << endl;

	accounts.put("Dneprov", 2345032);
	accounts.put("Silkin", 8687687);
	accounts.put("Kozhevnikov", 98896);
	accounts.put("Konyagin", 9999999);
	accounts.put("Vasilyev", 12007);
	accounts.put("Ulyanov", 98329938);

	string s = "Silkin";

	cout << "Account of " << s << ": " << accounts.get(s) << endl;
	cout << "Number of collisions : " << accounts.collisions() << endl;	

	return 0;

}
