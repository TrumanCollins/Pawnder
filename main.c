
#include <string.h>
#include <iostream>
#include <glob.h>
#include <initial.h>
#include <dispmenu.h>


void print_usage()
{
   std::cout << "Usage: pawnder [-xboard, -winboard] [-moves_generated] [-moves_and_values]\n";
   std::cout << "         [-evaluator_call_count] [-to_forsyth_debug] [-check_data_after_move]\n";
   std::cout << "         [-expected_line_of_play] [-print_detailed_timings]\n";
}

bool process_command_line_arguments(
   int                          argc,
   char                        *argv[],
   bool                        &exit_now
)
{
   exit_now = false;
   
   // Go through the command line arguments.  Note that curr_arg may
   // also be incremented inside the loop.

   for(int curr_arg = 1; curr_arg < argc; ++curr_arg) {

      if(strcmp(argv[curr_arg], "-moves_generated") == 0) {
         glob__print_moves_generated = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-moves_and_values") == 0) {
         glob__print_moves_and_values = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-evaluator_call_count") == 0) {
         glob__print_evaluator_call_count = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-to_forsyth_debug") == 0) {
         glob__print_forsyth_debug = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-check_data_after_move") == 0) {
         glob__check_data_after_move = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-expected_line_of_play") == 0) {
         glob__print_expected_line_of_play = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-print_detailed_timings") == 0) {
         glob__print_detailed_timings = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-xboard") == 0 || strcmp(argv[curr_arg], "-winboard") == 0) {
         glob__running_under_winboard = true;
         continue;
      }

      if(strcmp(argv[curr_arg], "-help") == 0) {
         print_usage();
         exit_now = true;
         break;
      }

      std::cout << "Invalid command line argument: " << argv[curr_arg]
                << std::endl;
      exit_now = true;
      return false;
   }

   // All of the command line arguments were ok.

   return true;
}


int main(
   int    argc,
   char  *argv[]
)
{
   bool exit_now;
   bool valid_arguments = process_command_line_arguments(argc, argv, exit_now);
   if(exit_now) {
      if(valid_arguments) {
         return 0;
      } else {
         return 1;
      }
   }
   initialize_variables();
   display_menu();
   return 0;
}
