#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <regex>
#include <ctype.h>
#include <algorithm>
#include <stack>

using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::string;
using std::getline;
using std::istringstream;
using std::endl;
using std::map;
using std::multimap;
using std::pair;
using std::tuple;
using std::vector;
using std::get;
using std::stack;
using std::to_string;
using std::stoi;

struct Tokens
{
	const string num{ "num" };
	const string id{ "id" };
	const string comentario{ "comentario" };
	const string OPR{ "OPR" };
	const string RCB{ "RCB" };
	const string OPM{ "OPM" };
	const string AB_P{ "AB_P" };
	const string FC_P{ "FC_P" };
	const string PT_V{ "PT_V" };
	const string ERRO{ "ERRO" };
	const string EndOfFile{ "EOF" };

	const string inicio{ "inicio" };
	const string varinicio{ "varinicio" };
	const string varfim{ "varfim" };
	const string leia{ "leia" };
	const string escreva{ "escreva" };
	const string se{ "se" };
	const string entao{ "entao" };
	const string senao{ "senao" };
	const string fimse{ "fimse" };
	const string fim{ "fim" };
	const string inteiro{ "int" };
	const string lit{ "lit" };
	const string real{ "real" };
};

multimap<int, tuple<string, string, string>> populateMainTable();
map<int, string> populatefinalStates();
map<pair<char, int>, int> populateLexicalStateMatrix();
map<pair<string, int>, int> populateSyntaticStateMatrix();
map<int, pair<string, string>> populateProductions();
pair<int, string> readMgol();

class Lexical
{
	public:
	int lineNum;
	int lineCount;
	string mgolCode;
	map <pair<char, int>, int> stateMatrix;
	multimap<int, tuple<string, string, string>> mainTable;
	map<int, string> finalStates;
	vector<string> ids;
	string lastLexeme;

	void initialize()
	{
		pair<int, string> sizeAndCode = readMgol();

		mainTable = populateMainTable();
		stateMatrix = populateLexicalStateMatrix();
		lineCount = sizeAndCode.first;
		mgolCode = sizeAndCode.second;
		finalStates = populatefinalStates();
		lineNum = 1;
		lastLexeme = "";
	}
};

class Token
{
	public:
	int line;
	int collumn;
	string lexeme;
	string token;

	void initialize()
	{
		line = 0;
		collumn = 0;
		lexeme = "";
	}
};

bool reservedWord(string word);
string getToken(int state);
char entry(char c, int state);
int countWords(string str);
string spaces(int previous);
void syntactic(Lexical& lex);
string getNextAction(int& state, string token);
void parseError(stack<string>& synth, Token& tok, Lexical& lex, string lastLexeme);
void printErrorHeader(int lastState, int line, int collumn, string lastLexeme);
Token nextToken(Lexical& lex);

int main()
{
	Tokens tokens;
	Lexical lex;
	lex.initialize();

	syntactic(lex);	

	if (lex.lineNum > 0 || lex.lineNum == lex.lineCount)
	{
		cout << endl << endl << "Tabela: " << endl << endl << "         Linha     Lexema                         Token      Tipo" << endl << endl;

		for (auto it = lex.mainTable.begin(); it != lex.mainTable.end(); ++it)
		{
			cout << "             " << it->first << "     " << get<0>(it->second) << spaces(get<0>(it->second).size()) << get<1>(it->second) << "        " << get<2>(it->second) << endl;
		}
	}

	cout << endl << endl;

	system("pause");
	return 0;
}

void syntactic(Lexical& lex)
{
	Token tok;
	tok.initialize();
	Tokens tokens;
	stack<string> synth;
	int i = 0;
	int state = -1;
	int stateAux = -1;
	string action = "";
	string lastLexeme = "";
	pair<string,string> reduction("","");
	map<pair<string, int>, int> synthMatrix = populateSyntaticStateMatrix();
	map<int, pair<string, string>> productions = populateProductions();

	tok = nextToken(lex);

	if (tok.line < 0 && !tok.token.empty())
	{
		cout << endl << "ERRO - linha " << tok.line*-1 << " - '" << tok.token << "'" << endl;
		return;
	}

	synth.push("-1");

	while (tok.line >= 0)
	{
		stateAux = synthMatrix[pair<string, int>(tok.token, stoi(synth.top()))];
		action = getNextAction(stateAux, tok.token);
		 
		if (action == "shift")
		{
			state = stateAux;
			synth.push(tok.token);
			synth.push(to_string(state));

			lastLexeme = tok.lexeme;
			tok = nextToken(lex);

			while (tok.token == tokens.comentario)
				tok = nextToken(lex);

			if (tok.line < 0)
			{
				lex.lineNum = tok.line;
				cout << endl << endl << "ERRO!" << endl << endl;
				cout << "Linha " << tok.line*-1 << ", Coluna " << tok.collumn << endl << tok.lexeme << endl
					<< "em '..." << string(&lex.mgolCode.front(), &lex.mgolCode.front() + tok.collumn + 5 > &lex.mgolCode.back() ?
						&lex.mgolCode.back() : & lex.mgolCode.front()+ tok.collumn + 5) << "...'" << endl;
				return;
			}

		}
		else if (action == "reduce")
		{
			state = stateAux;
			reduction = productions[state];

			for (i = 0; i < 2*countWords(reduction.second); i++)
			{
				synth.pop();
			}

			state = stoi(synth.top());
			synth.push(reduction.first);

			stateAux = synthMatrix[pair<string, int>(reduction.first, state)];
			synth.push(to_string(stateAux));

			cout << endl << "Producao: " << reduction.first << " -> " << reduction.second;

		}
		else if (action == "accept")
		{
			cout << endl << endl << "Analise Sintatica: Sucesso!";
			return;
		}
		else
		{
			cout << endl << endl << "ERRO!" << endl << endl;
			parseError(synth, tok, lex, lastLexeme);
			return;
		}
	}
}

void printErrorHeader(int lastState, int line, int collumn, string lastLexeme)
{
	cout << "Ultimo Estado: " << lastState << ", Linha " << line << ", Coluna " << collumn << endl
		<< " >> " << lastLexeme << endl;
}

void parseError(stack<string>& synth, Token& tok, Lexical& lex, string lastLexeme)
{
	int lastState = 0;
	lastState = stoi(synth.top());
	synth.pop();
	printErrorHeader(lastState, tok.line, tok.collumn, lastLexeme);

	switch (lastState)
	{

	case 39:
	case 37:
	case 38:
	case 17:
	case 23:
	case 26:
	case 44:
		cout << "Faltou um ; antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 49:
		cout << "Faltou um operador antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 58:
		cout << "Faltou um 'se' antes desse 'entao'" << endl;
		lex.lineNum *= -1;
		break;

	case 18:
		cout << "Faltou o tipo da variavel antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 14:
		cout <<  "Faltou parenteses antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 43:
	case 54:	
	case 10:
	case 11:	
		cout << "Faltou algo antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 12:
		cout << "Token invalido antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 53:
		cout << "Faltou o entao antes de '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	case 29:
		cout << "Faltou um 'fimse' em algum lugar" << endl;
		lex.lineNum *= -1;
		break;

	default:
		cout << "Em '" << tok.lexeme << "' " << endl;
		lex.lineNum *= -1;
		break;

	}
}

string getNextAction(int& state, string token)
{
	if (state == 999) return "accept";

	if (state > 200)
	{
		state = (state - 200);
		return "reduce";
	}

	if (state > 100)
	{
		state = (state - 100);
		return "shift";
	}

	if (state == 0) return "ërro";

	return "";
}

Token nextToken(Lexical& lex)
{	
	Token tk;
	tk.initialize();
	tuple<string, string, string> mainTableValue;
	struct Tokens tokens;
	char *begin = &lex.mgolCode.front();
	char *beginLexeme = begin;
	string lexeme = "";
	string token = "";
	string error = "";
	int stateAux = -1;
	int state = -1;
	int collumn = 1;
	char ent;

	for (char& c : lex.mgolCode)
	{
		if (c == '\0' && stateAux == -1 && state == -1)
		{
			tk.line = lex.lineNum;
			tk.lexeme = tokens.EndOfFile;
			tk.token = tokens.EndOfFile;
			tk.collumn = collumn;
			return tk;
		}

		begin = &c;
		ent = entry(c, state);		
		stateAux = lex.stateMatrix[pair<char, int>(ent, state)];
			
		if (stateAux == -1 && state != -1) break;
		else if (stateAux == -1 && state == -1)
		{
			if (c == '\n')
			{
				lex.lineNum++;
				collumn = 1;
			}
			beginLexeme++;
		}
		else if (stateAux == 0)
		{
			token = lex.finalStates[state];
			error = lex.finalStates[lex.stateMatrix[pair<char, int>(ent, -1)]];

			if (!token.empty() && !error.empty())
			{
				if (token != error)
				{
					if (token == tokens.id || token == tokens.num)
					{
						if (string(beginLexeme, begin) == tokens.se)
						{
							if (error == tokens.AB_P) break;
						}
						if (error == tokens.PT_V || error == tokens.OPR || error == tokens.OPM || error == tokens.FC_P) break;
					}

					if (token == tokens.AB_P)
					{
						if(error == tokens.id || error == tokens.num) break;
						else
						{
							tk.line = lex.lineNum * -1;
							tk.lexeme = "Falta um valor ou variavel depois desse Abre Parenteses: '" + string(beginLexeme, begin + 1) + "'";
							tk.collumn = collumn;
							return tk;
						}
					}		
					

					if (token == tokens.OPR || token == tokens.OPM || token == tokens.RCB)
					{
						if(error == tokens.id || error == tokens.num) break;
						else
						{
							tk.line = lex.lineNum * -1;
							tk.lexeme = "Falta um valor ou variavel aqui: '" + string(beginLexeme, begin + 1) + "'";
							tk.collumn = collumn;
							return tk;
						}
					}
					
				}
				else
				{
					tk.line = lex.lineNum * -1;
					tk.lexeme = "Token duplicado: '" + string(beginLexeme, begin + 1) + "'";
					tk.collumn = collumn;
					return tk;
				}
			}
			else if (!token.empty() && error.empty())
			{
				tk.line = lex.lineNum* -1;
				tk.lexeme = "Caractere invalido: '" + string(begin, begin + 1) + "'";
				tk.collumn = collumn;
				return tk;
			}

			tk.line = lex.lineNum * -1;
			tk.lexeme = "Caractere invalido: '" + string(beginLexeme, begin + 1) + "'";
			tk.collumn = collumn;
			return tk;
		}
		else collumn++;
		state = stateAux;
	} 

	token = lex.finalStates[state];
	lexeme = string(beginLexeme, begin);

	if (!token.empty())
	{		
		lex.mgolCode = string(begin, lex.mgolCode.length());

		if (token == tokens.id)
		{
			if (!reservedWord(lexeme))
			{
				if (std::find(lex.ids.begin(), lex.ids.end(), lexeme) == lex.ids.end())
				{
					lex.ids.push_back(lexeme);
				}
				mainTableValue = tuple<string, string, string>(lexeme, token, "");
				lex.mainTable.emplace(lex.lineNum, mainTableValue);

			}
			else
			{
				token = lexeme;
			}
		}

		cout << endl << "Linha " << lex.lineNum << " [" << token << "] " << lexeme;

		tk.line = lex.lineNum;
		tk.lexeme = lexeme;
		tk.collumn = collumn;
		tk.token = token;
		return tk;
	}
	else
	{
		if (state == 16 || state == 18)
		{
			tk.line = lex.lineNum*-1;
			tk.lexeme = "Vc esqueceu de fechar alguma coisa aqui: " + lexeme.substr(0, 7) + "...";
			tk.collumn = collumn;
			return tk;
		}

		tk.line = lex.lineNum*-1;
		tk.lexeme = lexeme;
		tk.collumn = collumn;
		return tk;
	}
}

pair<int,string> readMgol()
{
	ifstream inmgol("mgol.alg");
	ofstream outmgol("x.alg");

	string mgolCode;
	string rawline;
	int lineCount = 0;

	while (getline(inmgol, rawline))
	{
		lineCount++;
		istringstream iss(rawline);
		cout << lineCount << " " << rawline << endl;
		if (lineCount > 1) rawline = "\n" + rawline;
		mgolCode += rawline;				
	}

	return pair<int,string>(lineCount,mgolCode);
}

bool reservedWord(string word)
{
	struct Tokens tokens;
	return (word == tokens.inicio ||
			word == tokens.varinicio ||
			word == tokens.varfim ||
			word == tokens.leia ||
			word == tokens.escreva ||
			word == tokens.se ||
			word == tokens.entao ||
			word == tokens.senao ||
			word == tokens.fimse ||
			word == tokens.fim ||
			word == tokens.inteiro ||
			word == tokens.lit ||
			word == tokens.real);
}

map<int, string> populatefinalStates()
{
	struct Tokens tokens;

	map<int, string> m =
	{
		{ 1,tokens.id },
		{ 2,tokens.num },
		{ 4,tokens.num },
		{ 7,tokens.num },
		{ 8,tokens.fim },
		{ 9,tokens.OPR },
		{ 10,tokens.OPR },
		{ 11,tokens.RCB },
		{ 12,tokens.OPR },
		{ 13,tokens.AB_P },
		{ 14,tokens.FC_P },
		{ 15,tokens.PT_V },
		{ 17,tokens.OPM },
		{ 19,tokens.lit },
		{ 20,tokens.comentario }
	};

	return m;

}

string getToken(int state)
{
	map <int, string> finalStates = populatefinalStates();

	return finalStates[state];
}

map<pair<char, int>, int> populateLexicalStateMatrix()
{
	//pair<entry,current state> destination state
	map<pair<char, int>, int> m =
	{
		{ pair<char,int>('S',-1),-1 },
		{ pair<char,int>('S',1),-1 },
		{ pair<char,int>('S',1),-1 },
		{ pair<char,int>('S',2),-1 },
		{ pair<char,int>('S',4),-1 },
		{ pair<char,int>('S',7),-1 },
		{ pair<char,int>('S',8),-1 },
		{ pair<char,int>('S',9),-1 },
		{ pair<char,int>('S',10),-1 },
		{ pair<char,int>('S',11),-1 },
		{ pair<char,int>('S',12),-1 },
		{ pair<char,int>('S',13),-1 },
		{ pair<char,int>('S',14),-1 },
		{ pair<char,int>('S',15),-1 },
		{ pair<char,int>('S',17),-1 },
		{ pair<char,int>('S',19),-1 },
		{ pair<char,int>('S',20),-1 },

		{ pair<char,int>(';',1),-1 },
		{ pair<char,int>(';',2),-1 },
		{ pair<char,int>(';',4),-1 },
		{ pair<char,int>(';',7),-1 },
		{ pair<char,int>(';',19),-1 },
			   
		{ pair<char,int>('L',-1),1 },
		{ pair<char,int>('L',1),1 },
		{ pair<char,int>('_',1),1 },		
			   
		{ pair<char,int>('D',-1),2 },
		{ pair<char,int>('D',1),1 },
		{ pair<char,int>('D',2),2 },
		{ pair<char,int>('D',3),4 },
		{ pair<char,int>('D',4),4 },
		{ pair<char,int>('D',5),7 },
		{ pair<char,int>('D',6),7 },
		{ pair<char,int>('D',7),7 },
			   
		{ pair<char,int>('E',2),5 },
		{ pair<char,int>('E',4),5 },
		{ pair<char,int>('e',2),5 },
		{ pair<char,int>('e',4),5 },
		{ pair<char,int>('.',2),3 },
		{ pair<char,int>('-',5),6 },
		{ pair<char,int>('+',5),6 },
			   
		{ pair<char,int>('(',-1),13 },
		{ pair<char,int>(')',-1),14 },
		{ pair<char,int>(';',-1),15 },
		{ pair<char,int>('+',-1),17 },
		{ pair<char,int>('*',-1),17 },
		{ pair<char,int>('-',-1),17 },
		{ pair<char,int>('/',-1),17 },
			   
		{ pair<char,int>('>',-1),9 },
		{ pair<char,int>('<',-1),10 },
		{ pair<char,int>('=',-1),9 },
		{ pair<char,int>('>',10),12 },
		{ pair<char,int>('=',10),12 },
		{ pair<char,int>('=',9),12 },
		{ pair<char,int>('-',10),11 },			   
		
		{ pair<char,int>('"',-1),18 },
		{ pair<char,int>('A',18),18 },
		{ pair<char,int>('"',18),19 },
		{ pair<char,int>('{',-1),16 },
		{ pair<char,int>('A',16),16 },
		{ pair<char,int>('}',16),20 }
	};

	return m;

}

map<pair<string, int>, int> populateSyntaticStateMatrix()
{
	Tokens tokens;
	//pair<entry,current state> destination state
	//destination state * 100 = shift
	//destination state * 200 = reduce
	map<pair<string, int>, int> m =
	{

	{ pair<string,int>("P",-1), 1 },
	{ pair<string,int>(tokens.inicio,-1),102 },

	{ pair<string,int>("V",2),3 },
	{ pair<string,int>(tokens.varinicio,2),104 },

	{ pair<string,int>("A",3),5 },
	{ pair<string,int>("ES",3),6 },
	{ pair<string,int>("CMD",3),7 },
	{ pair<string,int>("COND",3),8 },
	{ pair<string,int>("CABECALHO",3),13 },
	{ pair<string,int>(tokens.fim,3),109 },
	{ pair<string,int>(tokens.escreva,3),110 },
	{ pair<string,int>(tokens.leia,3),111 },
	{ pair<string,int>(tokens.id,3),112 },
	{ pair<string,int>(tokens.se,3),114 },

	{ pair<string,int>("LV",4),15 },
	{ pair<string,int>("D",4),16 },
	{ pair<string,int>(tokens.varfim,4),117 },
	{ pair<string,int>(tokens.id,4),118 },

	{ pair<string,int>("A",6),19 },
	{ pair<string,int>("ES",6),6 },
	{ pair<string,int>("CMD",6),7 },
	{ pair<string,int>("COND",6),8 },
	{ pair<string,int>("CABECALHO",6),13 },
	{ pair<string,int>(tokens.fim,6),109 },
	{ pair<string,int>(tokens.escreva,6),110 },
	{ pair<string,int>(tokens.leia,6),111 },
	{ pair<string,int>(tokens.id,6),112 },
	{ pair<string,int>(tokens.se,6),114 },

	{ pair<string,int>("A",7),20 },
	{ pair<string,int>("ES",7),6 },
	{ pair<string,int>("CMD",7),7 },
	{ pair<string,int>("COND",7),8 },
	{ pair<string,int>("CABECALHO",7),13 },
	{ pair<string,int>(tokens.fim,7),109 },
	{ pair<string,int>(tokens.escreva,7),110 },
	{ pair<string,int>(tokens.leia,7),111 },
	{ pair<string,int>(tokens.id,7),112 },
	{ pair<string,int>(tokens.se,7),114 },

	{ pair<string,int>("A",8),21 },
	{ pair<string,int>("ES",8),6 },
	{ pair<string,int>("CMD",8),7 },
	{ pair<string,int>("COND",8),8 },
	{ pair<string,int>("CABECALHO",8),13 },
	{ pair<string,int>(tokens.fim,8),109 },
	{ pair<string,int>(tokens.escreva,8),110 },
	{ pair<string,int>(tokens.leia,8),111 },
	{ pair<string,int>(tokens.id,8),112 },

	{ pair<string,int>(tokens.se,8),114 },

	{ pair<string,int>("ARG",10),22 },
	{ pair<string,int>(tokens.lit,10),123 },
	{ pair<string,int>(tokens.num,10),124 },
	{ pair<string,int>(tokens.id,10),125 },

	{ pair<string,int>(tokens.id,11),126 },

	{ pair<string,int>(tokens.RCB,12),127 },

	{ pair<string,int>("ES",13),29 },
	{ pair<string,int>("CMD",13),30 },
	{ pair<string,int>("CORPO",13),28 },
	{ pair<string,int>("COND",13),31 },
	{ pair<string,int>("CABECALHO",13),13 },
	{ pair<string,int>(tokens.escreva,13),110 },
	{ pair<string,int>(tokens.leia,13),111 },
	{ pair<string,int>(tokens.id,13),112 },
	{ pair<string,int>(tokens.se,13),114 },
	{ pair<string,int>(tokens.fimse,13),132 },
	
	{ pair<string,int>(tokens.AB_P,14),133 },

	{ pair<string,int>(tokens.varfim,16),117 },
	{ pair<string,int>(tokens.id,16),118 },
	{ pair<string,int>("D",16),16 },
	{ pair<string,int>("LV",16),34 },

	{ pair<string,int>(tokens.PT_V,17),135 },

	{ pair<string,int>(tokens.inteiro,18),137 },
	{ pair<string,int>(tokens.real,18),138 },
	{ pair<string,int>(tokens.lit,18),139 },
	{ pair<string,int>("TIPO",18),36 },

	{ pair<string,int>(tokens.PT_V,22),140 },

	{ pair<string,int>(tokens.PT_V,26),158 },

	{ pair<string,int>(tokens.id,27),143 },
	{ pair<string,int>(tokens.num,27),144 },
	{ pair<string,int>("LD",27),41 },
	{ pair<string,int>("OPRD",27),42 },

	{ pair<string,int>("CORPO",29),45 },
	{ pair<string,int>("ES",29),29 },
	{ pair<string,int>("CMD",29),30 },
	{ pair<string,int>("COND",29),31 },
	{ pair<string,int>("CABECALHO",29),13 },
	{ pair<string,int>(tokens.fimse,29),132 },
	{ pair<string,int>(tokens.escreva,29),110 },
	{ pair<string,int>(tokens.leia,29),111 },
	{ pair<string,int>(tokens.id,29),112 },
	{ pair<string,int>(tokens.se,29),114 },

	{ pair<string,int>("CORPO",30),46 },
	{ pair<string,int>("ES",30),29 },
	{ pair<string,int>("CMD",30),30 },
	{ pair<string,int>("COND",30),31 },
	{ pair<string,int>("CABECALHO",30),13 },
	{ pair<string,int>(tokens.fimse,30),132 },
	{ pair<string,int>(tokens.escreva,30),110 },
	{ pair<string,int>(tokens.leia,30),111 },
	{ pair<string,int>(tokens.id,30),112 },
	{ pair<string,int>(tokens.se,30),114 },

	{ pair<string,int>("CORPO",31),47 },
	{ pair<string,int>("ES",31),29 },
	{ pair<string,int>("CMD",31),30 },
	{ pair<string,int>("COND",31),31 },
	{ pair<string,int>("CABECALHO",31),13 },
	{ pair<string,int>(tokens.fimse,31),132 },
	{ pair<string,int>(tokens.escreva,31),110 },
	{ pair<string,int>(tokens.leia,31),111 },
	{ pair<string,int>(tokens.id,31),112 },
	{ pair<string,int>(tokens.se,31),114 },

	{ pair<string,int>(tokens.id,33),143 },
	{ pair<string,int>(tokens.num,33),144 },
	{ pair<string,int>("OPRD",33),49 },
	{ pair<string,int>("EXP_R",33),48 },

	{ pair<string,int>(tokens.PT_V,36),150 },

	{ pair<string,int>(tokens.PT_V,41),151 },

	{ pair<string,int>(tokens.OPM,42),152 },

	{ pair<string,int>(tokens.FC_P,48),153 },

	{ pair<string,int>(tokens.OPR,49),154 },

	{ pair<string,int>("OPRD",52),55 },
	{ pair<string,int>(tokens.id,52),143 },
	{ pair<string,int>(tokens.num,52),144 },

	{ pair<string,int>(tokens.entao,53),156 },

	{ pair<string,int>("OPRD",54),57 },
	{ pair<string,int>(tokens.id,54),143 },
	{ pair<string,int>(tokens.num,54),144 },

	{ pair<string,int>(tokens.EndOfFile,1),999 },

	{ pair<string,int>(tokens.EndOfFile,5),202 },

	{ pair<string,int>(tokens.EndOfFile,9),230 },

	{ pair<string,int>(tokens.leia,15),203 },
	{ pair<string,int>(tokens.escreva,15),203 },
	{ pair<string,int>(tokens.id,15),203 },
	{ pair<string,int>(tokens.se,15),203 },
	{ pair<string,int>(tokens.fim,15),203 },

	{ pair<string,int>(tokens.EndOfFile,19),210 },

	{ pair<string,int>(tokens.EndOfFile,20),216 },

	{ pair<string,int>(tokens.EndOfFile,21),222 },

	{ pair<string,int>(tokens.PT_V,23),213 },

	{ pair<string,int>(tokens.PT_V,24),214 },

	{ pair<string,int>(tokens.PT_V,25),215 },

	{ pair<string,int>(tokens.leia,28),223 },
	{ pair<string,int>(tokens.escreva,28),223 },
	{ pair<string,int>(tokens.id,28),223 },
	{ pair<string,int>(tokens.se,28),223 },
	{ pair<string,int>(tokens.fim,28),223 },
	{ pair<string,int>(tokens.fimse,28),223 },

	{ pair<string,int>(tokens.leia,32),229 },
	{ pair<string,int>(tokens.escreva,32),229 },
	{ pair<string,int>(tokens.id,32),229 },
	{ pair<string,int>(tokens.se,32),229 },
	{ pair<string,int>(tokens.fim,32),229 },
	{ pair<string,int>(tokens.fimse,32),229 },

	{ pair<string,int>(tokens.leia,34),204 },
	{ pair<string,int>(tokens.escreva,34),204 },
	{ pair<string,int>(tokens.id,34),204 },
	{ pair<string,int>(tokens.se,34),204 },
	{ pair<string,int>(tokens.fim,34),204 },

	{ pair<string,int>(tokens.leia,35),205 },
	{ pair<string,int>(tokens.escreva,35),205 },
	{ pair<string,int>(tokens.id,35),205 },
	{ pair<string,int>(tokens.se,35),205 },
	{ pair<string,int>(tokens.fim,35),205 },

	{ pair<string,int>(tokens.PT_V,37),207 },

	{ pair<string,int>(tokens.PT_V,38),208 },

	{ pair<string,int>(tokens.PT_V,39),209 },

	{ pair<string,int>(tokens.leia,40),212 },
	{ pair<string,int>(tokens.escreva,40),212 },
	{ pair<string,int>(tokens.id,40),212 },
	{ pair<string,int>(tokens.se,40),212 },
	{ pair<string,int>(tokens.fim,40),212 },
	{ pair<string,int>(tokens.fimse,40),212 },

	{ pair<string,int>(tokens.PT_V,42),219 },

	{ pair<string,int>(tokens.PT_V,43),220 },
	{ pair<string,int>(tokens.FC_P,43),220 },
	{ pair<string,int>(tokens.OPM,43),220 },
	{ pair<string,int>(tokens.OPR,43),220 },

	{ pair<string,int>(tokens.PT_V,44),221 },
	{ pair<string,int>(tokens.FC_P,44),221 },
	{ pair<string,int>(tokens.OPM,44),221 },
	{ pair<string,int>(tokens.OPR,44),221 },

	{ pair<string,int>(tokens.leia,45),226 },
	{ pair<string,int>(tokens.escreva,45),226 },
	{ pair<string,int>(tokens.id,45),226 },
	{ pair<string,int>(tokens.se,45),226 },
	{ pair<string,int>(tokens.fim,45),226 },
	{ pair<string,int>(tokens.fimse,45),226 },

	{ pair<string,int>(tokens.leia,46),227 },
	{ pair<string,int>(tokens.escreva,46),227 },
	{ pair<string,int>(tokens.id,46),227 },
	{ pair<string,int>(tokens.se,46),227 },
	{ pair<string,int>(tokens.fim,46),227 },
	{ pair<string,int>(tokens.fimse,46),227 },

	{ pair<string,int>(tokens.leia,47),228 },
	{ pair<string,int>(tokens.escreva,47),228 },
	{ pair<string,int>(tokens.id,47),228 },
	{ pair<string,int>(tokens.se,47),228 },
	{ pair<string,int>(tokens.fim,47),228 },
	{ pair<string,int>(tokens.fimse,47),228 },

	{ pair<string,int>(tokens.id,50),206 },
	{ pair<string,int>(tokens.varfim,50),206 },

	{ pair<string,int>(tokens.leia,51),217 },
	{ pair<string,int>(tokens.escreva,51),217 },
	{ pair<string,int>(tokens.id,51),217 },
	{ pair<string,int>(tokens.se,51),217 },
	{ pair<string,int>(tokens.fim,51),217 },
	{ pair<string,int>(tokens.fimse,51),217 },

	{ pair<string,int>(tokens.PT_V,55),218 },

	{ pair<string,int>(tokens.leia,56),224 },
	{ pair<string,int>(tokens.escreva,56),224 },
	{ pair<string,int>(tokens.id,56),224 },
	{ pair<string,int>(tokens.se,56),224 },
	{ pair<string,int>(tokens.fim,56),224 },
	{ pair<string,int>(tokens.fimse,56),224 },

	{ pair<string,int>(tokens.FC_P,57),225 },

	{ pair<string,int>(tokens.leia,58),211 },
	{ pair<string,int>(tokens.escreva,58),211 },
	{ pair<string,int>(tokens.id,58),211 },
	{ pair<string,int>(tokens.se,58),211 },
	{ pair<string,int>(tokens.fim,58),211 },
	{ pair<string,int>(tokens.fimse,58),211 },

	};

	return m;

}

map<int, pair<string,string>> populateProductions()
{
	struct Tokens tokens;
	map<int, pair<string, string>> m =
	{
	{1,pair<string,string>("P'","P")},
	{2,pair<string,string>("P",tokens.inicio+" V A")},
	{3,pair<string,string>("V",tokens.varinicio + " LV")},
	{4,pair<string,string>("LV","D LV")},
	{5,pair<string,string>("LV",tokens.varfim+" "+tokens.PT_V)},
	{6,pair<string,string>("D",tokens.id+" TIPO "+tokens.PT_V)},
	{7,pair<string,string>("TIPO",tokens.inteiro)},
	{8,pair<string,string>("TIPO",tokens.real)},
	{9,pair<string,string>("TIPO",tokens.lit)},
	{10,pair<string,string>("A","ES A")},
	{11,pair<string,string>("ES",tokens.leia + " "+tokens.id+" "+tokens.PT_V)},
	{12,pair<string,string>("ES",tokens.escreva + " ARG "+tokens.PT_V)},
	{13,pair<string,string>("ARG",tokens.lit)},
	{14,pair<string,string>("ARG",tokens.num)},
	{15,pair<string,string>("ARG",tokens.id)},
	{16,pair<string,string>("A","CMD A")},
	{17,pair<string,string>("CMD",tokens.id+" "+tokens.RCB+" LD "+tokens.PT_V)},
	{18,pair<string,string>("LD","OPRD "+tokens.OPM+" OPRD")},
	{19,pair<string,string>("LD","OPRD")},
	{20,pair<string,string>("OPRD",tokens.id)},
	{21,pair<string,string>("OPRD",tokens.num)},
	{22,pair<string,string>("A","COND A")},
	{23,pair<string,string>("COND","CABECALHO CORPO")},
	{24,pair<string,string>("CABECALHO",tokens.se+" "+tokens.AB_P+" EXP_R "+tokens.FC_P+" "+tokens.entao)},
	{25,pair<string,string>("EXP_R","OPRD "+tokens.OPR+" OPRD")},
	{26,pair<string,string>("CORPO","ES CORPO")},
	{27,pair<string,string>("CORPO","CMD CORPO")},
	{28,pair<string,string>("CORPO","COND CORPO")},
	{29,pair<string,string>("CORPO",tokens.fimse)},
	{30,pair<string,string>("A",tokens.fim)},
	};

	return m;
}

multimap<int, tuple<string, string, string>> populateMainTable()
{
	struct Tokens tokens;
	// line number, lexeme, token, type
	multimap<int, tuple<string, string, string>> m =
	{
		{ 0, tuple<string, string, string>(tokens.inicio, tokens.inicio, " ") },
		{ 0, tuple<string, string, string>(tokens.varinicio, tokens.varinicio, " ") },
		{ 0, tuple<string, string, string>(tokens.varfim, tokens.varfim, " ") },
		{ 0, tuple<string, string, string>(tokens.leia, tokens.leia, " ") },
		{ 0, tuple<string, string, string>(tokens.escreva, tokens.escreva, " ") },
		{ 0, tuple<string, string, string>(tokens.se, tokens.se, " ") },
		{ 0, tuple<string, string, string>(tokens.entao, tokens.entao, " ") },
		{ 0, tuple<string, string, string>(tokens.senao, tokens.senao, " ") },
		{ 0, tuple<string, string, string>(tokens.fimse, tokens.fimse, " ") },
		{ 0, tuple<string, string, string>(tokens.inteiro, tokens.inteiro, " ") },
		{ 0, tuple<string, string, string>(tokens.lit, tokens.lit, " ") },
		{ 0, tuple<string, string, string>(tokens.real, tokens.real, " ") },
		{ 0, tuple<string, string, string>(tokens.fim, tokens.fim, " ") },
	};

	return m;
}

int countWords(string str)
{
	bool inSpaces = true;
	int numWords = 0;

	for (char& c : str)
	{
		if (c == '\0') break;
		if (isspace(c))
		{
			inSpaces = true;
		}
		else if (inSpaces)
		{
			numWords++;
			inSpaces = false;
		}
	}

	return numWords;
}

char entry(char c, int state)
{
	if (state == 2 || state == 4)
	{
		if (c == 'e') return c;
		if (c == 'E') return c;
	}

	if (state == 18)
	{
		if (c != '"') return 'A';
		else return c;
	}

	if (state == 16)
	{
		if (c != '}') return 'A';
		else return c;
	}

	if (c == '\n' || c == ' ' || c == '\t' || c == '\0') return 'S';

	if (isalpha(c)) return 'L';
	if (isdigit(c)) return 'D';
	else return c;
}

string spaces(int previous)
{
	int sp = 30;
	int i = 0;
	string spac = " ";

	for (i = 0; i < sp - previous; i++)
	{
		spac += " ";
	}

	return spac;
}


