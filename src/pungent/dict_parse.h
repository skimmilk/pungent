/*
 * wik_parse.h
 *
 *  Created on: May 1, 2014
 *      Author: skim
 */

#ifndef WIK_PARSE_H_
#define WIK_PARSE_H_

#include "ipa_parse.h"

namespace dict{

// Initialize the dictionary
bool init_dict(const char* fname, bool logerr = false);
void destroy_dict();

// Search the dictionary for string, put found IPA pronunciations into vector
// Returns true if the word has pronunciation guide
bool search(const std::string&, std::vector<ipa::gstring>&);

} /* namespace wik */

#endif /* WIK_PARSE_H_ */
