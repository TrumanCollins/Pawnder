
#include <glob.h>
#include <chngdpth.h>
#include <chngmxtm.h>
#include <evaluate.h>
#include <hashbrd.h>
#include <initial.h>
#include <play.h>
#include <position.h>
#include <dispmenu.h>

void display_menu()
     // This function will display the top level menu in this program.
     // All functions of this program are selected from this menu.
    {
        int     option;          // The option read in.
        int     user_finished;   // Set to true when the user is done.
        int     input;           // Used when inputting information.


        user_finished = False;
//cout << "Position type size = " << sizeof(Position_type) << "\n";
        // If running for Xchess then use minimal output.

        if(glob__running_under_winboard) {
            glob__new_board  = False;
            cout << "Chess\n";
            // Put in to make xboard happy.
            cout << "transposition table is 131072\n";
            play(&glob__current_position);
            return;
        }
        do {
            cout << "\n\n\n\n\n";
            cout << "            - - C H E S S - -\n\n";
            cout << "            By Truman Collins\n\n";
            cout << "      Current Board Condition    : ";
            if(glob__new_board) cout << "New\n";
            else cout << "Not New\n";
            cout << "      Current Computer Color     : ";
            if(glob__computer_color == Black) cout << "Black\n";
            else cout << "White\n";
            cout << "      Current Computer Max Depth : " << glob__max_depth[glob__computer_color] << '\n';
            if(glob__use_fischer_timing) {
                cout << "      Fischer time control       : " << glob__fischer_base_time[glob__computer_color] << " secs base. " << glob__fischer_time_inc[glob__computer_color] << " secs inc.\n";
            } else {
                cout << "      Computer's time control    : " << glob__moves_per_tc[glob__computer_color] << " in " << glob__time_per_tc[glob__computer_color] / 60 << " minutes.\n";
            }
            cout << "      Color To Move              : ";
            if(glob__current_position.mover == White) cout << "White\n";
            else cout << "Black\n";
            cout << "      Opening Library To Be Used : ";
            if(glob__use_library) cout << "Yes\n";
            else cout << "No\n";
            cout << "      Board Printed Before Move  : ";
            if(glob__print_pos) cout << "Yes\n\n";
            else cout << "No\n\n";
            cout << "      <0> Start Play\n";
            cout << "      <1> Change Computer's Time Control.\n";
            cout << "      <2> Change Max Search Depth for computer\n";
            cout << "      <3> Toggle Computer's Color\n";
            cout << "      <4> Have The Computer Play Itself\n";
            cout << "      <5> Set Up A Position On The Board\n";
            cout << "      <6> Reset The Board To Its Original Position\n";
            cout << "      <7> Toggle Use Of Opening Library\n";
            cout << "      <8> Toggle Board Printed Before Move\n";
            cout << "      <9> Quit\n";
            cout << "\n\n";
            cout << "          Choose Option (0 - 9): ";
            cin >> option;

            switch(option) {
                case 0  :
                    glob__new_board  = False;
                    play(&glob__current_position);
                    break;
                case 1  :
                    change_max_time(glob__computer_color);
                    break;
                case 2  :
                    change_depth(glob__computer_color);
                    break;
                case 3  :
                    if(glob__computer_color == White) {
                        glob__computer_color = Black;
                        glob__max_depth[Black] = glob__max_depth[White];
                        glob__time_per_tc[Black] = glob__time_per_tc[White];
                        glob__time_left_in_tc[Black] = glob__time_left_in_tc[White];
                        glob__moves_per_tc[Black] = glob__moves_per_tc[White];
                        glob__moves_left_in_tc[Black] = glob__moves_left_in_tc[White];
                        glob__fischer_base_time[Black] = glob__fischer_base_time[White];
                        glob__fischer_time_inc[Black] = glob__fischer_time_inc[White];
                    } else {
                        glob__computer_color = White;
                        glob__max_depth[White] = glob__max_depth[Black];
                        glob__time_per_tc[White] = glob__time_per_tc[Black];
                        glob__time_left_in_tc[White] = glob__time_left_in_tc[Black];
                        glob__moves_per_tc[White] = glob__moves_per_tc[Black];
                        glob__moves_left_in_tc[White] = glob__moves_left_in_tc[Black];
                        glob__fischer_base_time[White] = glob__fischer_base_time[Black];
                        glob__fischer_time_inc[White] = glob__fischer_time_inc[Black];
                    }
                    break;
                case 4  :
                    glob__computer_against_computer = True;
                    cout << "Using maximum depth and time control already set for ";
                    if(glob__computer_color == White) cout << "white.\n\n";
                    else cout << "black.\n\n";
                    change_depth(other_color(glob__computer_color));
                    if(glob__use_fischer_timing) {
                        cout << "Input fischer base time in minutes: ";
                        cin >> input;
                        glob__fischer_base_time[White] = input * 60;
                        glob__fischer_base_time[Black] = input * 60;
                        cout << "Input fischer increment time in seconds: ";
                        cin >> input;
                        glob__fischer_time_inc[White] = input;
                        glob__fischer_time_inc[Black] = input;
                    } else {
                        change_max_time(other_color(glob__computer_color));
                    }
                    glob__new_board = False;
                    play(&glob__current_position);
                    glob__computer_against_computer = False;
                    break;
                case 5  :
                    glob__current_position.set_up_position();
                    glob__new_board = False;
                    break;
                case 6  :
                    initialize_variables();
                    break;
                case 7  :
                    if(glob__use_library) glob__use_library = False;
                    else glob__use_library = True;
                    break;
                case 8  :
                    if(glob__print_pos) glob__print_pos = False;
                    else glob__print_pos = True;
                    break;
                case 9  :
                    user_finished = True;
                    break;
                case 10 :
                    cout << "Value is: " << evaluate(&glob__current_position, glob__computer_color) << "\n";
                    break;
                case 11 :
                    glob__human_against_human = True;
                    glob__new_board = False;
                    play(&glob__current_position);
                    glob__human_against_human = False;
                    break;
                case 12 :
                    glob__use_fischer_timing = True;
                    cout << "Input fischer base time in minutes: ";
                    cin >> input;
                    glob__fischer_base_time[White] = input * 60;
                    glob__fischer_base_time[Black] = input * 60;
                    glob__time_left_in_tc[White] = input * 60;
                    glob__time_left_in_tc[Black] = input * 60;
                    cout << "Input fischer increment time in seconds: ";
                    cin >> input;
                    glob__fischer_time_inc[White] = input;
                    glob__fischer_time_inc[Black] = input;
                    cout << "Input operator time in seconds: ";
                    cin >> input;
                    glob__operator_time = input;
                    break;
                default :
                    cout << "Invalid choice.  Try again.\n";
                    break;
            }
        } while(!user_finished);
    }

