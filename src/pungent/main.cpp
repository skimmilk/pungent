/*
 * main.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <iostream>
#include <cstdlib>
#include "ipa_parse.h"
#include "diff.h"
#include "dict_parse.h"
#include "wordplay.h"

void test_pun();

int main(int, char**)
{
	test_pun();
	return 0;
}

static int prints_done = 0;
bool print_pun(const std::string& str)
{
	std::cout << str << "\n";
	if (++prints_done == 10)
	{
		prints_done = 0;
		return false;
	}
	return true;
}
void test_pun()
{
	std::cerr << "Loading dictionary...\n";
	if (!wordplay::init("res/ipa_dict", "res/wordlist2"))
	{
		std::cerr << "Error loading files\n";
		return;
	}
	std::cerr << "Done\n\n";

	srand(time(0));

	std::cout << "Testing pun...\n";
	wordplay::play("technology", 0.06, print_pun);
	std::cout << "Done\n\n";

	prints_done = 0;

	std::string line;
	while (std::getline(std::cin, line))
	{
		std::cerr << "Playing with words...\n";
		if (!wordplay::play(line, 0.06, print_pun))
			std::cout << "Couldn't find a pronunciation for one of the words\n";
		std::cerr << "Done\n\n";
		prints_done = 0;
	}

	wordplay::destroy();
}
