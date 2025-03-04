
#include <glob.h>
#include <chngdpth.h>
#include <chngmxtm.h>
#include <hashbrd.h>
#include <initial.h>
#include <position.h>
#include <utility.h>
#include <docomand.h>

int do_command(
        Position_type  *posit  // The current position.
    )
    // This function will display a menu with available functions when
    // the game is in progress.
    {
        char  code[70];               // Holds the Forsyth notation of the position.
        int   done = False;           // Flag for quit.
        int   option;                 // The input option.
        int   user_finished = False;  // Flag for done.


        do {
            cout << "            Command Menu\n\n";
            cout << "      <0> Change Maximum Search Depth\n";
            cout << "      <1> Change Time Control\n";
            cout << "      <2> Toggle Board Printed Before Move\n";
            cout << "      <3> Print Forsyth Notation of Board\n";
            cout << "      <4> End Game Now\n";
            cout << "      <5> Return To Playing\n\n\n";
            cout << "          Choose Option (0 - 5): ";
            cin  >> option;
            cout.put('\n');
            switch(option) {
                case 0 : {
                    change_depth(glob__computer_color);
                    break;
                }
                case 1 : {
                    change_max_time(glob__computer_color);
                    break;
                }
                case 2 : {
                    if(glob__print_pos) glob__print_pos = False;
                    else glob__print_pos = True;
                    break;
                }
                case 3 : {
                    posit->to_forsyth(code);
                    cout << "The Forsyth code of this position is:\n    " << code << "\n\n\n\n";
                    break;
                }
                case 4 : {
                    done = True;
                    glob__list_file_buf.close();
                    initialize_variables();
                    user_finished =  True;
                    break;
                }
                case 5 : {
                    user_finished =  True;
                    if(glob__print_pos) print_board(posit);
                    break;
                }
                case 6 : {
                    glob__human_against_human = False;
                    if(posit->mover == Black) {
                        glob__computer_color = Black;
                        glob__max_depth[Black] = glob__max_depth[White];
                        glob__time_per_tc[Black] = glob__time_per_tc[White];
                        glob__time_left_in_tc[Black] = glob__time_left_in_tc[White];
                        glob__moves_per_tc[Black] = glob__moves_per_tc[White];
                        glob__moves_left_in_tc[Black] = glob__moves_left_in_tc[White];
                    } else {
                        glob__computer_color = White;
                        glob__max_depth[White] = glob__max_depth[Black];
                        glob__time_per_tc[White] = glob__time_per_tc[Black];
                        glob__time_left_in_tc[White] = glob__time_left_in_tc[Black];
                        glob__moves_per_tc[White] = glob__moves_per_tc[Black];
                        glob__moves_left_in_tc[White] = glob__moves_left_in_tc[Black];
                    }
                    break;
                }
                default : {
                    cout << "    Bad input.  Try again.\n";
                    break;
                }
            }
        } while(!user_finished);

        return(done);
    }

