//============================================================================
// Name        : BinPacking.cpp
// Author      : phill
// Version     :
// Copyright   : PK inc.
// Description : Bin Packing algorithm in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stack>
#include <map>
#include <unordered_map>
#include <boost/unordered_map.hpp>
#include <string>
#include <queue>

using namespace std;

typedef vector<vector<int > > BINS_TYPE;

static unsigned int dp[1001][1001] = {{0}};
bool b_output = true;
bool bStop = false;

struct bp_context{
	vector<int> left_tasks;
	vector<int> used_tasks;
	BINS_TYPE used_bins;
	int wasted_sum;   // sum plus wasted unit
};
const int MAX_BOUND = 10001;

std::stack<bp_context> bp_stack;
std::queue<bp_context> bp_queue;


static int LOWER_BOUND = MAX_BOUND;

static int CUR_MIN_SIZE = MAX_BOUND;

static int dominance_count = 0;

std::map<vector<int>, int> bp_map;

std::vector< vector<vector<int> > > bp_dominance;

class BinPacking{
public:

	// make sure the vectors are sorted by ascending
	static bool is_dominant(const vector<int>& A, const vector<int>& b){
//		std::sort(a.rbegin(), a.rend());
//		std::sort(b.rbegin(), b.rend());

		static vector<int> a;
		a.clear();
		a = A;

		if(b.back() > a.back())
			return false;

		for(int j = b.size()-1; j>=0; j--)
		{
			int item = b[j];
			int min_left = MAX_BOUND;
			int index = 0;
			for(int i=a.size()-1; i>=0; i--){

				if(item<=a[i] && (a[i]-item)<=min_left){
					index = i;
					min_left = a[i] - item;
				}
				if(item == a[i]){
					break;
				}
			}

			if(min_left == MAX_BOUND){  // not found
				return false;
			}
			else{
				a[index] -= item;
			}
		}
		return true;
	}

	// make sure the vectors are sorted by descending
	static bool is_dominant_desc(const vector<int>& A, const vector<int>& b){
	//		std::sort(a.rbegin(), a.rend());
	//		std::sort(b.rbegin(), b.rend());

			static vector<int> a;
			a.clear();
			a = A;

			if(b.front() > a.front())
				return false;

			for(auto& item:b)
			{

				int min_left = MAX_BOUND;
				int index = 0;
				for(int i=0; i<a.size(); i++){

					if(item<=a[i] && (a[i]-item)<=min_left){
						index = i;
						min_left = a[i] - item;
					}
					if(item == a[i]){
						break;
					}
				}

				if(min_left == MAX_BOUND){  // not found
					return false;
				}
				else{
					a[index] -= item;
				}
			}
			return true;
		}

	static bool found_dominant(const BINS_TYPE& a,  const vector<int>& b){


		for(const auto& item:a){
			if(is_dominant(item, b))
				return true;
		}

		return false;

	}
	static BINS_TYPE& get_feasible_sets_dp(const int capacity,  vector<int>& tasks){
//		std::sort(tasks.rbegin(), tasks.rend());
		static BINS_TYPE dp_bins;
		dp_bins.clear();

		if(tasks.empty())
			return dp_bins;

		std::sort(tasks.rbegin(), tasks.rend());

		if(tasks[0] == capacity){
			vector<int> temp;
			temp.push_back(tasks[0]);
			dp_bins.push_back(temp);
//			remove_item(tasks, capacity);
			return dp_bins;
		}

//		memset(dp, 0, sizeof(dp));
		for( int j=0; j<= capacity; j++){
			if(j < tasks[0]){
				dp[0][j] = 0;
			}
			else{
				dp[0][j] = tasks[0];
			}
		}

		for(size_t i=1; i< tasks.size(); i++){
			for( int j=0; j<= capacity; j++){
				const int days = tasks[i];
				if(j < days){
					dp[i][j] = dp[i-1][j];
				}
				else{
					dp[i][j] = std::max(dp[i-1][j], dp[i-1][j-days] + days);
				}
			}
		}

		// dump dp
		//		for(size_t i=0; i<= tasks.size(); i++){
		//			for( int j=0; j<=capacity; j++){
		//				cout<<dp[i][j]<<" ";
		//			}
		//			cout<<endl;
		//		}


		static vector<int> used_bin;
		used_bin.clear();
		//     single item first
//		int fea_single = tasks[0];
//
//		int last_item = 0;
//		if(tasks.size()>1){
//			last_item = tasks.back();
//		}
//		if((fea_single+last_item) > capacity || tasks.size()==1){
//
//			used_bin.push_back(fea_single);
//			dp_bins.push_back(std::move(used_bin));
//			used_bin.clear();
//		}

		int task_index = tasks.size() - 1;


		int low_index = tasks[0];

		for(int cap_index = capacity; cap_index > low_index; cap_index-- ){

			int max = dp[task_index][cap_index];
			if(max == low_index)
				break;

			vector<int> index_list;
			for(size_t i=tasks.size()-1; i >= 1; i--){

				if (max == dp[i][cap_index] ){
					// if it's reachable
					if((dp[i][cap_index] - tasks[i]) == dp[i-1][cap_index-tasks[i]]){
						index_list.push_back(i);
					}
				}
			}


			std::sort(index_list.begin(), index_list.end());
			multimap<int, vector<int> > bins_map;

			for(auto& index: index_list){
				used_bin.clear();
				used_bin.push_back(tasks[index]);
				int j = cap_index - tasks[index];
				for(int i = index -1 ; i>=0; i-- ){
					if(i == 0 && dp[i][j]!=0){
						used_bin.push_back(tasks[i]);
					}
					else if(dp[i][j] > dp[i-1][j]){ //used
						used_bin.push_back(tasks[i]);
						j = j - tasks[i];
					}
				}
				bins_map.insert(make_pair(used_bin.size(), std::move(used_bin)));

				used_bin.clear();
			}

			for(auto& item:bins_map){
				if(!found_dominant(dp_bins, item.second)){
					dp_bins.push_back(std::move(item.second));
				}
			}

		}
//		std::sort(dp_bins.begin(), dp_bins.end(), std::greater<vector<int> >());
		return dp_bins;
	}

	static BINS_TYPE& get_feasible_sets(const int capacity, vector<int>& tasks){
		std::sort(tasks.rbegin(), tasks.rend());
		static BINS_TYPE bins;
		bins.clear();
		if(tasks.empty())
			return bins;

		if(tasks[0] == capacity){
			vector<int> used_bin;
			used_bin.push_back(capacity);
			bins.push_back(used_bin);
			return bins;
		}


		bins = get_feasible_sets_dp(capacity, tasks);

		vector<int> used_bin;
		// dp
		//		vector<int> mini_tasks = tasks;;
		//		used_bin = dp_bin(capacity, mini_tasks);
		//		if(!found_dominant(bins, used_bin)){
		//			bins.push_back(used_bin);
		//		}
		//		used_bin.clear();




		// single
		int fea_single = tasks[0];

		int last_item = 0;
		if(tasks.size()>1){
			last_item = tasks.back();
		}
		if((fea_single+last_item) > capacity || tasks.size()==1){
			used_bin.push_back(fea_single);
			if(!found_dominant(bins, used_bin)){
				bins.push_back(used_bin);
			}
			used_bin.clear();
		}

		return bins;



		//pair
		for(size_t i=0; i<tasks.size(); i++){
			for(size_t j=i+1; j<tasks.size(); j++){
				int pair_sum = tasks[i] + tasks[j];
				if(pair_sum<= capacity && pair_sum > fea_single){
					used_bin.push_back(tasks[i]);
					used_bin.push_back(tasks[j]);
					if(!found_dominant(bins, used_bin)){
						bins.push_back(used_bin);
					}
					used_bin.clear();
					break;
				}
			}
		}
		//trip

		for(size_t i=0; i<tasks.size(); i++){
			for(size_t j=i+1; j<tasks.size(); j++){
				for(size_t k=j+1; k<tasks.size(); k++){
					int trip_sum = tasks[i] + tasks[j] + tasks[k];
					if(trip_sum<= capacity && trip_sum > fea_single){
						used_bin.push_back(tasks[i]);
						used_bin.push_back(tasks[j]);
						used_bin.push_back(tasks[k]);
						if(!found_dominant(bins, used_bin)){
							bins.push_back(used_bin);
						}
						used_bin.clear();
						break;
					}
				}
			}
		}

		//quadrant

		for(size_t i=0; i<tasks.size(); i++){
			for(size_t j=i+1; j<tasks.size(); j++){
				for(size_t k=j+1; k<tasks.size(); k++){
					for(size_t l=k+1; l<tasks.size(); l++){
						int trip_sum = tasks[i] + tasks[j] + tasks[k] + tasks[l];
						if(trip_sum<= capacity && trip_sum > fea_single){
							used_bin.push_back(tasks[i]);
							used_bin.push_back(tasks[j]);
							used_bin.push_back(tasks[k]);
							used_bin.push_back(tasks[l]);
							if(!found_dominant(bins, used_bin)){
								bins.push_back(used_bin);
							}
							used_bin.clear();
							break;
						}
					}
				}
			}
		}

		// dump bins
		//		for(auto& one_bin:bins){
		//			for(auto& item:one_bin){
		//				cout<<item<<" ";
		//			}
		//			cout<<endl;
		//		}

		return bins;
	}

	static BINS_TYPE& get_ibc_branch(const int capacity, vector<int>& tasks, vector<int>& used_tasks,const BINS_TYPE& pre_bins, int wasted_sum, int pre_size){
//		std::sort(tasks.rbegin(), tasks.rend());
		static BINS_TYPE bins;
		bins.clear();
		bins = std::move(pre_bins);
		bool branch_cut = false;
		static vector<int> used_bins;

		while(!tasks.empty()){
			used_bins.clear();
			int first_value = tasks[0];
			int left_cap = capacity - first_value;
			vector<int> mini_tasks;
			int mini_sum = 0;
			for(size_t i=1; i<tasks.size(); i++){
				if(tasks[i] <= left_cap){
					mini_tasks.push_back(tasks[i]);
					mini_sum += tasks[i];
				}
			}


			// if all mini items could fit into the left space
			if(mini_sum <= left_cap){
				used_bins = mini_tasks;
			}
			else if(mini_tasks.size()>0){
				BINS_TYPE fea_bins = get_feasible_sets(left_cap, mini_tasks);

				if( (fea_bins.size() >= 1) && !bStop ){
//					cout<<"feasible set num: "<<fea_bins.size()<<"; bins num:"<<bins.size()<<endl;;

					used_bins = fea_bins[0];
					for(int i=fea_bins.size()-1; i>=1; i--){
						int expected_size = bins.size()+1;

						bp_context bpc;
						bpc.used_bins = bins;
						fea_bins[i].push_back(first_value);
						bpc.used_bins.push_back(fea_bins[i]);
						bpc.left_tasks = tasks;
						bpc.used_tasks = used_tasks;
						for(auto& item:fea_bins[i]){
							bpc.used_tasks.push_back(item);
							remove_item(bpc.left_tasks, item);
						}
						if(bpc.left_tasks.size()>0){
							expected_size += 1;
						}

						if(expected_size + pre_size >= CUR_MIN_SIZE)
							continue;


						int used_sum = 0;
						for(const auto& item:fea_bins[i]){
							used_sum += item;
						}

						bpc.wasted_sum = wasted_sum + capacity - used_sum;
						if(bpc.wasted_sum > (CUR_MIN_SIZE-1)*capacity){
							continue;
						}

						auto iter = bp_map.find(bpc.left_tasks);
						if(iter == bp_map.end()){
							bp_map.insert(make_pair(bpc.left_tasks, bpc.used_bins.size()));
							int used_size = bpc.used_bins.size();
							if(used_size<0){
								if(!bp_dominance[used_size].empty()){
									vector<vector<int > >& left_tasks = bp_dominance[used_size];
									bool b_dominant = false;

									for(auto& item_task:left_tasks){
										if(is_dominant_desc( bpc.left_tasks,item_task))
										{
											b_dominant = true;
											dominance_count++;
											break;
										}
									}

									if(b_dominant){
//										cout<<used_size<<endl;
										continue;
									}
								}

								bp_dominance[used_size].push_back(bpc.left_tasks);

							}


						}
						else{
							if(iter->second <= bpc.used_bins.size()){
								continue;
							}
							else{
								iter->second = bpc.used_bins.size();
							}

						}

						bp_stack.push(bpc);

					}
				}

			}

			used_bins.push_back(first_value);
			// remove used items
			for(auto& item:used_bins){
				used_tasks.push_back(item);
				remove_item(tasks, item);
			}
			bins.push_back(used_bins);
			// calculated wasted units
			int used_sum = 0;
			for(const auto& item:used_bins){
				used_sum += item;
			}
			wasted_sum += capacity - used_sum;

			if(wasted_sum > (CUR_MIN_SIZE-1)*capacity){
				//				    	cout<<"no hope branch"<<endl;
				bins.clear();
				break;
			}


			auto iter = bp_map.find(tasks);
			if(iter == bp_map.end()){

				bp_map.insert(make_pair(tasks, bins.size()));
				int used_size = bins.size();
				if(used_size<0){
					if(!bp_dominance[used_size].empty()){
						vector<vector<int > >& left_tasks = bp_dominance[used_size];
						bool b_dominant = false;

						for(auto& item_task:left_tasks){
							if(is_dominant_desc( tasks, item_task))
							{
								b_dominant = true;
								dominance_count++;
								break;
							}
						}

						if(b_dominant){
//							cout<<used_size<<endl;
							bins.clear();
							break;
						}

					}
					bp_dominance[used_size].push_back(tasks);

				}


			}
			else{
				if((iter->second) > bins.size()){

					iter->second = bins.size();
				}
				else{
					bins.clear();
					break;
				}
			}
		}
		return bins;
	}

	// improved bin completion algorithm
	static BINS_TYPE get_ibc(const int capacity, vector<int> tasks){

		std::sort(tasks.rbegin(), tasks.rend());

		int sum = 0;
		for(const auto& item:tasks){
			sum += item;
		}

		BINS_TYPE bins;
		// first packing the single and pair values
		vector<int> used_bin;
		for(size_t i=0; i<tasks.size(); i++){
			if(capacity == tasks[i]){
				used_bin.push_back(tasks[i]);
				bins.push_back(used_bin);
				used_bin.clear();
			}
		}
		//remove all single full item
		for(auto iter = tasks.begin(); iter != tasks.end();){
			if(*iter == capacity){
				iter = tasks.erase(iter);
			}
			else
				iter++;
		}

		// check all pair full items
		while(!tasks.empty()){
			bool isFound = false;
			for(size_t i =0; i<tasks.size()-1; i++){
				for(size_t j=i+1; j<tasks.size(); j++){
					if((tasks[i] + tasks[j]) == capacity){
						isFound = true;
						int first = tasks[i];
						int second = tasks[j];
						used_bin.push_back(first);
						used_bin.push_back(second);
						bins.push_back(used_bin);
						used_bin.clear();
						remove_item(tasks, second);
						remove_item(tasks, first);

						break;
					}
				}
				if(isFound)
					break;
			}

			if(!isFound){
				break;
			}
		}



		BINS_TYPE pre_bins = bins;
		const int pre_size = bins.size();

		bp_context context;
		context.left_tasks = tasks;
		context.wasted_sum = sum;   // no wasted unit yet
		bp_queue.push(context);

		while(bp_queue.size() <= 0 && bp_queue.size()>0){
			bp_context front_bpc = bp_queue.front();
			bp_queue.pop();
			tasks = front_bpc.left_tasks;
			bins = front_bpc.used_bins;
			vector<int> used_tasks = front_bpc.used_tasks;
			int wasted_sum = front_bpc.wasted_sum;
			if(!tasks.empty()){

				int first_value = tasks[0];
				int left_cap = capacity - first_value;
				vector<int> mini_tasks;
				int mini_sum = 0;
				for(size_t i=1; i<tasks.size(); i++){
					if(tasks[i] <= left_cap){
						mini_tasks.push_back(tasks[i]);
						mini_sum += tasks[i];
					}
				}
				vector<int> used_bins;

				if(mini_tasks.size()>0){
					BINS_TYPE fea_bins = get_feasible_sets(left_cap, mini_tasks);

					//					cout<<"feasible set num: "<<fea_bins.size()<<"; bins num:"<<bins.size()<<endl;;

					for(int i=0; i<fea_bins.size(); i++){

						bp_context bpc;
						bpc.used_bins = bins;
						fea_bins[i].push_back(first_value);
						bpc.used_bins.push_back(fea_bins[i]);
						bpc.left_tasks = tasks;
						bpc.used_tasks = used_tasks;
						int used_sum = 0;
						for(const auto& item:fea_bins[i]){
							used_sum += item;
						}
						for(auto& item:fea_bins[i]){
							bpc.used_tasks.push_back(item);
							remove_item(bpc.left_tasks, item);
						}
						bpc.wasted_sum = wasted_sum + capacity - used_sum;
						bp_queue.push(bpc);

					}
				}
				else{
					bp_context bpc;
					bpc.used_bins = bins;
					used_bin.push_back(first_value);
					bpc.used_bins.push_back(used_bin);
					bpc.left_tasks = tasks;
					bpc.used_tasks = used_tasks;
					int used_sum = first_value;

					remove_item(bpc.left_tasks, first_value);
					bpc.used_tasks.push_back(first_value);

					bpc.wasted_sum = wasted_sum + capacity - used_sum;
					bp_queue.push(bpc);
				}

			}
		}

        if(b_output){
        	cout<<"seed tree size "<<bp_queue.size()<<endl;
        }
		int total_count = 0;
		const int COUNT_LIMIT_TOTAL = 200000000;
		const int COUNT_LIMIT = COUNT_LIMIT_TOTAL/bp_queue.size();

		BINS_TYPE min_bins;
		int min_size = 100001;
		bool b_found = false;

		while(!bp_queue.empty()){
			int count = 0;
			bp_map.clear();
			bp_context seed_bpc = bp_queue.front();
			bp_queue.pop();
			bp_stack.push(seed_bpc);
			bStop = false;

			while(!bp_stack.empty()){
//				bp_context bpc = bp_stack.top();

				bins.clear();

				tasks = std::move( bp_stack.top().left_tasks);

				bins = std::move( bp_stack.top().used_bins );
				vector<int> used_tasks = std::move(bp_stack.top().used_tasks);
				int wasted_sum = bp_stack.top().wasted_sum;

				bp_stack.pop();

				bins = get_ibc_branch(capacity,tasks, used_tasks, bins, wasted_sum, pre_size);

				count++;
				if(count>COUNT_LIMIT){
					bStop = true;
				}


				// back from no hope branch
				if(bins.size() == 0){
					continue;
				}



//				cout<<"bin size: "<<bins.size()<<endl;
//				for(size_t i=0; i<bins.size(); i++){
//					for(size_t j=0; j<bins[i].size(); j++){
//						cout<<bins[i][j]<<" ";
//					}
//					cout<<endl;
//				}

				int new_size = bins.size() + pre_size;
				if(new_size < min_size){
					min_size = new_size;
					CUR_MIN_SIZE = min_size;
//					if(CUR_MIN_SIZE > LOWER_BOUND){
//						CUR_MIN_SIZE = LOWER_BOUND + 1;
//					}
					min_bins = bins;

				}
				if(new_size <= LOWER_BOUND){
					b_found = true;
					break;
					//				for(auto& item:min_bins){
					//					for(auto& value:item){
					//						cout<<value<<" ";
					//					}
					//					cout<<endl;
					//				}
				}

			}

			total_count += count;
			if(b_found){
				break;
			}
		}

		if(b_output){
			std::cout<<"count: "<<total_count<<endl<<"bp map size:"<<bp_map.size()<<endl;
			std::cout<<"dominance count: "<<dominance_count<<endl;//<<"bp map size:"<<bp_map.size()<<endl;
		}

		std::copy(min_bins.begin(), min_bins.end(), std::back_inserter(pre_bins));
		return pre_bins;
	}

	// fast bin completion algorithm
	static BINS_TYPE get_fbc(const int capacity, vector<int> tasks){
		std::sort(tasks.rbegin(), tasks.rend());
		BINS_TYPE bins;

		while(!tasks.empty()){
			int first_value = tasks[0];
			int left_cap = capacity - first_value;
			vector<int> mini_tasks;
			for(size_t i=1; i<tasks.size(); i++){
				if(tasks[i] <= left_cap){
					mini_tasks.push_back(tasks[i]);
				}
			}


			vector<int> used_bins = dp_bin(left_cap, mini_tasks);

			used_bins.push_back(first_value);
			// remove used items
			for(auto& item:used_bins){
				remove_item(tasks, item);
			}
			bins.push_back(used_bins);
		}
		return bins;
	}


	static BINS_TYPE get_bfd(const int capacity, vector<int>& tasks){
		std::sort(tasks.rbegin(), tasks.rend());
		//		for(size_t i=0; i<tasks.size(); i++){
		//			cout<<tasks[i]<<" ";
		//		}
		//		cout<<endl;
		vector<int> bin_remain;
		vector<vector<int > > bins;
		vector<int> first_bin;
		bins.push_back(first_bin);

		bin_remain.push_back(capacity);

		for(size_t i=0; i<tasks.size(); i++){
			int min_left = capacity+1;
			int selected_bin = 0;
			const int days = tasks[i];

			for(size_t j=0; j<bin_remain.size(); j++){

				if(days <= bin_remain[j] && (bin_remain[j]-days <= min_left) ){
					selected_bin = j;
					min_left = bin_remain[j] - days;
				}
			}

			if(min_left == capacity+1){
				bin_remain.push_back(capacity-days);
				vector<int> new_bin(1, days);
				bins.push_back(new_bin);

			}
			else{
				bin_remain[selected_bin] -= days;
				bins[selected_bin].push_back(days);
			}
		}

		//		cout<<bins.size()<<endl;
		//		for(size_t i=0; i<bins.size(); i++){
		//			for(size_t j=0; j<bins[i].size(); j++){
		//				cout<<bins[i][j]<<" ";
		//			}
		//			cout<<endl;
		//		}

		return bins;
	}

	// return a list of candidate bins
	static BINS_TYPE dp_bins(const int capacity, vector<int> tasks){
		BINS_TYPE bins;
		if(tasks.empty())
			return bins;

		std::sort(tasks.rbegin(), tasks.rend());

		if(tasks[0] == capacity){
			vector<int> temp_bin;
			temp_bin.push_back(tasks[0]);
			bins.push_back(temp_bin);
			remove_item(tasks, capacity);
			return bins;
		}

		memset(dp, 0, sizeof(dp));
		for( int j=0; j<= capacity; j++){
			if(j < tasks[0]){
				dp[0][j] = 0;
			}
			else{
				dp[0][j] = tasks[0];
			}
		}

		for(size_t i=1; i< tasks.size(); i++){
			for( int j=0; j<= capacity; j++){
				const int days = tasks[i];
				if(j < days){
					dp[i][j] = dp[i-1][j];
				}
				else{
					dp[i][j] = std::max(dp[i-1][j], dp[i-1][j-days] + days);
				}
			}
		}

		// dump dp
		//		for(size_t i=0; i<= tasks.size(); i++){
		//			for( int j=0; j<=capacity; j++){
		//				cout<<dp[i][j]<<" ";
		//			}
		//			cout<<endl;
		//		}


		vector<int> used_bin;
		int j = capacity;
		for(int i = tasks.size()-1; i>=0; i-- ){
			if(i == 0 && dp[i][j]!=0){
				used_bin.push_back(tasks[i]);
			}
			else if(dp[i][j] > dp[i-1][j]){ //used
				used_bin.push_back(tasks[i]);
				j = j - tasks[i];
			}
		}

		bins.push_back(used_bin);
		used_bin.clear();

		return bins;
	}

	static vector<int> dp_bin(const int capacity, vector<int>& tasks){

		vector<int> temp;
		if(tasks.empty())
			return temp;

		std::sort(tasks.rbegin(), tasks.rend());

		if(tasks[0] == capacity){
			temp.push_back(tasks[0]);
			remove_item(tasks, capacity);
			return temp;
		}

		memset(dp, 0, sizeof(dp));
		for( int j=0; j<= capacity; j++){
			if(j < tasks[0]){
				dp[0][j] = 0;
			}
			else{
				dp[0][j] = tasks[0];
			}
		}

		for(size_t i=1; i< tasks.size(); i++){
			for( int j=0; j<= capacity; j++){
				const int days = tasks[i];
				if(j < days){
					dp[i][j] = dp[i-1][j];
				}
				else{
					dp[i][j] = std::max(dp[i-1][j], dp[i-1][j-days] + days);
				}
			}
		}

		// dump dp
		//		for(size_t i=0; i<= tasks.size(); i++){
		//			for( int j=0; j<=capacity; j++){
		//				cout<<dp[i][j]<<" ";
		//			}
		//			cout<<endl;
		//		}



		int max = 0;
		int index = 0;
		for(size_t i=0; i< tasks.size(); i++){
			if(dp[i][capacity] == capacity){
				index = i;
				break;
			}
			if (max < dp[i][capacity] ){
				max = dp[i][capacity];
				index = max;
			}
		}
		vector<int> used_bin;
		int j = capacity;
		for(int i = index; i>=0; i-- ){
			if(i == 0 && dp[i][j]!=0){
				used_bin.push_back(tasks[i]);
			}
			else if(dp[i][j] > dp[i-1][j]){ //used
				used_bin.push_back(tasks[i]);
				j = j - tasks[i];
			}
		}

		// remove used items
		for(auto& item:used_bin){
			//			cout<<item<<" ";
			remove_item(tasks, item);
		}
		//		cout<<endl;


		return used_bin;
	}

	static void remove_item(vector<int>& array, const int item){
		for(auto iter = array.begin(); iter != array.end();){
			if(*iter == item){
				iter = array.erase(iter);
				break;
			}
			else
				iter++;
		}
	}

	static BINS_TYPE get_dp(const int capacity, vector<int> tasks){
		std::sort(tasks.rbegin(), tasks.rend());
		vector<vector<int > > bins;
		while(!tasks.empty()){
			vector<int> used_bin = dp_bin(capacity, tasks);
			bins.push_back(used_bin);
		}
		//		cout<<bins.size()<<endl;
		//		for(size_t i=0; i<bins.size(); i++){
		//			for(size_t j=0; j<bins[i].size(); j++){
		//				cout<<bins[i][j]<<" ";
		//			}
		//			cout<<endl;
		//		}
		return bins;
	}

	static int get_lower_bound(const int capacity, vector<int> tasks){
		std::sort(tasks.rbegin(), tasks.rend());
		//		for(size_t i=0; i<tasks.size(); i++){
		//			cout<<tasks[i]<<" ";
		//		}
		double sum = 0;
		for(auto& item: tasks){
			sum += item;
		}


		double avg = sum/(double)capacity;
		int l1 = std::ceil(avg);

		// get lower bound plus must wasted space
		int wasted_unit = 0;
		int carried_over = 0;
		int bin_count = 0;
		while(!tasks.empty()){
			int first_value = tasks[0];
			remove_item(tasks, first_value);
			int left_cap = capacity - first_value;
			vector<int> mini_tasks;
			for(auto& item : tasks){
				if(item <= left_cap){
					mini_tasks.push_back(item);
				}
			}

			int used_sum = carried_over;
			for(auto& item:mini_tasks){
				used_sum += item;
				remove_item(tasks, item);
			}

			if(used_sum <= left_cap){
				wasted_unit += left_cap -used_sum;
				carried_over = 0;
			}
			else if(used_sum > left_cap){
				carried_over = used_sum - left_cap;
			}


			bin_count++;
			//			cout<<"current wasted unit: "<<wasted_unit<<" carried:"<<carried_over<<endl;
			//			for(auto&item:tasks){
			//				cout<<item<<" ";
			//			}
			//			cout<<endl;
		}
		if(carried_over > 0){
			if(carried_over%capacity != 0)
			{
				wasted_unit += capacity - carried_over%capacity;
							bin_count += carried_over/capacity;
			}
			bin_count++;

		}

		if(b_output){
			cout<<"carried over:"<<carried_over<<" bin count: "<<bin_count<<endl;
			cout<<"sum: "<<sum<<endl;
			cout<<"wasted sum:"<<wasted_unit+sum<<endl;
		}
		avg = (sum + (double)wasted_unit)/(double)capacity;
		int l2 = std::ceil(avg);

		int l = std::max(l1,l2);

		return l;
	}
};


int main(int argc, char* argv[]) {
	auto start = std::chrono::system_clock::now();
	//	cout << "!!!Bin Packing!!!" << endl; //  start of bin packing algorithm solution
	std::string input_file = "test_data.txt";
	if(argc >= 2){
		input_file = argv[1];
	}
	std::string output_file = "output.txt";
	if(argc >=3){
		output_file = argv[2];
	}

	if(argc >=4){
		int do_output = std::atoi(argv[3]);
		if(do_output>0){
			b_output = true;
		}
		else{
			b_output = false;
		}

	}

	int capacity = 100;
	int count = 0;
	vector<int> test_data;
	std::ifstream data_file;
	data_file.open(input_file.c_str());

	bool bformat = false;
	if(data_file.is_open()){
		std::string line;


		std::getline(data_file, line);
		stringstream(line)>>count;

		std::getline(data_file, line);
		stringstream(line)>>capacity;

		if(bformat){
			int temp = capacity;
			capacity = count;
			count = temp;
		}

		while(std::getline(data_file, line)){
			stringstream ss(line);
			int value = 0;
			ss>>value;
			if(value>0)
				test_data.push_back(value);
		}

	}

	data_file.close();

	int sum = 0;
	for(auto& item: test_data){
		sum += item;
	}

	//	std::cout<<"capacity: "<<capacity<<endl;

	LOWER_BOUND = BinPacking::get_lower_bound(capacity, test_data);
	if(b_output){
		std::cout<<"lower bound: "<<LOWER_BOUND<<endl;
	}
    for(int i=0; i<= LOWER_BOUND+10; i++){
    	vector<vector<int> > init_task;
    	bp_dominance.push_back(init_task);
    }
//	BINS_TYPE bins = BinPacking::get_dp(capacity, test_data);
//	BINS_TYPE bins = BinPacking::get_bfd(capacity, test_data);
	BINS_TYPE bins = BinPacking::get_ibc(capacity, test_data);
//	if(bins.size() > LOWER_BOUND){
//		BINS_TYPE fbc_bins = BinPacking::get_fbc(capacity, test_data);
//		if(fbc_bins.size()<bins.size()){
//			bins = fbc_bins;
//		}
//		if(bins.size() > LOWER_BOUND){
//			BINS_TYPE ibc_bins = BinPacking::get_ibc(capacity, test_data);
//			if(ibc_bins.size() <= bins.size()){
//				bins = ibc_bins;
//			}
//		}
//	}

	std::ofstream result(output_file.c_str());

	if(bins.size() > LOWER_BOUND){
		if(b_output){
			std::cout<<"Not accurate lower bound!"<<endl;
		}
	}

	int answer_sum = 0;
	cout<<bins.size()<<endl;
	result<<bins.size()<<endl;
//	b_output = true;
	for(size_t i=0; i<bins.size(); i++){
		for(size_t j=0; j<bins[i].size(); j++){
			if(b_output){
				cout<<bins[i][j]<<" ";
			}
			result<<bins[i][j]<<" ";
			answer_sum += bins[i][j];
		}
		if(b_output){
			cout<<endl;
		}
		result<<endl;
	}

	result.close();

	if(sum != answer_sum){
		cout<<"incorrect answer"<<endl;
	}

	auto time1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = time1 -start;
	if(b_output){
		std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second get the solution"<<endl;
	}

	return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))
    );

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}
int main1()
{
    ofstream fout("output.txt");

    std::string days_temp;
    getline(cin, days_temp);

    int days = std::stoi(ltrim(rtrim(days_temp)));

    string tasks_count_temp;
    getline(cin, tasks_count_temp);

    int tasks_count = stoi(ltrim(rtrim(tasks_count_temp)));

    vector<int> tasks(tasks_count);

    for (int i = 0; i < tasks_count; i++) {
        string tasks_item_temp;
        getline(cin, tasks_item_temp);

        int tasks_item = stoi(ltrim(rtrim(tasks_item_temp)));

        tasks[i] = tasks_item;
    }
    LOWER_BOUND = BinPacking::get_lower_bound(days, tasks);



     BINS_TYPE bins = BinPacking::get_ibc(days, tasks);
 	int result = bins.size();
 	// 	cout << result << "\n";
 	for(size_t i=0; i<bins.size(); i++){
 		for(size_t j=0; j<bins[i].size(); j++){

 			cout<<bins[i][j]<<" ";

 		}
 		cout<<endl;
 	}

    fout << result << "\n";

    fout.close();

    return 0;
}


