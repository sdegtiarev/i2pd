#ifndef BOB_H__
#define BOB_H__

#include <inttypes.h>
#include <thread>
#include <memory>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include "I2PTunnel.h"
#include "Identity.h"
#include "LeaseSet.h"

namespace i2p
{
namespace client
{
	const size_t BOB_COMMAND_BUFFER_SIZE = 1024;
	const char BOB_COMMAND_ZAP[] = "zap";
	const char BOB_COMMAND_QUIT[] = "quit";
	const char BOB_COMMAND_START[] = "start";
	const char BOB_COMMAND_SETNICK[] = "setnick";	
	const char BOB_COMMAND_NEWKEYS[] = "newkeys";	
	const char BOB_COMMAND_OUTHOST[] = "outhost";	
	const char BOB_COMMAND_OUTPORT[] = "outport";
	const char BOB_COMMAND_INHOST[] = "inhost";	
	const char BOB_COMMAND_INPORT[] = "inport";
	
	const char BOB_REPLY_OK[] = "OK %s\n";
	const char BOB_REPLY_ERROR[] = "ERROR %s\n";

	class BOBI2PInboundTunnel: public I2PTunnel
	{
		public:

			BOBI2PInboundTunnel (boost::asio::io_service& service, int port, ClientDestination * localDestination);
			~BOBI2PInboundTunnel ();

			void Start ();
			void Stop ();

		private:

			void Accept ();
			void HandleAccept (const boost::system::error_code& ecode, boost::asio::ip::tcp::socket * socket);

			void ReceiveAddress (boost::asio::ip::tcp::socket * socket);
			void HandleReceivedAddress (const boost::system::error_code& ecode, std::size_t bytes_transferred,
				boost::asio::ip::tcp::socket * socket);

			void HandleDestinationRequestTimer (const boost::system::error_code& ecode, boost::asio::ip::tcp::socket * socket, i2p::data::IdentHash ident);

			void CreateConnection (boost::asio::ip::tcp::socket * socket, const i2p::data::LeaseSet * leaseSet);

		private:

			boost::asio::ip::tcp::acceptor m_Acceptor;	
			boost::asio::deadline_timer m_Timer;
			char m_ReceiveBuffer[BOB_COMMAND_BUFFER_SIZE + 1]; // for destination base64 address
	};

	class BOBCommandChannel;
	class BOBCommandSession: public std::enable_shared_from_this<BOBCommandSession>
	{
		public:

			BOBCommandSession (BOBCommandChannel& owner);
			~BOBCommandSession ();	
			void Terminate ();

			boost::asio::ip::tcp::socket& GetSocket () { return m_Socket; };
			void Receive ();

			// command handlers
			void ZapCommandHandler (const char * operand, size_t len);
			void QuitCommandHandler (const char * operand, size_t len);
			void StartCommandHandler (const char * operand, size_t len);
			void SetNickCommandHandler (const char * operand, size_t len);
			void NewkeysCommandHandler (const char * operand, size_t len);
			void OuthostCommandHandler (const char * operand, size_t len);
			void OutportCommandHandler (const char * operand, size_t len);
			void InhostCommandHandler (const char * operand, size_t len);
			void InportCommandHandler (const char * operand, size_t len);			

		private:

			void HandleReceived (const boost::system::error_code& ecode, std::size_t bytes_transferred);

			void Send (size_t len);
			void HandleSent (const boost::system::error_code& ecode, std::size_t bytes_transferred);
			void SendReplyOK (const char * msg);
			void SendReplyError (const char * msg);

		private:

			BOBCommandChannel& m_Owner;
			boost::asio::ip::tcp::socket m_Socket;
			char m_ReceiveBuffer[BOB_COMMAND_BUFFER_SIZE + 1], m_SendBuffer[BOB_COMMAND_BUFFER_SIZE + 1];
			size_t m_ReceiveBufferOffset;
			bool m_IsOpen, m_IsOutbound;
			std::string m_Nickname, m_Address;
			int m_Port;
			i2p::data::PrivateKeys m_Keys;
	};
	typedef void (BOBCommandSession::*BOBCommandHandler)(const char * operand, size_t len);

	class BOBCommandChannel
	{
		public:

			BOBCommandChannel (int port);
			~BOBCommandChannel ();

			void Start ();
			void Stop ();

			boost::asio::io_service& GetService () { return m_Service; };
			std::map<std::string, BOBCommandHandler>& GetCommandHandlers () { return m_CommandHandlers; };
			void AddTunnel (const std::string& name, I2PTunnel * tunnel);
			
		private:

			void Run ();
			void Accept ();
			void HandleAccept(const boost::system::error_code& ecode, std::shared_ptr<BOBCommandSession> session);

		private:

			bool m_IsRunning;
			std::thread * m_Thread;	
			boost::asio::io_service m_Service;
			boost::asio::ip::tcp::acceptor m_Acceptor;
			std::map<std::string, I2PTunnel *> m_Tunnels;
			std::map<std::string, BOBCommandHandler> m_CommandHandlers;
	};	
}
}

#endif

