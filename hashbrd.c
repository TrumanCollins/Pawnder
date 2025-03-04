
#include <glob.h>
#include <hashbrd.h>
#include <hashtabl.h>
#include <position.h>
#include <utility.h>

#if Use_Hashtable

static unsigned short clear_mask[4] = { 0xFFF0, 0xFF0F, 0xF0FF, 0x0FFF };

static unsigned short convert_to_hash_piece(int piece)
    {
        switch(piece) {
            case Empty  :  return(H_Empty);  
            case Wpawn  :  return(H_Wpawn);  
            case Wknight:  return(H_Wknight);
            case Wbishop:  return(H_Wbishop);
            case Wrook  :  return(H_Wrook);  
            case Wqueen :  return(H_Wqueen); 
            case Wking  :  return(H_Wking);  
            case Bpawn  :  return(H_Bpawn);  
            case Bknight:  return(H_Bknight);
            case Bbishop:  return(H_Bbishop);
            case Brook  :  return(H_Brook);  
            case Bqueen :  return(H_Bqueen); 
            case Bking  :  return(H_Bking);  
        }
        return H_Empty;
    }


Hash_board_type::Hash_board_type()
    {
        clear_board();
        mover = White;
    }

int operator==(const Hash_board_type &board1, const Hash_board_type &board2)
    {
        int       i;
        long     *ptr1, *ptr2;


        // Here we do a little cheating for efficiency's sake.  We go
        // through the array as if it were longs instead of shorts.
        // This assumes no alignment.

        if(board1.mover != board2.mover) return(False);
//        for(i = 1; i <= 16; i++) if(board1.board[i] != board2.board[i]) return(False);
        ptr1 = (long *) &board1.board[0];
        ptr2 = (long *) &board2.board[0];
        for(i = 0; i < 8; i++) if(*ptr1++ != *ptr2++) return(False);
        return(True);
    }

void Hash_board_type::clear_board()
    {
        int i;

        for(i = 0; i < 16; i++) board[i] = 0;
    }


void Hash_board_type::add_piece(int location, int piece)
    {
        int            column     = file_number(location);
        int            row        = rank_number(location);
        int            field      = (column - 1) & 0x3;
        unsigned short hash_piece = convert_to_hash_piece(piece);
        int            index      = ((row - 1) << 1) + ((column - 1) >> 2);

        
        board[index] = (board[index] & clear_mask[field]) | (hash_piece << (field << 2));
    }


void Hash_board_type::add_hash_piece(int location, int hash_piece)
    {
        int            column     = file_number(location);
        int            row        = rank_number(location);
        int            field      = (column - 1) & 0x3;
        int            index      = ((row - 1) << 1) + ((column - 1) >> 2);

        
        board[index] = (board[index] & clear_mask[field]) | (hash_piece << (field << 2));
    }


int Hash_board_type::hashvalue()
    {
        unsigned long   h = mover;
        unsigned long   g;
        int             i;
        unsigned short *ptr = &board[0];

        for(i = 0; i < 16; i++) {
            h = (h << 3) + *ptr++;
            if(g = h & glob__hash_mask_off) {
                h = h ^ (g >> HASH_BITS);
            }
        }
        return(h & glob__hash_mask_on);
    }


void Hash_board_type::set_like_posit(Position_type *posit)
    {
        int              i;
        int              j;


        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + (next_u_file * 8); j += next_u_file) {
                add_piece(j, posit->board[j]);
            }
        }
        mover = posit->mover;
    }


void Hash_board_type::print_hash_board()
    {
        int  i;

        cout << "\nPrinting state of hash board.\n";
        for(i = 0; i < 8; i++) {
           cout << hex << board[i * 2] << " " << hex << board[i * 2 + 1] << dec << endl;
//            cout << form("%x %x\n", board[i * 2], board [i*2+1]);
        }
        cout << "\n";
    }



void Hash_board_type::check_consistency(Position_type *posit)
    {
        int              i;
        int              j;
        int              problem = False;
        Hash_board_type  temp_board;


        // Create temporary hash board corresponding with the posit board.

        for(i = white_left_rook_home; i <= white_right_rook_home; i += next_r_rank) {
            for(j = i; j < i + (next_u_file * 8); j += next_u_file) {
                temp_board.add_piece(j, posit->board[j]);
            }
        }
        temp_board.mover = posit->mover;

        // Check that the temp board is the same as this one.

        for(i = 0; i < 16; i++) if(temp_board.board[i] != board[i]) problem = True;
        if(temp_board.mover != mover) problem = True;
        if(problem) {
            cout << "There is an error in the correspondence between the main and hash boards.\n";
            print_board(posit);
            print_hash_board();
        }
    }

#endif

