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

void create_keys(const char* fname)
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

}/* namespace ipa */
