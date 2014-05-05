/*
 * diff.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <math.h>
#include <algorithm>
#include "diff.h"

namespace ipa{

struct glyph_differences
{
	glyph_t glyph;
	std::vector<glyph_t> other_glyphs;
	std::vector<float> differences;
};

std::vector<glyph_differences>* difference_table;
void populate_diff_table();

void init_diff()
{
	difference_table = new std::vector<glyph_differences>();
	populate_diff_table();
}
void destroy_diff()
{
	delete difference_table;
}

// http://en.wikipedia.org/wiki/Levenshtein_distance
float glyphstring_diff(const gstring& a, const gstring& b)
{
	if (a == b)
		return 0.f;
	if (!a.size() || !b.size())
		return 1.f;

	std::vector<float> prev_dist, cur_dist;
	cur_dist.resize(b.size() + 1);
	prev_dist.resize(b.size() + 1);

	// Init the previous row of distances
	int index = 0;
	for (auto& f : prev_dist) f = index++;

	int asize = a.size(), bsize = b.size();
	for (int i_a = 0; i_a < asize; i_a++)
	{
		// Calculate current row distance from previous row prev_dist
		cur_dist[0] = i_a + 1;

		for (int i_b = 0; i_b < bsize; i_b++)
		{
			float cost = glyph_diff(a[i_a], b[i_b]);
			cur_dist[i_b + 1] = std::min(
					std::min(cur_dist[i_b] + 1.f, prev_dist[i_b + 1] + 1.f),
					prev_dist[i_b] + cost);
		}

		// Copy current distance row into previous row
		prev_dist = cur_dist;
	}

	return cur_dist[b.size()] / std::min(b.size(), a.size());
}

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
int indexof(ipa_key* child)
{
	int result = 0;

	for (auto c : child->parent->children)
		if (c == child)
			return result;
		else
			result++;

	throw;
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
			if (t1->index_similar)
				result += t1->parent->fam_dissimilarity *
					(1. + (float)abs(indexof(t1) - indexof(t2)));
			else
				result += t1->parent->class_dissimilarity;
			return true;
		}

		result += t1->class_dissimilarity + t2->class_dissimilarity;
		t1 = t1->parent;
		t2 = t2->parent;
	}
	return false;
}
float internal_glyph_diff(const glyph_t& g1, const glyph_t& g2)
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

void add_unique_glyphs(std::vector<glyph_t>& glyphs, ipa_key* key)
{
	for (const auto& g : key->characters)
		if (std::find(glyphs.begin(), glyphs.end(), g) == glyphs.end())
			glyphs.push_back(g);

	for (auto child : key->children)
		add_unique_glyphs(glyphs, child);
}
std::vector<glyph_t> unique_glyphs()
{
	std::vector<glyph_t> result;
	add_unique_glyphs(result, ipa::root);
	return result;
}

void populate_diff_table()
{
	auto glyphs = unique_glyphs();

	// Go over every possible combination of two glyphs
	for (auto thisglyph : glyphs)
	{
		glyph_differences diff;
		diff.glyph = thisglyph;

		for (auto other_glyph : glyphs)
		{
			diff.other_glyphs.push_back(other_glyph);
			diff.differences.push_back(internal_glyph_diff(thisglyph, other_glyph));
		}
		difference_table->push_back(diff);
	}
}

float glyph_diff(const glyph_t& g1, const glyph_t& g2)
{
	// Consult the lookup table for their differences
	int i_g1 = 0;
	for (const auto& a : *difference_table)
		if (a.glyph == g1)
			break;
		else
			i_g1++;

	const auto& diffs = difference_table->at(i_g1);

	int i_g2 = 0;
	for (const auto& a : diffs.other_glyphs)
		if (a == g2)
			break;
		else
			i_g2++;

	return diffs.differences[i_g2];
}

} /* namespace ipa */
