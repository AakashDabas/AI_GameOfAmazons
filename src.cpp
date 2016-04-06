#include<iostream>
#include<climits>
#include<cmath>
#include<ctime>
#include<map>
#include<vector>

#define timeBound   1000.98
#define alphaBeta   true
#define cutOff  true
#define inRange(x, y) ( (x >= 0) && (x < 10) && (y >= 0) && (y < 10) )
#define loop2(itr1, itr2, lim1, lim2) for(; itr1 < lim1; itr1++)  for(; itr2 < lim2; itr2++)
#define displayMat()  for(int itr1=0; itr1<10; itr1++)    {for(int itr2=0; itr2<10; itr2++)    cout<<mat[itr1][itr2]<<" ";     cout<<endl;}
#define checkMap(mappy, val)    (mappy.find(val)) != mappy.end()

using namespace std;

clock_t clockStart; 
long long int cnt, cnt2;
bool contTurn = true;
int level;
long long int cntStates;

#define checkTime() ((double)(clock() - clockStart)/ CLOCKS_PER_SEC)

class node{
    public:
        map<float, vector<int> > arr;
        map<int, node> treeRecord;
        map<int, bool> stateChk;

        void markNodes(bool order, int n=20)
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
                bool generate(vector<vector<int> > mat){
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

        float evaluate()
        {
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
                    if(i1 == 0)     finalValue -= cell*8;
                    else    finalValue += cell*12;
                }

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
                        if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0) {
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
                            if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0) {
                                x += dirX;
                                y += dirY;
                            }
                            else    continue;
                        }
                        cell++;
                    }
                    if(i == 0)  finalValue += cell*10;
                    else    finalValue -= cell*8;
                }
            //This is to check for directional moves available
            int cell = 0;
            for(int i=0; i<2 && 0; i++){
                for(int j=0; j<4; j++)
                {
                    int pX, pY;
                    pX = position[i][j].x;
                    pY = position[i][j].y;
                    int dirX, dirY, x, y;
                    x = pX;
                    y = pY;
                    dirX = dirY = -1;
                    while(1)
                    {
                        if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0) {
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
                            if(inRange(dirX + x, dirY + y) && mat[dirY + y][dirX + x] == 0) {
                                x += dirX;
                                y += dirY;
                            }
                        }
                        int x2, y2;
                        int dirX2 = -1, dirY2 = -1;
                        x2 = x;
                        y2 = y;
                        while(1)
                        {
                            if(inRange(dirX2 + x2, dirY2 + y2) && mat[dirY2 + y2][dirX2 + x2] == 0) {
                                x2 += dirX2;
                                y2 += dirY2;
                            }
                            else{
                                x2 = x;
                                y2 = y;
                                if(dirX2 == -1 && dirY2 == -1)  dirX2 = 0;
                                else if(dirX2 == 0 && dirY2 == -1)  dirX2 = 1;
                                else if(dirX2 == 1 && dirY2 == -1)  dirY2 = 0;
                                else if(dirX2 == 1 && dirY2 == 0)  dirY2 = 1;
                                else if(dirX2 == 1 && dirY2 == 1)  dirX2 = 0;
                                else if(dirX2 == 0 && dirY2 == 1)  dirX2 = -1;
                                else if(dirX2 == -1 && dirY2 == 1)  dirY2 = 0;
                                else  break;
                                if(inRange(dirX2 + x2, dirY2 + y2) && mat[dirY2 + y2][dirX2 + x2] == 0) {
                                    x2 += dirX2;
                                    y2 += dirY2;
                                }
                            }
                            if(mat[y2][x2] == 0)
                            {
                                cell++;
                                mat[y2][x2] = -2;
                            }
                        }
                    }
                }
                if(i == 0)  finalValue += cell*8;
                else    finalValue -= cell*12;
                cell = 0;
                for(int i=0; i<10; i++)
                    for(int j=0; j<10; j++)
                        if(mat[i][j] == -2)
                            mat[i][j] = 0;
            }

            return finalValue;
        }

        float decideMove(int pCode, int depth, bool isMaximizer, node &enlist, int alpha = INT_MIN, int beta = INT_MAX, bool topMostLevel = false){

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

            if(depth == 0){
                ++cnt;
                return evaluate();//Returns leaf node heuristic value
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
                        if(cutOff){
                            int i = stateCnt;
                            if(depth == 1 ){
                                float val = evaluate();
                                enlist.arr[val].push_back(stateCnt);
                            }
                            else if(enlist.stateChk[i] == false){
                                mat[fire.y][fire.x] = 0;
                                continue;
                            }
                            cntStates++;
                            /*if(depth == 2 ){
                                cout<<depth << " * "<< stateCnt;
                                cout<<" $ "<<enlist.stateChk.count(stateCnt) <<endl;
                            }*/
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
                            float valTmp = decideMove( pCode%2 + 1, depth - 1, (isMaximizer?false:true), enlistNxt, alpha, beta);
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
                            float valTmp = decideMove( pCode%2 + 1, depth - 1, (isMaximizer?false:true), enlistNxt, alpha, beta);
                            if(valTmp < bestPoint)  bestPoint = valTmp;
                            if(bestPoint < beta)   beta = bestPoint;
                            if(beta <= alpha && alphaBeta)
                            {
                                //cout<<"Alpha CutOff V;";
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

    clock_t begin = clock();
    vector<vector<int> > mat(10, vector<int>(10));
    for(int i=0; i<10; i++)
        for(int j=0; j<10; j++)
            cin>> mat[i][j];

    state stBegin;
    stBegin.initialize(mat);
    int i = 1;
    int cntFinal = 0;
    node enlist;
    for(i = 1; i<4 && checkTime()<timeBound && contTurn; i++)
    {
        stBegin.decideMove(1, i, true, enlist, INT_MIN, INT_MAX, true);
        if(contTurn)
        {
            stBegin.finalMove = stBegin.moveTmp;
            level++;
            cntFinal = cnt;
            cout<<"\n i : "<<i<<" time: "<<checkTime()<<" N: "<<cnt<<endl;
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
