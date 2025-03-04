
#include <glob.h>
#include <debug.h>
#include <hashbrd.h>
#include <hashtabl.h>
#include <killer.h>
#include <openbook.h>
#include <position.h>

// This is an array containing all king moves.

const bd_index king_next_moves[8] =
    { next_ld_diag,
      next_l_rank,
      next_lu_diag,
      next_u_file,
      next_ru_diag,
      next_r_rank,
      next_rd_diag,
      next_d_file
    };


// This is an array containing all knight moves.

const bd_index knight_next_moves[8] =
    { knight_ldd,
      knight_lld,
      knight_llu,
      knight_luu,
      knight_ruu,
      knight_rru,
      knight_rrd,
      knight_rdd
    };


// This is an array containing all bishop moves.

const bd_index bishop_next_moves[4] =
    { next_ld_diag,
      next_lu_diag,
      next_ru_diag,
      next_rd_diag
    };


// This is an array containing all rook moves.

const bd_index rook_next_moves[4] =
    { next_d_file,
      next_l_rank,
      next_u_file,
      next_r_rank
    };


// Here we have two arrays to convert from a single board index
// to a file number or rank number.

int file_numb[total_files * total_ranks] = 
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
      0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
      0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
      0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
      0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0,
      0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0,
      0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0,
      0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int rank_numb[total_files * total_ranks] = 
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


// These arrays contain piece values for the particular squares.

int white_pawn_values[total_files * total_ranks] = 
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 100, 100, 100, 115, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 100, 115, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 100, 115, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 123, 123, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 123, 123, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 100, 115, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 100, 115, 200, 300, 0, 0, 0,
      0, 0, 0, 100, 100, 100, 115, 200, 300, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int black_pawn_values[total_files * total_ranks] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 300, 200, 115, 100, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 115, 100, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 115, 100, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 123, 123, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 123, 123, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 115, 100, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 115, 100, 100, 100, 0, 0, 0,
      0, 0, 0, 300, 200, 115, 100, 100, 100, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int knight_values[total_files * total_ranks] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 270, 290, 290, 290, 290, 290, 290, 270, 0, 0,
      0, 0, 290, 300, 300, 300, 300, 300, 300, 290, 0, 0,
      0, 0, 290, 300, 310, 310, 310, 310, 300, 290, 0, 0,
      0, 0, 290, 300, 310, 320, 320, 310, 300, 290, 0, 0,
      0, 0, 290, 300, 310, 320, 320, 310, 300, 290, 0, 0,
      0, 0, 290, 300, 310, 310, 310, 310, 300, 290, 0, 0,
      0, 0, 290, 300, 300, 300, 300, 300, 300, 290, 0, 0,
      0, 0, 270, 290, 290, 290, 290, 290, 290, 270, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int bishop_values[total_files * total_ranks] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 305, 315, 315, 315, 315, 315, 315, 305, 0, 0,
      0, 0, 315, 325, 325, 325, 325, 325, 325, 315, 0, 0,
      0, 0, 315, 325, 335, 335, 335, 335, 325, 315, 0, 0,
      0, 0, 315, 325, 335, 345, 345, 335, 325, 315, 0, 0,
      0, 0, 315, 325, 335, 345, 345, 335, 325, 315, 0, 0,
      0, 0, 315, 325, 335, 335, 335, 335, 325, 315, 0, 0,
      0, 0, 315, 325, 325, 325, 325, 325, 325, 315, 0, 0,
      0, 0, 305, 315, 315, 315, 315, 315, 315, 305, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int queen_values[total_files * total_ranks] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 895, 895, 895, 895, 895, 895, 895, 895, 0, 0,
      0, 0, 895, 900, 900, 900, 900, 900, 900, 895, 0, 0,
      0, 0, 895, 900, 910, 910, 910, 910, 900, 895, 0, 0,
      0, 0, 895, 900, 910, 920, 920, 910, 900, 895, 0, 0,
      0, 0, 895, 900, 910, 920, 920, 910, 900, 895, 0, 0,
      0, 0, 895, 900, 910, 910, 910, 910, 900, 895, 0, 0,
      0, 0, 895, 900, 900, 900, 900, 900, 900, 895, 0, 0,
      0, 0, 895, 895, 895, 895, 895, 895, 895, 895, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


// Global variables

Open_book_type glob__opening_book;
#if Use_Hashtable
Hashtable_type *glob__hash_table = new Hashtable_type();
#endif
Gen_move_type *glob__move_free_list = NULL;
Position_type  glob__current_position;
Killer_type    glob__killers;
char          *glob__past_positions[510];
filebuf        glob__list_file_buf;
int            glob__computer_against_computer;
int            glob__human_against_human;
int            glob__computer_color;
int            glob__current_move;
int            glob__depth_reached;
int            glob__extension_depth_reached;
int            glob__draw_count;
int            glob__max_depth[2];
int            glob__new_board;
int            glob__position_count = 0;
int            glob__curr_level;
int            glob__max_main_level;
// int            glob__low_window;
// int            glob__high_window;
long           glob__posits_evaled;
long           glob__hash_boards_used;
long           glob__hash_nodes_allocated;
int            glob__hash_mask_off;
int            glob__hash_mask_on;
int            glob__print_pos;
int            glob__use_library;
int            glob__alpha_betas[20];
long           glob__elapsed_time;
long           glob__end_time;
long           glob__start_time;
long           glob__total_time[2];
long           glob__moves_per_tc[2];
long           glob__time_per_tc[2];
long           glob__moves_left_in_tc[2];
long           glob__time_left_in_tc[2];
long           glob__moves_per_stc[2];
long           glob__time_per_stc[2];
int            glob__use_fischer_timing;
long           glob__fischer_base_time[2];
long           glob__fischer_time_inc[2];
long           glob__operator_time;
long           glob__total_bytes_allocated = 0;
ostream       *glob__list_file;
int            glob__moves_on_list = 0;
int            glob__moves_alloced = 0;
long           glob__priority_1_count;
long           glob__priority_2_count;
long           glob__priority_3_count;
long           glob__priority_4_count;
long           glob__priority_5_count;

bool           glob__running_under_winboard = false;

bool           glob__print_moves_generated = false;
bool           glob__print_moves_and_values = false;
bool           glob__print_evaluator_call_count = false;
bool           glob__print_forsyth_debug = false;
bool           glob__check_data_after_move = false;
bool           glob__print_expected_line_of_play = false;
bool           glob__print_detailed_timings = false;

