#include "Parser.hpp"

Parser::Parser(VirtualCPU * const cpu) : _cpu(cpu) {
  _instructions["push"] =	1;
  _instructions["pop"] =	0;
  _instructions["dump"] =	0;
  _instructions["assert"] =	1;
  _instructions["add"] =	0;
  _instructions["sub"] =	0;
  _instructions["mul"] =	0;
  _instructions["div"] =	0;
  _instructions["mod"] =	0;
  _instructions["print"] =	0;
  _instructions["exit"] =	0;
  _arguments["int8"] =		Integer;
  _arguments["int16"] =		Integer;
  _arguments["int32"] =		Integer;
  _arguments["float"] =		Decimal;
  _arguments["double"] =	Decimal;
}

Parser::~Parser() {
  std::ifstream *tmp;
  if ((tmp = dynamic_cast<std::ifstream *>(_is))) {
    if (tmp) {
      tmp->close();
      delete _is;
    }
  }
}

Parser::Parser(const Parser &) {
}

IOperand *	Parser::createOperand(eOperandType type, const std::string & value)
{

}

void		Parser::parse() {
  char buffer[BUFF_SIZE];

  while (_is->getline(buffer, BUFF_SIZE)) {
    executeLine(buffer);
    // std::cout << buffer << std::endl;
  }
}

void		Parser::initIO(const std::string& filename) {
  std::ifstream * ifs = new std::ifstream(filename.data());
  if (!ifs->is_open())
    throw IOException(std::string("Unable to open ") + filename);
  _is = ifs;
}

void		Parser::initIO() {
  _is = &std::cin;
}

void			Parser::executeLine(const std::string & line)
{
  std::stringstream	s;
  std::string		instruct;
  std::map<std::string, nb_arguments>::iterator iti;
  std::map<std::string, eArgumentType>::iterator ita;
  std::vector<std::string>	args;

  s << line;
  s >> instruct;
  if (instruct.empty())
    return ;
  iti = _instructions.find(instruct); // iti contains a number of arguments
  if (iti == _instructions.end())
    throw SyntaxException(std::string("instruction ") + s.str() + " not found");

  for (nb_arguments i = iti->second; i > 0; i--) {
      std::size_t token1, token2;
      std::string arg_str;

      s >> arg_str;
      if (arg_str.empty())
	throw SyntaxException(std::string("Missing argument for ") + instruct);

      token1 = arg_str.find('(');
      if (token1 == std::string::npos)
	throw SyntaxException(std::string("Unable to find value for ") + arg_str);

      // arg_str.substr(0, token1) == "int8" ...
      ita = _arguments.find(arg_str.substr(0, token1)); // ita contains eArgumentType
      if (ita == _arguments.end())
	throw SyntaxException(std::string("No type found (") + arg_str.substr(0, token1) + ")");

      token2 = arg_str.find(')');
      if (token2 == std::string::npos)
	throw SyntaxException(std::string("Unable to find value for ") + arg_str);

      // arg_str.substr(token1, token2 - token1) == "42"
      args.push_back(arg_str.substr(0, token1));
    }
  std::string str;
  s >> str;
  if (!str.empty())
    throw SyntaxException(std::string("Too many arguments for ") + instruct + s.str());
  _cpu->executeInstruction(instruct, args);
}
