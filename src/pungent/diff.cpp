/*
 * diff.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include "ipa_parse.h"

namespace ipa{

void glyph_classes(std::vector<ipa_key*>& classes, ipa_key* key, const glyph_t& glyph)
{
	for (ipa_key* child : key->children)
		glyph_classes(classes, child, glyph);

	for (const glyph_t& g : key->characters)
		if (g == glyph)
		{
			classes.push_back(key);
			return;
		}
}

// Returns a vector of ipa_keys that directly contain the glyph
std::vector<ipa_key*> glyph_classes(const glyph_t& glyph)
{
	std::vector<ipa_key*> classes;
	glyph_classes(classes, root, glyph);
	return classes;
}
bool class_contains(ipa_key* key, const glyph_t& glyph)
{
	for (const glyph_t& g : key->characters)
		if (g == glyph)
			return true;
	return false;
}
ipa_key* go_up(ipa_key* child, int upness)
{
	ipa_key* parent = child;
	while (upness--)
		if (parent)
			parent = parent->parent;
		else return nullptr;
	return parent;
}
bool tree_diff(ipa_key* t1, ipa_key* t2, float& result)
{
	// Find least-distant cousin
	// Between root:abc:123 and root:abc:456
	//  root:abc is the least-distant cousin

	// Get on the same depth
	while (t1->depth > t2->depth)
	{
		result += t1->class_dissimilarity;
		t1 = t1->parent;
	}
	while (t1->depth < t2->depth)
	{
		result += t2->class_dissimilarity;
		t2 = t2->parent;
	}

	while (t1->parent != nullptr)
	{
		if (t1 == t2)
		{
			result += t1->fam_dissimilarity;
			return true;
		}
		if (t1->parent == t2->parent)
		{
			result += t1->parent->class_dissimilarity;
			return true;
		}

		result += t1->class_dissimilarity + t2->class_dissimilarity;
		t1 = t1->parent;
		t2 = t2->parent;
	}
	return false;
}
float glyph_diff(const glyph_t& g1, const glyph_t& g2)
{
	if (g1 == g2)
		return 0.f;
	std::vector<ipa_key*>	c1 = glyph_classes(g1),
							c2 = glyph_classes(g2);

	float diff = 2.f;
	float tmp;
	// diff between backness:back and backness:back
	// diff between manner:latteral:flick and backness:back
	// diff between manner:latteral:flick and manner:latteral:approx
	// etc.

	for (ipa_key* key1 : c1)
		for (ipa_key* key2 : c2)
		{
			tmp = 0.f;
			if (tree_diff(key1, key2, tmp))
				diff = std::min(diff, tmp);
		}

	return diff;
}

} /* namespace ipa */
