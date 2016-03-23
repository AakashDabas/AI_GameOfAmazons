#include<iostream>

#define inRange(x, y) ( (x >= 0) && (x < 10) && (y >= 0) && (y < 10) )
#define registerMove(m, pCode) (mat[m.x1][m.y1] = 0), (mat[m.x2][m.y2] = pCode), (mat[m.f1][m.f2] = -1)
#define deregisterMove(m, pCode) (mat[m.x1][m.y1] = pCode), (mat[m.x2][m.y2] = 0), (mat[m.f1][m.f2] = 0)

using namespace std;

class state{
  public:

    int mat[10][10];
    
    struct pos{
      int x,y;
    }position[2][4];

    class move{
      public:
        int x1, y1;
        int x2, y2;
        int f1, f2;

        void set(int x1, int y1, int x2, int y2, int f1, int f2){
          this->x1 = x1;
          this->y1 = y1;
          this->x2 = x2;
          this->y2 = y2;
          this->f1 = f1;
          this->f2 = f2;
        }
    };

    class moveGenerator{        //To generate the next possbile moves available
      public:
        int x, y;
        int xOriginal, yOriginal;
        int dirX, dirY;

        void set(int x,int y){
          this->x = x;
          this->y = y;
          xOriginal = x;
          yOriginal = y;
          dirX = dirY = -1;
        }
        bool generate(int mat[10][10]){
          if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0) {
            x += dirX;
            y += dirY;
          }
          else{
            x = xOriginal;
            y = yOriginal;
            if(dirX == -1 && dirY == -1)  dirX = 0;
            else if(dirX == 0 && dirY == -1)  dirX = 1;
            else if(dirX == 1 && dirY == -1)  dirY = 0;
            else if(dirX == 1 && dirY == 0)  dirY = 1;
            else if(dirX == 1 && dirY == 1)  dirX = 0;
            else if(dirX == 0 && dirY == 1)  dirX = -1;
            else if(dirX == -1 && dirY == 1)  dirY = 0;
            else  return false;
            if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0) {
             x += dirX;
             y += dirY;
            }
            else  return generate(mat);
          }
          return true;
        }
    };

    void initialize(int mat[10][10]){
      int idx[2];//To store the index of both bots to be feeded in
      idx[1] = idx[0] = 0;
      for(int i=0; i<10; i++)
        for(int j=0; j<10; j++){
          this->mat[i][j] = mat[i][j];
          int v = mat[i][j];
          if(v != 0 && v != -1)
          {
            position[v][idx[v]].y = i;
            position[v][idx[v]].x = j;
            idx[v]++;
          }
        }
    }

    void searchMove(int pCode){
      for(int i=0; i<4; i++){
        int pX = position[pCode][i].x;
        int pY = position[pCode][i].y;
        moveGenerator moveAmazon;
        moveAmazon.set(pX, pY);
        while(moveAmazon.generate(mat)){

        }
        }
      }
};

int main(){

  int mat[10][10];
  for(int i=0;i<10;i++)
    for(int j=0; j<10;j++)
      //cin>>mat[i][j];
      mat[i][j] = 0;

  state stBegin;
  stBegin.initialize(mat);
  stBegin.searchMove(1);
}
