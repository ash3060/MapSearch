#include "param.h"
#include "genHtml.h"
#include "Trie.h"

RTree<int, double, 2, double> tree;
Trie trie;

std::vector<std::string> name;
std::vector< std::vector<std::string>> tags;
std::vector<std::string> addr;
std::vector<std::string> urls;
std::vector<double> lats;
std::vector<double> lngs;
std::unordered_map<std::string, int> frequency_list;
std::unordered_map<std::string, std::vector<int>> inversed_list;



double dmin(double a, double b) { return (a<b)?a:b; }
double dmax(double a, double b) { return (a>b)?a:b; }

/*************************** R-Tree Part ***************************/
struct Rect
{
    Rect()  {}
    Rect(double a_minX, double a_minY, double a_maxX, double a_maxY)
    {
        min[0] = a_minX;
        min[1] = a_minY;

        max[0] = a_maxX;
        max[1] = a_maxY;
    }
    double min[2];
    double max[2];
};

void split(int id) {
    std::string line = name[id];
    char first = line.c_str()[0];

    int word_num = 0;
    int length = line.length();

    //split word
    int word_l = 0;
    int word_r = 0;
    std::string word;
    std::vector< std::string > tmp;
    while(word_r < length) {
        if ( word_r == length-1 || line[word_r+1]==' ') {  //split word
            if (word_l <= word_r) {
                word = line.substr(word_l, word_r-word_l+1);
                tmp.push_back(word);
            }
            word_r += 2;
            word_l = word_r;
        }
        else {
            word_r++;
        }
    }
    tags.push_back(tmp);
    if (id != tags.size()-1) {
        fprintf(stdout, "split tags error!\n");
        exit(5);
    }
    return;
}


void search_tags(int id) {
    std::vector< std::string > tags_tmp = tags[id];
    std::string word;
    for (int ii=0; ii<tags_tmp.size(); ii++) {
        word = tags_tmp[ii];
        frequency_list[word] ++;
        if (inversed_list[word].size()==0 || inversed_list[word][inversed_list[word].size()-1] != id) {
            inversed_list[word].push_back(id);
        }
    }

    return;
}

bool MySearchCallback(int id, void* arg)
{
  //printf("Hit data point %d\n", id);
  search_tags(id);
  return true; // keep going
}

//create Index from data set
void createIndex() {
    std::ifstream fin;
    fin.open(DATASET);
    if (!fin.is_open()) exit(2);

    Json::Reader reader;
    Json::Value root;
    std::string line;
    int lineCount = 0;
    while (getline(fin, line)){
        if (!reader.parse(line.c_str(), root, false)){
            return;
        }
        name.push_back(root["name"].asString());
        split(lineCount);
        addr.push_back(root["addr"].asString());
        urls.push_back(root["url"].asString());
        double lat = root["latlng"][0u].asDouble();
        double lng = root["latlng"][1].asDouble();
        lats.push_back(lat);
        lngs.push_back(lng);

        Rect tmp_rect = Rect(lat, lng, lat, lng);
        tree.Insert(tmp_rect.min, tmp_rect.max, lineCount);
        lineCount ++;
    }
    fprintf(stdout, "In total %d POIs\n", tree.Count());
    fprintf(stdout, "RTree finish!\n");

    trie.createIndex(tags);
    fprintf(stdout, "Trie Tree finish!\n");

    fin.close();
    return;
}


bool compare_frequency(std::pair<std::string, int> a , std::pair<std::string, int> b){
    if (a.second > b.second) return true;
	return false;
}

//search in a given area
void search_area(double xmin, double ymin, double xmax, double ymax, int k)
{
    frequency_list.clear();
    inversed_list.clear();
    fprintf(stdout, "search (%lf, %lf) to (%lf, %lf)\n", xmin, ymin, xmax, ymax);
    Rect search_rect(xmin, ymin, xmax, ymax);
    int hit = tree.Search(search_rect.min, search_rect.max, MySearchCallback, NULL);
    fprintf(stdout, "In total %d hits\n", hit);

    if (hit == 0) {
        generateError("tags", "No POI in this area!");
        return;
    }
    std::vector<std::pair<std::string, int>> new_list;
    for (std::unordered_map<std::string, int>::iterator i=frequency_list.begin(); i!=frequency_list.end(); i++) {
        new_list.push_back(std::pair<std::string, int>(i->first, i->second));
    }

    sort(new_list.begin() , new_list.end() , compare_frequency);

    double xcenter = (xmin+xmax)/2.0;
    double ycenter = (ymin+ymax)/2.0;
    generateTags(new_list, k, xcenter, ycenter);
    return;

}

//search key word
void search_key_word(std::string keyword) {
    std::vector<int> result;
    trie.trieED(keyword.c_str(), result);
    /*for (int i=0; i<result.size(); i++) {
        fprintf(stdout, "id=%d, name=%s\n", result[i], name[result[i]].c_str());
    }*/
    if (result.size()==0) {
        generateError(keyword, "No POI contains this keyword!");
        return;
    }
    inversed_list[keyword] = result;
    generatePOIs(keyword, CENTER_LAT, CENTER_LNG);

    return;
}

/*************************** End of R-Tree Part ***************************/



/*************************** Server Part ***************************/

//create socket
int getSocket(int* port){
    int ret, on;
    register int s_socketfd;        // server 描述符
    struct sockaddr_in local;   // 本地地址

    if ((s_socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stdout, "Fail to initial socket!\n");
        exit(1);
    }
    ret = setsockopt( s_socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = inet_addr(server_host);
    local.sin_port = htons(*port);

    if (bind(s_socketfd, (struct sockaddr *)&local, sizeof(local)) == -1) {
        fprintf(stdout, "Bind unseccessfully!\n");
        exit(2);
    }
    int len = sizeof(local);
    getsockname(s_socketfd,(struct sockaddr*)&local, (socklen_t *)&len);
    *port = ntohs(local.sin_port);
    fprintf(stdout, "create socket seccessfully!\n");
    return s_socketfd;
}

//return hou zhui
char* file_type(char *f)
{
    char *cp;
    if ( (cp = strrchr(f, '.' )) != NULL ) return cp+1;
    return "";
}

//HTTP response header
void header( FILE *fp, char *content_type )

{
    //fprintf(stdout, "start header!\n");
    fprintf(fp, "HTTP/1.0 200 OK\r\n");
    if ( content_type ){
        fprintf(fp, "Content-type: %s\r\n", content_type );
    }
    //fprintf(stdout, "header!\n");
    return;
}

//respond
void respond(char *_f, int fd)
{
    //fprintf(stdout, "_f = %s\n", _f);
    char* f;
    std::string f_tmp = std::string(_f);
    if(_f[1]=='s' && _f[2]=='e' && _f[3] =='a') { //search
        f = _f;
        int position = f_tmp.find("submit=submit");
        int bound_pos = f_tmp.find("bound1=");
        if (bound_pos == f_tmp.npos) {
            exit(15);
        }
        //submit
        if (position != f_tmp.npos){
            std::string ff = f_tmp.substr(bound_pos, f_tmp.length()-bound_pos);
            double x1, y1, x2, y2;
            int k;
            int res = sscanf(ff.c_str(), "bound1=%lf%%2C%lf&bound2=%lf%%2C%lf&k=%d&submit=submit",
                                &x1, &y1, &x2, &y2, &k);
            if ( res != 5) {
                fprintf(stdout, "search error: %s\nres = %d\n", ff.c_str(), res);
                generateError("error", "Invalid query!");
                f = "./html/error.html";
            }
            else {
                double xmin, ymin, xmax, ymax;
                xmin = dmin(x1, x2);
                xmax = dmax(x1, x2);
                ymin = dmin(y1, y2);
                ymax = dmax(y1, y2);
                search_area(xmin, ymin, xmax, ymax, k);
                f = "./html/tags.html";
            }
        }
        //seasrch keyword
        else
        {
            fprintf(stdout,"no submit=submit\n");
            std::string keyword = f_tmp.substr(17, bound_pos-15-17);
            if (keyword.size()==0){
                generateError("error", "Please enter a keyword!");
                f = "./html/error.html";
            }
            else{
                fprintf(stdout, "kw = %s\n", keyword.c_str());
                search_key_word(keyword);
                std::string f_str = "./html/" + keyword + ".html";
                f = (char*)malloc(sizeof(char)*32);
                strcpy(f, f_str.c_str());
            }
        }
    }
    else if (strcmp(_f, "/") == 0) {
        f = "./html/index.html";
    }
    else {
        std::string f_str = "./html" + std::string(_f);
        f = (char*)malloc(sizeof(char)*32);
        strcpy(f, f_str.c_str());
    }
    char    *extension = file_type(f);
    char    *content = "text/plain";
    FILE    *fpsock, *fpfile;
    int c;

    if ( strcmp(extension,"html") == 0 ){
        content = "text/html";
    }
    fpsock = fdopen(fd, "w");
    fpfile = fopen(f , "r");
    //fprintf(stdout, "f = %s\n", f);
    if ( fpsock != NULL && fpfile != NULL )
    {
        header( fpsock, content );
        fprintf(fpsock, "\r\n");
        while( (c = getc(fpfile) ) != EOF )
            putc(c, fpsock);
        fclose(fpfile);
        fclose(fpsock);
    }
    //fprintf(stdout, "respond!\n");
    return;
}

//process request
void process_rq( char *rq, int fd )
{
    //fprintf(stdout, "start process_rq!\n");
    char    cmd[BUFSIZE], arg[BUFSIZE];
    //strcpy(arg, "./");      /* precede args with ./ */
    if ( sscanf(rq, "%s%s", cmd, arg) != 2 ) return;
    if ( strcmp(cmd,"GET") != 0 ){
        fprintf(stdout, "cmd error!\n");
        return;
    }
    else {
        respond(arg, fd);
    }
    //fprintf(stdout, "process_rq!\n");
    return;
}

/*************************** End of Server Part ***************************/



int main(int argc, char *argv[]){

    createIndex();

    int clientNum = 0;      // client数
    int ret;
    int s_socketfd;         // server socket描述符
    int c_socketfd;     // client socket描述符
    char buf[BUFSIZE];                  //
    int port = server_port;
    s_socketfd = getSocket(&port);
    // listen
    listen(s_socketfd, 20);
    struct sockaddr_in client_in;       // client address
    int len = sizeof(struct sockaddr_in);

    while(1){

        /* take a call and buffer it */

        if ((c_socketfd = accept(s_socketfd, (struct sockaddr *)&client_in, (socklen_t*)&len)) == -1){
            printf("accept error\n");
        }
        printf("\n\n\nClient address: %s\n", inet_ntoa(client_in.sin_addr));


        /* read request */

        if(read(c_socketfd,buf,BUFSIZE)>0){
            fprintf(stdout, "Command from client: %s\n", buf);
            process_rq(buf, c_socketfd);
        }
        close(c_socketfd);
        //fprintf(stdout, "close a client\n\n\n");

    }
    return 0;
}

