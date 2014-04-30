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

struct ipa_key
{
	std::string key;
	int depth;
	ipa_key* parent;
	std::vector<ipa_key*> children;
	float dissimilarity;
	std::vector<std::string> characters;
	// The difference in index in children vector implies their similarity
	bool index_similar;
};

extern ipa_key* root;
void create_keys(const char* fname);

}

#endif /* IPA_PARSE_H_ */
