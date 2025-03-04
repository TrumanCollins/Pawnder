
#include <glob.h>
#include <chngdpth.h>

void change_depth(
        int player // The player to change.
    )
    // This procedure will allow the user to change the maximum depth
    // that the computer is currently set to look to.
    {
        int new_depth;  // Holds the input depth.


        cout << "\n\nChange Computer's Maximum Search Depth.\n";
        cout << "    Computer is playing ";
        if(player ==White) cout << "white\n";
        else cout << "black\n";
        cout << "\n\nComputer's current maximum depth is: " << glob__max_depth[player] << '\n';
        cout << "    Input the Computer's new maximum search depth (1 - 20): ";
        cin >> new_depth;
        while(new_depth < 1 || new_depth > 20) {
            cout << "Depth not in bounds.  Try again.\n\n";
            cout << "    Input the Computer's maximum search depth (1 - 20): ";
            cin >> new_depth;
        }
        glob__max_depth[player] = new_depth;
    }

