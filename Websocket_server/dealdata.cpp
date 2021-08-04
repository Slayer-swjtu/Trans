#include"dealdata.hpp"
#include <time.h>  
#define com "COM19"
using namespace boost::asio;
size_t count = 0;
struct Origin
{
	double x=0;
	double y=0;
	
};
std::string dealdata()  
{   
	std::string serialbuf;
	io_service iosev;
	serial_port sp1(iosev, com);
	sp1.set_option(serial_port::baud_rate(115200));
	sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp1.set_option(serial_port::parity(serial_port::parity::none));
	sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp1.set_option(serial_port::character_size(8));
	/*deadline_timer timer(iosev);
	timer.expires_from_now(boost::posix_time::millisec(800));
	timer.async_wait(boost::bind(&serial_port::cancel, boost::ref(sp1)));*/
    //std::size_t count = sp1.read_some(buffer(serialbuf));
	std::size_t count = boost::asio::read_until(sp1, boost::asio::dynamic_buffer(serialbuf), '$');
	std::cout << "串口收到数据:" << std::endl;
	std::cout << count << std::endl;
    std::cout << serialbuf <<"  -- server read from serial port" << std::endl;
	std::cout << std::endl;
	if (serialbuf.size() > 300)
		return  std::string(serialbuf);
	else
		return  "{\"code\":20000 }";
}

bool trans_command()               
{
		io_service iosev;
		serial_port sp1(iosev, com);
		sp1.set_option(serial_port::baud_rate(115200));
		sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
		sp1.set_option(serial_port::parity(serial_port::parity::none));
		sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
		sp1.set_option(serial_port::character_size(8));
		std::ifstream  in("F:/gcrobot_server/api/robots/remote_command.json",std::ios::in|std::ios::binary);
		std::istreambuf_iterator<char>  beg(in), end;
		std::string  str(beg, end);   
		write(sp1, buffer((char*)str.data(), str.length()));
		in.close();
		std::fstream file("F:/gcrobot_server/api/robots/remote_command.json", std::ios::out);
		file.close();
		return true;    
}
std::vector<std::vector<double> > trans_mission()        
{
	io_service iosev;
	serial_port sp1(iosev, com);
	sp1.set_option(serial_port::baud_rate(115200));
	sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp1.set_option(serial_port::parity(serial_port::parity::none));
	sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp1.set_option(serial_port::character_size(8));

	std::vector<std::vector<double> > edge(4, std::vector<double>(2, 0));

	Json::Reader reader;
	Json::Value root;
	Json::Value leaf;
	std::ifstream  in("F:/gcrobot_server/api/mission/group/mission.json",std::ios::in|std::ios::binary);
	if (reader.parse(in, root))
	{
		Json::Value leaf_; Json::FastWriter writer_;
		leaf_["sn"] = root["sn"];
		leaf_["corner_points"] = root["corner_points"];
		write(sp1, buffer(writer_.write(leaf_), writer_.write(leaf_).length()));

	}
	in.close();
	/*std::istreambuf_iterator<char>  beg(in), end;
	std::string  str(beg, end);
	std::cout << "str is :"<<str << std::endl;
	write(sp1, buffer((char*)str.data(), str.length() + 1));
	in.close();*/
	std::ifstream  in_("F:/gcrobot_server/api/mission/group/mission.json", std::ios::binary);  // 本程序很多地方并不能优化代码，变成在一个流中解析并写走!!!  boost和jsoncpp在文指操作上的api有冲突！
	if (reader.parse(in_, root))
	{
		for (unsigned int i = 0; i < root["corner_points"].size(); i++)
		{
			leaf = root["corner_points"][i];
			edge[i][0] = leaf["x"].asDouble();
			edge[i][1] = leaf["y"].asDouble();
		}
	}in_.close();
	return edge;
}

std::string plot(std::vector<std::vector<double> > edge)
{
	system("F:/gcrobot_server/plot.bat");
}

std::string parse2trans_json(std::string str,std::vector<std::vector<double> > sum)      
{
	Origin origin;
	Json::Reader reader(Json::Features::strictMode());
	Json::Value root; 
	if (reader.parse(str, root))
	{
		std::cout << "JSON字符串解析正确\n";
		Json::Value _root;   
		Json::Value state; 
		if (count==0)
		{
			origin.x = root["x"].asDouble();
			origin.y = root["y"].asDouble();
			count++;
		}
		//http state
		/*---------------------------------------------------------------------*/
		state["code"] = Json::Value(20000);
		Json::Value s;
		s["id"] = Json::Value(10);
		s["name"] = Json::Value("gc1");
		s["sn"] = Json::Value("123456");
		s["robot_sn"] = Json::Value("123456");
		s["state"] = Json::Value("UNKNOWN");
		s["x"] = root["x"];
		s["y"] = root["y"];
		s["enable"] = Json::Value(true);
		s["power"] = root["power"];
		s["oil"] = root["oil"];
		state["data"].append(s);
		std::ofstream os;         
		os.open("F:/gcrobot_server/api/robots/robots_state.json",std::ios::binary);  
		os << state.toStyledString();
		os.close();
		/*---------------------------------------------------------------------------*/
		_root = root;
		_root["x"] = root["x"].asDouble()+50;
		_root["y"] = root["y"].asDouble()-50;
		//path 1元
		int index = root["index"].asInt();
		Json::Value __leaf;
		Json::Value __leaf__;
		__leaf__["x"] = Json::Value(origin.x);
		__leaf__["y"] = Json::Value(origin.y);			
		__leaf__["z"] = Json::Value(0.0);
		__leaf.append(__leaf__);
		__leaf__["x"] = Json::Value(sum[0][0]);
		__leaf__["y"] = Json::Value(sum[0][1]);
		__leaf__["z"] = Json::Value(0.0);
		__leaf.append(__leaf__);
		_root["path"].append(__leaf);
		//path 2元
		Json::Value _leaf;
		for (int i = 0; i < sum.size(); i++)
		{
			Json::Value _leaf_;
			_leaf_["x"] = Json::Value(sum[i][0]);
			_leaf_["y"] = Json::Value(sum[i][1]);
			_leaf_["z"] = Json::Value(0.0);
			_leaf.append(_leaf_);
		}_root["path"].append(_leaf);

		if (!index < 0)
		{
			for (int i = index; i < sum.size(); i++)
			{
				_root["scan"].append(sum[i][0]);
				_root["scan"].append(sum[i][1]);

			}
		}
		else
		{
			for (int i = 0; i < sum.size(); i++)
			{
				_root["scan"].append(sum[i][0]);
				_root["scan"].append(sum[i][1]);
			}
		}
		_root["sn"] = Json::Value("123456");
		_root["name"] = Json::Value("gc1");
		_root["id"] = Json::Value(10);
		Json::FastWriter w;
		return w.write(_root);
	}
	else
	{
		std::cout << "JSON字符串解析错误\n";
		root["code"] = 20000;
		root["name"] = "gc1";
		root["id"] = 10;
		return root.toStyledString();
	}
}
bool transmap(Json::Value &leaf) 
{
	
	io_service iosev;
	serial_port sp1(iosev, com);
	sp1.set_option(serial_port::baud_rate(115200));
	sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp1.set_option(serial_port::parity(serial_port::parity::none));
	sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp1.set_option(serial_port::character_size(8));
	Json::FastWriter writer;
	std::string serialbuf;
	for (;;)
	{
		write(sp1, buffer(writer.write(leaf), writer.write(leaf).length()));
		std::cout << "has written\n";
		std::size_t count = boost::asio::read_until(sp1, boost::asio::dynamic_buffer(serialbuf), '$');
		//count = sp1.read_some(buffer(serialbuf));
		std::cout << "count\n";
		if (serialbuf[0] == '*' && serialbuf.back() == '$')
		{
			serialbuf = serialbuf.substr(1, serialbuf.length() - 2);
			Json::Value root; Json::Reader reader;
			reader.parse(serialbuf, root);
			if (!root["got"].isNull())
			{
				serialbuf.clear();
				Sleep(1000);
				break;
			}
			serialbuf.clear();
		}
		Sleep(1000);
	}
	return true;
}
void trans_set()
{
	Json::Value root;
	root["sn"] = Json::Value("123456");
	root["set"] = Json::Value(true);
	Json::FastWriter writer;
	io_service iosev;
	serial_port sp1(iosev, com);
	sp1.set_option(serial_port::baud_rate(115200));
	sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp1.set_option(serial_port::parity(serial_port::parity::none));
	sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp1.set_option(serial_port::character_size(8));
	write(sp1, buffer(writer.write(root), writer.write(root).length()));
	return;
}

bool mission_check()
{
	io_service iosev;
	serial_port sp1(iosev, com);
	sp1.set_option(serial_port::baud_rate(115200));
	sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp1.set_option(serial_port::parity(serial_port::parity::none));
	sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp1.set_option(serial_port::character_size(8));
	std::string serialbuf;
	std::size_t count = boost::asio::read_until(sp1, boost::asio::dynamic_buffer(serialbuf), '$');
	Json::Reader reader; Json::Value root;
	if (reader.parse(serialbuf, root))
		if (!root["corner_points"].isNull())
			return true;
}

bool write_mission(std::vector<std::vector<double> > v)
{
	Json::Value root; Json::Value leaf; Json::Value xyz; Json::Value xyz_;
	xyz_["x"] = 0.0; xyz_["y"] = 0.0; xyz_["z"] = 0.0; xyz_["w"] = 0.0;

	for (size_t i = 0; i < v.size(); i++)
	{
		Json::Value stem;
		xyz["x"] = v[i][0];xyz["y"] = v[i][1];xyz["z"] = 0.0;
		leaf["position"] = xyz;
		leaf["rotation"] = xyz_;
		stem.append(leaf);

		if ((i+1)<v.size())
		{
			xyz["x"] = v[i+1][0]; xyz["y"] = v[i+1][1]; xyz["z"] = 0.0;
			leaf["position"] = xyz;
			leaf["rotation"] = xyz_;
			stem.append(leaf);
		}
		else
		{
			xyz["x"] = v[0][0]; xyz["y"] = v[0][1]; xyz["z"] = 0.0;
			leaf["position"] = xyz;
			leaf["rotation"] = xyz_;
			stem.append(leaf);
		}
		root.append(stem);
	}
	Json::Value earth; earth.append(root);
	Json::FastWriter wr;
	std::ofstream os;
	os.open("F:/gcrobot_server/api/mission/group/mission_result.json");
	os << wr.write(earth);
	os.close();
}