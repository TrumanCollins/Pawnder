
#include <glob.h>
#include <piececod.h>

char piece_code(
        square piece  // The piece to return the character for.
    )
    // Returns the character corresponding to the piece sent in.
    {
        switch(piece) {
            case Wpawn   : return('P');
            case Wknight : return('N');
            case Wbishop : return('B');
            case Wrook   : return('R');
            case Wqueen  : return('Q');
            case Wking   : return('K');
            case Bpawn   : return('p');
            case Bknight : return('n');
            case Bbishop : return('b');
            case Brook   : return('r');
            case Bqueen  : return('q');
            case Bking   : return('k');
            default      : cerr << "Error in piece_code.\n"; return('#');
        }
    }
