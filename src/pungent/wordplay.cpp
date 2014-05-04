/*
 * wordplay.cpp
 *
 *  Created on: May 2, 2014
 *      Author: skim
 */

#include <algorithm>

#include "wordplay.h"
#include "diff.h"

namespace wordplay{

// Word pronunciations are a vector of glyph strings
// It is getting hard to keep track of all these std::vectors,
//   so typedef'ing them away makes the code much easier to read
typedef std::vector<ipa::gstring> pronunciations_t;

// Grab some of the next glyphs in the sentence
// Randomly select pronunciations if there are multiple ones
void leak(int num_glyphs, const std::vector<pronunciations_t>& words,
		int& sentence_pos, int& word_pos, ipa::gstring& out)
{
	int pronunce_index = rand() % words[sentence_pos].size();

	int grab_amt = std::min(num_glyphs, (int)words[sentence_pos][pronunce_index].size() - word_pos);
	// Copy
	for (int i = 0; i < grab_amt; i++)
		out.push_back(
				words[sentence_pos]
				      [pronunce_index]
				       [word_pos++]);

	// Grab more glyphs from the next word if we need to
	if (grab_amt < num_glyphs && sentence_pos != (int)words.size() - 1)
	{
		word_pos = 0;
		leak(num_glyphs - grab_amt, words, ++sentence_pos, word_pos, out);
	}
}

// Go through the list of words
// See if it fits well at the start of the sentence
// Continue on with the rest of the sentence
bool gen_pun(const std::vector<pronunciations_t>& sentence, int& sentence_pos,
		int& word_pos, const float& max_diff, std::string& output)
{
	if (sentence_pos >= (int)sentence.size())
		return true;
	const int max_tries = 10000;
	int i = 0;

	int prev_spos = sentence_pos;
	int prev_wpos = word_pos;
	while (i++ < max_tries)
	{
		// Insert a word and see if it fits into the sentence well
		const dict::dict_entry& random_word = dict::random_word();
		const ipa::gstring& random_pronun =
				random_word.ipa[rand() % random_word.ipa.size()];

		if (random_pronun.size() == 0)
			continue;

		ipa::gstring sentence_part;
		leak(random_pronun.size(), sentence, sentence_pos, word_pos, sentence_part);

		if (ipa::glyphstring_diff(random_pronun, sentence_part) < max_diff)
		{
			output += random_word.word + " ";
			return gen_pun(sentence, sentence_pos, word_pos, max_diff, output);
		}
		sentence_pos = prev_spos;
		word_pos = prev_wpos;
	}
	if (sentence_pos + 1 >= (int)sentence.size() &&
			word_pos >= (int)sentence[sentence_pos].size())
		// close enough
		return true;
	return false;
}

// Turn sentence into a vector of glyph strings
// Since each word can have multiple pronunciations,
//  each word corresponds to a vector of glyph strings
bool get_glyph_string(const std::string& sentence,
		std::vector<pronunciations_t>& glyph_str)
{
	std::string tmp = sentence;
	while (1)
	{
		auto i_space = tmp.find(' ');
		std::string word;

		if (i_space == tmp.npos)
			// No space found
			word = tmp;
		else
			// Space found
			word = tmp.substr(0, i_space);

		// Get pronunciations for current word
		pronunciations_t pronunciations;
		if (!dict::search(word, pronunciations) || !pronunciations.size())
		{
			std::cerr << "Could not find pronunciation for " << word << "\n";
			// Could not find pronunciations
			return false;
		}

		glyph_str.push_back(pronunciations);

		tmp = tmp.substr(i_space + 1);
		if (i_space == tmp.npos)
			// Done parsing
			return true;
	}
	return true;
}
void explain_sentence(const std::vector<pronunciations_t>& sentence_pronuns)
{
	for (const auto& word_pronuns : sentence_pronuns)
	{
		for (const auto& pronunciation : word_pronuns)
		{
			for (const auto& glyph : pronunciation)
				std::cout << glyph;
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	ipa::gstring a;
	int c = 0, d = 0;
	leak(50, sentence_pronuns, c, d, a);
	for (const auto& glyphs : a)
		std::cout << glyphs;
	std::cout << "\n\n";
}
bool play(std::string sentence, float diff_max,
		fn_callback_t callback, bool do_test)
{
	// Lower-case-ify string
	std::transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);

	// Get the pronunciations of all words in the sentence
	std::vector<pronunciations_t> sentence_pronuns;
	if (!get_glyph_string(sentence, sentence_pronuns))
		return false;

	if (do_test)
		explain_sentence(sentence_pronuns);

	int i_sentence = 0, i_word = 0;
	int retries = 0;
	std::string pun;

	while (retries++ < 15)
	{
		if (gen_pun(sentence_pronuns, i_sentence, i_word, diff_max, pun))
		{
			if (!callback(pun))
				return true;
			i_sentence = i_word = 0;
			pun = "";
			retries = 0;
		}
		else
		{
			retries++;
			diff_max += 0.01;
		}
	}

	return true;
}
bool init(const char* ipa_file, const char* words_file)
{
	if (!ipa::init_keys(ipa_file))
	{
		std::cerr << "Error loading ipa glyphs\n";
		return false;
	}
	if (!dict::init_dict(words_file))
	{
		std::cerr << "Error loading wordlist\n";
		return false;
	}
	return true;
}
void destroy()
{
	ipa::destroy_keys();
	dict::destroy_dict();
}
bool _internal_print_pun(const std::string& str)
{
	std::cout << str << "\n";
	return true;
}
} /* namespace wordplay */
