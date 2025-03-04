int give_me_a_value(
        Position_type  *posit,      // The position.
        Gen_move_type  *move,       // The move to give value of.
        int             depth,      // The depth to search to.
        int             curr_depth, // The depth currently being searched.
        int             alpha_beta, // The alpha beta value for min-max.
        char            comp_color, // The color of the computer.
        Gen_move_type **best_line   // The best seen continuation.
    );
    // This procedure is sent the position and a pointer variable which points
    // to a record holding a move.  This move is made and then the tree from
    // the corresponding position is evaluated by generating all possible
    // moves and then using this procedure to evaluate each one.  If the
    // appropriate debug flag is set, best_line will be filled with a pointer
    // to a move list of the best line from here.  If we are set to use the
    // hashtable, this is checked and filled in for levels that are greater
    // than 2 and less than the maximum level.

