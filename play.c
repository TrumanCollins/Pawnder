
#include <glob.h>
#include <gncmpmov.h>
#include <hashbrd.h>
#include <hashtabl.h>
#include <io.h>
#include <letusrmv.h>
#include <position.h>
#include <utility.h>
#include <play.h>

void play(
        Position_type  *posit  // The current position.
    )
    // This function will start the game from the current position.
    {
        int   done;


        get_file_to_put_move_list_in();

        *glob__list_file << "       White    Value  Depth  Time       Black    Value  Depth  Time \n";

        done = False;

        // Loop until end of game or user wants to stop.

        do {
            if(glob__print_pos) print_board(posit);
            if(!glob__human_against_human && (posit->mover == glob__computer_color || glob__computer_against_computer)) {
                generate_computer_move(posit, glob__max_depth[posit->mover], &done);
#if Use_Hashtable
                glob__hash_table->clear_table();
#endif
            } else {
                let_user_move(posit, &done);
            }
            if(posit->mover == White) glob__current_move++;

        } while(!done);

        delete glob__list_file;
        glob__list_file_buf.close();

        return_to_continue();
    }

