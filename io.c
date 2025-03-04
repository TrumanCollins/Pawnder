
#include <string.h>
#include <glob.h>
#include <hashbrd.h>
#include <position.h>
#include <utility.h>
#include <io.h>

// This file contains IO related routines.

static int note_calls = 0;

void write_thinking_message()
    {
        if(!glob__running_under_winboard) {
            cout << "Please wait, I'm thinking...\n";
            cout.flush();
        }
    }


void newline()
    {
        if(!glob__running_under_winboard) cout << '\n';
        cout.flush();
    }


void print_move_text_prefix()
    {
        if(glob__running_under_winboard) {
            cout << glob__current_move << ". ... ";
        } else {
            cout << "Move #: " << glob__current_move << "  My move is ";
        }
        cout.flush();
    }


void write_out_computer_move(bd_index from_loc, bd_index to_loc, int taken, int value)
    {
        char            adj_char;          // Used to adjust to lower case for xchess.


        print_move_text_prefix();

        // Write out for glob__running_under_winboard in lower case.

        if(glob__running_under_winboard) adj_char = 'a' - 'A';
        else adj_char = 0;

        cout << (char) (file_to_char(file_number(from_loc)) + adj_char) << rank_number(from_loc);
        if(!glob__running_under_winboard) {
            if(taken) cout << 'x';
            else cout << '-';
        }
        cout << (char) (file_to_char(file_number(to_loc)) + adj_char) << rank_number(to_loc);
        if(glob__running_under_winboard) cout << '\n';
        else cout << " with a value of " << value;
        cout.flush();
    }


void write_out_time(Position_type  *posit)
    {
        if(!glob__running_under_winboard) {
            if(posit->mover == White) cout << "\nWhite ";
            else cout << "\nBlack ";
            if(glob__use_fischer_timing) {
                cout << "Time: " << glob__elapsed_time << "    Available: "
                     << glob__time_left_in_tc[posit->mover] / 60 << " min " << glob__time_left_in_tc[posit->mover] % 60 << " sec.\n";
            } else {
                cout << "Time: " << glob__elapsed_time << "    "
                     << "Total time: " << glob__total_time[posit->mover] / 60 << " min " << glob__total_time[posit->mover] % 60 << " sec.\n";
            }
        }
        cout.flush();
    }


void write_message_to_request_move()
    {
        if(!glob__running_under_winboard) cout << "What is your move? (X for a command or quit) ";
    }


void write_bad_input()
    {
        if(!glob__running_under_winboard) cout << "Bad input, try again.\n\n";
    }


void write_move_input(char *input)
    {
        if(glob__running_under_winboard) {
            cout << glob__current_move << ". " << input << '\n';
            cout.flush();
        }
    }


void get_file_to_put_move_list_in()
    {
        int   count;
        char  file_name[100];
        char  temp[20];


        if(glob__running_under_winboard) {

            // Find unique file name.

            count = 0;
            do {
                count++;
                strcpy(file_name, "chess_lst.");
                utility_itoa(count, temp);
                strcat(file_name, temp);
            } while(file_exists(file_name) || glob__list_file_buf.open(file_name, ios::out) == 0);
        } else {
            do {
                cout << "\nInput the file name of where to put the list of moves: ";
                cin >> file_name;
            } while(!glob__list_file_buf.open(file_name, ios::out));
        }
        glob__list_file = new ostream(&glob__list_file_buf);
    }


void return_to_continue()
    {
        if(!glob__running_under_winboard) {
            clear_input_stream();
            cout << "\n\nPress return to continue.";
            clear_input_stream();
        }
    }


void write_check()
    {
        if(!glob__running_under_winboard) cout << " Check!";
    }


void write_checkmate()
    {
        if(!glob__running_under_winboard) cout << "Checkmate!\n";
    }


void write_draw_50()
    {
        if(!glob__running_under_winboard) cout << "Draw by 50 move rule.\n";
    }

void write_draw_repetition()
    {
        if(!glob__running_under_winboard) cout << "Draw by repetition.\n";
    }

void write_stalemate()
    {
        if(!glob__running_under_winboard) cout << "Stalemate.\n";
    }

void write_reading_library()
    {
        if(!glob__running_under_winboard) {
            cout << "Reading opening library";
            cout.flush();
        }
    }

void note_progress()
    // Only write a . every 10 opening moves processed.
    {
        if(!glob__running_under_winboard) {
            if(++note_calls == 10) {
                cout << '.';
                cout.flush();
                note_calls = 0;
            }
        }
    }


