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

const int NODE_LIST_LEN = 1971278 + 1;	// �����
const int QUERYSET_LEN = 5;				// Kֵ
const int INFINITY_MAX = 1 << 25;		// ���ֵ
int querySet[QUERYSET_LEN];				// ��ѯ����
int S_Q_T[QUERYSET_LEN + 2];			//���S,T,QuerySet

int f_start = 0;						// ��ʼ�� S
int f_end = 0;							// ������ T

struct Node {
	char weight;			//0-127��ʡ�ռ�
	int nodeID;		     //��ͨ����±�,�±�Ϊ�������
	Node *next;
};
Node nodeList[NODE_LIST_LEN];


void loadGraph(char *filePath) {
	//��ʼ��nodeList
	for (int i = 0; i < NODE_LIST_LEN; i++) {
		nodeList[i].nodeID = -1;
		nodeList[i].weight = -1;
		nodeList[i].next = NULL;
	}
	cout << "Begin to read graph....." << endl;

	std::ios::sync_with_stdio(false); //ֹͣcin��stdin��ͬ��
									  //��ͼ�ļ�
	ifstream is = ifstream(filePath);
	if (!is) {
		cout << "Open file failed" << endl;
		return;
	}

	//��ʼ����,����ͷ���뷨
	int start, end;
	srand((unsigned int)time(NULL));

	while (!is.eof()) {
		is >> start >> end;

		Node *newNode = new Node();
		newNode->nodeID = end;
		newNode->weight = rand() % 127 + 1;	//����1-127�����

											//cout << nodeList[start].next->nodeID << endl;

		newNode->next = nodeList[start].next;
		nodeList[start].next = newNode;

		//cout << nodeList[start].next->nodeID << endl;
	}
	//��ȡ���
	is.close();
	cout << endl << "Read complete...." << endl;
}


//------------------------------DijkstraѰ�����·��--------------------------------
struct Vertex
{
	int id;						//��ǰ���λ��
	int d;						//s - > v����̾���
	int piror;					// ���ǰ��������֮����ݲ������·��
};
struct VertexCmp {
	bool operator ()(const Vertex &a, const Vertex &b)const {
		if (a.d == b.d) {
			return a.id < b.id;
		}
		return a.d < b.d;
	}
};

vector<int> S;						//���ҵ����·���ļ���

Vertex v[NODE_LIST_LEN];			// v
set<Vertex, VertexCmp> vertexSet;	//Q

int extractMin() {					//�ڵ㼯��Ѱ�� ��С���·������ֵ
	Vertex temp = *vertexSet.begin();
	vertexSet.erase(temp);

	return temp.id;
}

bool isInQuerySet(int nodeID) {			//�ж��Ƿ����ڲ�ѯ����Q���ҷ��յ�
	
	if (nodeID == S_Q_T[0] && f_start != S_Q_T[0]) {								//Sֻ����Ϊ���
		return true;
	}
	if (nodeID == S_Q_T[QUERYSET_LEN + 1] && f_end != S_Q_T[QUERYSET_LEN + 1]) {	//Tֻ����Ϊ�յ�
		return true;
	}
	for (int i = 0; i < QUERYSET_LEN; i++) {
		if (nodeID == querySet[i] && nodeID != f_end)
			return true;
	}

	return false;
}

void relax(int u) {				//���ɳڲ�����- �������·��
	Node *temp = nodeList[u].next;
	// foreach v �� adj[u]
	while (temp) {
		int index = temp->nodeID;

		//��Ѱ�����·����;�У��ų���ѯ���� QuerySet - {f_end} �е�Ԫ�أ�ȷ�����·���в����������ѯ�����еĵ�
		//���ڣ�����������㡣
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
	//��ʼ����S,V��Q
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
		
		u = extractMin();		//��Q��V - S�����ҡ����ᣬ������ĵ���뵽S�� 
		
		S.push_back(u);
		relax(u);

		if (u == f_end) return;
	}
}
//---------------------------------------------------------------------------------------------

//---------------------------��ǰ�������ҵ����·��---------------------------------------
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

		//��¼S,T,Q���ִ���
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







//-------------------------------------��̬�滮��������------------------------------------

int getOnesNum(int j) {
	int count = 0;
	while (j) {
		count++;
		j = j >> 1;
	}
	return count;
}
int getOnesPosition(int *binaryJ) {			//�ҵ�1��λ�ã��������ֻ���ڡ�1������ĿΪ1ʱ����
	int count = 1;
	for (int i = QUERYSET_LEN - 1; i >= 0; i--,count++) {
		if (binaryJ[i] == 1) break;
	}
	return count;
}
void integer2Binary(int *binaryJ,int j) {
	int i;
	int one = 1,flag;
	//��ʼ����0
	for (i = 0; i < QUERYSET_LEN; i++) {
		binaryJ[i] = 0;
	}
	//
	for (i = QUERYSET_LEN - 1; i >= 0; i--) {
		flag = 1 & j;	// 1 ��λ�� j ������λ�Ƿ�Ϊ1
		if (flag) {		//flag == 1  <==> ���λΪ1
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
	MyTimer myTimer;		//��ʱ��
	cout << "��ǰ��ģ: " << QUERYSET_LEN << endl;
	//------------------------------------��ȡͼ�ļ�----------------------------------------

	myTimer.timerStart();
	loadGraph("roadNet.txt");
	myTimer.timerStop();
	cout << endl << "Reading graph costs time: " << myTimer.getTime() << " s" << endl;

	//--------------------------------------------------------------------------------------

	//-----------------------��ʼ����ѯ����Q���������S,T ----------------------------------

	f_start = rand() % NODE_LIST_LEN;
	f_end = rand() % NODE_LIST_LEN;
	
	cout << endl << "��ʼ��: " << f_start << "  �ս��: " << f_end << endl;
	cout << "QuerySet����ѯ����Q��: ";
	/*for (int i = 0; i < QUERYSET_LEN; i++) {
		cout<<querySet[i]<<' ';
	}*/

	for (int i = 0; i < QUERYSET_LEN; i++) {
		querySet[i] = rand() % NODE_LIST_LEN;
		cout << querySet[i] << ' ';
	}

	cout << endl << endl;

	//-------------------------------------------------------------------------------------

	//-----------------------------����S,querySet��T �����/�ڽӾ���--------------------------------
	int payMatrix[QUERYSET_LEN + 2][QUERYSET_LEN + 2];		// 0: S  K+1: T
	for (int i = 0; i < QUERYSET_LEN + 2; i++) {			//��ʼ����ȫ������
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
			payMatrix[i][j] = v[f_end].d;				//v.d��¼���� s->v�����·��
			showPath();
		}
	}
	showCount();
	myTimer.timerStop();
	cout << endl << "�����ڽӾ����ʱ: " << myTimer.getTime() << " s" << endl << endl;
	/*cout << endl << "Minpay: " << v[f_end].d << endl;
	showPath();*/
	
	////payMatrix�̶�ֵ��������.....
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

//-------------------------------��̬�滮���--------------------------------
	myTimer.timerStart();
	int **table,**tmpPath;
	int yLength = (int)pow(2, QUERYSET_LEN);	// 2 ^ k�� 0 ~  2^k -1
	int xLength = QUERYSET_LEN + 1;				// k + 1

	int *binaryJ = new int[QUERYSET_LEN];		//j�Ķ�������ʽ��kλ,����������˳��洢����λ��ǰ
	table = new int *[xLength];
	tmpPath = new int *[xLength];
	
	for (int i = 0; i < xLength; i++) {
		table[i] = new int[yLength];
		tmpPath[i] = new int[yLength];
	}												
	for (int i = 0; i < xLength; i++) {				//��ʼ��
		for (int j = 0; j < yLength; j++) {
			table[i][j] = tmpPath[i][j] = 0;
		}
	}

													//���� ��k+1�� * 2^k �ı�
													//�Լ� (k+1) * 2^k ��int���鱣����;��·��

	int onesNum, onesPosition;
	int k = pow(2, QUERYSET_LEN);					// k = 2^k

	for (int j = 0; j < yLength; j++) {				//�������
		onesNum = getOnesNum(j);
		integer2Binary(binaryJ,j);

		for (int i = 0; i < xLength; i++) {
			
			if (j == 0) {							//�����һ��
				table[i][j] = payMatrix[i][xLength];		
			}
			// onesNum == 1
			else if (onesNum == 1 || (i==0 && onesNum == 1)) {
				onesPosition = getOnesPosition(binaryJ);
				if (onesPosition == i);				//����1����λ������i��ͬ��˵����λ�ò�����
				else {
					table[i][j] = payMatrix[i][onesPosition] + table[onesPosition][0];
					tmpPath[i][j] = onesPosition * k + 0;
				}
			}
			//onesNum > 1
			else if(onesNum > 1 && binaryJ[QUERYSET_LEN - i] != 1){
				int group = onesNum;				//��1���ĸ���һ���ж��ٸ�����һ���ж������
				int minPay = INFINITY_MAX;
				int h1Position = getHighest1Pos(binaryJ);
				int l1Position = 0;

				int *symbol = new int[QUERYSET_LEN];	//��symbol��01���и��ģ�ÿ�λ�õ����λ��1����λ�ö���ͬ
				memcpy(symbol, binaryJ, sizeof(int)*QUERYSET_LEN);
				int *g1 = new int[QUERYSET_LEN];	// "ֻ��1����1����"
				int *g2 = new int[QUERYSET_LEN];	// "ʣ���"

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

					int onePos = getOnesPosition(g1);	//��ȡ1��λ��
					int n = binary2Integer(g2);			//��ȡʮ����
					
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
	//��������
	/*cout << endl << "�����" << endl;
	for (int i = 0; i < xLength; i++) {
		for (int j = 0; j < yLength; j++) {
			cout << tmpPath[i][j] << ' ';
		}
		cout << endl;
	}*/

	//�����̬��
	/*cout << endl << "��̬��" << endl;
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
	
	//���·��
	cout << endl << "���·���� S -> ";
	for (int i = 0; i < QUERYSET_LEN; i++) {
		cout << "Q" << minPath[i] << " -> ";
	}
	cout << "T" << endl;
	
	//������·������
	cout <<endl<<"���·������: "<< table[0][k - 1] << endl;
	myTimer.timerStop();
	
	cout << endl << "�滮��ʱ�� " << myTimer.getTime() << " s" << endl;

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