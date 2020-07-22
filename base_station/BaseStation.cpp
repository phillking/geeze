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


struct Point{
	int x = 0;
	int y = 0;
};


class Solution{
public:

	int dir[4][2] = {{-1,0}, {1,0},{0,1},{0,-1}};
	const int base = 14;
	vector<bool> status;
	vector<int> matchRight;
	vector<int> matchLeft;


public:
	int getID(Point p){
		return (p.x<<base) + p.y;
	}
	int getID(int x, int y){
		return (x<<base) + y;
	}

	Point getPoint(int id){
		Point p;
		p.x = id>>base;
		p.y = id&0x3FFF;
		return p;
	}



	bool dfs(vector<vector<int>>& adj, int index){
		for(size_t i=0; i<adj[index].size(); i++){
			int v = adj[index][i];
			if(!status[v]){
				status[v] = true;
				if(!matchRight[v] || dfs(adj, matchRight[v])){
					matchRight[v] = index;
					matchLeft[index] = v;
					return true;
				}
			}
		}
		return false;
	}


	vector<vector<Point>> getMinStations(vector<Point> places){
		vector<int> right = {0};
		vector<int> left = {0};
		unordered_map<int , int> idMap;
		for(auto p:places){
			int id = getID(p);
			if( (p.x+p.y)%2 == 0){
				left.push_back(id);
			}else{
				right.push_back(id);
			}
		}

		if(left.size()>right.size()){
			swap(left, right);
		}


		for(size_t i=1; i<right.size(); i++){
			idMap[right[i]] = i;
		}

		const int size = left.size();
		vector<vector<int>> adj(size);
		for(int i=1; i<size; i++){
			Point p = getPoint(left[i]);
			for(int j=0; j<4; j++){
				int nx = p.x + dir[j][0];
				int ny = p.y + dir[j][1];
				int tmp = getID(nx,ny);
				if(idMap.count(tmp)){
					adj[i].push_back(idMap[tmp]);
				}
			}
		}

		matchLeft.resize(size);
		matchRight.resize(right.size());
		status.resize(right.size());

		for(int i=1; i<size; i++){
			std::fill(status.begin(), status.end(), false);
			if(!adj.empty()){
				dfs(adj, i);
			}
		}




		vector<vector<Point>> res;
		for(int i=1; i<size; i++){
			Point p = getPoint(left[i]);
			vector<Point> tmp{p};
			if(matchLeft[i] != 0){
				Point p2 = getPoint(right[matchLeft[i]]);
				tmp.push_back(p2);
			}
			res.push_back(tmp);
		}
		for(size_t i=1; i<right.size(); i++){
			if(matchRight[i] == 0){
				Point p = getPoint(right[i]);
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
	std::string output_filename = file_name.substr(0, pos);
	output_filename += ".out";

	vector<Point> places;

#ifdef LOCAL
	freopen(file_name.c_str(), "r", stdin);
//	freopen(output_filename.c_str(), "w", stdout);
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
	for(auto& station:res){
		for(size_t i=0; i<station.size(); i++){
			cout<<station[i].x<<","<<station[i].y<<";";
		}
		cout<<endl;
	}
	return 0;
}
