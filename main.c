#include <stdio.h>

enum COLOR {black,white,empty};

enum COLOR b[5][5]; //4Eはb[3][4],2Bはb[1][1],というように…y座標、x座標の順
enum COLOR color;

void init(void) //初期化関数
{
  int x,y;

  for (x=0;x<5;x++) //基本全部空
    for (y=0;y<5;y++)
      b[x][y]=empty; 
  b[0][1]=black; //B1には黒
  b[0][3]=black; //D1には黒
  b[3][2]=black; //C4には黒
  b[4][1]=white; //B5には白
  b[4][3]=white; //D5には白
  b[1][2]=white; //C2には白
}

int *moveable(int y,int x) //座標を受け取って動ける場所を列挙する関数
{
  int i,j,s,answer[8][2]={100};
  for (i=-1;i<2;i++)
    for(j=-1;j<2;j++){ //全方位探索
      
      if (i==0 && j==0) //方向がないときは考えない
	continue;

      for (s=1;s<5;s++){ //その方位でどこまで行けるか
	
	if (
	    0<=y+j*s &&
	    y+j*s<5 &&
	    0<=x+j*s &&
	    x+j*s<5
	    ){
	  
	  if (b[y+j*s][x+j*s]==empty){ //進んだ先のマスが空なら
	    answer[i+j-2][0]=y+j*s; //answerのi+j-2（方角のパラメータ）にそのマスを書き込む
	    answer[i+j-2][1]=x+j*s;
	  }
	}
      }
    }
  return answer; //answerには動ける場所全てが格納されている、進めない方位には100が入っている  
}

void move(enum COLOR color) //石を動かす関数
{
  char input[5];
  int y,x,w,z,i;

  scanf("%s",input); //入力"4E2C"などを受け取って

  y=input[0]-49; //ASCIIコードを座標に変換
  x=input[1]-65;
  w=input[2]-49;
  z=input[3]-65;

  if (0<=y && y<5 && 0<=x && x<5 && 0<=w && w<5 && 0<=z && z<5) //存在するマスを指定していて
    if (b[y][x]==color) {//元々石が置いてあって
      for (i=0;i<8;i++)
	if (movebable(y,x)[i][0]==w && moveable(y,x)[i][1]==z){ //石が移動できるなら
	  b[y][x]=empty; //元いた場所を空にして
	  b[w][z]=color; //移動する
	}
	else
	  ;
      if (b[y][x]==color) //もし石が移動していなかったら
	puts("You lost!");
	exit(1);
    }
    else　//元々石が置いてなかったら
      puts("You lost!");
      exit(1); 
}
