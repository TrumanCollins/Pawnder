
#include <glob.h>
#include <ctype.h>
#include <stdlib.h>
#include <debug.h>
#include <hashbrd.h>
#include <io.h>
#include <string.h>
#include <position.h>
#include <utility.h>
#include <openbook.h>


// This is used to hold a move in the opening library structure.

class Lib_move_type {

public :

    bd_index       from_loc;
    bd_index       to_loc;
    short          probability;
    Lib_move_type *next;

    Lib_move_type() {}

    Lib_move_type(bd_index f_loc, bd_index t_loc, short prob)
        { from_loc = f_loc; to_loc = t_loc; probability = prob; next = NULL; }

};


// This is used to hold a position in the opening library structure.

class Lib_pos_type {

public :

    char           *code;
    Lib_move_type  *move_list;
    Lib_pos_type   *next_left;
    Lib_pos_type   *next_right;

    Lib_pos_type() {}

    Lib_pos_type(char *cd, Lib_move_type *m_l, Lib_pos_type *n_l, Lib_pos_type *n_r)
        { code = cd; move_list = m_l; next_left = n_l; next_right = n_r; }

};


Open_book_type::Open_book_type()
    {
        char           color;            // Holds color.
        Position_type  current_position; // Scratch position for library work.
        bd_index       from_loc;         // Holds location the move is from.
        filebuf        lib_file_buf;     // The library file.
        int            move_numb;        // Holds the move number;
        int            probab;           // Holds the probability the opening move is made.
        bd_index       to_loc;           // Holds the location the move is to.


        library_top  = NULL;
        current_position.setup_board();
        if(lib_file_buf.open("openlibr.bok", ios::in)) {
            write_reading_library();
            istream lib_file(&lib_file_buf);
            read_next_line(lib_file, &move_numb, &color, &from_loc, &to_loc, &probab);
            while(move_numb) {
                process_opening(lib_file, &current_position, &move_numb, &color, &from_loc, &to_loc, &probab);
            }
            newline();
        }
    }


int Open_book_type::check_library(
        Position_type  *posit, // The current position.
        Gen_move_type **move   // Filled in if found.
    )
    // This function will look through the opening library for the given
    // position and color to move.  If it is not found then False will be
    // returned.  If it is found, True will be returned, and a new move
    // object will be returned in move.
    {
        Gen_move_type *chosen_mv;     // The move chosen.
        char           code1[70];      // Holds the Forsyth notation of the position.
        int            comp_val;      // Comparison of strings.
        int            rand_numb;     // Used to select move.
        int            sum;           // Holds sum of weightings.
        Lib_move_type *temp_move;     // Pointer to a move on list.
        Lib_pos_type  *temp_pos;      // Temporary pointer to position in tree.


        posit->to_forsyth(code1);

        // Look for the current position in the tree.

        temp_pos = library_top;
        while(temp_pos) {
            comp_val = strcmp(code1, temp_pos->code);
            if(!comp_val) break;
            if(comp_val > 1) temp_pos = temp_pos->next_right;
            else temp_pos = temp_pos->next_left;
        }

        // If we found it then select a move from the list of choices.

        if(!temp_pos) return(False);

        // Find the sum of the weightings.  If sum is 0 we don't want to make
        // a book move.

        for(temp_move = temp_pos->move_list, sum = 0; temp_move; temp_move = temp_move->next) sum += temp_move->probability;
        if(sum == 0) return(False);

        // Choose move randomly, but by weightings.

        rand_numb = rand() % sum + 1;

        temp_move = temp_pos->move_list;
        do {
            if(rand_numb <= temp_move->probability) {
                chosen_mv = new_move_type(temp_move->from_loc, temp_move->to_loc, 0, NULL);
                *move = chosen_mv;
                return(True);
            }
            rand_numb -= temp_move->probability;
            temp_move = temp_move->next;
        } while(True);
    }


void Open_book_type::add_opening(
        Position_type  *posit,    // The position that this move is for.
        bd_index        from_loc, // The location to move from.
        bd_index        to_loc,   // The location to move to.
        int             probab    // The probability for this move.
    )
    // This routine will add the move passed in to the tree containing the
    // opening library.  The position passed will identify the position to
    // make the move from.
    {
        char          *code;          // Holds the Forsyth notation of the position.
        int            comp_val;      // Used for string comparison.
        char           insert;        // Flag for whether to insert move or not.
        Lib_move_type *move;          // Holds the move to put in.
        Lib_move_type *move_temp;     // Used to find where to put move in list.
        Lib_pos_type  *old_pos;       // Temporary position pointer.
        char           temp_code[70]; // Temporary holder for Forsyth code.
        Lib_pos_type  *temp_pos;      // Temporary position pointer.


        posit->to_forsyth(temp_code);
        code = new char[strlen(temp_code)+1];
        strcpy(code, temp_code);

        if(library_top) {

            // Look for this position in the tree.
            // If not found, then old_pos points at the position node
            // which the new position node should be hung off of.

            temp_pos = library_top;

            while(temp_pos) {
                comp_val = strcmp(code, temp_pos->code);
                if(!comp_val) break;
                old_pos = temp_pos;
                if(comp_val > 1) temp_pos = temp_pos->next_right;
                else temp_pos = temp_pos->next_left;
            }

            if(temp_pos) {

                // Here we have found the position.  We make sure the
                // move is in the list.

                delete code;

                for(insert = True, move_temp = temp_pos->move_list; move_temp && insert; move_temp = move_temp->next) {
                    if(from_loc == move_temp->from_loc && to_loc == move_temp->to_loc) insert = False;
                }

                // Insert the move if it didn't already exist.

                if(insert) {
                    move = new Lib_move_type(from_loc, to_loc, probab);
                    move->next = temp_pos->move_list;
                    temp_pos->move_list = move;
                }
            } else {

                // Here the position is not in the current library, so add it to the library
                // along with the move.

                move = new Lib_move_type(from_loc, to_loc, probab);
                temp_pos = new Lib_pos_type(code, move, NULL, NULL);
                if(comp_val > 1) old_pos->next_right = temp_pos;
                else old_pos->next_left = temp_pos;
            }
        } else {

            // This is the first position on the list.

            move = new Lib_move_type(from_loc, to_loc, probab);
            library_top = new Lib_pos_type(code, move, NULL, NULL);
        }
    }


void Open_book_type::process_opening(
        istream       &lib_in,    // The stream to read from.
        Position_type *old_posit, // The current position before this move.
        int           *move_numb, // The move number of the move that was read.
        char          *color,     // The color of the side that made the move.
        bd_index      *from_loc,  // The location the move was from.
        bd_index      *to_loc,    // The location the move was to.
        int           *probab     // The probability to make this move.
    )
    // This is a recursive function that takes a position and a move.
    // It makes the move on the board in position and then stores
    // the move associated with the position using the routine
    // add_opening.
    {
        Gen_move_type   move;         // Holds the move.
        char            new_color;    // Newly read color
        bd_index        new_from_loc; // Newly read from location.
        int             new_numb;     // Newly read move number.
        int             new_probab;   // Newly read probability.
        bd_index        new_to_loc;   // Newly read to location.
        char            next_color;   // The next expected color.
        int             next_numb;    // The next expected move number.
        Position_type  *posit;        // The position after this move.


        // Make sure the move is legal.

        posit = new_position();
        *posit = *old_posit;

        if(!posit->check_for_legal_move(*from_loc, *to_loc)) goto error_code;

        note_progress();
        if(*color == White) {
            next_color = Black;
            next_numb  = *move_numb;
        } else {
            next_color = White;
            next_numb  = *move_numb + 1;
        }

        // Make the move on the current board.

        move.from_loc = *from_loc;
        move.to_loc   = *to_loc;

        // Add opening to library.

        add_opening(posit, *from_loc, *to_loc, *probab);

        posit->make_move_on_board(&move);

        // Get information on next move.

        read_next_line(lib_in, &new_numb, &new_color, &new_from_loc, &new_to_loc, &new_probab);

        if(new_numb > next_numb || (new_numb == next_numb && next_color == White && new_color == Black)) goto error_code;

        while(new_numb == next_numb && new_color == next_color) {
            process_opening(lib_in, posit, &new_numb, &new_color, &new_from_loc, &new_to_loc, &new_probab);
        }

        *move_numb = new_numb;
        *color     = new_color;
        *from_loc  = new_from_loc;
        *to_loc    = new_to_loc;
        *probab    = new_probab;

        delete_position(posit);
        return;

    error_code :

        // The move was bad, so generate an error message.

        cerr << "An illegal move was found in the openings file.\n" << "Move number was: " << *move_numb << ", probability was: " << *probab << ".\n";
        cerr << "Move was: " << (char) (file_to_char(file_number(*from_loc))) << rank_number(*from_loc);
        cerr << "-" << (char) (file_to_char(file_number(*to_loc))) << rank_number(*to_loc) << "\n";
        delete_position(posit);
        exit(1);
    }


#define check_for_error(ist, lab) if(ist.rdstate() != ifstream::goodbit) goto lab

#define skip_white_space(ist, ch) do ist.get(ch); while(isspace(ch) && ist.rdstate() == ifstream::goodbit)

void Open_book_type::read_next_line(
        istream  &lib_in,    // The stream to read from.
        int      *move_numb, // The move number of the move that was read.
        char     *color,     // The color of the side that made the move.
        bd_index *from_loc,  // The location the move was from.
        bd_index *to_loc,    // The location the move was to.
        int      *probab     // The probability to make this move.
    )
    // The function will read the next useful line from the opening library
    // file.  It will put the information from this line into the locations
    // passed.  We ignore all lines starting with '#', and all spaces before
    // the first non-space character.  If there is a problem with the input
    // we will exit.  Move_numb will be set to 0 when we reach the end of file.
    {
        char   ch;
        char   col;
        int    i;
        char   move[5];


        if(lib_in.rdstate() != ifstream::goodbit) {
            *move_numb = 0;
            return;
        }

        skip_white_space(lib_in, ch);

        // Skip comments.

        while(ch == '#' && lib_in.rdstate() == ifstream::goodbit) {

            // Ignore this line.

            do lib_in.get(ch); while(ch != '\n' && lib_in.rdstate() == ifstream::goodbit);

            skip_white_space(lib_in, ch);
        }

        if(lib_in.rdstate() != ifstream::goodbit) {
            *move_numb = 0;
            return;
        }

        // Read move number and color.

        *move_numb = 0;
        while(isdigit(ch)) {
            *move_numb = *move_numb * 10 + (ch - '0');
            lib_in.get(ch);
        }

        check_for_error(lib_in, error_code);

        while(isspace(ch) && lib_in.rdstate() == ifstream::goodbit) lib_in.get(ch);

        col = ch;

        check_for_error(lib_in, error_code);

        skip_white_space(lib_in, ch);

        // Put the move into a buffer.

        move[0] = ch;
        for(i = 1; i <= 3; i++) lib_in.get(move[i]);
        move[4] = '\0';

        check_for_error(lib_in, error_code);

        lib_in >> *probab;

        // Check for good input.

        if(*move_numb < 1) goto error_code;

        if(*probab < 0 || *probab > 10) goto error_code;

        for(i = 0; i <= 2; i += 2) {
            if(move[i] >= 'A' && move[i] <= 'H') move[i] = move[i] + ('a' - 'A');
            if(move[i] < 'a' || move[i] > 'h') goto error_code;
        }
        for(i = 1; i <= 3; i += 2) {
            if(move[i] < '1' || move[i] > '8') goto error_code;
        }

        if(col == 'w' || col == 'W') *color = White;
        else if(col == 'b' || col == 'B') *color = Black;
        else goto error_code;

        *from_loc = single_index_from_chars(move[0], move[1]);
        *to_loc = single_index_from_chars(move[2], move[3]);

        return;

        // This code handles an error.

    error_code :

        cerr << "There has been an error in the openings file.\nThe problem occurred at the following entry: " << move << '\n';
        exit(1);
    }

