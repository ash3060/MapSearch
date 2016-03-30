#ifndef PARAM_H
#define PARAM_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fstream>
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
#include <json/json.h>

#include "Rtree.h"

#define BUFSIZE 1024
#define server_port 8000
#define server_host "127.0.0.1"
#define CENTER_LAT 1.31391
#define CENTER_LNG 103.883217



extern RTree<int, double, 2, double> tree;
extern std::vector<std::string> name;
extern std::vector< std::vector<std::string>> tags;
extern std::vector<std::string> addr;
extern std::vector<std::string> urls;
extern std::vector<double> lats;
extern std::vector<double> lngs;
extern std::unordered_map<std::string, int> frequency_list;
extern std::unordered_map<std::string, std::vector<int>> inversed_list;

#endif
