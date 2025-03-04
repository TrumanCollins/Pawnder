
#include <glob.h>
#include <debug.h>
#include <hashbrd.h>
#include <hashtabl.h>
#include <position.h>


#if Use_Hashtable

// Class for hash table entries.

class Hash_entry_type {

public :

    //
    //  Data.
    //

    Hash_board_type   board_state;
        // The board.

    short             depth;
        // The depth that this value is for.

    int               alpha_beta;
        // The alpha beta value.

    short             value;
        // Value of this position.

    Hash_entry_type  *next;
        // Next one on list.

};


Hashtable_type::Hashtable_type()
    // Create a hashtable with nulls for all table entries.
    // Create initial free list.
    {
        int              i;
        Hash_entry_type *new_entry;


        for(i = 0; i < HASH_TABLE_SIZE; i++) table[i] = NULL;

        freelist = NULL;
        glob__hash_nodes_allocated = 0;
        glob__hash_boards_used = 0;
        glob__hash_mask_off = 0x7;
        glob__hash_mask_on  = 0;
        for(i = 1; i <= HASH_BITS; i++) {
            glob__hash_mask_off = glob__hash_mask_off << 1;
            glob__hash_mask_on  = (glob__hash_mask_on << 1) + 1;
        }
        for(i = 1; i <= 60000; i++) {
            glob__hash_nodes_allocated++;
            new_entry = new Hash_entry_type;
            new_entry->next = freelist;
            freelist = new_entry;
        }
    }


void Hashtable_type::clear_table()
    // Clear the hashtable returning the nodes to the freelist.
    {
        int              i;
        Hash_entry_type *temp;
        int              used_buckets = 0;

        for(i = 0; i < HASH_TABLE_SIZE; i++) {
            if(table[i]) used_buckets++;
            while(table[i]) {
                temp = table[i];
                table[i] = temp->next;
                temp->next = freelist;
                freelist = temp;
            }
        }
        if(Debug14 && glob__hash_boards_used) cout << "Total hashed boards: " << glob__hash_boards_used << ".  " << used_buckets << " out of " << HASH_TABLE_SIZE << " buckets used.  " << (double) used_buckets * 100 / HASH_TABLE_SIZE << "% usage\n";
        glob__hash_boards_used = 0;
    }


void Hashtable_type::add_to_table(Hash_board_type *board, short value, short depth, int alpha_beta)
    // Add this board to table if there is enought memory.
    {
        int              hash_val = board->hashvalue();
        Hash_entry_type *new_entry;


        // First allocate the node to hold the board and value.

        if(freelist) {
            new_entry = freelist;
            freelist  = new_entry->next;
        } else {
            return;
//            glob__hash_nodes_allocated++;
//            new_entry = new Hash_entry_type;
        }

        // Initialize it and put it in the table.

        new_entry->value = value;
        new_entry->depth = depth;
        new_entry->alpha_beta = alpha_beta;
        new_entry->board_state = *board;
        new_entry->next = table[hash_val];
        table[hash_val] = new_entry;
        glob__hash_boards_used++;
    }


int Hashtable_type::check_table(Hash_board_type *board, int *value, short depth, int alpha_beta, char comp_color)
    // Look for this board in the table and return true if found.
    // Set value if found.  If the position is found, but the
    // depth of the found board is greater than the depth passed
    // in, it is not considered found.  This is so we don't use
    // a value generated with only two levels of search for one
    // that needs four.  We make sure that the stored value uses
    // a low or high enough alpha beta value.
    {
        int              hash_val = board->hashvalue();
        Hash_entry_type *temp     = table[hash_val];


        if(board->get_mover() == comp_color) {
            while(temp) {
                if(temp->depth <= depth && temp->board_state == *board && (temp->alpha_beta >= alpha_beta || temp->value > alpha_beta)) {
                    *value = temp->value;
                    return(True);
                }
                temp = temp->next;
            }
        } else {
            while(temp) {
                if(temp->depth <= depth && temp->board_state == *board && (temp->alpha_beta <= alpha_beta || temp->value < alpha_beta)) {
                    *value = temp->value;
                    return(True);
                }
                temp = temp->next;
            }
        }
        return(False);
    }

#endif

