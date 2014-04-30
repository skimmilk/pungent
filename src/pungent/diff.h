/*
 * diff.h
 *
 *  Created on: Apr 30, 2014
 *      Author: skim
 */

#ifndef DIFF_H_
#define DIFF_H_

#include "ipa_parse.h"

namespace ipa {

// Returns the difference between two glyphs
// Ranges from 1 to 0
float glyph_diff(const glyph_t&, const glyph_t&);

} /* namespace ipa */

#endif /* DIFF_H_ */
