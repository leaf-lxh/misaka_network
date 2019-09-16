#include <string>

namespace parser
{
	struct XMLNode
	{
		//当前标签节点的标签名
		std::string name;
		//当前标签节点的属性
		std::map<std::string, std::string> property;
		//当前标签节点的值
		std::string childText;
	};

	namespace HTTPParser
	{
		enum class TokenKind : unsigned char {
			Literal = 0, //文本
			OpenTag = 1, //起始标签的<
			OpenTagEnd = 2, //起始标签的>
			CloseTag = 3, //结束标签的/
			WhiteSpace = 4, //空格
			AttrValueEq = 5, //属性中的=
			AttrValueNq = 6, //没有被引号包括的属性
			AttrValueSq = 7, //属性中的单引号
			AttrValueDq = 8 //属性中的双引号

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
