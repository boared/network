/**
 * A quick way to create sockets and send and receive messages.
 * 
 * EXAMPLE OF USE:
 * 
 * SERVER EXAMPLE
 *    #include <iostream>
 *    #include "Socket.h"
 * 
 *    int main()
 *     {
 *        ServerSocket server( 50 );
 *    
 *         if ( server.setup( "3490" ) )
 *         {
 *             size_t addressCount = server.getSocketAddressCount();
 *
 *             int address;
 *            for (size_t i = 0; i < addressCount; ++i)
 *            {
 *                const SocketAddress* socketAddress = server.getSocketAddress( i );
 *
 *                // Choose an address based in some criteria
 *                address = i;
 *            }
 *    
 *            if ( server.start( address ) )
 *            {
 *                // Wait for connection
 *               Socket* socket = server.accept();
 * 
 *                if ( socket != nullptr )
 *                {
 *                    char* buffer = new char[bufferSize];
 *
 *                    // received contains the number of bytes received or 0 in case
 *                    // the connection was closed
 *                    ssize_t received = socket->receive( buffer, bufferSize);
 *        
 *                    // The function send will always try to send bufferSize bytes
 *                    // to the remote side. sent contains the number of bytes sent
 *                    // or -1 in case of error.
 *                    ssize_t sent = socket->send( buffer, bufferSize );
 *                    std::cout << "Sent " << sent << " bytes\n";
 *                    
 *                    delete socket;
 *                }
 * 
 *                server.close();
 *            }
 *        }
 *        
 *        return 0;
 *    }
 * 
 * 
 * CLIENT EXAMPLE
 *    #include <iostream>
 *    #include "Socket.h"
 * 
 *    int main()
 *     {
 *        ClientSocket client;
 *    
 *         if ( client.setup( "192.168.0.1", "3490" ) )
 *         {
 *             size_t addressCount = client.getSocketAddressCount();
 *
 *            int address;
 *            for (size_t i = 0; i < addressCount; ++i)
 *            {
 *                const SocketAddress* socketAddress = client.getSocketAddress( i );
 *
 *                // Choose an address based in some criteria
 *                address = i;
 *            }
 *
 *            Socket* socket = client.connect( address );
 * 
 *            if ( socket != nullptr )
 *            {
 *                // This step is the same as in SERVER EXAMPLE
 *            }
 * 
 *            client.close();
 *        }
 *        
 *        return 0;
 *    }
 */



#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>



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

// Internet address ( IPV4ADDRESS is always in host byte order)
typedef uint32_t IPV4ADDRESS;
typedef unsigned char IPV6ADDRESS[16];

// Port number (always in host byte order)
typedef uint16_t PORT;

// Used only by IPv6 addresses
typedef uint32_t IPV6FLOWINFO;
typedef uint32_t IPV6SCOPEID;



/**
 * This class is used to convert system socket types to the types defined in this
 * file and vice-versa.
 */
class SocketParameterConverter
{
    public:

    static void getParam( int from, SocketFlags& to )
    {
        switch ( from )
        { 
            case AI_PASSIVE:
                to = SocketFlags::PASSIVE;
                break;
                
            case AI_CANONNAME:
                to = SocketFlags::CANONICALNAME;
                break;
                
            default:
                to = SocketFlags::PASSIVE;
                break;
        }
    }
    
    static void getParam( int from, SocketFamily& to )
    {
        switch ( from )
        {
            case AF_UNSPEC:
                to = SocketFamily::UNSPECIFIED;
                break;
                
            case AF_INET:
                to = SocketFamily::IPV4;
                break;
                
            case AF_INET6:
                to = SocketFamily::IPV6;
                break;
                
            default:
                to = SocketFamily::UNSPECIFIED;
                break;
        }
    }
    
    static void getParam( int from, SocketType& to )
    {
        switch ( from )
        {
            case SOCK_STREAM:
                to = SocketType::STREAM;
                break;
                
            case SOCK_DGRAM:
                to = SocketType::DATAGRAM;
                break;
                
            default:
                to = SocketType::STREAM;
                break;
        }
    }
    
    static void getParam( int from, SocketProtocol& to )
    {
        switch ( from )
        {
            case 0:
                to = SocketProtocol::ANY;
                break;
                
            default:
                to = SocketProtocol::ANY;
                break;
        }
    }
    
    static void getParam( SocketFlags from, int& to )
    {
        switch ( from )
        {
            case SocketFlags::PASSIVE:
                to = AI_PASSIVE;
                break;
                
            case SocketFlags::CANONICALNAME:
                to = AI_CANONNAME;
                break;
            
            default:
                to = AI_PASSIVE;
        }
    }
    
    static void getParam( SocketFamily from, int& to )
    {
        switch ( from )
        {
            case SocketFamily::UNSPECIFIED:
                to = AF_UNSPEC;
                break;
                
            case SocketFamily::IPV4:
                to = AF_INET;
                break;
                
            case SocketFamily::IPV6:
                to = AF_INET6;
                break;
                
            default:
                to = AF_UNSPEC;
                break;
        }
    }
    
    static void getParam( SocketType from, int& to )
    {
        switch ( from )
        {
            case SocketType::STREAM:
                to = SOCK_STREAM;
                break;
                
            case SocketType::DATAGRAM:
                to = SOCK_DGRAM;
                break;
                
            default:
                to = SOCK_STREAM;
                break;
        }
    }
    
    static void getParam( SocketProtocol from, int& to )
    {
        switch ( from )
        {
            case SocketProtocol::ANY:
                to = 0;
                break;
                
            default:
                to = 0;
                break;
        }
    }
};



/**
 * This class holds information about Internet address.
 */
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
        
        setIPv4Address( ntohl( sa.sin_addr.s_addr ) );
    }
    
    void setIPv6Address( IPV6ADDRESS address )
    {
        memcpy( mIPv6Address, address, sizeof( IPV6ADDRESS ) );
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
        IPV4ADDRESS nboIPv4 = htonl( mIPv4Address );
        inet_ntop(AF_INET, &nboIPv4, ipv4, INET_ADDRSTRLEN);
        
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
        memcpy( address, mIPv6Address, sizeof( IPV6ADDRESS ) );
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



/**
 * This class sends/receives messages to/from sockets.
 */
class Socket
{
    public:

    /**
     * Construct a connectionless socket
     */
    Socket()
    {
        int family = AF_UNSPEC, socketType = SOCK_DGRAM, protocol = 0;
        mSocketDescriptor = socket( family, socketType, protocol );
    }
    
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
    
    ssize_t send( const void* buffer, ssize_t size )
    {
        ssize_t totalSentSize = -1;

        if ( mSocketDescriptor != -1 )
        {
            totalSentSize = ::send( mSocketDescriptor, buffer, size, 0 );
            
            if ( totalSentSize != - 1 )
            {
                ssize_t remainingSize = size - totalSentSize;

                while ( remainingSize > 0 )
                {
                    ssize_t sentSize = ::send( mSocketDescriptor, (reinterpret_cast<const char*>(buffer) + totalSentSize), remainingSize, 0 );
                    
                    if ( sentSize != -1 )
                    {
                        totalSentSize += sentSize;
                        remainingSize = size - totalSentSize;
                    }
                    else
                    {
                        remainingSize = -1;
                    }
                }
            }
        }

        return totalSentSize;
    }
    
    ssize_t receive( void* buffer, ssize_t size )
    {
        if ( mSocketDescriptor != -1 )
        {
            return ::recv( mSocketDescriptor, buffer, size, 0 );
        }

        return 0;
    }
    
    ssize_t sendTo( const SocketAddress& receiver, const void* buffer, ssize_t size )
    {
        ssize_t totalSentSize = -1;

        if ( mSocketDescriptor != -1 )
        {
            struct sockaddr address;
            size_t addressSize;
            
            convertAddress( receiver, address, addressSize );
            
            totalSentSize = ::sendto( mSocketDescriptor, buffer, size, 0, &address, addressSize );
            
            if ( totalSentSize != - 1 )
            {
                ssize_t remainingSize = size - totalSentSize;

                while ( remainingSize > 0 )
                {
                    ssize_t sentSize = ::sendto( mSocketDescriptor, (reinterpret_cast<const char*>(buffer) + totalSentSize), remainingSize, 0, &address, addressSize );
                    
                    if ( sentSize != -1 )
                    {
                        totalSentSize += sentSize;
                        remainingSize = size - totalSentSize;
                    }
                    else
                    {
                        remainingSize = -1;
                    }
                }
            }
        }

        return totalSentSize;
    }
    
    ssize_t receiveFrom( const SocketAddress& sender, void* buffer, ssize_t size )
    {
        if ( mSocketDescriptor != -1 )
        {
            struct sockaddr address;
            socklen_t addressSize;
            
            convertAddress( sender, address, addressSize );

            return ::recvfrom( mSocketDescriptor, buffer, size, 0, &address, &addressSize);
        }

        return 0;
    }
    

    private:
        
    void convertAddress( const SocketAddress& socketAddress, struct sockaddr& address, socklen_t& addressSize )
    {
        size_t size;
        convertAddress( socketAddress, address, size );
        addressSize = static_cast<socklen_t>( size );
    }
    
    void convertAddress( const SocketAddress& socketAddress, struct sockaddr& address, size_t& addressSize )
    {
        memset( &address, 0, sizeof( sockaddr ) );

        int family = AF_UNSPEC;

        SocketParameterConverter::getParam( socketAddress.getFamily(), family );

        if ( socketAddress.getFamily() == SocketFamily::IPV4 )
        {
            addressSize = sizeof( sockaddr_in );
            struct sockaddr_in* addressIPv4 = reinterpret_cast< struct sockaddr_in* >( &address );
            IPV4ADDRESS ipv4 = 0;

            socketAddress.getIPv4Address( ipv4 );

            addressIPv4->sin_family = family;
            addressIPv4->sin_port = htons( socketAddress.getPort() );
            addressIPv4->sin_addr.s_addr = htonl( ipv4 );
        }
        else
        {
            addressSize = sizeof( sockaddr_in6 );
            struct sockaddr_in6* addressIPv6 = reinterpret_cast< struct sockaddr_in6* >( &address );
            IPV6ADDRESS ipv6;

            socketAddress.getIPv6Address( ipv6 );

            addressIPv6->sin6_family = family;
            addressIPv6->sin6_port = htons( socketAddress.getPort() );
            addressIPv6->sin6_flowinfo = socketAddress.getIPv6FlowInfo();
            memcpy( addressIPv6->sin6_addr.s6_addr, ipv6, sizeof( IPV6ADDRESS ) );
            addressIPv6->sin6_scope_id = socketAddress.getIPv6ScopeId();
        }
    }

    int mSocketDescriptor;
    
    SocketFamily mFamily;
    PORT mPort;
    IPV4ADDRESS mIPv4Address;
    IPV6ADDRESS mIPv6Address;
    IPV6FLOWINFO mIPv6FlowInfo;
    IPV6SCOPEID mIPv6ScopeId;
};


/**
 * This is a superclass for server and client sockets.
 */
class SocketHandler
{
    public:
    
    SocketHandler() : mSocketDescriptor(-1)
    {
    }
    
    virtual ~SocketHandler()
    {
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
    
    void close()
    {
        ::close( mSocketDescriptor );
        mSocketDescriptor = -1;
    }

    protected:
        
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
        
        mSocketAddressList.clear();
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
                socketAddress.setIPv4Address( ntohl( addr->sin_addr.s_addr ) );
            }
            // IPv6
            else if ( p->ai_family == AF_INET6 )
            {
                struct sockaddr_in6* addr = reinterpret_cast< struct sockaddr_in6* >( p->ai_addr );
                socketAddress.setPort( ntohs( addr->sin6_port ) );
                socketAddress.setIPv6Address( addr->sin6_addr.s6_addr );
                socketAddress.setIPv6FlowInfo( addr->sin6_flowinfo );
                socketAddress.setIPv6ScopeId( addr->sin6_scope_id );
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



/**
 * This class creates a server socket.
 */
class ServerSocket : public SocketHandler
{
    public:
    
    ServerSocket() : SocketHandler(), mBacklog(10)
    {
    }

    ServerSocket( int backlog ) : SocketHandler(), mBacklog( backlog )
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
        if ( mSocketDescriptor != -1 )
        {
            close();
        }
        
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
    
    bool start( size_t socketAddressIndex )
    {
        if ( mSocketDescriptor != -1 )
        {
            std::cerr << "ServerSocket error: socket already bound.\n";
            return false;
        }
        
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
            std::cerr << "ServerSocket error: file descriptor creation failed.\n";
            return false;
        }

        // Specifies that the rules used in validating addresses supplied to bind()
        // should allow reuse of local addresses, if this is supported by the protocol.
        int yes = 1;
        int status = setsockopt( mSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) );

        if ( status == -1 )
        {
            std::cerr << "ServerSocket error: setsockopt()\n";
            return false;
        }

        // Fill with the server address
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
            addressIPv4->sin_port = htons( socketAddress.getPort() );
            addressIPv4->sin_addr.s_addr = htonl( ipv4 );
        }
        else
        {
            addressSize = sizeof( sockaddr_in6 );
            struct sockaddr_in6* addressIPv6 = reinterpret_cast< struct sockaddr_in6* >( &address );
            IPV6ADDRESS ipv6;

            socketAddress.getIPv6Address( ipv6 );

            addressIPv6->sin6_family = family;
            addressIPv6->sin6_port = htons( socketAddress.getPort() );
            addressIPv6->sin6_flowinfo = socketAddress.getIPv6FlowInfo();
            memcpy( addressIPv6->sin6_addr.s6_addr, ipv6, sizeof( IPV6ADDRESS ) );
            addressIPv6->sin6_scope_id = socketAddress.getIPv6ScopeId();
        }

        status = ::bind( mSocketDescriptor, &address, addressSize );
        if ( status == -1 )
        {
            close();
            std::cerr << "ServerSocket error: Error while binding address to the socket.\n";
            return false;
        }
        
        status = ::listen( mSocketDescriptor, mBacklog );
        if ( status == -1 )
        {
            close();
            std::cerr << "ServerSocket error: Cannot starting listening.\n";
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
            PORT port;
            IPV4ADDRESS ipv4;

            struct sockaddr_in* address = reinterpret_cast<struct sockaddr_in*>( connectorSockAddr);

            port = ntohs( address->sin_port );
            ipv4 = ntohl( address->sin_addr.s_addr );

            socket = new Socket( socketDescriptor, port, ipv4 );
        }
        else if ( connectorSockAddr && connectorSockAddr->sa_family ==  AF_INET6 )
        {
            PORT port;
            IPV6ADDRESS ipv6;
            IPV6FLOWINFO ipv6FlowInfo;
            IPV6SCOPEID ipv6ScopeId;

            struct sockaddr_in6* address = reinterpret_cast<struct sockaddr_in6*>( connectorSockAddr);

            port = ntohs( address->sin6_port );
            memcpy( ipv6, address->sin6_addr.s6_addr, sizeof( IPV6ADDRESS ) );
            ipv6FlowInfo = address->sin6_flowinfo;
            ipv6ScopeId = address->sin6_scope_id;

            socket = new Socket( socketDescriptor, port, ipv6, ipv6FlowInfo, ipv6ScopeId );
        }

        return socket;
    }

    private:

    int mBacklog;
};



/**
 * This class creates a client socket.
 */
class ClientSocket : public SocketHandler
{
    public:
    
    ClientSocket() : SocketHandler()
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
            std::cerr << "ClientSocket error: file descriptor creation failed.\n";
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
            addressIPv4->sin_port = htons( socketAddress.getPort() );
            addressIPv4->sin_addr.s_addr = htonl( ipv4 );

            socket = new Socket( mSocketDescriptor, socketAddress.getPort(), ipv4 );
        }
        else
        {
            addressSize = sizeof( sockaddr_in6 );
            struct sockaddr_in6* addressIPv6 = reinterpret_cast< struct sockaddr_in6* >( &address );
            IPV6ADDRESS ipv6;

            socketAddress.getIPv6Address( ipv6 );

            addressIPv6->sin6_family = family;
            addressIPv6->sin6_port = htons( socketAddress.getPort() );
            addressIPv6->sin6_flowinfo = socketAddress.getIPv6FlowInfo();
            memcpy( addressIPv6->sin6_addr.s6_addr, ipv6, sizeof( IPV6ADDRESS ) );
            addressIPv6->sin6_scope_id = socketAddress.getIPv6ScopeId();

            socket = new Socket( mSocketDescriptor, socketAddress.getPort(), ipv6, socketAddress.getIPv6FlowInfo(), socketAddress.getIPv6ScopeId() );
        }

        int status = ::connect( mSocketDescriptor, &address, addressSize );
        if ( status == -1 )
        {
            this->close();
            std::cerr << "ClientSocket error: Cannot connect.\n";
            return nullptr;
        }

        return socket;
    }
};
