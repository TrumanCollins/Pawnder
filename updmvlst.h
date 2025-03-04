void update_move_list(
        Position_type  *posit,  // The current position.
        Gen_move_type  *move,   // The move made.
        int             taken,  // True if this was a capture.
        int            *done    // Set to true if end of game.
    );
    // It will record the move with a dash if no piece was taken and with
    // an X if a piece was taken.  This procedure should be called after
    // MakeMoveOnBoard and it should only be called when an actual move
    // is made.  It should not be called when the computer analizes a
    // position.  Taken must be set to true if a piece was taken and to
    // false if not.

