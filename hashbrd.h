//
// HEADER
//    HASHBRD.H -- A board to use with a hashtable.
//
// WRITTEN BY:
//    Truman Collins
//
// DESCRIPTION
//    This class implements a board that can be easily hashed.
//    It allows puting pieces at given locations on the board, and generating
//    a hash value for the current board among other things.
//
// HISTORY
//    T.C. June 1992 - Added hashing capability.


class Hash_board_type {

private :

    //
    //  Data.
    //

    unsigned short   board[16];
        // Holds board position.

    char             mover;
        // Side to move next.

public :

    //
    //  Member functions.
    //

    Hash_board_type();
        // Create a blank one.
             
    friend int operator==(const Hash_board_type &board1, const Hash_board_type &board2);
        // Equality comparison operator between two boards.

    void clear_board();
        // Clear the board of pieces.

    void add_piece(int location, int piece);
        // Put the given piece into the given location.

    void add_hash_piece(int location, int hash_piece);
        // Put the given piece into the given location.
        // Here we assume that the piece passed in is a hash piece.

    void set_mover(int mov)
        // Sets the side to move next.
        { mover = mov; }

    int get_mover()
        // Returns the mover.
        { return(mover); }

    int hashvalue();
        // Return the hash value associated with this position.

    void set_like_posit(Position_type *posit);
        // Set this to reflect the position in posit.

    void print_hash_board();
        // Prints the hash board in hex.

    void check_consistency(Position_type *posit);
        // Check to make sure that this position agrees with the one given.

};

