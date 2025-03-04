
#include <glob.h>
#include <debug.h>
#include <hashbrd.h>
#include <io.h>
#include <string.h>
#include <position.h>
#include <utility.h>
#include <updmvlst.h>

void update_move_list(
        Position_type  *posit,  // The current position.
        Gen_move_type  *move,   // The move made.
        int             taken,  // True if this was a capture.
        int            *done    // Set to true if end of game.
    )
    // It will record the move with a dash if no piece was taken and with
    // an X if a piece was taken.  This procedure should be called after
    // MakeMoveOnBoard and it should only be called when an actual move
    // is made.  It should not be called when the computer analizes a
    // position.  Taken must be set to true if a piece was taken and to
    // false if not.
    {
        char            check_char;      // Chyaracter to write out for check.
        char           *code;            // Holds the Forsyth notation of the position.
        int             count = 0;       // Number of same positions.
        int             i;               // Counter.
        char            just_moved;      // The color that just moved.
        Gen_move_type  *move_top;        // Holds list of moves.
        Two_rep_elem   *rep_pos_element; // Holds repeated position structure.
        char            separator;       // The character to separate the from and to locations.
        char            temp_code[70];   // Temporary holder for Forsyth code.


        just_moved = other_color(posit->mover);

        if(taken || posit->board[move->from_loc] & pawn_mask) glob__draw_count = 0;
        else glob__draw_count++;

        // Write out to list file.

        if(taken) separator = 'x';
        else separator = '-';

        if(posit->king_in_check()) {
            check_char = '+';
            write_check();
        } else check_char = ' ';
        newline();

        if(just_moved == White) {
            *glob__list_file << '\n' << glob__current_move << ".   ";
        }
        glob__list_file->put((char)file_as_char(move->from_loc));
        glob__list_file->put((char)rank_as_char(move->from_loc));
        glob__list_file->put(separator);
        glob__list_file->put((char)file_as_char(move->to_loc));
        glob__list_file->put((char)rank_as_char(move->to_loc));
        glob__list_file->put(check_char);
        *glob__list_file << move->val;
        if(glob__depth_reached >= 0) *glob__list_file << "     " << glob__depth_reached << " ";
        else *glob__list_file << "     " << -glob__depth_reached << "-";
        *glob__list_file << " " << glob__elapsed_time;

        // Make sure it isn't checkmate or stalemate.

        glob__curr_level = 0;
        move_top = posit->gen_moves();
        if(!move_top) {
            *done = True;
            if(check_char == '+') {
                write_checkmate();
                *glob__list_file << " Mate\n";
            } else {
                write_stalemate();
                *glob__list_file << " Stalemate\n";
            }
            if(glob__print_pos) print_board(posit);

        }
        free_list(move_top);

        // Check for 50 move rule.

        if(glob__draw_count == 100) {
            *done = True;
            write_draw_50();
            *glob__list_file << " Draw by 50 move rule.\n";
        }

        glob__current_position.to_forsyth(temp_code);
        code = new char[strlen(temp_code)+1];
        strcpy(code, temp_code);

        // Check for position repetition.

        for(i = 1; i <= glob__position_count; i++) {
            if(!strcmp(code, glob__past_positions[i])) count++;
        }

        if(count >= 2) {
            *done = True;
            write_draw_repetition();
            *glob__list_file << " Draw by repetition.\n";
        } else {

            glob__past_positions[++glob__position_count] = code;

            // If this position has occured before, then this is the
            // second.  Record it in the position record as a position
            // that has occured twice for draw recognition purposes.

            if(count == 1) {
                rep_pos_element = new Two_rep_elem;
                rep_pos_element->index = glob__position_count;
                rep_pos_element->next  = posit->positions_repeated_twice;
                posit->positions_repeated_twice = rep_pos_element;
            }
        }

        if(just_moved == White) {
            *glob__list_file << "       ";
        }
        glob__list_file->flush();
    }

