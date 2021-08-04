
/*httpЭ��ķ������������������ն˽�������滮�����Ʒ�����*/

/*
boost asio����http���÷����£�
1 ��main�����е���server��Ĺ��캯�� ��������
2 ��server���е�����connection_manager��
3 ��connection_manager���е�����connection ��������ص�Ҫ��ע��һ���� ������
  ���ݵĶ�ȡdo_read()�����Լ����ݵ�д��do_write()����
  �����ݵĶ�ȡ���ڲ�����������request_parser���ڽ������������
  ��������󣬵���request_handler�н��н������ݵķ�������������reply�������Ӧ���ݵķ���
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
	const std::string ip = "0.0.0.0";      //����һ������
	const std::string port = "80";	       //���ն�һ��
	const std::string docroot = "F:/gcrobot_server";   //��������Ҫ���ŵ���/�ļ���
	 //1 ��һ����main��������server�� �������������ǳ�ʼ��server�еĹ��캯�� ��һ����server.cpp��
	http::server::server s(ip, port, docroot);
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }
  return 0;
}
