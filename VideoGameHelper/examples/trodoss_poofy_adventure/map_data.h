#define MAP_PATTERN_COUNT 50
#define MAP_ROOM_COUNT 24
#define MAP_WIDTH 8
#define MAP_HEIGHT 3

//map pattern data
PROGMEM prog_uchar map_pattern_data[] = {
0, 0, 0, 0, 0, 0, 2, 3, 3, 3,
0, 0, 0, 0, 0, 2, 3, 3, 3, 3,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 2, 3, 3, 3, 3, 3,
0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 2, 3,
0, 0, 0, 0, 0, 0, 0, 2, 3, 3,
0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
0, 0, 0, 0, 1, 0, 0, 0, 1, 0,
0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
0, 0, 0, 0, 0, 0, 3, 3, 3, 3,
0, 0, 0, 0, 0, 0, 0, 3, 3, 3,
3, 0, 0, 0, 0, 0, 0, 3, 3, 3,
3, 0, 0, 0, 0, 0, 0, 0, 3, 3,
3, 3, 0, 0, 0, 0, 0, 0, 3, 3,
3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
3, 3, 3, 0, 0, 0, 0, 0, 3, 3,
3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
3, 3, 3, 3, 0, 0, 0, 0, 3, 3,
3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
3, 3, 3, 0, 0, 0, 0, 3, 3, 3,
3, 3, 0, 0, 0, 0, 0, 3, 3, 3,
0, 0, 0, 0, 0, 0, 0, 0, 3, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
3, 3, 3, 3, 0, 0, 0, 0, 0, 3,
3, 3, 3, 0, 0, 0, 0, 0, 0, 3,
3, 3, 3, 3, 0, 0, 0, 3, 3, 3,
3, 3, 0, 0, 0, 0, 3, 0, 0, 0,
3, 0, 0, 0, 0, 0, 3, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
0, 0, 0, 0, 0, 3, 0, 0, 0, 0,
3, 3, 0, 0, 0, 0, 3, 3, 3, 3,
3, 0, 0, 0, 0, 3, 3, 3, 3, 3,
3, 0, 0, 0, 0, 0, 3, 3, 3, 3,
3, 0, 0, 0, 0, 0, 0, 0, 0, 3,
3, 3, 0, 0, 0, 0, 0, 0, 0, 3,
};

//map room data
PROGMEM prog_uchar map_room_data[] = {
0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 1, 2, 2, 1, 1,
1, 1, 0, 0, 0, 0, 0, 1, 1, 4, 4, 5, 5, 2, 3, 3,
3, 3, 2, 2, 0, 0, 5, 5, 2, 6, 6, 5, 5, 2, 4, 4,
4, 4, 6, 6, 2, 2, 6, 6, 2, 5, 5, 2, 4, 4, 0, 0,
0, 0, 2, 2, 7, 8, 0, 1, 1, 1, 1, 0, 8, 8, 0, 1,
1, 1, 0, 8, 7, 2, 2, 7, 8, 8, 8, 8, 7, 7, 7, 8,
8, 8, 7, 2, 2, 6, 6, 2, 5, 5, 2, 6, 6, 5, 5, 2,
8, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 1, 1, 1,
1, 1, 1, 2, 9, 9, 10, 10, 2, 9, 9, 2, 10, 2, 10, 10,
10, 10, 9, 11, 12, 9, 9, 10, 10, 2, 2, 10, 10, 2, 2, 10,
10, 10, 13, 13, 14, 10, 10, 4, 4, 10, 10, 11, 10, 10, 4, 4,
15, 5, 10, 16, 17, 9, 2, 17, 4, 18, 6, 5, 5, 5, 5, 5,
5, 5, 2, 0, 0, 2, 2, 1, 1, 2, 2, 0, 0, 2, 11, 11,
12, 11, 12, 19, 20, 19, 20, 19, 6, 10, 11, 11, 13, 13, 2, 13,
2, 21, 13, 9, 9, 2, 10, 10, 2, 2, 10, 22, 19, 19, 23, 23,
23, 23, 23, 19, 19, 22, 10, 2, 2, 1, 1, 1, 1, 3, 3, 3,
24, 24, 25, 26, 26, 27, 28, 28, 29, 29, 28, 30, 31, 31, 30, 32,
30, 28, 33, 31, 28, 30, 34, 35, 26, 26, 28, 30, 33, 29, 27, 36,
36, 2, 37, 34, 35, 37, 29, 28, 30, 38, 39, 28, 26, 37, 37, 26,
35, 40, 31, 29, 27, 27, 29, 31, 35, 26, 26, 37, 29, 41, 42, 37,
43, 43, 44, 44, 44, 2, 43, 43, 2, 44, 44, 44, 44, 2, 36, 26,
45, 45, 45, 46, 46, 46, 37, 29, 30, 27, 37, 37, 35, 35, 31, 29,
26, 26, 37, 29, 34, 26, 47, 45, 45, 45, 26, 27, 29, 31, 35, 35,
37, 37, 48, 39, 49, 27, 26, 26, 45, 45, 45, 34, 35, 47, 46, 46};
