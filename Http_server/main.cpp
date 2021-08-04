
/*http协议的服务器，负责与手提终端进行任务规划、控制发布等*/

/*
boost asio创建http的用法如下：
1 在main函数中调用server类的构造函数 启动服务
2 在server类中调用了connection_manager类
3 在connection_manager类中调用了connection 这个类是重点要关注的一个类 包含了
  数据的读取do_read()函数以及数据的写入do_write()函数
  在数据的读取中内部调用两个类request_parser用于解析请求的数据
  当解析完后，调用request_handler中进行接收数据的分析，进而调用reply类进行响应数据的发送
*/

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include <boost/bind.hpp>

int main(int argc, char* argv[])
{                
  try
  {
	const std::string ip = "0.0.0.0";      //允许一切连接
	const std::string port = "80";	       //跟终端一致
	const std::string docroot = "F:/gcrobot_server";   //开放你想要开放的盘/文件夹
	 //1 第一步在main函数调用server类 下面这条语句就是初始化server中的构造函数 下一步到server.cpp中
	http::server::server s(ip, port, docroot);
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }
  return 0;
}
