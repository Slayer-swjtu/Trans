#pragma once
#include <string>
#include <fstream>
#include <json/json.h>

class Datatrans 
{
	public:

		void remotecontrol(std::string request);  //方向等控制指令

		void plan_to_point(Json::Value root);//
		void walk_to_point(Json::Value root);

	private:



};

