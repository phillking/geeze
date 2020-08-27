//============================================================================
// Name        : BaseStation.cpp
// Author      : phill king
// Version     :
// Copyright   : PK
// Description : Hungarian algorithm to solve the problem
//============================================================================

#include <iostream>
#include <bits/stdc++.h>
using namespace std;

#define DEBUG

struct Point{
	int x = 0;
	int y = 0;
};

#define MAXSIZE 2048

static int graph[MAXSIZE][MAXSIZE];

class Solution{
public:

	int dir[4][2] = {{-1,0}, {1,0},{0,1},{0,-1}};
	const int base = 14;
	vector<int> matchRight;
	vector<int> matchLeft;

	vector<bool> status;
	vector<int> visited;


public:

	bool dfs(vector<vector<int>>& adj, int u){
		for(size_t i=0; i<adj[u].size(); i++){
			int v = adj[u][i];
			if(!status[v]){
				status[v] = true;
				visited.push_back(v);
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
//		memset(graph, 0, sizeof graph);
		const int placesSize = places.size();
		vector<vector<Point>> res;
		// pre-process the points with 0 and 1 degree ;
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
					if(count>1) break;
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
		matchRight.resize(rightSize);
		status.resize(rightSize);

		for(int i=1; i<leftSize; i++){
			for(auto v:visited){
				status[v] = false;
			}
			visited.clear();

			if(!adj[i].empty()){
				dfs(adj, i);
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



class BFSSolution{
public:

	int dir[4][2] = {{-1,0}, {1,0},{0,1},{0,-1}};

public:

	vector<vector<Point>> getMinStations(vector<Point> places){
//		memset(graph, 0, sizeof graph);
		const int placesSize = places.size();
		vector<vector<Point>> res;
		// pre-process the points with 0 and 1 degree ;
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
					if(count>1) break;
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

		vector<int> prev(leftSize);
		vector<int> matchLeft(leftSize);

		vector<int> matchRight(rightSize);
		vector<int> visited(rightSize);
		vector<int> que(rightSize);


		for(int i=1; i<leftSize; i++){
			if(matchLeft[i] != 0) continue;
			int head = 0;
			int tail = 0;
			que[tail++] = i;
			prev[i] = -1;
			bool flag = false;
			while(head<tail && !flag){
				int u = que[head++];
				for(size_t j=0;j<adj[u].size()&&!flag; j++){
					int v = adj[u][j];
					if(visited[v] != i){
						visited[v] = i;
						que[tail++] = matchRight[v];
						if(matchRight[v] == 0){
							flag = true;
							int d = u;
							int e = v;
							while(d!=-1){
								int tmp = matchLeft[d];
								matchRight[e] = d;
								matchLeft[d] = e;
								e = tmp;
								d = prev[d];

							}
						}else{
							prev[matchRight[v]] = u;
						}
					}
				}
			}

		}


		// collect the results
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

#ifndef ONLINE_DEBUG
	freopen(file_name.c_str(), "r", stdin);
	//	freopen(outputFileName.c_str(), "w", stdout);
	FILE* fp = fopen(outputFileName.c_str(), "w");
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
	BFSSolution s;
	auto start = std::chrono::system_clock::now();
	vector<vector<Point>> res = s.getMinStations(places);
	auto time1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = time1 -start;
#ifdef DEBUG
	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" seconds to get the solution"<<endl;

#endif

	cout<<res.size()<<endl;
//	for(auto& station:res){
//		for(size_t i=0; i<station.size(); i++){
//			cout<<station[i].x<<","<<station[i].y<<";";
//		}
//		cout<<endl;
//	}

#ifndef ONLINE_DEBUG
	fprintf(fp,"%d\n", res.size());
	for(auto& station:res){
		for(size_t i=0; i<station.size(); i++){
			fprintf(fp,"%d,%d;", station[i].x, station[i].y);
		}
		fprintf(fp, "\n");
	}	
	fclose(fp);
#endif

	return 0;
}
