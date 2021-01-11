#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <iterator>
#include <ostream>
#include <stack>
#include <queue>
#include "finiteAutomata.h"

using namespace std;

// взета готова за извеждане на вектор
template <typename T>
ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    if (!v.empty()){
        out << '[';
        std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out<<"\b\b]";
    }
    return out;
}

bool isOperator(char op) // Проверка дали даден символ е оператор (без скобите)
{
    if (op == '*' || op == '.' || op == '&' || op == '+')
        return true;
    return false;
}

int PrecedenceOfOp(char op) // Приоритет на операторите
{
    int precNum;
    switch (op)
    {
        case '(': { precNum = 4; break; }
        case ')': { precNum = 4; break; }
        case '*': { precNum = 3; break; }
        case '.': { precNum = 2; break; }
        case '&': { precNum = 1; break; }
        case '+': { precNum = 1; break; }
        default : {precNum = 0; break;}
    }
    return precNum;
}

// обратен полски запис с Shunting-Yard Algorithm
// необходим за преобразуването на регулярен израз до автомат
string rpn(const string& expr)
{
    string regularExpr = expr;
    string newExpr = ""; // регулярният израз в полски запис
    stack<char> st; // стек, в който се въвеждат операторите от регулярния израз
    queue<char> q; // опашка, в която се преобразува регулярният израз в полски запис
    while (!regularExpr.empty())
    {
        char sym = regularExpr[0]; // първи елемент от регулярния израз (моментен символ)
        if (sym >= 'a' && sym <= 'z') 
        {
            q.push(sym); // ако символът е малка латинска буква, се добавя в опашката
        }
        else if (isOperator(sym)) // ако символът sym е оператор (и не е скоба)
            {
                while (!st.empty() // проверява се дали стекът е празен,
                        && isOperator(st.top()) //  дали top-а на стека е оператор,
                        && (PrecedenceOfOp(st.top()) >= PrecedenceOfOp(sym)) // приоритетът на операторите
                        && (st.top() != '(')) // дали top-а на стека не е лява скоба
                {   // ако горните 4 условия са изпълнени, top-а на стека се добавя в опашката и се трие от стека
                    q.push(st.top()); 
                    st.pop(); 
                }
                st.push(sym); // моментният символ sym се добавя в стека
            }
        else if (sym == '(') // ако символът sym е лява скоба се добавя към стека
        {
            st.push(sym);
        }
        else if (sym == ')') // ако моментният символ е дясна скоба
            {
                while (st.top() != '(' && !st.empty()) // докато top-а на стека не е лява скоба и стекът не е празен,
                // символите от стека се прибавят към опашката и се трият от стека
                {
                    q.push(st.top());
                    st.pop();
                }
                if (st.empty())  // ако стекът е празен, т.е. не сме стигнали до затваряща лява скоба, излиза съобщение "Error" и излизаме от функцията
                { 
                    cout << "Error"<<endl; return "";
                }
                st.pop();   // ако сме достигнали до лява скоба в стека я премахваме от него
            }
        regularExpr.erase(0,1); // премахваме първият елемент на регулярния израз
    }
    while (!st.empty()) // ако все още имаме елементи в стека, ги премахваме от стека и ги добавяме към опашката
    {
        q.push(st.top());
        st.pop();
    }
   
    while(!q.empty()) // регулярният израз в полски запис от опашка към стринг
    {
        newExpr+=q.front();
        q.pop();
    }
    return newExpr;
}

void FiniteAutomata::fromWord(const string& word) // създаваме автомат с дума word
{ 
    clear();
    first = "0";
    for (unsigned i = 0;i <= word.length(); i++)
    {
        states.emplace(to_string(i)); // добавяме състояния колкото са буквите в думата + 1
    }
    finalStates.emplace(to_string(word.length())); // правим последното състояние финално
    for (unsigned i = 0; i < word.length(); i++)
    {
        string c(1,word[i]); // от char към string
        transitions[{to_string(i), c}] = {to_string(i+1)}; // създаваме преходите
    }
}

void FiniteAutomata::emptyWord() // създаваме автомат, който чете празната дума
{
    clear();
    first = "0";
    states.emplace("0");
    finalStates.emplace("0");
}

FiniteAutomata FiniteAutomata::emptyWord2()// създаваме автомат, който чете празната дума, но методът връща обект от тип FiniteAutomata
{
    emptyWord();
    return *this;
}

bool FiniteAutomata::isFromAlphabet(char c) // проверка дали даден символ е малка латинска дума
{
    return ('a' <= c && 'z' >= c);
}

void FiniteAutomata::wildcat() // създаване на автомат, който чете произволен символ
{ 
    clear();
    first = "0";
    states.emplace("0");
    for (char c='a'; c<='z';c++) 
    {
        State state = to_string(unsigned(c - 'a') + 1); 
        states.emplace(state);
        finalStates.emplace(state);
        
        Transition transition(1,c);
        transitions[{"0", transition}] = {state}; 
    }
}

FiniteAutomata FiniteAutomata::wildcat2() // създаване на автомат, който чете произволен символ, но методът връща обект от тип FiniteAutomata
{
    wildcat();
    return *this;
}

bool FiniteAutomata::isFinalState(const State& st) // проверка дали дадено състояние е финално
{
    return finalStates.count(st);
}

struct WordStatePair{
    string word;
    State state;    
};

// Проверка за принадлежност на дума към езика на автомата. Използване на DFS.
bool FiniteAutomata::isMatch(const string& word)
{
    if (word.empty() && isFinalState(first)) // ако думата е празната дума и първото състояние е финално, думата принадлежи на езика на автомата
    {
        return true;   
    }
    if (word.empty() && !isFinalState(first)) // ако думата е празната дума и първото състояние не е финално, думата не принадлежи на езика на автомата
    {
        return false;
    }
    stack<WordStatePair> DFSStack; // създаваме стек от тип структурата WordStatePair (двойка дума и състояние)
    WordStatePair wsp{word,first}; // създаваме обект от тип структурата WordStatePair
    DFSStack.push(wsp); // добавяме wsp към стека
    while (!DFSStack.empty()) // докато стекът не е празен
    {
        wsp = DFSStack.top(); // wsp е top-a на стека
        DFSStack.pop(); // премахваме top-a на стека
        if (wsp.word.empty() && isFinalState(wsp.state))
        {
            return true; // ако думата на wsp е празна и състоянието на wsp е финално, думата word принадлежи на езика на автомата
        }
        if (!wsp.word.empty()) // ако думата на wsp не е празна
        {
            string character = string(1,wsp.word.front()); // първият символ на думата на wsp
            if (transitions.count({wsp.state, character})) // ако съществува преход character от състоянието на wsp
            {
                for (const State& nextState : transitions[{wsp.state, character}]) // за всяко състояние, към което има преход character от състоянието на wsp
                {
                    
                    WordStatePair nextPair{wsp.word.substr(1,wsp.word.length()-1), nextState}; // създаваме двойка с думата на wsp, без първия символ, и състоянието nextState
                    DFSStack.push(nextPair); // добавяме двойката nextPair в стека
                    
                }
            }
            if (transitions.count({wsp.state, ""})) // ако съществува епсилон-преход от състоянието wsp
            {
                for (const State& nextState : transitions[{wsp.state, ""}]) 
                {
                    if (nextState != wsp.state)
                    {
                        WordStatePair nextPair{wsp.word, nextState}; // създаваме двойка с думата на wsp и състоянието nextState
                        DFSStack.push(nextPair); // добавяме двойката в стека
                    }
                }
            }            
        }
    }
    return false; // думата не принадлежи на езика на автомата
}

// Обединение
FiniteAutomata FiniteAutomata::operator+ (const FiniteAutomata& other) const
{
    State newFirst = "0"; // създаваме ново начално състояние "0"
    // ламбда функция за добавяне на стринг в края на името на състоянията (приема като аргумент състояния)
    auto addStringToItem = [](const unordered_set<State>& from, const string& str){
       unordered_set<State> result;
        for(auto state: from)
        {
            string resStr = state + str;
            result.emplace(resStr);
        }
        return result;
    };
    // ламбда функция за добавяне на стринг в края на името на състоянията (приема като аргумент двойки от състояния и преходи)
    auto transformTransitions = [](const unordered_map<StateTransitionPair, vector<State>>& m, const string& str) {
        unordered_map<StateTransitionPair, vector<State>> newMap;
        for (auto mapPair: m){
            StateTransitionPair pair = mapPair.first;
            pair.state += str;
            vector<State> newDestinations;
            for(auto destState: mapPair.second){
                newDestinations.push_back(destState + str);
            }
            newMap[pair] = newDestinations;
        }
        return newMap;
    };
    // правим копие на първия автомат, като променяме имената на състоянията му (добавяме "1" в края)
    unordered_map<StateTransitionPair, vector<State>> firstTransitions = transformTransitions(transitions, "1");
    unordered_set<State> firstStates = addStringToItem(states, "1"); 
    unordered_set<State> firstFinalStates = addStringToItem(finalStates, "1");
    State firstFirst = this->first + "1";
    // правим копие на втория автомат, като променяме имената на състоянията му (добавяме "2" в края)
    unordered_map<StateTransitionPair, vector<State>> secondTransitions = transformTransitions(other.transitions, "2");
    unordered_set<State> secondStates = addStringToItem(other.states, "2"); 
    unordered_set<State> secondFinalStates = addStringToItem(other.finalStates, "2");
    State secondFirst = other.first + "2";

// за обединяване на двата автомата в 1
    firstTransitions.merge(secondTransitions); // обединяваме преходите на копията на първия и втория автома
    firstStates.merge(secondStates); // обединяваме състояния на копията на първия и втория автомат
    firstFinalStates.merge(secondFinalStates); // обединяваме финалните състояния на копията на първия и втория автомат

    firstTransitions[{newFirst,""}] = {firstFirst, secondFirst}; // правим епсилон-преход от новото начално състояние към началните състояния на копията на първия и втория автомат

    firstStates.emplace(newFirst);
        
    return FiniteAutomata(firstTransitions, firstStates, firstFinalStates, newFirst); // връща автомат с
                                                                                      // преходи - всички преходи на двата автомата,
                                                                                      // състояния - всички състояния на двата автомата,
                                                                                      // крайни състояния - всички крайни на двата автомата
                                                                                      // начално състояние - newFirst
}
// Конкатенация
FiniteAutomata FiniteAutomata::concat(const FiniteAutomata& other) const
{
    // ламбда функция за добавяне на стринг в края на името на състоянията (приема като аргумент състояния)
    auto addStringToItem = [](const unordered_set<State>& from, const string& str){
        unordered_set<State> result;

        for(auto state: from)
        {
            string resStr = state + str;
            result.emplace(resStr);
        }
        return result;
    };
    // ламбда функция за добавяне на стринг в края на името на състоянията (приема като аргумент двойки от състояния и преходи)
    auto transformTransitions = [](const unordered_map<StateTransitionPair, vector<State>>& m, const string& str) {
        unordered_map<StateTransitionPair, vector<State>> newMap;

        for (auto mapPair: m)
        {
            StateTransitionPair pair = mapPair.first;
            pair.state += str;
            vector<State> newDestinations;
            for(auto destState: mapPair.second)
            {
                newDestinations.push_back(destState + str);
            }
                newMap[pair] = newDestinations;
        }
            return newMap;
        };
    // правим копие на първия автомат, като променяме имената на състоянията му (добавяме "1" в края)
    unordered_map<StateTransitionPair, vector<State>> firstTransitions = transformTransitions(transitions, "1");
    unordered_set<State> firstStates = addStringToItem(states, "1"); // std знае как да хешира стринг
    unordered_set<State> firstFinalStates = addStringToItem(finalStates, "1");
    State firstFirst = this->first + "1";
    // правим копие на втория автомат, като променяме имената на състоянията му (добавяме "2" в края)
    unordered_map<StateTransitionPair, vector<State>> secondTransitions = transformTransitions(other.transitions, "2");
    unordered_set<State> secondStates = addStringToItem(other.states, "2"); // std знае как да хешира стринг
    unordered_set<State> secondFinalStates = addStringToItem(other.finalStates, "2");
    State secondFirst = other.first + "2";

    // за обединяване на двата автомата в един
    firstTransitions.merge(secondTransitions); // обединяваме преходите на копията на първия и втория автома
    firstStates.merge(secondStates); // обединяваме състояния на копията на първия и втория автомат
    // от всяко финално състояние на първия автомат правим епсилон-преход към началното състояние на втория автомат
    for(auto finstate: firstFinalStates)
    {
        firstTransitions[{finstate, ""}].push_back(secondFirst);  // Добавяме епсилон-преход от финалните състояния към началното на копието на втория автомат
    }
    return FiniteAutomata(firstTransitions, firstStates, secondFinalStates, firstFirst); // връща автомат с
                                                                                      // преходи - всички преходи на двата автомата,
                                                                                      // състояния - всички състояния на двата автомата,
                                                                                      // крайни състояния - крайните състояния на копието на втория автомат
                                                                                      // начално състояние - началното състояние на коието на първия автомат  
}

FiniteAutomata FiniteAutomata::Kleene()
{
    // от всяко финално състояние правим епсилон преход към началното
    for(auto finstate: finalStates)
    {
        transitions[{finstate, ""}] = {first};   
    }
    // ако началното състояние не е крайно, го правим крайно
    if (!isFinalState(first)) 
    {
        finalStates.emplace(first);
    }
    return FiniteAutomata(transitions, states, finalStates, first);
}
   
void FiniteAutomata::print() // Информация за автомата
{
    cout<<"First state: "<<first<<endl;
    cout<<"States:";
    for (const State& state : states)
        {
                cout<<" "<<state;
        }
    cout<<endl;
    cout<<"Final states:";
    for (const State& finstate : finalStates)
        {
            cout<<" "<<finstate;
        }
    cout<<endl;
    cout<<"Transitions:"<<endl;
    for (auto transition : transitions)
        {
            cout<<"{"<<transition.first.state<<", "<<transition.first.transition<<"}->"<<transition.second<<endl; // {"", tr} -> st
        }
}
// Преобразуване на регулярен израз до автомат
FiniteAutomata FiniteAutomata::exprToAutomata(const string& expr)
{
    string RegularExpr = rpn(expr); // преобразуваме регулярния израз expr в полски запис RegularExpr 
    vector<FiniteAutomata> st; // вектор от обекти от класа FiniteAutomata
    if (RegularExpr.empty()) // ако регулярният израз е празната дума, създаваме автомат, разпознаващ празната дума
    {
        FiniteAutomata emptyAutomata;
        emptyAutomata.fromWord("");
        return emptyAutomata;
    } 
    char sym = RegularExpr[0]; // първи елемент от регулярния израз
    if (isOperator(sym)) // регулярният израз не е коректен, ако започна с оператор
    {
        cout<<"The regular expression is not correct"<<endl;
        FiniteAutomata emptyAutomata;
        emptyAutomata.fromWord("");
        return emptyAutomata;
    }

    while (!RegularExpr.empty())
    {
        sym = RegularExpr[0]; // първи символ от регулярния израз (моментен символ)
        switch (sym)
        {
            case '*': // ако моментният символ е звезда на Клини
            { 
                // Премахваме последния елемент(автомат) на вектора,
                // добавяме автомат във вектора с език (езика на стария автомат)*
                FiniteAutomata autKl; 
                autKl = st.back();
                st.pop_back();
                autKl = autKl.Kleene(); 
                st.push_back(autKl); 
                RegularExpr.erase(0,1); // премахваме първия символ на регулярния израз
                break; 
            }
            case '.': // ако моментният символ е конкатенация
            { 
                // Премахваме последните два елемента(автомата) на вектора,
                // добавяме нов автомат във вектора, който е конкатенацията на двата премахнати
                FiniteAutomata aut1, aut2, newAut;
                aut2 = st.back();
                st.pop_back();
                aut1 = st.back();
                st.pop_back(); 
                newAut = aut1.concat(aut2);
                st.push_back(newAut);
                RegularExpr.erase(0,1); // премахваме първия елемент от регулярния израз
                break; 
            }
            case '&': { break; } // сечение
            case '+':  // ако моментният символ е обединение
            {
                // Премахваме последните два елемента(автомата) на вектора,
                // добавяме нов автомат във вектора, който е обединението на двата премахнати
                FiniteAutomata aut1, aut2, newAut;
                aut2 = st.back();
                st.pop_back();
                aut1 = st.back();
                st.pop_back(); 
                newAut = aut1 + aut2;
                st.push_back(newAut);
                RegularExpr.erase(0,1);
                break;
            } 
            default : // иначе добавяме във вектора автомат с език моментния символ
            {
                FiniteAutomata autSym;
                autSym.fromWord(string(1,sym));
                st.push_back(autSym);
                RegularExpr.erase(0,1); // премахваме първия елемент на регулярния израз
                break;
            }
        } 
    }
    return st.back();
}
