/*
 * ipa_parse.h
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#ifndef IPA_PARSE_H_
#define IPA_PARSE_H_

#include <string>
#include <vector>

namespace ipa{

typedef std::string glyph_t;
typedef std::vector<glyph_t> gstring;

struct ipa_key
{
	std::string key;
	int depth;
	ipa_key* parent;
	std::vector<ipa_key*> children;
	float fam_dissimilarity;
	float class_dissimilarity;
	std::vector<glyph_t> characters;
	// The difference in index in children vector implies their similarity
	bool index_similar;
};
extern ipa_key* root;

// Initializes the IPA list, loading file fname
bool init_keys(const char* fname);
void destroy_keys();

// Returns a vector of glyphs sorted by size
std::vector<glyph_t> sorted_keys();

// Strips useless valid IPA glyphs from the provided string
std::string glyph_strip(const std::string&);
// Produces a glyph string from a standard string with IPA characters
gstring glyph_str(std::string);

// Returns false if string contains unrecognizable glyphs
// Keys need to be created with sorted_keys()
// str is the string to look up
bool glyph_try_str(std::vector<glyph_t> keys, std::string str, gstring& result);

}/* namespace ipa */

#endif /* IPA_PARSE_H_ */
