//============================================================================
// Name        : hk.cpp
// Author      : phill
// Version     :
// Copyright   : Your copyright notice
// Description : HK algorithm for bipartite graph
//============================================================================

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>
#include <set>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <ctime>

#define cl(a,b) memset(a,b, sizeof(a))

using namespace std;
typedef std::pair<int, int> Point;

const int maxn=20008;
const int inf=999999;

vector<int> G[maxn];
int match[maxn] = {0};
int my[maxn] = {0};
int dx[maxn] = {0};
int dy[maxn] = {0};
bool vis[maxn] = {0};


int dis = 999999999;

bool bfs(int N){
	queue<int> q;
	cl(dx,-1);
	cl(dy,-1);
	dis=inf;
	for(int i=0;i<N;i++){
		if(match[i]==-1){
			q.push(i);
			dx[i]=0;
		}
	}
	while(!q.empty()){
		int u=q.front();q.pop();
		if(dx[u]>dis)break;
		for(size_t i=0;i<G[u].size();i++){
			int v=G[u][i];
			if(dy[v]==-1){
				dy[v]=dx[u]+1;
				if(my[v]==-1)dis=dy[v];
				else {
					dx[my[v]]=dy[v]+1;
					q.push(my[v]);
				}
			}
		}
	}
	return dis!=inf;
}
bool dfs(int u){
	int N=G[u].size();
	for(int i=0;i<N;i++){
		int v=G[u][i];
		if(!vis[v]&&dy[v]==dx[u]+1){
			vis[v]=true;
			if(my[v]!=-1&&dy[v]==dis)continue;
			if(my[v]==-1||dfs(my[v]))
			{
				my[v]=u;
				match[u]=v;
				return true;
			}
		}
	}
	return false;
}
int Maxmatch(int N){
	int ans=0;
	cl(match,-1);
	cl(my,-1);
	while(bfs(N))
	{
		cl(vis,false);
		for(int i=0;i<N;i++)
		{
			if(match[i]==-1&&dfs(i))
				ans++;
		}
	}
	return ans;
}


int main(int argc, char* argv[])
{
	std::string filename = "test_data.txt";
	if(argc == 2)
	{
		filename = argv[1];
	}

	std::ifstream data_file;
	data_file.open(filename.c_str());


	if(data_file.is_open())
	{
		std::string line;
		int case_number = 0;
		while(std::getline(data_file, line))
		{
			std::cout<<"case "<<++case_number<<":"<<endl;
			std::set<int> rows;
			std::set<int>  columns;
			std::set<Point> points;
			vector<string> pointstr;
			boost::split(pointstr, line, boost::is_any_of(";"));

			for(size_t i=0; i<pointstr.size(); i++)
			{
				vector<string> xy;
				boost::split(xy, pointstr[i], boost::is_any_of("(,)"));
				if(xy.size() == 2)
				{
					int x = 0;
					int y = 0;
					stringstream(xy[0])>>x;
					stringstream(xy[1])>>y;
					rows.insert(x);
					columns.insert(y);
					points.insert(std::make_pair(x,y));

				}
			}

			int row = *rows.rbegin()+1;
			int column = *columns.rbegin()+1;

			std::cout<<"row: "<< row<<" column: "<<column<<std::endl;

			auto start = std::chrono::system_clock::now();
			// init
			for(int i=0; i<maxn; i++)
				G[i].clear();
			memset(match, -1, sizeof(match));

			auto time1 = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = time1 -start;
			std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for initialize data structures"<<endl;

			//set up edges
			for(auto point:points)
			{
				G[point.first].push_back(point.second+row);
				G[point.second+row].push_back(point.first);
			}


			std::cout<<"The result is: "<<Maxmatch(row)<<std::endl;
			auto time2 = std::chrono::system_clock::now();
			elapsed_seconds = time2 - time1;
			std::cout<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for max matching"<<endl;

		}
	}

	data_file.close();

	return 0;
}
