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

// Initialize
// Must be called only after the IPA dictionary has been loaded
void init_diff();
void destroy_diff();

// Returns the difference between two glyphs
// Ranges from 1 to 0
float glyph_diff(const glyph_t&, const glyph_t&);
// Returns how different a string of glyph sounds
float glyphstring_diff(const gstring&, const gstring&);

} /* namespace ipa */

#endif /* DIFF_H_ */
