
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

std::map<std::string ,std::string>  record2map(std::string &str) /*�ж�get����url�����޴��εĺ����������˿����Ż���������ʽ*/
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

void request_handler::handle_request_POST(const request& req, reply& rep)/*post����Ļص�����*/
{
   std::cout << std::endl;
   std::cout << "�յ��ն�POST����" << std::endl;
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
  if (request_path == "/api/missions/coverage_path/")    /*·�����������*/
  {
      std::cout << "request body��" << req.body << std::endl;
      Json::Value root; 
      writeFileJson_mission_allo(req.body);  
      for(;;)
      {
          std::cout << "�����\n";
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
              root[zero][i][j]["position"]["y"]=root[zero][i][j]["position"]["y"].asDouble() - 4.0;  /*�����ն˸�����˷��ص������y����ƫ��ģ��ղ���һ���в������ϵftq*/
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


  if (request_path == "/api/missions/group/set/") /*ȷ�Ϸ������������*/
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
  
  if (request_path == "/api/missions/move_to_point_plan/")/*����ǰ������㹦�ܵ����󣬹���û��*/
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


  if (request_path == "/api/missions/move_to_point_exec/")/*ȷ��ǰ�����������󣬹���û��*/
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

  if (request_path == "/api/robots/system_config/")/*���̿��Ƶ�����*/
  {
      Json::Reader reader; Json::Value root;
      if (reader.parse(req.body, root))
      {
          if (root["cutter_height"].isNull())
              cuttercluth(req.body);            //�����������
          else
              cuttercontrol(req.body);          //������������
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
  if (request_path == "/api/robots/navigation-mode/")/*����ģʽ�л�����*/
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
  if (request_path == "/api/robots/control/")/*�������˶���������*/
  {                   
      remotecontrol(req.body);    //�ģ�����HTTP�ʹ��ڹ����Ѿ����룬������websocket����沢����HTTP��������������  
      std::cout << "�������յ�ң��ָ�������˷���ң��ָ��..." << std::endl;
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
void request_handler::handle_request_GET(const request& req, reply& rep)/*�����ն˷��͵�get����*/
{
    std::cout << "�յ��ն�GET����" << std::endl;
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
     // �����Σ�û�в������ؿձ�
     std::map<std::string, std::string > pv_map = record2map(request_path);
     printMap(pv_map);
     //ȥ������
     request_path = request_path.substr(0, request_path.find('?'));

     if (request_path == "/api/robots/") /*�ն�ͬ������������״̬�����󣬴��ڶ�Ӧ·���µ�json�ļ���*/
     {
         readFileJson_robots(pv_map);   
         if (request_path[request_path.size() - 1] == '/')
         {
             request_path += "robots_state.json";
         }
     }
     if (request_path == "/api/maps/")   /*�ն�ͬ��������ͼ�����󣬴��ڶ�Ӧ·���µ�json�ļ���*/
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
     if (request_path == "/api/maps/areas/")   /*�ϰ�����Ϣ,�滮��Ҫ�ܿ���,���ܻ�û��*/
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

     if (request_path == "/api/maps/points/")  /*��������*/
     {
         //�ݲ���Ҫ
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
bool request_handler::url_decode(const std::string& in, std::string& out)/*���뺯��*/
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
