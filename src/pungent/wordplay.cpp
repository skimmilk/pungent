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

// Remember these puns so they don't have to be re-computed
//  in the recursive method
struct pun_stat
{
	const std::string& word;
	int glyph_length;
};
std::vector<std::vector<pun_stat>>* generated_puns;
void save_pun(size_t position, const std::string& pun, int glyph_length)
{
	generated_puns->at(position).push_back({pun, glyph_length});
}
bool has_puns(size_t position)
{
	if (position >= generated_puns->size())
	{
		generated_puns->resize(position + 1);
		return false;
	}
	return generated_puns->at(position).size() > 0;
}
bool print_puns(size_t position, fn_callback_t callback, std::string base)
{
	if (position >= generated_puns->size())
		return callback(base);

	for (const auto& a : generated_puns->at(position))
		if (!print_puns(position + a.glyph_length, callback,
				base + a.word + " "))
			return false;
	return true;
}

// Return false to stop processing
// Return true to keep processing
bool gen_pun_sequential(const ipa::gstring& sentence_pron,
		float delta_max, fn_callback_t callback,
		size_t glyph_pos = 0, std::string result = std::string ())
{
	// Print puns that won't be caught by print_puns
	if (glyph_pos >= sentence_pron.size())
		return callback(result);
	if (has_puns(glyph_pos))
		return print_puns(glyph_pos, callback, result);

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
					ipa::gstring (
							&sentence_pron[glyph_pos], &sentence_pron[size]))
				< delta_max)
			{
				save_pun(glyph_pos, entry.word, pronunciation.size());

				if (!gen_pun_sequential(sentence_pron, delta_max, callback,
						glyph_pos + pronunciation.size(),
						result + entry.word + " "))
					return false;
			}
		}
	}
	return true;
}

// Go through every permutation of possible pronunciations of the sentence
// Pretty much a nested for loop
// for (a : prons[0]) for (b : prons[1]) ...
bool permutate_pronunciations(const std::vector<pronunciations_t>& prons,
		const float delta_max, const fn_callback_t callback,
		bool permute, int i_pron = 0,
		ipa::gstring sentence_pron = ipa::gstring())
{
	if (i_pron == (int)prons.size())
	{
		generated_puns = new std::vector<std::vector<pun_stat>>();
		auto ret = gen_pun_sequential(sentence_pron, delta_max, callback);
		delete generated_puns;

		if (permute)
			return ret;
		return false;
	}

	const ipa::gstring old = sentence_pron;
	for (const ipa::gstring& pron : prons[i_pron])
	{
		sentence_pron.insert(sentence_pron.end(), pron.begin(), pron.end());
		if (!permutate_pronunciations(prons, delta_max, callback, permute,
				i_pron + 1, sentence_pron))
			return false;
		sentence_pron = old;
	}
	return true;
}
bool play_sequential(std::string sentence, float delta_max, bool permute,
		fn_callback_t callback)
{
	std::vector<pronunciations_t> sentence_pronuns;
	if (!get_pronunciations(sentence, sentence_pronuns))
		return false;

	permutate_pronunciations(sentence_pronuns, delta_max, callback, permute);
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
