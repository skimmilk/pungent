/*
 * wordplay.h
 *
 *  Created on: May 2, 2014
 *      Author: skim
 */

#ifndef WORDPLAY_H_
#define WORDPLAY_H_

#include <iostream>
#include "dict_parse.h"

namespace wordplay{

bool init(const char* ipa_file, const char* words_file);
void destroy();

bool _internal_print_pun(const std::string& str);

typedef bool (*fn_callback_t)(const std::string&);

/**
 * Input:
 * A sentence to find puns
 * The maximum difference a pun should sound
 *   (0.0 == exactly same, 1.0 == completely different)
 * The callback function that is called when this finds something
 *   If the callback returns false, this function will stop and return true
 *
 * Returns:
 *   false if an error occurred getting the pronunciation guide for one of
 *     the words
 */
bool play_sequential(std::string sentence, float delta_max, bool permute,
		fn_callback_t callback = _internal_print_pun);

} /* namespace wordplay */

#endif /* WORDPLAY_H_ */
