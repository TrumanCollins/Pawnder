
#include <glob.h>
#include <hashbrd.h>
#include <position.h>
#include <statval.h>
#include <utility.h>
#include <string.h>

static void reverse(char *s)
    // Reverse the string.
    {
        int c, i, j;
    
        for(i = 0, j = strlen(s)-1; i < j; i++, j--) {
            c = s[i];
            s[i] = s[j];
            s[j] = c;
        }
    }


void utility_itoa(int n, char *s)
    // Convert the integer n to a string starting at s.
    {
        int i, sign;
    
        if((sign = n) < 0) n = -n;
        i = 0;
        do {
            s[i++] = n % 10 + '0';
        } while((n /= 10) > 0);
        if(sign < 0) s[i++] = '-';
        s[i] = '\0';
        reverse(s);
    }


int file_exists(char *file_name)
    // Returns true if the file exists, false otherwise.
    {
        filebuf f1;
    
        if(f1.open(file_name, ios::in)) {
            f1.close();
            return(True);
        } else return(False);
    }
        

void clear_input_stream()
    // This routine will read the remaining characters from cin including
    // the next '\n'.  It will stop after that.
    {
        char ch;

        do cin.get(ch); while(ch != '\n');
    }

Gen_move_type *sort_moves(
        Gen_move_type *move_top   // Points to the first move in list.
    )
    // This procedure will sort the list according to the variable Value.
    // Higher values will come first.  An insertion sort is used.  This
    // Could clearly be sped up.
    {
        Gen_move_type *end_move; // Points to end of list we are building.
        Gen_move_type *temp;     // temporary pointer variable.
        Gen_move_type *temp_org; // temporary pointer variable holding Movetop.
        Gen_move_type *temp1;    // temporary pointer variable.


        if(move_top) {
            temp_org = move_top->next;
            move_top->next = NULL;
            end_move = move_top;
            while(temp_org) {
                if(end_move->val >= temp_org->val) {
                   
                    // Put this at the end of the list.

                    end_move->next = temp_org;
                    end_move = temp_org;
                    temp_org = temp_org->next;
                    end_move->next = NULL;
                } else if(move_top->val < temp_org->val) {

                    // Insert this in front of move_top.

                    temp = move_top;
                    move_top = temp_org;
                    temp_org = temp_org->next;
                    move_top->next = temp;
                } else {

                    // Insert at the proper point in the list.

                    temp = move_top;
                    while(temp->next) {
                        if(temp->next->val < temp_org->val) break;
                        else temp = temp->next;
                    }
                    temp1 = temp_org;
                    temp_org = temp1->next;
                    temp1->next = temp->next;
                    if(!temp->next) end_move = temp1;
                    temp->next = temp1;
                }
            }
        }
        return(move_top);
    }


Gen_move_type *sort_moves_reverse(
        Gen_move_type *move_top   // Points to the first move in list.
    )
    // This procedure will sort the list according to the variable Value.
    // Lower values will come first.  An insertion sort is used.  This
    // Could clearly be sped up.
    {
        Gen_move_type *end_move; // Points to end of list we are building.
        Gen_move_type *temp;     // temporary pointer variable.
        Gen_move_type *temp_org; // temporary pointer variable holding Movetop.
        Gen_move_type *temp1;    // temporary pointer variable.


        if(move_top) {
            temp_org = move_top->next;
            move_top->next = NULL;
            end_move = move_top;
            while(temp_org) {
                if(end_move->val <= temp_org->val) {
                   
                    // Put this at the end of the list.

                    end_move->next = temp_org;
                    end_move = temp_org;
                    temp_org = temp_org->next;
                    end_move->next = NULL;
                } else if(move_top->val > temp_org->val) {

                    // Insert this in front of move_top.

                    temp = move_top;
                    move_top = temp_org;
                    temp_org = temp_org->next;
                    move_top->next = temp;
                } else {

                    // Insert at the proper point in the list.

                    temp = move_top;
                    while(temp->next) {
                        if(temp->next->val > temp_org->val) break;
                        else temp = temp->next;
                    }
                    temp1 = temp_org;
                    temp_org = temp1->next;
                    temp1->next = temp->next;
                    if(!temp->next) end_move = temp1;
                    temp->next = temp1;
                }
            }
        }
        return(move_top);
    }


void free_list(
        Gen_move_type *point  // pointer to top of list to dispose of.
    )
    // This function will dispose of all of the
    // nodes from point down in the list.
    {
        Gen_move_type *temp;

        while(point) {
            temp  = point;
            point = point->next;
            delete_move_type(temp);
        }
    }

void print_board(
        Position_type *posit // Holds current position of board.
    )
    // This procedure will print the current position on the board in the
    // far upper left part of the screen.
    {
        char name;
        int  i;
        int  j;


        if(glob__running_under_winboard) return;
        for(i = 8; i >= 1; i--) {
            cout << "\n    " << i << "| ";
            for(j = 1; j <= 8; j++) {
                switch(posit->board[single_index_from_double(j,i)]) {
                    case Empty   : name = '-'; break;
                    case Wpawn   : name = 'P'; break;
                    case Wknight : name = 'N'; break;
                    case Wbishop : name = 'B'; break;
                    case Wrook   : name = 'R'; break;
                    case Wqueen  : name = 'Q'; break;
                    case Wking   : name = 'K'; break;
                    case Out     : name = 'O'; break;
                    case Bpawn   : name = 'p'; break;
                    case Bknight : name = 'n'; break;
                    case Bbishop : name = 'b'; break;
                    case Brook   : name = 'r'; break;
                    case Bqueen  : name = 'q'; break;
                    case Bking   : name = 'k'; break;
                    default      : cout << "Bad thing on the board.\n";
                }
                cout.put(name); cout.put(' ');
            }
        }
        cout << "\n       ---------------\n";
        cout << "       A B C D E F G H\n";
        cout << "\n";
    }


char file_to_char(int in_file)
    // Returns a character corresponding to this number file with
    // 1 being 'A' and 8 being 'H'.
    {
        return('A' - 1 + in_file);
    }


void print_move_list(
        Position_type *posit,        // Holds current position of the board.
        Gen_move_type *move_top,     // Pointer to top of moves to print.
        int            prn_board     // Board will be printed if true.
    )
    // This is a debugging procedure which will print out all of the moves in
    // the list of moves pointed to by MoveTo.
    {
        int            file;
        int            i;   
        int            rank;
        Gen_move_type *temp;


        if(prn_board) print_board(posit);
        cout << "Moves For ";
        if(posit->mover == White) cout << "White:\n";
        else cout << "Black:\n";

        temp = move_top;
        if(!temp) cout << "No Moves.\n";
        for(i = 1; temp != NULL; i++, temp = temp->next) {
            file = file_number(temp->from_loc);
            rank = rank_number(temp->from_loc);
            cout.put(file_to_char(file));
            cout << rank << "-";
            file = file_number(temp->to_loc);
            rank = rank_number(temp->to_loc);
            cout.put(file_to_char(file));
            cout << rank << " " << temp->val << "   ";
            if(i == 6) {
                cout.put('\n');
                i = 0;
            }
        }
        cout.put('\n');
        cout.flush();
    }


void print_continuation(
        Gen_move_type  *forseen_line  // The forseen line of play.
    )
    // This will print out the list of moves as the forseen line of play.
    {
        int            file;
        int            rank;
        Gen_move_type *temp;


        cout << "\nForseen line: ";
        temp = forseen_line;
        while(temp) {
            file = file_number(temp->from_loc);
            rank = rank_number(temp->from_loc);
            cout.put(file_to_char(file));
            cout << rank << "-";
            file = file_number(temp->to_loc);
            rank = rank_number(temp->to_loc);
            cout.put(file_to_char(file));
            cout << rank << "  ";
            temp = temp->next;
        }
        cout.put('\n');
        cout.flush();
    }


