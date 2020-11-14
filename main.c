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
                }
                else {
                    break;
                }
            }
            k++;
        }
    }

}

int is_valid_move(char* input) {
    if (input[0] < '0' || input[0] > '4') return 0;
    if (input[1] < 'A' || input[1] > 'E') return 0;
    if (input[2] < '0' || input[2] > '4') return 0;
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
        puts("Error");
        exit(1);
    }

    moveable(b, y, x);

    for (i = 0; i < 9; i++) {
        if (answer[i][0] == w && answer[i][1] == z) { //石が移動できるなら
            b.state[y][x] = 0; //元いた場所を空にして
            b.state[w][z] = color; //移動する
            return;
        }
    }

    // 移動できない場合
    puts("Error");
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

void diff(board b, board b_next) {
    // b -> b_nextで変化したところを出力する
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
    printf("%s\n", res);
}

void compute_output(int color) {
    // コンピュータの色がcolor
    // bの値を変化させ、動きをprint
    board b_next = minimax(b, 3, color);
    diff(b, b_next);
    b = b_next;
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
    }
    else {
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
