#include <tinyHttpd.h>

class BlogSpacePassport : public TinyHttpd
{
public:
	void InitRouteTabel() noexcept;


private:
	void HTTPPacketHandler(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept;
	TinyHttpd::HTTPResponsePacket GetUserInfo(int clientfd, TinyHttpd::HTTPRequestPacket request) noexcept(false);
};
