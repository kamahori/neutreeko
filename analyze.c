#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BLACK = 1;
const int WHITE = 2;
const int MAX_DEPTH = 1e5;
const int INF = 1e8;
const int LIST_LEN = 4e6;
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

int scores[1 << 30];
int next_move[1 << 30];

int switch_player(const int x) {
    int y = (x & ((1 << 15) - 1)) << 15;
    y |= (x >> 15);
    return y;
}

int is_won(int x, int color) {
    if (color == WHITE) return is_won(switch_player(x), BLACK);
    
    int board[5][5];
    board[((x >> 25) & 31) / 5][((x >> 25) & 31) % 5] = BLACK;
    board[((x >> 20) & 31) / 5][((x >> 20) & 31) % 5] = BLACK;
    board[((x >> 15) & 31) / 5][((x >> 15) & 31) % 5] = BLACK;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 3; k++) {
                if (board[i][j] == BLACK && board[i][j] == board[i + 1][j] && board[i + 1][j] == board[i + 2][j]) return 1;
                if (board[j][i] == BLACK && board[j][i] == board[j][i + 1] && board[j][i + 1] == board[j][i + 2]) return 1;
                if (board[i][k] == BLACK && board[i][k] == board[i + 1][k + 1] && board[i + 1][k + 1] == board[i + 2][k + 2]) return 1;
                if (board[i + 2][k] == BLACK && board[i + 2][k] == board[i + 1][k + 1] && board[i + 1][k + 1] == board[i][k + 2]) return 1;
            }
        }
    }

    return 0;
}

void recursive_comb(int** res, int* positions, int* indexes, int s, int rest, int* iter) {
    if (rest == 0) {
        for (int i = 0; i < 3; i++) res[*iter][i] = indexes[i];
        printf("%d, %d, %d\n", indexes[0], indexes[1], indexes[2]);
        *iter += 1;
        return;
    }
    if (s >= 25 || positions[s] == -1) return;
    recursive_comb(res, positions, indexes, s + 1, rest, iter);
    indexes[3 - rest] = s;
    recursive_comb(res, positions, indexes, s + 1, rest - 1, iter);
}

int** comb(int* positions) {
    int indexes[3];
    
    int **res = (int**)malloc(sizeof(int*) * COMB_LEN);
    for (int i = 0; i < COMB_LEN; i++) {
        res[i] = (int*)malloc(sizeof(int) * 3);
    }
    int iter = 0;
    // printf("before recursion\n");
    recursive_comb(res, positions, indexes, 0, 3, &iter);
    return res;
}

int* get_all_states() {
    int* res = (int*)malloc(sizeof(int) * LIST_LEN); // 4e6 > 25C3 * 22C3
    int positions[25];
    int iter = 0;

    for (int i = 0; i < 25; i++) positions[i] = i;
    // printf("before comb1\n");
    int** comb1 = comb(positions);
    printf("------------------------\n");
    for (int i = 0; i < COMB_LEN; i++) {
        if (comb1[i][0] == 0 && comb1[i][1] == 0) continue;
        if (comb1[i][1] == 0 || comb1[i][2] == 0) continue;
        int remain[25];
        memcpy(remain, positions, sizeof(positions));
        for (int k = 0; k < 3; k++) remain[comb1[i][k]] = -1;
        int** comb2 = comb(remain);
        for (int j = 0; j < COMB_LEN; j++) {
            if (comb2[j][0] == 0 && comb2[j][1] == 0) continue;
            if (comb2[j][1] == 0 || comb2[j][2] == 0) continue;
            int state = (comb1[i][0] << 25) + (comb1[i][1] << 20) + (comb1[i][2] << 15) + (comb2[j][0] << 10) + (comb2[j][1] << 5) + comb2[j][2];
            if (is_won(state, BLACK) && is_won(state, WHITE)) continue;
            // printf("%d, %d, %d, %d, %d, %d\n", comb1[i][0], comb1[i][1], comb1[i][2], comb2[j][0], comb2[j][1], comb2[j][2]);
            res[iter] = state;
            iter++;
        }
    }
    printf("in get_all_states, iter:%d\n", iter);

    return res;
}

int* get_moves(const int x) {
    // given a certain state, return list of 
    // all valid states after one step by black
    int* res = (int*)malloc(sizeof(int) * 24);
    int board[5][5];
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
                    }else{
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
    int* states = get_all_states();
    // printf("get states\n");
    int* neutrals = (int*)malloc(sizeof(int) * LIST_LEN);
    // printf("neutrals\n");
    int iter = 0;
    
    for (int i = 0; i < LIST_LEN; i++) {
        if (states[i] == 0) continue; // valid state cannot be 0
        printf("%d\n", states[i]);
        if (is_won(states[i], BLACK)) scores[i] = MAX_DEPTH;
        else if (is_won(states[i], WHITE)) scores[i] = -MAX_DEPTH;
        else {
            neutrals[iter] = states[i];
            iter++;
        }
    }
    // printf("first itertaion\n");
    
    int prev = -1;
    // printf("state,move\n");
    while (prev != iter) {
        printf("in while, %d\n", iter);
        prev = iter;
        iter = 0;
        int* tmp = (int*)malloc(sizeof(int) * LIST_LEN);
        
        for (int i = 0; i < LIST_LEN; i++) {
            if (neutrals[i] == 0) continue;
            int min = INF;
            int best_move;
            int* moves = get_moves(neutrals[i]);
            for (int j = 0; j < 24; j++) {
                if (moves[j] == 0) continue;
                int opposite = switch_player(moves[j]);
                if (scores[opposite] < min) {
                    min = scores[opposite];
                    best_move = moves[j];
                }
            }
            if (min == 0) {
                tmp[iter] = neutrals[i];
                iter++;
                continue;
            }

            if (min < 0) scores[neutrals[i]] = -1 - min;
            else scores[neutrals[i]] = 1 - min;

            next_move[neutrals[i]] = best_move;
            printf("%d,%d\n", neutrals[i], best_move);
        }
        
        memcpy(neutrals, tmp, sizeof(tmp));
        free(tmp);
    }
    free(states);
    free(neutrals);
}