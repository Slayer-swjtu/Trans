
#include "cstjson.hpp"
#include <boost/json.hpp> 
#include <json/json.h>
using namespace std;

std::string writeFileJson_maps(map<string,string> m)      
{
	std::cout << "(对车体地图的GET请求)" << std::endl;
	std::string used;
	map<string, string>::iterator pos = m.find("used");
	if (pos != m.end())
	{
		if ((*pos).second == "true")
		{
			Json::Reader reader; 
			Json::Value root; 
			Json::Value leaf;
			std::ifstream _("F:/gcrobot_server/api/maps/robots_maps.json", ios::binary);
			if (reader.parse(_, root))
			{
				for (unsigned int i = 0; i < root["data"].size(); i++)
				{
			
					if (root["data"][i]["used"].asBool() == true)
					{
						leaf["code"] = Json::Value(20000);
						leaf["data"].append(root["data"][i]);
						used = leaf.toStyledString(); 
						break;   
					}
				}
			}
			_.close();
			return used;
		}
	}
	else
		return "robots_maps.json";
}
void writeFileJson_missions(map<string, string> m)
{
	std::cout << "(对车体任务的GET请求)" << std::endl;
}
void readFileJson_robots(map<string, string> m)
{
	std::cout << "(对车体状态的GET请求)" << std::endl;   //需要读websocketserver写入内存里的串口数据
	/*std::ifstream  in("F:/gcrobot_server/api/robots/robots_state.json");
	std::istreambuf_iterator<char>  beg(in), end;
	std::string  str(beg, end);
	std::cout << "str is :" << str << std::endl;*/
	return;
	/*Json::Value root;
	Json::Value leaf1;
	Json::Value leaf2;
	leaf1["id"] = Json::Value(1);
	leaf1["sn"] = Json::Value("robot1");
	leaf1["name"] = Json::Value(NULL);
	leaf1["state"] = Json::Value("UNKNOWN");
	leaf1["x"] = Json::Value("0.0");
	leaf1["y"] = Json::Value("0.0");
	leaf1["orientation"] = Json::Value("0");
	leaf1["enabled"] = Json::Value(false);
	leaf1["power"] = Json::Value(0);
	leaf1["oil"] = Json::Value(0);
	leaf1["IP"] = Json::Value(NULL);
	leaf1["map"] = Json::Value(NULL);
	leaf2["id"] = Json::Value(2);
	leaf2["sn"] = Json::Value("robot2");
	leaf2["name"] = Json::Value(NULL);
	leaf2["state"] = Json::Value("UNKNOWN");
	leaf2["x"] = Json::Value("0.0");
	leaf2["y"] = Json::Value("0.0");
	leaf2["orientation"] = Json::Value("0");
	leaf2["enabled"] = Json::Value(false);
	leaf2["power"] = Json::Value(0);
	leaf2["oil"] = Json::Value(0);
	leaf2["IP"] = Json::Value(NULL);
	leaf2["map"] = Json::Value(NULL);
	root["code"] = Json::Value("20000");
	root["data"].append(leaf1);
	root["data"].append(leaf2);
	std::ofstream fout;
	fout.open("F:/gcrobot_server/api/robots/robot_state.json");
	assert(fout.is_open());
	fout << root.toStyledString() << std::endl;
	fout.close();*/
}

void writeFileJson_mission_allo(std::string str)  //换算终端传来的地图数据信息，计算真实的任务坐标
{
	Json::Value root; Json::Reader reader;
	if (reader.parse(str,root))
	{
		std::cout << "calling\n";
		for (size_t i = 0; i < root["corner_points"].size(); i++)
			root["corner_points"][i]["y"] = root["corner_points"][i]["y"].asDouble() + 4.0;/*这个残差数跟map中的分辨率，地图大小有关，详情联系佛跳墙*/
	}
	std::ofstream os;    
	os.open("F:/gcrobot_server/api/mission/group/mission.json"); 
	os <<root.toStyledString();   
	os.close();
	return;
}
void remotecontrol(std::string request)
{
	std::ofstream os;
	os.open("F:/gcrobot_server/api/robots/remote_command.json");
	std::cout << "摇杆控制指令" << std::endl;
	os << request;
	os.close();
	return;
}
void cuttercontrol(std::string str)
{
	std::ofstream os;
	os.open("F:/gcrobot_server/api/robots/remote_command.json");
	std::cout << "刀盘升降指令" << std::endl;
	os << str;
	os.close(); 
	return;
}

void cuttercluth(std::string str)
{
	std::ofstream os;
	Json::Value root; root["cutter"] = Json::Value(true); root["robot_sn"] = Json::Value("123456");
	os.open("F:/gcrobot_server/api/robots/remote_command.json");
	std::cout << "刀盘离合指令" << std::endl;
	os << root.toStyledString();
	os.close();
	return;
}
void modechange(void)
{
	std::ofstream os;
	Json::Value root; root["change"] = Json::Value(true);root["robot_sn"]=Json::Value("123456");
	os.open("F:/gcrobot_server/api/robots/remote_command.json");
	std::cout << "模式切换指令" << std::endl;
	os << root.toStyledString();
	os.close();
	return;
}

