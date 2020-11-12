#include <stdio.h>
#include <stdlib.h>

int b[5][5]; //4Eはb[3][4],2Bはb[1][1],というように…y座標、x座標の順
int answer[9][2];

void init(void) //初期化関数
{
    int x, y;

    for (x = 0; x < 5; x++){ //基本全部空
        for (y = 0; y < 5; y++){
			b[x][y] = 0; 
        }
    }
    b[0][1] = 1; //B1には黒
    b[0][3] = 1; //D1には黒
    b[3][2] = 1; //C4には黒
    b[4][1] = 2; //B5には白
    b[4][3] = 2; //D5には白
    b[1][2] = 2; //C2には白
}

void moveable(int y, int x) //座標を受け取って動ける場所を列挙する関数
{
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
					k++;
				} else {
					break;
				}
			}
        }
    }
//answerには動ける場所全てが格納されている  
}

void move(int color) //石を動かす関数
{
    char input[5];
    int y, x, w, z, i;

    scanf("%s", input); //入力"4E2C"などを受け取って

    y = input[0] - 49; //ASCIIコードを座標に変換
    x = input[1] - 65;
    w = input[2] - 49;
    z = input[3] - 65;

    for (i = 0; i < 9; i++){ //answerの初期化
        answer[i][0] = 100;
        answer[i][1] = 100;
    }

    if (0 <= y && y < 5 && 0 <= x && x < 5 && 0 <= w && w < 5 && 0 <= z && z < 5){ //存在するマスを指定していて
		if (b[y][x] == color) {//元々石が置いてあって
			moveable(y, x);
			for (i = 0; i < 8; i++){
				if (answer[i][0] == w && answer[i][1] == z){ //石が移動できるなら
					b[y][x] = 0; //元いた場所を空にして
					b[w][z] = color; //移動する
				}
			}
			if (b[y][x] == color){ //もし石が移動していなかったら
				puts("You lost!");
			}
		} else { //もし石がなかったら
			puts("You lost!");
		}
	}
}

void finish(void) //終了判定
{
    int i, j, k;
    for (i = 0; i < 3; i++){
		for (j = 0; j < 5; j++){
			for (k = 0; k < 3; k++){
				if (
					(
					b[i][j]==1 &&
					b[i][j]==b[i+1][j] &&
					b[i][j]==b[i+2][j]
					) ||  //縦に一致
					(
					b[j][i]==1 &&
					b[j][i]==b[j][i+1] &&
					b[j][i]==b[j][i+2]
					) ||  //横に一致
					(
					b[i][k]==1 &&
					b[i][k]==b[i+1][k+1] &&
					b[i][k]==b[i+2][k+2]
					) || //斜めに一致
					(
					b[i+2][k]==1 &&
					b[i+2][k]==b[i+1][k+1] &&
					b[i+2][k]==b[i][k+2]
					)){  //斜めに一致
						puts("You won!"); //黒の勝ち
						exit(0);
				}
			}
		}
	}

	for (i = 0; i < 3; i++){
		for (j = 0; j < 5; j++){
			for (k = 0; k < 3; k++){
				if ((b[i][j]==2 && b[i][j]==b[i+1][j] && b[i][j]==b[i+2][j]) ||
				(
				b[j][i]==2 &&
				b[j][i]==b[j][i+1] &&
				b[j][i]==b[j][i+2]
				) ||
				(
				b[i][k]==2 &&
				b[i][k]==b[i+1][k+1] &&
				b[i][k]==b[i+2][k+2]
				) ||
				(
				b[i+2][k]==2 &&
				b[i+2][k]==b[i+1][k+1] &&
				b[i+2][k]==b[i][k+2]
				)){
					puts("You lost!");
					exit(0);
				}
			}
		}
	}
}


int main(void)
{
	init();
	int i,j; 
	move(1);
	for (i=0;i<5;i++)
		for (j=0;j<5;j++)
			printf("%d",b[i][j]);
}
