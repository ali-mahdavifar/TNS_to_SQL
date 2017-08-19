#include "shared.h"

void ui_infinite_loop() {

    while(1) {
        set_cyan();
        cout << "Enter Your TNS Query: ";

        set_white();
        cout << "('exit' for exit)\n" << endl;

        set_yellow();
        string tns_query = get_tns_query();
        if(tns_query == "exit") {
            break;
        }

        string result = sql_result(tns_query);
        set_output_color(result);
        prune_result(result);

        cout << "\n" << result << "\n\n" << endl;
    }
}


string get_tns_query() {

    string res;
    res += Data::left_out_input;
    Data::left_out_input = "";

    while(1) {
        string temp;
        getline(cin, temp);

        if((temp == "exit") || (temp == "exit;")) {
            return temp;
        }
        res += " ";

        bool reached_semicolon = false;
        for(int i = 0; i < temp.size(); i++) {
            res += temp[i];
            if(temp[i] == ';') {
                reached_semicolon = true;
                for(int j = i + 1; j < temp.size(); j++) {
                    Data::left_out_input += temp[j];
                }
                break;
            }
        }
        if(reached_semicolon) {
            break;
        }
    }
    return res;
}



void set_output_color(string result) {

    if(string_contains(result, "Error")) {
        set_red();
        return;
    }
    if(string_contains(result, "Success")) {
        set_green();
        return;
    }
    if(result == "") {
        return;
    }
    set_blue();
}


void prune_result(string& result) {

    int pos = result.find("Error");
    if(pos == string::npos) {
        return;
    }
    if(!string_contains(result,"{")) {
        return;
    }

    string err = extract_error(result);
//    string err = result;

    result = err;
}


string extract_error(string input) {
    // input is like :
    //      ...Error:{...}....Error:{...}...
    vector<int> occurrences;
    int pos = 0;
    while(true) {
        int temp = input.find("Error",pos);
        if(temp == string::npos) {
            break;
        }
        occurrences.push_back(temp);
        pos = temp + 1;
    }

    vector<string> errors;
    for(auto occur : occurrences) {
        string err_temp;
        err_temp += "Error >> ";
        int first = occur;
        int last  = input.size() - 1;
        int stack_counter = 0;
        bool started = false;
        for(int i = occur; i < input.size(); i++) {
            if(input[i] == '{') {
                if(!started) {
                    first = i;
                }
                started = true;
                stack_counter++;
            }
            if(input[i] == '}') {
                stack_counter--;
                if(stack_counter == 0) {
                    last = i;
                    break;
                }
            }
        }
        err_temp += ' ';
        for(int i = first + 1; i < last; i++) {
            err_temp += input[i];
        }
        err_temp += "\n";
        errors.push_back(err_temp);
    }

    sort(errors.begin(), errors.end());
    auto ut = unique(errors.begin(), errors.end());
    errors.resize(ut - errors.begin());

    string res;
    for(auto error : errors) {
        res += error;
    }

    return res;
}


void print_existing_metadata() {

    // print apply_buffer
    cout << "apply_buffer:" << endl;
    for(auto elem : Data::apply_buffer) {
        cout << elem << endl;
    }
    cout << endl;

    // print from_tables_names
    cout << "from_table_names:" << endl;
    for(auto elem : Data::from_table_names) {
        cout << elem.first << " -> " << elem.second << endl;
    }
    cout << endl;

    // print tables
    cout << "tables:" << endl;
    for(auto elem : Data::tables) {
        cout << elem << endl;
    }
    cout << endl;

    // print table_relations
    cout << "table_relations:" << endl;
    for(auto elem : Data::table_relations) {
        cout << "(" << elem.first.first << ", " + elem.first.second << ")"
                    << " -> " << elem.second << endl;
    }
    cout << endl;

    // print reference_isMultiple
    cout << "reference_isMultiple:" << endl;
    for(auto elem : Data::reference_isMultiple) {
        cout << "(" << elem.first.first << ", " + elem.first.second << ")" << " -> ";
        if(elem.second) {
            cout << "true";
        }
        else {
            cout << "false";
        }
        cout << endl;
    }
    cout << "\n\n" << endl;
}




