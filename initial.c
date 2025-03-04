
#include <glob.h>
#include <debug.h>
#include <stdlib.h>
#include <time.h>
#include <hashbrd.h>
#include <position.h>
#include <initial.h>
#include <string.h>

void initialize_variables()
    // This function will initialize all of the variables, pointers, and
    // conditions needed for the program to play chess.
    {
        char    *code;          // Holds the coded board position.
        time_t   current_time;  // Current time.
        char     temp_code[70]; // Temporary holder for Forsyth code.


        glob__current_position.setup_board();

        glob__current_move      = 1;
        glob__draw_count        = 0;
        glob__max_depth[White]  = 20;
        glob__max_depth[Black]  = 20;
        glob__new_board         = True;
        glob__print_pos         = True;
        glob__use_library       = True;
        glob__computer_color    = Black;
        glob__computer_against_computer = False;
        glob__human_against_human = False;

        // Delete positions from a previous game.

        while(glob__position_count) {
            delete glob__past_positions[glob__position_count--];
        }

        // Place original position on list.

        glob__current_position.to_forsyth(temp_code);
        code = new char[strlen(temp_code)+1];
        strcpy(code, temp_code);
        glob__past_positions[++glob__position_count] = code;

        time(&current_time);
        srand(current_time);

        glob__total_time[White]   = 0;
        glob__total_time[Black]   = 0;
        glob__moves_per_tc[White] = 30;
        glob__moves_per_tc[Black] = 30;
        glob__time_per_tc[White]  = 30 * 10;
        glob__time_per_tc[Black]  = 30 * 10;
        glob__moves_left_in_tc[White]  = 30;
        glob__moves_left_in_tc[Black]  = 30;
        glob__time_left_in_tc[White]   = 30 * 10;
        glob__time_left_in_tc[Black]   = 30 * 10;

        glob__use_fischer_timing = False;
    }

