#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BLACK = 1;
const int WHITE = 2;
int DEBUG = 0;
int answer[8][2];

// situation of the game is stored in `board` or int
// `board` holds the state of the whole board
typedef struct board {
    int state[5][5];
    // 0: empty
    // 1: black (first player)
    // 2: white (second player)
} board;

board g_board;
// global varibale to contain current state

void init() {
    // initialize
    int x, y;
    for (x = 0; x < 5; x++) {
        for (y = 0; y < 5; y++) {
            g_board.state[x][y] = 0;
        }
    }
    g_board.state[0][1] = BLACK;
    g_board.state[0][3] = BLACK;
    g_board.state[3][2] = BLACK;
    g_board.state[4][1] = WHITE;
    g_board.state[4][3] = WHITE;
    g_board.state[1][2] = WHITE;

    //    A B C D E
    //   ----------
    // 5| 0 2 0 2 0 
    // 4| 0 0 1 0 0 
    // 3| 0 0 0 0 0 
    // 2| 0 0 2 0 0 
    // 1| 0 1 0 1 0
}

void print(void) {
    // print current board if debug mode is on
    if (DEBUG == 0) return;

    printf("   A B C D E\n");
    printf("  ----------\n");
    for (int i = 0; i < 5; i++) {
        printf("%d| ", 5 - i);
        for (int j = 0; j < 5; j++) {
            printf("%d ", g_board.state[4 - i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int is_finished(board b) {
    // judge if the game is finished or not
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 3; k++) {
                if (b.state[i][j] != 0 &&
                    b.state[i][j] == b.state[i + 1][j] && 
                    b.state[i + 1][j] == b.state[i + 2][j]) return 1;
                if (b.state[j][i] != 0 && 
                    b.state[j][i] == b.state[j][i + 1] && 
                    b.state[j][i + 1] == b.state[j][i + 2]) return 1;
                if (b.state[i][k] != 0 && 
                    b.state[i][k] == b.state[i + 1][k + 1] && 
                    b.state[i + 1][k + 1] == b.state[i + 2][k + 2]) return 1;
                if (b.state[i + 2][k] != 0 && 
                    b.state[i + 2][k] == b.state[i + 1][k + 1] && 
                    b.state[i + 1][k + 1] == b.state[i][k + 2]) return 1;
            }
        }
    }
    return 0;
}

void moveable(board b, int y, int x) {
    // given a board and coordinate, compute all the locations
    // that the pawn on (y, x) can move to.
    // output is stored in global variable `answer`
    for (int i = 0; i < 8; i++) {
        // initialize answer
        answer[i][0] = 100;
        answer[i][1] = 100;
    }

    int iter;
    iter = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // i and j are for direction
            if (i == 0 && j == 0) {
                continue;
            }
            for (int s = 1; s < 5; s++) {
                // s is for distance
                if (y + j * s < 0 || y + j * s >= 5 || x + i * s < 0 || x + i * s >= 5) {
                    break;
                }
                if (b.state[y + j * s][x + i * s] == 0) {
                    // if destination is empty
                    answer[iter][0] = y + j * s;
                    answer[iter][1] = x + i * s;
                } else {
                    break;
                }
            }
            iter++;
        }
    }
}

int is_valid_move(char* input) {
    // judge if the input is given properly
    if (input[0] < '1' || input[0] > '5') return 0;
    if (input[1] < 'A' || input[1] > 'E') return 0;
    if (input[2] < '1' || input[2] > '5') return 0;
    if (input[3] < 'A' || input[3] > 'E') return 0;
    return 1;
}

void get_input(int color) {
    // move pawns according to the input
    char input[5];
    int y, x, w, z;

    scanf("%s", input);

    // transform input to numbers
    y = input[0] - '1';
    x = input[1] - 'A';
    w = input[2] - '1';
    z = input[3] - 'A';

    if (!is_valid_move(input) || g_board.state[y][x] != color) {
        printf("Error: illegal input\n");
        exit(1);
    }

    moveable(g_board, y, x);

    for (int i = 0; i < 8; i++) {
        if (answer[i][0] == w && answer[i][1] == z) {
            // if the pawn on (y, x) can move to (w, z)
            g_board.state[y][x] = 0;
            g_board.state[w][z] = color;
            return;
        }
    }

    // if the pawn cannot move
    printf("Error: cannot move\n");
    exit(1);
}

int sort_state(const int state) {
    // make state in int increasing order
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

board mirror_board(board B) {
    // flip the current board horizontally given state in `board`
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 2; j++) {
            int tmp = B.state[i][j];
            B.state[i][j] = B.state[i][4 - j];
            B.state[i][4 - j] = tmp;
        }
    }
    return B;
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

board rotate_board(board B) {
    // rotate the current board given state in `board`
    board res;
    for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) res.state[i][j] = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            res.state[i][j] = B.state[j][i];
        }
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

int distance(board x, board y) {
    // compute how many states of x and y are different
    int res = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (x.state[i][j] != y.state[i][j]) res++;
        }
    }
    return res;
}

void print_diff(board b, board b_next) {
    // print the movement from b to b_next
    char res[4];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (b.state[i][j] == b_next.state[i][j]) continue;
            if (b_next.state[i][j] == 0) {
                res[0] = '1' + i;
                res[1] = 'A' + j;
            } else {
                res[2] = '1' + i;
                res[3] = 'A' + j;
            }
        }
    }
    printf("%c%c%c%c\n", res[0], res[1], res[2], res[3]);
    g_board = b_next;
}

void diff(board b, board b_next) {
    // compute the movement from b to b_next
    // `b_next` can be whatever situation (rotation, mirroring) out of 8 equivalent ones
    if (distance(b, b_next) == 2) return print_diff(b, b_next);

    b_next = mirror_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);
    b_next = rotate_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);

    b_next = mirror_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);
    b_next = rotate_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);

    b_next = mirror_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);
    b_next = rotate_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);

    b_next = mirror_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);
    b_next = rotate_board(b_next);
    if (distance(b, b_next) == 2) return print_diff(b, b_next);
}

int switch_player(const int x) {
    // change first and second players given state in int
    int y = (x & ((1 << 15) - 1)) << 15;
    y |= (x >> 15);
    return equiv(y);
}

int search(int state) {
    // search the movement that corresponds to `state` from 'output.txt'
    FILE *fp;
    int tmp = equiv(state);
    int current, next, score;
    if ((fp = fopen("output.txt", "r")) == NULL) {
        printf("cannot read file\n");
        exit(1);
    } else {
        while (fscanf(fp, "%d,%d,%d", &current, &next, &score) != EOF) {
            if (current == tmp && score > 0) {
                return equiv(next);
            }
        }
    }
    return -1;
}

void compute_output(int color) {
    // calculate the computer's movement
    // `color` is 1 (black) or 2 (white)
    int cpu = 0, human = 0;
    int state = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (g_board.state[i][j] == color) {
                state |= ((5 * i + j) << (25 - 5 * cpu));
                cpu++;
            } else if (g_board.state[i][j] == 3 - color) {
                state |= ((5 * i + j) << (10 - 5 * human));
                human++;
            }
        }
    }
    state = equiv(state);
    int next = search(state);

    board b_tmp;
    for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) b_tmp.state[i][j] = 0;

    for (int i = 0; i < 3; i++) {
        int loc = (state >> (25 - 5 * i)) & 0b11111;
        b_tmp.state[loc / 5][loc % 5] = BLACK;
    }
    for (int i = 0; i < 3; i++) {
        int loc = (state >> (10 - 5 * i)) & 0b11111;
        b_tmp.state[loc / 5][loc % 5] = WHITE;
    }

    if (next == -1) {
        for (int i = 0; i < 3; i++) {
            int isfound = 0;
            int loc = (state >> (25 - 5 * i)) & 0b11111;
            moveable(b_tmp, loc / 5, loc % 5);
            for (int j = 0; j < 8; j++) {
                if (answer[j][0] == 100 && answer[j][1] == 100) continue;
                // move from (loc / 5,  loc % 5) to (answer[j][0], answer[j][1])
                int mask = (1 << 15) - 1;
                int tmp_state = state & mask;
                int b[3] = {(state >> 25) & 0b11111, (state >> 20) & 0b11111, (state >> 15) & 0b11111};
                b[i] = answer[j][0] * 5 + answer[j][1];
                tmp_state |= (b[0] << 25) | (b[1] << 20) | (b[2] << 15);
                tmp_state = equiv(tmp_state);
                tmp_state = switch_player(tmp_state);
                if (search(tmp_state) < 0) {
                    next = switch_player(tmp_state);
                    isfound = 1;
                    break;
                }
            }
            if (isfound) break;
        }
    }

    if (next == -1) {
        printf("cannot choose next move\n");
        exit(1);
    }

    board b_next;
    for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) b_next.state[i][j] = 0;

    for (int i = 0; i < 3; i++) {
        int loc = (next >> (25 - 5 * i)) & 0b11111;
        b_next.state[loc / 5][loc % 5] = color;
    }
    for (int i = 0; i < 3; i++) {
        int loc = (next >> (10 - 5 * i)) & 0b11111;
        b_next.state[loc / 5][loc % 5] = 3 - color;
    }

    diff(g_board, b_next);
}

int main(int argc, char* argv[]) {
    init();
    int cnt = 0;

    if (argc >= 3 && !strcmp(argv[2], "1")) DEBUG = 1;

    if (!strcmp(argv[1], "0")) {
        // human plays first (black)
        while (1) {
            print();
            // human's turn
            get_input(BLACK);
            cnt++;
            print();
            if (is_finished(g_board)) {
                printf("You Win\n");
                exit(0);
            }

            // computer's turn
            compute_output(WHITE);
            cnt++;
            if (is_finished(g_board)) {
                print();
                printf("You Lose\n");
                exit(0);
            }

            if (cnt >= 300) {
                // stop after 300 turns
                print();
                printf("Even\n");
                exit(0);
            }
        }
    } else {
        // computer plays first (black)
        while (1) {
            print();
            // computer's turn
            compute_output(BLACK);
            cnt++;
            print();
            if (is_finished(g_board)) {
                printf("You Lose\n");
                exit(0);
            }

            // human's turn
            get_input(WHITE);
            cnt++;
            if (is_finished(g_board)) {
                print();
                printf("You Win\n");
                exit(0);
            }

            if (cnt >= 300) {
                // stop after 300 turns
                print();
                printf("Even\n");
                exit(0);
            }
        }
    }

    return 0;
}
