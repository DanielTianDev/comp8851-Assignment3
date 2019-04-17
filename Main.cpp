//Author: Daniel Tian
//Date: April 16, 2019
// comp 8851 - Assignment 3
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include "Hashing.h"
#include "Renderer.h"
#include "RoundRobinScheduler.h"

int main(void)
{
	printf("Question 6: Cuckoo hash table\n");

	int keys_1[] = { 20, 50, 53, 75, 100, 67, 105, 3, 36, 39 };
	HashTable cuckooHashTable;
	cuckooHashTable.Cuckoo(keys_1, sizeof(keys_1) / sizeof(int));
	cuckooHashTable.printTable();


	//this array has a cycle so we will test rehash
	int keys_2[] = { 10, 50, 53, 75, 100, 67, 105, 3, 36, 39, 6 };
	cuckooHashTable.Cuckoo(keys_2, sizeof(keys_2) / sizeof(int));
	cuckooHashTable.printTable();



	printf("Question 7: Round Robin\n\n\n");

	/*
		You	are	a	tournament	director	and	need	to	arrange	a	round	robin	tournament
		among	N =	2k players.	In	this	tournament,	everyone	plays	exactly	one	game	each	day;	after	N− 1
		days,	a	match	has	occurred	between	every	pair	of	players.	Give	a	recursive	algorithm	to	do	this.
		Implement	your	algorithm	in	C++	and	demonstrate	that	it	works.
	*/
	//I'm basing round robin of this diagram: https://upload.wikimedia.org/wikipedia/commons/b/b7/Round-robin_tournament_10teams_en.png

	std::vector<int> teams = {1, 2, 3, 4, 5, 6, 7, 8};
	RoundRobinScheduler(teams , teams.size(), day);




	printf("Question 8:\nNow rendering 300 squares with openGL, and calculating collisions with quadtree\n\n");

	GameLoop();


	return 0;
}

