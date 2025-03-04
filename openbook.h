//
// HEADER
//    OPENBOOK.H -- Contains the opening book.  Implements functions
//                  to read in the book and to query it for moves.
//
// WRITTEN BY:
//    Truman Collins
//
// DESCRIPTION
//      This class holds the opening book in a tree structure
// based on the Forsyth notation of the position.  For each
// position in the opening book there is a list of available
// moves.  These moves may be weighted from 0 to 9.  If there
// were two moves for a position with each weighted the same,
// then each would be chosen with equal probability.  If one
// move was weighted 9 and the other 1, the one weighted 9
// would be chosen 9 out of 10 times.
//
// HISTORY
//    T.C. Fall 1985 - Spring 1986  Initial code done in Pascal.
//    T.C. Spring 1991 Improvements and ported to C++.

//
// QUICK REFERENCE:      
//
//   CONSTRUCTORS:
//       Open_book_type()
//
//   DESTRUCTOR:
//
//   PUBLIC FUNCTIONS: 
//       int check_library(Position_type *posit, Gen_move_type **move)
//

class Lib_pos_type;

class Open_book_type {

private :


    //
    //  Data.
    //

    Lib_pos_type  *library_top;
        // Points to the top of the openining book binary tree.

public :

    //
    //  Member functions.
    //

    Open_book_type();
        // Read the opening book into memory and initialize.

    int check_library(
        Position_type  *posit,   // The current position.
        Gen_move_type **move     // Filled in if found.
        );
        // This function will look through the opening library for the given
        // position and color to move.  If it is not found then False will be
        // returned.  If it is found, True will be returned, and a new move
        // object will be returned in move.

private :

    void add_opening(
        Position_type  *posit,    // The position that this move is for.
        bd_index        from_loc, // The location to move from.
        bd_index        to_loc,   // The location to move to.
        int             probab    // The probability for this move.
        );
        // This routine will add the move passed in to the tree containing the
        // opening library.  The position passed will identify the position to
        // make the move from.

    void process_opening(
        istream       &lib_in,    // The stream to read from.
        Position_type *old_posit, // The current position before this move.
        int           *move_numb, // The move number of the move that was read.
        char          *color,     // The color of the side that made the move.
        bd_index      *from_loc,  // The location the move was from.
        bd_index      *to_loc,    // The location the move was to.
        int           *probab     // The probability to make this move.
        );
        // This is a recursive function that takes a position and a move.
        // It makes the move on the board in position and then stores
        // the move associated with the position using the routine
        // add_opening.  It then will read the next line from the book
        // file and depending on the move number will either recurse
        // or return.

    void read_next_line(
        istream  &lib_in,    // The stream to read from.
        int      *move_numb, // The move number of the move that was read.
        char     *color,     // The color of the side that made the move.
        bd_index *from_loc,  // The location the move was from.
        bd_index *to_loc,    // The location the move was to.
        int      *probab     // The probability to make this move.
        );
        // The function will read the next useful line from the opening library
        // file.  It will put the information from this line into the locations
        // passed.  We ignore all lines starting with '#', and all spaces before
        // the first non-space character.  If there is a problem with the input
        // we will exit.  Move_numb will be set to 0 when we reach the end of file.

};


