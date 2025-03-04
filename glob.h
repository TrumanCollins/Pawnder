#ifndef INCLUDED_GLOB
#define INCLUDED_GLOB

#include <iostream>
#include <fstream>
using namespace std;

//// Define to 1 to compile to run under Xchess.
//// Define to 0 if not.
//
//#define Xchess 0


// Define this to 1 to use 1 ply check extension.
// This helps to sort out the tactics involved in check.

#define Check_Extension 1


// Define to 1 to use hashtable.
// Define to 0 to not.

#define Use_Hashtable 1


// Define the number of bits to use for hash table addresses.
// 2 ^ HASH_BITS is the size of the table.

#define HASH_BITS 15

#define HASH_TABLE_SIZE  (1 << HASH_BITS)


// Define size of board.

#define total_files 12
#define total_ranks 12

// Here is a picture of the internal board.
// It is actually stored as a one dimensional array with
// the lower left cell being board[0], the lower left E cell
// begin board[34].  It can be though of as a two dimensional
// array with a declaration board[12][12].  The file is the
// first index, and the rank is the second.  The E's are cells
// representing actual board squares.  The O's are a boarder
// which simplifies move generation.  We don't have to test
// for the boarder.  The ?'s are not used, but are there to
// make address calculation easier when given a file and a
// rank.  We can use a shift 4 rather than a multiply for
// array indexing.
// 
//    | O O O O O O O O O O O O
//    | O O O O O O O O O O O O
// 8  | O O E E E E E E E E O O
// 7  | O O E E E E E E E E O O
// 6  | O O E E E E E E E E O O
// 5  | O O E E E E E E E E O O
// 4  | O O E E E E E E E E O O
// 3  | O O E E E E E E E E O O
// 2  | O O E E E E E E E E O O
// 1  | O O E E E E E E E E O O
//    | O O O O O O O O O O O O
//    | O O O O O O O O O O O O
//      -----------------------
//          A B C D E F G H

// Arrays used to convert from a single board index to file and rank.

extern int file_numb[total_files * total_ranks];
extern int rank_numb[total_files * total_ranks];

// Macro to get single index from double index.

#define single_index_from_double(f,r) (((f + 1) * total_files) + r + 1)


// Macro to get single index from two characters such as 'a' '4'.

#define single_index_from_chars(f,r) single_index_from_double((f - 'a' + 1), (r - '1' + 1))


// Macro to return the file number (1 to 8) given the location.

#define file_number(curr) (file_numb[curr])


// Macro to return the rank number (1 to 8) given the location.

#define rank_number(curr) (rank_numb[curr])


// Macro to return one if two numbers are on same file, 0 otherwise.

#define same_file(loc1, loc2) (file_numb[loc1] == file_numb[loc2])


// Macro to return if the location given by the first parameter is
// on the rank denoted by the second parameter.

#define is_on_rank(curr, rank) ((rank_number(curr) == rank) ? 1 : 0)


// Macro to return the character representing the file (a - h) from the location.

#define file_as_char(loc) ((char) file_number(loc) + 'a' - 1)


// Macro to return the character representing the rank (1 - 8) from the location.

#define rank_as_char(loc) ((char) rank_number(loc) + '0')


// Type to index into the board array.

typedef short bd_index;


// Define locations of selected pieces.

#define white_king_home         single_index_from_chars('e','1')
#define black_king_home         single_index_from_chars('e','8')
#define white_left_rook_home    single_index_from_chars('a','1')
#define white_right_rook_home   single_index_from_chars('h','1')
#define black_left_rook_home    single_index_from_chars('a','8')
#define black_right_rook_home   single_index_from_chars('h','8')
#define white_left_knight_home  single_index_from_chars('b','1')
#define white_right_knight_home single_index_from_chars('g','1')
#define black_left_knight_home  single_index_from_chars('b','8')
#define black_right_knight_home single_index_from_chars('g','8')
#define white_left_bishop_home  single_index_from_chars('c','1')
#define white_right_bishop_home single_index_from_chars('f','1')
#define black_left_bishop_home  single_index_from_chars('c','8')
#define black_right_bishop_home single_index_from_chars('f','8')


// Define constants for moving around the board.

// Add these to a board index to get to the next
// diagonal square in the given direction.

#define next_ld_diag (-total_ranks - 1)
#define next_lu_diag (-total_ranks + 1)
#define next_rd_diag (total_ranks - 1)
#define next_ru_diag (total_ranks + 1)

// Add these to a board index to get to the next
// rank and file squares in the given direction.

#define next_d_file (-1)
#define next_u_file 1
#define next_l_rank (-total_ranks)
#define next_r_rank total_ranks

// Add these to a board index to get the next
// knight moves.

#define knight_ldd (-total_ranks - 2)
#define knight_lld (-(total_ranks * 2) - 1)
#define knight_llu (-(total_ranks * 2) + 1)
#define knight_luu (-total_ranks + 2)
#define knight_ruu (total_ranks + 2)
#define knight_rru (total_ranks * 2 + 1)
#define knight_rrd (total_ranks * 2 - 1)
#define knight_rdd (total_ranks - 2)


// This is an array containing all king moves.

extern const bd_index king_next_moves[8];


// This is an array containing all knight moves.

extern const bd_index knight_next_moves[8];


// This is an array containing all bishop moves.

extern const bd_index bishop_next_moves[4];


// This is an array containing all rook moves.

extern const bd_index rook_next_moves[4];

// These arrays contain piece values for the particular squares.

extern int white_pawn_values[total_files * total_ranks];
extern int black_pawn_values[total_files * total_ranks];
extern int knight_values[total_files * total_ranks];
extern int bishop_values[total_files * total_ranks];
extern int queen_values[total_files * total_ranks];


#define tollerance 0
   // This is the rating tollerance for moves
   // which are chosen randomly.
#define max_value  32000
   // This is the maximum position value.
#define checkmate_value (max_value - 2)
   // Value of a checkmate.



// Boolean values.

#define False 0
#define True  1


// Piece values.

typedef short square;

#define Empty     0x0
#define Out       0x1
#define Wpawn     0x2
#define Wknight   0x4
#define Wbishop   0x8
#define Wrook     0x10
#define Wqueen    0x20
#define Wking     0x40
#define Bpawn     0x80
#define Bknight   0x100
#define Bbishop   0x200
#define Brook     0x400
#define Bqueen    0x800
#define Bking     0x1000
#define Max_piece 0x1FFF

#define H_Empty     0
#define H_Wpawn     1
#define H_Wknight   2
#define H_Wbishop   3
#define H_Wrook     4
#define H_Wqueen    5
#define H_Wking     6
#define H_Bpawn     7
#define H_Bknight   8
#define H_Bbishop   9
#define H_Brook     10
#define H_Bqueen    11
#define H_Bking     12


// Masks to find white and black pieces.

#define white_mask   (Wpawn | Wknight | Wbishop | Wknight | Wrook | Wqueen | Wking)
#define black_mask   (Bpawn | Bknight | Bbishop | Bknight | Brook | Bqueen | Bking)
#define pawn_mask    (Wpawn | Bpawn)
#define knight_mask  (Wknight | Bknight)
#define bishop_mask  (Wbishop | Bbishop)
#define rook_mask    (Wrook | Brook)
#define queen_mask   (Wqueen | Bqueen)
#define king_mask    (Wking | Bking)


// Color values.

#define White 0
#define Black 1

// Macro to return the opposite color.

#define other_color(col) ((col == White) ? Black : White)

// Define absolute value.

#define abs_v(val) (((val) < 0) ? -(val) : (val))


// Player values.

#define Human    0
#define Computer 1


class Gen_move_type {

public :

    bd_index       from_loc;
    bd_index       to_loc;
    int            val;
    Gen_move_type *next;
    Gen_move_type *continuation;
    Gen_move_type *next_lev_moves;

    Gen_move_type() {}

    Gen_move_type(bd_index f_loc, bd_index t_loc, int vl, Gen_move_type *nxt)
        { from_loc = f_loc; to_loc = t_loc; val = vl; next = nxt; }

};


//
// Inline function declarations.
//


inline void set_piece_sets(
        int     color,    // The color to move.
        square *friendly, // Place to put friend mask.
        square *enemy     // Place to put enemy mask and empty.
    )
    // This function will set the friend and enemy masks based on the
    // color to move.  The friend mask will result in a non-zero value
    // when anded with a friendly piece.  The enemy mask works the same
    // way with enemy pieces.
    {
        if(color == White) {
            *friendly = white_mask;
            *enemy    = black_mask;
        } else {
            *friendly = black_mask;
            *enemy    = white_mask;
        }
    }

class Position_type;
class Open_book_type;
class Hash_board_type;
class Hashtable_type;
class Killer_type;


extern Open_book_type glob__opening_book;
extern Gen_move_type *glob__move_free_list;
extern Position_type  glob__current_position;
extern Killer_type    glob__killers;
extern char          *glob__past_positions[510];
extern filebuf        glob__list_file_buf;
extern int            glob__computer_against_computer;
extern int            glob__human_against_human;
extern int            glob__computer_color;
extern int            glob__current_move;
extern int            glob__depth_reached;
extern int            glob__extension_depth_reached;
extern int            glob__draw_count;
extern int            glob__max_depth[2];
extern int            glob__new_board;
extern int            glob__position_count;
extern int            glob__curr_level;
extern int            glob__max_main_level;
extern long           glob__posits_evaled;
extern long           glob__hash_boards_used;
extern long           glob__hash_nodes_allocated;
extern int            glob__hash_mask_off;
extern int            glob__hash_mask_on;
extern int            glob__print_pos;
extern int            glob__use_library;
extern int            glob__max_check_extension;
extern int            glob__alpha_betas[20];
// extern int            glob__low_window;
// extern int            glob__high_window;
extern long           glob__elapsed_time;
extern long           glob__end_time;
extern long           glob__start_time;
extern long           glob__total_time[2];
extern long           glob__moves_per_tc[2];
extern long           glob__time_per_tc[2];
extern long           glob__moves_left_in_tc[2];
extern long           glob__time_left_in_tc[2];
extern long           glob__moves_per_stc[2];
extern long           glob__time_per_stc[2];
extern int            glob__use_fischer_timing;
extern long           glob__fischer_base_time[2];
extern long           glob__fischer_time_inc[2];
extern long           glob__operator_time;
extern long           glob__total_bytes_allocated;
extern ostream       *glob__list_file;
extern int            glob__moves_on_list;
extern int            glob__moves_alloced;
extern long           glob__priority_1_count;
extern long           glob__priority_2_count;
extern long           glob__priority_3_count;
extern long           glob__priority_4_count;
extern long           glob__priority_5_count;

extern bool           glob__running_under_winboard;

extern bool           glob__print_moves_generated;
extern bool           glob__print_moves_and_values;
extern bool           glob__print_evaluator_call_count;
extern bool           glob__print_forsyth_debug;
extern bool           glob__check_data_after_move;
extern bool           glob__print_expected_line_of_play;
extern bool           glob__print_detailed_timings;

#if Use_Hashtable
extern Hashtable_type *glob__hash_table;
#endif


//
// Move free list functions.
//

inline Gen_move_type *new_move_type()
    // Returns a pointer to a new Gen_move_type.
    // A free list is used to minimize time spent in malloc.
    {
        Gen_move_type *new_move;


        if(glob__move_free_list) {
            new_move = glob__move_free_list;
            glob__move_free_list = new_move->next;
        } else {
            new_move = new Gen_move_type;
        }
        new_move->next_lev_moves = NULL;
        return(new_move);
    }

inline Gen_move_type *new_move_type(bd_index f_loc, bd_index t_loc, int vl, Gen_move_type *nxt)
    // Get a new move type and initialize values.
    {
        Gen_move_type *new_move;

        new_move = new_move_type();
        new_move->from_loc = f_loc;
        new_move->to_loc   = t_loc;
        new_move->val      = vl;
        new_move->next     = nxt;
        return(new_move);
    }

inline void delete_move_type(Gen_move_type *move)
    // Puts the move object on free list.
    {
        move->next = glob__move_free_list;
        glob__move_free_list = move;
    }



#endif
