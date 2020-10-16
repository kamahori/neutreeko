#include <stdio.h>

struct locations {
  char b1x; //黒玉1のx座標 A B C D E
  char b1y; //黒玉1のy座標 1 2 3 4 5
  char b2x; //黒玉2のx座標 A B C D E
  char b2y; //黒玉2のy座標 1 2 3 4 5
  char b3x; //黒玉3のx座標 A B C D E
  char b3y; //黒玉3のy座標 1 2 3 4 5

  char w1x; //白玉1のx座標 A B C D E
  char w1y; //白玉1のy座標 1 2 3 4 5
  char w2x; //白玉2のx座標 A B C D E
  char w2y; //白玉2のy座標 1 2 3 4 5
  char w3x; //白玉3のx座標 A B C D E
  char w3y; //白玉3のy座標 1 2 3 4 5
};

void blackmove(char *input)
{



int main(int argc) {

  struct locations locations;

  locations.b1x ='B';
  locations.b1y ='1';
  locations.b2x ='D';
  locations.b2y ='1';
  locations.b3x ='C';
  locations.b3y ='4';

  locations.w1x ='B';
  locations.w1y ='5';
  locations.w2x ='D';
  locations.w2y ='5';
  locations.w3x ='C';
  locations.w3y ='2'; //最初の配置

  char input[5];
  scanf("%s",input);
  if (argc==0) //黒が先手

  else if (argc==1){ //白が先手

  else 
    exit(1);

}
