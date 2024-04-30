#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum tile {
  L = 'L', // Land
  B = 'B', // Bridge
  W = 'W', // Water
  D = 'D', // Dinosaur
  V = 'V', // Volcano
};

char tile_to_char(enum tile t) { return (char)t; }

enum tile tile_from_char(char c) { return (enum tile)c; }

void print_tile(enum tile t) { printf("%c", tile_to_char(t)); }

void read_map(enum tile *map, FILE *in) {
  for (int y = 0; y < 9; ++y) {
    for (int x = 0; x < 9; ++x) {
      char c;
      do {
        c = (char)fgetc(in);
      } while (c == '\n' || c == ' ');
      map[y*9+x] = tile_from_char(c);
    }
  }
  // for (int y = 0; y < 9; ++y) {
  //   char cs[9];
  //   fscanf(in, "%c %c %c %c %c %c %c %c %c\n", &cs[0], &cs[1], &cs[2],
  //   &cs[3],
  //          &cs[4], &cs[5], &cs[6], &cs[7], &cs[8]);
  //   for (int x = 0; x < 9; ++x) {
  //     map[y * 9 + x] = tile_from_char(cs[x]);
  //   }
  // }
}

bool is_safe(enum tile tile) {
  switch (tile) {
  case L:
    return true;
  case B:
    return true;
  case W:
    return false;
  case D:
    return false;
  case V:
    return false;
  }
}

enum direction {
  Up,
  Down,
  Left,
  Right,
  UpLeft,
  UpRight,
  DownLeft,
  DownRight,
};

enum direction num_to_direction(int num) {
  switch (num % 8) {
  case 0:
    return Up;
  case 1:
    return Down;
  case 2:
    return Left;
  case 3:
    return Right;
  case 4:
    return UpLeft;
  case 5:
    return UpRight;
  case 6:
    return DownLeft;
  case 7:
    return DownRight;
  default:
    printf("unreachable\n");
    exit(1);
  }
}

void print_direction(enum direction d) {
  switch (d) {
  case Up:
    printf("Up");
    break;
  case Down:
    printf("Down");
    break;
  case Left:
    printf("Left");
    break;
  case Right:
    printf("Right");
    break;
  case UpLeft:
    printf("UpLeft");
    break;
  case UpRight:
    printf("UpRight");
    break;
  case DownLeft:
    printf("DownLeft");
    break;
  case DownRight:
    printf("DownRight");
    break;
  }
}

void init_random() { srand(123456); }

enum direction random_direction() { return num_to_direction(rand()); }

struct coordinate {
  int x; // horizontal (left to right)
  int y; // vertical (up to down)
};

void print_coordinate(struct coordinate c) { printf("(%d, %d)", c.x, c.y); }

#define MAP_SIDE_LENGTH 9

enum tile tile_on(struct coordinate coord, enum tile map[]) {
  assert(coord.x >= 0 && coord.x < MAP_SIDE_LENGTH);
  assert(coord.x >= 0 && coord.x < MAP_SIDE_LENGTH);
  int i = coord.y * MAP_SIDE_LENGTH + coord.x;
  return map[i];
}

/// Increment a component (x or y value) of a coordinate.
/// With consideration of min and max value.
void inc_x_or_y(int *value) {
  assert(*value >= 0 && *value < MAP_SIDE_LENGTH);
  if (*value == MAP_SIDE_LENGTH - 1) {
    return;
  }
  ++(*value);
}

/// Decrement a component (x or y value) of a coordinate.
/// With consideration of min and max value.
void dec_x_or_y(int *value) {
  assert(*value >= 0 && *value < MAP_SIDE_LENGTH);
  if (*value == 0) {
    return;
  }
  --(*value);
}

void move(struct coordinate *coord, enum direction dir) {
  switch (dir) {
  case Up:
    dec_x_or_y(&coord->y);
    break;
  case Down:
    inc_x_or_y(&coord->y);
    break;
  case Left:
    dec_x_or_y(&coord->x);
    break;
  case Right:
    inc_x_or_y(&coord->x);
    break;
  case UpLeft:
    dec_x_or_y(&coord->y);
    dec_x_or_y(&coord->x);
    break;
  case UpRight:
    dec_x_or_y(&coord->y);
    inc_x_or_y(&coord->x);
    break;
  case DownLeft:
    inc_x_or_y(&coord->y);
    dec_x_or_y(&coord->x);
    break;
  case DownRight:
    inc_x_or_y(&coord->y);
    inc_x_or_y(&coord->x);
    break;
  }
}

struct walk_result {
  bool success;
  int length;
};

struct walk_result perform_walk(struct coordinate start, enum tile map[]) {
  struct coordinate coord = start;
  enum tile start_tile = tile_on(coord, map);
  if (!is_safe(start_tile)) {
    return (struct walk_result){
        .success = false,
        .length = 0,
    };
  } else if (start_tile == B) {
    return (struct walk_result){
        .success = true,
        .length = 0,
    };
  }
  bool success = false;
  int length;
  for (length = 0; length < 10; ++length) {
    enum direction dir = random_direction();
    move(&coord, dir);
    enum tile current_tile = tile_on(coord, map);
    if (!is_safe(current_tile)) {
      break;
    } else if (current_tile == B) {
      success = true;
      break;
    }
  }
  return (struct walk_result){
      .success = success,
      .length = length,
  };
}

/// The statistics on one tile.
struct stats {
  int escape_count;
  int lengths[1000];
};

double mean_length(struct stats *stats) {
  double sum = 0;
  for (int i = 0; i < 1000; i++) {
    sum += (double)stats->lengths[i];
  }
  return sum / 1000.0;
}

double sd_lengths(struct stats *stats, double mean) {
  double sum = 0;
  for (int i = 0; i < 1000; i++) {
    double diff = (double)stats->lengths[i] - mean;
    sum += diff * diff;
  }
  return sqrt(sum / 1000.0);
}

double escape_probability(struct stats *stats) {
  double escape_count = (double)stats->escape_count;
  return escape_count / 1000;
}

struct stats perform_1000_walks(struct coordinate coord, enum tile map[]) {
  struct stats stats = {0};
  for (int i = 0; i < 1000; ++i) {
    struct walk_result walk_result = perform_walk(coord, map);
    stats.lengths[i] = walk_result.length;
    if (walk_result.success) {
      stats.escape_count += 1;
    }
  }
  return stats;
}

int main() {
  init_random();
  static enum tile map[9 * 9];
  FILE *island_map_txt = fopen("island_map.txt", "r");
  assert(island_map_txt != NULL);
  read_map(map, island_map_txt);
  fclose(island_map_txt);
  static double map_probabilities[MAP_SIDE_LENGTH * MAP_SIDE_LENGTH];
  static double map_mean[MAP_SIDE_LENGTH * MAP_SIDE_LENGTH];
  static double map_sd[MAP_SIDE_LENGTH * MAP_SIDE_LENGTH];
  for (int y = 0; y < MAP_SIDE_LENGTH; ++y) {
    for (int x = 0; x < MAP_SIDE_LENGTH; ++x) {
      struct stats stats = perform_1000_walks((struct coordinate){x, y}, map);
      map_probabilities[y * MAP_SIDE_LENGTH + x] = escape_probability(&stats);
      double mean = mean_length(&stats);
      map_mean[y * MAP_SIDE_LENGTH + x] = mean;
      map_sd[y * MAP_SIDE_LENGTH + x] = sd_lengths(&stats, mean);
    }
  }
  printf("Map:\n");
  for (int y = 0; y < 9; ++y) {
    for (int x = 0; x < 9; ++x) {
      print_tile(map[y * MAP_SIDE_LENGTH + x]);
      printf(" ");
    }
    printf("\n");
  }
  printf("\n");
  printf("Probability of escape:\n");
  for (int y = 0; y < 9; ++y) {
    for (int x = 0; x < 9; ++x) {
      // * 100 because we want to print in percentage.
      double probability = map_probabilities[y * MAP_SIDE_LENGTH + x] * 100;
      if (probability < 10.0)
        printf("  ");
      else if (probability < 100.0)
        printf(" ");
      printf("%.02lf ", probability);
    }
    printf("\n");
  }
  printf("\n");
  printf("Mean path length:\n");
  for (int y = 0; y < 9; ++y) {
    for (int x = 0; x < 9; ++x) {
      printf("%.02lf ", map_mean[y * MAP_SIDE_LENGTH + x]);
    }
    printf("\n");
  }
  printf("\n");
  printf("Standard deviation of path length:\n");
  for (int y = 0; y < 9; ++y) {
    for (int x = 0; x < 9; ++x) {
      printf("%.02lf ", map_sd[y * MAP_SIDE_LENGTH + x]);
    }
    printf("\n");
  }
  return 0;
}
