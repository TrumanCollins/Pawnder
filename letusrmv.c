
#include <glob.h>
#include <ctype.h>
#include <docomand.h>
#include <hashbrd.h>
#include <io.h>
#include <position.h>
#include <time.h>
#include <string.h>
#include <updmvlst.h>
#include <utility.h>
#include <letusrmv.h>

void let_user_move(
        Position_type  *posit,  // The current position.
        int            *done    // Set to true if end of game.
    )
    // This routine will let the user input a move.  It checks
    // for legality of the move.
    {
        int             dest_char_index; // Index to dest char in input array.
        int             dest_dig_index;  // Index to dest char in input array.
        square          enemy;           // Holds mask for enemy pieces.
        square          friendly;        // Holds mask for friendly pieces.
        bd_index        from_loc;        // The location the move is from.
        Gen_move_type  *move;            // The move made.
        int             good_input;      // Set to true when input is good.
        int             i;               // Loop variable.
        char            input[50];       // Place input is put.
        int             is_command;      // Set to true if this was a command.
        int             taken;           // Set to true if piece was taken.
        bd_index        to_loc;          // The location the move is to.
        int             value;           // Used to add to time control.


        set_piece_sets(posit->mover, &friendly, &enemy);
        glob__depth_reached = 0;
        *done = False;
        time(&glob__start_time);

        do {

            // Read input and process a command until move made.

            do {
                write_message_to_request_move();
                cin >> input;

                if(!strcmp(input, "quit")) {
                    *done = True;
                }

                if(glob__use_fischer_timing && (input[0] == '+' || input[0] == '-')) {
                    value = 0;
                    for(i = 1; input[i]; i++) {
                        if(!isdigit(input[i])) {
                            cerr << "Non-digit input.  No change made to time control.\n";
                            break;
                        }
                        value = value * 10 + input[i] - '0';
                    }
                    if(!input[i]) {
                        if(input[0] == '+') {
                            glob__time_left_in_tc[other_color(posit->mover)] += value;
                        } else {
                            glob__time_left_in_tc[other_color(posit->mover)] -= value;
                        }
                        cout << "    New computer time left is: " << glob__time_left_in_tc[other_color(posit->mover)] << "\n";
                    }
                    is_command = True;
                    continue;
                }

                if(input[0] == 'X' || input[0] == 'x') {
                    is_command = True;
                    *done = do_command(posit);
                    return;
                } else is_command = False;

            } while(!*done && is_command);

            if(!*done) {

                // Here we have a move to look at.
                // Make sure it is legal.

                good_input = True;
                if(strlen(input) < 4 || strlen(input) > 5) good_input = False;

                // Allow either e2-e4 or e2e4.

                if((input[2] >= 'A' && input[2] <= 'H') || (input[2] >= 'a' && input[2] <= 'h')) {
                    dest_char_index = 2;
                    dest_dig_index  = 3;
                } else {
                    dest_char_index = 3;
                    dest_dig_index  = 4;
                }

                if(input[0] >= 'A' && input[0] <= 'H') input[0] = input[0] + ('a' - 'A');
                if(input[dest_char_index] >= 'A' && input[dest_char_index] <= 'H') input[dest_char_index] = input[dest_char_index] + ('a' - 'A');
                if(input[0] < 'a' || input[0] > 'h') good_input = False;
                if(input[1] < '1' || input[1] > '8') good_input = False;
                if(input[dest_char_index] < 'a' || input[dest_char_index] > 'h') good_input = False;
                if(input[dest_dig_index] < '1' || input[dest_dig_index] > '8') good_input = False;
                if(good_input) {
                    from_loc = single_index_from_chars(input[0], input[1]);
                    to_loc   = single_index_from_chars(input[dest_char_index], input[dest_dig_index]);
                    good_input = posit->check_for_legal_move(from_loc, to_loc);
                    if(posit->board[to_loc] & enemy) taken = True;
                    else taken = False;
                }
                if(!good_input) {
                    write_bad_input();
                }
            }
        } while(!good_input && !*done);

        if(!*done) {
            time(&glob__end_time);
            glob__elapsed_time = glob__end_time - glob__start_time;
            glob__total_time[posit->mover] += glob__elapsed_time;

            write_move_input(input);
            write_out_time(posit);

            move = new_move_type(from_loc, to_loc, 0, NULL);
            posit->make_move_on_board(move);
            update_move_list(posit, move, taken, done);
            delete_move_type(move);
        }
    }

