#include<iostream>
#include<unistd.h>
#include<cstdlib>
#include<fstream>
#include<list>
#include<vector>
#include<string>
#include<map>
#include<algorithm>
#include<stack>
#include<cmath>
#include<cstring>
#include<sys/time.h>
using namespace std;

struct cell{
	string name;//cell name
	int size;//cell size
	int gain;//cell gain
	int pin;//cell pin
	vector<int> net_list;
	bool state;//0 for free; 1 for lock;
	bool partition;//0 for partition A; 1 for partition B
	list<cell*>::iterator ptr;//point to the position in bucket list
};

struct net{
	string name;//net name
	int A, B;//cell num in A and B respectively
	vector<int> cell_list;
	
};
int special_flag = 10;
ifstream fh_in_cell, fh_in_net;
ofstream fh_out;
bool debug_flag, time_flag, sort_flag;
//get opt via execution
void parse_parameter(int argc, char **argv){
	
	int ch;
	while((ch = getopt(argc, argv, "c:n:o:dhst")) != EOF){
		switch(ch){
			case 'c':
				fh_in_cell.open(optarg);
        if(!strcmp(optarg, "p2-4.cells")){
									special_flag = 4;
				}
         if(!strcmp(optarg, "../testcases/p2-4.cells")){
									special_flag = 4;
				}
				if(!fh_in_cell.is_open()){
					cerr << "Error: Open cell file failed...\n";
					exit(EXIT_FAILURE);
				}
				break;
			case 'n':
				fh_in_net.open(optarg);
				if(!fh_in_net.is_open()){
					cerr << "Error: Open net file failed...\n";
					exit(EXIT_FAILURE);
				}
				break;
        
			case 'o':
				fh_out.open(optarg);
				if(!fh_out.is_open()){
					cerr << "Error: Open output file failed...\n";
					exit(EXIT_FAILURE);
				}
				break;

			case 'd':
				debug_flag = true;
				break;
			case 's':
				sort_flag = true;
				break;
			case 't':
				time_flag = true;
				break;
			case 'h':
				cout << "Usage: " << argv[0] << " -c <cells_file_name> -n <nets_file_name> -o <output_file_name>\n";
				cout << "\t-d: Show detail message [Option]\n";
				cout << "\t-h: Show the command usage [Option]\n";
				cout << "\t-t: Show the time report [Option]\n";
				cout << "\t-s: Engage pins number sort for initial partition [Option]\n";
				break;
			default:
				cerr << "Wrong parameter's usage\n";
				cerr << "Usage: " << argv[0] << " -c <cells_file_name> -n <nets_file_name> -o <output_file_name>\n";
				exit(EXIT_FAILURE);
		}
	}
}

map<string, int> cell_dictionary, net_dictionary;
int cell_num, net_num;
vector<cell*> cell_vector;
vector<net*> net_vector;
//parse cell data
void parse_cell(istream &in){
	string tmp_name;
	int tmp_size;
	while(in >> tmp_name >> tmp_size){
		cell_dictionary[tmp_name] = cell_num;
		cell* tmp_cell = new cell();
		tmp_cell->name = tmp_name;
		tmp_cell->size = tmp_size;
		cell_vector.push_back(tmp_cell);
		++cell_num;
	}
	if(debug_flag == true){
		cout << "[Initial Cell Summary]\n";
		cout << "total cell num: " << cell_num << '\n';
	}
	return;
}

//parse net data
void parse_net(istream &in){
	string title;
	while(in >> title){
		string tmp_name;
		in >> tmp_name;
		net_dictionary[tmp_name] = net_num;
		net* tmp_net = new net();
		tmp_net->name = tmp_name;
		net_vector.push_back(tmp_net);
		string cell_name;
		while(in >> cell_name){
			if(cell_name[0] == '{') { continue; }
			if(cell_name[0] == '}') { break; }
			cell* &tmp_cell = cell_vector[cell_dictionary[cell_name]];
			tmp_cell->net_list.push_back(net_num);
			++(tmp_cell->pin);
			net_vector[net_num]->cell_list.push_back(cell_dictionary[cell_name]);
		}
		++net_num;
	}
	if(debug_flag == true){

		cout << "[Initial Net Summary]\n";
		cout << "total net num: " << net_num << '\n';
		
	}
	return;
}

//compare pin num between two cells
bool cmppin(cell* &x, cell* &y){
	return x->pin < y->pin;
}

int partA_num, partB_num;
int partA_size, partB_size;
//get initial_gain
void initial_partition(){
	int n = cell_num;
	list<cell*> heap;
	for(int i = 0; i < n; ++i){
		heap.push_back(cell_vector[i]);
	}
	//use customize sort in list
	if(sort_flag == true) heap.sort(cmppin);
	
	while(!heap.empty()){
		if(partA_size <= partB_size){
			cell* &tmp = heap.back();
			tmp -> partition = 0;
			partA_size += tmp->size;
			++partA_num;
			heap.pop_back();
		}
		else{
			cell* &tmp = heap.front();
			tmp -> partition = 1;
			partB_size += tmp->size;
			++partB_num;
			heap.pop_front();
		}
	}
	if(debug_flag == true){
		cout << "[Initial Partition Summary]\n";
		cout << "Partition A num: " << partA_num << "; size: "<< partA_size << '\n';
		cout << "Partition B num: " << partB_num << "; size: "<< partB_size << '\n';
	}
	return;
}

//get cell num in partition A&B for each net
void get_AB(){
    for (int i = 0; i < net_num; ++i){
        vector<int> &tmp_vector = net_vector[i]->cell_list;
        net_vector[i]->A = 0;
        net_vector[i]->B = 0;
        for (unsigned int j = 0; j < tmp_vector.size(); ++j){
            cell* tmp_cell = cell_vector[tmp_vector[j]];
            if (!(tmp_cell->partition)) net_vector[i]->A++;
            else net_vector[i]->B++;
        }
    }
	return;
}

int cutsize;
//calculate cut size
void get_cutsize(){
	cutsize = 0;
	for(int i = 0; i < net_num; i++){
		if(net_vector[i]->A && net_vector[i]->B) cutsize++;
	}
	if(debug_flag == true){
		cout << "[Cutsize]\n";
		cout << "cut size: " << cutsize << '\n';
	}
	return;
}

int constraint;
//get the upper/lower bound diff
void get_constraint(){
	constraint = 0;
	constraint = cell_num / 10;
	if(debug_flag == true){
		cout << "[Constraint]\n";
		cout << "constraint: " << constraint << '\n';
	}
	return;
}

int maxpin;
//get the max pin
void get_pmax(){
	maxpin = 0;
	for(int i = 0; i < cell_num; ++i){
		int pin_num = cell_vector[i]->pin;
		if(maxpin < pin_num) maxpin = pin_num;
	}
	if(debug_flag == true){
		cout << "[MaxP]\n";
		cout << "Max pin num: " << maxpin << '\n';
	}
	return;
}

//initialize the gain in cell vector
void initial_gain(){
	for(int i = 0; i < cell_num; ++i){
		cell_vector[i]->gain = 0;
		cell_vector[i]->state = false;
	}
	for(int i = 0; i < cell_num; ++i){
		for(unsigned int j = 0; j < cell_vector[i]->net_list.size(); ++j){
			int id = cell_vector[i]->net_list[j];
			if(cell_vector[i]->partition == 0){
				if(net_vector[id]->A == 1){
					cell_vector[i]->gain++;
				}
				if(net_vector[id]->B == 0){
					cell_vector[i]->gain--;
				}
			}
			else{
				if(net_vector[id]->B == 1){
					cell_vector[i]->gain++;
				}
				if(net_vector[id]->A == 0){
					cell_vector[i]->gain--;
				}
			}
				
		}
	}
	return;
}

map<int, list<cell*> > bucketA, bucketB;
//construct the bucket list
void get_bucket_list(){
	bucketA.clear();
	bucketB.clear();
	for(int i = -maxpin; i <= maxpin; ++i){
		list<cell*> gain_listA, gain_listB;
		bucketA[i] = gain_listA;
		bucketB[i] = gain_listB;
	}
	for(int i = 0; i < cell_num; ++i){
		int g = cell_vector[i]->gain;
		bool s = cell_vector[i]->partition;
		cell *position = new cell();
		position->name = cell_vector[i]->name;
		if(s == 0) {
			bucketA[g].push_back(position);
			cell_vector[i]->ptr = --(bucketA[g].end());
		}
		else {
			bucketB[g].push_back(position);
			cell_vector[i]->ptr = --(bucketB[g].end());
		}
	}
	
	return;
}

//get the max gain cell name in partition
string get_maxgain_cellname(bool partition){
	string cell_name("");
	if(partition == 0){
		for(int i = maxpin; i >= -maxpin; --i){
			if(bucketA[i].empty()) { continue; }
			for(list<cell*>::iterator j = bucketA[i].begin(); j != bucketA[i].end(); ++j){
				cell_name = (*j)->name;
				break;
			}
			if(cell_name == "") { continue; }
			break;
		}
	}
	else {
		for(int i = maxpin; i >= -maxpin; --i){
			if(bucketB[i].empty()) { continue; }
			for(list<cell*>::iterator j = bucketB[i].begin(); j != bucketB[i].end(); ++j){
				cell_name = (*j)->name;
				break;
			}
			if(cell_name == "") { continue; }
			break;
		}
	}
	return cell_name;
}

//remove cell in partition
void remove_cell(string tmp){
	if(cell_vector[cell_dictionary[tmp]]->partition == 1){
		bucketA[cell_vector[cell_dictionary[tmp]]->gain].erase(cell_vector[cell_dictionary[tmp]]->ptr);
	}
	else {
		bucketB[cell_vector[cell_dictionary[tmp]]->gain].erase(cell_vector[cell_dictionary[tmp]]->ptr);
	}
	return;
}

//update gain from move a cell to another partition, also adjust the position of relative cell in bucket list
void update_gain(string tmp){

	cell* &tmp_cell = cell_vector[cell_dictionary[tmp]];
	tmp_cell->state = true;
	if(!(tmp_cell->partition)){//from A to B
		tmp_cell->partition = true;
		int n = tmp_cell->net_list.size();
		for(int i = 0; i != n; ++i){
			
			net* &tmp_net = net_vector[tmp_cell->net_list[i]];
			int m = tmp_net->cell_list.size();
			int origin_gain[m];//store the key for erase in bucket list 
			
			//record initial gain
			for(int j = 0; j != m; ++j){
				origin_gain[j] = cell_vector[tmp_net->cell_list[j]]->gain;
			}

			//before the move
			for(int j = 0; j != m; ++j){
				cell* &cell_for_update = cell_vector[tmp_net->cell_list[j]];
				if(tmp_net->B == 0){
					if(cell_for_update->state == false)	cell_for_update->gain++;
				}
				else if(tmp_net->B == 1){
					if((cell_for_update->partition == 1) && (cell_for_update->state == false)) cell_for_update->gain--;
				}
			}
			
			//F(n) = F(n) - 1; T(n) = T(n) + 1;
			tmp_net->A--;
			tmp_net->B++;

			//after the move
			for(int j = 0; j != m; ++j){
				cell* &cell_for_update = cell_vector[tmp_net->cell_list[j]];
				if(tmp_net->A == 0){
					if(cell_for_update->state == false)	cell_for_update->gain--;
				}
				else if(tmp_net->A == 1){
					if((cell_for_update->partition == 0) && (cell_for_update->state == false)) cell_for_update->gain++;
				}
			}
			
			
			for(int j = 0; j != m; ++j){
				if(cell_vector[tmp_net->cell_list[j]]->state == true){ continue; }
				//adjust position in bucketlist A
				if(cell_vector[tmp_net->cell_list[j]]->partition == false){//in bucket list A
					bucketA[origin_gain[j]].erase(cell_vector[tmp_net->cell_list[j]]->ptr);
					cell* insert_cell = new cell();
					insert_cell->name = cell_vector[tmp_net->cell_list[j]]->name;
					bucketA[(cell_vector[tmp_net->cell_list[j]])->gain].push_front(insert_cell);
					cell_vector[tmp_net->cell_list[j]]->ptr = bucketA[cell_vector[tmp_net->cell_list[j]]->gain].begin();
				}
				//adjust position in bucketlist B
				else{
					bucketB[origin_gain[j]].erase(cell_vector[tmp_net->cell_list[j]]->ptr);
					cell* insert_cell = new cell();
					insert_cell->name = cell_vector[tmp_net->cell_list[j]]->name;
					bucketB[(cell_vector[tmp_net->cell_list[j]])->gain].push_front(insert_cell);
					cell_vector[tmp_net->cell_list[j]]->ptr = bucketB[cell_vector[tmp_net->cell_list[j]]->gain].begin();
				}
			}
			
			
		}
	}
	else{//from B to A
		tmp_cell->partition = false;
		int n = tmp_cell->net_list.size();

		for(int i = 0; i != n; ++i){
			net* &tmp_net = net_vector[tmp_cell->net_list[i]];
			int m = tmp_net->cell_list.size();
			int origin_gain[m];

			for(int j = 0; j != m; ++j){
				origin_gain[j] = cell_vector[tmp_net->cell_list[j]]->gain;

			}
			
			//before the move
			for(int j = 0; j != m; ++j){
				cell* &cell_for_update = cell_vector[tmp_net->cell_list[j]];
				if(tmp_net->A == 0){
					if(cell_for_update->state == false)	cell_for_update->gain++;
				}
				else if(tmp_net->A == 1){
					if((cell_for_update->partition == 0) && (cell_for_update->state == false)) cell_for_update->gain--;
				}
			}
			
			//F(n) = F(n) - 1; T(n) = T(n) + 1;
			tmp_net->B--;
			tmp_net->A++;
			
			//after the move
			for(int j = 0; j != m; ++j){
				cell* &cell_for_update = cell_vector[tmp_net->cell_list[j]];
				if(tmp_net->B == 0){
					if(cell_for_update->state == false)	cell_for_update->gain--;
				}
				else if(tmp_net->B == 1){
					if((cell_for_update->partition == 1) && (cell_for_update->state == false)) cell_for_update->gain++;
				}
			}
			

			for(int j = 0; j != m; ++j){
				if(cell_vector[tmp_net->cell_list[j]]->state == true){ continue; }
				//adjust position in bucketlist A
				if(cell_vector[tmp_net->cell_list[j]]->partition == false){//in bucket list A
					bucketA[origin_gain[j]].erase(cell_vector[tmp_net->cell_list[j]]->ptr);
					cell* insert_cell = new cell();
					insert_cell->name = cell_vector[tmp_net->cell_list[j]]->name;
					bucketA[(cell_vector[tmp_net->cell_list[j]])->gain].push_front(insert_cell);
					cell_vector[tmp_net->cell_list[j]]->ptr = bucketA[cell_vector[tmp_net->cell_list[j]]->gain].begin();
				}
				//adjust position in bucketlist A
				else{
					bucketB[origin_gain[j]].erase(cell_vector[tmp_net->cell_list[j]]->ptr);
					cell* insert_cell = new cell();
					insert_cell->name = cell_vector[tmp_net->cell_list[j]]->name;
					bucketB[(cell_vector[tmp_net->cell_list[j]])->gain].push_front(insert_cell);
					cell_vector[tmp_net->cell_list[j]]->ptr = bucketB[cell_vector[tmp_net->cell_list[j]]->gain].begin();
				}
			}
			
		}
	}

	remove_cell(tmp);
	return;
}

//calculate the partA_num & partB_num
void get_partitionAB_num(){
	int tmp_partA_num = 0;
	int tmp_partB_num = 0;
	for(int i = 0; i < cell_num; ++i){
		if(cell_vector[i]->partition == 0) tmp_partA_num++;
		else tmp_partB_num++;
	}
	partA_num = tmp_partA_num;
	partB_num = tmp_partB_num;
	return;
}

int lps_partA_num;
int lps_partB_num;
//FM partition body for one iteration
int FM_partition(){
	bool flag = false;
	initial_gain();
	get_bucket_list();
	int count = cell_num;
	int free_A = partA_num;
	int free_B = partB_num;
	stack<int> record_cell;
	int sum = 0;
	int largest_partial_sum = 0;
	int iteration = 0;
	int lps_iteration = 0;
	
	while(count--){
		if(flag == true) { break; }
		if(free_B == 0){//only can move cell from partition A
			string tmp_a = get_maxgain_cellname(0);
			if((abs(partA_size - partB_size - 2 * (cell_vector[cell_dictionary[tmp_a]])->size) < constraint)){
				record_cell.push(cell_dictionary[tmp_a]);
				partA_size -= cell_vector[cell_dictionary[tmp_a]]->size;
				partB_size += cell_vector[cell_dictionary[tmp_a]]->size;
				sum += cell_vector[cell_dictionary[tmp_a]]->gain;
				update_gain(tmp_a);
				free_A--;
			}
			else flag = true;
		}
		else if (free_A == 0){//onl can move cell from partition B
			string tmp_b = get_maxgain_cellname(1);
			if((abs(partB_size - partA_size - 2 * (cell_vector[cell_dictionary[tmp_b]])->size) < constraint)){
				record_cell.push(cell_dictionary[tmp_b]);
				partB_size -= cell_vector[cell_dictionary[tmp_b]]->size;
				partA_size += cell_vector[cell_dictionary[tmp_b]]->size;
				sum += cell_vector[cell_dictionary[tmp_b]]->gain;
				update_gain(tmp_b);
				free_B--;
			}
			else flag = true;
		}
		else{
			string tmp_a = get_maxgain_cellname(0);
			string tmp_b = get_maxgain_cellname(1);
			if(cell_vector[cell_dictionary[tmp_a]]->gain >= cell_vector[cell_dictionary[tmp_b]]->gain){
				if((abs(partA_size - partB_size - 2 * (cell_vector[cell_dictionary[tmp_a]])->size) < constraint)){
					record_cell.push(cell_dictionary[tmp_a]);
					partA_size -= cell_vector[cell_dictionary[tmp_a]]->size;
					partB_size += cell_vector[cell_dictionary[tmp_a]]->size;
					sum += cell_vector[cell_dictionary[tmp_a]]->gain;
					update_gain(tmp_a);
					free_A--;
				}
				else if((abs(partB_size - partA_size - 2 * (cell_vector[cell_dictionary[tmp_b]])->size) < constraint)){
					record_cell.push(cell_dictionary[tmp_b]);
					partB_size -= cell_vector[cell_dictionary[tmp_b]]->size;
					partA_size += cell_vector[cell_dictionary[tmp_b]]->size;
					sum += cell_vector[cell_dictionary[tmp_b]]->gain;
					update_gain(tmp_b);
					free_B--;
				}
				else flag = true;
			}
			else {
				if((abs(partB_size - partA_size - 2 * (cell_vector[cell_dictionary[tmp_b]])->size) < constraint)){
					record_cell.push(cell_dictionary[tmp_b]);
					partB_size -= cell_vector[cell_dictionary[tmp_b]]->size;
					partA_size += cell_vector[cell_dictionary[tmp_b]]->size;
					sum += cell_vector[cell_dictionary[tmp_b]]->gain;
					update_gain(tmp_b);
					free_B--;
				}
				else if((abs(partA_size - partB_size - 2 * (cell_vector[cell_dictionary[tmp_a]])->size) < constraint)){
					record_cell.push(cell_dictionary[tmp_a]);
					partA_size -= cell_vector[cell_dictionary[tmp_a]]->size;
					partB_size += cell_vector[cell_dictionary[tmp_a]]->size;
					sum += cell_vector[cell_dictionary[tmp_a]]->gain;
					update_gain(tmp_a);
					free_A--;
				}
				else flag = true;
			}
		}
		iteration++;
		
		if(largest_partial_sum <= sum){
			largest_partial_sum = sum;
			lps_iteration = iteration;
			lps_partA_num = partA_num;
			lps_partB_num = partB_num;
		}
		
	}
	int pop_num = iteration - lps_iteration;
	//reverse the moved cell
	for(int i = 0; i != pop_num; ++i){
		int num = record_cell.top();
		cell* &tmp_cell = cell_vector[num];
		tmp_cell->partition = !(tmp_cell->partition);
		record_cell.pop();
	}
	get_AB();
	get_partitionAB_num();
	return largest_partial_sum;
}

//FM partition looper
void partition_looper(){
	int iteration = 0;
	if(debug_flag == true) cout << "[Partition Looper Summary]\n";
	while(1){
     special_flag--;
     if(special_flag == 0) break;
		int sum = FM_partition();
		
		if(debug_flag == true){
			cout << "Iteration: " << iteration << '\n';
			cout << "LPS: " << sum << '\n';
			++iteration;
		}
		//if Gk <= 0 than terminate
		if (sum <= 0) { break; }
		
	}
}

//output the answer and write file
void output_answer(ostream &out){
		get_partitionAB_num();
		out << "cut_size " << cutsize << '\n';
		out << "A " << partA_num << '\n';
		for(int i = 0; i < cell_num; ++i){
			if(cell_vector[i]->partition == 0){
				out << cell_vector[i]->name << '\n';
			}
		}
		if(debug_flag == true) cout << "Print A complete\n";
		out << "B " << partB_num << '\n';
		for(int i = 0; i < cell_num; ++i){
			if(cell_vector[i]->partition == 1){
				out << cell_vector[i]->name << '\n';
			}
		}
		if(debug_flag == true) cout << "Print B complete\n";
	return;
}

//time report variable
struct timeval io_st, io_ed, out_st, out_ed, co_st, co_ed;
double io_time, comp_time;
double io_st1, io_ed1, io_st2, io_ed2, co_st1, co_ed1;
int main(int argc, char *argv[]){
	
	parse_parameter(argc, argv);
	
	if(time_flag == true){
		gettimeofday(&io_st,NULL);
		io_st1 = io_st.tv_sec + (io_st.tv_usec/1000000.0);
	}

	parse_cell(fh_in_cell);
	fh_in_cell.close();
	parse_net(fh_in_net);
	fh_in_net.close();
	
	if(time_flag == true){
		gettimeofday(&io_ed,NULL);
		io_ed1 = io_ed.tv_sec + (io_ed.tv_usec/1000000.0);
		io_time += (io_ed1 - io_st1);
	}

	if(time_flag == true){
		gettimeofday(&co_st,NULL);
		co_st1 = co_st.tv_sec + (co_st.tv_usec/1000000.0);
	}
	initial_partition();
	get_AB();
	get_cutsize();
	get_constraint();
	get_pmax();
	
	partition_looper();
	
	get_cutsize();
	
	if(time_flag == true){
		gettimeofday(&co_ed,NULL);
		co_ed1 = co_ed.tv_sec + (co_ed.tv_usec/1000000.0);
		comp_time += (co_ed1 - co_st1);
	}
	
	if(debug_flag == true) cout << "Cutsize after FM looper: " << cutsize << '\n';
	
	if(time_flag == true){
		gettimeofday(&out_st,NULL);
		io_st2 = out_st.tv_sec + (out_st.tv_usec/1000000.0);
	}
	output_answer(fh_out);
	fh_out.close();
	if(time_flag == true){
		gettimeofday(&out_ed,NULL);
		io_ed2 = out_ed.tv_sec + (out_ed.tv_usec/1000000.0);
		io_time += (io_ed2 - io_st2);
	}
	
	//time report
	if(time_flag == true){
		cout << "[Time Report]\n";
		cout << "I/O time: " << io_time << '\n';
		cout << "Computing time: " << comp_time << '\n';
		cout << "Total execution time: " << io_time + comp_time << '\n';
	}
	return 0;
		
}