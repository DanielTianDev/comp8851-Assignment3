#pragma once



int day = 1;

void PrintVector(std::vector<int> vec) {

	for (int i = 0;i < vec.size();i++) std::cout << vec[i] << " ";
	std::cout << std::endl;
}

//recursive round robin function
void RoundRobinScheduler(std::vector<int> teams, int numberOfTeams, int day) {

	if (numberOfTeams % 2 != 0) {
		std::cout << numberOfTeams << " is not a valid number of teams; Number of teams has to be even.\n\n";
		return;
	}


	int halfOfTeamSize = numberOfTeams / 2;



	std::vector<int> firstHalf(halfOfTeamSize);
	std::vector<int> secondHalf(halfOfTeamSize);

	int counter = 0;

	//Divide team into two even groups
	for (int i = 0;i < halfOfTeamSize;i++) firstHalf[counter++] = teams[i];
	counter = 0;
	for (int i = halfOfTeamSize;i < numberOfTeams;i++) secondHalf[counter++] = teams[i];

	//have current teams play each other.



	for (int i = 0;i < halfOfTeamSize;i++) {

		//play
		printf("Day %d:\n", day++);
		PrintVector(firstHalf);
		PrintVector(secondHalf);


		//now shuffle the second team (keep first team in same order)
		int temp = secondHalf[0]; //save first element
		for (int i = 0;i < halfOfTeamSize - 1;i++) secondHalf[i] = secondHalf[i + 1];
		secondHalf[halfOfTeamSize - 1] = temp;

	}

	printf("\n");

	if (halfOfTeamSize <= 1) {
		printf("Tournament took %d days\n", day);
		return;
	}

	RoundRobinScheduler(firstHalf, firstHalf.size(), day);
	RoundRobinScheduler(secondHalf, secondHalf.size(), day);

}