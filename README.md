# Pungent
##### The shitty pun generator

## What it does
The program tries to do wordplay with sentences and output sentences that sound alike.

Example output ran with `./pungent "we conjure the spirits of the computer with our spells" --diff-max 0.01 | shuf | head`
```
we conjure the 's peer it's of the computer with ou are 's pels 
whee conjure the spear it 's of thee computer with ow are spell 's 
whee conjure thee 's peer it's of the computer with hour 's pels 
wi conjure the spear it 's of the computer with ow are spell 's 
we conjure thee spear its of the computer with hour 's pels 
wee conjure the 's peer it's of the computer wi thou are spell 's 
whee conjure the spear it 's of the computer wi thou are 's pels 
wee conjure the 's peer it 's of the computer with our spell 's 
wee conjure the spear it's of the computer with hour 's pels 
wi conjure thee 's peer it 's of the computer with hour spell 's 
```
## How it does it
The program understands how words are pronounced based on the International Phonetic Alphabet (IPA) spelling of all words Wiktionary has provided.
Each letter in the IPA resembles a distinct sound a person can make, so a word spelled with the IPA tells you exactly how to pronounce it and how it sounds.

For example, the word 'technology', with an American accent, is spelled 'tɛknɑlədʒi' with the IPA, according to Wiktionary.

Pungent uses a chart of all IPA glyphs (`res/ipa_dict`) and checks the chart to determine how different two glyphs sound.
Pungent checks how similar words and sentences sound with above, if the IPA glyphs that make up a word are similar sounding, the word or sentence are pronounced and sound similar.

To generate wordplay based on a given input sentence, the program looks up its IPA spellings, then incrementally adds random words that sound similar to the input, till the sentence is fully built and you get a shitty pun.
#### License
Everything in src/ is GPLv3

Everything in res/ is licensed under GNU Free Documentation License v1.3

Check COPYING for explanations




