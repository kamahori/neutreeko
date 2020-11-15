#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const int BLACK = 1;
const int WHITE = 2;
const int MAX_DEPTH = 1e5;
const int INF = 1e8;
const int LIST_LEN = 1e8; //4e6;
const int COMB_LEN = 25000;

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
    // vの2進数表記を出力する
    // maskは2進数表記で100...0
    unsigned int mask = (int)1 << (sizeof(v) * CHAR_BIT - 1);
    //maskの1の位置を右に1つずつずらしながらvとの論理積をとることで、2進数表記のvの各桁を順に出力
    do putchar(mask & v ? '1' : '0');
    while (mask >>= 1);
    printf("\n");
}

int switch_player(const int x) {
    //xの黒を表す15bitと白を表す15bitを交換
    int y = (x & ((1 << 15) - 1)) << 15;
    y |= (x >> 15);
    return y;
}

int state_to_int(state s) {
    //stateで表されている盤面の情報をintに変える
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
    //intで表されている盤面の情報をstateに変える
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
    //盤面xにおいてcolor側が勝利していれば1、そうでなければ0を返す
    //colorがWHITEのときは、盤面の白黒を入れ替えて、黒が勝っているかを判定
    if (color == WHITE) return is_won(switch_player(x), BLACK);
　　
    //黒の勝利判定
    //a,b,cは黒の位置(昇順)
    int a = (x >> 25) & 0b11111;
    int b = (x >> 20) & 0b11111;
    int c = (x >> 15) & 0b11111;
　　
    //横に3つ並んでいる
    if (a / 5 == b / 5 && b / 5 == c / 5) {
        if (a + 1 == b && b + 1 == c) {
            return 1;
        }
    }
   
    //縦に3つ並んでいる
    if (a + 5 == b && b + 5 == c) return 1;

    //ななめに3つ並んでいる
    if (a / 5 + 1 == b / 5 && b / 5 + 1 == c / 5) {
        if (a + 6 == b && b + 6 == c) return 1;
        if (a + 4 == b && b + 4 == c) return 1;
    }

    return 0;
}

void recursive_comb(int** res, int* positions, int* indexes, int s, int rest, int* iter) {
    //res[i][j](j = 0,1,2)をi = 0から順番に埋めていく
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
    // positions[i] == -1 であるようなiは数えない(白の位置を列挙するときに黒が入っているところを含まないようにする)
    int indexes[3];
    
    //resはint*をCOMB_LEN個格納している部分の先頭へのポインタ
    //格納されているint*はintが3つ格納されている部分の先頭へのポインタ。3つのintは黒の位置を表す
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

int* get_all_states() {
    // ありうるすべての状態を列挙
    //すべての状態(intで表す)を格納するメモリを確保。resはその先頭へのポインタ
    int* res = (int*)malloc(sizeof(int) * LIST_LEN); // 4e6 > 25C3 * 22C3
    for (int i = 0; i < LIST_LEN; i++) {
        res[i] = 0;
    }
    int positions[25];
    int iter = 0;

    for (int i = 0; i < 25; i++) positions[i] = i;
    //25か所のうち黒を配置する3か所の組み合わせを列挙
    //comb1はint*をCOMB_LEN個格納している部分の先頭へのポインタ
    //格納されているint*はintが3つ格納されている部分の先頭へのポインタ。3つのintは黒の位置を表す
    //例えば、comb1[0]は{22, 23, 24}の22へのポインタ
    int** comb1 = comb(positions);

    for (int i = 0; i < COMB_LEN; i++) {
        int remain[25];
        for (int i = 0; i < 25; i++) remain[i] = i;
        //remainのうち、黒が既に配置されている場所は-1にする
        for (int k = 0; k < 3; k++) remain[comb1[i][k]] = -1;
        //まだ空いている22か所のうち白を配置する3か所の組み合わせを列挙
        int** comb2 = comb(remain);
        for (int j = 0; j < COMB_LEN; j++) {
            //列挙した黒と白の位置に対応する盤面をintで表す
            int state = (comb1[i][0] << 25) | (comb1[i][1] << 20) | (comb1[i][2] << 15) | (comb2[j][0] << 10) | (comb2[j][1] << 5) | comb2[j][2];
            if (is_won(state, BLACK) && is_won(state, WHITE)) continue; // 両者が同時に勝つような場合は除外
            res[iter] = state;
            iter++;
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
    //xを読み取って盤面を5 * 5行列で表現
    board[((x >> 25) & 31) / 5][((x >> 25) & 31) % 5] = BLACK;
    board[((x >> 20) & 31) / 5][((x >> 20) & 31) % 5] = BLACK;
    board[((x >> 15) & 31) / 5][((x >> 15) & 31) % 5] = BLACK;
    board[((x >> 10) & 31) / 5][((x >> 10) & 31) % 5] = WHITE;
    board[((x >> 5) & 31) / 5][((x >> 5) & 31) % 5] = WHITE;
    board[(x & 31) / 5][(x & 31) % 5] = WHITE;
    int iter = 0;
    for (int pawn = 0; pawn < 3; pawn++) {
        //昇順でpawn番目の黒を動かす
        //その黒の位置はboard[a][b]
        int a = ((x >> (25 - 5 * pawn)) & 31) / 5;
        int b = ((x >> (25 - 5 * pawn)) & 31) % 5;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                //8方位の1つを指定
                res[iter] = -1;
                if (i == 0 && j == 0) continue;
                for (int s = 1; s < 5; s++) {
                    if (a + j * s < 0 || a + j * s >= 5 || b + i * s < 0 || b + i * s >= 5) continue;
                    if (board[a + j * s][b + i * s] == 0) {
                        // (a, b) -> (a + j * s, b + i * s)
                        int mask = (1 << 15) - 1;
                        //tmpはxのうち白の位置の情報を残し、他の部分を0にしたもの
                        int tmp = x & mask;
                        //b1からb3は黒の位置
                        int b1 = (x >> 25) & 31;
                        int b2 = (x >> 20) & 31;
                        int b3 = (x >> 15) & 31;
                        if(pawn == 0){
                            b1 = (a + j * s) * 5 + b + i * s;
                        }
                        else if(pawn == 1){
                            b2 = (a + j * s) * 5 + b + i * s;
                        }
                        else {
                            b3 = (a + j * s) * 5 + b + i * s;
                        }
                        //b1、b2、b3を昇順にする
                        int keep;
                        if(b2 > b3){
                            keep = b2;
                            b2 = b3;
                            b3 = keep;
                        }
                        if(b1 > b2){
                            keep = b1;
                            b1 = b2;
                            b2 = keep;
                        }
                        if(b2 > b3){
                            keep = b2;
                            b2 = b3;
                            b3 = keep;
                        }
                        //tmp(白の位置の情報)と黒の位置の情報の論理和をかんがえることで、両方の情報を持つようにする
                        res[iter] = tmp | (b1 << 25) | (b2 << 20) | (b3 << 15);
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
    // int scores[1 << 30];
    // int next_move[1 << 30];
    // for (int i = 0; i < (1 << 30); i++) {
    //     scores[i] = 0;
    //     next_move[i] = 0;
    // }
    // printf("done\n");

    int* states = get_all_states();
    int* neutrals = (int*)malloc(sizeof(int) * LIST_LEN);
    for (int i = 0; i < LIST_LEN; i++) {
        neutrals[i] = 0;
    }
    // neutrals にはその時点で最善手を決められてないものを入れる
    int iter = 0;
    
    // どちらかが勝っているものをスコア付け
    for (int i = 0; i < LIST_LEN; i++) {
        if (states[i] == 0) {
            continue; // valid state cannot be 0
        }
        if (is_won(states[i], BLACK)) {
            scores[states[i]] = MAX_DEPTH;
        } else if (is_won(states[i], WHITE)) {
            scores[states[i]] = -MAX_DEPTH;
        } else {
            neutrals[iter] = states[i];
            iter++;
        }
    }
    
    int prev = -1;
    while (prev != iter) {
        // これ以上進まなくなるまで，すでにスコア付けされている状態の一手前にスコア付け
        //1回のループでneutralな状態の数が変化しなかったらprev = iterとなるので終了
        prev = iter;
        iter = 0;
        int* tmp = (int*)malloc(sizeof(int) * LIST_LEN);
        for (int i = 0; i < LIST_LEN; i++) {
            tmp[i] = 0;
        }
        
        for (int i = 0; i < LIST_LEN; i++) {
            if (neutrals[i] == 0) continue;
            int min = INF;
            int best_move;
            int* moves = get_moves(neutrals[i]);
            //可能な移動のうち、相手からみたスコアが最小になるような移動を選択
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
        }
        //neutralsを更新
        memcpy(neutrals, tmp, sizeof(int) * LIST_LEN);
        free(tmp);
    }

    for (int i = 0; i < LIST_LEN; i++) {
        if (scores[states[i]] != 0) {
            printf("%d,%d,%d\n", i, scores[states[i]], next_move[states[i]]);
        }
    }

    free(states);
    free(neutrals);
}
