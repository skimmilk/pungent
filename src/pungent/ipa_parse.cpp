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

#include "ipa_parse.h"

namespace ipa{

// Global root node
ipa_key* root;

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
	ret->dissimilarity = current->dissimilarity;
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
		else
		{
			current = family.substr(0, found);
			// Turn class:example:another into example:other
			family = family.substr(found + 1);

			key = get_family(key, current);
		}
	}
	return key;
}

void init_keys(const char* fname)
{
	root = new ipa_key();
	root->depth = 0;
	root->dissimilarity = 1.f;
	root->index_similar = false;

	ipa_key* current = 0;
	std::ifstream file (fname);
	std::string line;

	while (std::getline(file, line))
	{
		if (line.size() == 0 || line[0] == '#')
			continue;
		else if (line[0] == ':')
			current = get_full_family(root, line);
		else if (line[0] == '@')
		{
			line = line.substr(1);

			size_t found = line.find(' ');
			assert(found != line.npos);

			std::string classname (line.substr(0, found));
			float dissimilarity = atof(line.substr(found + 1).c_str());
			ipa_key* family = get_full_family(root, classname);
			family->dissimilarity = dissimilarity;
		}
		else if (line[0] == '*')
		{
			std::string classname = line.substr(1);
			get_full_family(root, classname)->index_similar = true;
		}
		else
			// Add character to the current ipa key
			current->characters.push_back(line);
	}
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
}

void add_glyph(std::vector<glyph_t>& vec, const glyph_t& toadd)
{
	if (std::find(vec.begin(), vec.end(), toadd) == vec.end())
	{
		auto siz = vec.size();
		bool inserted = false;

		for (size_t i = 0; i < siz; i++)
			if (vec[i].size() < toadd.size())
			{
				vec.insert(vec.begin() + i, toadd);
				inserted = true;
				break;
			}

		if (!inserted)
			vec.push_back(toadd);
	}
}
void add_key_glyphs(std::vector<glyph_t>& vec, ipa_key* key)
{
	for (auto a : key->children)
		add_key_glyphs(vec, a);

	for (const auto& a : key->characters)
		add_glyph(vec, a);
}

std::vector<glyph_t> sorted_keys()
{
	std::vector<glyph_t> ret;
	add_key_glyphs(ret, root);
	return ret;
}

}/* namespace ipa */
