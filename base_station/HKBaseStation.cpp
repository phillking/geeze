//============================================================================
// Name        : BaseStation.cpp
// Author      : phill king
// Version     :
// Copyright   : PK
// Description : Hopcroft-Karp algorithm to solve the problem
//============================================================================

#include <iostream>
#include <bits/stdc++.h>
using namespace std;


struct Point{
	int x = 0;
	int y = 0;
};

#define MAXSIZE 2048

uint32_t graph[MAXSIZE][MAXSIZE];

class Solution{
public:

	int dir[4][2] = {{-1,0}, {1,0},{0,1},{0,-1}};

	vector<bool> status;
	vector<int> visited;
	vector<int> matchRight;
	vector<int> matchLeft;
	vector<int> disLeft;
	vector<int> disRight;
	int curDistance = INT_MAX;

public:

	bool bfs(vector<int>& left,  vector<vector<int>>& adj){
		std::fill(disLeft.begin(), disLeft.end(), 0);
		std::fill(disRight.begin(), disRight.end(), 0);
		curDistance = INT_MAX;
		queue<int> q;
		for(size_t i=0; i<left.size(); i++){
			if(matchLeft[i] == 0 && !adj[i].empty()){
				q.push(i);
			}
		}

		bool flag = false;
		while(!q.empty()){
			int u = q.front();
			q.pop();
//			if(disLeft[u]>curDistance){
//				break;
//			}
			for(size_t i=0; i<adj[u].size(); i++){
				int v = adj[u][i];
				if(disRight[v] == 0){
					disRight[v] = disLeft[u] + 1;
					if(matchRight[v] == 0){
						flag = true;
//						curDistance = disRight[v];
					}else{
						disLeft[matchRight[v]] = disRight[v] + 1;
						q.push(matchRight[v]);
					}
				}
			}
		}


		return flag;
	}

	bool dfs(vector<vector<int>>& adj, int u){
		for(size_t i=0; i<adj[u].size(); i++){
			int v = adj[u][i];
			if(!status[v] && disRight[v] == disLeft[u]+1){
				status[v] = true;
				visited.push_back(v);
//				if(matchRight[v]!=0 && disRight[v] == curDistance) continue;
				if(!matchRight[v] || dfs(adj, matchRight[v])){
					matchRight[v] = u;
					matchLeft[u] = v;
					return true;
				}
			}
		}
		return false;
	}


	vector<vector<Point>> getMinStations(vector<Point> places){

		memset(graph, 0, sizeof graph);
		vector<vector<Point>> res;
		// handle the points with 0 degree first;
		const int placesSize = places.size();
		int countOne = 0;
		for(int i=0; i<placesSize; i++){
			const Point& p = places[i];
			graph[p.x][p.y] = i+1;
		}
		for(auto& p:places){
			if(p.x == 0) continue;
			int count = 0;
			Point next;
			for(int j=0; j<4; j++){
				int nx = p.x + dir[j][0];
				int ny = p.y + dir[j][1];
				if(graph[nx][ny] != 0){
					count++;
					next.x = nx;
					next.y = ny;
				}

			}
			if(count == 0){
				res.push_back({p});
				p.x = 0;
				graph[p.x][p.y] = 0;
			}
			if(count == 1){
				countOne++;
				vector<Point> tmp;
				tmp.push_back(p);
				tmp.push_back(next);
				res.emplace_back(tmp);
				p.x = 0;
				int index = graph[next.x][next.y]-1;
				places[index].x = 0;
				graph[p.x][p.y] = 0;
				graph[next.x][next.y] = 0;
			}
		}


		vector<int> right = {0};
		vector<int> left = {0};


		for(int i=0; i<placesSize; i++){
			const Point& p = places[i];
			if(p.x == 0) {
				continue;
			}
			if( (p.x+p.y)%2 == 0){
				left.push_back(i);
			}else{
				right.push_back(i);
			}
		}
		if(left.size()>right.size()){
			left.swap(right);
		}
		const int leftSize = left.size();
		const int rightSize = right.size();

		//		memset(graph, 0, sizeof graph);
		// map the right point to right index
		for(int i=1; i<rightSize; i++){
			const Point& p = places[right[i]];
			graph[p.x][p.y] = i;
		}

		vector<vector<int>> adj(leftSize);
		for(int i=1; i<leftSize; i++){
			Point p = places[left[i]];
			for(int j=0; j<4; j++){
				int nx = p.x + dir[j][0];
				int ny = p.y + dir[j][1];
				if(graph[nx][ny]!=0){
					adj[i].push_back(graph[nx][ny]);
				}
			}
		}

		matchLeft.resize(leftSize);
		disLeft.resize(leftSize);
		matchRight.resize(rightSize);
		disRight.resize(rightSize);
		status.resize(rightSize);


		while(bfs(left, adj)){
			for(const auto v:visited){
				status[v] = false;
			}
			visited.clear();
			for(int i=1; i<leftSize; i++){
				if(matchLeft[i] == 0 && !adj[i].empty()){
					dfs(adj, i);
				}
			}
		}

		for(int i=1; i<leftSize; i++){
			Point p = places[left[i]];
			vector<Point> tmp{p};
			if(matchLeft[i] != 0){
				Point p2 = places[right[matchLeft[i]]];
				tmp.push_back(p2);
			}
			res.push_back(tmp);
		}
		for(int i=1; i<rightSize; i++){
			if(matchRight[i] == 0){
				Point p = places[right[i]];
				res.push_back({{p}});
			}
		}

		return res;
	}
};

#define LOCAL

int main(int argc, char* argv[]) {

	std::string file_name = "data.txt";
	if(argc == 2)
	{
		file_name = argv[1];
	}
	int pos = file_name.find_last_of(".");
	std::string outputFileName = file_name.substr(0, pos);
	outputFileName += ".out";

	vector<Point> places;

#ifdef LOCAL
	freopen(file_name.c_str(), "r", stdin);
	//	freopen(outputFileName.c_str(), "w", stdout);
	ofstream outputFile(outputFileName);
#endif

	int total = 0;
	scanf("%d", &total);

	int px = 0;
	int py = 0;
	while(scanf("%d,%d",&px, &py) != EOF){
		Point p;
		p.x = px;
		p.y = py;
		places.push_back(p);
	}

	if(places.size() != (size_t)total){
		cout<<"format error!"<<endl;
		return -1;
	}
	Solution s;
	auto start = std::chrono::system_clock::now();
	vector<vector<Point>> res = s.getMinStations(places);
	auto time1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = time1 -start;

	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second to get the solution"<<endl;

	cout<<res.size()<<endl;
	//	for(auto& station:res){
	//		for(size_t i=0; i<station.size(); i++){
	//			cout<<station[i].x<<","<<station[i].y<<";";
	//		}
	//		cout<<endl;
	//	}

#ifdef LOCAL
	outputFile<<res.size()<<endl;
	for(auto& station:res){
		for(size_t i=0; i<station.size(); i++){
			outputFile<<station[i].x<<","<<station[i].y<<";";
		}
		outputFile<<endl;
	}
	outputFile.close();
#endif
	return 0;
}
