/* 046267 Computer Architecture - Spring 2021 - HW #2 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "cache.h"

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}
//cache_main C1=cache_main();

	L_mem L1;//=L_mem();
	L_mem L2;//=L_mem();
	L1.L_set(1,BSize,L1Assoc,L1Size,L1Cyc,WrAlloc);
	L2.L_set(2,BSize,L2Assoc,L2Size,L2Cyc,WrAlloc);
	//cache_set(L1,L2,Bsize,WrAlloc,MemCyc);
	
	int num_rows_1=(L1.L_size)/(L1.block_size*L1.ways_num);
	int num_rows_2=(L2.L_size)/(L2.block_size*L2.ways_num);

	while (getline(file, line))
	{

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		// DEBUG - remove this line
		cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		// DEBUG - remove this line
		cout << ", address (hex)" << cutAddress;

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);

		// DEBUG - remove this line
		cout << " (dec) " << num << endl;

	
	int tag_1 = get_tag(num, L1.block_size, num_rows_1);
	int set_1 = get_set(num, L1.block_size, num_rows_1);
	int tag_2 = get_tag(num, L2.block_size, num_rows_2);
	int set_2 = get_set(num, L2.block_size, num_rows_2);


	
///*******  all of the above is done  *******///

//what is the object of the cache itselt? do we even create it?
//because now we are calling only L1, L2 without creating a cache object

	
		
		if (operation == 'r') //read mode// sapir
		{
			
			if (was_found(L1,tag_1,set_1)) //L1
			{
				
				L1.hit_num++;
				L1.call_lru(tag_1,set_1);

			}
			else
				{//was not found in L1
				
				L1.miss_num++;
				if (was_found(L2,tag_2,set_2)) //L2
					{
					L2.hit_num++;
					L2.call_lru(tag_2,set_2);//TODO: this
					change_tag(L1,tag_1,set_1);
					//bring the block to l1 change_tag(L1)
					//TODO add change_tag_l1
					}
				else //was not found in L2
					{
				
					L2.miss_num++;
					change_tag(L2,tag_2,set_2);
					change_tag(L1,tag_1,set_1);
					}//end of was not found in L2
				}//end of was not found in l1
			
		}
		
		else //write mode// yuval
		{
			if (was_found(L1,tag_1,set_1)) //L1
			{
				L1.hit_num++;
				L1.call_lru(tag_1,set_1);
				make_dirty(L1, tag_1,set_1);
			}
			else 
			{
				if(was_found(L2,tag_2,set_2)) //L2
				{
					L1.miss_num++;
					L2.hit_num++;
					L2.call_lru(tag_2,set_2);
					//wralloc
					if (WrAlloc)
					{	//TODO: this
						change_tag(L1,tag_1,set_1);//update l1 with info from l2 , if neccesary evict from l1 and update lru
						make_dirty(L1, tag_1,set_1);	
					}
					
					//no_wralloc
					else
					{
						make_dirty(L2, tag_2,set_2);
					}
				}
				else
				{
					L1.miss_num++;
					L2.miss_num++;
				
				
					if (WrAlloc)
						{
							//change tag or make dirty first?
							//also change l1 or change l2 
							change_tag(L2,tag_2,set_2);
							change_tag(L1,tag_1,set_1);
							make_dirty(L1, tag_1,set_1);
						}
				}
			}
		}
		printf("L1.miss_num%d \n",L1.miss_num);
		printf("L2.miss_num%d \n",L2.miss_num);
		printf("L1.hit_num%d \n",L1.hit_num);
		printf("L2.hit_num%d \n",L2.hit_num);
	}

///*******  all of the below is done  *******///
	double L1MissRate;
	double L2MissRate;
	double avgAccTime;
	double sum;
	/*
	printf("L1.miss_num%d \n",L1.miss_num);
	printf("L2.miss_num%d \n",L2.miss_num);
	printf("L1.hit_num%d \n",L1.hit_num);
	printf("L2.hit_num%d \n",L2.hit_num);
*/
	///TODO: casting
	L1MissRate = (double)L1.miss_num/(double)(L1.miss_num+L1.hit_num);
	L2MissRate = (double)L2.miss_num/(double)(L2.miss_num+L2.hit_num);
	

	

	sum = L1.cycle_time*L1.hit_num;
	sum = sum + L1.cycle_time*L1.miss_num + L2.cycle_time*L2.hit_num;
	sum = sum + (L2.cycle_time+MemCyc)*L2.miss_num;
	
	avgAccTime = (double)sum/(double)(L1.miss_num+L1.hit_num);
	

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}
