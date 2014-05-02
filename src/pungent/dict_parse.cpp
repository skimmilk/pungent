/*
 * wik_parse.cpp
 *
 *  Created on: May 1, 2014
 *      Author: skim
 */

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "dict_parse.h"
#include "ipa_parse.h"

namespace dict{

struct dict_entry
{
	std::string word;
	std::vector<ipa::gstring> ipa;
};

// Global
std::vector<dict_entry>* entries;

bool search(const std::string& word, std::vector<ipa::gstring>& pronunciation)
{
	for (const auto& a : *entries)
		if (word == a.word)
		{
			pronunciation = a.ipa;
			return true;
		}
	return false;
}
void add_ipa(const std::vector<ipa::glyph_t>& glyphs,
		dict_entry& current, std::string line, bool log)
{
	ipa::gstring result;

	while (true)
	{
		result.clear();
		size_t found = line.find('|');
		if (found == line.npos)
		{
			if (ipa::glyph_try_str(glyphs, line, result))
			{
				if (result.size())
					current.ipa.push_back(result);
			}
			else if (log)
				std::cerr << line << " contains invalid characters\n";
			break;
		}


		if (ipa::glyph_try_str(glyphs, line.substr(0, found), result))
			current.ipa.push_back(result);
		else if (log)
			std::cerr << line.substr(0, found) <<
					" contains invalid characters\n";

		line = line.substr(found + 1);
	}
}
bool init_dict(const char* fname, bool log)
{
	std::string line;
	std::ifstream file (fname);

	auto glyphs = ipa::sorted_keys();

	if (file.bad())
		return false;

	entries = new std::vector<dict_entry>();

	// File's format:
	// word
	// IPA
	// type
	// word...
	while (true)
	{
		// Word
		if (!std::getline(file, line))
			break;

		dict_entry current;
		current.word = line;

		// IPA pronunciation
		if (!std::getline(file, line))
			throw std::runtime_error("Unexpected EOF");

		add_ipa(glyphs, current, line, log);

		// Type of word
		if (!std::getline(file, line))
			throw std::runtime_error("Unexpected EOF");

		if (current.ipa.size())
			entries->push_back(current);
	}
	return true;
}
void destroy_dict()
{
	delete entries;
}

} /* namespace wik */
