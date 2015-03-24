#ifndef ROUTER_CONTEXT_H__
#define ROUTER_CONTEXT_H__

#include <inttypes.h>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <cryptopp/dsa.h>
#include <cryptopp/osrng.h>
#include "Identity.h"
#include "RouterInfo.h"
#include "Garlic.h"

namespace i2p
{
	const char ROUTER_INFO[] = "router.info";
	const char ROUTER_KEYS[] = "router.keys";	
	const int ROUTER_INFO_UPDATE_INTERVAL = 1800; // 30 minutes
	
	const char ROUTER_INFO_PROPERTY_LEASESETS[] = "netdb.knownLeaseSets";
	const char ROUTER_INFO_PROPERTY_ROUTERS[] = "netdb.knownRouters";		

	enum RouterStatus
	{
		eRouterStatusOK = 0,
		eRouterStatusTesting = 1,
		eRouterStatusFirewalled = 2
	};	

	class RouterContext: public i2p::garlic::GarlicDestination 
	{
		public:

			RouterContext ();
			void Init ();

			i2p::data::RouterInfo& GetRouterInfo () { return m_RouterInfo; };
			std::shared_ptr<const i2p::data::RouterInfo> GetSharedRouterInfo () const 
			{ 
				return std::shared_ptr<const i2p::data::RouterInfo> (&m_RouterInfo, 
					[](const i2p::data::RouterInfo *) {});
			}
			CryptoPP::RandomNumberGenerator& GetRandomNumberGenerator () { return m_Rnd; };	
			uint32_t GetUptime () const;
			uint32_t GetStartupTime () const { return m_StartupTime; };
			uint64_t GetLastUpdateTime () const { return m_LastUpdateTime; };
			RouterStatus GetStatus () const { return m_Status; };
			void SetStatus (RouterStatus status) { m_Status = status; };

			void UpdatePort (int port); // called from Daemon
			void UpdateAddress (const boost::asio::ip::address& host);	// called from SSU or Daemon
			bool AddIntroducer (const i2p::data::RouterInfo& routerInfo, uint32_t tag);
			void RemoveIntroducer (const boost::asio::ip::udp::endpoint& e);
			bool IsUnreachable () const;
			void SetUnreachable ();		
			void SetReachable ();
			bool IsFloodfill () const { return m_IsFloodfill; };	
			void SetFloodfill (bool floodfill);	
			void SetHighBandwidth ();
			void SetLowBandwidth ();
			bool AcceptsTunnels () const { return m_AcceptsTunnels; };
			void SetAcceptsTunnels (bool acceptsTunnels) { m_AcceptsTunnels = acceptsTunnels; };
			bool SupportsV6 () const { return m_RouterInfo.IsV6 (); };
			void SetSupportsV6 (bool supportsV6);
			void UpdateNTCPV6Address (const boost::asio::ip::address& host); // called from NTCP session		
			void UpdateStats ();		

			// implements LocalDestination
			const i2p::data::PrivateKeys& GetPrivateKeys () const { return m_Keys; };
			const uint8_t * GetEncryptionPrivateKey () const { return m_Keys.GetPrivateKey (); };
			const uint8_t * GetEncryptionPublicKey () const { return GetIdentity ().GetStandardIdentity ().publicKey; };
			void SetLeaseSetUpdated () {};

			// implements GarlicDestination
			const i2p::data::LeaseSet * GetLeaseSet () { return nullptr; };
			void HandleI2NPMessage (const uint8_t * buf, size_t len, std::shared_ptr<i2p::tunnel::InboundTunnel> from);
			
		private:

			void CreateNewRouter ();
			void NewRouterInfo ();
			void UpdateRouterInfo ();
			bool Load ();
			void SaveKeys ();
			
		private:

			i2p::data::RouterInfo m_RouterInfo;
			i2p::data::PrivateKeys m_Keys; 
			CryptoPP::AutoSeededRandomPool m_Rnd;
			uint64_t m_LastUpdateTime;
			bool m_AcceptsTunnels, m_IsFloodfill;
			uint64_t m_StartupTime; // in seconds since epoch
			RouterStatus m_Status;
	};

	extern RouterContext context;
}	

#endif
