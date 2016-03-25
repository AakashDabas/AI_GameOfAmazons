#include<iostream>
#include<stdio.h>
#include<climits>
#include<cmath>

#define inRange(x, y) ( (x >= 0) && (x < 10) && (y >= 0) && (y < 10) )
#define loop2(itr1, itr2, lim1, lim2) for(; itr1 < lim1; itr1++)  for(; itr2 < lim2; itr2++)

using namespace std;

long long int cnt;
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
        }finalMove;

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
                        v--;
                        position[v][idx[v]].y = i;
                        position[v][idx[v]].x = j;
                        idx[v]++;
                    }
                }
        }

        float evaluate()
        {
            float finalValue = 0;
            for(int i1=0 ; i1<=1; i1++)
                for(int i2=0; i2<=3; i2++)
                {
                    int pX,pY;
                    pX = position[i1][i2].x;
                    pY = position[i1][i2].y;
                    int cell = 0;
                    for(int i=-1; i<=1; i++)
                        for(int j=-1;j <=1; j++){
                            int xT, yT;
                            xT = pX + i;
                            yT = pY + j;
                            if(inRange(xT, yT)){
                                if(mat[yT][xT] != 0)
                                    cell++;
                            }
                            else    cell++;
                        }
                    if(i1 == 0)     finalValue -= cell*2;
                    else    finalValue += cell*3;
                }
            return finalValue;
        }

        float decideMove(int pCode, int depth){
            float bestPoint = INT_MIN;
            int pCodeTmp = pCode;
            int cntTmp = 0;
            cnt++;

            if(depth == 0)  return evaluate();//Returns leaf node heuristic value

            for(int i=0; i<4; i++){

                int pX = position[pCode-1][i].x;
                int pY = position[pCode-1][i].y;
                moveGenerator moveAmazon;
                moveAmazon.set(pX, pY);

                while(moveAmazon.generate(mat)){  //Iterates all possible moves the selected amazon
                    moveGenerator fire;
                    fire.set(moveAmazon.x, moveAmazon.y);
                    mat[pY][pX] = 0;
                    mat[moveAmazon.y][moveAmazon.x] = pCode;
                    position[pCode-1][i].x=moveAmazon.x;
                    position[pCode-1][i].y=moveAmazon.y;

                    while(fire.generate(mat)){
                        cntTmp++;
                        mat[fire.y][fire.x] = -1;
                        float valTmp = decideMove( pCode%2 + 1, depth - 1);
                        if(valTmp > bestPoint){
                            bestPoint = valTmp;
                            finalMove.set(pX, pY, moveAmazon.x, moveAmazon.y, fire.x, fire.y);
                        }
                        mat[fire.y][fire.x] = 0;
                    }
                    mat[pY][pX] = pCode;
                    mat[moveAmazon.y][moveAmazon.x] = 0;
                    position[pCodeTmp-1][i].x=pX;
                    position[pCodeTmp-1][i].y=pY;
                }
            }
            return bestPoint;
        }
};

int main(){

    int mat[10][10];
    for(int i=0; i<10; i++)
        for(int j=0; j<10; j++)
            cin>> mat[i][j];

    state stBegin;
    stBegin.initialize(mat);
    stBegin.decideMove(1,2);
    cout<<stBegin.finalMove.y1<<" "<<stBegin.finalMove.x1<<endl;
    cout<<stBegin.finalMove.y2<<" "<<stBegin.finalMove.x2<<endl;
    cout<<stBegin.finalMove.f2<<" "<<stBegin.finalMove.f1<<endl;
    //cout<<stBegin.decideMove(1, 1)<<endl;
    //cout<<"\nNo of nodes to be evaluated: "<<cnt;
    return 0;
}
