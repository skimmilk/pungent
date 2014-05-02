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

void test_glyphs();
void test_dict();

int main(int argc, char**)
{
	// Just put an argument into the command line to test dictionary
	if (argc == 1)
		test_glyphs();
	else
		test_dict();
	return 0;
}

void test_dict()
{
	ipa::init_keys("res/ipa_dict");
	std::cout << "Loading dictionary...\n";
	dict::init_dict("res/wordlist2");
	std::cout << "Done\n\n";

	std::string line;
	while (std::getline(std::cin, line))
	{
		std::vector<ipa::gstring> prns;
		if (dict::search(line, prns))
			for (const auto& glyphstr : prns)
			{
				for (const auto& glyph : glyphstr)
					std::cout << glyph;
				std::cout << "\n";
			}
		else
			std::cout << "(not found)\n";

		std::cout << "\n";
	}

	dict::destroy_dict();
	ipa::destroy_keys();
}
void test_glyphs()
{
	ipa::init_keys("res/ipa_dict");

	auto sorted = ipa::sorted_keys();

	for (const auto& a : sorted)
		std::cout << a << " : " << a.size() << "\n";

	std::string a = ipa::glyph_strip("....a");

	ipa::glyph_diff("r", "θ");

	std::cout << ipa::glyphstring_diff(ipa::glyph_str("ənsəɪkləpidiə"), ipa::glyph_str("ɪnsaɪkləpidiə")) << "\n";
	std::cout << ipa::glyphstring_diff(ipa::glyph_str("θɜzdeɪ"), ipa::glyph_str("tjuzdeɪ")) << "\n";
	std::cout << ipa::glyphstring_diff(ipa::glyph_str("daɪəlɛkt"), ipa::glyph_str("dijalɛkt")) << "\n";
	std::cout << ipa::glyphstring_diff(ipa::glyph_str("abaktoɾ"), ipa::glyph_str("æbəkəs")) << "\n";
	std::cout << ipa::glyphstring_diff(ipa::glyph_str("ɡɹɑtɪs"), ipa::glyph_str("fɹi")) << "\n";

	srand(time(0));
	for (int i = 0; i < 20; i++)
	{
		auto rand1 = sorted[rand() % sorted.size()], rand2 = sorted[rand() % sorted.size()];
		std::cout << "Test diff: " <<  rand1 << " & " << rand2 << " = " <<
				ipa::glyph_diff(rand1,rand2) << "\n";
	}
	while (std::getline(std::cin, a))
	{
		auto pos = a.find('|');
		if (pos == a.npos)
		{
			std::cout << a.size() << "\n";
			auto gstr = ipa::glyph_str(a);

			for (const auto& res : gstr)
				std::cout << res << "    ";
			std::cout << "\n";

			int size = a.size();
			for (int i = 0; i < size - 1; i++)
			{
				auto c1 = gstr[i], c2 = gstr[i+1];
				std::cout << "Diff " << c1 << " + " << c2 << " = " <<
						ipa::glyph_diff(c1, c2) << "\n";
			}
		}
		else
		{
			// Test difference
			std::string string1 = ipa::glyph_strip(a.substr(pos + 1)),
					string2 = ipa::glyph_strip(a.substr(0, pos));

			ipa::gstring gs1 = ipa::glyph_str(string1),
					gs2 = ipa::glyph_str(string2);

			std::cout << "String difference: " << ipa::glyphstring_diff(gs1, gs2) << "\n";
		}
	}

	ipa::destroy_keys();
}

