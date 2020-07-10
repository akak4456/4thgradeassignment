#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
struct LeaderNode;
struct Node;
struct LeaderNode{
	int contained = 1;
	//������ ������ ������ ��
	Node* head = NULL;
	//������ ù��° ����
	Node* tail = NULL;
	//������ ������ ����
	int costSum = 0;
	//�� ��� 
};
struct Node{
	LeaderNode* leader = NULL;
	//�� ��尡 ���� ������ ���� 
	Node* nxt = NULL;
	//������ ���� ����
	int numOfCity;
	//���� ��ȣ
	string nameOfCity;
	//���� �̸�
	int numOfPeople;
	//���� �α��� 
};
struct Edge{
	int cost;
	//���
	int population;
	//�α���
	int vertex1,vertex2; 
};
LeaderNode* find(Node* node){
	//����� ������ ��ȯ�մϴ�. 
	return node->leader;
}
int getLeaderId(LeaderNode* leader){
	//leader id�� return �ϰ� �˴ϴ�. 
	return leader->head->numOfCity;
}
LeaderNode* merge(Node* a,Node* b,int cost){
	//leader Node�� ��ȯ�ϰ� �˴ϴ�. 
	LeaderNode* leader1 = find(a);
	LeaderNode* leader2 = find(b);
	if(leader1 == leader2){
		//������ ���ٸ� merge�� �ʿ䰡 �����ϴ�.
		return NULL; 
	}
	if(leader1->contained < leader2->contained||
	(leader1->contained == leader2->contained && getLeaderId(leader1)> getLeaderId(leader2))){
		LeaderNode* tmp = leader1;
		leader1 = leader2;
		leader2 = tmp;	
	} 
	/*
	leader1�� ������ leader2�� ���պ��� ũ�ų�
	leader1�� ������ leader2�� ���հ� ���� leader1�� leaderId�� leader2�� leaderId���� �۽��ϴ�. 
	*/
	Node* cur = leader2->head;
	while(cur != NULL){
		//leader2�� ���� ������ leader�� �ٲߴϴ�.
		cur->leader = leader1;
		cur = cur->nxt;
	} 
	leader1->tail->nxt = leader2->head;
	leader1->tail = leader2->tail;
	leader1->contained += leader2->contained;
	leader1->costSum += cost;
	leader1->costSum += leader2->costSum;
	delete leader2;
	return leader1;
}
int N,M,q;
Node* cities[1000010];
LeaderNode* leaderNode[50000];
vector<Edge> edges;
bool sortBy(const Edge& a,const Edge& b){
	//Edge�� ���� ������ ����ϴ�. 
	if(a.cost > b.cost)
		return true;
	if(a.cost < b.cost)
		return false;
	//����� ���� ��
	if(a.population < b.population)
		return true;
	if(a.population > b.population)
		return false;
	//�α��� ���� ���� ��
	int minA = a.vertex1;
	int minB = b.vertex1;
	if(minA > a.vertex2){
		minA = a.vertex2;
	} 
	if(minB > b.vertex2){
		minB = b.vertex2;
	}
	return minA < minB;
}
int main(){
	cin >> N >> M >> q; 
	for(int i=0;i<N;i++){
		//vertex�� ����ϴ�. 
		int num;
		cin >> num;
		cities[num] = new Node();
		leaderNode[i] = new LeaderNode();
		cities[num]->leader = leaderNode[i];
		leaderNode[i]->head = cities[num];
		leaderNode[i]->tail = cities[num];
		cities[num]->numOfCity = num;
		cin >> cities[num]->nameOfCity >> cities[num]->numOfPeople;
	}
	edges.resize(M);
	for(int i=0;i<M;i++){
		//edge�� ����ϴ�. 
		cin >> edges[i].vertex1 >> edges[i].vertex2 >> edges[i].cost;
		//ù��° ��� �ι�°��� ��� 
		edges[i].population = (cities[edges[i].vertex1]->numOfPeople+cities[edges[i].vertex2]->numOfPeople);
		//�α��� 
	}
	sort(edges.begin(),edges.end(),sortBy);
	//������ �մϴ�. 
	while(q--){
		//query�� �����մϴ�. 
		char ch;
		cin >> ch;
		if(ch == 'N'){
			//Ư�� ���ð� ���Ե� ���Ḯ��Ʈ�� ���� ���� �ľ�
			int v,k;
			cin >> v >> k;
			Node* cur = find(cities[v])->head;
			//cur�� ������ ó�� ���Ҹ� ����Ű�� �˴ϴ�.
			while(cur != NULL&&k > 0){
				cur = cur->nxt;
				k--;
			}
			if(cur == NULL){
				//k��°�� ������ 
				cout << "no exist\n";
			}else{
				//k��°�� ������ 
				cout << cur->numOfCity << ' ' << cur->nameOfCity << '\n';
			}
		}else if(ch == 'L'){
			//Ư�� ���ð� ���Ե� ���Ḯ��Ʈ�� ũ��
			int v;
			cin >> v;
			cout << find(cities[v])->contained << '\n';
		}else if(ch == 'I'){
			//�˰��򿡼� �ϳ��� �ݺ�(iteration) ����
			Edge chosenEdge = edges.back();
			edges.pop_back();
			Node* a = cities[chosenEdge.vertex1];
			Node* b = cities[chosenEdge.vertex2];
			int cost = chosenEdge.cost;
			LeaderNode* ret = merge(a,b,cost);
			if(ret == NULL){
				//union�� ���������� 
				cout << "not union\n";
			}else{
				if(ret->contained == N){
					//MST�� ������� ��  
					cout << getLeaderId(ret) << ' ' << ret->contained << ' ' << getLeaderId(ret) << ' ' << ret->costSum<<'\n';
					break;
				}else{
					cout << getLeaderId(ret) << ' ' << ret->contained <<'\n';
				}
			}
		}else if(ch == 'F'){
			//�� ���ð� ���� ���տ� ���ϴ��� Ȯ��
			int V1,V2;
			cin >> V1 >> V2;
			LeaderNode* leader1 = find(cities[V1]);
			LeaderNode* leader2 = find(cities[V2]);
			if(leader1 == leader2){
				cout << "True "<<getLeaderId(leader1)<<'\n';
			}else{
				cout << "False " << getLeaderId(leader1) << ' ' << getLeaderId(leader2)<<'\n';
			}
		}else if(ch == 'W'){
			//������ ���ø� �����ϴ� ������ ��� ���嵵���� ����
			int v;
			cin >> v;
			cout << find(cities[v])->costSum << '\n';
		}else if(ch == 'Q'){
			// �˰����� ������ �ܰ� ���Ӽ��� �� ���α׷� ����
			while(!edges.empty()){
				Edge chosenEdge = edges.back();
				edges.pop_back();
				Node* a = cities[chosenEdge.vertex1];
				Node* b = cities[chosenEdge.vertex2];
				int cost = chosenEdge.cost;
				LeaderNode* ret = merge(a,b,cost);
				if(ret != NULL&&ret->contained == N){
					cout << getLeaderId(ret) << ' ' << ret->costSum<<'\n';
					break;
				}
			}
		}
	}
}
