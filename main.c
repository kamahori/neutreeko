#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int answer[8][2];
int BLACK = 1;
int WHITE = 2;
int DEBUG = 0;

//boardは盤面の状態stateと盤面の点数pointを保持する構造体
typedef struct board {
    int state[5][5];
    int point;
} board;

typedef struct move {
    //(x1, y1)から(x2, y2)への遷移
    int x1;
    int y1;
    int x2;
    int y2;
} move;

typedef struct next_board{
    move travel[24];
} next_board;

board b;

void init() { //初期化関数
    int x, y;
    for (x = 0; x < 5; x++) { //基本全部空
        for (y = 0; y < 5; y++) {
            b.state[x][y] = 0;
        }
    }
    b.state[0][1] = BLACK; //B1には黒
    b.state[0][3] = BLACK; //D1には黒
    b.state[3][2] = BLACK; //C4には黒
    b.state[4][1] = WHITE; //B5には白
    b.state[4][3] = WHITE; //D5には白
    b.state[1][2] = WHITE; //C2には白
}

void print(void) {
    if (DEBUG == 0) return;

    printf("   A B C D E\n");
    printf("  ----------\n");
    for (int i = 0; i < 5; i++) {
        printf("%d| ", 5 - i);
        for (int j = 0; j < 5; j++) {
            printf("%d ", b.state[4 - i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int is_finished(board b) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 3; k++) {
                if (b.state[i][j] != 0 && b.state[i][j] == b.state[i + 1][j] && b.state[i + 1][j] == b.state[i + 2][j]) return 1;
                if (b.state[j][i] != 0 && b.state[j][i] == b.state[j][i + 1] && b.state[j][i + 1] == b.state[j][i + 2]) return 1;
                if (b.state[i][k] != 0 && b.state[i][k] == b.state[i + 1][k + 1] && b.state[i + 1][k + 1] == b.state[i + 2][k + 2]) return 1;
                if (b.state[i + 2][k] != 0 && b.state[i + 2][k] == b.state[i + 1][k + 1] && b.state[i + 1][k + 1] == b.state[i][k + 2]) return 1;
            }
        }
    }
    return 0;
}

void moveable(board B, int y, int x) {
    //座標を受け取って動ける場所を列挙する関数
    //answerに動ける場所全てが格納される 
    int i;
    for (i = 0; i < 8; i++) { //answerの初期化
        answer[i][0] = 100;
        answer[i][1] = 100;
    }

    int j, s, k;
    k = 0;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) { //全方位探索
            if (i == 0 && j == 0) {
                continue;
            }
            for (s = 1; s < 5; s++) { //その方位でどこまで行けるか
                if (y + j * s < 0 || y + j * s >= 5 || x + i * s < 0 || x + i * s >= 5) {
                    break;
                }
                if (B.state[y + j * s][x + i * s] == 0) { //進んだ先のマスが空なら
                    answer[k][0] = y + j * s; //answerのi+j-2（方角のパラメータ）にそのマスを書き込む
                    answer[k][1] = x + i * s;
                } else {
                    break;
                }
            }
            k++;
        }
    }

}

int is_valid_move(char* input) {
    if (input[0] < '1' || input[0] > '5') return 0;
    if (input[1] < 'A' || input[1] > 'E') return 0;
    if (input[2] < '1' || input[2] > '5') return 0;
    if (input[3] < 'A' || input[3] > 'E') return 0;
    return 1;
}

void get_input(int color) { //石を動かす関数
    char input[5];
    int y, x, w, z, i;

    scanf("%s", input); //入力"4E2C"などを受け取って

    y = input[0] - '1'; //ASCIIコードを座標に変換
    x = input[1] - 'A';
    w = input[2] - '1';
    z = input[3] - 'A';

    if (!is_valid_move(input) || b.state[y][x] != color) {
        puts("Error: illegal");
        exit(1);
    }

    moveable(b, y, x);

    for (i = 0; i < 8; i++) {
        if (answer[i][0] == w && answer[i][1] == z) { //石が移動できるなら
            b.state[y][x] = 0; //元いた場所を空にして
            b.state[w][z] = color; //移動する
            return;
        }
    }

    // 移動できない場合
    puts("Error: cannot move");
    exit(1);
}

board minimax(board x, int depth, int color) {
    if (depth == 0) {
        x.point = 0;
        return x;
    }
    int i, j, k;
    int count = 0;
    next_board next;

    //盤面から可能な遷移をnextにいれる。
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (x.state[i][j] == color) {

                moveable(x, i, j);

                for (k = 0; k < 8; k++) {
                    if (answer[k][0] != 100) {
                        next.travel[count].x1 = i;
                        next.travel[count].y1 = j;
                        next.travel[count].x2 = answer[k][0];
                        next.travel[count].y2 = answer[k][1];
                        count++;
                    }
                }
            }
        }
    }
    board ans;
    for (i = 0; i < count; i++) {
        board y = x;
        //遷移前のマスを0に、遷移先のマスをcolorに
        y.state[next.travel[i].x1][next.travel[i].y1] = 0;
        y.state[next.travel[i].x2][next.travel[i].y2] = color;

        if (is_finished(y)) {
            y.point = 100;
            return y;
        }
        board z = minimax(y, depth - 1, 3 - color);
        if (i == 0) {
            ans = y;
            continue;
        }
        if (depth % 2 == 1) {
            if (z.point > ans.point) {
                ans = y;
            }
        } else {
            if (z.point < ans.point) {
                ans = y;
            }
        }
    }
    ans.point -= 1;
    return ans;
}

int sort_state(const int state) {
    int res = 0;

    int B[3] = {(state >> 25) & 0b11111, (state >> 20) & 0b11111, (state >> 15) & 0b11111};
    int w[3] = {(state >> 10) & 0b11111, (state >>  5) & 0b11111, (state >>  0) & 0b11111};
    
    if (B[1] > B[2]) {
        int keep = B[1];
        B[1] = B[2];
        B[2] = keep;
    }
    if (B[0] > B[1]) {
        int keep = B[0];
        B[0] = B[1];
        B[1] = keep;
    }
    if (B[1] > B[2]) {
        int keep = B[1];
        B[1] = B[2];
        B[2] = keep;
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

    res = (B[0] << 25) | (B[1] << 20) | (B[2] << 15) | (w[0] << 10) | (w[1] << 5) | w[2];
    
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

board mirror_board(board B) {
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
    int res = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (x.state[i][j] != y.state[i][j]) res++;
        }
    }
    return res;
}

void print_diff(board B, board b_next) {
    // b -> b_nextで変化したところを出力する
    char res[4];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (B.state[i][j] == b_next.state[i][j]) continue;
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
    b = b_next;
}

void diff(board b, board b_next) {
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

int search(int state, int mode) {
    // state に該当する次の動きをファイルから読み込む
    FILE *fp;
    int tmp = equiv(state);
    int current, next, score;
    if ((fp = fopen("output.txt", "r")) == NULL) {
        printf("cannot read file\n");
        return -1;
    } else {
        while (fscanf(fp, "%d,%d,%d", &current, &next, &score) != EOF) {
            if (current == tmp && score > 0) {
                return equiv(next);
                // if (mode == 1) return equiv(next);
                // if (mode == 0) return -1;
            }
        }
    }
    // if (mode == 0) return 1;
    return -1;
}

void compute_output(int color) {
    // コンピュータの色がcolor
    // bの値を変化させ、動きをprint
    // board b_next = minimax(b, 3, color);
    int cpu = 0, human = 0;
    int state = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (b.state[i][j] == color) {
                state |= ((5 * i + j) << (25 - 5 * cpu));
                cpu++;
            } else if (b.state[i][j] == 3 - color) {
                state |= ((5 * i + j) << (10 - 5 * human));
                human++;
            }
        }
    }
    // if (color == WHITE) state = switch_player(state);
    state = equiv(state);
    int next = search(state, 1);

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
                // (loc/5 loc%5) -> (answer[j][0], answer[j][1])
                int mask = (1 << 15) - 1;
                int tmp_state = state & mask;
                int B[3] = {(state >> 25) & 0b11111, (state >> 20) & 0b11111, (state >> 15) & 0b11111};
                B[i] = answer[j][0] * 5 + answer[j][1];
                tmp_state |= (B[0] << 25) | (B[1] << 20) | (B[2] << 15);
                tmp_state = equiv(tmp_state);
                tmp_state = switch_player(tmp_state);
                int candidate = search(tmp_state, 0);
                if (candidate < 0) {
                    next = switch_player(tmp_state);
                    if (DEBUG == 1) printf("fine\n");
                    isfound = 1;
                    break;
                }
                // if (!is_won(tmp_state, WHITE)) {
                //     next = tmp_state;
                // }
            }
            if (isfound) break;
        }
    } else {
        if (DEBUG == 1) printf("FINE\n");
    }
    // if (color == WHITE) state = switch_player(state);

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
    diff(b, b_next);
    // b = b_next;
}

int main(int argc, char* argv[]) {
    init();
    int cnt = 0;

    if (argc >= 3 && !strcmp(argv[2], "1")) DEBUG = 1;

    if (!strcmp(argv[1], "0")) {
        // 人が先手(黒)
        while (1) {
            print();
            // 人の手番
            get_input(BLACK);
            cnt++;
            print();
            if (is_finished(b)) {
                puts("You Win");
                exit(0);
            }

            // CPUの手番
            compute_output(WHITE);
            cnt++;
            if (is_finished(b)) {
                print();
                puts("You Lose");
                exit(0);
            }

            if (cnt >= 300) {
                print();
                puts("Even");
                exit(0);
            }
        }
    } else {
        // コンピュータが先手(黒)
        while (1) {
            print();
            // CPUの手番
            compute_output(BLACK);
            cnt++;
            print();
            if (is_finished(b)) {
                puts("You Lose");
                exit(0);
            }

            // 人の手番
            get_input(WHITE);
            cnt++;
            if (is_finished(b)) {
                print();
                puts("You Win");
                exit(0);
            }

            if (cnt >= 300) {
                print();
                puts("Even");
                exit(0);
            }
        }
    }

    return 0;
}
