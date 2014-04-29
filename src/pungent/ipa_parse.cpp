/*
 * ipa_parce.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <vector>
#include <string>
#include <cassert>

namespace ipa
{

struct ipa_key
{
	std::string key;
	int depth;
	ipa_key* parent;
	std::vector<ipa_key*> children;
	int count_children;
	float dissimilarity;
	std::vector<std::string> characters;
};

ipa_key* root;

/**
 * Input: string
 *        ":class:something:else"
 */
ipa_key* get_family(const ipa_key* current, const std::string& family)
{

	for (auto p : current->children)
	{

	}
}
ipa_key* get_full_family(const ipa_key* rootnode, std::string family)
{
	assert(family[0] == ":");
	family = family.substr(1);

	std::string current;
	size_t found;
	while ((found = family.find(':')) != family.npos)
	{
		current = family.substr(0, found);
		family = family.substr(found + 1);
	}
}

void add_character(ipa_key* key, const std::string& character)
{
}

void create_keys(const char* fname)
{
	root = new ipa_key();
	root->depth = 0;
}
}/* namespace ipa */
