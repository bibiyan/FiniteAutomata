#ifndef _FINITEAUTOMATA_H_
#define _FINITEAUTOMATA_H_

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

using namespace std;

using State = string; 
using Transition = string; 

// Структура за двойка състояние и преход
struct StateTransitionPair
{
    State state;
    Transition transition;

    bool operator==(const StateTransitionPair& pair) const{
        return (state == pair.state && transition == pair.transition);
    }
};

// хеширащ функтор за unordered_map
// направена с помощ
namespace std
{
    template <> 
    struct hash<StateTransitionPair>
    {
        size_t operator()(const StateTransitionPair& pair) const
        {
            size_t h1 = hash<State>()(pair.state);
            size_t h2 = hash<Transition>()(pair.transition);

            return h1 ^ h2;
        }
    };
}

class FiniteAutomata {
    public:
    FiniteAutomata() = default; // Конструктор по подразбиране
    FiniteAutomata(const FiniteAutomata&) = default; // Копи-конструктор

    FiniteAutomata( const unordered_map<StateTransitionPair, vector<State>>& transitions,
                    const unordered_set<State>& states,
                    const unordered_set<State>& finalStates,
                    const State& first)
        : transitions(transitions), states(states), finalStates(finalStates), first(first){} // Конструктор с параметри
    FiniteAutomata &operator= (FiniteAutomata const& other) = default; // Оператор "="  
    
    void fromWord(const string& word); // Създава автомат, който чете дадената дума
   
    void emptyWord(); // Празната дума
    FiniteAutomata emptyWord2();

    static bool isFromAlphabet(char c); // Малки латински символи

    void wildcat(); // Произволен символ
    FiniteAutomata wildcat2();

    bool isFinalState(const State& st); // Проверка дали дадено състояние е крайно

    bool isMatch(const string& word); // Проверка дали дума е от езика на автомата

    FiniteAutomata operator+ (const FiniteAutomata& other) const; // Обединение

    FiniteAutomata concat(const FiniteAutomata& other) const; // Конкатенация
   
    FiniteAutomata Kleene(); // Звезда на Клини
   
    void print(); // Информация за автомата

    static FiniteAutomata exprToAutomata(const string& expr); // Създава автомат по даден регулярен израз
    
    private:
    unordered_map<StateTransitionPair, vector<State>> transitions; // Преходи
    unordered_set<State> states; // Всички състояния
    unordered_set<State> finalStates; // Крайни състояния
    State first; // Начално състояние

    void clear() // Унищожаване на автомат
    {
        transitions.clear();
        states.clear();
        finalStates.clear();
        first = "";
    }
};

#endif
