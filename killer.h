//
// HEADER
//    KILLER.H - Holds killer moves found for all levels.
//
// WRITTEN BY:
//    Truman Collins
//
// DESCRIPTION
//    This class implements a structure to keep track of killer
//    moves found for different levels in the tree search.
//    Looking at killer moves first causes more alpha-beta cutoff.
//
// HISTORY
//    T.C. January 1993 - Initial code.


    struct move {
        int the_move;
        int count;
    };

    struct level {
        int          max_index;
        struct move  killers[15];
    };

class Killer_type {

private :

    //
    //  Data.
    //

    struct level   level_array[20];
      // Holds array of levels.  Each level holds an array of killers.

public :

    //
    //  Member functions.
    //

    Killer_type();
        // Create a new killer object.

    void save_killer(int level, Gen_move_type *move);
        // Puts the move in the killer array corresponding to level.

    int is_killer(int level, Gen_move_type *move);
        // Returns true if this move is in the first few most common killers
        // in the array associated with the given level.
        // The number of most common killers looked at is determined by
        // a define in killer.c

    void clear_killers();
        // Clears all of the killer arrays.

    void print_killers();
        // Prints out all killer moves.

};
