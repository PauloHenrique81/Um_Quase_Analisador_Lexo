#include <QCoreApplication>
#include <iostream>
#include <QStringList>
#include <stdio.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <conio.h>
#include <ctype.h>
#include <map>
using namespace std;


//-----------------DEFINIÇÃO DE VARIAVEIS-----------------------------------------------------------------------



//-----------------DEFINIÇÃO DE VETORES GOBLAIS - DEFINIÇÃO DA LINGUAGEM-----------------------------------------
QString vet_palavras_reservadas[13];
QString vet_tokens_sem_atributos_chave[28];
QString vet_tokens_sem_atributos_valor[28];

//-------------------------------FUNÇOES DA LINGUAGEM--------------------------------------------------------------------------------------
void inicializa_vetor_de_palavras_reservadas() {
    vet_palavras_reservadas[0] = "include";
    vet_palavras_reservadas[1] = "main";
    vet_palavras_reservadas[2] = "int";
    vet_palavras_reservadas[3] = "float";
    vet_palavras_reservadas[4] = "string";
    vet_palavras_reservadas[5] = "char";
    vet_palavras_reservadas[6] = "bool";
    vet_palavras_reservadas[7] = "if";
    vet_palavras_reservadas[8] = "iostream";
    vet_palavras_reservadas[9] = "using";
    vet_palavras_reservadas[10] = "namespace";
    vet_palavras_reservadas[11] = "std";
    vet_palavras_reservadas[12] = "cout";

}

bool verifica_se_eh_uma_palavra_reservada(QString palavra) {

    for (int i = 0; i < 13; i++) {
        if (palavra.compare(vet_palavras_reservadas[i]) == 0)
            return true;
    }

    return false;
}

void inicializa_vet_de_tokens_sem_atributos() {
    vet_tokens_sem_atributos_chave[0] = "ADD"; vet_tokens_sem_atributos_valor[0] = "+";
    vet_tokens_sem_atributos_chave[1] = "SUB"; vet_tokens_sem_atributos_valor[1] = "-";
    vet_tokens_sem_atributos_chave[2] = "MUL"; vet_tokens_sem_atributos_valor[2] = "*";
    vet_tokens_sem_atributos_chave[3] = "DIV"; vet_tokens_sem_atributos_valor[3] = "/";
    vet_tokens_sem_atributos_chave[4] = "RES"; vet_tokens_sem_atributos_valor[4] = "%";
    vet_tokens_sem_atributos_chave[5] = "IGU"; vet_tokens_sem_atributos_valor[5] = "==";
    vet_tokens_sem_atributos_chave[6] = "DIF"; vet_tokens_sem_atributos_valor[6] = "!=";
    vet_tokens_sem_atributos_chave[7] = "ME";  vet_tokens_sem_atributos_valor[7] = "<";
    vet_tokens_sem_atributos_chave[8] = "MA";  vet_tokens_sem_atributos_valor[8] = ">";
    vet_tokens_sem_atributos_chave[9] = "MEI"; vet_tokens_sem_atributos_valor[9] = "<=";
    vet_tokens_sem_atributos_chave[10] = "MAI"; vet_tokens_sem_atributos_valor[10] = ">=";
    vet_tokens_sem_atributos_chave[11] = "AND"; vet_tokens_sem_atributos_valor[11] = "&&";
    vet_tokens_sem_atributos_chave[12] = "OR";  vet_tokens_sem_atributos_valor[12] = "||";
    vet_tokens_sem_atributos_chave[13] = "NOT"; vet_tokens_sem_atributos_valor[13] = "!";
    vet_tokens_sem_atributos_chave[14] = "CA";  vet_tokens_sem_atributos_valor[14] = "{";
    vet_tokens_sem_atributos_chave[15] = "CF";  vet_tokens_sem_atributos_valor[15] = "}";
    vet_tokens_sem_atributos_chave[16] = "PA";  vet_tokens_sem_atributos_valor[16] = "(";
    vet_tokens_sem_atributos_chave[17] = "PF";  vet_tokens_sem_atributos_valor[17] = ")";
    vet_tokens_sem_atributos_chave[18] = "PV";  vet_tokens_sem_atributos_valor[18] = ";";
    vet_tokens_sem_atributos_chave[19] = "CE";  vet_tokens_sem_atributos_valor[19] = "#";
    vet_tokens_sem_atributos_chave[20] = "AA";  vet_tokens_sem_atributos_valor[20] = "“";
    vet_tokens_sem_atributos_chave[21] = "AF";  vet_tokens_sem_atributos_valor[21] = "”";
    vet_tokens_sem_atributos_chave[22] = "APA"; vet_tokens_sem_atributos_valor[22] = "‘";
    vet_tokens_sem_atributos_chave[23] = "APF"; vet_tokens_sem_atributos_valor[23] = "’";
    vet_tokens_sem_atributos_chave[24] = "PT";  vet_tokens_sem_atributos_valor[24] = ".";
    vet_tokens_sem_atributos_chave[25] = "IG";  vet_tokens_sem_atributos_valor[25] = "=";
    vet_tokens_sem_atributos_chave[26] = "EB";  vet_tokens_sem_atributos_valor[26] = " ";
}

QString verifica_se_eh_um_tipo_de_token_sem_atributo(QString palavra) {

    for (int i = 0; i < 27; i++) {
        if (palavra.compare(vet_tokens_sem_atributos_valor[i]) == 0)
            return vet_tokens_sem_atributos_chave[i];
    }

    return "vazio";
}

QString concatenaString(QString palavra){

    palavra = "["+palavra+"]";
    return palavra;
}

bool verificaSeEhNumero(QString palavra){
    for(int i=0; i<palavra.size(); i++){
        if(!palavra[i].isNumber())
            return false;
    }

    return true;
}
//---------------------------------------Automatos------------------------------------------------------------------------------------


map<pair<QString, int>, int> geraAutomato2()
{

    map<pair<QString, int>, int> m =
        {
            { pair<QString,int>("[VAR]",-1),1 },
            { pair<QString,int>("[EB]",1),1 },

            { pair<QString,int>("[IG]",1),2 },
            { pair<QString,int>("[EB]",2),2 },

            { pair<QString,int>("[VAR]",2),3 },
            { pair<QString,int>("[EB]",3),3 },

            { pair<QString,int>("[ADD]",3),4 },
            { pair<QString,int>("[SUB]",3),4 },
            { pair<QString,int>("[MUL]",3),4 },
            { pair<QString,int>("[DIV]",3),4 },

            { pair<QString,int>("[EB]",4),4 },
            { pair<QString,int>("[VAR]",4),50 },



        };
    return m;
}

map<pair<QChar, int>, int> geraAutomato()
{
	
    map<pair<QChar, int>, int> m =
        {
            { pair<QChar,int>('S',-1),-1 },
            { pair<QChar,int>('S',1),-1 },
            { pair<QChar,int>('S',2),-1 },
            { pair<QChar,int>('S',4),-1 },
            { pair<QChar,int>('S',5),-1 },
            { pair<QChar,int>('S',6),-1 },
            { pair<QChar,int>('S',7),-1 },
            { pair<QChar,int>('S',8),-1 },
            { pair<QChar,int>('S',9),-1 },
            { pair<QChar,int>('S',10),-1 },
            { pair<QChar,int>('S',11),-1 },
            { pair<QChar,int>('S',12),-1 },
            { pair<QChar,int>('S',13),-1 }, //--------
            { pair<QChar,int>('S',14),-1 },
            { pair<QChar,int>('S',15),-1 },  //--------
            { pair<QChar,int>('S',16),-1 },
            { pair<QChar,int>('S',18),-1 },
            { pair<QChar,int>('S',20),-1 },
            { pair<QChar,int>('S',21),-1 },
            { pair<QChar,int>('S',22),-1 },
            { pair<QChar,int>('S',23),-1 },
            { pair<QChar,int>('S',24),-1 },
            { pair<QChar,int>('S',25),-1 },
            { pair<QChar,int>('S',26),-1 },
            { pair<QChar,int>('S',28),-1 },
            { pair<QChar,int>('S',29),-1 },
            { pair<QChar,int>('S',30),-1 },

            { pair<QChar,int>('L',-1),1 },
            { pair<QChar,int>('L',1),1  },
            { pair<QChar,int>('>',1),15 },
            { pair<QChar,int>(';',1),25 },

            { pair<QChar,int>('D',-1),2 },
            { pair<QChar,int>('D',1),1 },
            { pair<QChar,int>('D',2),2 },

            { pair<QChar,int>('=',-1),4 },
            { pair<QChar,int>('=',4),5 },

            { pair<QChar,int>('+',-1),6 },

            { pair<QChar,int>('-',-1),7 },

            { pair<QChar,int>('*',-1),8 },

            { pair<QChar,int>('/',-1),9 },

            { pair<QChar,int>('%',-1),10 },

            { pair<QChar,int>('!',-1),11 },

            { pair<QChar,int>('=',11),12 },

            { pair<QChar,int>('<',-1),13 }, //--------

            { pair<QChar,int>('<',13),13 },

            { pair<QChar,int>('"', 13),27 },

            { pair<QChar,int>('=',13),14 },

            { pair<QChar,int>('L',13),1 },

            { pair<QChar,int>('>',-1),15 }, //--------

            { pair<QChar,int>('=',15),16 },

            { pair<QChar,int>('&',-1),17 },

            { pair<QChar,int>('&',17),18 },

            { pair<QChar,int>('|',-1),19 },

            { pair<QChar,int>('|',19),20 },

            { pair<QChar,int>('{',-1),21 },

            { pair<QChar,int>('\n',21), 30 },

            { pair<QChar,int>('}',-1),22 },

            { pair<QChar,int>('(',-1),23 },

            { pair<QChar,int>(')',23),24 },

            { pair<QChar,int>(')',-1),24 },

            { pair<QChar,int>(';',-1),25 },

            { pair<QChar,int>('\n',25), 30 },

            { pair<QChar,int>('#',-1),26 },

            { pair<QChar,int>('L',26),1 },

            { pair<QChar,int>('"',-1),27 },

            { pair<QChar,int>('L',27),27 },

            { pair<QChar,int>('D',27),27 },

            { pair<QChar,int>('S',27),27 },

            { pair<QChar,int>('"',27),28 },

            { pair<QChar,int>(';',28),25 },

            { pair<QChar,int>('\n', 28),30 },

            { pair<QChar,int>('.',-1),29 },

            { pair<QChar,int>('\n',-1),30 },

            { pair<QChar,int>('\n', 30),30 },

            { pair<QChar,int>('L',30),1 }
        };

	return m;

}

map<int, QString> geraEstadosFinais()
{
    map<int, QString> m =
    {
        { 1,"VAR" },
        { 2,"DIG" },
        { -1,"EB" },
        { 4,"IG" },
        { 5,"IGU" },
        { 6,"ADD" },
        { 7,"SUB" },
        { 8,"MUL" },
        { 9,"DIV" },
        { 10,"RES" },
        { 11,"NOT" },
        { 12,"DIF" },
        { 13,"ME" },  //--------
        { 14,"MEI" },
        { 15,"MA" },  //--------
        { 16,"MAI" },
        { 18,"AND" },
        { 20,"OR" },
        { 21,"CA" },
        { 22,"CF" },
        { 23,"PA" },
        { 24,"PF" },
        { 25,"PV" },
        { 26,"CE" },
        { 28,"TEXTO ENTRE ASPAS DUPLAS" },
        { 29,"PT" },
        { 30,"\N" }



     };

	return m;

}

QChar entry(QChar c)
{

    if (c == ' ') return 'S';
    if (c.isLetter()) return 'L';
    if (c.isNumber()) return 'D';

    else return c;
}

//---------------------------ARQUIVOS---------------------------------------------------------------------------

void write(QString FileName, QString texto){

    QFile file(FileName);

    if(!file.open(QFile::WriteOnly | QFile::Text)){
        cout<<"Erro ao abrir o arquivo"<<endl;
    }

    QTextStream out(&file);
    out << texto;
    file.flush();
    file.close();
}

QString read (QString FileName){

    QFile file(FileName);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
          cout<<"Erro ao abrir o arquivo"<<endl;
    }

    QTextStream in (&file);
    QString mText = in.readAll();

    file.flush();
    file.close();

    return mText;
}

//--------------------------------------------------------------------------------------------------------------

QString retiraIntervalo(int inicio, int fim, QString palavra, int tamanho){

    QString saida;
    QString a,b,c,d,e;

    a = "[";
    b = "V";
    c = "A";
    d = "R";
    e = "]";


    for(int i=0; i<tamanho; i++){

        if(i >= inicio && i <= fim ){

            if(a != ""){
                saida += a;
                a = "";
            }
            if(b != ""){
                saida += b;
                b = "";
            }
            if(c != "" ) {
                saida += c;
                c = "";
            }
            if(d != "") {
                saida += d;
                d = "";
            }

            if(e != "" ){
                saida += e;
                e = "";
            }

        }else{
              saida += palavra[i];
        }

    }

    return saida;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    inicializa_vetor_de_palavras_reservadas();
    inicializa_vet_de_tokens_sem_atributos();

    map<pair<QString, int>, int> automato2 = geraAutomato2();
    map<pair<QChar, int>, int> automato = geraAutomato();
    map<int, QString> estadosFinais = geraEstadosFinais();

    QString arquivo1 = "C:/Users/invent/Desktop/AnalisadorLexoLFA/entrada.txt";
    QString arquivoCopia = "C:/Users/invent/Desktop/AnalisadorLexoLFA/copia.txt";
    QString arquivo2 = "C:/Users/invent/Desktop/AnalisadorLexoLFA/saida.txt";

    QString leituraArquivo;
    QString arquivoSaida;
    leituraArquivo = read(arquivo1);

    write(arquivoCopia,leituraArquivo);

    QChar caracter;
    QString stringAuxiliar;

    int estadoAtual = -1;
    int proximoEstado = -1;
    QString estadoFinal;


    int tamanhoDoArquivo = leituraArquivo.size();


    for(int i=0; i<tamanhoDoArquivo; i++){

        caracter = leituraArquivo[i];


        estadoAtual = proximoEstado;
        proximoEstado = automato[pair<QChar, int>(entry(caracter), estadoAtual)];

        //cout<<caracter.toLatin1()<<endl;
       // cout<<proximoEstado<<endl;

        estadoFinal = estadosFinais[proximoEstado];



        if(entry(caracter) == 'S' || leituraArquivo[i+1] == '>' || leituraArquivo[i+1] == '\0' || entry(caracter) == '>' || entry(caracter) == '\n' || entry(caracter) == ';' ){

//            cout<<stringAuxiliar.toStdString()<<endl;
//            cout<<caracter.toLatin1()<<endl;
//            cout<<proximoEstado<<endl;



            if(leituraArquivo[i+1] == '\0'){
                 if(estadoFinal != 0)
                 arquivoSaida += concatenaString(estadoFinal);
             }

            if(stringAuxiliar.isNull()){

                if(entry(caracter) == '>')
                     arquivoSaida += concatenaString(estadoFinal);

                if(entry(caracter) == '\0')
                     arquivoSaida += concatenaString(estadoFinal);

                if(entry(caracter) == '\n')
                     arquivoSaida += '\n';

                if(entry(caracter) == 'S')
                     arquivoSaida += "[EB]";


            }else{

                if(entry(caracter) == 'L') {
                    stringAuxiliar += caracter;
                }else{
                    if(entry(caracter) == 'D'){
                        stringAuxiliar += caracter;
                    }/*else{
                        if(proximoEstado != 0 && proximoEstado != -1)
                            arquivoSaida += concatenaString(estadoFinal);
                            cout<<"Aqui"+estadoFinal.toStdString()<<endl;

                    }*/
                }

                if(verificaSeEhNumero(stringAuxiliar)){
                    //cout<<stringAuxiliar.toStdString()<<endl;
                    arquivoSaida += "[DIG]";
                    stringAuxiliar.clear();
                }else{
                    if(verifica_se_eh_uma_palavra_reservada(stringAuxiliar)){
                       // cout<<"PR"+stringAuxiliar.toStdString()<<endl;
                        arquivoSaida += "[PALAVRA_RESERVADA]";
                        stringAuxiliar.clear();
                    }else{

                           // cout<<"AQUIIIIII"+stringAuxiliar.toStdString()<<endl;
                            arquivoSaida += "[VAR]";
                            stringAuxiliar.clear();

                    }
            }

                if(!(leituraArquivo[i+1] == '\0')){
                    if(estadoFinal.compare("VAR") != 0 )
                        if(estadoFinal != 0)
                             arquivoSaida += concatenaString(estadoFinal);
                }


         }

        }else{
//            cout<<"else"<<endl;
//            cout<<caracter.toLatin1()<<endl;
//            cout<<proximoEstado<<endl;
//            cout<<estadoFinal.toStdString()<<endl;


            if(entry(caracter) == 'L') {
                stringAuxiliar += caracter;

            }else{
                if(entry(caracter) == 'D'){
                    stringAuxiliar += caracter;
                }else{
                    if(proximoEstado != 0)
                    arquivoSaida += concatenaString(estadoFinal);

                }
            }


        }

    }


    stringAuxiliar.clear();
    tamanhoDoArquivo = arquivoSaida.size();
    proximoEstado = -1;

    int inicio = 0;
    int fim;
    int aux;
    int aux2 = 0;
    bool boler;

    for(int i=0; i<tamanhoDoArquivo; i++){

        caracter = arquivoSaida[i];

        cout<<caracter.toLatin1()<<endl;

        if(caracter == ']'){

            stringAuxiliar += caracter;
            estadoAtual = proximoEstado;

            aux = automato2[pair<QString, int>(stringAuxiliar, estadoAtual)];

            if(aux != 0)
                proximoEstado = aux;

            //cout<<stringAuxiliar.toStdString()<<endl;
            cout<<"ProximoEstado";
            cout<<proximoEstado<<endl;
            cout<<i<<endl;

            if(proximoEstado == 1 && inicio == 0)
                inicio = i - 3;


            if(proximoEstado == 50 && aux2 ==0){
                fim = i;
                aux2 = 1;
                boler = true;

            }

            stringAuxiliar.clear();

        }else{
            stringAuxiliar += caracter;

        }

    }


    cout<<tamanhoDoArquivo<<endl;

    QString arquivoSaida2;

    if(boler){

        cout<<arquivoSaida.toStdString()<<endl;

        arquivoSaida2 = retiraIntervalo(inicio,fim,arquivoSaida,arquivoSaida.size());
        cout<<"-----------------------------------------------------------------------------------------------"<<endl;
        cout<<arquivoSaida2.toStdString()<<endl;
    }




//    cout<<"inicio";
//    cout<<inicio<<endl;
//    cout<<"Fim";
//    cout<<fim<<endl;

    write(arquivo2,arquivoSaida);

    return a.exec();
}




