#include <tinyHttpd>

class BlogSpaceAdmin : public TinyHttpd
{
	void HTTPPacketHandler(int clientfd, HTTPRequestPacket request);
};
