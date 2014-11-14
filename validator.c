
#include "validator.h"
#include "state_container.h"
#include "tests.h"

#define TEST_AG 10
#define TEST_AM 11
#define TEST_UD 12

#define ROUNDLIMIT 50

int func_test(int type) {
	int value = 1, update = 1, changes = 1, rounds = 0;
	char a = ' ';
	uint8_t (*testfunc) (uint8_t value, uint8_t i, uint8_t j, int* changes);
	
	switch(type) {
		case TEST_AG:
			testfunc = test_attack_gain;
			a = 'V';
			printf("D | D Q\n");
			break;
		case TEST_AM:
			testfunc = test_asset_misuse_potential;
			a = 'V';
			printf("D | D L\n");
			break;
		case TEST_UD:
			testfunc = test_user_damage;
			a = 'V';
			printf("D | D C\n");
			break;
		default:
			printf("Invalid mode\n");
			break;
	}
	
	printf("%c | 1 | 2 | 3 | 4 | 5 | 6 |\n",a);
	printf("---------------------------\n");
	for(int i = 1; i < LIMIT; i++) {
		printf("%d |",i);
		for(int j = 1; j < LIMIT; j++) {
			value = 1;
			update = 1;
			rounds = 0;
			
			do {
				changes = 0;
				update = (*testfunc)(update, i, j, &changes);
				// No change
				if(changes > 0) value = update;
				rounds++;
			}while(changes > 0 && rounds < ROUNDLIMIT);
			
			printf(" %d%s|", value, rounds == ROUNDLIMIT ? "-" : " ");
		}
		printf("\n---------------------------\n");
	}
	return 0;
}

int main(int argc, char* argv[]) {

	char *input_state = NULL;
	char *input_file = NULL;
	int type = -1, optc = 0;
	
	extern char *optarg;
	extern int optopt;

	// Check command line options
	while ((optc = getopt(argc,argv,":chCf:v:t:")) != -1) {
		switch (optc) {
			case 'f':
				input_file = optarg;
				type = VALIDATE_FROM_FILE;
				break;
			// Validate inputted state
			case 'v':
				input_state = optarg;
				type = VALIDATE_SINGLE;
				break;
			case 'c':
				type = CREATE_STATES;
				break;
			case 'C':
				type = CREATE_AND_VALIDATE;
				break;
			case 't':
				type = FUNC_TEST_LOOP;
				input_state = optarg;
				break;
			case 'h':
				printf("Usage: %s -[chC] || -f PATH || -v STATE\n",argv[0]);
				return 0;
				break;
			case ':':
				printf("Parameter -%c is missing a operand\n", optopt);
				return 0;
				return -1;
			case '?':
				printf("Unknown parameter\n");
				return 0;
				break;
		}
	}
	state_container* state = NULL;
	int updates = 0;
	uint32_t repeating_one_value_updates = 0;
	
	initialize_functions();
	
	switch(type) {
		case VALIDATE_FROM_FILE:
			state = new_state_container_from_file(input_file);
			while((updates = update_state(state)) > 0) {
				//print_state(state,++round,updates);
				if(state->round_number == 90000) {
					printf("!!!A STUCK STATE WAS DETECTED, 90k ROUNDS!");
					break;
				}
			}
			print_state(state);
			break;
		case VALIDATE_SINGLE:
			if((state = new_state_container_from_initial(input_state)) == NULL) {
				printf("invalid state\n");
				break;
			}
			initialize_state(state);
			//print_state(state);

			while((updates = update_state(state)) > 0) {
				//print_state(state,++round,updates);
				if(state->round_number == 90000) {
					printf("!!!A STUCK STATE WAS DETECTED, 90k ROUNDS! (input: %s\n",input_state);
					print_state(state);
					print_state_history(state);
					break;
				}
				if(updates == 1) repeating_one_value_updates++;
				else repeating_one_value_updates = 0;
				
				if(repeating_one_value_updates > HISTORY) {
					if(check_state_history_repetition(state) == 1) {
						printf("History full of repetition, quitting at %u rounds (input: %s)\n",
							state->round_number,input_state);
						print_state(state);
						print_state_history(state);
						break;
					}
				}
			}
			print_state_history(state);
			print_state(state);
			printf("Risk = %hd\n",calc_privacy_risk(state->state));
			
			break;
		case CREATE_STATES:
			create_all_states();
			break;
		case CREATE_AND_VALIDATE:
			break;
		case FUNC_TEST_LOOP:
			func_test(atoi(input_state));
			break;
		default:
			break;
	}
	if(state != NULL) free(state);
	return 0;
}

