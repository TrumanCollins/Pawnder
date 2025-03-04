
#include <glob.h>
#include <statval.h>

int static_piece_value(
        square piece  // The piece to give the value of.
    )
    // Returns the value of the piece.
    {
        // Convert black pieces to white to make case easier.

        if(piece > Wking) piece = piece >> 6;
        switch(piece) {
            case Wpawn   :
                return(100);
            case Wknight :
                return(300);
            case Wbishop :
                return(325);
            case Wrook   :
                return(500);
            case Wqueen  :
                return(900);
            case Wking   :
                return(2000);
        }
        return(0);
    }
