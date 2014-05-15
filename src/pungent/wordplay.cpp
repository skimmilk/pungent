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
		int& sentence_pos, int& word_pos, int rand_seed, ipa::gstring& out)
{
	int pronunce_index = rand_seed % words[sentence_pos].size();

	int grab_amt = std::min(num_glyphs, (int)words[sentence_pos][pronunce_index].size() - word_pos);
	// Copy
	for (int i = 0; i < grab_amt; i++)
		out.push_back(
				words[sentence_pos]
				      [pronunce_index]
				       [word_pos++]);

	// If we ran out of glyphs
	if (grab_amt < num_glyphs && sentence_pos != (int)words.size() - 1)
		leak(num_glyphs - grab_amt, words, ++sentence_pos, word_pos = 0,
				rand_seed, out);
}

// Go through the list of words
// See if it fits well at the start of the sentence
// Continue on with the rest of the sentence
bool gen_pun(
		const std::vector<pronunciations_t>& sentence,
		int& sentence_pos,
		int& word_pos,
		const float& starting_diff,
		const float& max_diff,
		const float& delta_diff,
		int rand_seed,
		std::string& output
		)
{
	const auto& selected_word_pronuns = sentence[sentence_pos];
	const auto& selected_pronun =
			selected_word_pronuns[rand_seed % selected_word_pronuns.size()];

	if (sentence_pos + 1 >= (int)sentence.size() &&
			word_pos >= (int)selected_pronun.size())
		return true;

	const int max_tries = 20000;

	float current_diff = starting_diff;

	int tries = 0;

	// Reset positions to these if the random word does not match these
	int prev_spos = sentence_pos, prev_wpos = word_pos;
	const std::string old_output = output;

	// Keep trying to find a random word
	while (tries++ < max_tries && current_diff < max_diff)
	{
		// Insert a word and see if it fits into the sentence well
		const dict::dict_entry& random_word = dict::random_word();
		const ipa::gstring& random_pronun =
				random_word.ipa[rand() % random_word.ipa.size()];

		if (random_pronun.size() == 0)
			continue;

		// Get the next part of the sentence
		ipa::gstring sentence_part;
		leak(random_pronun.size(), sentence, sentence_pos, word_pos,
				rand_seed,sentence_part);

		// Determine if the random word fits well into the sentence
		if (ipa::glyphstring_diff(random_pronun, sentence_part) < max_diff)
		{
			output += random_word.word + " ";

			if (gen_pun(sentence, sentence_pos, word_pos,
					starting_diff, max_diff, delta_diff, rand(), output))
				return true;

			// Failed to generate a pun with these parameters
			// Lower required similarity levels
			output = old_output;
			current_diff += delta_diff;
		}

		sentence_pos = prev_spos;
		word_pos = prev_wpos;
	}
	// If we're just one glyph away from making a complete sentence
	//  just finish it here
	if (prev_spos + 1 >= (int)sentence.size() &&
			prev_wpos + 1 >= (int)selected_pronun.size())
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

bool get_pronunciations(std::string& sentence,
		std::vector<pronunciations_t>& pron)
{
	// Lower-case-ify string
	std::transform(sentence.begin(), sentence.end(), sentence.begin(),
			::tolower);

	// Get the pronunciations of all words in the sentence
	return get_glyph_string(sentence, pron);
}

bool play(std::string sentence,
		float diff_start, float diff_max, float delta_diff,
		fn_callback_t callback)
{
	std::vector<pronunciations_t> sentence_pronuns;
	if (!get_pronunciations(sentence, sentence_pronuns))
		return false;

	int i_sentence = 0, i_word = 0;
	std::string pun;

	while (true)
	{
		if (gen_pun(sentence_pronuns, i_sentence, i_word,
				diff_start, diff_max, delta_diff, rand(), pun))
		{
			if (!callback(pun))
				return true;
			i_sentence = i_word = 0;
			pun = "";
		}
	}

	return true;
}

void gen_pun_sequential(const ipa::gstring& sentence_pron,
		float delta_max, fn_callback_t callback,
		size_t glyph_pos = 1, std::string result = {0})
{
	if (glyph_pos >= sentence_pron.size())
	{
		callback(result);
		return;
	}

	for (const auto& entry : *dict::entries)
	{
		for (const auto& pronunciation : entry.ipa)
		{
			if (pronunciation.size() == 0)
				continue;
			size_t size = std::min(glyph_pos + pronunciation.size(),
					sentence_pron.size());
			if (ipa::glyphstring_diff(
					pronunciation,
					ipa::gstring (sentence_pron.begin() + glyph_pos,
							sentence_pron.begin() + size))
				< delta_max)
			{
				gen_pun_sequential(sentence_pron, delta_max, callback,
						glyph_pos + pronunciation.size(), result + entry.word + " ");
			}
		}
	}
}

// Go through every permutation of possible pronunciations of the sentence
// Pretty much a nested for loop
// for (a : prons[0]) for (b : prons[1]) ...
void permutate_pronunciations(const std::vector<pronunciations_t>& prons,
		const float delta_max, const fn_callback_t callback,
		int i_pron = 0, ipa::gstring sentence_pron = {0})
{
	if (i_pron == (int)prons.size())
		gen_pun_sequential(sentence_pron, delta_max, callback);

	const ipa::gstring old = sentence_pron;
	for (const ipa::gstring& pron : prons[i_pron])
	{
		sentence_pron.insert(sentence_pron.end(), pron.begin(), pron.end());
		permutate_pronunciations(prons, delta_max, callback, i_pron + 1,
				sentence_pron);
		sentence_pron = old;
	}
}
bool play_sequential(std::string sentence, float delta_max,
		fn_callback_t callback)
{
	std::vector<pronunciations_t> sentence_pronuns;
	if (!get_pronunciations(sentence, sentence_pronuns))
		return false;

	permutate_pronunciations(sentence_pronuns, delta_max, callback);
	return true;
}
bool init(const char* ipa_file, const char* words_file)
{
	if (!ipa::init_keys(ipa_file))
	{
		std::cerr << "Error loading ipa glyphs\n";
		return false;
	}
	ipa::init_diff();
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
	ipa::destroy_diff();
	dict::destroy_dict();
}
bool _internal_print_pun(const std::string& str)
{
	std::cout << str << "\n";
	return true;
}
} /* namespace wordplay */
