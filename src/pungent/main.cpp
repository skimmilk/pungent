/*
 * main.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <iostream>
#include "ipa_parse.h"

int main()
{
	ipa::init_keys("res/ipa_dict");

	for (const auto& a : ipa::sorted_keys())
		std::cout << a << " : " << a.size() << "\n";

	ipa::destroy_keys();
}

