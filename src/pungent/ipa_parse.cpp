/*
 * ipa_parce.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

#include "ipa_parse.h"

namespace ipa{

// Global root node
ipa_key* root;
std::vector<glyph_char_t>* sorted_glyph_chars;
std::vector<glyph_t>* sorted_glyphs;

// Will create family if not in current
ipa_key* get_family(ipa_key* current, const std::string& family)
{
	for (ipa_key* p : current->children)
	{
		if (p->key == family)
			return p;
	}
	ipa_key* ret = new ipa_key();
	ret->depth = current->depth + 1;
	ret->key = family;
	ret->parent = current;
	ret->class_dissimilarity = current->class_dissimilarity;
	ret->fam_dissimilarity = current->fam_dissimilarity;
	ret->index_similar = current->index_similar;
	current->children.push_back(ret);
	return ret;
}
/**
 * Input: string
 *        ":class:something:else"
 */
ipa_key* get_full_family(ipa_key* rootnode, std::string family)
{
	if (family[0] == ':')
		family = family.substr(1);

	std::string current;
	size_t found;
	ipa_key* key = rootnode;

	while (true)
	{
		found = family.find(':');

		if (found == family.npos)
		{
			key = get_family(key, family);
			break;
		}
		current = family.substr(0, found);
		// Turn class:example:another into example:other
		family = family.substr(found + 1);

		key = get_family(key, current);
	}
	return key;
}
glyph_t glyph_char_hash(const glyph_char_t& a)
{
	glyph_t result = 0;
	for (const auto& c : a)
	{
		result *= 1099511628211;
		result ^= (glyph_t)c;
	}
	return result;
}

// Function will generate the lookup tables
void sort_keys();
bool init_keys(const char* fname)
{
	root = new ipa_key();
	root->depth = 0;
	root->class_dissimilarity = 1.f;
	root->fam_dissimilarity = 1.f;
	root->index_similar = false;

	ipa_key* current = 0;
	std::ifstream file (fname);
	std::string line;

	if (file.bad())
		return false;
	while (std::getline(file, line))
	{
		if (line.size() == 0 || line[0] == '#')
			continue;
		// Reading :class:name
		else if (line[0] == ':')
			current = get_full_family(root, line);
		// Reading @class:name:thing 1.0 0.1
		else if (line[0] == '@')
		{
			std::string tmp = line.substr(1);

			// Space delimits class name and numbers
			size_t found = tmp.find(' ');
			if (found == line.npos)
				throw std::runtime_error("Invalid line: " + line);

			std::string classname (tmp.substr(0, found));
			tmp = tmp.substr(found + 1);

			found = tmp.find(' ');
			if (found == line.npos)
				throw std::runtime_error("Invalid line: " + line);

			// String should contain two floats
			float class_dissimilar = atof(tmp.substr(0, found).c_str());
			float fam_dissimilar = atof(tmp.substr(found + 1).c_str());

			ipa_key* family = get_full_family(root, classname);
			family->class_dissimilarity = class_dissimilar;
			family->fam_dissimilarity = fam_dissimilar;
		}
		// Reading *class:name
		else if (line[0] == '*')
		{
			std::string classname = line.substr(1);
			get_full_family(root, classname)->index_similar = true;
		}
		else
		{
			// Add character to the current ipa key
			current->characters.push_back(glyph_char_hash(line));
			current->characters_str.push_back(line);
		}
	}
	root->class_dissimilarity = 1.f;
	sort_keys();
	return true;
}

void destroy_key(ipa_key* key)
{
	for (auto k : key->children)
		destroy_key(k);
	delete key;
}

void destroy_keys()
{
	destroy_key(root);
	root = nullptr;
	delete ipa::sorted_glyph_chars;
	delete ipa::sorted_glyphs;
}

std::string glyph_strip(const std::string& glyphs)
{
	// Strip stresses and unusable characters
	std::vector<glyph_char_t> unusable {"-", "̩", "ˈ","ː","ˑ",".","(",")","'","ˌ"};
	std::string tmp = glyphs;
	std::string result;
	while (true)
	{
		bool switched = false;
		for (const glyph_char_t& a : unusable)
		{
			if (!tmp.size())
				return result;
			if (tmp.compare(0, a.size(), a) == 0)
			{
				switched = true;
				tmp = tmp.substr(a.size());
			}
		}

		// Advance
		if (!switched)
		{
			result += tmp[0];
			tmp = tmp.substr(1);
		}
	}
	return result;
}
// Puts the next recognizable glyph from the string into glyph
// Glyphs is the array of recognizable glyphs sorted by size
// Failed is set if there is unrecognizable glyphs in the string
// Returns true if there is more parsable glyphs in the string
bool glyph_next(std::string& str, glyph_t& glyph, bool& failed)
{
	if (str.size() == 0)
		return false;

	int i = 0;
	for (const glyph_char_t& a : *sorted_glyph_chars)
	{
		if (str.compare(0, a.size(), a) == 0)
		{
			glyph = sorted_glyphs->at(i);
			str = str.substr(a.size());
			return true;
		}
		i++;
	}
	failed = true;
	return false;
}
gstring glyph_str(std::string str)
{
	gstring result;
	glyph_t glyph;
	bool failed = false;
	str = glyph_strip(str);

	while (glyph_next(str, glyph, failed))
		result.push_back(glyph);

	if (failed)
		throw std::runtime_error("Unknown glyphs in: " + str);

	return result;
}

// Returns false if string contains unrecognizable glyphs
// Keys need to be created with sorted_keys()
// str is the string to look up
bool glyph_try_str(std::string str, gstring& result)
{
	bool failed = false;
	glyph_t glyph;

	str = glyph_strip(str);

	while (glyph_next(str, glyph, failed))
		result.push_back(glyph);

	return !failed;
}

// Insertion sort based on the size of glyph character
void add_glyph(
		std::vector<glyph_char_t>& gchars,
		std::vector<glyph_t>& glyphs,
		const glyph_char_t& gchar_toadd,
		const glyph_t& glyph_toadd)
{
	if (std::find(glyphs.begin(), glyphs.end(), glyph_toadd) == glyphs.end())
	{
		auto siz = glyphs.size();
		bool inserted = false;

		for (size_t i = 0; i < siz; i++)
			if (gchars[i].size() < gchar_toadd.size())
			{
				gchars.insert(gchars.begin() + i, gchar_toadd);
				glyphs.insert(glyphs.begin() + i, glyph_toadd);
				inserted = true;
				break;
			}

		if (!inserted)
		{
			gchars.push_back(gchar_toadd);
			glyphs.push_back(glyph_toadd);
		}
	}
}
void add_key_glyphs(
		std::vector<glyph_char_t>& gchars,
		std::vector<glyph_t>& glyphs,
		ipa_key* key)
{
	for (auto a : key->children)
		add_key_glyphs(gchars, glyphs, a);

	int i = 0;
	for (const auto& a : key->characters_str)
	{
		add_glyph(gchars, glyphs, a, key->characters[i]);
		i++;
	}
}

void sort_keys()
{
	sorted_glyph_chars = new std::vector<glyph_char_t>();
	::ipa::sorted_glyphs = new std::vector<glyph_t>();
	add_key_glyphs(*sorted_glyph_chars, *sorted_glyphs, root);
}

}/* namespace ipa */
