#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <map>
#include <vector>

const int BLACK = 1;
const int WHITE = 2;
const int MAX_DEPTH = 1e5;
const int INF = 1e8;
// const int LIST_LEN = 1e8; //4e6;
const int COMB_LEN = 2500;

/*
   A B C D E
  ----------
5| u v w x y 
4| p q r s t 
3| k l m n o 
2| f g h i j 
1| a b c d e 

    A  B  C  D  E
  ---------------
5| 20 21 22 23 24
4| 15 16 17 18 19
3| 10 11 12 13 14
2|  5  6  7  8  9
1|  0  1  2  3  4
*/

typedef struct state {
    char pos[6];
    // {黒，黒，黒，白，白，白} 場所のリスト
    // 昇順を仮定
} state;

void printb(unsigned int v) {
    unsigned int mask = (int)1 << (sizeof(v) * CHAR_BIT - 1);
    do putchar(mask & v ? '1' : '0');
    while (mask >>= 1);
    printf("\n");
}

int switch_player(const int x) {
    int y = (x & ((1 << 15) - 1)) << 15;
    y |= (x >> 15);
    return y;
}

int state_to_int(state s) {
    int res = 0;
    res |= (s.pos[0] - 'a') << 25;
    res |= (s.pos[1] - 'a') << 20;
    res |= (s.pos[2] - 'a') << 15;
    res |= (s.pos[3] - 'a') << 10;
    res |= (s.pos[4] - 'a') <<  5;
    res |= (s.pos[5] - 'a') <<  0;
    return res;
}

state int_to_state(int x) {
    state res;
    res.pos[0] = ((x >> 25) & 0b11111) + 'a';
    res.pos[1] = ((x >> 20) & 0b11111) + 'a';
    res.pos[2] = ((x >> 15) & 0b11111) + 'a';
    res.pos[3] = ((x >> 10) & 0b11111) + 'a';
    res.pos[4] = ((x >>  5) & 0b11111) + 'a';
    res.pos[5] = ((x >>  0) & 0b11111) + 'a';
    return res;
}

int is_won(int x, int color) {
    if (color == WHITE) return is_won(switch_player(x), BLACK);

    int a = (x >> 25) & 0b11111;
    int b = (x >> 20) & 0b11111;
    int c = (x >> 15) & 0b11111;

    if (a / 5 == b / 5 && b / 5 == c / 5) {
        if (a + 1 == b && b + 1 == c) {
            return 1;
        }
    }

    if (a + 5 == b && b + 5 == c) return 1;

    if (a / 5 + 1 == b / 5 && b / 5 + 1 == c / 5) {
        if (a + 6 == b && b + 6 == c) return 1;
        if (a + 4 == b && b + 4 == c) return 1;
    }

    return 0;
}

void recursive_comb(int** res, int* positions, int* indexes, int s, int rest, int* iter) {
    if (rest == 0) {
        for (int i = 0; i < 3; i++) res[*iter][i] = indexes[i];
        *iter += 1;
        return;
    }
    if (s >= 25) return;
    recursive_comb(res, positions, indexes, s + 1, rest, iter);
    if (positions[s] == -1) return;
    indexes[3 - rest] = s;
    recursive_comb(res, positions, indexes, s + 1, rest - 1, iter);
}

int** comb(int* positions) {
    // recursive_comb とあわせて，25C3または22C3を全列挙する
    // positions[i] == -1 であるようなiは数えない
    int indexes[3];
    
    int **res = (int**)malloc(sizeof(int*) * COMB_LEN);
    for (int i = 0; i < COMB_LEN; i++) {
        res[i] = (int*)malloc(sizeof(int) * 3);
    }

    for (int i = 0; i < COMB_LEN; i++) {
        for (int j = 0; j < 3; j++) res[i][j] = 0;
    }

    int iter = 0;
    recursive_comb(res, positions, indexes, 0, 3, &iter);
    return res;
}

std::vector<int> get_all_states() {
    // ありうるすべての状態を列挙
    std::vector<int> res;
    // int* res = (int*)malloc(sizeof(int) * LIST_LEN); // 4e6 > 25C3 * 22C3
    // for (int i = 0; i < LIST_LEN; i++) {
    //     res[i] = 0;
    // }
    int positions[25];
    // int iter = 0;

    for (int i = 0; i < 25; i++) positions[i] = i;
    int** comb1 = comb(positions);

    for (int i = 0; i < COMB_LEN; i++) {
        if (comb1[i][2] == 0) continue;
        int remain[25];
        for (int i = 0; i < 25; i++) remain[i] = i;
        for (int k = 0; k < 3; k++) remain[comb1[i][k]] = -1;
        int** comb2 = comb(remain);
        for (int j = 0; j < COMB_LEN; j++) {
            if (comb2[j][2] == 0) continue;
            int state = (comb1[i][0] << 25) | (comb1[i][1] << 20) | (comb1[i][2] << 15) | (comb2[j][0] << 10) | (comb2[j][1] << 5) | comb2[j][2];
            if (is_won(state, BLACK) && is_won(state, WHITE)) continue; // 両者が同時に勝つような場合は除外
            // res[iter] = state;
            // iter++;
            printf("%d\n", state);
            res.push_back(state);
        }
        free(comb2);
    }
    free(comb1);

    return res;
}

int* get_moves(const int x) {
    // given a certain state, return list of 
    // all valid states after one step by black
    int* res = (int*)malloc(sizeof(int) * 24);
    for (int i = 0; i < 24; i++) {
        res[i] = 0;
    }
    int board[5][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    board[((x >> 25) & 31) / 5][((x >> 25) & 31) % 5] = BLACK;
    board[((x >> 20) & 31) / 5][((x >> 20) & 31) % 5] = BLACK;
    board[((x >> 15) & 31) / 5][((x >> 15) & 31) % 5] = BLACK;
    board[((x >> 10) & 31) / 5][((x >> 10) & 31) % 5] = WHITE;
    board[((x >> 5) & 31) / 5][((x >> 5) & 31) % 5] = WHITE;
    board[(x & 31) / 5][(x & 31) % 5] = WHITE;
    int iter = 0;

    for (int pawn = 0; pawn < 3; pawn++) {
        int a = ((x >> (25 - 5 * pawn)) & 31) / 5;
        int b = ((x >> (25 - 5 * pawn)) & 31) % 5;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                res[iter] = -1;
                if (i == 0 && j == 0) continue;
                for (int s = 1; s < 5; s++) {
                    if (a + j * s < 0 || a + j * s >= 5 || b + i * s < 0 || b + i * s >= 5) continue;

                    if (board[a + j * s][b + i * s] == 0) {
                        // (a, b) -> (a + j * s, b + i * s)
                        int tmp = x;
                        int mask = ((1 << 30) - 1) - ((1 << (30 - 5 * pawn)) - 1) + ((1 << (25 - 5 * pawn)) - 1);
                        tmp &= mask;
                        res[iter] = tmp | (((a + j * s) * 5 + b + i * s) << (25 - 5 * pawn));
                    } else {
                        continue;
                    }
                }
                iter++;
            }
        }
    }

    return res;
}

int main() {
    std::map<int, int> scores;
    // std::map<int, int> next_move;
    // int* scores = (int*)malloc(sizeof(int) * (1 << 30));
    // int* next_move = (int*)malloc(sizeof(int) * (1 << 30));
    // for (int i = 0; i < (1 << 30); i++) {
    //     scores[i] = 0;
    //     next_move[i] = 0;
    // }
    // printf("done\n");

    std::vector<int> states = get_all_states();
    exit(0);
    std::vector<int> neutrals;
    // int* states = get_all_states();
    // int* neutrals = (int*)malloc(sizeof(int) * LIST_LEN);
    // for (int i = 0; i < LIST_LEN; i++) {
    //     neutrals[i] = 0;
    // }
    // neutrals にはその時点で最善手を決められてないものを入れる
    int iter = 0;
    
    // どちらかが勝っているものをスコア付け
    for (int i = 0; i < states.size(); i++) {
        // if (states[i] == 0) {
        //     continue; // valid state cannot be 0
        // }

        if (is_won(states[i], BLACK)) {
            scores[states[i]] = MAX_DEPTH;
        } else if (is_won(states[i], WHITE)) {
            scores[states[i]] = -MAX_DEPTH;
        } else {
            neutrals.push_back(states[i]);
            iter++;
        }
    }
    
    int prev = -1;
    while (prev != neutrals.size()) {
        // これ以上進まなくなるまで，すでにスコア付けされている状態の一手前にスコア付け
        prev = neutrals.size();
        printf("while\n");
        // iter = 0;
        // int* tmp = (int*)malloc(sizeof(int) * LIST_LEN);
        // for (int i = 0; i < LIST_LEN; i++) {
        //     tmp[i] = 0;
        // }
        std::vector<int> tmp;
        
        for (int i = 0; i < neutrals.size(); i++) {
            if (neutrals[i] == 0) continue;
            int min = INF;
            int best_move;
            int* moves = get_moves(neutrals[i]);
            for (int j = 0; j < 24; j++) {
                if (moves[j] == 0) continue;
                // if (scores[moves[j]] < min) {
                //     min = scores[moves[j]];
                //     best_move = moves[j];
                // }
                int opposite = switch_player(moves[j]);
                if (scores[opposite] < min) {
                    min = scores[opposite];
                    best_move = moves[j];
                }
            }
            if (min == 0) {
                // tmp[iter] = neutrals[i];
                tmp.push_back(neutrals[i]);
                // iter++;
                continue;
            }

            if (min < 0) scores[neutrals[i]] = -1 - min;
            else scores[neutrals[i]] = 1 - min;

            // next_move[neutrals[i]] = best_move;

            // printf("%d,%d,%d\n", neutrals[i], best_move, scores[neutrals[i]]);
        }

        neutrals = tmp;
        
        // memcpy(neutrals, tmp, sizeof(int) * LIST_LEN);
        // free(tmp);
    }

    // for (int i = 0; i < LIST_LEN; i++) {
    //     if (scores[i] != 0) {
    //         printf("%d,%d,%d\n", i, scores[i], next_move[i]);
    //     }
    // }

    // free(states);
    // free(neutrals);
}