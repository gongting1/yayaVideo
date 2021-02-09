#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <vector>

#include <fstream>

class Client
{
    typedef Client this_type;
    typedef boost::asio::ip::tcp::endpoint endpoint_type;
    typedef boost::asio::ip::tcp::socket socket_type;
    typedef boost::asio::ip::address address_type;
    typedef boost::shared_ptr<socket_type> sock_ptr;
    typedef std::vector<char> buffer_type;

private:
    boost::asio::io_service m_io;
    buffer_type m_buf;
    endpoint_type m_ep;

public:
    Client():
        m_buf(100,0),
        m_ep(address_type::from_string("127.0.0.1"), 6688)
    {
        start();
    }

    void run()
    {
        m_io.run();
    }
    void start()
    {
        sock_ptr sock(new socket_type(m_io));
        sock->async_connect(m_ep,
                            boost::bind(&this_type::conn_handler,this,
                                              boost::asio::placeholders::error, sock));
    }

    void conn_handler(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec)
            return;

        std::cout<<"send to "<<sock->remote_endpoint().address()<<std::endl;

        //登录
        //发送用户名和密码
        //用户名和密码不能含有空格，用户名、密码不超过20个字符
        std::string username, password;
        username="gongting";
        password="12345";
        std::string name_and_password=std::to_string(username.length()+password.length()+1);//用户名和密码用空格分开


        std::vector<char> send_name_password;
        int i=0;
        for(auto it:name_and_password){     //头部装入大小
            send_name_password.push_back(it);
            i++;
        }
        if(i<4){
            for(;i<4; i++)
                send_name_password.push_back('\\');
        }

        for(auto it:username){
            send_name_password.push_back(it);
        }
        send_name_password.push_back(' ');
        for(auto it:password)
        {
            send_name_password.push_back(it);
        }

        std::cout<<&send_name_password[0]<<std::endl;
        sock->async_write_some(boost::asio::buffer(send_name_password),
                               boost::bind(&this_type::write_handler1, this,
                                           boost::asio::placeholders::error, sock));

        //检查是否登录成功
        sock->async_read_some(boost::asio::buffer(m_buf),
                              boost::bind(&this_type::read_handler1, this,
                                          boost::asio::placeholders::error, sock));
    }

    void write_handler1(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec)
            return;
        std::cout<<"发送成功"<<std::endl;
    }
    void read_handler1(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec)
            return;
        if(m_buf[0]=='0')
        {
            std::cout<<"登录成功！"<<std::endl;
            read_handler(ec, sock);
        }else if(m_buf[0]=='1'){
            std::cout<<"密码错误！"<<std::endl;
        }else if(m_buf[0]=='2')
        {
            std::cout<<"该用户不存在！"<<std::endl;
        }
    }


    void read_handler(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec){
            return;
        }

        //接受视频文件
        std::ofstream in;
        in.open("/run/media/root/study/graduation_project/asio/com.txt", std::ios::app);
        in<<&m_buf[0]<<std::endl;

        //缓冲区大小不够，继续调用该函数继续接受数据。
        sock->async_read_some(boost::asio::buffer(m_buf),
                              boost::bind(&Client::read_handler, this,
                                          boost::asio::placeholders::error, sock));
    }
};

#endif // CLIENT_H
