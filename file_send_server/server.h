#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <vector>
#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <string>


class Server{
    typedef Server this_type;
    typedef boost::asio::ip::tcp::acceptor acceptor_type;
    typedef boost::asio::ip::tcp::endpoint endpoint_type;
    typedef boost::asio::ip::tcp::socket socket_type;
    typedef boost::asio::ip::address address_type;
    typedef boost::shared_ptr<socket_type> sock_ptr;

private:
    boost::asio::io_service m_io;
    acceptor_type m_acceptor;
    std::vector<char> m_buf;

public:
    Server():
        m_acceptor(m_io, endpoint_type(boost::asio::ip::tcp::v4(),6688)),
        m_buf(100,0)
    {
        accept();
    }

    void run()
    {
        m_io.run();
    }

    void accept()
    {
        sock_ptr sock(new socket_type(m_io));

        m_acceptor.async_accept(*sock,
                                boost::bind(&this_type::accept_handler, this,
                                            boost::asio::placeholders::error, sock));
    }

    void accept_handler(const boost::system::error_code& ec,
                        sock_ptr sock)
    {
        if(ec)
            return;
        std::cout<<"client:"<<sock->remote_endpoint().address()<<std::endl;

        //接受用户的用户名和密码
        sock->async_read_some(boost::asio::buffer(m_buf),
                              boost::bind(&this_type::name_password, this,
                                          boost::asio::placeholders::error, sock));

        accept();
    }

    void name_password(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec)
            return;

        //解析出用户名和密码
        std::cout<<&m_buf[0]<<std::endl;

        int len=0;
        int i;
        for(i=0; i<4; i++){
            if(m_buf[i]!='\\')
            {
                len=len*10+(m_buf[i]-'0');
            }
        }
        std::cout<<"len="<<len<<std::endl;  //数据大小

        std::string username;
        for(;i<len+4;i++){
            if(m_buf[i]!=' '){
                username.push_back(m_buf[i]);
            }else{
                break;
            }
        }
        std::cout<<"username="<<username<<std::endl;    //用户名字

        std::cout<<"i="<<i<<std::endl;

        std::string password;
        for(i=i+1; i<len+4; i++)
        {

            password.push_back(m_buf[i]);
        }
        std::cout<<"password="<<password<<std::endl;    //用户密码


        //和数据库中的用户进行验证
        int flag=0;     //0表示登录成功    1表示密码错误    2表示不存在该用户
        //flag=数据库函数
        if(flag==0){
            sock->async_write_some(boost::asio::buffer("0"),
                                   boost::bind(&this_type::read_and_send, this,
                                               boost::asio::placeholders::error, sock));
        }else if(flag==1)
        {
            sock->async_write_some(boost::asio::buffer("1"),
                                   boost::bind(&this_type::handle1, this,
                                               boost::asio::placeholders::error, sock));
        }else if(flag==2)
        {
            sock->async_write_some(boost::asio::buffer("2"),
                                   boost::bind(&this_type::handle2, this,
                                               boost::asio::placeholders::error, sock));
        }

    }

    void handle1(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec)
            return;
        std::cout<<"密码错误."<<std::endl;
    }

    void handle2(const boost::system::error_code& ec, sock_ptr sock)
    {
        if(ec)
            return;
        std::cout<<"该用不存在."<<std::endl;
    }

    void read_and_send(const boost::system::error_code& ec, sock_ptr sock)
    {
        std::ifstream infile;
        infile.open("/run/media/root/study/graduation_project/asio/hello.txt");
        assert(infile.is_open());

        std::string s;
        while(getline(infile, s))
        {
            sock->async_write_some(boost::asio::buffer(s),
                                   boost::bind(&this_type::send_file, this,
                                               boost::asio::placeholders::error));

        }
        infile.close();
    }

    void write_handler(const boost::system::error_code& )
    {
        std::cout<<"send msg complete."<<std::endl;
    }

    void send_file(const boost::system::error_code&)
    {
        std::cout<<"send file complete."<<std::endl;
    }
};

#endif // SERVER_H















