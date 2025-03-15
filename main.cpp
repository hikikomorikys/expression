#include <iostream>
#include "expression.hpp"

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cerr << "Usage:\n";
        std::cerr << "  " << argv[0] << " --eval \"expression\" var=value ...\n";
        std::cerr << "  " << argv[0] << " --diff \"expression\" --by var\n";
        return 1;
    }
    std::string command = argv[1];
    if (command == "--eval" && argc >= 3){
        Expression<double>::Ptr expr = Expression<double>::fromString(argv[2]);
        std::map<std::string, double> values;
        for (int i = 3; i < argc; ++i){
            std::string arg(argv[i]);
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos){
                std::string var = arg.substr(0, eq_pos);
                double value = std::stod(arg.substr(eq_pos + 1));
                values[var] = value;
            }
        }
        std::cout << expr->evaluate(values) << std::endl;
    }
    else if (command == "--diff" && argc == 5 && std::string(argv[3]) == "--by"){
        Expression<double>::Ptr expr = Expression<double>::fromString(argv[2]);
        std::string var = argv[4];
        Expression<double>::Ptr derivative = expr->differentiate(var);
        std::cout << derivative->toString() << std::endl;
    }
    else{
        std::cerr << "Invalid command format." << std::endl;
        return 1;
    }
    return 0;
}
