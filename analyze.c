#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SIZE 999983 // size for hashtable

const int BLACK = 1;
const int WHITE = 2;
const int MAX_SCORE = 1e5;
const int INF = 1e8;
const int COMB_LEN = 2500;

// location of pawn is expressed by an integer between 0 and 24
// state of board is expressed by 5 * 6 = 30 bit integer
//     A  B  C  D  E
//   ---------------
// 5| 20 21 22 23 24
// 4| 15 16 17 18 19
// 3| 10 11 12 13 14
// 2|  5  6  7  8  9
// 1|  0  1  2  3  4

typedef struct DI {
    // data element of hashtable
    int data;
    int key;
} DataItem;

typedef struct node{
    // node of queue
    struct node* next;
    int value;
} Node;

typedef struct tag{
    // data format of queue
    Node* front;
    Node* tail;
} Tag;

// hashtable
DataItem *hash_array[SIZE];

int hash_code(int key) {
    // hash function
    return key % SIZE;
}

DataItem *hash_search(int key) {
    // search key from hashtable linearly
    int hashIndex = hash_code(key);
    while(hash_array[hashIndex] != NULL){
        if(hash_array[hashIndex]->key == key) {
            return hash_array[hashIndex];
        }
        hashIndex++;
        hashIndex %= SIZE;
    }
    // when not found
    return NULL;
}

void hash_insert(int key, int data) {
    // insert data to hashtable
    DataItem *item = (DataItem *)malloc(sizeof(DataItem));
    item->data = data;
    item->key = key;
    int hashIndex = hash_code(key);
    while(hash_array[hashIndex] != NULL && hash_array[hashIndex]->key != -1){
        hashIndex++;
        hashIndex %= SIZE;
    }
    hash_array[hashIndex] = item;
}

Tag* queue_create(){
    // implement queue by list
    Tag* queue = (Tag*)malloc(sizeof(Tag));
    (*queue).front = NULL;
    (*queue).tail = NULL;
    return queue;
}

void queue_enqueue(Tag* queue, int value){
    // push a value to the tail
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
    // pop the front value of queue
    if((*queue).front == NULL){
        // when queue is empty
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
    // make `state` increasing order for each player
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
    // flip the current board horizontally given state in int
    int res = 0;
    for (int i = 0; i < 6; i++) {
        int tmp = (state >> (25 - i * 5)) & 0b11111;
        tmp = 5 * (tmp / 5) + 4 - (tmp % 5);
        res |= tmp << (25 - i * 5);
    }
    return res;
}

int rotate(const int state) {
    // rotate the current board given state in int
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
    // only consider one situation out of 8 equivalent situations
    // that can be reached by mirroring or rotation
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
    // change first and second players given state in int
    int y = (x & ((1 << 15) - 1)) << 15;
    y |= (x >> 15);
    return equiv(y);
}

int is_won(int x, int color) {
    // judge if player of `color` has won the game
    if (color == WHITE) return is_won(switch_player(x), BLACK);

    int a = (x >> 25) & 0b11111;
    int b = (x >> 20) & 0b11111;
    int c = (x >> 15) & 0b11111;

    // horizontally
    if (a / 5 == b / 5 && b / 5 == c / 5) {
        if (a + 1 == b && b + 1 == c) {
            return 1;
        }
    }

    // vertically
    if (a + 5 == b && b + 5 == c) return 1;

    // diagonally
    if (a / 5 + 1 == b / 5 && b / 5 + 1 == c / 5) {
        if (a + 6 == b && b + 6 == c) return 1;
        if (a + 4 == b && b + 4 == c) return 1;
    }

    return 0;
}

void recursive_comb(int** res, int* positions, int* indexes, int s, int rest, int* iter) {
    // compute combination and store them to `res`
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
    // along with recursive_comb, enumerate all three combinations out of 25 or 22 locations
    // `positions` is used to prevent multiple pawns from locating at the same position
    // when positions[i] == -1, location i is used by the opposite player
    int indexes[3];
    
    int **res = (int**)malloc(sizeof(int*) * COMB_LEN);
    // COMB_LEN > 25C3
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
    // list every possible situation
    // each situation is expressed by one int
    Tag* res = queue_create();
    int positions[25];

    for (int i = 0; i < 25; i++) positions[i] = i;
    int** comb1 = comb(positions);
    // comb1 contains the locations of first player's pawns

    for (int i = 0; i < COMB_LEN; i++) {
        if (comb1[i][2] == 0) continue;
        int remain[25];
        for (int i = 0; i < 25; i++) remain[i] = i;
        for (int k = 0; k < 3; k++) remain[comb1[i][k]] = -1;
        int** comb2 = comb(remain);
        // comb1 contains the locations of second player's pawns
        for (int j = 0; j < COMB_LEN; j++) {
            if (comb2[j][2] == 0) continue;
            int state = (comb1[i][0] << 25) 
                        | (comb1[i][1] << 20) 
                        | (comb1[i][2] << 15) 
                        | (comb2[j][0] << 10) 
                        | (comb2[j][1] <<  5) 
                        | (comb2[j][2] <<  0);
            if (state != equiv(state)) continue;
            // only consider one out of 8 equivalent situations
            if (is_won(state, BLACK) && is_won(state, WHITE)) continue;
            // two players cannot win at the same time
            queue_enqueue(res, state);
        }
        if (comb2 != NULL) free(comb2);
    }
    if (comb1 != NULL) free(comb1);

    return res;
}

int* get_moves(const int x) {
    // given a certain state, return list of 
    // all valid states after one step by first player
    int* res = (int*)malloc(sizeof(int) * 24);
    for (int i = 0; i < 24; i++) res[i] = 0;

    // convert state to 5 * 5 matrix
    int board[5][5] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    board[((x >> 25) & 31) / 5][((x >> 25) & 31) % 5] = BLACK;
    board[((x >> 20) & 31) / 5][((x >> 20) & 31) % 5] = BLACK;
    board[((x >> 15) & 31) / 5][((x >> 15) & 31) % 5] = BLACK;
    board[((x >> 10) & 31) / 5][((x >> 10) & 31) % 5] = WHITE;
    board[((x >>  5) & 31) / 5][((x >>  5) & 31) % 5] = WHITE;
    board[((x >>  0) & 31) / 5][((x >>  0) & 31) % 5] = WHITE;
    int iter = 0;

    for (int pawn = 0; pawn < 3; pawn++) {
        int a = ((x >> (25 - 5 * pawn)) & 31) / 5;
        int b = ((x >> (25 - 5 * pawn)) & 31) % 5;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // i and j are for direction
                if (i == 0 && j == 0) continue;
                for (int s = 1; s < 5; s++) {
                    // s is for distance
                    if (a + i * s < 0 || a + i * s >= 5 || b + j * s < 0 || b + j * s >= 5) break;

                    if (board[a + i * s][b + j * s] == 0) {
                        // move from (a, b) to (a + j * s, b + i * s)
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
    Tag* states = get_all_states();
    Tag* neutrals = queue_create();
    // `neutrals` stores neutral (= yet to be determined next movement) situations at that time
    int cnt = 0;
    
    // give sufficiently big or small score to situations that one player is winning
    while(1) {
        int tmp_state = queue_dequeue(states);
        if (tmp_state == -1) break;

        if (is_won(tmp_state, BLACK)) {
            hash_insert(tmp_state, MAX_SCORE);
        } else if (is_won(tmp_state, WHITE)) {
            hash_insert(tmp_state, -MAX_SCORE);
        } else {
            queue_enqueue(neutrals, tmp_state);
            cnt++;
        }
    }
    
    int prev = -1;
    while (prev != cnt) {
        // give score to situations that is one step before situations which is already scored
        prev = cnt;
        cnt = 0;
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
                // choose next movement so that the score for the opposite player minimizes
                int opposite = switch_player(moves[j]);
                DataItem *opposite_data = hash_search(opposite);
                int opposite_score;
                if (opposite_data == NULL) opposite_score = 0;
                else opposite_score = opposite_data->data;
                if (opposite_score < min) {
                    min = opposite_score;
                    best_move = moves[j];
                }
            }
            if (min == 0) {
                queue_enqueue(tmp, tmp_state);
                cnt++;
                continue;
            }

            int score = min < 0 ? -1 - min : 1 - min;
            hash_insert(tmp_state, score);

            // save current state, next movement, and current score to use during the match
            printf("%d,%d,%d\n", tmp_state, best_move, score);
        }

        neutrals = tmp;
    }
}