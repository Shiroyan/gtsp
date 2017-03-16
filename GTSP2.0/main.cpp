#include<iostream>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<Windows.h>
#include<vector>
#include<queue>
#include<set>
#include"MyTimer.h"
using namespace std;

const int NODE_LIST_LEN = 1971278 + 1;	// 点个数
const int QUERYSET_LEN = 5;				// K值
const int INFINITY_MAX = 1 << 25;		// 最大值
int querySet[QUERYSET_LEN];				// 查询集合
int S_Q_T[QUERYSET_LEN + 2];			//存放S,T,QuerySet

int f_start = 0;						// 开始点 S
int f_end = 0;							// 结束点 T

struct Node {
	char weight;			//0-127节省空间
	int nodeID;		     //连通点的下标,下标为点的象征
	Node *next;
};
Node nodeList[NODE_LIST_LEN];


void loadGraph(char *filePath) {
	//初始化nodeList
	for (int i = 0; i < NODE_LIST_LEN; i++) {
		nodeList[i].nodeID = -1;
		nodeList[i].weight = -1;
		nodeList[i].next = NULL;
	}
	cout << "Begin to read graph....." << endl;

	std::ios::sync_with_stdio(false); //停止cin与stdin的同步
									  //读图文件
	ifstream is = ifstream(filePath);
	if (!is) {
		cout << "Open file failed" << endl;
		return;
	}

	//开始读入,采用头插入法
	int start, end;
	srand((unsigned int)time(NULL));

	while (!is.eof()) {
		is >> start >> end;

		Node *newNode = new Node();
		newNode->nodeID = end;
		newNode->weight = rand() % 127 + 1;	//产生1-127随机数

											//cout << nodeList[start].next->nodeID << endl;

		newNode->next = nodeList[start].next;
		nodeList[start].next = newNode;

		//cout << nodeList[start].next->nodeID << endl;
	}
	//读取完毕
	is.close();
	cout << endl << "Read complete...." << endl;
}


//------------------------------Dijkstra寻找最短路径--------------------------------
struct Vertex
{
	int id;						//当前点的位置
	int d;						//s - > v的最短距离
	int piror;					// 点的前驱，用于之后回溯查找最短路径
};
struct VertexCmp {
	bool operator ()(const Vertex &a, const Vertex &b)const {
		if (a.d == b.d) {
			return a.id < b.id;
		}
		return a.d < b.d;
	}
};

vector<int> S;						//已找到最短路径的集合

Vertex v[NODE_LIST_LEN];			// v
set<Vertex, VertexCmp> vertexSet;	//Q

int extractMin() {					//在点集中寻找 最小最短路径估计值
	Vertex temp = *vertexSet.begin();
	vertexSet.erase(temp);

	return temp.id;
}

bool isInQuerySet(int nodeID) {			//判断是否属于查询集合Q，且非终点
	
	if (nodeID == S_Q_T[0] && f_start != S_Q_T[0]) {								//S只能作为起点
		return true;
	}
	if (nodeID == S_Q_T[QUERYSET_LEN + 1] && f_end != S_Q_T[QUERYSET_LEN + 1]) {	//T只能作为终点
		return true;
	}
	for (int i = 0; i < QUERYSET_LEN; i++) {
		if (nodeID == querySet[i] && nodeID != f_end)
			return true;
	}

	return false;
}

void relax(int u) {				//“松弛操作”- 更新最短路径
	Node *temp = nodeList[u].next;
	// foreach v ∈ adj[u]
	while (temp) {
		int index = temp->nodeID;

		//在寻找最短路径的途中，排除查询集合 QuerySet - {f_end} 中的元素，确保最短路径中不出现其余查询集合中的点
		//存在，则跳过这个点。
		if (isInQuerySet(index)) {
			temp = temp->next;
			continue;
		}

		if (v[index].d > v[u].d + temp->weight) {
			vertexSet.erase(v[index]);
			v[index].d = v[u].d + temp->weight;
			v[index].piror = u;
			vertexSet.insert(v[index]);
		}
		temp = temp->next;
	}
}
void Dijkstra() {
	int i, j;
	
	vertexSet.clear();
	//初始化，S,V，Q
	for (i = 0; i < NODE_LIST_LEN ; i++) {	//V init
		if (i == f_start) continue;

		v[i].d = INFINITY_MAX;
		v[i].piror = -1;
		v[i].id = i;
		vertexSet.insert(v[i]);
	}
	v[f_start].d = 0;
	v[f_start].id = f_start;
	vertexSet.insert(v[f_start]);

	S.clear();								//S init


	//loop
	int u;
	while (!vertexSet.empty()) {
		
		u = extractMin();		//从Q（V - S）中找“最轻，最近”的点加入到S中 
		
		S.push_back(u);
		relax(u);

		if (u == f_end) return;
	}
}
//---------------------------------------------------------------------------------------------

//---------------------------从前驱回溯找到最短路径---------------------------------------
int QsetAppearCount[QUERYSET_LEN] = { 0 };
int SAppearCount = 0;
int TAppearCount = 0;

void showPath() {
	int *minPath = new int[NODE_LIST_LEN];

	int i = 0;
	//cout << endl << "MinPath: ";
	while (1) {
		minPath[i++] = f_end;
		f_end = v[f_end].piror;

		if (f_end == f_start) {
			minPath[i++] = f_start;
			break;
		}
	}

	i--;
	//cout << minPath[i--];
	for (int j = i; j >= 0; j--) {
		//cout << " -> " << minPath[j];

		//记录S,T,Q出现次数
		if (minPath[j] == S_Q_T[0]) {
			SAppearCount++;
		}
		if (minPath[j] == S_Q_T[QUERYSET_LEN + 1]) {
			TAppearCount++;
		}
		else {
			for (int k = 0; k < QUERYSET_LEN; k++) {
				if (minPath[j] == querySet[k]) {
					QsetAppearCount[k]++;
				}
			}
		}
	}
	//cout << endl;
}
void showCount() {
	cout << "S appear: " << SAppearCount << endl;
	cout << "T appear: " << TAppearCount << endl;
	for (int i = 0; i < QUERYSET_LEN; i++) {
		cout << "Q" << (i + 1) << " appear: " << QsetAppearCount[i] << endl;
	}
}
//-------------------------------------	Dijkstra ---------------------------------------------------







//-------------------------------------动态规划函数部分------------------------------------

int getOnesNum(int j) {
	int count = 0;
	while (j) {
		count++;
		j = j >> 1;
	}
	return count;
}
int getOnesPosition(int *binaryJ) {			//找到1的位置，这个函数只会在‘1’的数目为1时调用
	int count = 1;
	for (int i = QUERYSET_LEN - 1; i >= 0; i--,count++) {
		if (binaryJ[i] == 1) break;
	}
	return count;
}
void integer2Binary(int *binaryJ,int j) {
	int i;
	int one = 1,flag;
	//初始化置0
	for (i = 0; i < QUERYSET_LEN; i++) {
		binaryJ[i] = 0;
	}
	//
	for (i = QUERYSET_LEN - 1; i >= 0; i--) {
		flag = 1 & j;	// 1 按位与 j 检测最低位是否为1
		if (flag) {		//flag == 1  <==> 最低位为1
			binaryJ[i] = 1;
		}
		j = j >> 1;
	}
}
int binary2Integer(int *binaryJ) {
	int count = 0, sum = 0;
	
	for (int i = QUERYSET_LEN - 1; i >= 0; i--,count++) {
		if (binaryJ[i]) {
			sum += (1 << count);
		}
	}
	return sum;
}
int getLowest1Pos(int *b) {
	return getOnesPosition(b);
}
int getHighest1Pos(int *b) {
	for (int i = 0; i < QUERYSET_LEN; i++) {
		if (b[i])
			return QUERYSET_LEN - i;
	}

}

//-----------------------------------------------------------------------------------------
int main() {
	MyTimer myTimer;		//计时器
	cout << "当前规模: " << QUERYSET_LEN << endl;
	//------------------------------------读取图文件----------------------------------------

	myTimer.timerStart();
	loadGraph("roadNet.txt");
	myTimer.timerStop();
	cout << endl << "Reading graph costs time: " << myTimer.getTime() << " s" << endl;

	//--------------------------------------------------------------------------------------

	//-----------------------初始化查询集合Q，随机生成S,T ----------------------------------

	f_start = rand() % NODE_LIST_LEN;
	f_end = rand() % NODE_LIST_LEN;
	
	cout << endl << "开始点: " << f_start << "  终结点: " << f_end << endl;
	cout << "QuerySet（查询集合Q）: ";
	/*for (int i = 0; i < QUERYSET_LEN; i++) {
		cout<<querySet[i]<<' ';
	}*/

	for (int i = 0; i < QUERYSET_LEN; i++) {
		querySet[i] = rand() % NODE_LIST_LEN;
		cout << querySet[i] << ' ';
	}

	cout << endl << endl;

	//-------------------------------------------------------------------------------------

	//-----------------------------生成S,querySet，T 距离表/邻接矩阵--------------------------------
	int payMatrix[QUERYSET_LEN + 2][QUERYSET_LEN + 2];		// 0: S  K+1: T
	for (int i = 0; i < QUERYSET_LEN + 2; i++) {			//初始化，全填无穷
		for (int j = 0; j < QUERYSET_LEN + 2; j++) {
			payMatrix[i][j] = INFINITY_MAX;
		}
	}

	
	S_Q_T[0] = f_start; S_Q_T[QUERYSET_LEN + 1] = f_end;
	for (int i = 1, j = 0; i < QUERYSET_LEN + 1 && j < QUERYSET_LEN; i++, j++) {
		S_Q_T[i] = querySet[j];
	}

	myTimer.timerStart();
	cout << endl << "Begin to search the path......." << endl;
	for (int i = 0; i < QUERYSET_LEN + 1; i++) {
		f_start = S_Q_T[i];
		for (int j = 1; j < QUERYSET_LEN + 2; j++) {
			if (i == j || (i==0 && j == QUERYSET_LEN+1)/*S,T*/) 
				continue;

			f_end = S_Q_T[j];
			//cout << endl << "Start: " << f_start << " End: " << f_end;
			Dijkstra();
			payMatrix[i][j] = v[f_end].d;				//v.d记录的是 s->v的最短路径
			showPath();
		}
	}
	showCount();
	myTimer.timerStop();
	cout << endl << "建立邻接矩阵耗时: " << myTimer.getTime() << " s" << endl << endl;
	/*cout << endl << "Minpay: " << v[f_end].d << endl;
	showPath();*/
	
	////payMatrix固定值，测试用.....
	//payMatrix[0][0] = INFINITY_MAX; payMatrix[0][1] = 4380; payMatrix[0][2] = 737;
	//payMatrix[0][3] = 1541; payMatrix[0][4] = INFINITY_MAX;
	//payMatrix[1][0] = INFINITY_MAX; payMatrix[1][1] = INFINITY_MAX; payMatrix[1][2] = 4414;
	//payMatrix[1][3] = 3705; payMatrix[1][4] = 2720;
	//payMatrix[2][0] = INFINITY_MAX; payMatrix[2][1] = 4067; payMatrix[2][2] = INFINITY_MAX;
	//payMatrix[2][3] = 1228; payMatrix[2][4] = 1906;
	//payMatrix[3][0] = INFINITY_MAX; payMatrix[3][1] = 3591; payMatrix[3][2] = 1217;
	//payMatrix[3][3] = INFINITY_MAX; payMatrix[3][4] = 1454;

	//show payMatrix
	for (int i = 0; i < QUERYSET_LEN + 2; i++) {
		for (int j = 0; j < QUERYSET_LEN + 2; j++) {
			if (payMatrix[i][j] == INFINITY_MAX)
				cout << "INF ";
			else
				cout << payMatrix[i][j] << ' ';
		}
		cout << endl;
	}

//-------------------------------动态规划填表--------------------------------
	myTimer.timerStart();
	int **table,**tmpPath;
	int yLength = (int)pow(2, QUERYSET_LEN);	// 2 ^ k， 0 ~  2^k -1
	int xLength = QUERYSET_LEN + 1;				// k + 1

	int *binaryJ = new int[QUERYSET_LEN];		//j的二进制形式，k位,按照正常的顺序存储，高位在前
	table = new int *[xLength];
	tmpPath = new int *[xLength];
	
	for (int i = 0; i < xLength; i++) {
		table[i] = new int[yLength];
		tmpPath[i] = new int[yLength];
	}												
	for (int i = 0; i < xLength; i++) {				//初始化
		for (int j = 0; j < yLength; j++) {
			table[i][j] = tmpPath[i][j] = 0;
		}
	}

													//生成 （k+1） * 2^k 的表
													//以及 (k+1) * 2^k 的int数组保存中途的路径

	int onesNum, onesPosition;
	int k = pow(2, QUERYSET_LEN);					// k = 2^k

	for (int j = 0; j < yLength; j++) {				//按列填表
		onesNum = getOnesNum(j);
		integer2Binary(binaryJ,j);

		for (int i = 0; i < xLength; i++) {
			
			if (j == 0) {							//填入第一列
				table[i][j] = payMatrix[i][xLength];		
			}
			// onesNum == 1
			else if (onesNum == 1 || (i==0 && onesNum == 1)) {
				onesPosition = getOnesPosition(binaryJ);
				if (onesPosition == i);				//若‘1’的位置与行i相同，说明该位置不放置
				else {
					table[i][j] = payMatrix[i][onesPosition] + table[onesPosition][0];
					tmpPath[i][j] = onesPosition * k + 0;
				}
			}
			//onesNum > 1
			else if(onesNum > 1 && binaryJ[QUERYSET_LEN - i] != 1){
				int group = onesNum;				//‘1’的个数一共有多少个，就一共有多少组解
				int minPay = INFINITY_MAX;
				int h1Position = getHighest1Pos(binaryJ);
				int l1Position = 0;

				int *symbol = new int[QUERYSET_LEN];	//对symbol的01进行更改，每次获得的最低位‘1’的位置都不同
				memcpy(symbol, binaryJ, sizeof(int)*QUERYSET_LEN);
				int *g1 = new int[QUERYSET_LEN];	// "只有1个‘1’的"
				int *g2 = new int[QUERYSET_LEN];	// "剩余的"

				do {
					l1Position = getLowest1Pos(symbol);
					int i1, count;
					for (i1 = QUERYSET_LEN - 1, count = 0; i1 >= 0; i1--, count++) {
						if (count == l1Position - 1) {
							g1[i1] = 1;
							g2[i1] = 0;
						}
						else {
							g1[i1] = 0;
							g2[i1] = binaryJ[i1];
						}
					}
					symbol[(QUERYSET_LEN - l1Position)] = 0;

					int onePos = getOnesPosition(g1);	//获取1的位置
					int n = binary2Integer(g2);			//获取十进制
					
					if (minPay > payMatrix[i][onePos] + table[onePos][n]) {
						minPay = payMatrix[i][onePos] + table[onePos][n];
						tmpPath[i][j] = onePos * k + n;
					}
				

					/*cout << "Symbol: ";
					for (i = 0; i < QUERYSET_LEN; i++) {
					cout << symbol[i];
					}
					cout << endl;
					cout << "G1: ";
					for (i = 0; i < QUERYSET_LEN; i++) {
					cout << g1[i];
					}
					cout << "     G2: ";
					for (i = 0; i < QUERYSET_LEN; i++) {
					cout << g2[i];
					}
					cout << endl << endl;*/

				} while (l1Position != h1Position);
				
				table[i][j] = minPay;

				delete[]g1;
				delete[]g2;
				delete[]symbol;
			}
		}
	}


	int i = 0, j = k - 1;
	int index = 0;
	int *minPath = new int[QUERYSET_LEN];
	while (tmpPath[i][j] != 0) {

		int i1 = tmpPath[i][j] / k;
		int j1 = tmpPath[i][j] % k;
		
		i = i1; j = j1;
		minPath[index++] = i;

	}
	//输出距离表
	/*cout << endl << "距离表：" << endl;
	for (int i = 0; i < xLength; i++) {
		for (int j = 0; j < yLength; j++) {
			cout << tmpPath[i][j] << ' ';
		}
		cout << endl;
	}*/

	//输出动态表
	/*cout << endl << "动态表：" << endl;
	for (int i = 0; i < xLength; i++) {
		for (int j = 0; j < yLength; j++) {
			if (table[i][j] == 0 || table[i][j] == INFINITY_MAX) {
				cout << "     ";
			}
			else {
				cout << table[i][j] << ' ';
			}
		}
		cout << endl;
	}*/
	
	//输出路径
	cout << endl << "最短路径： S -> ";
	for (int i = 0; i < QUERYSET_LEN; i++) {
		cout << "Q" << minPath[i] << " -> ";
	}
	cout << "T" << endl;
	
	//输出最短路径开销
	cout <<endl<<"最短路径开销: "<< table[0][k - 1] << endl;
	myTimer.timerStop();
	
	cout << endl << "规划耗时： " << myTimer.getTime() << " s" << endl;

//---------------------------------------------------------------------------
	/*cout << "Graph: ";
	for (int i = 0; i <= 6; i++) {
		Node *temp = nodeList[i].next;
		cout << endl << i << " : ";
		while (temp) {
			cout << temp->nodeID << "(" << (int)temp->weight << "), ";
			temp = temp->next;
		}
	}*/
	int c;
	cin >> c;

}