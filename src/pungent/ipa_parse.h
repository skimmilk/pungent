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

// A glyph_t is a unique identifier for a glyph
typedef uint64_t glyph_t;
typedef std::string glyph_char_t;
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
	std::vector<std::string> characters_str;
	// The difference in index in children vector implies their similarity
	bool index_similar;
};
extern ipa_key* root;

// Initializes the IPA list, loading file fname
bool init_keys(const char* fname);
void destroy_keys();

// Gets the hash value for a glyph character
glyph_t glyph_char_hash(const glyph_char_t&);

// Strips useless valid IPA glyphs from the provided string
std::string glyph_strip(const std::string&);
// Produces a glyph string from a standard string with IPA characters
gstring glyph_str(std::string);

// Returns false if string contains unrecognizable glyphs
// str is the string to translate
bool glyph_try_str(std::string str, gstring& result);

}/* namespace ipa */

#endif /* IPA_PARSE_H_ */
