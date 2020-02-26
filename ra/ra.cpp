//============================================================================
// Name        : playground.cpp
// Author      : phill
// Version     :
// Copyright   :Geeze
// Description : Hungarian algorithm to find max matching and select the result
//============================================================================

#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <set>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <ctime>

using namespace std;


typedef std::pair<int, int> Point;
#define maxn 20008


bool check[maxn] = {false};   // the status of checking
int match[maxn] = {0};     // match of rows and columns


int result[maxn] = {0};   // mark the rows and columns when generating the Hungarian BFS tree


vector<int>  G[maxn];    // the bipartite graph

bool dfs(int u)
{
	for(size_t i=0; i<G[u].size(); i++)
	{
		int v = G[u][i];
		if(!check[v])
		{
			check[v] = true;
			if((match[v] == -1) || dfs(match[v]))
			{
				match[v] = u;
				match[u] = v;
				return true;
			}
		}
	}
	return false;
}

int max_matching(int n)
{
	int match_edge_num = 0;
	for(int i=0; i<n; i++)
	{
		memset(check, 0, sizeof(check));
		match_edge_num += dfs(i);
	}
	return match_edge_num;
}

int mark_result(int u)
{
	// skip if it's already marked
	if(result[u] == -1)
		return 0;

	result[u] = -1;
	for(size_t i=0; i<G[u].size(); i++)
	{
		int v = G[u][i];
		if(!check[v])
		{
			check[v] = true;

			if(match[v] != -1  && match[v] !=u)
			{
				result[v] = 1;
				mark_result(match[v]);
			}
		}
	}

	return 0;

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
			memset(result, 0, sizeof(result));


			//set up edges
			for(auto point:points)
			{
				G[point.first].push_back(point.second+row);
				G[point.second+row].push_back(point.first);
			}
			auto time1 = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = time1 -start;
			std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for initialize data structures"<<endl;


			std::cout<<"The result is: "<<max_matching(row)<<std::endl;
			auto time2 = std::chrono::system_clock::now();
			elapsed_seconds = time2 - time1;
			std::cout<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for max matching"<<endl;

			// pick the row and column to fix
			for(auto item:rows)
			{
				if(match[item] == -1 )
				{
					//					cout<<"unmatched row: "<<point.first<<" "<<match[point.first]<<endl;
					memset(check, 0, sizeof(check));
					mark_result(item);
				}
			}

			for(auto item:columns)
			{
				if(match[item+row] == -1 )
				{
					//					cout<<"unmatched column: "<<point.first<<" "<<match[point.first]<<endl;
					memset(check, 0, sizeof(check));
					mark_result(item+row);
				}
			}
			//

			std::set<int> result_row;
			std::set<int> result_column;

			for(auto item:rows)
			{
                // take the row as priority when either is ok
				if(result[item] == 1 )
				{
					result_row.insert(item);
				}
			}
			std::cout<<"row("<<result_row.size()<<"):";
			for(auto item:result_row)
				std::cout<<" "<<item;
			std::cout<<endl;


			for(auto item:columns)
			{
				if(result[item+row] == 1 ||result[item+row] == 0)
				{
					result_column.insert(item);
				}
			}
			std::cout<<"column("<<result_column.size()<<"):";
			for(auto item:result_column)
				std::cout<<" "<<item;
			std::cout<<endl;
			auto time3 = std::chrono::system_clock::now();
			elapsed_seconds = time3 - time2;
			std::cout<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second to find the result set"<<endl<<endl;
		}
	}

	data_file.close();

	cout<<"end"<<endl;
	return 0;
}
