
#include <glob.h>
#include <string.h>
#include <debug.h>
#include <evaluate.h>
#include <evaluate1.h>
#include <evaluate2.h>
#include <hashbrd.h>
#include <hashtabl.h>
#include <killer.h>
#include <position.h>
#include <utility.h>
#include <givmeval.h>
static int all_tried = 0;
static int all_cut = 0;

int give_me_a_value(
        Position_type  *old_posit,  // The position.
        Gen_move_type  *move,       // The move to give value of.
        int             depth,      // The depth to search to.
        int             curr_depth, // The depth currently being searched.
        int             alpha_beta, // The alpha beta value for min-max.
        char            comp_color, // The color of the computer.
        Gen_move_type **best_line   // The best seen continuation.
    )
    // This procedure is sent the position and a pointer variable which points
    // to a record holding a move.  This move is made and then the tree from
    // the corresponding position is evaluated by generating all possible
    // moves and then using this procedure to evaluate each one.  If the
    // appropriate debug flag is set, best_line will be filled with a pointer
    // to a move list of the best line from here.  If we are set to use the
    // hashtable, this is checked and filled in for levels that are greater
    // than 2 and less than the maximum level.
    {
        Gen_move_type *best_below = NULL;   // The best line found so far.
        Gen_move_type *best_moves_below;    // The best line returned from call of give_me_a_value.
        char           code[70];            // Holds the Forsyth notation of the position.
        int            extension = False;   // Set to true if we have done a check extension.
        Gen_move_type *last_evaluated;      // Points to last move evaluated.
        int            min_max;             // Keeps track of min-max value.
        Gen_move_type *move_top;            // Top of list of moves.
        Position_type *posit;               // The position after this move.
        Two_rep_elem  *repeated_pos;        // Holds repeated position structure.
        int            ret_val;             // The value to return.
        Gen_move_type *temp;                // Used when processing move list.
        Gen_move_type *temp1;               // Used when processing move list.
        int            value;               // Keeps track of current value.


        // Copy the position.

        posit = new_position();
        *posit = *old_posit;

        // Make the move on the local board.

        posit->make_move_on_board(move);

        // Check to see if this position has occured twice already if
        // we are at a depth of 1 or 2.

        if(curr_depth <= 2 && posit->positions_repeated_twice) {
            posit->to_forsyth(code);
            for(repeated_pos = posit->positions_repeated_twice; repeated_pos; repeated_pos = repeated_pos->next) {
                if(strcmp(code, glob__past_positions[repeated_pos->index]) == 0) {
                    delete_position(posit);
                    if(glob__print_expected_line_of_play) *best_line = NULL;
                    return(0);
                }
            }
        }

        // Return the evaluation of this position if we are at maximum depth.

#if Check_Extension

        // Extend our search one ply if the mover is in check.
        // This helps to sort out tactics.

        if(curr_depth == depth) {
            if(posit->king_in_check()) {

                // Here we look ahead one more ply to illuminate the complications
                // from the check.

                depth++;
                extension = True;
                glob__extension_depth_reached = depth;
            } else {
                if(glob__print_expected_line_of_play) *best_line = NULL;
                if(Debug11) {
                    if(comp_color == White) {
                        value = evaluate1(posit, comp_color);
                    } else {
                        value = evaluate2(posit, comp_color);
                    }
                } else {
                    value = evaluate(posit, comp_color);
                }
    
                delete_position(posit);
                return(value);
            }
        }
#else

        if(curr_depth == depth) {
            if(glob__print_expected_line_of_play) *best_line = NULL;
            if(Debug11) {
                if(comp_color == White) {
                    value = evaluate1(posit, comp_color);
                } else {
                    value = evaluate2(posit, comp_color);
                }
            } else {
                value = evaluate(posit, comp_color);
            }

            delete_position(posit);
            return(value);
        }

#endif

        // First check to see if this position is stored in the hashtable, and if so
        // use the value there.  We don't bother for levels 1 and 2 since there
        // could not be a repeated position.  We don't do it for the maximum depth
        // because of the huge hashtable needed, and the fact that the the gains are
        // not as large as for nodes higher in the tree.

#if Use_Hashtable
        if(Use_Hashtable && curr_depth > 2 && glob__hash_table->check_table(&posit->hash_board, &value, curr_depth, alpha_beta, comp_color)) {
            delete_position(posit);
            if(glob__print_expected_line_of_play) *best_line = NULL;
            return(value);
        }
#endif

//        // If we are at the level just above the level where we evaluate, evaluate the
//        // position with the opposite side to move.  If we are out of the alpha_beta range
//        // then we will almost surely be after generating the moves and evaluating.  Return
//        // without doing this.  The main place where this could cause a problem is in a
//        // zugswang position, but it saves a lot of time elsewhere.  This is the null-move
//        // heuristic.
//
//        if(curr_depth == depth - 1 && curr_depth > 1 && !posit->king_in_check()) {
//            posit->mover = other_color(posit->mover);
//            value = evaluate(posit, comp_color);
//            posit->mover = other_color(posit->mover);
//            if(posit->mover = comp_color) {
//                if(value > alpha_beta) {
//                    delete_position(posit);
//                    return(value);
//                }
//            } else {
//                if(value < alpha_beta) {
//                    delete_position(posit);
//                    return(value);
//                }
//            }
//        }

        // Generate all possible moves from this position for the color to move.

        if(curr_depth == 1 && depth > 2) {
            move_top = move->next_lev_moves;
        } else {
            if(curr_depth == depth - 1) {
                glob__curr_level = 0;
            } else {
                glob__curr_level = curr_depth;
            }
            move_top = posit->gen_moves();
        }

        if(move_top) {
            temp  = move_top;
            temp1 = temp;
            min_max = glob__alpha_betas[curr_depth];

            // Set up values for processing based on color.

            if(posit->mover == comp_color) {
                glob__alpha_betas[curr_depth + 2] = -max_value + 1;
                value   = -max_value;

                while(temp && value <= alpha_beta) {
                    value = give_me_a_value(posit, temp, depth, curr_depth + 1, min_max, comp_color, &best_moves_below);
                    if(value > alpha_beta && curr_depth > 2 && curr_depth < depth - 1) {
                        glob__killers.save_killer(curr_depth, temp);
                    }
                    temp->val = value;
                    if(value - tollerance > min_max) {
                        min_max = value - tollerance;
                        glob__alpha_betas[curr_depth + 2] = min_max;
                    } else if(value - tollerance > glob__alpha_betas[curr_depth + 2]) {
                        glob__alpha_betas[curr_depth + 2] = value - tollerance;
                    }
                    if(temp1->val <= temp->val) {
                        if(glob__print_expected_line_of_play) {
                            free_list(best_below);
                            best_below = best_moves_below;
                        }
                        temp1 = temp;
                    } else if(glob__print_expected_line_of_play) free_list(best_moves_below);
                    last_evaluated = temp;
                    temp = temp->next;
                }
            } else {
                glob__alpha_betas[curr_depth + 2] = max_value - 1;
                value   = max_value;

                while(temp && value >= alpha_beta) {
                    value = give_me_a_value(posit, temp, depth, curr_depth + 1, min_max, comp_color, &best_moves_below);
                    if(value < alpha_beta && curr_depth > 2 && curr_depth < depth - 1) {
                        glob__killers.save_killer(curr_depth, temp);
                    }
                    temp->val = value;
                    if(value + tollerance < min_max) {
                        min_max = value + tollerance;
                        glob__alpha_betas[curr_depth + 2] = min_max;
                    } else if(value + tollerance < glob__alpha_betas[curr_depth + 2]) {
                        glob__alpha_betas[curr_depth + 2] = value + tollerance;
                    }
                    if(temp1->val >= temp->val) {
                        if(glob__print_expected_line_of_play) {
                            free_list(best_below);
                            best_below = best_moves_below;
                        }
                        temp1 = temp;
                    } else if(glob__print_expected_line_of_play) free_list(best_moves_below);
                    last_evaluated = temp;
                    temp = temp->next;
                }
            }

            ret_val = temp1->val;
            if(glob__print_expected_line_of_play) {
                *best_line = new_move_type();
                **best_line = *temp1;
                (*best_line)->next = best_below;
            }

            if(curr_depth == 1 && !extension) {

                // Sort the level 2 moves as far as we analysed.
                // First cut the list where we stopped evaluating.
                // Sort the first part of the list, then append the
                // saved portion of the list to the end of the sorted list.

                temp = last_evaluated->next;
                last_evaluated->next = NULL;
                move_top = sort_moves_reverse(move_top);
                while(last_evaluated->next) last_evaluated = last_evaluated->next;
                last_evaluated->next = temp;
                move->next_lev_moves = move_top;
            } else {
                free_list(move_top);
            }

#if Use_Hashtable
            // Put this value into the hashtable.

            if(Use_Hashtable && curr_depth > 2) glob__hash_table->add_to_table(&posit->hash_board, ret_val, curr_depth, alpha_beta);
#endif

            delete_position(posit);
            return(ret_val);

        } else {

            // Here we have either a stalemate or checkmate position.

            if(glob__print_expected_line_of_play) *best_line = NULL;
            if(posit->king_in_check()) {
                if(posit->mover == comp_color) value = -checkmate_value;
                else value = checkmate_value;
            } else value = 0;

#if Use_Hashtable
            // Put this value into the hashtable.

            if(Use_Hashtable && curr_depth > 2) glob__hash_table->add_to_table(&posit->hash_board, value, curr_depth, alpha_beta);
#endif

            delete_position(posit);
            return(value);
        }
    }


