/*
 * main.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: skim
 */

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <argp.h>
#include "ipa_parse.h"
#include "diff.h"
#include "dict_parse.h"
#include "wordplay.h"

struct pungent_state
{
	char* sentence;
	int pun_num;
	int verbose;
	int seed;
	float max, starting, delta;
};

struct pungent_state* punstate;

void run_pun(pungent_state*);
static error_t parse_opt(int key, char* arg, struct argp_state* state);

static char doc[] =
  "pungent - generate sentences that sound alike";
static char args_doc[] = "SENTENCE";
static struct argp_option options[] = {
		{"verbose",		'v', 0, 0, "Produce verbose output", 0},
		{"num-puns",	'n', "NUMBER", 0,
				"Produce N puns, -1 for never-ending", 0},
		{"rand-seed",	's', "NUMBER", 0,
				"Random seed to use in generation", 0},
		{"diff-max",	'm', "NUMBER", 0,
				"Maximum allowable difference (Default 0.16)", 0},
		{"diff-min",	'd', "NUMBER", 0,
				"Starting allowable difference (Default 0.06)", 0},
		{"diff-change",	'c', "NUMBER", 0,
				"Amount to change allowable sentence difference"
				" on failure to find pun (Default 0.05)", 0},
		// The things I do to silence compiler warnings...
		{ 0, 0, 0, 0, 0, 0}
};
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0};
int main(int argc, char** argv)
{
	pungent_state args;
	punstate = &args;
	memset(&args, 0, sizeof(pungent_state));
	args.max = 0.16;
	args.starting = 0.06;
	args.delta = 0.05;

	argp_parse(&argp, argc, argv, 0, 0, &args);
	run_pun(&args);
	return 0;
}

bool print_pun(const std::string& str)
{
	static int prints_done = 0;
	std::cout << str << "\n";

	if (punstate->pun_num != 0 && ++prints_done == punstate->pun_num)
		return false;

	return true;
}
void run_pun(pungent_state* state)
{
	if (state->verbose)
		std::cerr << "Loading dictionary...\n";
	if (!wordplay::init("res/ipa_dict", "res/wordlist2"))
	{
		std::cerr << "Error loading files\n";
		return;
	}
	if (state->verbose)
		std::cerr << "Done\n\n";

	int seed = state->seed ? state->seed : time(NULL);
	srand(seed);

	wordplay::play(state->sentence,
			state->starting, state->max, state->delta, print_pun);

	if (state->verbose)
		std::cerr << "Done\n\n";

	wordplay::destroy();
}
error_t parse_opt (int key, char* arg, struct argp_state* state)
{
	pungent_state* punstate = (pungent_state*)state->input;
	switch (key)
	{
	case 'n':
		punstate->pun_num = atoi(arg);
		break;
	case 'v':
		punstate->verbose = 1;
		break;
	case 's':
		punstate->seed = atoi(arg);
		break;
	case 'm':
		punstate->max = atof(arg);
		break;
	case 'd':
		punstate->starting = atof(arg);
		break;
	case 'c':
		punstate->delta = atof(arg);
		break;
	case ARGP_KEY_ARG:
		if (state->arg_num > 1)
			argp_usage(state);
		punstate->sentence = arg;
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 1)
			argp_usage(state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
