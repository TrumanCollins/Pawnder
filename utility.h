
void utility_itoa(int n, char *s);
    // Convert into to char string.

int file_exists(char *file_name);
    // Returns True if file exists.


void clear_input_stream();
    // This routine will read the remaining characters from cin including
    // the next '\n'.  It will stop after that.

Gen_move_type *sort_moves(
        Gen_move_type *move_top   // Points to the first move in list.
    );
    // This procedure will sort the list according to the variable Value.
    // Higher values will come first.  An insertion sort is used.  This
    // Could clearly be sped up.


Gen_move_type *sort_moves_reverse(
        Gen_move_type *move_top   // Points to the first move in list.
    );
    // This procedure will sort the list according to the variable Value.
    // Lower values will come first.  An insertion sort is used.  This
    // Could clearly be sped up.


void append_move_list(
    );
    // This routine will put the move lists together and set glob__move_top
    // to the first move.  The move lists are ordered as follows:
    // glob__moves_high_priority, glob__moves_med_priority,
    // and glob__moves_low_priority.


void add_move(
        bd_index  from_loc,      // The location the piece moved from.
        bd_index  to_loc         // The location the piece moved to.
    );
    // This function will add the move to the appropriate move list.
    // There are three move lists, based on the priority of the move.
    // Ideally, we would like to put the moves on the list in best to
    // worst order.  This would maximize alpha-beta pruning.
    // Unfortunately, we need to use a quick heuristic based on the
    // current position.  In general we put captures early.  Some
    // of the choices of lists may seem counter intuitive, but they
    // worked best for this set of tests.


void free_list(
        Gen_move_type *point  // Pointer to top of list to dispose of.
    );
    // This function will dispose of all of the
    // nodes from Point down in the list.


void print_board(
        Position_type *posit  // Holds current position of board.
    );
    // This procedure will print the current position on the board in the
    // far upper left part of the screen.


char file_to_char(int in_file);
    // Returns a character corresponding to this number file with
    // 2 being 'A' and 9 being 'H'.


void print_move_list(
        Position_type *posit,              // Holds current position of the board.
        Gen_move_type *move_top,           // Pointer to top of moves to print.
        int            prn_board = True    // Board will be printed if true.
    );
    // This is a debugging procedure which will print out all of the moves in
    // the list of moves pointed to by move_top.

void print_continuation(
        Gen_move_type  *forseen_line  // The forseen line of play.
    );
    // This will print out the list of moves as the forseen line of play.

