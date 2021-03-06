#include<iostream>
#include<climits>
#include<cmath>
#include<ctime>
#include<map>
#include<vector>
#include<deque>

#define timeBound   0.95
#define alphaBeta   true
#define cutOff  true
#define inRange(x, y) ( (x >= 0) && (x < 10) && (y >= 0) && (y < 10) )
#define loop2(itr1, itr2, lim1, lim2) for(; itr1 < lim1; itr1++)  for(; itr2 < lim2; itr2++)
#define displayMat(matPr)  for(int itr1=0; itr1<10; itr1++)    {for(int itr2=0; itr2<10; itr2++)    if(matPr[itr1][itr2] == -1)     cout << "x"; else if(matPr[itr1][itr2] == 1) cout << "o";    else if(matPr[itr1][itr2] == 2)    cout << "c"; else   cout << ".";     cout<<endl;}
#define checkMap(mappy, val)    (mappy.find(val)) != mappy.end()

using namespace std;

clock_t clockStart; 
long long int cnt, cnt2, cntStates;
bool contTurn = true;
int level;
int cntMoves;

#define checkTime() ((double)(clock() - clockStart)/ CLOCKS_PER_SEC)

struct dir{
    bool dir1, dir2, dir3, dir4, dir5, dir6, dir7, dir8;
};

class node{
    public:
        map<float, vector<int> > arr;
        map<int, node> treeRecord;
        map<int, bool> stateChk;

        void markNodes(bool order, int n=10)
        {
            if(!order){
                //cout<<"\n----------------------------"<<endl;
                map<float, vector<int> > :: iterator it = arr.begin();
                for(int i=0; it != arr.end(); it++){
                    //if(i < n)
                        //cout<<endl<<it->first<<" : ";
                    if(i < n)
                        while(it->second.empty() == false){
                            //cout<<it->second.back()<<" ";
                            stateChk[it->second.front()] = true;
                            it->second.pop_back();
                            i++;
                        }
                    it->second.clear();
                }
            }
            else if(order){
                //cout<<"\n++++++++++++++++++++++++++++++"<<endl;
                map<float, vector<int> > :: iterator it = arr.end();
                it--;
                for(int i=0; it != arr.begin(); it--){
                    //if(i < n)
                        //cout<<endl<<it->first<<" : ";
                    if(i < n)
                        while(it->second.empty() == false){
                            //cout<<it->second.back()<<" ";
                            stateChk[it->second.back()] = true;
                            it->second.pop_back();
                            i++;
                        }
                    it->second.clear();
                }
            }
            //cout << endl;
        }
        void display(){
            cout<<"\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
            for(int i=0; i<2000; i++)
                if(stateChk[i] == true)
                    cout<<i<<" ";
            cout<<endl;
        }

}dfNode;

class state{
    public:

        //int mat[10][10];
        vector<vector<int> > mat;

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
        }finalMove, moveTmp;

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

                inline bool generate(vector<vector<int> >& mat){
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

        void initialize(vector<vector<int> > mat){
            int idx[2];//To store the index of both bots to be feeded in
            idx[1] = idx[0] = 0;
            this->mat.resize(10, vector<int>(10));
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

        class stPoint{
            public:
                int x, y;
                int flag;
                int cnt;
        };

        class matPoint{
            public:
                int own = -1;
                int steps = 1000;
                int dir[3] = {0, 0, 0};
        };

        int calcConnectedRegion(int x, int y, int own, vector<vector<matPoint> > &matTmp, vector<vector<bool> > &ref){
            int count = 0;
            int f = 1;
            for(int i = -f; i <= f; i++)
                for(int j = -f; j <= f; j++){
                    if(i != 0 || j != 0){
                        if(inRange(y + i, x + j) && matTmp[y + i][x + j].own == own && ref[y + i][x + j] == false){
                            ref[y + i][x + j] = true;
                            count = calcConnectedRegion(x + j, y + i, own, matTmp, ref) + 1;
                        }
                    }
                }
            return count;
        }

        float calcRegion(bool dis = false){
            int cnt_ = 0, cnt__ = 0;
            matPoint matPt__;
            matPt__.own = -1;
            matPt__.steps = 1000;
            vector<vector<matPoint> > matTmp(10, vector<matPoint>(10, matPt__));
            deque<stPoint> buffer;
            for(int i1=0 ; i1<=1; i1++)
                for(int i2=0; i2<=3; i2++)
                {
                    stPoint stTmp;
                    stTmp.x = position[i1][i2].x;
                    stTmp.y = position[i1][i2].y;
                    if(i1 == 0)  stTmp.flag = 0;
                    else    stTmp.flag = 1;
                    stTmp.cnt = 0;
                    buffer.push_back(stTmp);
                }
            while(buffer.empty() == false)
            {
                int pX, pY;
                stPoint stTmp = buffer.front();
                buffer.pop_front();
                pX = stTmp.x;
                pY = stTmp.y;
                int dir = 1;
                int dirX, dirY, x, y, cell = 0;
                x = pX;
                y = pY;
                dirX = dirY = -1;
                if(stTmp.flag <= 1 && matTmp[y][x].own == 2){
                    //cout << "SKIPPED\n";
                    //cout << "OWNED : " << stTmp.flag << endl;
                    continue;//Checks if this cell still belongs to the first element or not;
                }
                /*int cntH = 0, cntV = 0;
                  cout << "OWNED : " << stTmp.flag << endl;
                  cout << "Y : " << y << " X : "<< x<< endl;
                  for(int i = 0; i < 10; i++){
                  for(int j = 0; j < 10; j++){
                  if( i == y && j == x)   cout << "&";
                  else if(matTmp[i][j].own == 0){
                  cntH++;
                  cout << "+";
                  }
                  else if(matTmp[i][j].own == 1){
                  cntV++;
                  cout << "-";
                  }
                  else if(matTmp[i][j].own == 2)    cout << "o";
                  else    cout << ".";
                  cout << " ";
                  }
                  cout << endl;
                  }
                  cout << "H : "<< cntH << "V : "<< cntV;
                  cout << "CNT1: "<<cnt_<<" CNT2: "<<cnt__ << endl;
                  cnt_ = cnt__ = 0;
                  int tmp__;
                  cin >> tmp__;*/
                while(1)
                {
                    cnt_++;
                    if(inRange(dirX + x, dirY + y) && (matTmp[dirY + y][dirX + x].dir[stTmp.flag] & dir) == 0 && mat[dirY + y][dirX + x] == 0){
                        x += dirX;
                        y += dirY;
                    }
                    else{
                        x = pX;
                        y = pY;
                        if(dirX == -1 && dirY == -1)  dirX = 0, dir = 2;
                        else if(dirX == 0 && dirY == -1)  dirX = 1, dir = 4;
                        else if(dirX == 1 && dirY == -1)  dirY = 0, dir = 8;
                        else if(dirX == 1 && dirY == 0)  dirY = 1, dir = 1;
                        else if(dirX == 1 && dirY == 1)  dirX = 0, dir = 2;
                        else if(dirX == 0 && dirY == 1)  dirX = -1, dir = 4;
                        else if(dirX == -1 && dirY == 1)  dirY = 0, dir = 8;
                        else  break;
                        if(inRange(dirX + x, dirY + y) && (matTmp[dirY + y][dirX + x].dir[stTmp.flag] & dir) == 0 && mat[dirY + y][dirX + x] == 0) {
                            x += dirX;
                            y += dirY;
                        }
                        else    continue;
                    }
                    cnt__++;
                    matPoint matPt_ = matTmp[y][x];
                    stPoint stTmp_ = stTmp;
                    stTmp_.x = x;
                    stTmp_.y = y;
                    if(stTmp.flag == 0){
                        stTmp_.cnt++;
                        if(matPt_.own == -1 && mat[y][x] == 0){
                            matPt_.steps = stTmp.cnt + 1;
                            matPt_.own = 0;
                            buffer.push_back(stTmp_);
                        }
                        matPt_.dir[0] |= dir;
                    }
                    else if(stTmp.flag == 1){
                        //cout << matPt_.steps << " " << stTmp.cnt << endl;
                        bool flag_ = false;
                        if(matPt_.steps >= stTmp.cnt + 1 && matPt_.own == 0)
                            matPt_.own = 2;
                        else if(matPt_.own == -1)    matPt_.own = 1, flag_ = true;
                        matPt_.dir[0] |= dir;
                        matPt_.dir[1] |= dir;
                        stTmp_.cnt++;
                        if(flag_){
                            matPt_.steps = stTmp.cnt + 1;
                            buffer.push_back(stTmp_);
                        }
                    }
                    matTmp[y][x] = matPt_;
                }
            }

            /*int cntH = 0, cntV = 0;
              cout << endl;
              for(int i = 0; i < 10; i++){
              for(int j = 0; j < 10; j++){
              if(matTmp[i][j].own == 0){
              cntH++;
              cout << "+";
              }
              else if(matTmp[i][j].own == 1){
              cntV++;
              cout << "-";
              }
              else if(matTmp[i][j].own == 2)   cout << "o";
              else    cout << ".";
              cout << " ";
              }
              cout << endl;
              }*/
            //cout << "H: "<<cntH << "V: "<< cntV<< endl;
            //cout << "CNT1: " << cnt_ << " CNT2: " << cnt__ << endl;

            dis = false;
            int cntH = 0, cntV = 0;
            vector<vector<bool> >ref(10, vector<bool>(10, false));
            int scr[2] = {0, 0};
            for(int i = 0; i < 10; i++){
                for(int j = 0; j < 10; j++){
                    int own = matTmp[i][j].own;
                    if(dis){
                        if(own == 0)    cout << "+";
                        else if(own == 1)   cout << "-";
                        else if(own == 2)   cout << "o";
                        else    cout << ".";
                    }
                    if(own <= 1){
                        if(own == 0)    cntH++;
                        else if(own == 1)   cntV++;
                        ref[i][j] = true;
                        int val = calcConnectedRegion(i, j, own, matTmp, ref) + 1;
                        if(val > scr[own])  scr[own] = val;
                    }
                }
                if(dis) cout << endl;
            }
            if(dis){
                cout << "HERO: " << scr[0] << "  Vellion: " << scr[1] << endl;
                cout << "cntH: " << cntH << " cntV: " << cntV << endl;
            }
            float d1 = scr[0] / (float)(100 - 8 - cntMoves);
            float d2 = scr[1] / (float)(100 - 8 - cntMoves);
            if(dis){
                cout << scr[0] << " " << d1 << " " << 100 - 8 - cntMoves << endl;
                cout << scr[1] << " " << d2 << " " << 100 - 8 - cntMoves << endl;
            }
            return scr[0] * 5 * (d1 > 0.2 ? 1 : -1) - scr[1] * 8 * (d2 > 0.2 ? 1 : -1) + cntH * 2 - cntV * 2 ;
        }

        float evaluate(bool dis = false){
            float finalValue = 0;

            //This part checks for the proximity for all amazons
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
                    else    finalValue += cell*2;
                }

            vector<vector<int> > matRef(10, vector<int>(10, 0));
            //To check the one move direction cells available
            for(int i=0; i<2; i++)
                for(int j=0; j<4; j++)
                {
                    int pX, pY;
                    pX = position[i][j].x;
                    pY = position[i][j].y;
                    int dirX, dirY, x, y, cell = 0;
                    x = pX;
                    y = pY;
                    dirX = dirY = -1;
                    while(1)
                    {
                        if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0 && ((mat[dirY + y][dirX + x] & (i + 1)) == 0)) {
                            x += dirX;
                            y += dirY;
                        }
                        else{
                            x = pX;
                            y = pY;
                            if(dirX == -1 && dirY == -1)  dirX = 0;
                            else if(dirX == 0 && dirY == -1)  dirX = 1;
                            else if(dirX == 1 && dirY == -1)  dirY = 0;
                            else if(dirX == 1 && dirY == 0)  dirY = 1;
                            else if(dirX == 1 && dirY == 1)  dirX = 0;
                            else if(dirX == 0 && dirY == 1)  dirX = -1;
                            else if(dirX == -1 && dirY == 1)  dirY = 0;
                            else  break;
                            if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0 && mat[dirY + y][dirX + x] & (i + 1)) {
                                x += dirX;
                                y += dirY;
                            }
                            else    continue;
                        }
                        cell++;
                        matRef[y][x] |= i + 1;
                    }
                    if(i == 0)  finalValue += cell*3;
                    else    finalValue -= cell*4;
                }
            finalValue += calcRegion(dis);
            return finalValue;
        }

        float decideMove(int pCode, int depth, bool isMaximizer, node &enlist, int alpha = INT_MIN, int beta = INT_MAX, bool topMostLevel = false){

            //evaluate(true);int tmp;cin>>tmp;
            /*int i = 0;
              while(checkTime() < timeBound)
              evaluate(), i++;
              cout << i << endl;
              int tmp;cin>>tmp;*/
            float bestPoint = INT_MIN;
            map<float, bool> mappy;
            if(isMaximizer == false)    bestPoint = INT_MAX;
            int pCodeTmp = pCode;

            if(checkTime() > timeBound)
            {
                contTurn = false;
                if(isMaximizer)     return alpha;
                else    return beta;
            }

            int stateCnt = 0;
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

                        //To check if state is marked or not
                        mat[fire.y][fire.x] = -1;
                        stateCnt++;
                        float val = 0;
                        val = evaluate();
                        if(cutOff){
                            int i = stateCnt;
                            if(depth == 1 ){
                                enlist.arr[val].push_back(stateCnt);
                            }
                            else if(enlist.stateChk[i] == false){
                                mat[fire.y][fire.x] = 0;
                                continue;
                            }
                            cntStates++;
                            if(depth > 1 && 0){
                                cout << val << "< - * -------\n";
                                if(isMaximizer) cout << "++++++++++++++++++++\n";
                                else    cout << "----------------------\n";
                                displayMat(mat);
                                cout << "\n\n";
                                cout << val << endl;
                                //cout << evaluate(true);
                            }
                        }
                        if(checkTime() > timeBound)
                        {
                            contTurn = false;
                            mat[fire.y][fire.x] = 0;
                            mat[pY][pX] = pCode;
                            mat[moveAmazon.y][moveAmazon.x] = 0;
                            position[pCodeTmp-1][i].x=pX;
                            position[pCodeTmp-1][i].y=pY;
                            return (isMaximizer?INT_MIN:INT_MAX);
                        }
                        node enlistNxt = enlist;
                        if(cutOff){
                            if(depth == 2)
                                enlist.treeRecord[stateCnt] = dfNode;
                            if(depth == 1)     enlistNxt = enlist;
                            else    enlistNxt = enlist.treeRecord[stateCnt];
                        }

                        if(isMaximizer)     //Maximizer layer
                        {
                            float valTmp = 0;
                            if(depth != 1)
                                valTmp = decideMove( pCode%2 + 1, depth - 1, (isMaximizer?false:true), enlistNxt, alpha, beta);
                            else    valTmp = val, cnt++;
                            if(valTmp > bestPoint){
                                bestPoint = valTmp;
                                if(topMostLevel)//To register the best move available
                                {
                                    moveTmp.set(pX, pY, moveAmazon.x, moveAmazon.y, fire.x, fire.y);
                                }
                            }
                            if(bestPoint > alpha)  alpha = bestPoint;
                            if(alpha >= beta && alphaBeta)
                            {
                                //cout<<"Beta CutOff;";
                                mat[fire.y][fire.x] = 0;
                                mat[pY][pX] = pCode;
                                mat[moveAmazon.y][moveAmazon.x] = 0;
                                position[pCodeTmp-1][i].x=pX;
                                position[pCodeTmp-1][i].y=pY;
                                return bestPoint;
                            }
                        }
                        if(isMaximizer == false)    //Minimizer layer
                        {
                            float valTmp = 0;
                            if(depth != 1)
                                valTmp = decideMove( pCode%2 + 1, depth - 1, (isMaximizer?false:true), enlistNxt, alpha, beta);
                            else    valTmp = val, cnt++;
                            if(valTmp < bestPoint)  bestPoint = valTmp;
                            if(bestPoint < beta)   beta = bestPoint;
                            if(beta <= alpha && alphaBeta)
                            {
                                //cout<<"Alpha CutOff;";
                                mat[fire.y][fire.x] = 0;
                                mat[pY][pX] = pCode;
                                mat[moveAmazon.y][moveAmazon.x] = 0;
                                position[pCodeTmp-1][i].x=pX;
                                position[pCodeTmp-1][i].y=pY;
                                return bestPoint;
                            }
                        }
                        if(depth > 1)
                            enlist.treeRecord[stateCnt] = enlistNxt;
                        mat[fire.y][fire.x] = 0;
                    }
                    mat[pY][pX] = pCode;
                    mat[moveAmazon.y][moveAmazon.x] = 0;
                    position[pCodeTmp-1][i].x=pX;
                    position[pCodeTmp-1][i].y=pY;
                }
            }
            if(depth == 1 && cutOff)
            {
                enlist.markNodes(isMaximizer?true:false);//, (50)/pow(2, level + 1 - depth));
                //enlist.display();
            }
            return bestPoint;
        }
};

int main(){

    vector<vector<int> > mat(10, vector<int>(10));
    for(int i=0; i<10; i++)
        for(int j=0; j<10; j++){
            cin>> mat[i][j];
            if(mat[i][j] == -1) cntMoves++;
        }

    state stBegin;
    stBegin.initialize(mat);
    int i = 1;
    int cntFinal = 0;
    node enlist;
    clock_t begin = clock();
    for(i = 1; i<3 && checkTime()<timeBound && contTurn; i++)
    {
        stBegin.decideMove(1, i, true, enlist, INT_MIN, INT_MAX, true);
        if(contTurn)
        {
            stBegin.finalMove = stBegin.moveTmp;
            cntFinal = cnt;
            //cout<<"\n i : "<<i<<" time: "<<checkTime()<<" N: "<<cnt<<endl;
            level++;
        }
        cnt = 0;
    }
    if(contTurn == false)   i--;
    //To give the final output
    cout<<stBegin.finalMove.y1<<" "<<stBegin.finalMove.x1<<endl;
    cout<<stBegin.finalMove.y2<<" "<<stBegin.finalMove.x2<<endl;
    cout<<stBegin.finalMove.f2<<" "<<stBegin.finalMove.f1<<endl;
    cout<<"D: "<<level<<" T: "<<checkTime()<<" N:"<<cntFinal<<endl;
    return 0;
}
