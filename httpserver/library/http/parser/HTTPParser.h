#include <string>

namespace parser
{
	struct XMLNode
	{
		//��ǰ��ǩ�ڵ�ı�ǩ��
		std::string name;
		//��ǰ��ǩ�ڵ������
		std::map<std::string, std::string> property;
		//��ǰ��ǩ�ڵ��ֵ
		std::string childText;
	};

	namespace HTTPParser
	{
		enum class TokenKind : unsigned char {
			Literal = 0, //�ı�
			OpenTag = 1, //��ʼ��ǩ��<
			OpenTagEnd = 2, //��ʼ��ǩ��>
			CloseTag = 3, //������ǩ��/
			WhiteSpace = 4, //�ո�
			AttrValueEq = 5, //�����е�=
			AttrValueNq = 6, //û�б����Ű���������
			AttrValueSq = 7, //�����еĵ�����
			AttrValueDq = 8 //�����е�˫����

		};

		enum class State : unsigned char {
			BeforeOpenTag = 0,
			OnOpenTag = 1,
			BeforeTagName = 2,
			OnTagName = 3,
			AfterTagName = 4,
			BeforeProperty = 5,
			OnProperty = 6,
			AfterProperty = 7,
			AfterOpenTag = 8,
			OnTagValue = 9,
			OnCloseTagEnd = 10

		};

		State ExamineCharacter(char c) noexcept
		{

		}

		std::vector<XMLNode> ParseHTMLText(std::string html) noexcept(false)
		{
			size_t length = html.length();
			size_t index = 0;
			size_t nodeStartIndex = 0;
			State state = State::BeforeOpenTag;
			while (index < length)
			{
				switch (state)
				{
				case parser::HTTPParser::State::BeforeOpenTag:
					break;
				case parser::HTTPParser::State::OnOpenTag:
					break;
				case parser::HTTPParser::State::BeforeTagName:
					break;
				case parser::HTTPParser::State::OnTagName:
					break;
				case parser::HTTPParser::State::AfterTagName:
					break;
				case parser::HTTPParser::State::BeforeProperty:
					break;
				case parser::HTTPParser::State::OnProperty:
					break;
				case parser::HTTPParser::State::AfterProperty:
					break;
				case parser::HTTPParser::State::AfterOpenTag:
					break;
				case parser::HTTPParser::State::OnTagValue:
					break;
				case parser::HTTPParser::State::OnCloseTagEnd:
					break;
				default:
					break;
				}
			}
		}
	}
}
