
#include <glob.h>
#include <killer.h>


#define KILLERS_TO_SEARCH 5

Killer_type::Killer_type()
    {
        int i;

        for(i = 1; i < 20; i++) level_array[i].max_index = 0;
    }


void Killer_type::save_killer(int level, Gen_move_type *move)
    {
        int          i;
        struct move *killer    = &level_array[level].killers[0];
        int          max_ind   = level_array[level].max_index;
        int          move_code = move->from_loc << 8 | move->to_loc;
        struct move  swap;


        for(i = 0; i < max_ind; i++) {
            if(move_code == killer[i].the_move) {

                // If we found the move, increment the count, and
                // sort the list by moving this entry up if needed.

                killer[i].count++;
                while(i && killer[i].count > killer[i-1].count) {
                    swap = killer[i];
                    killer[i] = killer[i-1];
                    killer[i-1] = swap;
                    i--;
                }
                return;
            }
        }

        // The move was not found, so add it to the end of the killer array.

        if(max_ind < 10) {
            killer[max_ind].count = 1;
            killer[max_ind].the_move = move_code;
            level_array[level].max_index++;
        }
    }

// static int killers_found = 0;
int Killer_type::is_killer(int level, Gen_move_type *move)
    {
        int          i;
        struct move *killer    = &level_array[level].killers[0];
        int          max_ind   = level_array[level].max_index;
        int          move_code = move->from_loc << 8 | move->to_loc;


//                killers_found++;
        for(i = 0; i < max_ind && i < KILLERS_TO_SEARCH; i++) {
            if(move_code == killer[i].the_move) {
                return(True);
            }
        }
        return(False);
    }


void Killer_type::clear_killers()
    {
        int i;

        for(i = 1; i < 20; i++) {
            level_array[i].max_index = 0;
        }
    }


void Killer_type::print_killers()
    {
        int i, j;

cerr << "Print killer called.\n";
        for(i = 1; i < 20; i++) {
            if(level_array[i].max_index) {
                cerr << "Killers for level " << i << "  ";
                for(j = 0; j < level_array[i].max_index && j < KILLERS_TO_SEARCH; j++) {
                    cerr << (char) file_as_char(level_array[i].killers[j].the_move >> 8)
                         << (char) rank_as_char(level_array[i].killers[j].the_move >> 8)
                         << "-"
                         << (char) file_as_char(level_array[i].killers[j].the_move & 0xFF)
                         << (char) rank_as_char(level_array[i].killers[j].the_move & 0xFF)
                         << " " << level_array[i].killers[j].count << "  ";
                }
                cerr << "\n";
            }
        }
// cerr << "is_killer called " << killers_found << " times.\n";
// killers_found = 0;
    }

