
// System headers
#include <stdio.h>

// define bitboard data type
// used to represent board
#define U64 unsigned long long // 64 bits long integer

// enum board squares
enum {
  a8, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1,
};

// sides to move (colors)
enum {White, Black};

/*
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8,"
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7,"
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6,"
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5,"
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4,"
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3,"
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2,"
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1,"
*/

// set/get/pop bit macros
//
// macro to see if bitboard has piece in a certain square
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
// set a bit in bitboard
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
// pop a bit from bitboard
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)


// print bitboard function
void print_bitboard(U64 bitboard) {
  int rank, file, square;
  
  printf("\n");
  // loop over board ranks
  for(rank = 0; rank < 8; rank++) {

    // loop for files
    for(file = 0; file < 8; file++) {
      // convert file & rank in square index
      square = rank * 8 + file;

      // print rank number
      if(!file)
        printf("  %d ", 8 - rank);

      // print bit state (1 -> piece, 0 -> no piece)
      printf(" %c", get_bit(bitboard, square) ? '1' : '.');
    }
    printf("\n");
  }

  // print board files
  printf("\n     a b c d e f g h\n\n");

  // print bitboard as unsigned decimal number
  printf("     Bitboard: %llud\n\n", bitboard);
}

/*
* ATTACKS
*/
// not A file constant -> checks that the pawn is not on the A file
// bitboard initializet to 1, except for the A file
const U64 not_a_file = 18374403900871474942ULL;

// not H file constant -> checks that the pawn is not on the H file
// bitboard initializet to 1, except for the H file
const U64 not_h_file = 9187201950435737471ULL;

// not HG files constant -> used for horses
const U64 not_hg_files = 4557430888798830399ULL;

// not AB files constant -> used for horses
const U64 not_ab_files = 18229723555195321596ULL;

// pawn attack table: [side (b/w)][square]
U64 pawn_atttacks[2][64];
// knight attack table: [square] (b/w have same table)
U64 knight_attacks[64];
// king attack table: [square] (b/w have same table)
U64 king_attacks[64];

// generate pawn attacks
U64 mask_pawn_attacks(int side, int square) {
  // piece bitboard
  U64 bitboard = 0ULL;
  //resulting attacks bitboard
  U64 attacks = 0ULL;

  // set piece on board
  set_bit(bitboard, square);

  if(!side) {
    // wite pawns
    // checks that a capture is possible and sets it
    attacks |= ((bitboard >> 7) & not_a_file);
    attacks |= ((bitboard >> 9) & not_h_file);
  }
  else {
    // black pawns
    attacks |= ((bitboard << 7) & not_h_file);
    attacks |= ((bitboard << 9) & not_a_file);
  }

  // return attack map
  return attacks;
}

// generate knight attacks
U64 mask_knight_attacks(int square) {
  // piece bitboard
  U64 bitboard = 0ULL;
  //resulting attacks bitboard
  U64 attacks = 0ULL;

  // set piece on board
  set_bit(bitboard, square);

  // not using ifs beacause at low level are more expensive
  attacks |= ((bitboard >> 17) & not_h_file);
  attacks |= ((bitboard >> 15) & not_a_file);
  attacks |= ((bitboard >> 10) & not_hg_files);
  attacks |= ((bitboard >> 6) & not_ab_files);

  attacks |= ((bitboard << 17) & not_a_file);
  attacks |= ((bitboard << 15) & not_h_file);
  attacks |= ((bitboard << 10) & not_ab_files);
  attacks |= ((bitboard << 6) & not_hg_files);

  return attacks;
}

// generate king attacks
U64 mask_king_attacks(int square) {
  // piece bitboard
  U64 bitboard = 0ULL;
  //resulting attacks bitboard
  U64 attacks = 0ULL;

  // set piece on board
  set_bit(bitboard, square);

  attacks |= ((bitboard >> 7)) & not_a_file;
  attacks |= ((bitboard >> 8));
  attacks |= ((bitboard >> 9)) & not_h_file;

  attacks |= ((bitboard >> 1)) & not_h_file;
  attacks |= ((bitboard << 1)) & not_a_file;

  attacks |= ((bitboard << 7)) & not_h_file;
  attacks |= ((bitboard << 8));
  attacks |= ((bitboard << 9)) & not_a_file;

  return attacks;
}

// mask bishop attacks
U64 mask_bishop_attacks(int square) {
  // result attacks bitboard
  U64 attacks = 0ULL;

  // init ranks & files
  int r, f;
  // init target ranks & files
  int target_r = square / 8;
  int target_f = square % 8;

  // mask relevant bishop occupancy bits, not initialize edges
  for(r = target_r + 1, f = target_f + 1; r < 7 && f < 7; r++, f++)
    attacks |= (1ULL << (r * 8 + f));

  for(r = target_r + 1, f = target_f - 1; r < 7 && f > 0; r++, f--)
    attacks |= (1ULL << (r * 8 + f));

  for(r = target_r - 1, f = target_f + 1; r > 0 && f < 7; r--, f++)
    attacks |= (1ULL << (r * 8 + f));
    
  for(r = target_r - 1, f = target_f - 1; r > 0 && f > 0; r--, f--)
    attacks |= (1ULL << (r * 8 + f));

  return attacks;
}

// mask rook attacks
U64 mask_rook_attacks(int square) {
  // result attacks bitboard
  U64 attacks = 0ULL;

  // init ranks & files
  int r, f;
  // init target ranks & files
  int target_r = square / 8;
  int target_f = square % 8;

  // mask relevant rook occupancy bits, not initialize edges
  for(r = target_r + 1, f = target_f; r < 7; r++)
    attacks |= (1ULL << (r * 8 + f));

  for(r = target_r - 1, f = target_f; r > 0; r--)
    attacks |= (1ULL << (r * 8 + f));

  for(r = target_r, f = target_f + 1; f < 7; f++)
    attacks |= (1ULL << (r * 8 + f));
    
  for(r = target_r, f = target_f - 1; f > 0; f--)
    attacks |= (1ULL << (r * 8 + f));

  return attacks;
}

// init leaper pieces attacks
void init_leapers_attacks() {
  int square;

  // loop over board squares
  for(square = 0; square < 64; square++) {
    //init pawn attacks
    pawn_atttacks[White][square] = mask_pawn_attacks(White, square);
    pawn_atttacks[Black][square] = mask_pawn_attacks(Black, square);

    // init knight attacks
    knight_attacks[square] = mask_knight_attacks(square);

    // init king attacks
    king_attacks[square] = mask_king_attacks(square);
  }
}

int main() {
  U64 bitboard = 0ULL; // define bitboard
  
  // init leaper pieces attacks
  init_leapers_attacks();

  for(int square = 0; square < 64; square++) { 
    //print_bitboard(pawn_atttacks[White][square]);
    print_bitboard(mask_rook_attacks(square));
  }


  return 0;
}
