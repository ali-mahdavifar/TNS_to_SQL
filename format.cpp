#include <iostream>
#include <string>
#include <vector>
using namespace std;


string spaces(int n){
    string res = "";
    for(int i=0;i<n;i++){
        res += " ";
    }
    return res;
}


void set_red(){
    cout<<"\033[1;31m";
}

void set_cyan(){
    cout<<"\033[1;36m";
}

void set_green(){
    cout<<"\033[1;32m";
}

void set_yellow(){
    cout<<"\033[1;33m";
}

void set_blue(){
    cout<<"\033[1;34m";
}

void set_magneta(){
    cout<<"\033[1;35m";
}

void set_white(){
    cout<<"\033[0m";
}

bool string_contains(string input, string sub_input) {
    bool res = false;
    if (input.find(sub_input) != string::npos) {
        res = true;
    }
    return res;
}

string drep_vector_string(vector<string> input) {
    string res;
    res += "{ ";
    for(auto elem : input) {
        res += '"';
        res += elem;
        res += '"';
        res += ", ";
    }
    res.pop_back();
    res.pop_back();
    res += " }";
    return res;
}







