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

	std::string a = ipa::glyph_strip("....a");
	while (std::getline(std::cin, a))
	{
		std::cout << a.size() << "\n";
		for (const auto& res : ipa::glyph_str(ipa::glyph_strip(a)))
			std::cout << res << "\n";
	}

	ipa::destroy_keys();
}

