//
// HEADER
//    HASHTABL.H -- A hashtable for hashboards.
//
// WRITTEN BY:
//    Truman Collins
//
// DESCRIPTION
//    A hashtable for boards.  Stores evaluation value and depth
//    with each board.  Can be searched for a specific board, or
//    a board, value, and depth can be added to the table.  The
//    Hash nodes are stored on a freelist when the board is cleared
//    to minimize allocating and deallocating.
//
// HISTORY
//    T.C. June 1992 - Added hashing capability.



class Hash_entry_type;


class Hashtable_type {

private :

    //
    //  Data.
    //

    Hash_entry_type  *table[HASH_TABLE_SIZE];
        // Hash table.

    Hash_entry_type  *freelist;
        // Pointer to free list.

public :

    //
    //  Member functions.
    //

    Hashtable_type();
        // Create an empty one.
             
    void clear_table();
        // Clear the hashtable returning the nodes to the freelist.

    void add_to_table(Hash_board_type *board, short value, short depth, int alpha_beta);
        // Add this board to table if there is enought memory.

    int check_table(Hash_board_type *board, int *value, short depth, int alpha_beta, char comp_color);
        // Look for this board in the table and return true if found.
        // Set value if found.  If the position is found, but the
        // depth of the found board is greater than the depth passed
        // in, it is not considered found.  This is so we don't use
        // a value generated with only two levels of search for one
        // that needs four.  We make sure that the stored value uses
        // a low or high enough alpha beta value.

};
