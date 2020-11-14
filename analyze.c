#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BLACK = 1;
const int WHITE = 2;
const int MAX_DEPTH = 1e8;
const int INF = 1e8;
const int LIST_LEN = 4e6;

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
} state

int scores[1 << 30];

int is_won(int state, int color) {
    if (color == WHITE) return is_won(switch_player(state), BLACK);
    //....
}

int state_to_int(state s) {
    int res = 0;
    res |= (s.pos[0] - 'a') << 25;
    res |= (s.pos[1] - 'a') << 20;
    res |= (s.pos[2] - 'a') << 15;
    res |= (s.pos[3] - 'a') << 10;
    res |= (s.pos[4] - 'a') << 5;
    res |= (s.pos[5] - 'a');
    return res;
}

state int_to_state(int x) {
    state res;
    res.pos[0] = ((x >> 25) + 'a') & 31;
    res.pos[1] = ((x >> 20) + 'a') & 31;
    res.pos[2] = ((x >> 15) + 'a') & 31;
    res.pos[3] = ((x >> 10) + 'a') & 31;
    res.pos[4] = ((x >> 5) + 'a') & 31;
    res.pos[5] = ((x >> 0) + 'a') & 31;
    return res;
}

int* get_states() {
    // 25C3 -> 22C3
    // return list of all valid board states
    // pos[0] < pos[1] < pos[2], pos[3] < pos[4] < pos[5]
    // two players do not win at the same time
    int* res = (int*)malloc(sizeof(int) * LIST_LEN); // 4e6 > 25C3 * 22C3

    // ....

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
        int a = ((x >> (25 - 5 * pawn)) & 31) / 5
        int b = ((x >> (25 - 5 * pawn)) & 31) % 5
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                res[iter] = -1;
                if (i == 0 && j == 0) continue;
                for (int s = 1; s < 5; s++) {
                    if (a + j * s < 0 || a + j * s >= 5 || b + i * s < 0 || b + i * s >= 5) break;

                    if (board[a + j * s][b + i * s] == 0) {
                        // (a, b) -> (a + j * s, b + i * s)
                        int tmp = x;
                        int mask = ((1 << 30) - 1) - ((1 << (30 - 5 * pawn)) - 1) + ((1 << (25 - 5 * pawn)) - 1)
                        tmp &= mask;
                        res[iter] = tmp | (((y + j * s) * 5 + x + i * s) << (25 - 5 * pawn));
                    }else{
                        break;
                    }
                }
                iter++;
            }
        }
    }

    return res
}

state switch_player(const int x) {
    int y = (x & ((1 << 15) - 1));
    y |= ((x >> 15) & ((1 << 15) - 1)) << 15;
    return y;
}

int main() {
    int* states = get_states();
    int* neutrals = (int*)malloc(sizeof(int) * LIST_LEN);
    int iter = 0;
    
    for (int i = 0; i < LIST_LEN; i++) {
        if (states[i] == 0) break; // valid state cannot be 0

        if (is_won(states[i], BLACK)) scores[i] = MAX_DEPTH;
        else if (is_won(states[i], WHITE)) scores[i] = -MAX_DEPTH;
        else {
            neutrals[iter] = states[i];
            iter++;
        }
    }
    
    int prev = -1;
    while (prev != iter) {
        prev = iter;
        iter = 0;
        int* next = (int*)malloc(sizeof(int) * LIST_LEN);
        
        for (int i = 0; i < LIST_LEN; i++) {
            if (neutrals[i] == 0) break;
            int min = INF;
            int best_move;
            int* moves = get_moves(neutrals[i]);
            for (int j = 0; j < 24; j++) {
                int opposite = switch_player(moves[j]);
                if (scores[opposite] < min) {
                    min = scores[opposite];
                    best = opposite;
                }
            }
            if (min == 0) {
                next[iter] = neutrals[i];
                iter++;
                continue;
            }

            if (min < 0) scores[neutrals[i]] = -1 - min;
            else scores[neutrals[i]] = 1 - min;
        }
        
        memcpy(neutrals, next, sizeof(next));
        free(next);
    }
    free(states)
    free(neutrals);
}