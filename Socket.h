//
//
//

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>



//
//
//

enum class SocketFlags
{
    PASSIVE = AI_PASSIVE,
    CANONICALNAME = AI_CANONNAME
};

enum class SocketFamily
{
    UNSPECIFIED = AF_UNSPEC,
    IPV4 = AF_INET,
    IPV6 = AF_INET6
};

enum class SocketType
{
    STREAM = SOCK_STREAM,
    DATAGRAM = SOCK_DGRAM
};

enum class SocketProtocol
{
    ANY = 0
};

// Internet address
typedef uint32_t IPV4ADDRESS;
typedef unsigned char IPV6ADDRESS[16];

typedef uint16_t PORT;

typedef uint32_t IPV6FLOWINFO;
typedef uint32_t IPV6SCOPEID;



//
//
//

class SocketParameterConverter
{
    public:

    static void getParam( int type, SocketFlags& flag )
    {
        switch ( type )
        {
            case AI_PASSIVE:
                flag = SocketFlags::PASSIVE;
                break;
                
            case AI_CANONNAME:
                flag = SocketFlags::CANONICALNAME;
                break;
                
            default:
                flag = SocketFlags::PASSIVE;
                break;
        }
    }
    
    static void getParam( int type, SocketFamily& family )
    {
        switch ( type )
        {
            case AF_UNSPEC:
                family = SocketFamily::UNSPECIFIED;
                break;
                
            case AF_INET:
                family = SocketFamily::IPV4;
                break;
                
            case AF_INET6:
                family = SocketFamily::IPV6;
                break;
                
            default:
                family = SocketFamily::UNSPECIFIED;
                break;
        }
    }
    
    static void getParam( int type, SocketType& socketType )
    {
        switch ( type )
        {
            case SOCK_STREAM:
                socketType = SocketType::STREAM;
                break;
                
            case SOCK_DGRAM:
                socketType = SocketType::DATAGRAM;
                break;
                
            default:
                socketType = SocketType::STREAM;
                break;
        }
    }
    
    static void getParam( int type, SocketProtocol& protocol )
    {
        switch ( type )
        {
            case 0:
                protocol = SocketProtocol::ANY;
                break;
                
            default:
                protocol = SocketProtocol::ANY;
                break;
        }
    }
    
    static void getParam( SocketFlags flag, int& type )
    {
        switch ( flag )
        {
            case SocketFlags::PASSIVE:
                type = AI_PASSIVE;
                break;
                
            case SocketFlags::CANONICALNAME:
                type = AI_CANONNAME;
                break;
            
            default:
                type = AI_PASSIVE;
        }
    }
    
    static void getParam( SocketFamily family, int& type )
    {
        switch ( family )
        {
            case SocketFamily::UNSPECIFIED:
                type = AF_UNSPEC;
                break;
                
            case SocketFamily::IPV4:
                type = AF_INET;
                break;
                
            case SocketFamily::IPV6:
                type = AF_INET6;
                break;
                
            default:
                type = AF_UNSPEC;
                break;
        }
    }
    
    static void getParam( SocketType socketType, int& type )
    {
        switch ( socketType )
        {
            case SocketType::STREAM:
                type = SOCK_STREAM;
                break;
                
            case SocketType::DATAGRAM:
                type = SOCK_DGRAM;
                break;
                
            default:
                type = SOCK_STREAM;
                break;
        }
    }
    
    static void getParam( SocketProtocol protocol, int& type )
    {
        switch ( protocol )
        {
            case SocketProtocol::ANY:
                type = 0;
                break;
                
            default:
                type = 0;
                break;
        }
    }
};



//
//
//

class SocketAddress
{
    public:
    
    SocketAddress() :
        mFlags( SocketFlags::PASSIVE ),
        mFamily( SocketFamily::UNSPECIFIED ),
        mSocketType( SocketType::STREAM ),
        mProtocol( SocketProtocol::ANY ),
        mPort( 0 ),
        mIPv4Address( 0 ),
        mIPv6FlowInfo( 0 ),
        mIPv6ScopeId( 0 )
    {
        memset( mIPv6Address, 0, sizeof( IPV6ADDRESS ) );
    }
    
    void setFlags( SocketFlags flags )
    {
        mFlags = flags;
    }
    
    void setFamily( SocketFamily family )
    {
        mFamily = family;
    }
    
    void setSocketType( SocketType socketType )
    {
        mSocketType = socketType;
    }
    
    void setProtocol( SocketProtocol protocol )
    {
        mProtocol = protocol;
    }
    
    void setPort( PORT port )
    {
        mPort = port;
    }
    
    void setIPv4Address( IPV4ADDRESS address )
    {
        mIPv4Address = address;
    }
    
    void setIPv4Address( const std::string& address )
    {
        struct sockaddr_in sa;
        
        inet_pton( AF_INET, address.c_str(), &(sa.sin_addr) );
        
        mIPv4Address = sa.sin_addr.s_addr;
    }
    
    void setIPv6Address( unsigned char address[16] )
    {
        memcpy( mIPv6Address, address, sizeof( unsigned char ) * 16 );
    }
    
    void setIPv6Address( const std::string& address )
    {
        struct sockaddr_in6 sa6;
        
        inet_pton( AF_INET6, address.c_str(), &(sa6.sin6_addr) );

        memcpy( mIPv6Address, sa6.sin6_addr.s6_addr, sizeof( unsigned char ) * 16 );
    }
    
    void setIPv6FlowInfo( IPV6FLOWINFO flowInfo )
    {
        mIPv6FlowInfo = flowInfo;
    }
    
    void setIPv6ScopeId( IPV6SCOPEID scopeID )
    {
        mIPv6ScopeId = scopeID;
    }
    
    void setCanonicalHostname( const std::string& canonicalName )
    {
        mCanonicalName = canonicalName;
    }
    
    SocketFlags getFlags() const
    {
        return mFlags;
    }
    
    SocketFamily getFamily() const
    {
        return mFamily;
    }
    
    SocketType getSocketType() const
    {
        return mSocketType;
    }
    
    SocketProtocol getProtocol() const
    {
        return mProtocol;
    }
    
    PORT getPort() const
    {
        return mPort;
    }
    
    void getIPv4Address( std::string& address ) const
    {
        char ipv4[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &mIPv4Address, ipv4, INET_ADDRSTRLEN);
        
        address = ipv4;
    }
    
    void getIPv4Address( IPV4ADDRESS& address ) const
    {
        address = mIPv4Address;
    }
    
    void getIPv6Address( std::string& address ) const
    {
        char ipv6[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &mIPv6Address, ipv6, INET6_ADDRSTRLEN);
        
        address = ipv6;
    }

    void getIPv6Address( IPV6ADDRESS& address ) const
    {
        memcpy( address, mIPv6Address, sizeof( unsigned char ) * 16 );
    }
    
    IPV6FLOWINFO getIPv6FlowInfo() const
    {
        return mIPv6FlowInfo;
    }
    
    IPV6SCOPEID getIPv6ScopeId() const
    {
        return mIPv6ScopeId;
    }
    
    std::string getCanonicalHostname() const
    {
        return mCanonicalName;
    }

    private:
    
    SocketFlags mFlags;
    SocketFamily mFamily;
    SocketType mSocketType;
    SocketProtocol mProtocol;
    PORT mPort;
    IPV4ADDRESS mIPv4Address;
    IPV6ADDRESS mIPv6Address;
    IPV6FLOWINFO mIPv6FlowInfo;
    IPV6SCOPEID mIPv6ScopeId;
    std::string mCanonicalName;
};



//
//
//

class Socket
{
    public:
    
    Socket( int socketDescriptor, PORT port, IPV4ADDRESS ipv4 ) :
        mSocketDescriptor( socketDescriptor ),
        mFamily( SocketFamily::IPV4 ),
        mPort( port ),
        mIPv4Address( ipv4 ),
        mIPv6FlowInfo( 0 ),
        mIPv6ScopeId( 0 )
    {
        memset( mIPv6Address, 0, sizeof( IPV6ADDRESS ) );
    }

    Socket( int socketDescriptor, PORT port, IPV6ADDRESS ipv6, IPV6FLOWINFO flowInfo, IPV6SCOPEID scopeId ) :
        mSocketDescriptor( socketDescriptor ),
        mFamily( SocketFamily::IPV6 ),
        mPort( port ),
        mIPv4Address( 0 ),
        mIPv6FlowInfo( flowInfo ),
        mIPv6ScopeId( scopeId )
    {
        memcpy( mIPv6Address, ipv6, sizeof( IPV6ADDRESS ) );
    }

    ~Socket()
    {
        ::close( mSocketDescriptor );
    }
    
    size_t send( const void* buffer, size_t size )
    {
        if ( mSocketDescriptor != -1 )
        {
            return ::send( mSocketDescriptor, buffer, size, 0 );
        }
        
        return 0;
    }
    
    size_t receive( void *buffer, size_t size )
    {
        if ( mSocketDescriptor )
        {
            return ::recv( mSocketDescriptor, buffer, size, 0 );
        }

        return 0;
    }
    
//    int sentTo( const SocketAddress& receiver )
//    {
//        ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
//        return 0;
//    }
//    
//    int receiveFrom( const SocketAddress& sender )
//    {
//        ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
//        return 0;
//    }
    

    private:

    int mSocketDescriptor;
    
    SocketFamily mFamily;
    PORT mPort;
    IPV4ADDRESS mIPv4Address;
    IPV6ADDRESS mIPv6Address;
    IPV6FLOWINFO mIPv6FlowInfo;
    IPV6SCOPEID mIPv6ScopeId;
};



//
//
//

class ServerSocket
{
    public:
    
    ServerSocket() : mSocketDescriptor(-1), mBacklog(10)
    {
    }

    ServerSocket( int backlog ) : mSocketDescriptor(-1), mBacklog( backlog )
    {
    }
    
    ~ServerSocket()
    {
        close();
    }
    
    void setBacklog( int backlog )
    {
        mBacklog = backlog;
    }
    
    bool setup( PORT port, SocketType socketType = SocketType::STREAM )
    {
        std::string strPort = std::to_string( port );

        return setup( strPort, socketType );
    }
    
    bool setup( const std::string& port, SocketType socketType = SocketType::STREAM )
    {
        struct addrinfo hints;
        struct addrinfo* serverInfo;
        int type = SOCK_STREAM;

        SocketParameterConverter::getParam( socketType, type );

        // AF_UNSPEC means don't care about IPv4 or IPv6
        // AI_PASSIVE means use my IP
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = type;
        hints.ai_flags = AI_PASSIVE;

        int status = getaddrinfo( nullptr, port.c_str(), &hints, &serverInfo );

        if ( status == 0)
        {
            fillSocketAddress( serverInfo );
            freeaddrinfo( serverInfo );
        }
        else
        {
            std::cerr << "ServerSocket error: " << gai_strerror( status ) << "\n";
            return false;
        }

        return true;
    }
    
    size_t getSocketAddressCount()
    {
        return mSocketAddressList.size();
    }

    const SocketAddress* getSocketAddress( size_t index )
    {
        if ( index < mSocketAddressList.size() )
        {
            return &mSocketAddressList[index];
        }

        return nullptr;
    }

    bool bind( size_t socketAddressIndex )
    {
        if ( socketAddressIndex >= mSocketAddressList.size() )
        {
            std::cerr << "ServerSocket error: invalid socket address index.\n";
            return false;
        }

        SocketAddress& socketAddress = mSocketAddressList[socketAddressIndex];

        int family = AF_UNSPEC, socketType = SOCK_STREAM, protocol = 0;

        SocketParameterConverter::getParam( socketAddress.getFamily(), family );
        SocketParameterConverter::getParam( socketAddress.getSocketType(), socketType );
        SocketParameterConverter::getParam( socketAddress.getProtocol(), protocol );

        mSocketDescriptor = socket( family, socketType, protocol );

        if ( mSocketDescriptor == -1 )
        {
            std::cerr << "ServerSocket error: socket()\n";
            return false;
        }

        // Specifies that the rules used in validating addresses supplied to bind()
        // should allow reuse of local addresses, if this is supported by the protocol.
        int yes = 1;
        int status = setsockopt( mSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) );

        if ( status == -1 )
        {
            std::cerr << "ServerSocket error: setsockopt()\n";
            return false;
        }

        struct sockaddr address;
        size_t addressSize;
        memset( &address, 0, sizeof(sockaddr) );

        if ( socketAddress.getFamily() == SocketFamily::IPV4 )
        {
            addressSize = sizeof( sockaddr_in );
            struct sockaddr_in* addressIPv4 = reinterpret_cast< struct sockaddr_in* >( &address );
            IPV4ADDRESS ipv4 = 0;

            socketAddress.getIPv4Address( ipv4 );

            addressIPv4->sin_family = family;
            addressIPv4->sin_port = socketAddress.getPort();
            addressIPv4->sin_addr.s_addr = ipv4;
        }
        else
        {
            addressSize = sizeof( sockaddr_in6 );
            struct sockaddr_in6* addressIPv6 = reinterpret_cast< struct sockaddr_in6* >( &address );
            IPV6ADDRESS ipv6;

            socketAddress.getIPv6Address( ipv6 );

            addressIPv6->sin6_family = family;
            addressIPv6->sin6_port = socketAddress.getPort();
            addressIPv6->sin6_flowinfo = socketAddress.getIPv6FlowInfo();
            memcpy( addressIPv6->sin6_addr.s6_addr, ipv6, sizeof( IPV6ADDRESS ) );
            addressIPv6->sin6_scope_id = socketAddress.getIPv6ScopeId();
        }

        status = ::bind( mSocketDescriptor, &address, addressSize );
        if ( status == -1 )
        {
            this->close();
            std::cerr << "ServerSocket error: bind()\n";
            return false;
        }

        return true;
    }
    
    bool listen()
    {
        int status = ::listen( mSocketDescriptor, mBacklog );
        if ( status == -1 )
        {
            std::cerr << "ServerSocket error: listen()\n";
            return false;
        }

        return true;
    }
    
    Socket* accept()
    {
        if ( mSocketDescriptor == -1 )
        {
            std::cerr << "ServerSocket error: server not set.\n";
            return nullptr;
        }

        // Connector's address information
        struct sockaddr_storage connectorAddress;
        socklen_t addressSize = sizeof( connectorAddress );

        memset( &connectorAddress, 0, sizeof( connectorAddress ) );

        int socketDescriptor = ::accept( mSocketDescriptor, reinterpret_cast<struct sockaddr*>( &connectorAddress ), &addressSize );

        if ( socketDescriptor == -1 )
        {
            std::cerr << "ServerSocket error: accept().\n";
            return nullptr;
        }

        Socket* socket = nullptr;

        struct sockaddr* connectorSockAddr = reinterpret_cast<struct sockaddr*>( &connectorAddress );
        if ( connectorSockAddr && connectorSockAddr->sa_family ==  AF_INET )
        {
            SocketFamily family;
            PORT port;
            IPV4ADDRESS ipv4;

            struct sockaddr_in* address = reinterpret_cast<struct sockaddr_in*>( connectorSockAddr);

            SocketParameterConverter::getParam( address->sin_family, family );
            port = ntohs( address->sin_port );
            ipv4 = ntohl( address->sin_addr.s_addr );

            socket = new Socket( socketDescriptor, port, ipv4 );
        }
        else
        {
            SocketFamily family;
            PORT port;
            IPV6ADDRESS ipv6;
            IPV6FLOWINFO ipv6FlowInfo;
            IPV6SCOPEID ipv6ScopeId;

            struct sockaddr_in6* address = reinterpret_cast<struct sockaddr_in6*>( connectorSockAddr);

            SocketParameterConverter::getParam( address->sin6_family, family );
            port = ntohs( address->sin6_port );
            memcpy( ipv6, address->sin6_addr.s6_addr, sizeof( IPV6ADDRESS ) );
            ipv6FlowInfo = address->sin6_flowinfo;
            ipv6ScopeId = address->sin6_scope_id;

            socket = new Socket( socketDescriptor, port, ipv6, ipv6FlowInfo, ipv6ScopeId );
        }

        return socket;
    }
    
    void close()
    {
        ::close( mSocketDescriptor );
        mSocketDescriptor = -1;
    }

    private:

    void fillSocketAddress( struct addrinfo* serverInfo )
    {
        int count = 0;
        struct addrinfo* p = serverInfo;

        // Reserve vector size based on list size
        while ( p != nullptr )
        {
            count++;
            p = p->ai_next;
        }
        mSocketAddressList.reserve( count );

        p = serverInfo;
        while ( p != nullptr )
        {
            SocketAddress socketAddress;

            SocketFlags flags;
            SocketParameterConverter::getParam( p->ai_flags, flags );
            socketAddress.setFlags( flags );

            SocketFamily family;
            SocketParameterConverter::getParam( p->ai_family, family );
            socketAddress.setFamily( family );

            SocketType socketType;
            SocketParameterConverter::getParam( p->ai_socktype, socketType );
            socketAddress.setSocketType( socketType );

            SocketProtocol protocol;
            SocketParameterConverter::getParam( p->ai_protocol, protocol );
            socketAddress.setProtocol( protocol );

            // IPv4
            if ( p->ai_family == AF_INET )
            {
                struct sockaddr_in* addr = reinterpret_cast< struct sockaddr_in* >( p->ai_addr );
                socketAddress.setPort( ntohs( addr->sin_port ) );
                socketAddress.setIPv4Address( addr->sin_addr.s_addr );
            }
            // IPv6
            else if ( p->ai_family == AF_INET6 )
            {
                struct sockaddr_in6* addr = reinterpret_cast< struct sockaddr_in6* >( p->ai_addr );
                socketAddress.setPort( ntohs( addr->sin6_port ) );
                socketAddress.setIPv6Address( addr->sin6_addr.s6_addr );
                socketAddress.setIPv6FlowInfo( ntohl( addr->sin6_flowinfo ) );
                socketAddress.setIPv6ScopeId( ntohl( addr->sin6_scope_id ) );
            }

            if ( p->ai_canonname != nullptr )
            {
                std::string canonicalName = p->ai_canonname;
                socketAddress.setCanonicalHostname( canonicalName );
            }

            mSocketAddressList.push_back( socketAddress );

            p = p->ai_next;
        }
    }

    int mSocketDescriptor;
    int mBacklog;
    std::vector< SocketAddress > mSocketAddressList;
};



//
//
//

class ClientSocket
{
    public:
    
    ClientSocket() : mSocketDescriptor(-1)
    {
    }

    ~ClientSocket()
    {
        close();
    }
    
    bool setup( const std::string& address, PORT port, SocketType socketType = SocketType::STREAM )
    {
        std::string strPort = std::to_string( port );

        return setup( address, strPort, socketType );
    }
    
    bool setup( const std::string& address, const std::string& port, SocketType socketType = SocketType::STREAM )
    {
        struct addrinfo hints;
        struct addrinfo* serverInfo;
        int type = SOCK_STREAM;

        SocketParameterConverter::getParam( socketType, type );

        // AF_UNSPEC means don't care about IPv4 or IPv6
        // AI_PASSIVE means use my IP
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = type;

        int status = getaddrinfo( address.c_str(), port.c_str(), &hints, &serverInfo );

        if ( status == 0)
        {
            fillSocketAddress( serverInfo );
            freeaddrinfo( serverInfo );
        }
        else
        {
            std::cerr << "ClientSocket error: " << gai_strerror( status ) << "\n";
            return false;
        }

        return true;
    }
    
    size_t getSocketAddressCount()
    {
        return mSocketAddressList.size();
    }

    const SocketAddress* getSocketAddress( size_t index )
    {
        if ( index < mSocketAddressList.size() )
        {
            return &mSocketAddressList[index];
        }

        return nullptr;
    }

    Socket* connect( size_t socketAddressIndex )
    {
        if ( socketAddressIndex >= mSocketAddressList.size() )
        {
            std::cerr << "ClientSocket error: invalid socket address index.\n";
            return nullptr;
        }

        SocketAddress& socketAddress = mSocketAddressList[socketAddressIndex];

        int family = AF_UNSPEC, socketType = SOCK_STREAM, protocol = 0;

        SocketParameterConverter::getParam( socketAddress.getFamily(), family );
        SocketParameterConverter::getParam( socketAddress.getSocketType(), socketType );
        SocketParameterConverter::getParam( socketAddress.getProtocol(), protocol );

        mSocketDescriptor = socket( family, socketType, protocol );

        if ( mSocketDescriptor == -1 )
        {
            std::cerr << "ClientSocket error: socket()\n";
            return nullptr;
        }

        struct sockaddr address;
        size_t addressSize;
        memset( &address, 0, sizeof( sockaddr ) );

        Socket* socket = nullptr;

        if ( socketAddress.getFamily() == SocketFamily::IPV4 )
        {
            addressSize = sizeof( sockaddr_in );
            struct sockaddr_in* addressIPv4 = reinterpret_cast< struct sockaddr_in* >( &address );
            IPV4ADDRESS ipv4 = 0;

            socketAddress.getIPv4Address( ipv4 );

            addressIPv4->sin_family = family;
            addressIPv4->sin_port = socketAddress.getPort();
            addressIPv4->sin_addr.s_addr = ipv4;

            socket = new Socket( mSocketDescriptor, socketAddress.getPort(), ipv4 );
        }
        else
        {
            addressSize = sizeof( sockaddr_in6 );
            struct sockaddr_in6* addressIPv6 = reinterpret_cast< struct sockaddr_in6* >( &address );
            IPV6ADDRESS ipv6;

            socketAddress.getIPv6Address( ipv6 );

            addressIPv6->sin6_family = family;
            addressIPv6->sin6_port = socketAddress.getPort();
            addressIPv6->sin6_flowinfo = socketAddress.getIPv6FlowInfo();
            memcpy( addressIPv6->sin6_addr.s6_addr, ipv6, sizeof( IPV6ADDRESS ) );
            addressIPv6->sin6_scope_id = socketAddress.getIPv6ScopeId();

            socket = new Socket( mSocketDescriptor, socketAddress.getPort(), ipv6, socketAddress.getIPv6FlowInfo(), socketAddress.getIPv6ScopeId() );
        }

        int status = ::connect( mSocketDescriptor, &address, addressSize );
        if ( status == -1 )
        {
            this->close();
            std::cerr << "ClientSocket error: connect()\n";
            return nullptr;
        }

        return socket;
    }

    void close()
    {
        ::close( mSocketDescriptor );
        mSocketDescriptor = -1;
    }

    private:

    void fillSocketAddress( struct addrinfo* serverInfo )
    {
        int count = 0;
        struct addrinfo* p = serverInfo;

        // Reserve vector size based on list size
        while ( p != nullptr )
        {
            count++;
            p = p->ai_next;
        }
        mSocketAddressList.reserve( count );

        p = serverInfo;
        while ( p != nullptr )
        {
            SocketAddress socketAddress;

            SocketFlags flags;
            SocketParameterConverter::getParam( p->ai_flags, flags );
            socketAddress.setFlags( flags );

            SocketFamily family;
            SocketParameterConverter::getParam( p->ai_family, family );
            socketAddress.setFamily( family );

            SocketType socketType;
            SocketParameterConverter::getParam( p->ai_socktype, socketType );
            socketAddress.setSocketType( socketType );

            SocketProtocol protocol;
            SocketParameterConverter::getParam( p->ai_protocol, protocol );
            socketAddress.setProtocol( protocol );

            // IPv4
            if ( p->ai_family == AF_INET )
            {
                struct sockaddr_in* addr = reinterpret_cast< struct sockaddr_in* >( p->ai_addr );
                socketAddress.setPort( ntohs( addr->sin_port ) );
                socketAddress.setIPv4Address( addr->sin_addr.s_addr );
            }
            // IPv6
            else if ( p->ai_family == AF_INET6 )
            {
                struct sockaddr_in6* addr = reinterpret_cast< struct sockaddr_in6* >( p->ai_addr );
                socketAddress.setPort( ntohs( addr->sin6_port ) );
                socketAddress.setIPv6Address( addr->sin6_addr.s6_addr );
                socketAddress.setIPv6FlowInfo( ntohl( addr->sin6_flowinfo ) );
                socketAddress.setIPv6ScopeId( ntohl( addr->sin6_scope_id ) );
            }

            if ( p->ai_canonname != nullptr )
            {
                std::string canonicalName = p->ai_canonname;
                socketAddress.setCanonicalHostname( canonicalName );
            }

            mSocketAddressList.push_back( socketAddress );

            p = p->ai_next;
        }
    }

    int mSocketDescriptor;
    std::vector< SocketAddress > mSocketAddressList;
};

