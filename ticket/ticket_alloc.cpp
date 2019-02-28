//============================================================================
// Name        : ticket_alloc.cpp
// Author      : phill
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <vector>
#include <iostream>
#include <unordered_set>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <set>
#include <fstream>
#include <map>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <sstream>

#define MAX_STATION 11
#define TOTAL_TICKET 50000

using namespace std;
struct ticket_info
{
	int start = -1;
	int end = -1;
	uint32_t num = 0;
};

std::vector<int> SEAT_LAYOUT[TOTAL_TICKET];

uint32_t seat_lsb[TOTAL_TICKET] = {0};

int get_random()
{
	return std::rand()%MAX_STATION;
}

uint32_t get_lowest_1(uint32_t num)
{
	if(num == 0)
		return num;
	return num^(num&(num-1));
}

std::vector<ticket_info> read_tickets(std::string& file_name)
		{
	std::vector<ticket_info> new_tickets;
	ifstream data_file;
	data_file.open(file_name.c_str());
	if(data_file.is_open())
	{
		std::string line;
		while(std::getline(data_file, line))
		{
			vector<string> ticketstr;
			boost::split(ticketstr, line, boost::is_any_of(","));


			int count = 0;
			std::stringstream(ticketstr[2])>>count;
			for(int i=0; i<count; i++)
			{
				ticket_info ticket;
				std::stringstream(ticketstr[0])>>ticket.start;
				ticket.start--;
				std::stringstream(ticketstr[1])>>ticket.end;
				ticket.end = ticket.end-2;
				new_tickets.push_back(ticket);
			}
		}
	}

	return new_tickets;
		}

int generate_ticket(std::vector<ticket_info>& tickets)
{
	const int generated_ticket = 1000;
	std::vector<ticket_info> new_tickets;

	int ticket_count = 0;

	for(int i=0; i<TOTAL_TICKET && ticket_count<generated_ticket; i++)
	{
		std::set<int> random_range ;
		int random_count = get_random();
		for(int j=0; j<random_count; j++)
		{
			random_range.insert(get_random());
		}

		for(auto iter = random_range.begin(); iter!= random_range.end(); iter++)
		{
			ticket_info ticket;
			ticket.start = *iter;
			iter++;
			if(iter == random_range.end())
				break;
			ticket.end = *iter ;
			ticket.num = 1;
			tickets.push_back(ticket);
			ticket_count++;

		}

	}

	std::ofstream test_file;
	std::string filename = __func__;
	filename +=".txt";
	test_file.open(filename.c_str(), std::fstream::trunc);
	//	test_file<<MAX_STATION+1<<endl;

	std::cout<<"Generated tickets number: "<<tickets.size()<<endl;
	for(auto& ticket: tickets)
	{
//		std::cout<<ticket.start+1<<","<<ticket.end+2<<","<<ticket.num<<std::endl;
		test_file<<ticket.start+1<<","<<ticket.end+2<<","<<ticket.num<<std::endl;
	}
	test_file.close();
	return 0;
}

std::vector<uint32_t> allocated_box;
int seats_count[MAX_STATION] = {0};

int get_max_seat(std::vector<ticket_info>& tickets)
{
	auto start = std::chrono::system_clock::now();
	for(auto& ticket:tickets)
	{
		int start_bits = ticket.start;
		int end_bits = ticket.end;
		for(int i=start_bits; i<=end_bits; i++)
		{
			seats_count[i]++;
		}
	}

	int max_seat = 0;
	for(int i=0; i<MAX_STATION; i++)
	{
		if(max_seat< seats_count[i])
			max_seat = seats_count[i];
	}

	std::cout<<"seat number: "<< max_seat<<endl;
	auto time1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = time1 -start;
	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for get seat number!"<<endl;

	return max_seat;
}

std::vector<ticket_info> allocate_ticket(std::vector<ticket_info>& tickets)
{
	auto start = std::chrono::system_clock::now();

	std::multimap<uint32_t, ticket_info*, std::greater<uint32_t> > bit_ticket;
	for(auto& ticket:tickets)
	{
		int start_bits = ticket.start;
		int end_bits = ticket.end;

		int bit_value = 0;
		for(int i=start_bits; i<=end_bits; i++)
		{
			bit_value |= 1<<(31-i);
		}
		bit_ticket.insert(std::make_pair(bit_value, &ticket));
	}

	auto time1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = time1 -start;
//	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for get the build  multi map"<<endl;

	start = time1;

	std::vector<ticket_info> new_tickets;

	allocated_box.clear();

	bool bnormal = false;

	for(auto item:bit_ticket)
	{
		bool new_line = true;
		int insert_line = 0;
		int insert_value = 0;
		uint32_t max_lsb = 0x80000000;
		for(size_t i=0; i<allocated_box.size(); i++)
		{
			if(item.first < seat_lsb[i])
			{
				if(bnormal)
				{
					allocated_box[i] |= item.first;
					new_line = false;
					item.second->num = i;
					new_tickets.push_back(*item.second);
					seat_lsb[i] = get_lowest_1(item.first);
					break;
				}
				else
				{
					new_line = false;
					uint32_t lsb = get_lowest_1(allocated_box[i]);
					if(lsb<max_lsb)
					{
						max_lsb = lsb;
						insert_value = item.first;
						insert_line = i;
					}
				}
			}
		}

		if(!new_line&&!bnormal)
		{
			allocated_box[insert_line] |= insert_value;
			item.second->num = insert_line;
			new_tickets.push_back(*item.second);
			seat_lsb[insert_line] = get_lowest_1(insert_value);
		}

		if(new_line)
		{

			item.second->num = allocated_box.size();
			new_tickets.push_back(*item.second);
			allocated_box.push_back(item.first);
			seat_lsb[item.second->num] = get_lowest_1(item.first);
		}
	}

	time1 = std::chrono::system_clock::now();
	elapsed_seconds = time1 -start;
	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for matching!"<<endl;
	return new_tickets;
}


int main(int argc, char* argv[])
{
	cout << "!!!Ticket allocation!!!" << endl;
	std::string filename = "test_data.txt";
	if(argc == 2)
	{
		filename = argv[1];
	}
	std::vector<ticket_info> gen_tickets;
	generate_ticket(gen_tickets);
	std::vector<ticket_info> tickets = read_tickets(filename);




	std::cout<<"allocated tickets!"<<endl;
	auto start = std::chrono::system_clock::now();
	get_max_seat(tickets);
	std::vector<ticket_info> new_tickets = allocate_ticket(tickets);

	auto time1 = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = time1 -start;
	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for calculate  tickets"<<endl;

//	for(auto ticket:new_tickets)
//	{
//		std::cout<<ticket.start+1<<","<<ticket.end+2<<","<<ticket.num<<std::endl;
//	}

//	std::cout<<"seat number: "<<allocated_box.size()<<std::endl;
//	for(int i=0; i<MAX_STATION; i++)
//	{
//
//		std::cout<<std::setw(4)<<i+1;
//	}
//	std::cout<<std::endl;
//
//
//	for(auto& item: allocated_box)
//	{
//		for(int i=0; i<MAX_STATION; i++)
//		{
//			if(item&(1<<(31-i)))
//				std::cout<<std::setw(4)<<"1";
//			else
//				std::cout<<std::setw(4)<<"0";
//		}
//		std::cout<<endl;
//	}

	for(size_t i=0; i<allocated_box.size(); i++)
	{
		SEAT_LAYOUT[i].resize(MAX_STATION, 0);
	}

	for(auto ticket:new_tickets)
	{
		for(int i=ticket.start; i<=ticket.end; i++)
		{
			SEAT_LAYOUT[ticket.num][i] = 8;
		}
		SEAT_LAYOUT[ticket.num][ticket.start] = 6;
		SEAT_LAYOUT[ticket.num][ticket.end] = 9;
	}



	std::cout<<setw(4)<<"";
	for(int i=0; i<MAX_STATION; i++)
	{

		std::cout<<std::setw(3)<<i+2<<" ";
	}
	std::cout<<std::endl;
	for(size_t i=0; i<allocated_box.size(); i++)
	{
		std::cout<<setw(4)<<i+1;
		for(int j=0; j<MAX_STATION; j++)
		{
			if(SEAT_LAYOUT[i][j] == 6)
				std::cout<<std::setw(4)<<"[1 ";
			if(SEAT_LAYOUT[i][j] == 9)
				std::cout<<std::setw(4)<<"1]";
			if(SEAT_LAYOUT[i][j] == 8)
				std::cout<<std::setw(4)<<"1 ";
			if(SEAT_LAYOUT[i][j] == 0)
				std::cout<<std::setw(4)<<"0 ";
		}

		std::cout<<endl;
	}
	std::cout<<"seat number: "<<allocated_box.size()<<std::endl;
	std::cout<<std::fixed<<std::setprecision(6)<<"It takes  "<<elapsed_seconds.count() <<" second for calculate tickets"<<endl;

	cout << "!!!End of test program!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
