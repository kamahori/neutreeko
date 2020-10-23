#include <stdio.h>
#include <stdlib.h>

int b[5][5]; //4Eはb[3][4],2Bはb[1][1],というように…y座標、x座標の順
int answer[9][2];

void init(void) //初期化関数
{
  int x,y;

  for (x=0;x<5;x++){ //基本全部空
    for (y=0;y<5;y++){
      b[x][y]=0; 
    }
  b[0][1]=1; //B1には黒
  b[0][3]=1; //D1には黒
  b[3][2]=1; //C4には黒
  b[4][1]=2; //B5には白
  b[4][3]=2; //D5には白
  b[1][2]=2; //C2には白
  }
}

void moveable(int y,int x) //座標を受け取って動ける場所を列挙する関数
{
  int i,j,s,k=0;
  for (i=-1;i<2;i++){
    for(j=-1;j<2;j++){ //全方位探索

      for (s=1;s<5;s++){ //その方位でどこまで行けるか
	
	if (
	    0<=y+j*s &&
	    y+j*s<5 &&
	    0<=x+i*s &&
	    x+i*s<5
	    ){
	  
	  if (b[y+j*s][x+i*s]==0){ //進んだ先のマスが空なら
	    answer[k][0]=y+j*s; //answerのi+j-2（方角のパラメータ）にそのマスを書き込む
	    answer[k][1]=x+i*s;
	  } 
	  else {
	    break;}
	}
	else {
	  break;}
      }
      k++;
    }
  }
 //answerには動ける場所全てが格納されている  
}

void move(int color) //石を動かす関数
{
  char input[5];
  int y,x,w,z,i;

  scanf("%s",input); //入力"4E2C"などを受け取って

  y=input[0]-49; //ASCIIコードを座標に変換
  x=input[1]-65;
  w=input[2]-49;
  z=input[3]-65;

  for (i=0;i<9;i++){ //answerの初期化
    answer[i][0]=100;
    answer[i][1]=100;
  }

  if (0<=y && y<5 && 0<=x && x<5 && 0<=w && w<5 && 0<=z && z<5){ //存在するマスを指定していて
    if (b[y][x]==color) {//元々石が置いてあって
      moveable(y,x);
      for (i=0;i<8;i++){
	if (answer[i][0]==w && answer[i][1]==z){ //石が移動できるなら
	  b[y][x]=0; //元いた場所を空にして
	  b[w][z]=color; //移動する
	}
      }
      if (b[y][x]==color){ //もし石が移動していなかったら
	puts("You lost!");

      }
    }
    else{ //もし石がなかったら
      puts("You lost!");

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
