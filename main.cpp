#include <iostream>
#include <fstream>
#include "finiteAutomata.h"

using namespace std;
//функция, която по даден текстов файл и регулярен израз 
//извежда всички редове, които се разпознават от регулярния израз
void readWords(string file, string regularExpr)
{
    FiniteAutomata ExprToAut;
    ExprToAut = ExprToAut.exprToAutomata(regularExpr);
    
    ifstream f(file);
    if (!f.good())
    {  
        cout<<"Error"<<endl;
        f.close();
    }
    string w;
    while(f.good())
    {
       f >> w;
       if (ExprToAut.isMatch(w)) cout<<w<<endl;
    } 
    f.close();
}

int main()
{
    // Примерен регулярен израз и файл
    string regularExpr = "c*.(a+b)*.b";
    string file = "words.txt";
    readWords(file, regularExpr);

    return 0;
}