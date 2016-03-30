#ifndef TRIE_H
#define TRIE_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <iterator>
#include <cstdlib>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unordered_map>
#include <set>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <unordered_set>

#define TRIE_WIDTH 255

std::vector< std::vector< std::string >> entities;

int lmin, lmax;
int min_2(int a ,int b) { return (a<b) ? a : b;}
int max_2(int a, int b) { return (a>b) ? a : b;}

int threshold = 0;

struct trie_node {
    int depth;
    char chr;
    struct trie_node* next[TRIE_WIDTH];
    int type; //root_node:0, leaf:1, other:-1
   std::vector<int> i_list;
};
trie_node nodes[1000000];
int allocp = 0;
trie_node* root_node = (trie_node*)malloc(sizeof(trie_node));
std::vector< trie_node* > can_nodes;

trie_node* init_node() {
    trie_node* p = &nodes[allocp++];
    p->depth = 0;
    for (int i=0; i<TRIE_WIDTH; i++) {
        p->next[i] = NULL;
    }
    return p;
}

const int SUCCESS = 0;
const int FAILURE = 1;

class Trie {
public:
    Trie(){};
    ~Trie(){};

    int createIndex(std::vector< std::vector< std::string >> tags);
    int trieED(const char *document, std::vector<int> &result);
};


bool insert_segment(trie_node* p,std::string s, int line) {
    //fprintf(stdout, "insert %6s, line %d\n", s.c_str(), line);
    const char* s_char = s.c_str();
    char first_char = s_char[0];
    //insert a leaf
    if (s.length() == 1) {
        //no such leaf
        if (p->next[(int)first_char] == NULL) {
            ////printf("no such leaf\n");
            p->next[(int)first_char] = init_node();
            p->next[(int)first_char]->depth = p->depth + 1;
            p->next[(int)first_char]->chr = first_char;
            p->next[(int)first_char]->type = 1;
            p->next[(int)first_char]->i_list.push_back(line);

            return true;
        }
        //leaf exist
        //printf("leaf exist\n");
        if (p->next[(int)first_char]->i_list.size() == 0 ||
                p->next[(int)first_char]->i_list[p->next[(int)first_char]->i_list.size()-1] != line) {
                p->next[(int)first_char]->type = 1;
                p->next[(int)first_char]->i_list.push_back(line);
                return true;
        }
        return true;
    }

   std::string new_s = s.substr(1, s.size()-1);
    //a new routine
    if (p->next[(int)first_char] == NULL) {
        ////printf("a new routine\n");
        p->next[(int)first_char] = init_node();
        p->next[(int)first_char]->depth = p->depth + 1;
        p->next[(int)first_char]->chr = first_char;
        p->next[(int)first_char]->type = -1;
    }
    ////printf("recurve\n");
    return insert_segment(p->next[(int)first_char], new_s, line);
}

void search_leaf(trie_node* p,std::string s) {
    const char* s_char = s.c_str();
    char first_char = s_char[0];
    if (p->next[(int)first_char] == NULL) return;
    if (p->next[(int)first_char]->type == 1 && s.size()==1) { //leaf
            can_nodes.push_back(p->next[(int)first_char]);
    }
    if (s.size() == 1) return;
   std::string new_s = s.substr(1, s.size()-1);
    search_leaf(p->next[(int)first_char], new_s);
    return;
}

int Trie::createIndex( std::vector< std::vector< std::string >> tags) {
    entities = tags;
    allocp = 0;
    root_node->depth = 0;
    for (int i=0; i<TRIE_WIDTH; i++) {
        root_node->next[i] = NULL;
    }
    root_node->type = 0;

    lmin = INT_MAX;
    lmax = INT_MIN;

    int tau = 2;
    std::string line;
    for (int lineCount=0; lineCount<tags.size(); lineCount++) {
        for (int i=0; i<tags[lineCount].size(); i++) {
            line = tags[lineCount][i];
            insert_segment(root_node, line, lineCount);
        }
    }

    return SUCCESS;
}

int calED(std::string s1,std::string s2, int ed)
{
    //printf("calED s1 = %s, s2 = %s\n", s1.c_str(), s2.c_str());
	int num1 = s1.length();
	int num2 = s2.length();
	if ( abs(num1-num2) > ed ) return ed+1;
	int D[num1+1][num2+1];
	memset(D,0,sizeof(D));
	int left;
	int right;
	for ( int i = 0; i <= num1; i++ ){
		bool if_ans = false;
		left = 0;
		if ( left < i - ed ) left = i - ed;
		right = num2;
		if ( right > i + ed ) right = i + ed;
		for ( int j = left; j <= right; j++ ) {
			if ( i == 0 ) D[i][j] = j;
			else if ( j == 0 ) D[i][j] = i;
			else {
				D[i][j] = D[i-1][j-1];
				if ( s1[i-1] != s2[j-1] ) D[i][j] += 1;
				if ( abs(i-1-j) <= ed && D[i-1][j] + 1 < D[i][j] )
					D[i][j] = D[i-1][j] + 1;
				if ( abs(i+1-j) <= ed && D[i][j-1] + 1 < D[i][j] )
					D[i][j] = D[i][j-1] + 1;
			}
			if ( D[i][j] <= ed ) if_ans = true;
		}
		if ( !if_ans ) return ed+1;
	}
	return D[num1][num2];
}

int Trie::trieED(const char *document, std::vector<int> &result) {
    result.clear();

    std::string doc(document);
    int doc_length = doc.length();
    int llmin = lmin - threshold;
    int llmax = lmax + threshold;
    //printf("llmin = %d, llmax = %d\n", llmin, llmax);

    for (int pos=0; pos<1; pos++) {

        can_nodes.clear();
        search_leaf(root_node, doc);

        for (int cc=0; cc<can_nodes.size(); cc++) {
            int match_length = can_nodes[cc]->depth;
            std::vector<int> e_list = can_nodes[cc]->i_list;

            for (int ee=0; ee<e_list.size(); ee++) {
                int ent_id = e_list[ee];
                result.push_back(ent_id);
            }
        }
    }

	//sort(result.begin() , result.end() , compare_Ed);
   std::vector<int>::iterator iter = unique(result.begin(),result.end());
   result.erase(iter,result.end());

   return SUCCESS;
}




#endif
