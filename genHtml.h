#ifndef GENHTML_H
#define GENHTML_H

#include "param.h"

#define DATASET "zipcode-address.json"

void generatePOIs(std::string word, double x, double y) {

        fprintf(stdout, "generate %s.html, appears in %d places\n", word.c_str(), inversed_list[word].size());
        std::string html_name = "./html/" + word + ".html";
        std::ofstream fout(html_name, std::ios::out);
        if (!fout.is_open()) exit(4);
        fout<<"<!DOCTYPE html><html><head><title>"<<word;
        fout<<"</title><meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\">"<<std::endl;
        fout<<"<meta charset=\"utf-8\"><style>html, body, #map-canvas {height: 100%;margin: 0px;padding: 0px}</style>"<<std::endl;
        fout<<"<script src=\"https:"<<"//maps.googleapis.com/maps/api/js?v=3.exp&signed_in=true\"></script><script>"<<std::endl;
        fout<<"function initialize(){"<<std::endl;
        fout<<"var mapOptions = {zoom: 14,center: new google.maps.LatLng("<<x<<","<<y<<")};"<<std::endl;
        fout<<"var map = new google.maps.Map(document.getElementById('map-canvas'),mapOptions);"<<std::endl;
        fout<<"var panoramaOptions = {position: new google.maps.LatLng("<<x<<","<<y<<")};"<<std::endl;
        fout<<"var panorama = new google.maps.StreetViewPanorama(document.getElementById('pano'), panoramaOptions);"<<std::endl;
        fout<<"map.setStreetView(panorama);"<<std::endl;


        //generate markers
        for (int j=0; j<inversed_list[word].size(); j++) {
            int id = inversed_list[word][j];
            double lat = lats[id];
            double lng = lngs[id];
            fout<<"var marker"<<j<<" = new google.maps.Marker({position: new google.maps.LatLng("<<lat<<","<<lng<<"), map: map});"<<std::endl;

            //generate infowindow
            fout<<"var infowindow"<<j<<" = new google.maps.InfoWindow({ content: "<<std::endl;
            fout<<"'<div id=\"content\"><div id=\"siteNotice\"></div><h1 id=\"firstHeading\" class=\"firstHeading\">";
            //fout<<tags[id]
            for (int kk=0; kk<name[id].size(); kk++) {
                if (name[id][kk] == '\n') {
                    fout<<", ";
                }
                else if (name[id][kk] == '\'') {
                    fout<<"\\'";
                }
                else if (name[id][kk] == '\"') {
                    fout<<"\\\"";
                }
                else fout<<name[id][kk];
            }

            fout<<"</h1><div id=\"bodyContent\">'+"<<std::endl;
            fout<<"'<p><b>Addr: </b>";
            //fout<<addr[id]
            for (int kk=0; kk<addr[id].size(); kk++) {
                if (addr[id][kk] == '\n') {
                    fout<<", ";
                }
                else if (addr[id][kk] == '\'') {
                    fout<<"\\'";
                }
                else if (addr[id][kk] == '\"') {
                    fout<<"\\\"";
                }
                else fout<<addr[id][kk];
            }

            fout<<"</p>'+"<<std::endl;
            fout<<"'<p><b>Url: </b><a href=\""<<urls[id]<<"\">"<<urls[id]<<"</a></div></div>'});"<<std::endl;
            fout<<"google.maps.event.addListener(marker"<<j<<", 'click', function() {"<<std::endl;
            //infoWindow
            fout<<"infowindow"<<j<<".open(map,marker"<<j<<");"<<std::endl;
            //StreetView
            fout<<"panoramaOptions.position = marker"<<j<<".position;"<<std::endl;
            fout<<"panorama.setOptions(panoramaOptions);"<<std::endl;

            fout<<"});"<<std::endl;
        }

        fout<<"}"<<std::endl;
        fout<<"google.maps.event.addDomListener(window, 'load', initialize);";
        fout<<"</script></head><body>"<<std::endl;
        fout<<"<div id=\"map-canvas\" style=\"width: 50%; height: 100%;float:left\"></div>"<<std::endl;
        fout<<"<div id=\"pano\" style=\"width: 50%; height: 100%;float:left\"></div>"<<std::endl;
        fout<<"</body></html>"<<std::endl;
        fout.close();


    return;
}

void generateTags(std::vector<std::pair<std::string, int>> new_list, int k, double x, double y) {
    fprintf(stdout, "generate tags.html\n");
    std::ofstream fout("./html/tags.html", std::ios::out);
    if (!fout.is_open()) exit(3);
    fout << "<html><head><title>Search Result</title></head><body><center><table >" <<std::endl;

    int size = k<new_list.size()?k:new_list.size();

    for (int i=0; i<size ;i++) {
        fout<< "<tr><td align=\"left\"><a href=\"http://"<< server_host<<":"<<server_port<<"/"<<new_list[i].first.c_str()<<".html\">";
        fout<< new_list[i].first.c_str() <<":"<<new_list[i].second<< "</a></td></tr>" <<std::endl;
    }
    fout << "</table></center></body></html>" <<std::endl;
    fout.close();

    for (int i=0; i<size; i++) {
        std::string word = new_list[i].first;
        generatePOIs(word, x, y);
    }

    return;
}

void generateError(std::string title, std::string err){
    std::string html_name = "./html/" + title + ".html";
    std::ofstream fout(html_name, std::ios::out);
    fout<<"<html><body><center>"<<err<<"</center></body></html>";
    fout.close();
    return;
}

#endif


