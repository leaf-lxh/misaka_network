#include <tinyHttpd>

class BlogSpacePassport : public TinyHttpd
{
public:
	void InitRoutePath();
	void HTTPPacketHandler(int clientfd, HTTPRequestPacket request);
};
