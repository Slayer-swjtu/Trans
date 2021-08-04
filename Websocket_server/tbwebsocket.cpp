/*websocket协议+串口数传的通讯程序，文件指针操作(未采用Mysql数据库)，对数传要求比较高*/

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include "dealdata.hpp"
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <sys\stat.h>
#include <windows.h>
#include <io.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;     // from <boost/asio/ip/tcp.hpp>
void* ptr;
std::vector<std::vector<double> > mission;
void
do_session(tcp::socket socket)
{
    for (;;)
    {
        std::cout << "检测是否确定发送\n";
        std::fstream fin_;
        fin_.open("F:/gcrobot_server/api/mission/group/mission_set.json", std::ios::binary);
        fin_.seekg(0, std::ios::end);
        std::streampos fp_ = fin_.tellg();
        if (int(fp_) != 0)
        {
            std::cout << "发送！\n";
            trans_set();
            fin_.close();
            //std::fstream out("F:/gcrobot_server/api/mission/group/mission_set.json", std::ios::out);//删盘
            //out.close();
            break;
        }
        Sleep(500);
    }
        try
        {
            websocket::stream<tcp::socket> ws{ std::move(socket) };
            ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type& res)
                {
                    res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync");
                }));

            ws.accept();
            std::cout << "--达成连接--" << std::endl;

            for (;;)
            {
                beast::flat_buffer buffer;
                //ws.read(buffer);
                //std::cout << beast::make_printable(buffer.data()) << std::endl;
                //std::ifstream file("F:\\jiesen.txt");/*只能在文件内io？*/
                //std::istreambuf_iterator<char> beg(file), end;
                std::string str;

                ws.text(true/*ws.got_text()*/);

                str = dealdata(); 

                if (str[0] == '*' && str.back() == '$')  //配合read_until方法，设置协议头和尾，坏数不解析
                {
                    str = str.substr(1, str.length() - 2);

                    std::cout << str << "   ----websocket message\n" << std::endl;

                    str = parse2trans_json(str, mission);

                    ws.write(net::buffer(str, str.length() + 1));
                }
                std::fstream fin;
                fin.open("F:/gcrobot_server/api/robots/remote_command.json", std::ios::in | std::ios::binary);
                fin.seekg(0, std::ios::end);
                std::streampos fp = fin.tellg();
                if (int(fp) != 0)
                {
                    std::cout << "收到终端的遥控指令！" << std::endl;
                    trans_command();
                }
                else
                    std::cout << "暂未收到终端的遥控指令..." << std::endl;
                fin.close();
            }
        }
        catch (beast::system_error const& se)
        {
            std::cout << "异常关闭！\n";
            if (se.code() != websocket::error::closed)
                std::cerr << "Error: " << se.code().message() << std::endl;
        }
        catch (std::exception const& e)
        {
            std::cout << "错误了！\n";
            std::cerr << "Error: " << e.what() << std::endl;
        }
}
//----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
        for (;;)
        {
            Sleep(1000);
            Json::Value root; Json::Reader reader;
            std::ifstream map("F:/gcrobot_server/api/maps/robots_maps.json", std::ios::binary);
            if (reader.parse(map, root))
            {
                for (size_t i = 0; i < root["data"].size(); i++)
                {
                    if (root["data"][i]["used"] == Json::Value(true))
                    {
                        Json::Value leaf;
                        leaf["boundary"] = root["data"][i]["boundary"];
                        leaf["obstacle"] = root["data"][i]["obstacle"];
                        leaf["robot_sn"] = root["data"][i]["sn"];//break;//##
                        if (transmap(leaf))  /*收到车反馈收到地图的信号*/
                        {
                            break;

                        }

                    }
                }
                break;
            }
        }

        for (;;)
        {
            std::cout << "检查有无任务\n";
            std::fstream fin_;
            fin_.open("F:/gcrobot_server/api/mission/group/mission.json", std::ios::in | std::ios::binary);
            fin_.seekg(0, std::ios::end);
            std::streampos fp_ = fin_.tellg();
            if (int(fp_) != 0)
            {
                std::cout << "收到终端的任务发布,本地规划任务中..." << std::endl;
                fin_.close();
                std::vector<std::vector<double> > mission_ = trans_mission();
                std::vector<double> origin(3, 0);
                std::ifstream _("F:/gcrobot_server/api/maps/robots_maps.json", std::ios::binary);
                Json::Reader reader; Json::Value _root; int j = 0; Json::Value ob;
                if (reader.parse(_, _root))
                {
                    for (unsigned int i = 0; i < _root["data"].size(); i++)
                    {
                        if (_root["data"][i]["used"].asBool() == true)
                        {

                            origin[0] = _root["data"][i]["boundary"][j]["lon"].asDouble();
                            origin[1] = _root["data"][i]["boundary"][j]["lat"].asDouble();
                            origin[2] = _root["data"][i]["boundary"][j]["height"].asDouble();
                            ob["obstacle"] = _root["data"][i]["obstacle"];
                        }
                    }
                }
                mission = path_planning(mission_, origin, ob);
                write_mission(mission);//break;//##

                //std::fstream file("F:/gcrobot_server/api/mission/group/mission.json",std::ios::out);//mission也要删盘
                //std::fstream file("F:/gcrobot_server/api/mission/group/mission_result.json",std::ios::out); //路径集合算完删盘
                std::ofstream map_set("F:/gcrobot_server/api/maps/map_set.json", std::ios::binary);
                map_set << "{\"set\":true}";
                map_set.close();
                if (mission_check())   /*收到车反馈收到任务的信号*/
                    break;
            }
            else
            {
                std::cout << "暂未发布机器人任务..." << std::endl;
                fin_.close();
                Sleep(500);
            }
        }
    
        try
        {
            auto const address = net::ip::make_address("0.0.0.0");
            auto const port = static_cast<unsigned short>(std::atoi("8001"));
            net::io_context ioc{ 1 };
            tcp::acceptor acceptor{ ioc, {address, port} };
            for (;;)
            {
                tcp::socket socket{ ioc };
                acceptor.accept(socket);
                std::thread(
                    &do_session,
                    std::move(socket)).detach();
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
   

}

