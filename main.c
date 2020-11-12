#include <stdio.h>
#include <stdlib.h>

int b[5][5]; //4Eはb[3][4],2Bはb[1][1],というように…y座標、x座標の順
int answer[9][2];

int BLACK = 1;
int WHITE = 2;
int DEBUG = 0;

void init(void) { //初期化関数
    int x, y;
    for (x = 0; x < 5; x++){ //基本全部空
        for (y = 0; y < 5; y++){
            b[x][y] = 0; 
        }
    }
    b[0][1] = BLACK; //B1には黒
    b[0][3] = BLACK; //D1には黒
    b[3][2] = BLACK; //C4には黒
    b[4][1] = WHITE; //B5には白
    b[4][3] = WHITE; //D5には白
    b[1][2] = WHITE; //C2には白
}

void print(void) {
    if (DEBUG == 0) return;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d", b[i][j]);
        }
        printf("\n");
    }
}

int is_finished(void) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 3; k++) {
                if (b[i][j] != 0 && b[i][j] == b[i + 1][j] && b[i + 1][j] == b[i + 2][j]) return 1;
                if (b[j][i] != 0 && b[j][i] == b[j][i + 1] && b[j][i + 1] == b[j][i + 2]) return 1;
                if (b[i][k] != 0 && b[i][k] == b[i + 1][k + 1] && b[i + 1][k + 1] == b[i + 2][k + 2]) return 1;
                if (b[i + 2][k] != 0 && b[i + 2][k] == b[i + 1][k + 1] && b[i + 1][k + 1] == b[i][k + 2]) return 1;
            }
        }
    }
    return 0;
}

void moveable(int y, int x) { 
    //座標を受け取って動ける場所を列挙する関数
    //answerに動ける場所全てが格納される 
    int i, j, s, k;
    for (i = -1; i <= 1; i++){
        for (j = -1; j <= 1; j++){ //全方位探索
            for (s = 1; s < 5; s++){ //その方位でどこまで行けるか
    
                if (y + j * s < 0 || y + j * s >= 5 || x + i * s < 0 || x + i * s >= 5){
                    break;
                }
            
                if (b[y + j * s][x + i * s] == 0){ //進んだ先のマスが空なら
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

int is_valid_move(char *input) {
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

    y = input[0] - 49; //ASCIIコードを座標に変換
    x = input[1] - 65;
    w = input[2] - 49;
    z = input[3] - 65;

    if (!is_valid_move(input) || b[y][x] != color) {
        puts("Error");
        exit(1);
    }

    for (i = 0; i < 9; i++){ //answerの初期化
        answer[i][0] = 100;
        answer[i][1] = 100;
    }

    moveable(y, x);

    for (i = 0; i < 9; i++){
        if (answer[i][0] == w && answer[i][1] == z){ //石が移動できるなら
            b[y][x] = 0; //元いた場所を空にして
            b[w][z] = color; //移動する
            return;
        }
    }

    // 移動できない場合
    puts("Error");
    exit(1);
}

void compute_output(int color) {
    // コンピュータの色がcolor
    // bの値を変化させ、動きをprint
}

int main(int argc, char *argv[]) {
    init();
    int cnt = 0;

    if (argv[1] == 0) {
        // 人が先手(黒)
        while (1) {
            // 人の手番
            get_input(BLACK);
            cnt++;
            print();
            if (is_finished()) {
                puts("You Win");
                exit(0);
            }

            // CPUの手番
            compute_output(WHITE);
            cnt++;
            print();
            if (is_finished()) {
                puts("You Lose");
                exit(0);
            }

            if (cnt >= 300) {
                puts("Even");
                exit(0);
            }
        }
    } else {
        // コンピュータが先手(黒)
        while (1) {
            // CPUの手番
            compute_output(BLACK);
            cnt++;
            print();
            if (is_finished()) {
                puts("You Lose");
                exit(0);
            }

            // 人の手番
            get_input(WHITE);
            cnt++;
            print();
            if (is_finished()) {
                puts("You Win");
                exit(0);
            }

            if (cnt >= 300) {
                puts("Even");
                exit(0);
            }
        }
    }

    return 0;
}
