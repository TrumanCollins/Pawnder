
// Debugging variables.

//#define Debug2     (0 & !Xchess)
//    // This will cause the program to print out the
//    // moves generated by GenMoves.
//#define Debug3     (0 & !Xchess)
//    // This will cause a printout of all moves with
//    // their values after GenerateComputerMove
//    // sorts the list.
//#define Debug4     (1 & !Xchess)
//    // This will cause GenerateComputerMove to
//    // print out the number of times the evaluator
//    // function is called to make a move.
//#define Debug5     (0 & !Xchess)
//    // This will cause the ToForsyth procedure to
//    // print out the board and the code representing
//    // the board whenever it is called.  It will do
//    // this if this variable is set to True.
//#define Debug6     (0 & !Xchess)
//    // If this is set then after each move is made on
//    // the board we check to see that all of the 
//    // extra data is correct in the position record.
//#define Debug7     (0 & !Xchess)
//    // If set then the expected line of play will be displayed
//    // after each move.
//#define Debug8     (0 & !Xchess)
//    // If set it will give timings on move processing for each move 
//    // at each level and for each level.
#define Debug9     (0 & !glob__running_under_winboard)
    // If set, generated moves will be printed out by priority classes.
#define Debug10    (0 & !glob__running_under_winboard)
    // If set optimizations to will_king_be_in_check will be compared with
    // the previous less efficient approach.
#define Debug11    (0 & !glob__running_under_winboard)
    // If true call evaluate1 and evaluate2 based on black white.
#define Debug12    (0 & !glob__running_under_winboard)
    // If true check consistency of hash_board after each move made.
#define Debug13    (1 & !glob__running_under_winboard)
    // Check for computer used too much time.
#define Debug14    (0 & !glob__running_under_winboard)
    // Print out hashtable bucket usage information.
#define Debug15    (0 & !glob__running_under_winboard)
    // Check consistency of piece_locations array after each make_move_on_board.
#define Debug16    (0 & !glob__running_under_winboard)
    // Count and print moves generated by priority.
#define Debug17    (0 & !glob__running_under_winboard)
    // Print out killer moves on each level.

