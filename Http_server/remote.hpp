#pragma once
#include <string>
#include <fstream>
#include <json/json.h>

class Datatrans 
{
	public:

		void remotecontrol(std::string request);  //����ȿ���ָ��

		void plan_to_point(Json::Value root);//
		void walk_to_point(Json::Value root);

	private:



};

