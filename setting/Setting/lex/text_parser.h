#include "parser.h"

class TextParser : public Parser
{
	public:
		TextParser();
		virtual ~TextParser();

		open(const char* filename);
		virtual  void close();
		virtual void read(string key);
};
