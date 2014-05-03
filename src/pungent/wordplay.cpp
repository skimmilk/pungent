/*
 * wordplay.cpp
 *
 *  Created on: May 2, 2014
 *      Author: skim
 */

#include <algorithm>

#include "wordplay.h"

namespace wordplay{

// Turn sentence into a vector of glyph strings
// Since each word can have multiple pronunciations,
//  each word corresponds to a vector of glyph strings
bool get_glyph_string(const std::string& sentence,
		std::vector<std::vector<ipa::gstring>>& glyph_str)
{
	std::string tmp = sentence;
	while (1)
	{
		auto i_space = tmp.find(' ');
		std::string word;

		if (i_space == tmp.npos)
			// No space found
			word = tmp;
		else
			// Space found
			word = tmp.substr(0, i_space);

		// Get pronunciations for current word
		std::vector<ipa::gstring> pronunciations;
		if (!dict::search(word, pronunciations) || !pronunciations.size())
		{
			std::cerr << "Could not find pronunciation for " << tmp << "\n";
			// Could not find pronunciations
			return false;
		}

		glyph_str.push_back(pronunciations);

		tmp = tmp.substr(i_space + 1);
		if (i_space == tmp.npos)
			// Done parsing
			return true;
	}
	return true;
}
bool play(std::string sentence, float diff_max,
		fn_callback_t callback)
{
	std::transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);

	std::vector< std::vector<ipa::gstring> > glyph_str;
	if (!get_glyph_string(sentence, glyph_str))
		return false;

	for (const auto& word_pronuns : glyph_str)
	{
		for (const auto& pronunciation : word_pronuns)
		{
			for (const auto& glyph : pronunciation)
				std::cout << glyph;
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	return true;
}
bool init(const char* ipa_file, const char* words_file)
{
	if (!ipa::init_keys(ipa_file))
	{
		std::cerr << "Error loading ipa glyphs\n";
		return false;
	}
	if (!dict::init_dict(words_file))
	{
		std::cerr << "Error loading wordlist\n";
		return false;
	}
	return true;
}
void destroy()
{
	ipa::destroy_keys();
	dict::destroy_dict();
}
bool _internal_print_pun(const std::string& str)
{
	std::cout << str << "\n";
	return true;
}
} /* namespace wordplay */
