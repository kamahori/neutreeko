#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const int BLACK = 1;
const int WHITE = 2;
const int MAX_DEPTH = 1e5;
const int INF = 1e8;
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

//keyは状態を表すint、dataはそのscore

#define SIZE 999983

typedef struct DI {
    int data;
    int key;
} DataItem;

//DataItemのポインタの配列、DataItemのポインタ
DataItem *hashArray[SIZE];

//ハッシュ関数の定義
int hash_code(int key){
    return key % SIZE;
}

//線形探索法による検索
DataItem *hash_search(int key)
{
    int hashIndex = hash_code(key);
    while(hashArray[hashIndex] != NULL){
        if(hashArray[hashIndex]->key == key) {
            return hashArray[hashIndex];
        }
        hashIndex++;
        hashIndex %= SIZE;
    }
    //見つからなかった場合
    return NULL;
}

//挿入
void hash_insert(int key, int data){
    DataItem *item = (DataItem *)malloc(sizeof(DataItem));
    item->data = data;
    item->key = key;
    int hashIndex = hash_code(key);
    while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1){
        hashIndex++;
        hashIndex %= SIZE;
    }
    hashArray[hashIndex] = item;
}

// listによるqueueの実装

typedef struct node{
    struct node* next;
    int value;
} Node;

typedef struct tag{
    Node* front;
    Node* tail;
} Tag;

Tag* queue_create(){
    Tag* queue = (Tag*)malloc(sizeof(Tag));
    (*queue).front = NULL;
    (*queue).tail = NULL;
    return queue;
}

void queue_delete(Tag* queue){
    Node* p = (*queue).front;
    Node* tmp;
    while(p != NULL) {
        tmp = (*p).next;
        if (p != NULL) free(p);
        p = tmp;
    }
    if (queue != NULL) free(queue);
}

void queue_enqueue(Tag* queue, int value){
    Node* p = (Node*)malloc(sizeof(Node));
    (*p).value = value;
    (*p).next = NULL;
    if((*queue).tail != NULL){
        (*(*queue).tail).next = p;
    }
    (*queue).tail = p;
    if((*queue).front == NULL){
        (*queue).front = p;
    }
}

int queue_dequeue(Tag* queue){
    //queueが空の時は、-1を返す。それ以外の時は先頭のvalueを返す
    if((*queue).front == NULL){
        return -1;
    }
    Node* p = (*queue).front;
    int pop_value = (*p).value;
    (*queue).front = (*p).next;
    if(p == (*queue).tail){
        (*queue).tail = NULL;
    }
    if (p != NULL) free(p);
    return pop_value;
}

int sort_state(const int state) {
    int res = 0;

    int b[3] = {(state >> 25) & 0b11111, (state >> 20) & 0b11111, (state >> 15) & 0b11111};
    int w[3] = {(state >> 10) & 0b11111, (state >>  5) & 0b11111, (state >>  0) & 0b11111};
    
    if (b[1] > b[2]) {
        int keep = b[1];
        b[1] = b[2];
        b[2] = keep;
    }
    if (b[0] > b[1]) {
        int keep = b[0];
        b[0] = b[1];
        b[1] = keep;
    }
    if (b[1] > b[2]) {
        int keep = b[1];
        b[1] = b[2];
        b[2] = keep;
    }

    if (w[1] > w[2]) {
        int keep = w[1];
        w[1] = w[2];
        w[2] = keep;
    }
    if (w[0] > w[1]) {
        int keep = w[0];
        w[0] = w[1];
        w[1] = keep;
    }
    if (w[1] > w[2]) {
        int keep = w[1];
        w[1] = w[2];
        w[2] = keep;
    }

    res = (b[0] << 25) | (b[1] << 20) | (b[2] << 15) | (w[0] << 10) | (w[1] << 5) | w[2];
    
    return res;
}

int mirror(const int state) {
    int res = 0;
    for (int i = 0; i < 6; i++) {
        int tmp = (state >> (25 - i * 5)) & 0b11111;
        tmp = 5 * (tmp / 5) + 4 - (tmp % 5);
        res |= tmp << (25 - i * 5);
    }
    return res;
}

int rotate(const int state) {
    int res = 0;
    for (int i = 0; i < 6; i++) {
        int tmp = (state >> (25 - i * 5)) & 0b11111;
        int x = tmp % 5, y = tmp / 5;
        tmp = x * 5 + y;
        res |= tmp << (25 - i * 5);
    }
    return res;
}

int equiv(const int state) {
    int res = sort_state(state);
    int tmp = state;

    tmp = mirror(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);
    tmp = rotate(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);

    tmp = mirror(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);
    tmp = rotate(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);

    tmp = mirror(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);
    tmp = rotate(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);

    tmp = mirror(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);
    tmp = rotate(tmp);
    if (res > sort_state(tmp)) res = sort_state(tmp);

    return res;
}

int switch_player(const int x) {
    int y = (x & ((1 << 15) - 1)) << 15;
    y |= (x >> 15);
    return equiv(y);
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

Tag* get_all_states() {
    // ありうるすべての状態を列挙
    Tag* res = queue_create();
    // std::vector<int> res;
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
            if (state != equiv(state)) continue;
            if (is_won(state, BLACK) && is_won(state, WHITE)) continue; // 両者が同時に勝つような場合は除外
            // res[iter] = state;
            // iter++;
            // printf("%d\n", state);
            // res.push_back(state);
            queue_enqueue(res, state);
        }
        if (comb2 != NULL) free(comb2);
    }
    if (comb1 != NULL) free(comb1);

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
                // res[iter] = -1;
                if (i == 0 && j == 0) continue;
                for (int s = 1; s < 5; s++) {
                    if (a + i * s < 0 || a + i * s >= 5 || b + j * s < 0 || b + j * s >= 5) continue;

                    if (board[a + i * s][b + j * s] == 0) {
                        // (a, b) -> (a + j * s, b + i * s)
                        int tmp = x;
                        int mask = ((1 << 30) - 1) - ((1 << (30 - 5 * pawn)) - 1) + ((1 << (25 - 5 * pawn)) - 1);
                        tmp &= mask;
                        tmp |= (((a + i * s) * 5 + b + j * s) << (25 - 5 * pawn));
                        res[iter] = equiv(tmp);
                    } else {
                        break;
                    }
                }
                iter++;
            }
        }
    }

    return res;
}

int main() {

    // std::map<int, int> scores;
    // std::map<int, int> next_move;
    // int* scores = (int*)malloc(sizeof(int) * (1 << 30));
    // int* next_move = (int*)malloc(sizeof(int) * (1 << 30));
    // for (int i = 0; i < (1 << 30); i++) {
    //     scores[i] = 0;
    //     next_move[i] = 0;
    // }
    // printf("done\n");

    Tag* states = get_all_states();
    Tag* neutrals = queue_create();
    // int* states = get_all_states();
    // int* neutrals = (int*)malloc(sizeof(int) * LIST_LEN);
    // for (int i = 0; i < LIST_LEN; i++) {
    //     neutrals[i] = 0;
    // }
    // neutrals にはその時点で最善手を決められてないものを入れる
    int iter = 0;
    
    // どちらかが勝っているものをスコア付け
    while(1) {
        int tmp_state = queue_dequeue(states);
        if (tmp_state == -1) break;
        // if (states[i] == 0) {
        //     continue; // valid state cannot be 0
        // }

        if (is_won(tmp_state, BLACK)) {
            hash_insert(tmp_state, MAX_DEPTH);
            // scores[states[i]] = MAX_DEPTH;
        } else if (is_won(tmp_state, WHITE)) {
            hash_insert(tmp_state, -MAX_DEPTH);
            // scores[states[i]] = -MAX_DEPTH;
        } else {
            queue_enqueue(neutrals, tmp_state);
            // neutrals.push_back(tmp_state);
            iter++;
        }
    }
    
    int prev = -1;
    while (prev != iter) {
        // これ以上進まなくなるまで，すでにスコア付けされている状態の一手前にスコア付け
        prev = iter;
        // printf("while\n");
        iter = 0;
        // int* tmp = (int*)malloc(sizeof(int) * LIST_LEN);
        // for (int i = 0; i < LIST_LEN; i++) {
        //     tmp[i] = 0;
        // }
        Tag* tmp = queue_create();
        
        while (1) {
            int tmp_state = queue_dequeue(neutrals);
            if (tmp_state == -1) break;
            if (tmp_state == 0) continue;
            int min = INF;
            int best_move;
            int* moves = get_moves(tmp_state);
            for (int j = 0; j < 24; j++) {
                if (moves[j] == 0) continue;
                // if (scores[moves[j]] < min) {
                //     min = scores[moves[j]];
                //     best_move = moves[j];
                // }
                int opposite = switch_player(moves[j]);
                DataItem *opposite_data = hash_search(opposite);
                int opposite_score;
                if (opposite_data == NULL) opposite_score = 0;
                else opposite_score = opposite_data->data;
                // if (opposite_data->data == NULL) opposite_score = 0;
                // else opposite_score = opposite_data->data;
                if (opposite_score < min) {
                    min = opposite_score;
                    best_move = moves[j];
                }
            }
            if (min == 0) {
                // tmp[iter] = neutrals[i];
                // tmp.push_back(neutrals[i]);
                queue_enqueue(tmp, tmp_state);
                iter++;
                continue;
            }

            int score = min < 0 ? -1 - min : 1 - min;
            // scores[neutrals[i]] = score;
            hash_insert(tmp_state, score);
            // if (min < 0) scores[neutrals[i]] = -1 - min;
            // else scores[neutrals[i]] = 1 - min;

            // next_move[neutrals[i]] = best_move;

            printf("%d,%d,%d\n", tmp_state, best_move, score);
        }

        neutrals = tmp;
        // queue_delete(tmp);

        // neutrals.resize(tmp.size());
        // for (int i = 0; i < tmp.size(); i++) neutrals[i] = tmp[i];
        
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