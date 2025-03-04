
#include <glob.h>
#include <debug.h>
#include <stdlib.h>
#include <time.h>
#include <givmeval.h>
#include <hashbrd.h>
#include <hashtabl.h>
#include <io.h>
#include <killer.h>
#include <openbook.h>
#include <position.h>
#include <updmvlst.h>
#include <utility.h>
#include <gncmpmov.h>

void generate_computer_move(
        Position_type  *posit,    // The current position.
        int             level,    // The level to look to.
        int            *done      // Set to true if end of game.
    )
    // This function will generate a move for the given color and
    // the given position.  It does this by looking ahead to the
    // specified depth in the game tree starting at the given
    // position.  It uses a modified Alpha-Beta search method.
    // If the game is ended, the variable done is set to true.
    {
        char            computer_color;      // Color of computer.
        int             curr_level;          // The current level we are working on.
        Gen_move_type  *forseen_line;        // The expected line of play.
        bd_index        from_loc;            // Where to move from.
        int             good_move_count;     // Number of good moves.
        int             i;                   // Counter.
        int             leave_loop;          // Set to true if a checkmate is found.
        long            level_end_time;      // End of level time for debug3.
        long            level_time_start;    // Start of analysis for this level.
        long            max_time;            // Max time for this move.
        long            max_time_90_percent; // Ninety percent of max time.
        int             max_val;             // Maximum value.
        int             min_max;             // Min max value for search.
        long            move_time_start;     // Start of analysis for this move.
        Gen_move_type  *move_top;            // Top of move list.
        int             picked_move;         // Move number chosen.
        int             taken;               // True if the move was a capture.
        Gen_move_type  *temp_move;           // Temporary move pointer.
        bd_index        to_loc;              // Where to move to.
        Gen_move_type  *the_move;            // The move chosen.
        int             value;               // Value of current move.


        glob__posits_evaled = 0;
        glob__depth_reached = 0;
        glob__extension_depth_reached = 0;
        leave_loop = False;

        if(Debug16) {
            glob__priority_1_count = 0;
            glob__priority_2_count = 0;
            glob__priority_3_count = 0;
            glob__priority_4_count = 0;
            glob__priority_5_count = 0;
        }

        // We set the max time that we should try to use for this move to
        // (TL - (TL/(2*ML))) / ML
        // which always leaves half a move's worth of time for overshoot.

        if(glob__use_fischer_timing) {
            glob__time_left_in_tc[posit->mover] += glob__fischer_time_inc[posit->mover] - glob__operator_time;
            max_time = (glob__time_left_in_tc[posit->mover] + 29 * (glob__fischer_time_inc[posit->mover] - glob__operator_time)) / 30;
            if(max_time * 2 > glob__time_left_in_tc[posit->mover]) max_time = glob__time_left_in_tc[posit->mover] / 2;
            max_time_90_percent = max_time;
        } else {
            max_time = (glob__time_left_in_tc[posit->mover] - (glob__time_left_in_tc[posit->mover] / (2 * glob__moves_left_in_tc[posit->mover]))) / glob__moves_left_in_tc[posit->mover];
            max_time_90_percent = 9 * max_time / 10;
        }
        time(&glob__start_time);

        // Check for this position in the opening library, and use
        // one of the moves if found.  If not found, then use
        // game tree search.

        if(glob__use_library && glob__opening_book.check_library(posit, &the_move)) {

            // Move found in the opening library.

            from_loc = the_move->from_loc;
            to_loc   = the_move->to_loc;

        } else {
            write_thinking_message();

            computer_color = posit->mover;
            glob__curr_level = 0;
            move_top = posit->gen_moves();
            if(move_top->next == NULL) {
                the_move = move_top;
                the_move->continuation = NULL;
            } else {
                if(glob__print_expected_line_of_play) {

                    // Null out the continuation fields.

                    temp_move = move_top;
                    while(temp_move) {
                        temp_move->continuation = NULL;
                        temp_move = temp_move->next;
                    }
                }
                for(curr_level = 1; curr_level <= level; curr_level++) {
                    glob__killers.clear_killers();
                    if(glob__print_detailed_timings) {
                        time(&level_time_start);
                        cout << "\nMove comp times: ";
                    }

#if Use_Hashtable
                    // Clear the hashtable if it was used for this round.

                    if(Use_Hashtable && curr_level > 2) glob__hash_table->clear_table();
#endif

                    glob__depth_reached = curr_level;
                    glob__extension_depth_reached = curr_level;
                    min_max = -max_value + 1;
                    glob__alpha_betas[1] = max_value - 1;
                    glob__alpha_betas[2] = min_max;
                    for(temp_move = move_top; temp_move; temp_move = temp_move->next) {
                        if(glob__print_detailed_timings) time(&move_time_start);
                        value = give_me_a_value(posit, temp_move, curr_level, 1, min_max, computer_color, &forseen_line);
                        if(glob__print_expected_line_of_play) {
                            if(temp_move->continuation) free_list(temp_move->continuation);
                            temp_move->continuation = forseen_line;
                        }
                        temp_move->val = value;
                        if(value == checkmate_value) {
                            leave_loop = True;
                            break;
                        }
                        if(-max_value + tollerance <= value) {
                            if(value - tollerance > min_max) {
                                min_max = value - tollerance;
                                glob__alpha_betas[2] = min_max;
                            }
                        } else {
                            min_max = -max_value;
                            glob__alpha_betas[2] = min_max;
                        }

                        // Make sure we haven't exceeded our time limit, but
                        // that we have at least completed level 1.

                        time(&glob__end_time);
                        if(glob__print_detailed_timings) cout << (glob__end_time - move_time_start) << " ";
                        glob__elapsed_time = glob__end_time - glob__start_time;
                        if(glob__elapsed_time >= max_time_90_percent && curr_level > 1) {
                            leave_loop = True;

                            // Only look at moves looked at on this level.

                            the_move = temp_move->next;
                            temp_move->next = NULL;
                            if(glob__print_expected_line_of_play) {

                                // Clear continuations of this list.

                                temp_move = the_move;
                                while(temp_move) {
                                    if(temp_move->continuation) free_list(temp_move->continuation);
                                    temp_move = temp_move->next;
                                }
                            }

                            // Free move lists for second level.

                            temp_move = the_move;
                            while(temp_move) {
                                free_list(temp_move->next_lev_moves);
                                temp_move = temp_move->next;
                            }
                            free_list(the_move);
                            glob__depth_reached = -curr_level;

                            break;
                        }
                    }

                    move_top = sort_moves(move_top);

                    // We have found a checkmate, so we can exit.

                    if(move_top->val >= checkmate_value - (tollerance + 1) * 32) leave_loop = True;
                    if(move_top->val <= -checkmate_value + (tollerance + 1) * 32) leave_loop = True;

                    if(Debug17) {
                        glob__killers.print_killers();
                    }
                    if(glob__print_detailed_timings) {
                        cout << "\nTotal time for level " << curr_level << " was: " << (glob__end_time - level_time_start) << '\n';
                    }
                    if(glob__print_moves_and_values) {
                        time(&level_end_time);
                        cout << "\nDepth is " << curr_level << " Ext depth is " << glob__extension_depth_reached << " time is " << level_end_time - glob__start_time << "\n";
                        cout << "Positions evaluated: " << glob__posits_evaled << "  Hashed positions this level: " << glob__hash_boards_used << '\n';
                        print_move_list(posit, move_top, False);
                    }

                    // If we wouldn't gain anything by looking another level deeper
                    // then save the time for use later.  It seems to take about
                    // the same amount of time to analyse the first move of level
                    // n+1 as it did to analyse the entire level n.  If we can't
                    // look at at least the first two moves in the next level then
                    // don't bother.

                    if(glob__elapsed_time + glob__elapsed_time + glob__elapsed_time > max_time) {
                        if(glob__depth_reached >= 0) glob__depth_reached = curr_level;
                        break;
                    }

                    if(leave_loop) break;
                }

                // Here we choose a random move of those with values within tollerance.

                max_val = move_top->val;
                temp_move = move_top;
                good_move_count = 0;
                while(temp_move && temp_move->val >= max_val - tollerance) {
                    good_move_count++;
                    temp_move = temp_move->next;
                }
                picked_move = rand() % good_move_count;
                for(i = 1, the_move = move_top; i < picked_move; i++) the_move = the_move->next;
            }
            from_loc = the_move->from_loc;
            to_loc   = the_move->to_loc;
            temp_move = the_move;
            the_move = new_move_type();
            *the_move = *temp_move;
            if(glob__print_expected_line_of_play) {

                // Print continuation of chosen move, then
                // clear al continuations in list.

                temp_move = new_move_type();
                *temp_move = *the_move;
                temp_move->next = the_move->continuation;

                print_continuation(temp_move);
                delete_move_type(temp_move);

                temp_move = move_top;
                while(temp_move) {
                    if(temp_move->continuation) free_list(temp_move->continuation);
                    temp_move = temp_move->next;
                }
            }

            // Free lists of moves on second level before freeing this list.

            temp_move = move_top;
            while(temp_move) {
                free_list(temp_move->next_lev_moves);
                temp_move = temp_move->next;
            }

            free_list(move_top);
        }
        newline();

        // Update timing.

        time(&glob__end_time);
        glob__elapsed_time = glob__end_time - glob__start_time;
        glob__total_time[posit->mover] += glob__elapsed_time;
        glob__time_left_in_tc[posit->mover] -= glob__elapsed_time;
        if(Debug13) {
            if(glob__time_left_in_tc[posit->mover] < 0) {
                cout << "We ran out of time.  We currently have " << glob__time_left_in_tc[posit->mover] << " seconds.\n";
            }
        }
        if(!glob__use_fischer_timing) {
            glob__moves_left_in_tc[posit->mover]--;
            if(glob__moves_left_in_tc[posit->mover] == 0) {
                glob__moves_left_in_tc[posit->mover] = glob__moves_per_tc[posit->mover];
                glob__time_left_in_tc[posit->mover] += glob__time_per_tc[posit->mover];
            }
    
            if(glob__print_detailed_timings) {
                cout << "Average of " << glob__time_left_in_tc[posit->mover] / glob__moves_left_in_tc[posit->mover] << " seconds per move for remaining time control.\n";
            }
        }

        if(glob__print_evaluator_call_count) {
            cout << "Pos: " << glob__posits_evaled << "  "
                 << "Hash: " << glob__hash_boards_used << "  "
                 << "Depth: ";
            if(glob__depth_reached >= 0) cout << glob__depth_reached << "  ";
            else cout << -glob__depth_reached << "-  ";
            cout << "Pos/sec: " << glob__posits_evaled / ((glob__elapsed_time == 0) ? 1 : glob__elapsed_time) << "  "
                 << "Time: " << glob__elapsed_time << "  "
                 << "Tot time: " << glob__total_time[posit->mover] / 60 << " min " << glob__total_time[posit->mover] % 60 << " sec.\n";
        }

        if(Debug16) {
            cout << "Priority 1 moves added: " << glob__priority_1_count << "\n";
            cout << "Priority 2 moves added: " << glob__priority_2_count << "\n";
            cout << "Priority 3 moves added: " << glob__priority_3_count << "\n";
            cout << "Priority 4 moves added: " << glob__priority_4_count << "\n";
            cout << "Priority 5 moves added: " << glob__priority_5_count << "\n";
        }

        if(posit->board[to_loc] == Empty) taken = False;
        else taken = True;

        // Write out move.
                 
        write_out_computer_move(from_loc, to_loc, taken, the_move->val);

        write_out_time(posit);

        // Make move on board and other things.

        posit->make_move_on_board(the_move);
        update_move_list(posit, the_move, taken, done);
        delete_move_type(the_move);
    }


