void generate_computer_move(
        Position_type  *posit,    // The current position.
        int             level,    // The level to look to.
        int            *done      // Set to true if end of game.
    );
    // This function will generate a move for the given color and
    // the given position.  It does this by looking ahead to the
    // specified depth in the game tree starting at the given
    // position.  It uses a modified Alpha-Beta search method.
    // If the game is ended, the variable done is set to true.

