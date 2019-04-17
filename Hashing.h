#pragma once


#define DEFAULT_SIZE 11 

#define EMPTY_VALUE -9876543

class HashTable {

private:
	int HASH_TABLE[2][DEFAULT_SIZE];	//single 2d array to store 2 tables
	int pos[2]; // Array to store possible positions for a key 


	int  ModulusHash(int key) {
		return key % DEFAULT_SIZE;
	}

	int  ModulusHash2(int key) {
		return (key / DEFAULT_SIZE) % DEFAULT_SIZE;
	}

public:

	HashTable() {}

	void FillTable() //Fills hashtable with the EMPTY_VALUE constant
	{ 
		for (int j = 0; j < DEFAULT_SIZE; j++)
			for (int i = 0; i < 2; i++)
				HASH_TABLE[i][j] = EMPTY_VALUE;
	}



	//hashes based on our 2 simple hash functions. Key being item to be hashed
	int HashKey(int function, int key)
	{
		switch (function)
		{
			case 1: 
				return ModulusHash(key);//return key % DEFAULT_SIZE;		//hash function 1
			case 2: 
				return ModulusHash2(key);//return (key / DEFAULT_SIZE) % DEFAULT_SIZE;	//hash function 2
		}
	}

	//Places key in one of possible positions within the 2 hash tables. 
	//TableID is the table where the key will be inserted
	//callCounter = number of times this insert method has been called
	//maxCount = max number of times this can be called before we conclude it's an infinite loop
	void Insert(int callCounter, int maxCount, int key, int tableID)
	{

		//if cycle/infinite loop is declared, rehash
		if (callCounter == maxCount)
		{
			std::cout << key << " has no position." << std::endl;
			std::cout << "Cycle has been detected, rehashing" << std::endl;
			return;
		}

		//check if there are still possible positions for the key
		for (int i = 0; i < 2; i++)
		{
			pos[i] = HashKey(i + 1, key);

			if (HASH_TABLE[i][pos[i]] == key) return;
		}

		//Ensure that if another key already occupies the current position for the new key,
		//then place the new key in its position, then move the pre-existing key into al alternate position for it in the next table.
		if (HASH_TABLE[tableID][pos[tableID]] != INT_MIN)
		{
			int dis = HASH_TABLE[tableID][pos[tableID]];
			HASH_TABLE[tableID][pos[tableID]] = key;
			Insert(callCounter + 1, maxCount, dis, (tableID + 1) % 2);
		}
		else {
			//if no key occupies, then place the key in its position.
			HASH_TABLE[tableID][pos[tableID]] = key;
		}
	}

	void printTable()
	{
		std::cout << "Printing hash table:" << std::endl;

		/*
		for (int i = 0; i < 2; i++, printf("\n"))
			for (int j = 0; j < DEFAULT_SIZE; j++)
				(HASH_TABLE[i][j] == EMPTY_VALUE) ? printf("- ") :
				printf("%d ", HASH_TABLE[i][j]);
		*/

		for (int i = 0;i < 2; i++) {
			for (int j = 0; j < DEFAULT_SIZE; j++) {



				(HASH_TABLE[i][j] == EMPTY_VALUE) ? printf("- ") :
					printf("%d ", HASH_TABLE[i][j]);
			}
			printf("\n");
		}

		std::cout << std::endl;
	}

	//function for cuckoo hashing keys. keys = array of int keys, n = size of keys array
	void Cuckoo(int keys[], int n)
	{
		// initialize hash tables
		FillTable();

		// start with placing every key at its position in 
		// the first hash table according to first hash 
		// function 
		for (int i = 0, cnt = 0; i < n; i++, cnt = 0) Insert(cnt, n, keys[i], 0);
	}
};

