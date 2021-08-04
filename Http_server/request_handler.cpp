
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
#include "cstjson.hpp"
#include "remote.hpp"
#include <vector>

std::string param ;
std::string value ;
void printMap(std::map<std::string, std::string>& m)
{
    for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++)
    {
        std::cout << "key = " << it->first << " value = " << it->second << std::endl;
    }
}

std::map<std::string ,std::string>  record2map(std::string &str) /*判断get请求url中有无传参的函数，有心人可以优化成正则表达式*/
{
    std::string fake(str);     
    std::vector<std::string> name;    
    std::vector<std::string> value;
    std::map<std::string, std::string> m;

    int flag = str.find('&');                 
    if (flag > 0) 
    {
        name.push_back(fake.substr(str.find('?')+1  , str.find('=') - str.find('?')-1));
        value.push_back(fake.substr(str.find('=')+1  , str.find('&') - str.find('=')-1)); 
        fake = fake.substr(str.find('&') + 1);
        while ((int)fake.find('&') > 0) 
        {
            name.push_back(fake.substr(0, fake.find('=')));
            value.push_back(fake.substr(fake.find('=')+1, fake.find('&') - fake.find('=')-1));
            fake = fake.substr(fake.find('&') + 1);
            
        }
        name.push_back(fake.substr(0, fake.find('=')));
        value.push_back(fake.substr(fake.find('=')+1, fake.size() - fake.find('=')-1)) ;    
        for (int c = 0; c < name.size(); c++) 
            m.insert(pair<string, string>(name[c], value[c])); 
        return m;
     }
    else if((int)str.find('?') > 0)
    {
        name.push_back(fake.substr(fake.find('?')+1, fake.find('=') - fake.find('?')-1));
        value.push_back(fake.substr(fake.find('=')+1, fake.size() - fake.find('=')-1));
        m.insert(pair<std::string, std::string>(name[0], value[0]));  
        return m;
    }
    else  
        return m;
}

namespace http {
namespace server {

request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root)
{
}

void request_handler::handle_request_POST(const request& req, reply& rep)/*post请求的回调函数*/
{
   std::cout << std::endl;
   std::cout << "收到终端POST请求" << std::endl;
   std::cout << req.body << std::endl;
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }
 
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }
  if (request_path == "/api/missions/coverage_path/")    /*路径计算的请求*/
  {
      std::cout << "request body：" << req.body << std::endl;
      Json::Value root; 
      writeFileJson_mission_allo(req.body);  
      for(;;)
      {
          std::cout << "检查中\n";
          std::fstream fi("F:/gcrobot_server/api/mission/group/mission_result.json", std::ios::binary);
          fi.seekg(0, ios::end); 
          streampos fp = fi.tellg(); 
          if (int(fp) != 0) 
          {
              fi.close();
              Json::Reader reader;
              std::ifstream json("F:/gcrobot_server/api/mission/group/mission_result.json",std::ios::in|std::ios::binary);
              reader.parse(json, root);
              json.close();
              break;
          }
      }
      int zero = 0;
      for (size_t i = 0; i < root[zero].size(); i++)
      {
          for (size_t j = 0; j < root[zero][i].size(); j++)
          {
              root[zero][i][j]["position"]["y"]=root[zero][i][j]["position"]["y"].asDouble() - 4.0;  /*手提终端给服务端返回的任务点y是有偏差的，凑补了一个残差，详情联系ftq*/
          }
      }
      rep.status = reply::ok;
      Json::Value _;
      _["code"] = Json::Value(20000);
      _["data"] = Json::Value(root.toStyledString());
      std::cout << "coverage_path: \n" << root.toStyledString() << std::endl;
     
      std::fstream fin_;
      //fin_.open("F:/gcrobot_server/api/mission/group/mission_result.json", std::ios::out);
      fin_.close(); fin_.clear();
      rep.content.append(_.toStyledString());
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = mime_types::extension_to_type("json");
      return;
  }


  if (request_path == "/api/missions/group/set/") /*确认发布任务的请求*/
  {
      std::ofstream os;
      Json::Value root;
      root["set"] = Json::Value(true);
      root["sn"] = Json::Value("123456");
      Json::FastWriter w;
      os.open("F:/gcrobot_server/api/mission/group/mission_set.json");
      os << w.write(root);
      os.close();
      return;
  }
  
  if (request_path == "/api/missions/move_to_point_plan/")/*设置前往特殊点功能的请求，功能没上*/
  {
      //writeFileJson_pointplan();
      Json::Reader reader; Json::Value root;

      if (reader.parse(req.body, root))
      {
          Json::Value point = root["position"];
      }
      rep.status = reply::ok;
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      Json::Value _;
      _["code"] = Json::Value(20000);
      _["data"] = Json::Value(NULL);
      rep.content.append(_.toStyledString());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = mime_types::extension_to_type("json"); 
      return;
  }


  if (request_path == "/api/missions/move_to_point_exec/")/*确定前往特殊点的请求，功能没上*/
  {
      //readFileJson_pointexec();
      rep.status = reply::ok;
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      Json::Value _;
      _["code"] = Json::Value(20000);
      _["data"] = Json::Value(NULL);
      rep.content.append(_.toStyledString());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = mime_types::extension_to_type("json");
      return;
  }

  if (request_path == "/api/robots/system_config/")/*刀盘控制的请求*/
  {
      Json::Reader reader; Json::Value root;
      if (reader.parse(req.body, root))
      {
          if (root["cutter_height"].isNull())
              cuttercluth(req.body);            //刀盘离合命令
          else
              cuttercontrol(req.body);          //刀盘升降命令
          rep.status = reply::ok;
          rep.headers.resize(2);
          rep.headers[0].name = "Content-Length";
          rep.headers[0].value = std::to_string(rep.content.size());
          Json::Value _;
          _["code"] = Json::Value(20000);
          _["data"] = Json::Value(NULL);
          rep.content.append(_.toStyledString());
          rep.headers[1].name = "Content-Type";
          rep.headers[1].value = mime_types::extension_to_type("json");
      }  
      return;
  }
  if (request_path == "/api/robots/navigation-mode/")/*控制模式切换请求*/
  {
      Json::Reader reader; Json::Value root;
      if (reader.parse(req.body, root))
          modechange();
      rep.status = reply::ok;
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      Json::Value _;
      _["code"] = Json::Value(20000);
      _["data"] = Json::Value(NULL);
      rep.content.append(_.toStyledString());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = mime_types::extension_to_type("json");
      return;
  }
  if (request_path == "/api/robots/control/")/*方向与运动控制请求*/
  {                   
      remotecontrol(req.body);    //改：现在HTTP和串口功能已经分离，串口由websocket服务兼并处理。HTTP服务仅将命令存盘  
      std::cout << "服务器收到遥控指令，向机器人发送遥控指令..." << std::endl;
      rep.status = reply::ok;
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());   
      Json::Value _;
      _["code"] = Json::Value(20000);
      _["data"] = Json::Value(NULL);
      rep.content.append(_.toStyledString());
      rep.headers[1].name = "Content-Type";
      rep.headers[1].value = mime_types::extension_to_type("json"); 
      return;
  }
   
  std::string full_path = doc_root_ + request_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
      rep = reply::stock_reply(reply::not_found);
      return;
  }
  rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
      rep.content.append(buf, is.gcount());
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = mime_types::extension_to_type("json");    
}

/*----------------------------------------------------------------------------------------------------------------------*/
void request_handler::handle_request_GET(const request& req, reply& rep)/*手提终端发送的get请求*/
{
    std::cout << "收到终端GET请求" << std::endl;
    std::string request_path;
    if (!url_decode(req.uri, request_path))
    {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }
    if (request_path.empty() || request_path[0] != '/'
        || request_path.find("..") != std::string::npos)
    {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }
     // 处理传参，没有参数返回空表
     std::map<std::string, std::string > pv_map = record2map(request_path);
     printMap(pv_map);
     //去除参数
     request_path = request_path.substr(0, request_path.find('?'));

     if (request_path == "/api/robots/") /*终端同步本机机器人状态的请求，存在对应路径下的json文件里*/
     {
         readFileJson_robots(pv_map);   
         if (request_path[request_path.size() - 1] == '/')
         {
             request_path += "robots_state.json";
         }
     }
     if (request_path == "/api/maps/")   /*终端同步本机地图的请求，存在对应路径下的json文件里*/
     {
         std::string used = writeFileJson_maps(pv_map); 
         if (pv_map.empty())
             request_path += used;
         else
         {
             rep.status = reply::ok;
             const std::string resp = used;
             std::cout << resp << std::endl;
             rep.content.append(resp);
             rep.headers.resize(2);
             rep.headers[0].name = "Content-Length";
             rep.headers[0].value = std::to_string(rep.content.size());
             rep.headers[1].name = "Content-Type";
             rep.headers[1].value = mime_types::extension_to_type("json"); 
             pv_map.erase(pv_map.begin(), pv_map.end());
             pv_map.clear();
             return;
         }
     }
     if (request_path == "/api/maps/areas/")   /*障碍物信息,规划需要避开等,功能还没上*/
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
                     std::string areas = leaf.toStyledString();
                     break;
                 }
             }
         }
         _.close();
     }

     if (request_path == "/api/maps/points/")  /*特殊任务*/
     {
         //暂不需要
     }
     pv_map.erase(pv_map.begin(), pv_map.end());
     pv_map.clear();
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }
    std::string full_path = doc_root_ + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary); 
    if (!is)
    {
        rep = reply::stock_reply(reply::not_found);
        return;
    }
    rep.status = reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)   
        rep.content.append(buf, is.gcount());  

    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = mime_types::extension_to_type(extension);
}
bool request_handler::url_decode(const std::string& in, std::string& out)/*解码函数*/
{
  out.clear();
  out.reserve(in.size());
  std::cout << in << std::endl;

  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

} // namespace server
} // namespace http
