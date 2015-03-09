#ifndef _I2P_HTTP_AUTO_H
#define _I2P_HTTP_AUTO_H
#include <memory>
#include <boost/asio.hpp>
namespace i2p {
namespace proxy {

class autoconf
{
using acceptor=boost::asio::ip::tcp::acceptor;
using socket=boost::asio::ip::tcp::socket;
using address=boost::asio::ip::address_v4;
using peer=boost::asio::ip::tcp::endpoint;
using err=boost::system::error_code;
public:
	template<typename SERVICE>
	autoconf(SERVICE *master, int port) {
		std::ostringstream cmd;
		cmd<<"function FindProxyForURL(url, host) {\r\n"
			<<"	if(shExpMatch(host, \"*.i2p\"))\r\n"
			<<"		return \"PROXY "
			<<"213.239.212.111:"<<port<<"\";\r\n"
			<<"	else\r\n"
			<<"		return \"DIRECT\";\r\n"
			<<"}\r\n";
		_cmd=cmd.str();


	
	
		std::shared_ptr<acceptor> ac{new acceptor{master->GetService(), peer(address::any(), port)}};
		restart(ac);
	}
	
private:
	void restart(std::shared_ptr<acceptor> ac) {
		std::shared_ptr<socket> s{new socket{ac->get_io_service()}};
		ac->async_accept(*s, [=](const err& x){
			if(x)
				return;
			respond(s);
			restart(ac);
		});
	}
	void respond(std::shared_ptr<socket> s) {
		char buf[2048];
		err x;
		s->read_some(boost::asio::buffer(buf), x);
		if(!x)
			boost::asio::write(*s, boost::asio::buffer(_cmd));
	}
	
	std::string _cmd;
};

}}
typedef i2p::proxy::autoconf HTTPAutoConf;

#endif
