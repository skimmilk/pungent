/*
 * extract.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <string>
#include <fstream>
#include <iostream>

// Extracts the plaintext Wikpedia dump from stdin
int main()
{
	std::string line;
	std::string word, ipa, section;
	bool ipa_read = false;
	bool noun = false, verb = false, adj = false;
	bool english = false;
	bool has_english = false;

	while (std::getline(std::cin, line))
	{
		if (line.find("==") == 0)
			ipa_read = false;

		// Check title
		//    <title>synonym</title>
		if (line.find("    <title>") == 0)
		{
			std::string current_word;
			current_word = line.substr(11);
			current_word = current_word.substr(0, current_word.find('<'));

			// If the word is valid
			if (has_english && word.find(':') == word.npos && word.find('|') == word.npos &&
					word.find(' ') == word.npos && word.find('-') == word.npos)
			{
				// Print it out
				std::cout << "==BEGIN==\n";
				std::cout << word << "\n";
				std::cout << ipa << "\n";

				if (noun) std::cout << "n";
				if (verb) std::cout << "v";
				if (adj) std::cout << "a";
				std::cout << "\n";
			}
			// Reset variables
			noun = verb = adj = has_english = false;
			english = true;
			section = ipa = "";

			word = current_word;
		}
		else if (line.find("==") != line.npos)
		{
			std::string uneq;
			for (const auto& c : line.substr(line.find("==") + 2))
				if (c != '=')
					uneq += c;

			size_t found = uneq.find("==");
			if (found != uneq.npos)
				uneq = uneq.substr(0, uneq.find("=="));

			section = uneq;

			if (section == "English")
				has_english = english = true;
			else if (section == "Pronunciation")
				ipa_read = true;
		}
		else if (line == "----")
			english = false;
		else if (ipa_read)
		{
			// Grab the IPA string
			//* {{IPA|/ˈsɪnənɪm/}}
			//* {{audio|en-us-synonym.ogg|Audio (US)|lang=en}}
			while (line.find("}}") != line.npos)
			{
				size_t found = line.find("{{");
				if (found == line.npos)
					break;
				std::string ipatext = line.substr(found + 2);

				ipatext = ipatext.substr(0, ipatext.find("}}"));
				if (ipatext.find("IPA") != line.npos)
				{
					found = ipatext.find('/');
					if (found == line.npos)
						break;
					ipatext=ipatext.substr(found + 1);
					ipatext=ipatext.substr(0, ipatext.find('/'));
					if (ipatext.length())
						ipa += ipatext + "|";
				}
				found = line.find("}}");
				if (found == line.npos)
					break;
				line = line.substr(found + 2);
			}
		}
		else if (english)
		{
			if (section == "Noun" || section == "Proper noun" || section == "Pronoun")
				noun = true;
			else if (section == "Adjective")
				adj = true;
			else if (section == "Verb")
				verb = true;
		}
	}
	return 0;
}
