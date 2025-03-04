
#include <glob.h>
#include <debug.h>
#include <hashbrd.h>
#include <killer.h>
#include <piececod.h>
#include <statval.h>
#include <utility.h>
#include <position.h>


#if Use_Hashtable

#define ADD_PIECE(loc, piece) hash_board.add_piece(loc, piece)
#define ADD_HASH_PIECE(loc, piece) hash_board.add_hash_piece(loc, piece)
#define CONSISTENCY_CHECK(posit) hash_board.check_consistency(posit)
#define SET_LIKE_POSIT(posit) hash_board.set_like_posit(posit)
#define SET_MOVER(mover) hash_board.set_mover(mover)

#else

#define ADD_PIECE(loc, piece)
#define ADD_HASH_PIECE(loc, piece)
#define CONSISTENCY_CHECK(posit)
#define SET_LIKE_POSIT(posit)
#define SET_MOVER(mover)

#endif


// Used to hold lists of generated moves by priority.

static Gen_move_type *moves_all;
static Gen_move_type *moves_priority_1;
static Gen_move_type *moves_priority_2;
static Gen_move_type *moves_priority_3;
static Gen_move_type *moves_priority_4;
static Gen_move_type *moves_priority_5;

// Holds mask for enemy pieces in move generation.

static square enemy_mask;

// Holds location of piece to move in move generation.

static bd_index piece_loc;

// Points to free list of positions.

static Position_type *pos_free_list = NULL;

static int white_advancement_points[total_files * total_ranks] = 
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 5, 6, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 5, 6, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 5, 6, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 5, 6, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 6, 6, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static int black_advancement_points[total_files * total_ranks] = 
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 6, 5, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 6, 5, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 6, 5, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 6, 5, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0,
      0, 0, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



//
// These functions are public.
//


Gen_move_type *Position_type::gen_moves()
    // This function will generate all legal moves from this position
    // and return a list containing them.  The list will be ordered
    // generally by priority.  See the comments to add_move and
    // append_move_list for more on how this is done.
    {
        square          friendly;       // Holds mask for friendly pieces.
        bd_index        i;              // Counter.


        // Initialize move holders.

        moves_priority_1 = NULL;
        moves_priority_2 = NULL;
        moves_priority_3 = NULL;
        moves_priority_4 = NULL;
        moves_priority_5 = NULL;

        set_piece_sets(mover, &friendly, &enemy_mask);

        // Go through files and ranks looking for pieces of mover's color to move.
        // When found call the appropriate function to generate the piece's moves.

        for(i = 0; (piece_loc = piece_locations[mover][i]) != 0; i++) {
//        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
//            for(piece_loc = i; piece_loc < i + (next_u_file * 8); piece_loc += next_u_file) {
//                if(board[piece_loc] & friendly) {
                    switch(board[piece_loc]) {
                        case Wpawn :
                        case Bpawn :
                            pawn_moves(); break;
                        case Wknight :
                        case Bknight :
                            knight_moves(); break;
                        case Wbishop :
                        case Bbishop :
                            bishop_moves(); break;
                        case Wrook :
                        case Brook :
                            rook_moves(); break;
                        case Wqueen :
                        case Bqueen :
                            queen_moves(); break;
                        case Wking :
                        case Bking :
                            king_moves(); break;
                    }
//                }
//            }
        }

        // Combine lists of moves and captures with captures in front.
        // This increases alpha-beta pruning.

        append_move_list();

        if(glob__print_moves_generated) print_move_list(this, moves_all);

        return(moves_all);
    }


//void Position_type::make_move_on_board(
//        Gen_move_type *move    // The move to make.
//    )
//    // This function will make the given move on the board, and will update
//    // all of the arrays and variables in the class to reflect the move.
//    // It will also switch the mover to the other color.
//    {
//        Position_type *debug_posit;
//        int            file_diff;
//        int            from_file_numb;
//        bd_index       from_loc;
//        int            i, j;
//        char           other_col;
//        int            pawn_struct_change = False;
//        int            pawn_value;
//        int            rank_diff;
//        int            to_file_numb;
//        bd_index       to_loc;
//
//
//        from_loc = move->from_loc;
//        to_loc   = move->to_loc;
//        from_file_numb = file_number(from_loc);
//        to_file_numb   = file_number(to_loc);
//        other_col = other_color(mover);
//
//        // Update total_piece_value array if a piece was taken.
//
//        if(board[to_loc]) {
//            total_piece_value[other_col] -= piece_value(to_loc);
//
//            piece_loc_remove_piece(other_col, to_loc);
//            update_square_attacked_remove_piece(to_loc);
//
//            if(board[to_loc] & pawn_mask) {               
//                for(i = 0; i < pawn_structure[other_col][to_file_numb]; i++) {
//                    if(pawn_location[other_col][to_file_numb][i] == to_loc) {
//                        pawn_location[other_col][to_file_numb][i] = pawn_location[other_col][to_file_numb][--pawn_structure[other_col][to_file_numb]];
//                        break;
//                    }
//                }
////                pawn_structure[other_col][to_file_numb]--;
//                pawn_struct_change = True;
//            }
//        }
//
//        // Erase enemy pawn in case of en pesant.
//        // Update piece value and pawn structure arrays.
//
//        if(board[from_loc] & pawn_mask) {
//            if(from_file_numb != to_file_numb) {
//                for(i = 0; i < pawn_structure[mover][from_file_numb]; i++) {
//                    if(pawn_location[mover][from_file_numb][i] == from_loc) {
//                        pawn_location[mover][from_file_numb][i] = pawn_location[mover][from_file_numb][--pawn_structure[mover][from_file_numb]];
//                        break;
//                    }
//                }
////                pawn_structure[mover][from_file_numb]--;
//                pawn_location[mover][to_file_numb][pawn_structure[mover][to_file_numb]++] = to_loc;
////                pawn_structure[mover][to_file_numb]++;
//                pawn_struct_change = True;
//
//                if(!board[to_loc]) {
//
//                    // En pesant case.
//
//                    i = to_loc + ((mover == White) ? next_d_file : next_u_file);
//                    total_piece_value[other_col] -= piece_value(i);
//                    piece_loc_remove_piece(other_col, i);
//                    update_square_attacked_remove_piece(i);
//                    board[i] = Empty;
//                    ADD_HASH_PIECE(i, H_Empty);
//                    for(j = 0; j < pawn_structure[other_col][to_file_numb]; j++) {
//                        if(pawn_location[other_col][to_file_numb][j] == i) {
//                            pawn_location[other_col][to_file_numb][j] = pawn_location[other_col][to_file_numb][--pawn_structure[other_col][to_file_numb]];
//                            break;
//                        }
//                    }
////                    pawn_structure[other_col][to_file_numb]--;
//                    pawn_struct_change = True;
//                }
//            } else {
//
//                // Update pawn location structure for pawn advance.
//
//                for(i = 0; i < pawn_structure[mover][from_file_numb]; i++) {
//                    if(pawn_location[mover][from_file_numb][i] == from_loc) {
//                        pawn_location[mover][from_file_numb][i] = to_loc;
//                        break;
//                    }
//                }
//            }
//        }
//
//        // Subtract the value of the piece where it was on the board.
//
//        total_piece_value[mover] -= piece_value(from_loc);
//
//        // Update attack array.
//        // Clear the to location first for attack updating in capture case.
//
//        board[to_loc] = Empty;
//        update_square_attacked_remove_piece(from_loc);
//
//        // Move the piece.
//
//        board[to_loc]   = board[from_loc];
//        ADD_PIECE(to_loc, board[from_loc]);
//        board[from_loc] = Empty;
//        ADD_HASH_PIECE(from_loc, H_Empty);
//
//        // If a pawn queened, the turn it into a queen.
//
//        if((board[to_loc] & pawn_mask) && (rank_number(to_loc) == 8 || rank_number(to_loc) == 1)) {
//            if(mover == White) {
//                board[to_loc] = Wqueen;
//                ADD_HASH_PIECE(to_loc, H_Wqueen);
//            } else {
//                board[to_loc] = Bqueen;
//                ADD_HASH_PIECE(to_loc, H_Bqueen);
//            }
//            for(j = 0; j < pawn_structure[mover][to_file_numb]; j++) {
//                if(pawn_location[mover][to_file_numb][j] == to_loc) {
//                    pawn_location[mover][to_file_numb][j] = pawn_location[mover][to_file_numb][--pawn_structure[mover][to_file_numb]];
//                    break;
//                }
//            }
////            pawn_structure[mover][to_file_numb]--;
//            pawn_struct_change = True;
//        }
//
//        // Add the value of the piece where it was moved on the board.
//
//        total_piece_value[mover] += piece_value(to_loc);
//
//        // Update attacks array.
//
//        update_square_attacked_add_piece(to_loc);
//        piece_loc_move_piece(mover, from_loc, to_loc);
//
//        // Update king position and possibly castling if king was moved.
//
//        if(board[to_loc] & king_mask) {
//
//            // Update king position.
//
//            king_loc[mover] = to_loc;
//
//            // Move rook if the move was castling.
//
//            if(from_loc == white_king_home || from_loc == black_king_home) {
//                file_diff = from_file_numb - to_file_numb;
//
//                // Check for castling.
//
//                if(abs_v(file_diff) == 2) {
//
//                    // Decide if queen or king side castling.
//
//                    if(file_diff == 2) {
//                        i = to_loc + next_l_rank + next_l_rank;
//                        j = to_loc + next_r_rank;
//                    } else {
//                        i = to_loc + next_r_rank;
//                        j = to_loc + next_l_rank;
//                    }
//
//                    // Update information for the rook.
//
//                    update_square_attacked_remove_piece(i);
//                    board[j] = board[i];
//                    ADD_PIECE(j, board[i]);
//                    board[i] = Empty;
//                    ADD_HASH_PIECE(i, H_Empty);
//                    piece_loc_move_piece(mover, i, j);
//                    update_square_attacked_add_piece(j);
//                }
//            }
//        }
//
//        // If the pawn structure changed, we have to update pawn_struct_val.
//
//        if(pawn_struct_change) {
//
//            pawn_value = 0;
//
//            for(j = 1; j <= 8; j++) {
//
//                // Subtract for doubles pawns.
//
//                if(pawn_structure[White][j] > 1) pawn_value -= pawn_structure[White][j]*15;
//                if(pawn_structure[Black][j] > 1) pawn_value += pawn_structure[Black][j]*15;
//
//                // Subtract for isolated pawns.
//
//                if(pawn_structure[White][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j+1] == 0)
//                    pawn_value -= pawn_structure[White][j]*25;
//                if(pawn_structure[Black][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j+1] == 0)
//                    pawn_value += pawn_structure[Black][j]*25;
//
//                // Add for passed pawns.
//
//                if(pawn_structure[White][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j] == 0 && pawn_structure[Black][j+1] == 0)
//                    pawn_value += 50;
//                if(pawn_structure[Black][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j] == 0 && pawn_structure[White][j+1] == 0)
//                    pawn_value -= 50;
//            }
//
//            pawn_struct_value = pawn_value;
//        }
//
//        // Make sure to update can_castle array based on rook movements.
//
//        if(board[white_left_rook_home] != Wrook) can_castle[White][1] = False;
//        if(board[white_right_rook_home] != Wrook) can_castle[White][2] = False;
//        if(board[black_left_rook_home] != Brook) can_castle[Black][1] = False;
//        if(board[black_right_rook_home] != Brook) can_castle[Black][2] = False;
//
//        // Set flag for vulnerable to en pesant if pawn moved out two.
//
//        if(board[move->to_loc] & pawn_mask) {
//            rank_diff = rank_number(move->to_loc) - rank_number(move->from_loc);
//            if(abs_v(rank_diff) == 2) vuln_ep[mover][file_number(move->to_loc)] = True;
//        }
//
//        // Deal with castling.
//
//        if(board[move->to_loc] & king_mask) {
//            if(abs_v(file_number(move->to_loc) - file_number(move->from_loc)) == 2) {
//                castled[mover] = True;
//            }
//            can_castle[mover][1] = False;
//            can_castle[mover][2] = False;
//        }
//
//        if(mover == White) mover = Black;
//        else mover = White;
//        SET_MOVER(mover);
//
//        // Clear EP array for side to move next since no longer vulnerable.
//
//        for(i = 0; i <= 9; i++) vuln_ep[mover][i] = False;
//
//        // If this debug flag is set, make sure all the information in the position record
//        // is correct.  Since many of the things here are kept track of as moves are made,
//        // this will check to make sure that that is being done correctly after each move.
//
//        if(Debug6) {
//            int found, k;
//
//            // Statically calculate all of the extra data from a copy of the current position.
//
//            debug_posit = new_position();
//            *debug_posit = *this;
//            debug_posit->calculate_auxillary_board_data();
//
//            // Check to make sure the square attacked data is correct.
//
//            for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
//                for(j = i; j < i + (next_u_file * 8); j += next_u_file) {
//                    if(debug_posit->white_attackers[j] != white_attackers[j]) {
//                        print_board(this);
//                        cout << "====White attackers is wrong for square " << j << '\n';
//                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                        cout << "Value should be " << debug_posit->white_attackers[j] << " and value is " << white_attackers[j] << '\n';
//                    }
//                    if(debug_posit->white_wimpiest_attacker[j] != white_wimpiest_attacker[j]) {
//                        print_board(this);
//                        cout << "====White wimpiest is wrong for square " << j << '\n';
//                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                        cout << "Value should be " << debug_posit->white_wimpiest_attacker[j] << " and value is " << white_wimpiest_attacker[j] << '\n';
//                    }
//                    if(debug_posit->black_attackers[j] != black_attackers[j]) {
//                        print_board(this);
//                        cout << "====Black attackers is wrong for square " << j << '\n';
//                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                        cout << "Value should be " << debug_posit->black_attackers[j] << " and value is " << black_attackers[j] << '\n';
//                    }
//                    if(debug_posit->black_wimpiest_attacker[j] != black_wimpiest_attacker[j]) {
//                        print_board(this);
//                        cout << "====Black wimpiest is wrong for square " << j << '\n';
//                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                        cout << "Value should be " << debug_posit->black_wimpiest_attacker[j] << " and value is " << black_wimpiest_attacker[j] << '\n';
//                    }
//                }
//            }
//
//            // Compare piece value data.
//
//            if(debug_posit->total_piece_value[White] != total_piece_value[White]) {
//                print_board(this);
//                cout << "====White total piece value is wrong.\n";
//                cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                cout << "Value should be " << debug_posit->total_piece_value[White] << " and value is " << total_piece_value[White] << '\n';
//            }
//            if(debug_posit->total_piece_value[Black] != total_piece_value[Black]) {
//                print_board(this);
//                cout << "====Black total piece value is wrong.\n";
//                cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                cout << "Value should be " << debug_posit->total_piece_value[Black] << " and value is " << total_piece_value[Black] << '\n';
//            }
//
//            // Compare pawn structure values.
//
//            for(i = 1; i <= 8; i++) {
//                if(debug_posit->pawn_structure[White][i] != pawn_structure[White][i]) {
//                    print_board(this);
//                    cout << "====White pawn structure is wrong for entry " << i << '\n';
//                    cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                    cout << "Value should be " << debug_posit->pawn_structure[White][i] << " and value is " << pawn_structure[White][i] << '\n';
//                }
//                for(j = 0; j < pawn_structure[White][i]; j++) {
//                    found = False;
//                    for(k = 0; k < pawn_structure[White][i]; k++) {
//                        if(debug_posit->pawn_location[White][i][j] == pawn_location[White][i][k]) found = True;
//                    }
//                    if(!found) {
//                        print_board(this);
//                        cout << "====White pawn locations are wrong for entry " << i << "," << j << '\n';
//                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                        cout << "Value should be " << debug_posit->pawn_location[White][i][j] << " and value is " << pawn_location[White][i][j] << '\n';
//                    }
//                }
//                if(debug_posit->pawn_structure[Black][i] != pawn_structure[Black][i]) {
//                    print_board(this);
//                    cout << "====Black pawn structure is wrong for entry " << i << '\n';
//                    cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                    cout << "Value should be " << debug_posit->pawn_structure[Black][i] << " and value is " << pawn_structure[Black][i] << '\n';
//                }
//                for(j = 0; j < pawn_structure[Black][i]; j++) {
//                    found = False;
//                    for(k = 0; k < pawn_structure[Black][i]; k++) {
//                        if(debug_posit->pawn_location[Black][i][j] == pawn_location[Black][i][k]) found = True;
//                    }
//                    if(!found) {
//                        print_board(this);
//                        cout << "====Black pawn locations are wrong for entry " << i << "," << j << '\n';
//                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
//                        cout << "Value should be " << debug_posit->pawn_location[Black][i][j] << " and value is " << pawn_location[Black][i][j] << '\n';
//                    }
//                }
//            }
//
//            if(pawn_struct_value != debug_posit->pawn_struct_value) {
//                cout << "====Pawn structure value is wrong.\n";
//            }
//
//            // Get rid of temporary position record.
//
//            delete_position(debug_posit);
//        }
//
//        if(Debug12) {
//
//            // Check to make sure that the hash board is consistent with the real board.
//
//            CONSISTENCY_CHECK(this);
//        }
//
//        if(Debug15) {
//
//            // Check consistency of piece_locations array.
//
//            compare_piece_locations();
//        }
//    }
void Position_type::make_move_on_board(
        Gen_move_type *move    // The move to make.
    )
    // This function will make the given move on the board, and will update
    // all of the arrays and variables in the class to reflect the move.
    // It will also switch the mover to the other color.
    {
        Position_type *debug_posit;
        int            file_diff;
        int            from_file_numb;
        bd_index       from_loc;
        int            i, j;
        char           other_col;
        int            pawn_struct_change = False;
        int            pawn_value;
        int            rank_diff;
        int            to_file_numb;
        bd_index       to_loc;


        from_loc = move->from_loc;
        to_loc   = move->to_loc;
        from_file_numb = file_number(from_loc);
        to_file_numb   = file_number(to_loc);
        other_col = other_color(mover);

        // Update total_piece_value array if a piece was taken.

        if(board[to_loc]) {
            total_piece_value[other_col] -= piece_value(to_loc);

            piece_loc_remove_piece(other_col, to_loc);
            update_square_attacked_remove_piece(to_loc);

            if(board[to_loc] & pawn_mask) {
                pawn_structure[other_col][to_file_numb]--;
                pawn_struct_change = True;
            }
        }

        // Erase enemy pawn in case of en pesant.
        // Update piece value and pawn structure arrays.

        if(board[from_loc] & pawn_mask) {
            if(from_file_numb != to_file_numb) {
                pawn_structure[mover][from_file_numb]--;
                pawn_structure[mover][to_file_numb]++;
                pawn_struct_change = True;
            }
            if(!board[to_loc] && !(from_file_numb == to_file_numb)) {

                // En pesant case.

                i = to_loc + ((mover == White) ? next_d_file : next_u_file);
                total_piece_value[other_col] -= piece_value(i);
                piece_loc_remove_piece(other_col, i);
                update_square_attacked_remove_piece(i);
                board[i] = Empty;
                ADD_HASH_PIECE(i, H_Empty);
                pawn_structure[other_col][to_file_numb]--;
                pawn_struct_change = True;
            }
        }

        // Subtract the value of the piece where it was on the board.

        total_piece_value[mover] -= piece_value(from_loc);

        // Update attack array.
        // Clear the to location first for attack updating in capture case.

        board[to_loc] = Empty;
        update_square_attacked_remove_piece(from_loc);

        // Move the piece.

        board[to_loc]   = board[from_loc];
        ADD_PIECE(to_loc, board[from_loc]);
        board[from_loc] = Empty;
        ADD_HASH_PIECE(from_loc, H_Empty);

        // If a pawn queened, the turn it into a queen.

        if((board[to_loc] & pawn_mask) && (rank_number(to_loc) == 8 || rank_number(to_loc) == 1)) {
            if(mover == White) {
                board[to_loc] = Wqueen;
                ADD_HASH_PIECE(to_loc, H_Wqueen);
            } else {
                board[to_loc] = Bqueen;
                ADD_HASH_PIECE(to_loc, H_Bqueen);
            }
            pawn_structure[mover][to_file_numb]--;
            pawn_struct_change = True;
        }

        // Add the value of the piece where it was moved on the board.

        total_piece_value[mover] += piece_value(to_loc);

        // Update attacks array.

        update_square_attacked_add_piece(to_loc);
        piece_loc_move_piece(mover, from_loc, to_loc);

        // Update king position and possibly castling if king was moved.

        if(board[to_loc] & king_mask) {

            // Update king position.

            king_loc[mover] = to_loc;

            // Move rook if the move was castling.

            if(from_loc == white_king_home || from_loc == black_king_home) {
                file_diff = from_file_numb - to_file_numb;

                // Check for castling.

                if(abs_v(file_diff) == 2) {

                    // Decide if queen or king side castling.

                    if(file_diff == 2) {
                        i = to_loc + next_l_rank + next_l_rank;
                        j = to_loc + next_r_rank;
                    } else {
                        i = to_loc + next_r_rank;
                        j = to_loc + next_l_rank;
                    }

                    // Update information for the rook.

                    update_square_attacked_remove_piece(i);
                    board[j] = board[i];
                    ADD_PIECE(j, board[i]);
                    board[i] = Empty;
                    ADD_HASH_PIECE(i, H_Empty);
                    piece_loc_move_piece(mover, i, j);
                    update_square_attacked_add_piece(j);
                }
            }
        }

        // If the pawn structure changed, we have to update pawn_struct_val.

        if(pawn_struct_change) {

            pawn_value = 0;

            for(j = 1; j <= 8; j++) {

                // Subtract for doubles pawns.

                if(pawn_structure[White][j] > 1) pawn_value -= pawn_structure[White][j]*15;
                if(pawn_structure[Black][j] > 1) pawn_value += pawn_structure[Black][j]*15;

                // Subtract for isolated pawns.

                if(pawn_structure[White][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j+1] == 0)
                    pawn_value -= pawn_structure[White][j]*25;
                if(pawn_structure[Black][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j+1] == 0)
                    pawn_value += pawn_structure[Black][j]*25;

                // Add for passed pawns.

                if(pawn_structure[White][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j] == 0 && pawn_structure[Black][j+1] == 0) {
                    pawn_value += 75;
                    if(pawn_structure[White][j-1] || pawn_structure[White][j+1]) pawn_value += 25;
                }
                if(pawn_structure[Black][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j] == 0 && pawn_structure[White][j+1] == 0) {
                    pawn_value -= 75;
                    if(pawn_structure[Black][j-1] || pawn_structure[Black][j+1]) pawn_value -= 25;
                }
            }

            pawn_struct_value = pawn_value;
        }

        // Make sure to update can_castle array based on rook movements.

        if(board[white_left_rook_home] != Wrook) can_castle[White][1] = False;
        if(board[white_right_rook_home] != Wrook) can_castle[White][2] = False;
        if(board[black_left_rook_home] != Brook) can_castle[Black][1] = False;
        if(board[black_right_rook_home] != Brook) can_castle[Black][2] = False;

        // Set flag for vulnerable to en pesant if pawn moved out two.

        if(board[move->to_loc] & pawn_mask) {
            rank_diff = rank_number(move->to_loc) - rank_number(move->from_loc);
            if(abs_v(rank_diff) == 2) vuln_ep[mover][file_number(move->to_loc)] = True;
        }

        // Deal with castling.

        if(board[move->to_loc] & king_mask) {
            if(abs_v(file_number(move->to_loc) - file_number(move->from_loc)) == 2) {
                castled[mover] = True;
            }
            can_castle[mover][1] = False;
            can_castle[mover][2] = False;
        }

        if(mover == White) mover = Black;
        else mover = White;
        SET_MOVER(mover);

        // Clear EP array for side to move next since no longer vulnerable.

        for(i = 0; i <= 9; i++) vuln_ep[mover][i] = False;

        // If this debug flag is set, make sure all the information in the position record
        // is correct.  Since many of the things here are kept track of as moves are made,
        // this will check to make sure that that is being done correctly after each move.

        if(glob__check_data_after_move) {

            // Statically calculate all of the extra data from a copy of the current position.

            debug_posit = new_position();
            *debug_posit = *this;
            debug_posit->calculate_auxillary_board_data();

            // Check to make sure the square attacked data is correct.

            for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
                for(j = i; j < i + (next_u_file * 8); j += next_u_file) {
                    if(debug_posit->white_attackers[j] != white_attackers[j]) {
                        print_board(this);
                        cout << "====White attackers is wrong for square " << j << '\n';
                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                        cout << "Value should be " << debug_posit->white_attackers[j] << " and value is " << white_attackers[j] << '\n';
                    }
                    if(debug_posit->white_wimpiest_attacker[j] != white_wimpiest_attacker[j]) {
                        print_board(this);
                        cout << "====White wimpiest is wrong for square " << j << '\n';
                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                        cout << "Value should be " << debug_posit->white_wimpiest_attacker[j] << " and value is " << white_wimpiest_attacker[j] << '\n';
                    }
                    if(debug_posit->black_attackers[j] != black_attackers[j]) {
                        print_board(this);
                        cout << "====Black attackers is wrong for square " << j << '\n';
                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                        cout << "Value should be " << debug_posit->black_attackers[j] << " and value is " << black_attackers[j] << '\n';
                    }
                    if(debug_posit->black_wimpiest_attacker[j] != black_wimpiest_attacker[j]) {
                        print_board(this);
                        cout << "====Black wimpiest is wrong for square " << j << '\n';
                        cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                        cout << "Value should be " << debug_posit->black_wimpiest_attacker[j] << " and value is " << black_wimpiest_attacker[j] << '\n';
                    }
                }
            }

            // Compare piece value data.

            if(debug_posit->total_piece_value[White] != total_piece_value[White]) {
                print_board(this);
                cout << "====White total piece value is wrong.\n";
                cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                cout << "Value should be " << debug_posit->total_piece_value[White] << " and value is " << total_piece_value[White] << '\n';
            }
            if(debug_posit->total_piece_value[Black] != total_piece_value[Black]) {
                print_board(this);
                cout << "====Black total piece value is wrong.\n";
                cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                cout << "Value should be " << debug_posit->total_piece_value[Black] << " and value is " << total_piece_value[Black] << '\n';
            }

            // Compare pawn structure values.

            for(i = 1; i <= 8; i++) {
                if(debug_posit->pawn_structure[White][i] != pawn_structure[White][i]) {
                    print_board(this);
                    cout << "====White pawn structure is wrong for entry " << i << '\n';
                    cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                    cout << "Value should be " << debug_posit->pawn_structure[White][i] << " and value is " << pawn_structure[White][i] << '\n';
                }
                if(debug_posit->pawn_structure[Black][i] != pawn_structure[Black][i]) {
                    print_board(this);
                    cout << "====Black pawn structure is wrong for entry " << i << '\n';
                    cout << "Move was from " << from_loc << " to " << to_loc << ".\n";
                    cout << "Value should be " << debug_posit->pawn_structure[Black][i] << " and value is " << pawn_structure[Black][i] << '\n';
                }
            }

            if(pawn_struct_value != debug_posit->pawn_struct_value) {
                cout << "====Pawn structure value is wrong.\n";
            }

            // Get rid of temporary position record.

            delete_position(debug_posit);
        }

        if(Debug12) {

            // Check to make sure that the hash board is consistent with the real board.

            CONSISTENCY_CHECK(this);
        }

        if(Debug15) {

            // Check consistency of piece_locations array.

            compare_piece_locations();
        }
    }


int Position_type::piece_value(
        bd_index        index   // The index into the board.
    )
    // Returns the value of the piece at the given location.  A piece may
    // be worth different amounts on different squares.
    {
        switch(board[index]) {
            case Wpawn   :
                return(white_pawn_values[index]);
            case Bpawn   :
                return(black_pawn_values[index]);
            case Wknight :
            case Bknight :
                return(knight_values[index]);
            case Wbishop :
            case Bbishop :
                return(bishop_values[index]);
            case Wrook   :
            case Brook   :
                return(500);
            case Wqueen  :
            case Bqueen  :
                return(queen_values[index]);
            case Wking   :
            case Bking   :
                return(0);
            default      : cerr << "Bad value to piece_value. value was: " << board[index] << "\n";
        }
        return(0);
    }


int Position_type::check_for_legal_move(
        bd_index       from_loc, // The location to move from.
        bd_index       to_loc    // The location to move to.
    )
    // This routine will will check the given move for legality in
    // this position.
    {
        square          friendly;       // Holds mask for friendly pieces.
        int             legal = False;  // Set to true if this is a legal move.
        Gen_move_type  *temp_move;      // Points to a move on the list while processing.


        // Make sure we are moving the right color piece.

        if(mover == White) {
            if(!(board[from_loc] & white_mask)) return(False);
        } else {
            if(!(board[from_loc] & black_mask)) return(False);
        }

        // Initialize move holders.

        moves_priority_1 = NULL;
        moves_priority_2 = NULL;
        moves_priority_3 = NULL;
        moves_priority_4 = NULL;
        moves_priority_5 = NULL;

        // Generate all legal moves for the piece moving.

        set_piece_sets(mover, &friendly, &enemy_mask);

        piece_loc = from_loc;
        switch(board[from_loc]) {
            case Wpawn :
            case Bpawn :
                pawn_moves(); break;
            case Wknight :
            case Bknight :
                knight_moves(); break;
            case Wbishop :
            case Bbishop :
                bishop_moves(); break;
            case Wrook :
            case Brook :
                rook_moves(); break;
            case Wqueen :
            case Bqueen :
                queen_moves(); break;
            case Wking :
            case Bking :
                king_moves(); break;
            default :
                break;
        }

        append_move_list();

        if(glob__print_moves_generated) print_move_list(this, moves_all);

        // See if the move is on the list of ones generated for the piece.

        for(temp_move = moves_all; temp_move; temp_move = temp_move->next) {
            if(to_loc == temp_move->to_loc) {

                // Check to make sure that the king won't be in check after move.

                if(!will_king_be_in_check(from_loc, to_loc)) legal = True;
                break;
            }
        }

        // Clear the moves generated.

        free_list(moves_all);

        return(legal);
    }


int Position_type::king_in_check()
    // This routine will look to see if the king of the side to move
    // is currently in check.
    {
        if(mover == White) {
            if(black_attackers[king_loc[mover]]) return(True);
        } else {
            if(white_attackers[king_loc[mover]]) return(True);
        }
        return(False);
    }


void Position_type::setup_board()
    // This function will set up the board to it's starting position,
    // and it will initialize all of the other data in the class to
    // correspond to the initial position.
    {
        int    i,j;  // Counters.


        mover = White;

        // Initialize squares on board with pieces in their starting
        // locations.  Initialize squares outside boarder to Out.

        for(i = 0; i < total_files * total_ranks; i++) {
            board[i] = Out;
        }

        board[single_index_from_double(1,1)] = Wrook;
        board[single_index_from_double(2,1)] = Wknight;
        board[single_index_from_double(3,1)] = Wbishop;
        board[single_index_from_double(4,1)] = Wqueen;
        board[single_index_from_double(5,1)] = Wking;
        board[single_index_from_double(6,1)] = Wbishop;
        board[single_index_from_double(7,1)] = Wknight;
        board[single_index_from_double(8,1)] = Wrook;

        board[single_index_from_double(1,8)] = Brook;
        board[single_index_from_double(2,8)] = Bknight;
        board[single_index_from_double(3,8)] = Bbishop;
        board[single_index_from_double(4,8)] = Bqueen;
        board[single_index_from_double(5,8)] = Bking;
        board[single_index_from_double(6,8)] = Bbishop;
        board[single_index_from_double(7,8)] = Bknight;
        board[single_index_from_double(8,8)] = Brook;

        for(i = 1; i <= 8; i++) {
            board[single_index_from_double(i,2)] = Wpawn;
            for(j = 3; j <= 6; j++) board[single_index_from_double(i,j)] = Empty;
            board[single_index_from_double(i,7)] = Bpawn;
        }

        // Initialize castling information.

        can_castle[White][1] = True;
        can_castle[White][2] = True;
        can_castle[Black][1] = True;
        can_castle[Black][2] = True;
        castled[White]       = False;
        castled[Black]       = False;

        // Initialize array showing vulnerability to en pesant.

        for(i = 0; i <= 9; i++) {
            vuln_ep[White][i] = False;
            vuln_ep[Black][i] = False;
        }

        // Initialize king location.

        king_loc[White] = single_index_from_chars('e', '1');
        king_loc[Black] = single_index_from_chars('e', '8');

        // There are no repeated positions now.

        positions_repeated_twice = NULL;

        // Set up piece location array.

        set_piece_locations();

        // Update values and pawn structure.

        calculate_auxillary_board_data();

        // Set up the hash board.

        SET_LIKE_POSIT(this);
   }


void Position_type::set_up_position()
    // This function will alow the user to set up a position on the board.
    // After the position is set, all of the other information in the
    // class will be set to correspond to the position.
    {
        char  answer;    // Answer to question.
        char  code[90];  // The Forsyth code.
        char  color;     // The color next to move.
        int   good_pos;  // The error code.
        int   i;         // Counter.


        cout << "Set up a position on the board.\n\n\n\n\n\n";

        // Get the color to move.

        do {
            cout << "What color is next to move? (W or B) ";
            cin >> color;
            if(color == 'w') color = 'W';
            if(color == 'b') color = 'B';
            if(color != 'W' && color != 'B') {
                cout << "Bad input.  Try again.\n\n";
            }
        } while(color != 'W' && color != 'B');
        code[0] = color;

        // Get the Forsyth code.

        do {
            cout << "Input the Forsyth code.\n    Remember to start at the lower left corner of the board.\n";
            cin >> &code[1];
            good_pos = from_forsyth(code);
            if(good_pos) {
                cout << '\n';
                print_board(this);
                cout << "\nIs this correct?";
                cin >> answer;
                if(answer == 'N' || answer == 'n') good_pos = False;
            } else {
                cout << "Bad Forsyth code.  Try again.\n";
            }
        } while(!good_pos);

        // Find out who can castle to what side.

        can_castle[White][1] = False;
        can_castle[White][2] = False;
        can_castle[Black][1] = False;
        can_castle[Black][2] = False;

        if(board[single_index_from_chars('e','1')] == Wking) {
            if(board[single_index_from_chars('a','1')] == Wrook) {
                cout << "Can white castle on the queen's side? (Y or N) ";
                cin >> answer;
                if(answer == 'y' || answer == 'Y') can_castle[White][1] = True;
            }
            if(board[single_index_from_chars('h','1')] == Wrook) {
                cout << "Can white castle on the king's side? (Y or N) ";
                cin >> answer;
                if(answer == 'y' || answer == 'Y') can_castle[White][2] = True;
            }
        }
        if(board[single_index_from_chars('e','8')] == Bking) {
            if(board[single_index_from_chars('a','8')] == Brook) {
                cout << "Can black castle on the queen's side? (Y or N) ";
                cin >> answer;
                if(answer == 'y' || answer == 'Y') can_castle[Black][1] = True;
            }
            if(board[single_index_from_chars('h','8')] == Brook) {
                cout << "Can black castle on the king's side? (Y or N) ";
                cin >> answer;
                if(answer == 'y' || answer == 'Y') can_castle[Black][2] = True;
            }
        }

        // Set other variables.

        castled[White] = False;
        castled[Black] = False;
        for(i = 0; i <= 9; i++) {
            vuln_ep[White][i] = False;
            vuln_ep[Black][i] = False;
        }

        // Find kings.

        set_king_locs();

        // Set up piece location array.

        set_piece_locations();

        // Update values and pawn structure.

        calculate_auxillary_board_data();

        // There are no repeated positions now.

        positions_repeated_twice = NULL;

        glob__current_move = 1;

        // Set up the hash board.

        SET_LIKE_POSIT(this);
    }


#define increment_board_location(loc) \
                loc += next_r_rank; \
                if(loc == end_of_rank) { \
                    loc = first_square + next_u_file; \
                    if(loc > end_of_file) return(False); \
                    first_square = loc; \
                    end_of_rank  = loc + (next_r_rank * 8); \
                }


int Position_type::from_forsyth(
        char          *code    // The start of the Forsyth code.
    )
    // This procedure is sent a Forsyth code.  It will create the position
    // corresponding to that code.  If an error is found, 0 will
    // be returned, otherwise 1 will be returned indicating a legal position.
    // All of the other information in the class will be set to correspond
    // to the position.
    {
        int      black_king = False;   // Set to true when the black king is found.
        bd_index current_square;       // The current square as we walk through the board.
        bd_index end_of_file;          // The square after the last square in the first file.
        bd_index end_of_rank;          // The square after the last square in the current rank.
        bd_index first_square;         // The first square in the current rank.
        bd_index i, j;                 // Counters.
        int      skip;                 // The number of squares to skip now.
        int      white_king = False;   // Set to true when the black king is found.


        // Make sure the color to move next was put in.

        if(*code == 'W') mover = White;
        else if( *code == 'B') mover = Black;
        else return(False);
        code++;

        // Clear out the board to start with.

        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + (next_u_file * 8); j += next_u_file) board[j] = Empty;
        }

        // Walk through the Forsyth code and the board at the same time
        // checking for errors and filling in the board.

        current_square = white_left_rook_home;
        first_square   = current_square;

        // Set up board boundaries.

        end_of_file = current_square + (next_u_file * 8);
        end_of_rank = current_square + (next_r_rank * 8);

        while(*code) {

            skip = 0;

            // If we have a number, then we will skip that many spaces.

            while(*code >= '0' && *code <= '9') {
                skip = skip * 10 + (*code++ - '0');
            }
            while(skip--) {
                increment_board_location(current_square);
            }

            // Here we handle a piece.

            switch(*code++) {
                case 'P' : board[current_square] = Wpawn; break;
                case 'N' : board[current_square] = Wknight; break;
                case 'B' : board[current_square] = Wbishop; break;
                case 'R' : board[current_square] = Wrook; break;
                case 'Q' : board[current_square] = Wqueen; break;
                case 'K' : board[current_square] = Wking;
                           if(white_king) return(False);
                           white_king = True;
                           break;
                case 'p' : board[current_square] = Bpawn; break;
                case 'n' : board[current_square] = Bknight; break;
                case 'b' : board[current_square] = Bbishop; break;
                case 'r' : board[current_square] = Brook; break;
                case 'q' : board[current_square] = Bqueen; break;
                case 'k' : board[current_square] = Bking;
                           if(black_king) return(False);
                           black_king = True;
                           break;
                default  : return(False);
            }

            increment_board_location(current_square);
        }

        // Now go through and make sure there are no pawns on the first
        // or eightth ranks.

        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            if(board[i] == Wpawn || board[i] == Bpawn) return(False);
            if(board[i + (next_u_file * 7)] == Wpawn || board[i + (next_u_file * 7)] == Bpawn) return(False);
        }

        // If we got to here, then all is ok with the board.

        return(True);
    }
 

void Position_type::to_forsyth(
        char           *dest    // The start of the string to put the Forsyth representation.
    )
    // This function will convert the current position and the color to move
    // next to a compact notation known as Forsyth notation.  This is
    // written into the string pointed to by dest.
    {
        int       blank = 0;         // The number of blanks we have seen in a row.
        bd_index  i, j;              // Counters.
        char     *start_code = dest; // The beginning of the Forsyth notation.
        int       tens;              // Holds tens part of blanks.


        if(mover == White) *dest++ = 'W';
        else *dest++ = 'B';

        // Go through the board from the lower left corner generating the code.

        for(i = white_left_rook_home; i <= black_left_rook_home; i += next_u_file) {
            for(j = i; j <= i + (next_r_rank * 7); j += next_r_rank) {
                if(board[j] == Empty) blank++;
                else {
                    if(blank > 0) {
                        if(blank > 9) {
                            tens = blank / 10;
                            *dest++ = '0' + tens;
                            blank = blank % 10;
                        }
                        *dest++ = '0' + blank;
                        blank = 0;
                    }
                    *dest++ = piece_code(board[j]);
                }
            }
        }
        *dest = '\0';

        if(glob__print_forsyth_debug) {
            print_board(this);
            cerr << "Forsyth code: " << start_code << '\n';
        }
    }


//
// These functions implement a free list for positions.
//

Position_type *new_position()
    // Returns a pointer to a new position.
    // A free list is used to minimize time spent in malloc.
    {
        Position_type *new_posit;


        if(pos_free_list) {
            new_posit = pos_free_list;
            pos_free_list = new_posit->next;
        } else {
            new_posit = new Position_type;
        }
        return(new_posit);
    }

void delete_position(Position_type *posit)
    // Puts this position on the free list for later use.
    {
        posit->next = pos_free_list;
        pos_free_list = posit;
    }



//
// These functions are private to the class.
//


void Position_type::set_piece_locations(
    )
    // This routine will set the values of the piece_locations array based
    // on the current position.  We try to order them well.
    {
        int        b_index = 0;
        bd_index   i, j;
        int        w_index = 0;


        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wbishop) piece_locations[White][w_index++] = j;
                if(board[j] == Bbishop) piece_locations[Black][b_index++] = j;
            }
        }
        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wknight) piece_locations[White][w_index++] = j;
                if(board[j] == Bknight) piece_locations[Black][b_index++] = j;
            }
        }
        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wqueen) piece_locations[White][w_index++] = j;
                if(board[j] == Bqueen) piece_locations[Black][b_index++] = j;
            }
        }
        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wrook) piece_locations[White][w_index++] = j;
                if(board[j] == Brook) piece_locations[Black][b_index++] = j;
            }
        }
        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wpawn) piece_locations[White][w_index++] = j;
                if(board[j] == Bpawn) piece_locations[Black][b_index++] = j;
            }
        }
        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wking) piece_locations[White][w_index++] = j;
                if(board[j] == Bking) piece_locations[Black][b_index++] = j;
            }
        }
        piece_locations[White][w_index] = 0;
        piece_locations[Black][b_index] = 0;
    }


void Position_type::compare_piece_locations(
    )
    // This function will make sure the piece_locations array matches the board.
    {
        int        found;
        bd_index   i, j, k;             // Counters.
        int        b_count = 0;
        int        w_count = 0;


        // Make sure each piece on the board is in the piece_locations array.

        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] & white_mask) {
                    for(k = 0, found = 0; piece_locations[White][k] && !found; k++) {
                        if(piece_locations[White][k] == j) {
                            found = 1;
                            w_count++;
                        }
                    }
                    if(!found) {
                        cerr << "Didn't find piece at " << (char) file_as_char(j) << (char) rank_as_char(j) << " in this board:\n";
                        print_board(this);
                    }
                }
                if(board[j] & black_mask) {
                    for(k = 0, found = 0; piece_locations[Black][k] && !found; k++) {
                        if(piece_locations[Black][k] == j) {
                            found = 1;
                            b_count++;
                        }
                    }
                    if(!found) {
                        cerr << "Didn't find piece at " << (char) file_as_char(j) << (char) rank_as_char(j) << " in this board:\n";
                        print_board(this);
                    }
                }
            }
        }

        // Now make sure we saw as many pieces on the board as are in the arrays.

        for(k = 0; piece_locations[White][k]; k++) ;
        if(w_count != k) {
            cerr << "White piece count in piece_locations array is not correct.  Here is the board:\n";
            print_board(this);
        }
        for(k = 0; piece_locations[Black][k]; k++) ;
        if(b_count != k) {
            cerr << "Black piece count in piece_locations array is not correct.  Here is the board:\n";
            print_board(this);
        }
    }


void Position_type::piece_loc_move_piece(
        char     color,   // Color of the piece to move.
        bd_index from,    // The location to move it from.
        bd_index to       // The location to move it to.
    )
    // This function will move a piece in the piece_location array.
    {
        int i;


        for(i = 0; piece_locations[color][i]; i++) {
            if(piece_locations[color][i] == from) {
                piece_locations[color][i] = to;
                return;
            }
        }
    }


void Position_type::piece_loc_remove_piece(
        char     color,
        bd_index loc
    )
    // This function will remove a piece from the piece_location array.
    {
        int i;


        // Bubble up the part of the array under the piece removed.

        for(i = 0; piece_locations[color][i]; i++) {
            if(piece_locations[color][i] == loc) {
                for(i++; piece_locations[color][i]; i++) {
                    piece_locations[color][i - 1] = piece_locations[color][i];
                }
                piece_locations[color][i - 1] = 0;
                return;
            }
        }
    }


void Position_type::set_king_locs(
    )
    // This function will set the king locations on the current board.
    // The entire board is scanned to do it.
    {
        bd_index   i, j;             // Counters.


        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + 8; j += next_u_file) {
                if(board[j] == Wking) king_loc[White] = j;
                if(board[j] == Bking) king_loc[Black] = j;
            }
        }
    }


//void Position_type::calculate_auxillary_board_data()
//    // This routine will calculate the information in this class that
//    // is supplementary to the board, the castling information, and
//    // the information on vulnerability to en pesant.  It uses the
//    // current board position to calculate total piece value, pawn
//    // structure, black and white attackers for each square, and
//    // least valuable direct attacker for each square.
//    {
//        square        curr_sqr_contents;   // The contents of the current square.
//        bd_index      i, j;                // Counters.
//        int           pawn_value;          // Holds the temporary pawn structure value.
//
//
//        // Initialize this data to 0 to be filled in later.
//
//        total_piece_value[White] = 0;
//        total_piece_value[Black] = 0;
//
//        for(j = 0; j <= 9; j++) {
//            pawn_structure[White][j] = 0;
//            pawn_structure[Black][j] = 0;
//        }
//
//        // Go through board filling in piece value, pawn structure and attackers.
//
//        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
//            for(j = i; j < i + (next_u_file * 8); j += next_u_file) {
//
//                curr_sqr_contents = board[j];
//
//                // Add to total value of white or black pieces if this is a piece.
//
//                if(curr_sqr_contents && !(curr_sqr_contents & king_mask)) {
//                    if(curr_sqr_contents & white_mask) {
//                        total_piece_value[White] += piece_value(j);
//                    } else {
//                        total_piece_value[Black] += piece_value(j);
//                    }
//                }
//
//                // Keep track of pawn structure.
//
//                if(curr_sqr_contents == Wpawn) pawn_location[White][file_number(j)][pawn_structure[White][file_number(j)]++] = j;
//                if(curr_sqr_contents == Bpawn) pawn_location[Black][file_number(j)][pawn_structure[Black][file_number(j)]++] = j;
//
//                // Figure out who is attacking this square.
//                
//                white_attackers[j] = square_attacked_count(j, White, &white_wimpiest_attacker[j]);
//                black_attackers[j] = square_attacked_count(j, Black, &black_wimpiest_attacker[j]);
//            }
//        }
//
//        // Fill in pawn_struct_value;
//
//        pawn_value = 0;
//
//        for(j = 1; j <= 8; j++) {
//
//            // Subtract for doubles pawns.
//
//            if(pawn_structure[White][j] > 1) pawn_value -= pawn_structure[White][j]*15;
//            if(pawn_structure[Black][j] > 1) pawn_value += pawn_structure[Black][j]*15;
//
//            // Subtract for isolated pawns.
//
//            if(pawn_structure[White][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j+1] == 0)
//                pawn_value -= pawn_structure[White][j]*25;
//            if(pawn_structure[Black][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j+1] == 0)
//                pawn_value += pawn_structure[Black][j]*25;
//
//            // Add for passed pawns.
//
//            if(pawn_structure[White][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j] == 0 && pawn_structure[Black][j+1] == 0)
//                pawn_value += 50;
//            if(pawn_structure[Black][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j] == 0 && pawn_structure[White][j+1] == 0)
//                pawn_value -= 50;
//        }
//
//        pawn_struct_value = pawn_value;
//    }
void Position_type::calculate_auxillary_board_data()
    // This routine will calculate the information in this class that
    // is supplementary to the board, the castling information, and
    // the information on vulnerability to en pesant.  It uses the
    // current board position to calculate total piece value, pawn
    // structure, black and white attackers for each square, and
    // least valuable direct attacker for each square.
    {
        square        curr_sqr_contents;   // The contents of the current square.
        bd_index      i, j;                // Counters.
        int           pawn_value;          // Holds the temporary pawn structure value.


        // Initialize this data to 0 to be filled in later.

        total_piece_value[White] = 0;
        total_piece_value[Black] = 0;

        for(j = 0; j <= 9; j++) {
            pawn_structure[White][j] = 0;
            pawn_structure[Black][j] = 0;
        }

        // Go through board filling in piece value, pawn structure and attackers.

        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + (next_u_file * 8); j += next_u_file) {

                curr_sqr_contents = board[j];

                // Add to total value of white or black pieces if this is a piece.

                if(curr_sqr_contents && !(curr_sqr_contents & king_mask)) {
                    if(curr_sqr_contents & white_mask) {
                        total_piece_value[White] += piece_value(j);
                    } else {
                        total_piece_value[Black] += piece_value(j);
                    }
                }

                // Keep track of pawn structure.

                if(curr_sqr_contents == Wpawn) pawn_structure[White][file_number(j)]++;
                if(curr_sqr_contents == Bpawn) pawn_structure[Black][file_number(j)]++;

                // Figure out who is attacking this square.
                
                white_attackers[j] = square_attacked_count(j, White, &white_wimpiest_attacker[j]);
                black_attackers[j] = square_attacked_count(j, Black, &black_wimpiest_attacker[j]);
            }
        }

        // Fill in pawn_struct_value;

        pawn_value = 0;

        for(j = 1; j <= 8; j++) {

            // Subtract for doubles pawns.

            if(pawn_structure[White][j] > 1) pawn_value -= pawn_structure[White][j]*15;
            if(pawn_structure[Black][j] > 1) pawn_value += pawn_structure[Black][j]*15;

            // Subtract for isolated pawns.

            if(pawn_structure[White][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j+1] == 0)
                pawn_value -= pawn_structure[White][j]*25;
            if(pawn_structure[Black][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j+1] == 0)
                pawn_value += pawn_structure[Black][j]*25;

            // Add for passed pawns.

            if(pawn_structure[White][j] > 0 && pawn_structure[Black][j-1] == 0 && pawn_structure[Black][j] == 0 && pawn_structure[Black][j+1] == 0) {
                pawn_value += 75;
                if(pawn_structure[White][j-1] || pawn_structure[White][j+1]) pawn_value += 25;
            }
            if(pawn_structure[Black][j] > 0 && pawn_structure[White][j-1] == 0 && pawn_structure[White][j] == 0 && pawn_structure[White][j+1] == 0) {
                pawn_value -= 75;
                if(pawn_structure[Black][j-1] || pawn_structure[Black][j+1]) pawn_value -= 25;
            }
        }

        pawn_struct_value = pawn_value;
    }


int Position_type::square_attacked_bool(
        bd_index       loc,    // The location of the square to check.
        char           enemy   // The enemy color.
    )
    // This function will return True if the square denoted by loc is 
    // attacked by a piece of the enemy color on the board.
    // This function is similar to square_attacked_count, except that this
    // one returns True or False, and the other returns a count of the
    // number of pieces attacking.  This routine is slightly faster if
    // all that is wanted is a boolean value.
    {
        square   attack_king;    // The attacking king.
        square   attack_knight;  // The attacking knight.
        square   attackers_diag; // Mask for pieces attacking this square diagonally.
        square   attackers_rf;   // Mask for pieces attacking this square horizontally or vertically.
        int      i;              // Counter.
        bd_index new_loc;        // Temporary location.
        square   terminate_diag; // Mask for terminating square with diagonal search.
        square   terminate_rf;   // Mask for terminating square with rank and file search.


        // Set up masks for searching for attacking pieces.
        // Also check for attacking pawns.

        if(enemy == White) {

            attack_king    = Wking;
            attack_knight  = Wknight;
            attackers_rf   = Wrook | Wqueen;
            terminate_rf   = Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            attackers_diag = Wbishop | Wqueen;
            terminate_diag = Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;

            // See if attacked by a pawn.

            if(board[loc + next_ld_diag] == Wpawn) return(True);
            if(board[loc + next_rd_diag] == Wpawn) return(True);
        } else {

            attack_king    = Bking;
            attack_knight  = Bknight;
            attackers_rf   = Brook | Bqueen;
            terminate_rf   = Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            attackers_diag = Bbishop | Bqueen;
            terminate_diag = Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;

            // See if attacked by a pawn.

            if(board[loc + next_lu_diag] == Bpawn) return(True);
            if(board[loc + next_ru_diag] == Bpawn) return(True);
        }

        // Look for attacking queens or rooks on same rank or file that are not blocked.
        // Look for attacking queens or bishops on same diagonal that are not blocked.

        for(i = 0; i <= 3; i++) {
            for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & terminate_rf); new_loc = new_loc + rook_next_moves[i])
                if(board[new_loc] & attackers_rf) return(True);
            for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & terminate_diag); new_loc = new_loc + bishop_next_moves[i])
                if(board[new_loc] & attackers_diag) return(True);
        }

        // Check for attacking knights.
        // Check for attacking king.

        for(i = 0; i <= 7; i++) {
            if(board[loc + knight_next_moves[i]] == attack_knight) return(True);
            if(board[loc + king_next_moves[i]] == attack_king) return(True);
        }

        return(False);
    }


// Define some constants for next routines.

#define WHITE_TERMINATE_RF   (Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking)
#define WHITE_TERMINATE_DIAG (Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking)

#define BLACK_TERMINATE_RF   (Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking)
#define BLACK_TERMINATE_DIAG (Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking)


int Position_type::square_attacked_count(
        bd_index       loc,       // The location of the square to check.
        char           enemy,     // The enemy color.
        square        *low_enemy  // Filled in with the least valuable direct attacker.
    )
    // This function will return the number of enemy pieces attacking the square
    // denoted by loc on the board.  This counts attacking pieces
    // behind other attacking pieces.  For example two rooks on a file count as
    // two attacking pieces along that file.  It will fill in the parameter
    // low_enemy to hold the least valuable direct attacker.
    // This function is similar to square_attacked_bool, except that this
    // one returns the number of attackers, and the other returns True or False.
    // This routine is made as efficient as possible because it is called a lot.
    {
        int      count = 0;      // The number of attackers.
        int      diag_found;     // Set to true when an attacker is found on this diagonal.
        int      i;              // Counter.
        square   weak_enemy;     // Holds weakest attacker seen so far.
        bd_index new_loc;        // Temporary location.
        int      rf_found;       // Set to true when an attacker is found on this rank or file.


        weak_enemy = Max_piece;

        // Set up masks for searching for attacking pieces.
        // Also check for attacking pawns.

        if(enemy == White) {

            // See if attacked by a pawn.

            if(board[loc + next_ld_diag] == Wpawn) {
                weak_enemy = Wpawn;
                count++;
            }
            if(board[loc + next_rd_diag] == Wpawn) {
                weak_enemy = Wpawn;
                count++;
            }
    
            // Look for attacking queens or rooks on same rank or file that are not blocked.
            // Look for attacking queens or bishops on same diagonal that are not blocked.
    
            for(i = 0; i <= 3; i++) {
                rf_found = False;
                for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & WHITE_TERMINATE_RF); new_loc = new_loc + rook_next_moves[i])
                    if(board[new_loc] & (Wrook | Wqueen)) {
                        if(!rf_found && board[new_loc] < weak_enemy) weak_enemy = board[new_loc];
                        rf_found = True;
                        count++;
                    }
                diag_found = False;
                for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & WHITE_TERMINATE_DIAG); new_loc = new_loc + bishop_next_moves[i])
                    if(board[new_loc] & (Wbishop | Wqueen)) {
                        if(!diag_found && board[new_loc] < weak_enemy) weak_enemy = board[new_loc];
                        diag_found = True;
                        count++;
                    }
            }
    
            // Check for attacking knights.
            // Check for attacking king.
    
            for(i = 0; i <= 7; i++) {
                if(board[loc + knight_next_moves[i]] == Wknight) {
                    if(Wknight < weak_enemy) weak_enemy = Wknight;
                    count++;
                }
                if(board[loc + king_next_moves[i]] == Wking) {
                    if(Wking < weak_enemy) weak_enemy = Wking;
                    count++;
                }
            }

        } else {

            // See if attacked by a pawn.

            if(board[loc + next_lu_diag] == Bpawn) {
                weak_enemy = Bpawn;
                count++;
            }
            if(board[loc + next_ru_diag] == Bpawn) {
                weak_enemy = Bpawn;
                count++;
            }

            // Look for attacking queens or rooks on same rank or file that are not blocked.
            // Look for attacking queens or bishops on same diagonal that are not blocked.
    
            for(i = 0; i <= 3; i++) {
                rf_found = False;
                for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & BLACK_TERMINATE_RF); new_loc = new_loc + rook_next_moves[i])
                    if(board[new_loc] & (Brook | Bqueen)) {
                        if(!rf_found && board[new_loc] < weak_enemy) weak_enemy = board[new_loc];
                        rf_found = True;
                        count++;
                    }
                diag_found = False;
                for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & BLACK_TERMINATE_DIAG); new_loc = new_loc + bishop_next_moves[i])
                    if(board[new_loc] & (Bbishop | Bqueen)) {
                        if(!diag_found && board[new_loc] < weak_enemy) weak_enemy = board[new_loc];
                        diag_found = True;
                        count++;
                    }
            }
    
            // Check for attacking knights.
            // Check for attacking king.
    
            for(i = 0; i <= 7; i++) {
                if(board[loc + knight_next_moves[i]] == Bknight) {
                    if(Bknight < weak_enemy) weak_enemy = Bknight;
                    count++;
                }
                if(board[loc + king_next_moves[i]] == Bking) {
                    if(Bking < weak_enemy) weak_enemy = Bking;
                    count++;
                }
            }

        }

        *low_enemy = weak_enemy;
        return(count);
    }


void Position_type::update_square_attacked_add_piece(
        bd_index       loc     // The location of the piece to be added.
    )
    // This routine will update the squares_attacked arrays for the addition
    // of the given piece.  It is assumed that the piece has already been
    // placed on the board.  It will update the number of attackers to any
    // square affected, and it will update the wimpiest piece to these squares
    // if they are not blocked.
    {
        square        attackers_diag;       // Mask for pieces attacking this square diagonally.
        square        attackers_enemy_diag; // Mask for enemy pieces attacking this square diagonally.
        square        attackers_rf;         // Mask for pieces attacking this square horizontally or vertically.
        square        attackers_enemy_rf;   // Mask for enemy pieces attacking this square horizontally or vertically.
        int           blocked;              // Set to true when piece blocked so wimpiest shouldn't be updated.
        int           blocked_earlier;      // Set to true when piece blocked behind added piece so wimpiest shouldn't be updated.
        square        contents;             // Holds the contents of the square moving.
        char          color;                // The color of the piece moving.
        char          enemy_color;          // The color of the enemy of the piece moving.
        char         *enemy_attackers;      // Points to square attacked array for enemy side.
        square       *enemy_wimpiest;       // Points to square attacked array for enemy side.
        int           i;                    // Counter.
        bd_index      new_loc;              // Temporary location.
        int           queen_moved = False;  // Set to true if a queen was put down, so don't count bishops or rooks behind.
        int           opp_i;                // Opposit index.
        bd_index      opposit_loc;          // Temporary location.
        int           pawn_left_catpture;   // Direction for a left pawn capture.
        int           pawn_right_catpture;  // Direction for a right pawn capture.
        square        terminate_rf;         // Mask for terminating square with rank and file search.
        square        terminate_enemy_rf;   // Mask for terminating square with enemy rank and file search.
        square        terminate_diag;       // Mask for terminating square with diagonal search.
        square        terminate_enemy_diag; // Mask for terminating square with enemy diagonal search.
        char         *this_color_attackers; // Points to square attacked array for the side of the piece being moved.
        square       *this_color_wimpiest;  // Points to square attacked array for the side of the piece being moved.
        

        contents = board[loc];

        if(contents & white_mask) {
            color = White;
            enemy_color = Black;
            pawn_right_catpture  = next_ru_diag;
            pawn_left_catpture   = next_lu_diag;
            this_color_attackers = white_attackers;
            enemy_attackers      = black_attackers;
            this_color_wimpiest  = white_wimpiest_attacker;
            enemy_wimpiest       = black_wimpiest_attacker;
            attackers_rf         = Wrook | Wqueen;
            attackers_enemy_rf   = Brook | Bqueen;
            terminate_rf         = Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            terminate_enemy_rf   = Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            attackers_diag       = Wbishop | Wqueen;
            attackers_enemy_diag = Bbishop | Bqueen;
            terminate_diag       = Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            terminate_enemy_diag = Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
        } else {
            color = Black;
            enemy_color = White;
            pawn_right_catpture  = next_rd_diag;
            pawn_left_catpture   = next_ld_diag;
            this_color_attackers = black_attackers;
            enemy_attackers      = white_attackers;
            this_color_wimpiest  = black_wimpiest_attacker;
            enemy_wimpiest       = white_wimpiest_attacker;
            attackers_rf         = Brook | Bqueen;
            attackers_enemy_rf   = Wrook | Wqueen;
            terminate_rf         = Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            terminate_enemy_rf   = Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            attackers_diag       = Bbishop | Bqueen;
            attackers_enemy_diag = Wbishop | Wqueen;
            terminate_diag       = Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            terminate_enemy_diag = Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
        }

        // First increment the squares that are attacked by this piece.
        // We also update the wimpiest piece that attacks the given square if this was the wimpiest.

        switch(contents) {
            case Wpawn :
            case Bpawn : {
                new_loc = loc + pawn_right_catpture;
                this_color_attackers[new_loc]++;
                if(contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                new_loc = loc + pawn_left_catpture;
                this_color_attackers[new_loc]++;
                if(contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                break;
            }
            case Wknight :
            case Bknight : {
                for(i = 0; i <= 7; i++) {
                    new_loc = loc + knight_next_moves[i];
                    this_color_attackers[new_loc]++;
                    if(contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                }
                break;
            }
            case Wking :
            case Bking : {
                for(i = 0; i <= 7; i++) {
                    new_loc = loc + king_next_moves[i];
                    this_color_attackers[loc + king_next_moves[i]]++;
                    if(contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                }
                break;
            }
            case Wbishop :
            case Bbishop : {
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & Out); new_loc += bishop_next_moves[i]) {
                        this_color_attackers[new_loc]++;
                        if(!blocked && contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                        if(board[new_loc] & terminate_diag) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                break;
            }
            case Wrook :
            case Brook : {
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & Out); new_loc += rook_next_moves[i]) {
                        this_color_attackers[new_loc]++;
                        if(!blocked && contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                        if(board[new_loc] & terminate_rf) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                break;
            }
            case Wqueen :
            case Bqueen : {
                queen_moved = True;
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & Out); new_loc += rook_next_moves[i]) {
                        this_color_attackers[new_loc]++;
                        if(!blocked && contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                        if(board[new_loc] & terminate_rf) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & Out); new_loc += bishop_next_moves[i]) {
                        this_color_attackers[new_loc]++;
                        if(!blocked && contents < this_color_wimpiest[new_loc]) this_color_wimpiest[new_loc] = contents;
                        if(board[new_loc] & terminate_diag) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                break;
            }
        }

        // Now update all of the pieces of the same color that attack through this square.

        // Here we look for bishops and queens.  It isn't necessary if the piece moved
        // is a bishop or queen because the pieces will attack through this piece.
        // We still may have to update wimpiest piece if a queen was put down.

        if(!(contents & bishop_mask)) {

            // Find any bishops or queens of this color that will be blocked.

            for(i = 0; i <= 3; i++) {
                blocked_earlier = False;
                for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & terminate_diag); new_loc += bishop_next_moves[i]) {
                    if(board[new_loc] & attackers_diag) {

                        // Now we start on the opposite diagonal of the piece added
                        // and update the squares not attacked by the bishop or queen any more.
                        // Also update the wimpiest piece attacking if necessary.

                        opp_i = (i > 1) ? i - 2 : i + 2;
                        blocked = blocked_earlier;
                        for(opposit_loc = loc + bishop_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += bishop_next_moves[opp_i]) {
                            if(!queen_moved) this_color_attackers[opposit_loc]--;
                            if(!blocked && board[new_loc] == this_color_wimpiest[opposit_loc]) {

                                // If there are no attackers now, then there isn't a wimpiest piece.

                                if(this_color_attackers[opposit_loc]) {
                                    square_attacked_count(opposit_loc, color, &this_color_wimpiest[opposit_loc]);
                                } else {
                                    this_color_wimpiest[opposit_loc] = Max_piece;
                                }
                            }
                            if(board[opposit_loc] & terminate_diag) break;
                            if(board[opposit_loc]) blocked = True;
                        }
                        blocked_earlier = True;
                    }
                }
            }
        }

        // Here we look for rooks and queens.  It isn't necessary if the piece moved
        // is a rook or queen because the pieces will attack through this piece.

        if(!(contents & rook_mask)) {

            // Find any rooks or queens of this color that will be blocked.

            for(i = 0; i <= 3; i++) {
                blocked_earlier = False;
                for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & terminate_rf); new_loc += rook_next_moves[i]) {
                    if(board[new_loc] & attackers_rf) {

                        // Now we start on the opposite rank or file of the piece added
                        // and update the squares not attacked by the bishop or queen any more.
                        // Also update the wimpiest piece attacking if necessary.

                        opp_i = (i > 1) ? i - 2 : i + 2;
                        blocked = blocked_earlier;
                        for(opposit_loc = loc + rook_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += rook_next_moves[opp_i]) {
                            if(!queen_moved) this_color_attackers[opposit_loc]--;
                            if(!blocked && board[new_loc] == this_color_wimpiest[opposit_loc]) {

                                // If there are no attackers now, then there isn't a wimpiest piece.

                                if(this_color_attackers[opposit_loc]) {
                                    square_attacked_count(opposit_loc, color, &this_color_wimpiest[opposit_loc]);
                                } else {
                                    this_color_wimpiest[opposit_loc] = Max_piece;
                                }
                            }
                            if(board[opposit_loc] & terminate_rf) break;
                            if(board[opposit_loc]) blocked = True;
                        }
                        blocked_earlier = True;
                    }
                }
            }
        }

        // Now update all of the pieces of the other color that attack through this square.

        // Here we look for bishops and queens.

        for(i = 0; i <= 3; i++) {

            // Find any bishops or queens of this color that will be blocked.

            blocked_earlier = False;
            for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & terminate_enemy_diag); new_loc += bishop_next_moves[i]) {
                if(board[new_loc] & attackers_enemy_diag) {

                    // Now we start on the opposite diagonal of the piece added
                    // and update the squares not attacked by the bishop or queen any more.
                    // Also update the wimpiest piece attacking if necessary.

                    opp_i = (i > 1) ? i - 2 : i + 2;
                    blocked = blocked_earlier;
                    for(opposit_loc = loc + bishop_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += bishop_next_moves[opp_i]) {
                        enemy_attackers[opposit_loc]--;
                        if(!blocked && board[new_loc] == enemy_wimpiest[opposit_loc]) {

                            // If there are no attackers now, then there isn't a wimpiest piece.

                            if(enemy_attackers[opposit_loc]) {
                                square_attacked_count(opposit_loc, enemy_color, &enemy_wimpiest[opposit_loc]);
                            } else {
                                enemy_wimpiest[opposit_loc] = Max_piece;
                            }
                        }
                        if(board[opposit_loc] & terminate_enemy_diag) break;
                        if(board[opposit_loc]) blocked = True;
                    }
                    blocked_earlier = True;
                }
            }
        }

        // Here we look for rooks and queens.

        for(i = 0; i <= 3; i++) {

            // Find any rooks or queens of this color that will be blocked.

            blocked_earlier = False;
            for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & terminate_enemy_rf); new_loc += rook_next_moves[i]) {
                if(board[new_loc] & attackers_enemy_rf) {

                    // Now we start on the opposite rank or file of the piece added
                    // and update the squares not attacked by the bishop or queen any more.
                    // Also update the wimpiest piece attacking if necessary.

                    opp_i = (i > 1) ? i - 2 : i + 2;
                    blocked = blocked_earlier;
                    for(opposit_loc = loc + rook_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += rook_next_moves[opp_i]) {
                        enemy_attackers[opposit_loc]--;
                        if(!blocked && board[new_loc] == enemy_wimpiest[opposit_loc]) {

                            // If there are no attackers now, then there isn't a wimpiest piece.

                            if(enemy_attackers[opposit_loc]) {
                                square_attacked_count(opposit_loc, enemy_color, &enemy_wimpiest[opposit_loc]);
                            } else {
                                enemy_wimpiest[opposit_loc] = Max_piece;
                            }
                        }
                        if(board[opposit_loc] & terminate_enemy_rf) break;
                        if(board[opposit_loc]) blocked = True;
                    }
                    blocked_earlier = True;
                }
            }
        }
    }


void Position_type::update_square_attacked_remove_piece(
        bd_index       loc     // The location of the piece to be removed.
    )
    // This routine will update the squares_attacked arrays for the removal
    // of the given piece.  It should be called before the piece is removed
    // from the board.  It will update the number of attackers to any
    // square affected, and it will update the wimpiest piece to these squares
    // if they are not blocked.
    {
        square        attackers_diag;       // Mask for pieces attacking this square diagonally.
        square        attackers_enemy_diag; // Mask for enemy pieces attacking this square diagonally.
        square        attackers_rf;         // Mask for pieces attacking this square horizontally or vertically.
        square        attackers_enemy_rf;   // Mask for enemy pieces attacking this square horizontally or vertically.
        int           blocked;              // Set to true when piece blocked so wimpiest shouldn't be updated.
        int           blocked_earlier;      // Set to true when piece blocked behind added piece so wimpiest shouldn't be updated.
        char          color;                // The color of the piece moving.
        square        contents;             // Holds the contents of the square moving.
        char         *enemy_attackers;      // Points to square attacked array for enemy side.
        square       *enemy_wimpiest;       // Points to square attacked array for enemy side.
        int           i;                    // Counter.
        bd_index      new_loc;              // Temporary location.
        int           queen_moved = False;  // Set to true if a queen was put down, so don't count bishops or rooks behind.
        int           opp_i;                // Opposit index.
        bd_index      opposit_loc;          // Temporary location.
        int           pawn_left_catpture;   // Direction for a left pawn capture.
        int           pawn_right_catpture;  // Direction for a right pawn capture.
        square        terminate_rf;         // Mask for terminating square with rank and file search.
        square        terminate_enemy_rf;   // Mask for terminating square with enemy rank and file search.
        square        terminate_diag;       // Mask for terminating square with diagonal search.
        square        terminate_enemy_diag; // Mask for terminating square with enemy diagonal search.
        char         *this_color_attackers; // Points to square attacked array for the side of the piece being moved.
        square       *this_color_wimpiest;  // Points to square attacked array for the side of the piece being moved.
        

        if(board[loc] & white_mask) {
            color = White;
            pawn_right_catpture  = next_ru_diag;
            pawn_left_catpture   = next_lu_diag;
            this_color_attackers = white_attackers;
            enemy_attackers      = black_attackers;
            this_color_wimpiest  = white_wimpiest_attacker;
            enemy_wimpiest       = black_wimpiest_attacker;
            attackers_rf         = Wrook | Wqueen;
            attackers_enemy_rf   = Brook | Bqueen;
            terminate_rf         = Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            terminate_enemy_rf   = Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            attackers_diag       = Wbishop | Wqueen;
            attackers_enemy_diag = Bbishop | Bqueen;
            terminate_diag       = Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            terminate_enemy_diag = Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
        } else {
            color = Black;
            pawn_right_catpture  = next_rd_diag;
            pawn_left_catpture   = next_ld_diag;
            this_color_attackers = black_attackers;
            enemy_attackers      = white_attackers;
            this_color_wimpiest  = black_wimpiest_attacker;
            enemy_wimpiest       = white_wimpiest_attacker;
            attackers_rf         = Brook | Bqueen;
            attackers_enemy_rf   = Wrook | Wqueen;
            terminate_rf         = Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            terminate_enemy_rf   = Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            attackers_diag       = Bbishop | Bqueen;
            attackers_enemy_diag = Wbishop | Wqueen;
            terminate_diag       = Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            terminate_enemy_diag = Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
        }

        // We have to clear the square so calls to square_attacked_count work out right.
        // It is replaced at the end of this routine.

        contents = board[loc];
        board[loc] = Empty;

        // First decrement the squares that are attacked by this piece.
        // We also update the wimpiest piece that attacks the given square if this was the wimpiest.

        switch(contents) {
            case Wpawn :
            case Bpawn : {
                new_loc = loc + pawn_right_catpture;
                this_color_attackers[new_loc]--;

                // If there are no attackers now, then there isn't a wimpiest piece.

                if(this_color_attackers[new_loc]) {
                    square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                } else {
                    this_color_wimpiest[new_loc] = Max_piece;
                }
                new_loc = loc + pawn_left_catpture;
                this_color_attackers[new_loc]--;

                // If there are no attackers now, then there isn't a wimpiest piece.

                if(this_color_attackers[new_loc]) {
                    square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                } else {
                    this_color_wimpiest[new_loc] = Max_piece;
                }
                break;
            }
            case Wknight :
            case Bknight : {
                for(i = 0; i <= 7; i++) {
                    new_loc = loc + knight_next_moves[i];
                    this_color_attackers[new_loc]--;

                    if(contents == this_color_wimpiest[new_loc]) {

                        // If there are no attackers now, then there isn't a wimpiest piece.

                        if(this_color_attackers[new_loc]) {
                            square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                        } else {
                            this_color_wimpiest[new_loc] = Max_piece;
                        }
                    }
                }
                break;
            }
            case Wking :
            case Bking : {
                for(i = 0; i <= 7; i++) {
                    new_loc = loc + king_next_moves[i];
                    this_color_attackers[new_loc]--;

                    if(contents == this_color_wimpiest[new_loc]) {

                        // If there are no attackers now, then there isn't a wimpiest piece.

                        if(this_color_attackers[new_loc]) {
                            square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                        } else {
                            this_color_wimpiest[new_loc] = Max_piece;
                        }
                    }
                }
                break;
            }
            case Wbishop :
            case Bbishop : {
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & Out); new_loc += bishop_next_moves[i]) {
                        this_color_attackers[new_loc]--;
                        if(!blocked && contents == this_color_wimpiest[new_loc]) {

                            // If there are no attackers now, then there isn't a wimpiest piece.

                            if(this_color_attackers[new_loc]) {
                                square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                            } else {
                                this_color_wimpiest[new_loc] = Max_piece;
                            }
                        }
                        if(board[new_loc] & terminate_diag) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                break;
            }
            case Wrook :
            case Brook : {
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & Out); new_loc += rook_next_moves[i]) {
                        this_color_attackers[new_loc]--;
                        if(!blocked && contents == this_color_wimpiest[new_loc]) {

                            // If there are no attackers now, then there isn't a wimpiest piece.

                            if(this_color_attackers[new_loc]) {
                                square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                            } else {
                                this_color_wimpiest[new_loc] = Max_piece;
                            }
                        }
                        if(board[new_loc] & terminate_rf) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                break;
            }
            case Wqueen :
            case Bqueen : {
                queen_moved = True;
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & Out); new_loc += rook_next_moves[i]) {
                        this_color_attackers[new_loc]--;
                        if(!blocked && contents == this_color_wimpiest[new_loc]) {

                            // If there are no attackers now, then there isn't a wimpiest piece.

                            if(this_color_attackers[new_loc]) {
                                square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                            } else {
                                this_color_wimpiest[new_loc] = Max_piece;
                            }
                        }
                        if(board[new_loc] & terminate_rf) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                for(i = 0; i <= 3; i++) {
                    blocked = False;
                    for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & Out); new_loc += bishop_next_moves[i]) {
                        this_color_attackers[new_loc]--;
                        if(!blocked && contents == this_color_wimpiest[new_loc]) {

                            // If there are no attackers now, then there isn't a wimpiest piece.

                            if(this_color_attackers[new_loc]) {
                                square_attacked_count(new_loc, color, &this_color_wimpiest[new_loc]);
                            } else {
                                this_color_wimpiest[new_loc] = Max_piece;
                            }
                        }
                        if(board[new_loc] & terminate_diag) break;
                        if(board[new_loc]) blocked = True;
                    }
                }
                break;
            }
        }

        // Now update all of the pieces of the same color that attack through this square.

        // Here we look for bishops and queens.  It isn't necessary if the piece moving
        // is a bishop or queen because the attacks would have been included before.
        // We still may have to update wimpiest piece if a queen was put down.

        if(!(contents & bishop_mask)) {

            // Find any bishops or queens of this color that this piece blocked.

            for(i = 0; i <= 3; i++) {
                blocked_earlier = False;
                for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & terminate_diag); new_loc += bishop_next_moves[i]) {
                    if(board[new_loc] & attackers_diag) {

                        // Now we start on the opposite diagonal of the piece removed
                        // and update the squares now attacked by the bishop or queen.
                        // Also update the wimpiest piece attacking if necessary.

                        opp_i = (i > 1) ? i - 2 : i + 2;
                        blocked = blocked_earlier;
                        for(opposit_loc = loc + bishop_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += bishop_next_moves[opp_i]) {
                            if(!queen_moved) this_color_attackers[opposit_loc]++;
                            if(!blocked && board[new_loc] < this_color_wimpiest[opposit_loc]) this_color_wimpiest[opposit_loc] = board[new_loc];
                            if(board[opposit_loc] & terminate_diag) break;
                            if(board[opposit_loc]) blocked = True;
                        }
                        blocked_earlier = True;
                    }
                }
            }
        }

        // Here we look for rooks and queens.  It isn't necessary if the piece moving
        // is a rook or queen because the attacks would have been included before.

        if(!(contents & rook_mask)) {

            // Find any rooks or queens of this color that this piece blocked.

            for(i = 0; i <= 3; i++) {
                blocked_earlier = False;
                for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & terminate_rf); new_loc += rook_next_moves[i]) {
                    if(board[new_loc] & attackers_rf) {

                        // Now we start on the opposite rank or file of the piece removed
                        // and update the squares now attacked by the rook or queen.
                        // Also update the wimpiest piece attacking if necessary.

                        opp_i = (i > 1) ? i - 2 : i + 2;
                        blocked = blocked_earlier;
                        for(opposit_loc = loc + rook_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += rook_next_moves[opp_i]) {
                            if(!queen_moved) this_color_attackers[opposit_loc]++;
                            if(!blocked && board[new_loc] < this_color_wimpiest[opposit_loc]) this_color_wimpiest[opposit_loc] = board[new_loc];
                            if(board[opposit_loc] & terminate_rf) break;
                            if(board[opposit_loc]) blocked = True;
                        }
                        blocked_earlier = True;
                    }
                }
            }
        }

        // Now update all of the pieces of the other color that attack through this square.

        // Here we look for bishops and queens.

        for(i = 0; i <= 3; i++) {

            // Find any bishops or queens of the enemy color that this piece blocked.

            blocked_earlier = False;
            for(new_loc = loc + bishop_next_moves[i]; !(board[new_loc] & terminate_enemy_diag); new_loc += bishop_next_moves[i]) {
                if(board[new_loc] & attackers_enemy_diag) {

                    // Now we start on the opposite diagonal of the piece removed
                    // and update the squares now attacked by the bishop or queen.
                    // Also update the wimpiest piece attacking if necessary.

                    opp_i = (i > 1) ? i - 2 : i + 2;
                    blocked = blocked_earlier;
                    for(opposit_loc = loc + bishop_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += bishop_next_moves[opp_i]) {
                        enemy_attackers[opposit_loc]++;
                        if(!blocked && board[new_loc] < enemy_wimpiest[opposit_loc]) enemy_wimpiest[opposit_loc] = board[new_loc];
                        if(board[opposit_loc] & terminate_enemy_diag) break;
                        if(board[opposit_loc]) blocked = True;
                    }
                    blocked_earlier = True;
                }
            }
        }

        // Here we look for rooks and queens.

        for(i = 0; i <= 3; i++) {

            // Find any rooks or queens of the enemy color that this piece blocked.

            blocked_earlier = False;
            for(new_loc = loc + rook_next_moves[i]; !(board[new_loc] & terminate_enemy_rf); new_loc += rook_next_moves[i]) {
                if(board[new_loc] & attackers_enemy_rf) {

                    // Now we start on the opposite rank or file of the piece removed
                    // and update the squares now attacked by the rook or queen.
                    // Also update the wimpiest piece attacking if necessary.

                    opp_i = (i > 1) ? i - 2 : i + 2;
                    blocked = blocked_earlier;
                    for(opposit_loc = loc + rook_next_moves[opp_i]; !(board[opposit_loc] & Out); opposit_loc += rook_next_moves[opp_i]) {
                        enemy_attackers[opposit_loc]++;
                        if(!blocked && board[new_loc] < enemy_wimpiest[opposit_loc]) enemy_wimpiest[opposit_loc] = board[new_loc];
                        if(board[opposit_loc] & terminate_enemy_rf) break;
                        if(board[opposit_loc]) blocked = True;
                    }
                    blocked_earlier = True;
                }
            }
        }

        // Replace the contents of the square.

        board[loc] = contents;
    }


int Position_type::will_king_be_in_check(
        bd_index     from_loc,    // The from location.
        bd_index     to_loc       // The to location.
    )
    // This function will test to see if the given king will be in check
    // after the move is made from this position.
    {

        square       attackers_diag;
        square       attackers_rf;
        char        *enemy_attackers;
        bd_index     king_location;
        int          king_file;
        int          king_rank;
        int          mover_file;
        int          mover_rank;
        bd_index     new_loc;
        int          ret_val;
        int          ret_val1;
        int          step;
        square       terminate_diag;
        square       terminate_rf;
        square       to_contents;


        king_location = king_loc[mover];

        // Save whatever was in the square moved to.

        to_contents = board[to_loc];

        // Make the move.  It turns out that for seeing if the king will be
        // in check, it is not necessary to do anything special for castling
        // or en pesant.  For speed's sake we temporarily make the move on the
        // board and then see if the king is in check, then un-make the move.

        board[to_loc] = board[from_loc];
        board[from_loc] = Empty;

        if(mover == White) {
            enemy_attackers = black_attackers;
        } else {
            enemy_attackers = white_attackers;
        }

        // We look at different things depending on if the king was in check
        // and if the king is moving.

        ret_val = False;
        if(king_location == from_loc || enemy_attackers[king_location]) {

            // These are complicated enough cases that we might as well call the function.

            if(king_location == from_loc) king_location = to_loc;
            if(square_attacked_bool(king_location, other_color(mover))) ret_val = True;

        } else {

            if(mover == White) {
                attackers_rf    = Brook | Bqueen;
                terminate_rf    = Out | Bpawn | Bknight | Bbishop | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
                attackers_diag  = Bbishop | Bqueen;
                terminate_diag  = Out | Bpawn | Bknight | Brook | Bking | Wpawn | Wknight | Wbishop | Wrook | Wqueen | Wking;
            } else {
                attackers_rf    = Wrook | Wqueen;
                terminate_rf    = Out | Wpawn | Wknight | Wbishop | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
                attackers_diag  = Wbishop | Wqueen;
                terminate_diag  = Out | Wpawn | Wknight | Wrook | Wking | Bpawn | Bknight | Bbishop | Brook | Bqueen | Bking;
            }

            // Test to see if the moving piece is on the same rank, file, or diagonal as the king.

            if(king_rank = rank_number(king_location), mover_rank = rank_number(from_loc), king_rank == mover_rank) {
                                    
                // See if piece on the same rank as the king.

                if(king_location > from_loc) {
                    step = next_l_rank;
                } else {
                    step = next_r_rank;
                }
                for(new_loc = king_location + step; !(board[new_loc] & terminate_rf); new_loc = new_loc + step) {
                    if(board[new_loc] & attackers_rf) ret_val = True;
                }
            } else if(king_file = file_number(king_location), mover_file = file_number(from_loc), king_file == mover_file) {

                // See if piece on the same file as the king.

                if(king_location > from_loc) {
                    step = next_d_file;
                } else {
                    step = next_u_file;
                }
                for(new_loc = king_location + step; !(board[new_loc] & terminate_rf); new_loc = new_loc + step) {
                    if(board[new_loc] & attackers_rf) ret_val = True;
                }
            } else {

                // We need to check both diagonals because there are cases
                // where the piece will be on both because of the way we
                // test to see if it is on the same diagonal.

                if(abs_v(king_location - from_loc) % next_rd_diag == 0) {
   
                    // See if the piece is on the right down diagonal from the king.

                    if(king_location > from_loc) {
                        step = next_lu_diag;
                    } else {
                        step = next_rd_diag;
                    }
                    for(new_loc = king_location + step; !(board[new_loc] & terminate_diag); new_loc = new_loc + step) {
                        if(board[new_loc] & attackers_diag) ret_val = True;
                    }
                }
                if(abs_v(king_location - from_loc) % next_ru_diag == 0) {
   
                    // See if the piece is on the right up diagonal from the king.

                    if(king_location > from_loc) {
                        step = next_ld_diag;
                    } else {
                        step = next_ru_diag;
                    }
                    for(new_loc = king_location + step; !(board[new_loc] & terminate_diag); new_loc = new_loc + step) {
                        if(board[new_loc] & attackers_diag) ret_val = True;
                    }
                }
            }
        }

        if(Debug10) {

            // Test to make sure we have the right answer.

            if(square_attacked_bool(king_location, other_color(mover))) ret_val1 = True;
            else ret_val1 = False;
        }

        // Undo move.

        board[from_loc] = board[to_loc];
        board[to_loc]   = to_contents;

        if(Debug10) {

            // Print board and error message if wrong answer.

            if(ret_val1 != ret_val) {
                cout << "Error in king_in_check.\n";
                print_board(this);
                cout << "Move was " << from_loc << " to " << to_loc << "\n";
            }
        }

        return(ret_val);
    }


void Position_type::append_move_list()
    // This routine will put the move lists together and set moves_all
    // to the first move.  The move lists are ordered as follows:
    // moves_priority_1 to moves_priority_5.
    {
        Gen_move_type  *top;   // Record that points to top of move list.
        Gen_move_type  *temp;  // Temporary pointer.


        if(Debug9) {
            cout << "Moves ordered by priority from this position:\n";
            print_board(this);
            cout << "Priority 1 moves.\n";
            print_move_list(this, moves_priority_1, False);
            if(moves_priority_1) cout << '\n';
            cout << "Priority 2 moves.\n";
            print_move_list(this, moves_priority_2, False);
            if(moves_priority_2) cout << '\n';
            cout << "Priority 3 moves.\n";
            print_move_list(this, moves_priority_3, False);
            if(moves_priority_3) cout << '\n';
            cout << "Priority 4 moves.\n";
            print_move_list(this, moves_priority_4, False);
            if(moves_priority_4) cout << '\n';
            cout << "Priority 5 moves.\n";
            print_move_list(this, moves_priority_5, False);
            if(moves_priority_5) cout << '\n';
        }
        if(Debug16) {
            for(temp = moves_priority_1; temp; temp = temp->next) glob__priority_1_count++;
            for(temp = moves_priority_2; temp; temp = temp->next) glob__priority_2_count++;
            for(temp = moves_priority_3; temp; temp = temp->next) glob__priority_3_count++;
            for(temp = moves_priority_4; temp; temp = temp->next) glob__priority_4_count++;
            for(temp = moves_priority_5; temp; temp = temp->next) glob__priority_5_count++;
        }
//        moves_priority_1 = sort_moves(moves_priority_1);
//        moves_priority_2 = sort_moves(moves_priority_2);
//        moves_priority_3 = sort_moves(moves_priority_3);
//        moves_priority_4 = sort_moves(moves_priority_4);
//        moves_priority_5 = sort_moves(moves_priority_5);
        top = new_move_type();
        temp = top;
        temp->next = moves_priority_1;
        while(temp->next) temp = temp->next;
        temp->next = moves_priority_2;
        while(temp->next) temp = temp->next;
        temp->next = moves_priority_3;
        while(temp->next) temp = temp->next;
        temp->next = moves_priority_4;
        while(temp->next) temp = temp->next;
        temp->next = moves_priority_5;
        moves_all = top->next;
        delete_move_type(top);
    }


#define link_move_into_list(move_list, move) { move->next = move_list; move_list = move; }


void Position_type::add_move(
        bd_index       from_loc, // The location the piece moved from.
        bd_index       to_loc    // The location the piece moved to.
    )
    // This function will add the move to the appropriate move list.
    // Before adding the move it makes sure the king won't be in check.
    // There are three move lists, based on the priority of the move.
    // Ideally, we would like to put the moves on the list in best to
    // worst order.  This would maximize alpha-beta pruning.
    // Unfortunately, we need to use a quick heuristic based on the
    // current position.  In general we put captures early.  Some
    // of the choices of lists may seem counter intuitive, but they
    // worked best for this set of tests.
    {
        int            is_a_killer;         // True if this is in the killer list.
        int            killer_move = False; // True if this move is a killer.
        char          *mover_attackers;     // The mover's attack array.
        char          *other_attackers;     // The defender's attack array.
        square        *other_wimpiest;      // The defender's wimpiest attackers.
        int            piece_value_from;    // The value of the piece moving.
        int            piece_value_to;      // The value of the piece being taken
        Gen_move_type *temp_move;           // Temporary move pointer.


        // Don't add the move if the mover's king would be in check.

        if(will_king_be_in_check(from_loc, to_loc)) return;

        temp_move = new_move_type();
        temp_move->from_loc = from_loc;
        temp_move->to_loc   = to_loc;
        temp_move->val      = 0;

        if(glob__curr_level && glob__killers.is_killer(glob__curr_level, temp_move)) is_a_killer = True;
        else is_a_killer = False;

        if(mover == White) {
            other_attackers = black_attackers;
            other_wimpiest  = black_wimpiest_attacker;
            mover_attackers = white_attackers;
        } else {
            other_attackers = white_attackers;
            other_wimpiest  = white_wimpiest_attacker;
            mover_attackers = black_attackers;
        }

        // Separate captures and non-captures.

        piece_value_from = static_piece_value(board[from_loc]);
        if(!board[to_loc]) {

//            if(glob__gen_just_captures) {
//                delete_move_type(temp_move);
//                return;
//            }

            // If the to location is attacked by the other side, but not
            // defended by the mover, or if it is attacked by a weaker piece
            // then this is not a good move.

            if(other_attackers[to_loc] && (mover_attackers[to_loc] == 1 ||
               (piece_value_from - static_piece_value(other_wimpiest[to_loc]) > 50))) {
//                temp_move->val = -piece_value_from;
                if(is_a_killer) {
                    link_move_into_list(moves_priority_4, temp_move);
                } else {
                    link_move_into_list(moves_priority_5, temp_move);
                }

            // If the piece moving is either attacked and not defended on its original
            // square, or if it is attacked by a weaker piece there, then moving it
            // is a high priority.

            } else if(other_attackers[from_loc] && (!mover_attackers[from_loc] ||
                      (piece_value_from - static_piece_value(other_wimpiest[from_loc]) > 50))) {
//                temp_move->val = piece_value_from;
                if(is_a_killer) {
                    link_move_into_list(moves_priority_2, temp_move);
                } else {
                    link_move_into_list(moves_priority_3, temp_move);
                }

            } else {

                // Any other moves are of medium priority.

                if(piece_value_from == 2000) {

                    // Treat king moves specially.

                    if(abs_v(from_loc - to_loc) == 2 * total_ranks) {

                        // Give castling priority.

                        if(is_a_killer) {
                            link_move_into_list(moves_priority_1, temp_move);
                        } else {
                            link_move_into_list(moves_priority_2, temp_move);
                        }

                    } else {

                        if(is_a_killer) {
                            link_move_into_list(moves_priority_3, temp_move);
                        } else {
                            link_move_into_list(moves_priority_5, temp_move);
                        }
                    }
                } else {
                    if(mover == White) {
                        if(white_advancement_points[to_loc] > white_advancement_points[from_loc]) {
                            if(is_a_killer) {
                                link_move_into_list(moves_priority_2, temp_move);
                            } else {
                                link_move_into_list(moves_priority_3, temp_move);
                            }
                        } else {
                            if(is_a_killer) {
                                link_move_into_list(moves_priority_2, temp_move);
                            } else {
                                link_move_into_list(moves_priority_4, temp_move);
                            }
                        }
                    } else {
                        if(black_advancement_points[to_loc] > black_advancement_points[from_loc]) {
                            if(is_a_killer) {
                                link_move_into_list(moves_priority_2, temp_move);
                            } else {
                                link_move_into_list(moves_priority_3, temp_move);
                            }
                        } else {
                            if(is_a_killer) {
                                link_move_into_list(moves_priority_2, temp_move);
                            } else {
                                link_move_into_list(moves_priority_4, temp_move);
                            }
                        }
                    }
                }
            }
        } else {

            // Separate captures where the value of the piece taken is more than
            // the taker or where the piece taken is not defended.

//            temp_move->val = piece_value_to - piece_value_from;
            piece_value_to = static_piece_value(board[to_loc]);
            if(!other_attackers[to_loc] || piece_value_to - piece_value_from > 50) {
                if(is_a_killer) {
                    link_move_into_list(moves_priority_1, temp_move);
                } else {
                    link_move_into_list(moves_priority_2, temp_move);
                }

            // Here deal with situations where the value of the attacker is more than
            // the value of the piece taken.

            } else if(piece_value_from - piece_value_to > 50) {

                // If the number of attackers of the captured piece is 1 then this is
                // a bad move since we know that the piece is defended, and that it is
                // worth less than the attacker.

                if(mover_attackers[to_loc] == 1) {
                    if(is_a_killer) {
                        link_move_into_list(moves_priority_4, temp_move);
                    } else {
                        link_move_into_list(moves_priority_5, temp_move);
                    }
 
                // If there are enough defenders to capture all of the attackers, then this
                // is a pretty bad move.

                } else if(other_attackers[to_loc] >= mover_attackers[to_loc]) {
                    if(is_a_killer) {
                        link_move_into_list(moves_priority_3, temp_move);
                    } else {
                        link_move_into_list(moves_priority_4, temp_move);
                    }
 
                // If the attacker's value is greater than the value of the caputured piece
                // and it's weakest defender then this is not too good even though there are
                // more attackers.
 
                } else if(piece_value_from > piece_value_to + static_piece_value(other_wimpiest[to_loc])) {
                    if(is_a_killer) {
                        link_move_into_list(moves_priority_2, temp_move);
                    } else {
                        link_move_into_list(moves_priority_3, temp_move);
                    }
            
                // We put the rest of the moves here in a medium category.  This group would be ones
                // where the captured piece is defended and attacked more than defended, and where
                // there could be a good exchange.

                } else {
                    if(is_a_killer) {
                        link_move_into_list(moves_priority_1, temp_move);
                    } else {
                        link_move_into_list(moves_priority_1, temp_move);
                    }
                }

            // Here we deal with the cases where the capturer and the captured are of
            // about equal value.

            } else {

                // If there are enough defenders to capture all of the attackers, then this
                // is probably an exchange.

                if(other_attackers[to_loc] >= mover_attackers[to_loc]) {
                    if(is_a_killer) {
                        link_move_into_list(moves_priority_1, temp_move);
                    } else {
                        link_move_into_list(moves_priority_1, temp_move);
                    }
 
                // If attacked more than defended, then this is better.
 
                } else {
                    if(is_a_killer) {
                        link_move_into_list(moves_priority_1, temp_move);
                    } else {
                        link_move_into_list(moves_priority_2, temp_move);
                    }
                }
            }
        }
        return;
    }


void Position_type::bishop_moves()
    // This function will generate all possible moves for a bishop located by
    // the static variable piece_loc on the board.  enemy_mask must be set by
    // the calling procedure to either white_mask or black_mask depending on
    // which color the enemy is.
    {
        int       i;
        bd_index  new_loc;

            
        for(i = 0; i <= 3; i++) {
            for(new_loc = piece_loc + bishop_next_moves[i]; !board[new_loc]; new_loc = new_loc + bishop_next_moves[i])
                add_move(piece_loc, new_loc);
            if(board[new_loc] & enemy_mask) add_move(piece_loc, new_loc);
        }

        return;
    }


void Position_type::king_moves()
    // This function will generate all possible moves for a king located by
    // the static variable piece_loc on the board.  enemy_mask must be set by
    // the calling procedure to either white_mask or black_mask depending on
    // which color the enemy is.
    {
        char          *enemy_attackers;
        bd_index       file_B;
        bd_index       file_C;
        bd_index       file_D;
        bd_index       file_F;
        bd_index       file_G;
        int            i;
        bd_index       king_square;
        bd_index       new_loc;

            
        // First we look for the normal king moves.  We don't need
        // to check if the king will be in check here because it
        // is done in add_move.

        for(i = 0; i <= 7; i++) {
            new_loc = piece_loc + king_next_moves[i];
            if(!board[new_loc] || board[new_loc] & enemy_mask) add_move(piece_loc, new_loc);
        }

        if(mover == White) {
            king_square = white_king_home;
            enemy_attackers = black_attackers;
        } else { 
            king_square = black_king_home;
            enemy_attackers = white_attackers;
        }

        // See if castling is legal.

        file_D = king_square + next_l_rank;
        file_C = file_D + next_l_rank;
        file_B = file_C + next_l_rank;
        file_F = king_square + next_r_rank;
        file_G = file_F + next_r_rank;

        // Queen side castling.

        if(can_castle[mover][1] && !board[file_B] && !board[file_C] && !board[file_D] &&
            !enemy_attackers[king_square] && !enemy_attackers[file_D] && !enemy_attackers[file_C])
                add_move(king_square, file_C);

        // King side castling.

        if(can_castle[mover][2] && !board[file_F] && !board[file_G] &&
            !enemy_attackers[king_square] && !enemy_attackers[file_F] && !enemy_attackers[file_G])
                add_move(king_square, file_G);

        return;
    }


void Position_type::knight_moves()
    // This function will generate all possible moves for a knight located by
    // the static variable piece_loc on the board.  enemy_mask must be set by
    // the calling procedure to either white_mask or black_mask depending on
    // which color the enemy is.
    {
        int       i;
        bd_index  new_loc;


        // Here we look to see if all possible places the knight  can
        // move are free or occupied by an enemy piece.

        for(i = 0; i <= 7; i++) {
            new_loc = piece_loc + knight_next_moves[i];
            if(!board[new_loc] || board[new_loc] & enemy_mask) add_move(piece_loc, new_loc);
        }

        return;
    }


void Position_type::pawn_moves()
    // This function will generate all possible moves for a pawn located by
    // the static variable piece_loc on the board.  enemy_mask must be set by
    // the calling procedure to either white_mask or black_mask depending on
    // which color the enemy is.
    {
        if(board[piece_loc] == Wpawn) {
            if(is_on_rank(piece_loc, 2) && !board[piece_loc + next_u_file] && !board[piece_loc + next_u_file + next_u_file]) add_move(piece_loc, piece_loc + next_u_file + next_u_file);
            if(!board[piece_loc + next_u_file]) add_move(piece_loc, piece_loc + next_u_file);
            if(board[piece_loc + next_lu_diag] & enemy_mask) add_move(piece_loc, piece_loc + next_lu_diag);
            if(board[piece_loc + next_ru_diag] & enemy_mask) add_move(piece_loc, piece_loc + next_ru_diag);
            if(is_on_rank(piece_loc, 5)) {
                if(vuln_ep[Black][file_number(piece_loc) - 1]) add_move(piece_loc, piece_loc + next_lu_diag);
                if(vuln_ep[Black][file_number(piece_loc) + 1]) add_move(piece_loc, piece_loc + next_ru_diag);
            }
        } else {
            if(is_on_rank(piece_loc, 7) && !board[piece_loc + next_d_file] && !board[piece_loc + next_d_file + next_d_file]) add_move(piece_loc, piece_loc + next_d_file + next_d_file);
            if(!board[piece_loc + next_d_file]) add_move(piece_loc, piece_loc + next_d_file);
            if(board[piece_loc + next_ld_diag] & enemy_mask) add_move(piece_loc, piece_loc + next_ld_diag);
            if(board[piece_loc + next_rd_diag] & enemy_mask) add_move(piece_loc, piece_loc + next_rd_diag);
            if(is_on_rank(piece_loc, 4)) {
                if(vuln_ep[White][file_number(piece_loc) - 1]) add_move(piece_loc, piece_loc + next_ld_diag);
                if(vuln_ep[White][file_number(piece_loc) + 1]) add_move(piece_loc, piece_loc + next_rd_diag);
            }
        }
        return;
    }


void Position_type::queen_moves()
    // This function will generate all possible moves for a queen located by
    // the static variable piece_loc on the board.  enemy_mask must be set by
    // the calling procedure to either white_mask or black_mask depending on
    // which color the enemy is.
    {
        bishop_moves();
        rook_moves();
        return;
    }


void Position_type::rook_moves()
    // This function will generate all possible moves for a rook located by
    // the static variable piece_loc on the board.  enemy_mask must be set by
    // the calling procedure to either white_mask or black_mask depending on
    // which color the enemy is.
    {
        int       i;
        bd_index  new_loc;

            
        for(i = 0; i <= 3; i++) {
            for(new_loc = piece_loc + rook_next_moves[i]; !board[new_loc]; new_loc = new_loc + rook_next_moves[i])
                add_move(piece_loc, new_loc);
            if(board[new_loc] & enemy_mask) add_move(piece_loc, new_loc);
        }

        return;
    }

