
#include <glob.h>
#include <hashbrd.h>
#include <position.h>
#include <statval.h>
#include <utility.h>
#include <evaluate2.h>

// Values for situations.

#define KNIGHT_ON_HOME_SQUARE   20
#define BISHOP_ON_HOME_SQUARE   20
#define BISHOP_MOBILITY_FACTOR   3
#define ROOK_MOBILITY_FACTOR     3
#define ROOKS_CONNECTED_ON_FILE 30
#define ROOKS_CONNECTED_ON_RANK 15
#define ROOK_OPEN_FILE          30
#define ROOK_SEMI_OPEN_FILE     25
#define QUEEN_MOBILITY_FACTOR    3
#define BISHOP_IN_HOLE          50
#define KNIGHT_IN_HOLE          50


typedef struct hang_piece_type {
    bd_index   loc;
    int        value;
} hang_type;


static square  piece_mask          = Wknight | Wbishop | Wrook | Wqueen | Bknight | Bbishop | Brook | Bqueen;
static square  piece_not_king_mask = Wpawn | Wknight | Wbishop | Wrook | Wqueen | Bpawn | Bknight | Bbishop | Brook | Bqueen;


int evaluate2(
        Position_type *posit,      // The position to make the move on.
        char           comp_color  // The computer's color.
    )
    // This routine will evaluate the given position.
    // It will assign a value to it where the higher the number
    // the better it is for the computer.
    {
        int            attacked_val;         // Value of attacked piece.
        int            attacker_val;         // Value of least valuable attaking piece.
        int            blocked;              // Used to find piece blocking connected rooks.
        square        *board = posit->board; // The board.
        char           color;                // Used to keep track of color of pieces we are looking at.
        square         mover_king;           // The mover's king.
        int            curr_file;            // Holds the current file number.
        int            curr_rank;            // Holds the current rank number.
        square         curr_sqr_contents;    // The contents of the current square.
        int            found;                // Used when looking for pawn holes.
        int            hang_index = 0;       // Index into hanging pieces array.
        hang_type      hanging_pieces[17];   // Array of pieces that just moved that are attacked by a wimpier piece than defended.
        bd_index       i, j, k, l, m;        // Counters.
        int            ind1, ind2;           // Counters.
        int            just_moved;           // The side that just moved.
        char          *just_moved_attackers; // Points to square attacked array for the side that just moved.
        square         just_moved_mask;      // Mask for pieces of side that just moved.
        square         just_moved_pieces;    // Holds mask for finding the pieces of the side that just moved.
        bd_index       just_moved_rook_1 = 0;// Holds location of first rook seen of the side that just moved.
        square        *just_moved_wimpiest;  // Points to square attacked array for the side that just moved.
        int            loss;                 // Loss by trade.
        Gen_move_type *move_top;             // Top of list of moves.
        int            mover;                // The side to move next.
        char          *mover_attackers;      // Points to square attacked array for the mover side.
        square         mover_mask;           // Mask for mover's pieces.
        bd_index       mover_rook_1 = 0;     // Holds location of first rook seen of the mover.
        square        *mover_wimpiest;       // Points to square attacked array for the mover side.
        int            sec_worst_loss = 0;   // The second worst potential trade forcable by person who just moved.
        int            sec_worst_loss_att;   // Attacker attacking the second worst loss.
        int            support_val;          // Value of defending piece.
        hang_type      swap;                 // Used for swapping in sort.
        int            value = 0;            // The position value is calculated into here.
        int            worst_loss = 0;       // The worst potential trade forcable by person who just moved.
        square         worst_loss_att;       // Attacker attacking worst loss.

             
        glob__posits_evaled++;

        // Set up variables based on who is the next mover.

        mover = posit->mover;
        if(mover == White) {
            just_moved_pieces    = Bpawn | Bknight | Bbishop | Brook | Bqueen;
            mover_attackers      = posit->white_attackers;
            mover_wimpiest       = posit->white_wimpiest_attacker;
            just_moved_attackers = posit->black_attackers;
            just_moved_wimpiest  = posit->black_wimpiest_attacker;
            just_moved           = Black;
            mover_mask           = white_mask;
            mover_king           = Wking;
            just_moved_mask      = black_mask;
            value += posit->pawn_struct_value;
        } else {
            just_moved_pieces    = Wpawn | Wknight | Wbishop | Wrook | Wqueen;
            mover_attackers      = posit->black_attackers;
            mover_wimpiest       = posit->black_wimpiest_attacker;
            just_moved_attackers = posit->white_attackers;
            just_moved_wimpiest  = posit->white_wimpiest_attacker;
            just_moved           = White;
            mover_mask           = black_mask;
            mover_king           = Bking;
            just_moved_mask      = white_mask;
            value -= posit->pawn_struct_value;
        }

        // See if the side to move next is checkmated.

#if !Check_Extension

        if(posit->king_in_check()) {

            glob__curr_level = 0;
            move_top = posit->gen_moves();

            if(!move_top) {
                if(posit->mover == enemy_color) return(checkmate_value);
                else return(-checkmate_value);
            } else free_list(move_top);
        }
#endif

        // Points for control of center.

        value += mover_attackers[single_index_from_double(4,4)] << 2;
        value += mover_attackers[single_index_from_double(4,5)] << 2;
        value += mover_attackers[single_index_from_double(5,4)] << 2;
        value += mover_attackers[single_index_from_double(5,5)] << 2;
        value -= just_moved_attackers[single_index_from_double(4,4)] << 2;
        value -= just_moved_attackers[single_index_from_double(4,5)] << 2;
        value -= just_moved_attackers[single_index_from_double(5,4)] << 2;
        value -= just_moved_attackers[single_index_from_double(5,5)] << 2;

        // Give points for castling.

        if(posit->castled[mover]) value += 70;
        else if(posit->can_castle[mover][1] || posit->can_castle[mover][2]) value += 20;

        if(posit->castled[just_moved]) value -= 70;
        else if(posit->can_castle[just_moved][1] || posit->can_castle[just_moved][2]) value -= 20;

        // Figure in valuation of pieces.

        value += posit->total_piece_value[mover] - posit->total_piece_value[just_moved];

        // Try to discourage trading pieces when down in material.

        if(posit->total_piece_value[mover] > posit->total_piece_value[just_moved]) {
            value += ((posit->total_piece_value[mover] - posit->total_piece_value[just_moved]) * (4050 - posit->total_piece_value[just_moved])) >> 12;
        } else {
            value -= ((posit->total_piece_value[just_moved] - posit->total_piece_value[mover]) * (4050 - posit->total_piece_value[mover])) >> 12;
        }

        // Go throught all the squares on the board and analyse pieces attacked.

        color = White;
        i = 0;
        j = posit->piece_locations[color][i];
        while(j) {
            curr_sqr_contents = board[j];

            if(curr_sqr_contents & piece_not_king_mask) {

                // If it is a piece, we give points for some positional situations.

                if(curr_sqr_contents & piece_mask) {
                    if(curr_sqr_contents & rook_mask) {

                        if(curr_sqr_contents & mover_mask) {

                            // Give points for rook on open file.

                            if(!posit->pawn_structure[mover][file_number(j)]) {
                                if(posit->pawn_structure[just_moved][file_number(j)]) {
                                    value += ROOK_SEMI_OPEN_FILE;
                                } else {
                                    value += ROOK_OPEN_FILE;
                                }
                            }

                            if(mover_rook_1) {

                                // Give points for connected rooks.

                                if(same_file(mover_rook_1, j)) {
                                    blocked = 0;
                                    if(mover_rook_1 > j) {
                                        for(k = j + next_u_file; k < mover_rook_1 && !blocked; k += next_u_file) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    } else {
                                        for(k = mover_rook_1 + next_u_file; k < j && !blocked; k += next_u_file) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    }
                                    if(!blocked) {
                                        value += ROOKS_CONNECTED_ON_FILE;
                                    }
                                } else if(rank_number(mover_rook_1) == rank_number(j)) {
                                    blocked = 0;
                                    if(mover_rook_1 > j) {
                                        for(k = j + next_r_rank; k < mover_rook_1 && !blocked; k += next_r_rank) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    } else {
                                        for(k = mover_rook_1 + next_r_rank; k < j && !blocked; k += next_r_rank) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    }
                                    if(!blocked) {
                                        value += ROOKS_CONNECTED_ON_RANK;
                                    }
                                }
                            } else {
                                mover_rook_1 = j;
                            }
                        } else {

                            // Give points for rook on open file.

                            if(!posit->pawn_structure[just_moved][file_number(j)]) {
                                if(posit->pawn_structure[mover][file_number(j)]) {
                                    value -= ROOK_SEMI_OPEN_FILE;
                                } else {
                                    value -= ROOK_OPEN_FILE;
                                }
                            }

                            if(just_moved_rook_1) {

                                // Give points for connected rooks.

                                if(same_file(just_moved_rook_1, j)) {
                                    blocked = 0;
                                    if(just_moved_rook_1 > j) {
                                        for(k = j + next_u_file; k < just_moved_rook_1 && !blocked; k += next_u_file) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    } else {
                                        for(k = just_moved_rook_1 + next_u_file; k < j && !blocked; k += next_u_file) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    }
                                    if(!blocked) {
                                        value -= ROOKS_CONNECTED_ON_FILE;
                                    }
                                } else if(rank_number(just_moved_rook_1) == rank_number(j)) {
                                    blocked = 0;
                                    if(just_moved_rook_1 > j) {
                                        for(k = j + next_r_rank; k < just_moved_rook_1 && !blocked; k += next_r_rank) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    } else {
                                        for(k = just_moved_rook_1 + next_r_rank; k < j && !blocked; k += next_r_rank) {
                                            if(board[k] != Empty) blocked = 1;
                                        }
                                    }
                                    if(!blocked) {
                                        value -= ROOKS_CONNECTED_ON_RANK;
                                    }
                                }
                            } else {
                                just_moved_rook_1 = j;
                            }
                        }
                    } else if(curr_sqr_contents & bishop_mask) {

                        // Points for bishop mobility.

                        k = 0;
                        for(m = 0; m <= 3; m++) {
                            for(l = j + bishop_next_moves[m]; !board[l]; l = l + bishop_next_moves[m]) {
                                k++;
                            }

                            // Include capturing an enemy piece as a move.

                            if(curr_sqr_contents & mover_mask && board[l] & just_moved_mask) k++;
                            else if(curr_sqr_contents & just_moved_mask && board[l] & mover_mask) k++;
                        }
                        if(curr_sqr_contents & mover_mask) value += (k << 2);
                        else value -= (k << 2);

                        if(curr_sqr_contents & white_mask) {

//                            // Give bonus for bishop in pawn hole.
//
//                            if(board[j + next_u_file] == Bpawn) {
//                                found = False;
//                                for(k = j + next_lu_diag, l = j + next_ru_diag; !(board[k] == Out && board[l] == Out) && !found; k += next_u_file, l += next_u_file) {
//                                    if(board[k] == Bpawn || board[l] == Bpawn) found = True;
//                                }
//                                if(!found) {
//                                    if(mover == White) value += BISHOP_IN_HOLE;
//                                    else value -= BISHOP_IN_HOLE;
//                                }
//                            }

                            // Take away some points if bishop on home square.

                            if(j == white_left_bishop_home || j == white_right_bishop_home) {
                                if(mover == White) value -= BISHOP_ON_HOME_SQUARE;
                                else value += BISHOP_ON_HOME_SQUARE;
                            }

                        } else {

//                            // Give bonus for bishop in pawn hole.
//
//                            if(board[j + next_d_file] == Wpawn) {
//                                found = False;
//                                for(k = j + next_ld_diag, l = j + next_rd_diag; !(board[k] == Out && board[l] == Out) && !found; k += next_d_file, l += next_d_file) {
//                                    if(board[k] == Wpawn || board[l] == Wpawn) found = True;
//                                }
//                                if(!found) {
//                                    if(mover == White) value -= BISHOP_IN_HOLE;
//                                    else value += BISHOP_IN_HOLE;
//                                }
//                            }

                            // Take away some points if bishop on home square.

                            if(j == black_left_bishop_home || j == black_right_bishop_home) {
                                if(mover == White) value += BISHOP_ON_HOME_SQUARE;
                                else value -= BISHOP_ON_HOME_SQUARE;
                            }
                        }
                    } else if(curr_sqr_contents & knight_mask) {

                        if(curr_sqr_contents & white_mask) {

                            // Give bonus for knight in pawn hole.

                            if(board[j + next_u_file] == Bpawn || board[j + next_u_file + next_u_file] == Bpawn) {
                                found = False;
                                for(k = j + next_lu_diag, l = j + next_ru_diag; !(board[k] == Out && board[l] == Out) && !found; k += next_u_file, l += next_u_file) {
                                    if(board[k] == Bpawn || board[l] == Bpawn) found = True;
                                }
                                if(!found) {
                                    if(mover == White) value += KNIGHT_IN_HOLE;
                                    else value -= KNIGHT_IN_HOLE;
                                }
                            }

                            // Take away some points if knight on home square.

                            if(j == white_left_knight_home || j == white_right_knight_home) {
                                if(mover == White) value -= KNIGHT_ON_HOME_SQUARE;
                                else value += KNIGHT_ON_HOME_SQUARE;
                            }
                        } else {

                            // Give bonus for knight in pawn hole.

                            if(board[j + next_d_file] == Wpawn || board[j + next_d_file + next_d_file] == Wpawn) {
                                found = False;
                                for(k = j + next_ld_diag, l = j + next_rd_diag; !(board[k] == Out && board[l] == Out) && !found; k += next_d_file, l += next_d_file) {
                                    if(board[k] == Wpawn || board[l] == Wpawn) found = True;
                                }
                                if(!found) {
                                    if(mover == White) value -= KNIGHT_IN_HOLE;
                                    else value += KNIGHT_IN_HOLE;
                                }
                            }

                            // Take away some points if knight on home square.

                            if(j == black_left_knight_home || j == black_right_knight_home) {
                                if(mover == White) value += KNIGHT_ON_HOME_SQUARE;
                                else value -= KNIGHT_ON_HOME_SQUARE;
                            }
                        }
                    }
                }

                // See if any pieces of the side that just moved are protected
                // less than attacked.  Also take into account pieces that are
                // attacked by pieces worth less than themselves.

                if(curr_sqr_contents & just_moved_pieces) {
                    if(mover_attackers[j]) {

                        // Give points for just attacking a piece.

                        value += (mover_attackers[j] << 2);

                        if(!just_moved_attackers[j]) {
                            hanging_pieces[hang_index].loc = j;
                            hanging_pieces[hang_index++].value = posit->piece_value(j);
                        } else {
                            attacked_val = static_piece_value(curr_sqr_contents);
                            attacker_val = static_piece_value(mover_wimpiest[j]);
                            if(mover_attackers[j] > just_moved_attackers[j]) {
                                support_val = static_piece_value(just_moved_wimpiest[j]);
                                if(attacker_val < attacked_val + support_val) {
                                    hanging_pieces[hang_index].loc = j;
                                    if(support_val < attacker_val) {
                                        hanging_pieces[hang_index++].value = attacked_val - attacker_val + support_val;
                                    } else {
                                        hanging_pieces[hang_index++].value = attacked_val;
                                    }
                                }
                            } else {
                                if(attacker_val < attacked_val) {
                                    hanging_pieces[hang_index].loc = j;
                                    hanging_pieces[hang_index++].value = attacked_val - attacker_val;
                                }
                            }
                        }
                    }
                } else {

                    // Give points for just attacking a piece..

                    value -= (just_moved_attackers[j] << 2);

                    // If there is more than one piece about to move that is attacked
                    // by a wimpier piece, then subtract the second
                    // worst loss since usually the side moving can't get out of
                    // two such situations at the same time.

                    if(just_moved_wimpiest[j] < Max_piece) {
                        attacked_val = static_piece_value(curr_sqr_contents);
                        attacker_val = static_piece_value(just_moved_wimpiest[j]);
                        if(!mover_attackers[j] || attacker_val < attacked_val) {
                            if(mover_attackers[j]) loss = attacked_val - attacker_val;
                            else loss = attacked_val;
                            if(loss > worst_loss) {
                                sec_worst_loss     = worst_loss;
                                sec_worst_loss_att = worst_loss_att;
                                worst_loss     = loss;
                                worst_loss_att = just_moved_wimpiest[j];
                            } else if(loss > sec_worst_loss) {
                                sec_worst_loss     = loss;
                                sec_worst_loss_att = just_moved_wimpiest[j];
                            }
                        }
                    }
                }

            } else {
                curr_rank = rank_number(j);
                curr_file = file_number(j);

                // Check for squares attacked around the king.

                if(curr_sqr_contents == mover_king) {
                    if(curr_rank < 8) value -= just_moved_attackers[j + next_u_file] << 3;
                    if(curr_rank > 1) value -= just_moved_attackers[j + next_d_file] << 3;
                    if(curr_file != 1) {
                        value -= just_moved_attackers[j + next_l_rank] << 3;
                        if(curr_rank < 8) value -= just_moved_attackers[j + next_lu_diag] << 3;
                        if(curr_rank > 1) value -= just_moved_attackers[j + next_ld_diag] << 3;
                    }
                    if(curr_file != 8) {
                        value -= just_moved_attackers[j + next_r_rank] << 3;
                        if(curr_rank < 8) value -= just_moved_attackers[j + next_ru_diag] << 3;
                        if(curr_rank > 1) value -= just_moved_attackers[j + next_rd_diag] << 3;
                    }
                } else {
                    if(curr_rank < 8) value += mover_attackers[j + next_u_file] << 3;
                    if(curr_rank > 1) value += mover_attackers[j + next_d_file] << 3;
                    if(curr_file != 1) {
                        value += mover_attackers[j + next_l_rank] << 3;
                        if(curr_rank < 8) value += mover_attackers[j + next_lu_diag] << 3;
                        if(curr_rank > 1) value += mover_attackers[j + next_ld_diag] << 3;
                    }
                    if(curr_file != 8) {
                        value += mover_attackers[j + next_r_rank] << 3;
                        if(curr_rank < 8) value += mover_attackers[j + next_ru_diag] << 3;
                        if(curr_rank > 1) value += mover_attackers[j + next_rd_diag] << 3;
                    }
                }
            }
            i++;
            j = posit->piece_locations[color][i];

            // Look at the black pieces if we are done with the whites.

            if(!j && color == White) {
                color = Black;
                i = 0;
                j = posit->piece_locations[color][i];
            }
        }

        // Sort the hanging_piece array for the side that just moved.
        // Use a bubble sort since this array will be very small.

        for(ind1 = 0; ind1 < hang_index - 1; ind1++) {
            for(ind2 = ind1 + 1; ind2 < hang_index; ind2++) {
                if(hanging_pieces[ind2].value > hanging_pieces[ind1].value) {
                    swap = hanging_pieces[ind2];
                    hanging_pieces[ind2] = hanging_pieces[ind1];
                    hanging_pieces[ind1] = swap;
                }
            }
        }

        // Here we check for the case where the two worst losses by the side to move next
        // are caused by the same piece.  If the piece is one of the ones attacked, then
        // we will take it and there will be no loss at all.  An example would be a knight
        // fork where the knight can be taken.

        if(sec_worst_loss && worst_loss_att == sec_worst_loss_att &&
            !(hang_index && hanging_pieces[0].value > worst_loss && board[hanging_pieces[0].loc] != worst_loss_att)) {
            for(ind1 = 0; ind1 < hang_index; ind1++) {
                if(board[hanging_pieces[ind1].loc] == worst_loss_att) {

                    // We assume we have found the piece causing the double attack, and
                    // we adjust the value accoringly.

                    value += hanging_pieces[ind1].value;

                    if(comp_color != mover) value = -value;
                    return(value);
                }
            }
        }

        // Adjust for a piece that the mover can't save and what the one
        // who just moved can't.

        value -= sec_worst_loss;
        if(hang_index) {

//            if(!glob__dont_go_any_deeper && (hang_index > 2 && sec_worst_loss)) {
// print_board(posit);
//                cerr << "Looking deeper.  Hang index = " << hang_index << " sec_worst_loss = " << sec_worst_loss << "  ";
//                // Look a little deeper since it is unclear how the captuures will sort out.
//
//                return(capture_extension_value);
//            }

            value += hanging_pieces[0].value;

            // We take off half the value of other hanging pieces.

            for(ind1 = 1; ind1 < hang_index; ind1++) value += hanging_pieces[ind1].value >> 1;
        }

        if(comp_color != mover) value = -value;

        return(value);
    }

