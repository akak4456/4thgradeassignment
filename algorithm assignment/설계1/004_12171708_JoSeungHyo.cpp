#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
struct LeaderNode;
struct Node;
struct LeaderNode{
	int contained = 1;
	//집합이 포함한 원소의 수
	Node* head = NULL;
	//집합의 첫번째 원소
	Node* tail = NULL;
	//집합의 마지막 원소
	int costSum = 0;
	//총 비용 
};
struct Node{
	LeaderNode* leader = NULL;
	//이 노드가 속한 집합의 리더 
	Node* nxt = NULL;
	//집합의 다음 원소
	int numOfCity;
	//도시 번호
	string nameOfCity;
	//도시 이름
	int numOfPeople;
	//도시 인구수 
};
struct Edge{
	int cost;
	//비용
	int population;
	//인구수
	int vertex1,vertex2; 
};
LeaderNode* find(Node* node){
	//노드의 리더를 반환합니다. 
	return node->leader;
}
int getLeaderId(LeaderNode* leader){
	//leader id를 return 하게 됩니다. 
	return leader->head->numOfCity;
}
LeaderNode* merge(Node* a,Node* b,int cost){
	//leader Node를 반환하게 됩니다. 
	LeaderNode* leader1 = find(a);
	LeaderNode* leader2 = find(b);
	if(leader1 == leader2){
		//리더가 같다면 merge할 필요가 없습니다.
		return NULL; 
	}
	if(leader1->contained < leader2->contained||
	(leader1->contained == leader2->contained && getLeaderId(leader1)> getLeaderId(leader2))){
		LeaderNode* tmp = leader1;
		leader1 = leader2;
		leader2 = tmp;	
	} 
	/*
	leader1의 집합이 leader2의 집합보다 크거나
	leader1의 집합이 leader2의 집합과 같고 leader1의 leaderId가 leader2의 leaderId보다 작습니다. 
	*/
	Node* cur = leader2->head;
	while(cur != NULL){
		//leader2에 속한 집합의 leader를 바꿉니다.
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
	//Edge의 정렬 기준을 만듭니다. 
	if(a.cost > b.cost)
		return true;
	if(a.cost < b.cost)
		return false;
	//비용이 같을 때
	if(a.population < b.population)
		return true;
	if(a.population > b.population)
		return false;
	//인구수 마저 같을 때
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
		//vertex를 만듭니다. 
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
		//edge를 만듭니다. 
		cin >> edges[i].vertex1 >> edges[i].vertex2 >> edges[i].cost;
		//첫번째 노드 두번째노드 비용 
		edges[i].population = (cities[edges[i].vertex1]->numOfPeople+cities[edges[i].vertex2]->numOfPeople);
		//인구수 
	}
	sort(edges.begin(),edges.end(),sortBy);
	//정렬을 합니다. 
	while(q--){
		//query를 진행합니다. 
		char ch;
		cin >> ch;
		if(ch == 'N'){
			//특정 도시가 포함된 연결리스트의 내부 정보 파악
			int v,k;
			cin >> v >> k;
			Node* cur = find(cities[v])->head;
			//cur은 집합의 처음 원소를 가리키게 됩니다.
			while(cur != NULL&&k > 0){
				cur = cur->nxt;
				k--;
			}
			if(cur == NULL){
				//k번째가 없으면 
				cout << "no exist\n";
			}else{
				//k번째가 있으면 
				cout << cur->numOfCity << ' ' << cur->nameOfCity << '\n';
			}
		}else if(ch == 'L'){
			//특정 도시가 포함된 연결리스트의 크기
			int v;
			cin >> v;
			cout << find(cities[v])->contained << '\n';
		}else if(ch == 'I'){
			//알고리즘에서 하나의 반복(iteration) 진행
			Edge chosenEdge = edges.back();
			edges.pop_back();
			Node* a = cities[chosenEdge.vertex1];
			Node* b = cities[chosenEdge.vertex2];
			int cost = chosenEdge.cost;
			LeaderNode* ret = merge(a,b,cost);
			if(ret == NULL){
				//union이 실패했으면 
				cout << "not union\n";
			}else{
				if(ret->contained == N){
					//MST가 만들어지 면  
					cout << getLeaderId(ret) << ' ' << ret->contained << ' ' << getLeaderId(ret) << ' ' << ret->costSum<<'\n';
					break;
				}else{
					cout << getLeaderId(ret) << ' ' << ret->contained <<'\n';
				}
			}
		}else if(ch == 'F'){
			//두 도시가 같은 집합에 속하는지 확인
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
			//임의의 도시를 포함하는 집합의 모든 포장도로의 길이
			int v;
			cin >> v;
			cout << find(cities[v])->costSum << '\n';
		}else if(ch == 'Q'){
			// 알고리즘의 나머지 단계 연속수행 및 프로그램 종료
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
