// This file contains IO related routines.

void write_thinking_message();

void newline();

void print_move_text_prefix();

void write_out_computer_move(bd_index from_loc, bd_index to_loc, int taken, int value);

void write_out_time(Position_type  *posit);

void write_message_to_request_move();

void write_bad_input();

void write_move_input(char *input);

void get_file_to_put_move_list_in();

void return_to_continue();

void write_check();

void write_checkmate();

void write_draw_50();

void write_draw_repetition();

void write_stalemate();

void write_reading_library();

void note_progress();

