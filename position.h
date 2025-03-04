//
// HEADER
//    POSITION.H -- Contains the information for a chess position.
//                  Implements the functions on a chess position.
//
// WRITTEN BY:
//    Truman Collins
//
// DESCRIPTION
//      This class represents a chess board.  It keeps other
// information which is helpful in analyzing the chess position.
//      Here is a picture of the internal board.
// It is actually stored as a one dimensional array with
// the lower left cell being board[0], the lower left E cell
// begin board[34].  It can be though of as a two dimensional
// array with a declaration board[12][12].  The file is the
// first index, and the rank is the second.  The E's are cells
// representing actual board squares.  The O's are a boarder
// which simplifies move generation.  We don't have to test
// for the boarder in move generation which makes it faster.
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
//
//      Along with the current board position, we keep the current
// side to move, information on who has castled and who can castle,
// the total piece value for each side, pawn structure information,
// the location of the kings, the number of white and black pieces
// attacking each square of the board, and the least valuable white
// and black piece directly attacking each square of the board.
//
//      A number of operations can be performed on a position using
// member functions of this class.  All legal moves can be generated
// for a given position, a move can be made on the board, translation
// can be done to and from Forsyth notation, and a number of other
// checks and operations.
// 
//
// HISTORY
//    T.C. Fall 1985 - Spring 1986  Initial code done in Pascal.
//    T.C. Spring 1991 Improvements and ported to C++.

//
// QUICK REFERENCE:      
//
//   CONSTRUCTORS:
//
//   DESTRUCTOR:
//
//   PUBLIC FUNCTIONS: 
//       Gen_move_type *gen_moves()
//       void make_move_on_board(Gen_move_type *move)
//       int piece_value(bd_index index)
//       int check_for_legal_move(bd_index from_loc, bd_index to_loc)
//       int king_in_check()
//       void setup_board()
//       void set_up_position()
//       int from_forsyth(char *code)
//       void to_forsyth(char *dest)
//

class Two_rep_elem {

public :

    int            index;
        // Index into glob__past_positions array.

    Two_rep_elem  *next;
        // Next one of these on list.

};


class Position_type {

public :

    //
    //  Data.
    //

    square          board[total_files * total_ranks];
        // The current board state.

#if Use_Hashtable
    Hash_board_type hash_board;
        // Holds the board used by the hashtable.
#endif

    char            mover;
        // The color to move next.

    bd_index        piece_locations[2][17];
        // Location of all of the pieces.

    char            can_castle[2][3];
        // Information on which sides it is still legal to castle on for
        // both black and white.

    char            castled[2];
        // Denotes if black and white have already castled.

    char            vuln_ep[2][10];
        // Tells if either white or black is vulnerable to en pesant capture
        // on given file.

    int             total_piece_value[2];
        // Holds the current total piece value for each side.  This is
        // based on different values for different squares for some 
        // pieces.

    int             pawn_structure[2][10];
        // Holds the number of pawns of each color for each file.

    int             pawn_struct_value;
        // The current combined pawn structure value.

    bd_index        king_loc[2];
        // The locaiton of the kings.

    char            white_attackers[total_files * total_ranks];
        // The number of white pieces attacking each square on the board.

    char            black_attackers[total_files * total_ranks];
        // The number of black pieces attacking each square on the board.

    square          white_wimpiest_attacker[total_files * total_ranks];
        // The weakest white attacker for each square on the board.

    square          black_wimpiest_attacker[total_files * total_ranks];
        // The weakest black attacker for each square on the board.

    Two_rep_elem   *positions_repeated_twice;
        // A list of positions repeated twice so far in the actual game.

    Position_type  *next;
        // This is used to hold the next position on the free list.

    //
    //  Member functions.
    //

    Gen_move_type *gen_moves();
        // This function will generate all legal moves from this position
        // and return a list containing them.  The list will be ordered
        // generally by priority.  See the comments to add_move and
        // append_move_list for more on how this is done.

    void make_move_on_board(
        Gen_move_type *move    // The move to make.
        );
        // This function will make the given move on the board, and will update
        // all of the arrays and variables in the class to reflect the move.
        // It will also switch the mover to the other color.

    int piece_value(
        bd_index        index   // The index into the board.
        );
        // Returns the value of the piece at the given location.  A piece may
        // be worth different amounts on different squares.

    int check_for_legal_move(
        bd_index       from_loc, // The location to move from.
        bd_index       to_loc    // The location to move to.
        );
        // This routine will will check the given move for legality in
        // this position.

    int king_in_check();
        // This routine will look to see if the king of the side to move
        // is currently in check.

    void setup_board();
        // This function will set up the board to it's starting position,
        // and it will initialize all of the other data in the class to
        // correspond to the initial position.

    void set_up_position();
        // This function will alow the user to set up a position on the board.
        // After the position is set, all of the other information in the
        // class will be set to correspond to the position.

    int from_forsyth(
        char          *code    // The start of the Forsyth code.
        );
        // This procedure is sent a Forsyth code.  It will create the position
        // corresponding to that code.  If an error is found, 0 will
        // be returned, otherwise 1 will be returned indicating a legal position.
        // All of the other information in the class will be set to correspond
        // to the position.

    void to_forsyth(
        char           *dest    // The start of the string to put the Forsyth representation.
        );
        // This function will convert the current position and the color to move
        // next to a compact notation known as Forsyth notation.  This is
        // written into the string pointed to by dest.

private :

    void set_piece_locations();
        // This routine will set the values of the piece_loc array based
        // on the current position.

    void compare_piece_locations();
        // This function will make sure the piece_loc array matches the board.

    void piece_loc_move_piece(
            char     color,   // Color of the piece to move.
            bd_index from,    // The location to move it from.
            bd_index to       // The location to move it to.
        );
        // This function will move a piece in the piece_location array.

    void piece_loc_remove_piece(
            char     color,
            bd_index loc
        );
        // This function will remove a piece from the piece_location array.

    void set_king_locs();
        // This function will set the king locations on the current board.
        // The entire board is scanned to do it.

    void calculate_auxillary_board_data();
        // This routine will calculate the information in this class that
        // is supplementary to the board, the castling information, and
        // the information on vulnerability to en pesant.  It uses the
        // current board position to calculate total piece value, pawn
        // structure, black and white attackers for each square, and
        // least valuable direct attacker for each square.

    int square_attacked_bool(
        bd_index       loc,    // The location of the square to check.
        char           enemy   // The enemy color.
        );
        // This function will return True if the square denoted by loc is 
        // attacked by a piece of the enemy color on the board.
        // This function is similar to square_attacked_count, except that this
        // one returns True or False, and the other returns a count of the
        // number of pieces attacking.  This routine is slightly faster if
        // all that is wanted is a boolean value.

    int square_attacked_count(
        bd_index       loc,       // The location of the square to check.
        char           enemy,     // The enemy color.
        square        *low_enemy  // Filled in with the least valuable direct attacker.
        );
        // This function will return the number of enemy pieces attacking the square
        // denoted by loc on the board.  This counts attacking pieces
        // behind other attacking pieces.  For example two rooks on a file count as
        // two attacking pieces along that file.  It will fill in the parameter
        // low_enemy to hold the least valuable direct attacker.
        // This function is similar to square_attacked_bool, except that this
        // one returns the number of attackers, and the other returns True or False.

    void update_square_attacked_add_piece(
        bd_index       loc     // The location of the piece added.
        );
        // This routine will update the squares_attacked arrays for the addition
        // of the given piece.  It is assumed that the piece has already been
        // placed on the board.  It will update the number of attackers to any
        // square affected, and it will update the wimpiest piece to these squares
        // if they are not blocked.  It also updates attackes that used to go
        // through this square that don't anymore.

    void update_square_attacked_remove_piece(
        bd_index       loc     // The location of the piece to be removed.
        );
        // This routine will update the squares_attacked arrays for the removal
        // of the given piece.  It must be called before the piece is removed
        // from the board.  It will update the number of attackers to any
        // square affected, and it will update the wimpiest piece to these squares
        // if they are not blocked.

    int will_king_be_in_check(
        bd_index     from_loc,    // The from location.
        bd_index     to_loc       // The to location.
        );
        // This function will test to see if the king of the moving side will be
        // in check after the move is made from this position.

    // Functions to generate moves for particular pieces.

    void append_move_list();
        // This routine will put the move lists together and set moves_all
        // to the first move.  The move lists are ordered as follows:
        // moves_priority_1 to moves_priority_5.

    void add_move(
        bd_index  from_loc,      // The location the piece moved from.
        bd_index  to_loc         // The location the piece moved to.
        );
        // This function will add the move to the appropriate move list.
        // Before adding the move it makes sure the king won't be in check.
        // There are three move lists, based on the priority of the move.
        // Ideally, we would like to put the moves on the list in best to
        // worst order.  This would maximize alpha-beta pruning.
        // Unfortunately, we need to use a quick heuristic based on the
        // current position.  In general we put captures early.  Some
        // of the choices of lists may seem counter intuitive, but they
        // worked best for this set of tests.

    void bishop_moves();
        // This function will generate all possible moves for a bishop located by
        // the static variable piece_loc on the board.  enemy_mask must be set by
        // the calling procedure to either white_mask or black_mask depending on
        // which color the enemy is.

    void king_moves();
        // This function will generate all possible moves for a king located by
        // the static variable piece_loc on the board.  enemy_mask must be set by
        // the calling procedure to either white_mask or black_mask depending on
        // which color the enemy is.

    void knight_moves();
        // This function will generate all possible moves for a knight located by
        // the static variable piece_loc on the board.  enemy_mask must be set by
        // the calling procedure to either white_mask or black_mask depending on
        // which color the enemy is.

    void pawn_moves();
        // This function will generate all possible moves for a pawn located by
        // the static variable piece_loc on the board.  enemy_mask must be set by
        // the calling procedure to either white_mask or black_mask depending on
        // which color the enemy is.

    void queen_moves();
        // This function will generate all possible moves for a queen located by
        // the static variable piece_loc on the board.  enemy_mask must be set by
        // the calling procedure to either white_mask or black_mask depending on
        // which color the enemy is.

    void rook_moves();
        // This function will generate all possible moves for a rook located by
        // the static variable piece_loc on the board.  enemy_mask must be set by
        // the calling procedure to either white_mask or black_mask depending on
        // which color the enemy is.

};


Position_type *new_position();
        // Returns a pointer to a new position.
        // A free list is used to minimize time spent in malloc.

void delete_position(Position_type *posit);
        // Puts this position on the free list for later use.

