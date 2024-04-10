#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum tile {
  L, // Land
  B, // Bridge
  W, // Water
  D, // Dinosaur
  V, // Volcano
};

void print_tile(enum tile t) {
  switch (t) {
  case L:
    printf("L");
    break;
  case B:
    printf("B");
    break;
  case W:
    printf("W");
    break;
  case D:
    printf("D");
    break;
  case V:
    printf("V");
    break;
  }
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
    printf("unreahcable");
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

static int prev_rand;

void init_random() { prev_rand = (int)time(NULL); }

enum direction random_direction() {
  srand((unsigned int)prev_rand);
  prev_rand = rand();
  return num_to_direction(prev_rand);
}

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
  bool is_alive;
  int length;
};

struct walk_result perform_walk(struct coordinate start, enum tile map[]) {
  struct coordinate coord = start;
  if (!is_safe(tile_on(coord, map))) {
    return (struct walk_result){
        .is_alive = false,
        .length = 0,
    };
  }
  bool is_alive = true;
  int length;
  for (length = 0; length < 10; ++length) {
    enum direction dir = random_direction();
    move(&coord, dir);
    enum tile current_tile = tile_on(coord, map);
    if (!is_safe(current_tile)) {
      is_alive = false;
      break;
    }
  }
  return (struct walk_result){
      .is_alive = is_alive,
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
  double escape_count_ = (double)stats->escape_count;
  return escape_count_ / 1000;
}

struct stats perform_1000_walks(struct coordinate coord, enum tile map[]) {
  struct stats stats = {0};
  for (int i = 0; i < 1000; ++i) {
    struct walk_result walk_result = perform_walk(coord, map);
    stats.lengths[i] = walk_result.length;
    if (walk_result.is_alive) {
      stats.escape_count += 1;
    }
  }
  return stats;
}

int main() {
  init_random();
  static enum tile map[] = {
      // 1  2  3  4  5  6  7  8
      B, W, W, B, B, W, B, W, W, // 0
      B, L, L, V, L, L, L, L, B, // 1
      W, L, L, L, L, D, L, L, B, // 2
      B, L, L, D, L, L, L, L, W, // 3
      B, L, D, L, L, L, L, L, W, // 4
      W, L, L, L, L, L, V, L, B, // 5
      W, V, L, L, L, L, L, L, W, // 6
      W, L, L, L, D, L, L, L, W, // 7
      B, B, W, B, W, B, B, W, B, // 8
  };
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
