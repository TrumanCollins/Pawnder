
#include <glob.h>
#include <chngmxtm.h>

void change_max_time(
        int player,    // The color of the player to change.
        int secondary  // True if change secondary time control.
    )
    // This function will allow the user to change the time control for
    // the computer.
    {            
        int  seconds;

        if(!secondary) {

            // Let the user input time control information.

            cout << "\n\nChange Computer's time control.\n";
            cout << "    Current time control for computer playing ";
            if(player == White) cout << "white";
            else cout << "black";
            cout << " is " << glob__moves_per_tc[player] << " per " << glob__time_per_tc[player] / 60 << " minutes.\n";
            cout << "Input the new number of moves per time control: ";
            cin >> glob__moves_per_tc[player];
            glob__moves_left_in_tc[player] = glob__moves_per_tc[player];
            cout << "Input the number of minutes per time control: ";
            cin >> glob__time_per_tc[player];
            glob__time_per_tc[player] = glob__time_per_tc[player] * 60;
            glob__time_left_in_tc[player] = glob__time_per_tc[player];
        } else {

            // Let the user input secondary time control information.

            cout << "\n\nChange Computer's time control.\n";
            cout << "    Current secondary time control for computer playing ";
            if(player == White) cout << "white";
            else cout << "black";
            cout << " is " << glob__moves_per_stc[player] << " per " << glob__time_per_stc[player] / 60 << " minutes.\n";
            cout << "Input the new number of moves per time control: ";
            cin >> glob__moves_per_stc[player];
            cout << "Input the number of minutes per time control: ";
            cin >> glob__time_per_stc[player];
            glob__time_per_stc[player] = glob__time_per_stc[player] * 60;
        }
    }

