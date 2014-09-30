#ifndef I2PTUNNEL_H__
#define I2PTUNNEL_H__

#include <inttypes.h>
#include <string>
#include <set>
#include <boost/asio.hpp>
#include "Identity.h"
#include "Streaming.h"

namespace i2p
{
namespace stream
{
	const size_t I2P_TUNNEL_CONNECTION_BUFFER_SIZE = 8192;
	const int I2P_TUNNEL_CONNECTION_MAX_IDLE = 3600; // in seconds	

	class I2PTunnel;
	class I2PTunnelConnection
	{
		public:

			I2PTunnelConnection (I2PTunnel * owner, boost::asio::ip::tcp::socket * socket,
				const i2p::data::LeaseSet * leaseSet);
			I2PTunnelConnection (I2PTunnel * owner, Stream * stream,  boost::asio::ip::tcp::socket * socket, 
				const boost::asio::ip::tcp::endpoint& target); 
			~I2PTunnelConnection ();

		private:

			void Terminate ();	

			void Receive ();
			void HandleReceived (const boost::system::error_code& ecode, std::size_t bytes_transferred);	
			void HandleWrite (const boost::system::error_code& ecode);	

			void StreamReceive ();
			void HandleStreamReceive (const boost::system::error_code& ecode, std::size_t bytes_transferred);
			void HandleConnect (const boost::system::error_code& ecode);

		private:

			uint8_t m_Buffer[I2P_TUNNEL_CONNECTION_BUFFER_SIZE], m_StreamBuffer[I2P_TUNNEL_CONNECTION_BUFFER_SIZE];
			boost::asio::ip::tcp::socket * m_Socket;
			Stream * m_Stream;
			I2PTunnel * m_Owner;
	};	

	class I2PTunnel
	{
		public:

			I2PTunnel (boost::asio::io_service& service): m_Service (service) {};
			virtual ~I2PTunnel () { ClearConnections (); }; 

			void AddConnection (I2PTunnelConnection * conn);
			void RemoveConnection (I2PTunnelConnection * conn);	
			void ClearConnections ();
			
			boost::asio::io_service& GetService () { return m_Service; };
			
		private:

			boost::asio::io_service& m_Service;
			std::set<I2PTunnelConnection *> m_Connections;
	};	
	
	class I2PClientTunnel: public I2PTunnel
	{
		public:

			I2PClientTunnel (boost::asio::io_service& service, const std::string& destination, int port);
			~I2PClientTunnel ();				
	
			void Start ();
			void Stop ();

		private:

			void Accept ();
			void HandleAccept (const boost::system::error_code& ecode, boost::asio::ip::tcp::socket * socket);
			
		private:

			boost::asio::ip::tcp::acceptor m_Acceptor;
			std::string m_Destination;
			const i2p::data::IdentHash * m_DestinationIdentHash;
			const i2p::data::LeaseSet * m_RemoteLeaseSet;
	};	

	class I2PServerTunnel: public I2PTunnel
	{
		public:

			I2PServerTunnel (boost::asio::io_service& service, const std::string& address, int port, 
				const i2p::data::IdentHash& localDestination);	

			void Start ();
			void Stop ();

		private:

			void Accept ();
			void HandleAccept (Stream * stream);

		private:

			StreamingDestination * m_LocalDestination;	
			boost::asio::ip::tcp::endpoint m_Endpoint;		
	};
}		
}	

#endif
