
//各类json的构造函数

#include <json/json.h>
#include <map>
#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <string>
#include <iostream>
#include <vector>

void readFileJson_robots(std::map<std::string,std::string> m);

std::string writeFileJson_maps(std::map<std::string, std::string> m);

void writeFileJson_missions(std::map<std::string, std::string> m);

void writeFileJson_mission_allo(std::string str);

void remotecontrol(std::string request);

//void readFileJson_pointexec();

//void writeFileJson_pointplan();

void cuttercontrol(std::string str);

void cuttercluth(std::string str);

void modechange(void);

std::vector<std::vector<double> > coverage_path(Json::Value root);

