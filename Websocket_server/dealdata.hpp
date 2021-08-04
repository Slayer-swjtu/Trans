#pragma once

#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <stdio.h>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/date_time.hpp>
//#include <boost/foreach.hpp>
#include <vector>
#include <json/json.h>
#include <math.h>
#include <algorithm>
struct Points
{
    double x0; double y0 ; double x1; double y1;
    double x2; double y2 ; double x3; double y3;

};

std::string dealdata();
bool trans_command();
bool mission_check();
std::string parse2trans_json(std::string str,std::vector<std::vector<double> > sum);

std::vector<std::vector<double> >  trans_mission();
std::vector<std::vector<double> >  path_planning(std::vector<std::vector<double> > &edge, std::vector<double>& origin ,Json::Value root);
std::string plot(std::vector<std::vector<double> > &edge);
void inCallBack(std::vector<double>& origin);
void trans_set();
struct ObsBuff
{
	double x;
	double y;
	int parity;
	int Dir;
	int row;
	int rang;
	int order;
	int flag;
	//int follow;
	int Num;//第几次割草
};

struct OutBuff_1
{
	double x;
	double y;
	int flag;
	int order;
	int dir;
	int Num;//第几次割草
};

void Init(void);
void CalculateObs(void);
bool cmp(ObsBuff a, ObsBuff b);
void CalOutB_1();
void ObsToOutB_2(void);
void Printf(void);
void test1(void);

struct ecef {
	double x;
	double y;
	double z;
	double theta;
};


struct gpsMessage {
	double lat;
	double lon;
	double height;
	double theta;
};

bool transmap(Json::Value &leaf);

void sortBound(std::vector<std::pair<double, double>>& boundV);

bool write_mission(std::vector<std::vector<double> > v);
//