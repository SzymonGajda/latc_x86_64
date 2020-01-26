#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Parser.H"
#include "Absyn.H"
#include "TopDefAnalyser.H"
#include "Error.h"
#include "SemanticAnalyser.h"
#include "ThreeAddressCodeConverter.h"
#include "TACPrinter.h"
#include "BasicBlockConverter.h"
#include "CodeGenerator.h"

String getPath(const std::string &str) {
    std::size_t found = str.find_last_of('/');
    return str.substr(0, found);
}

String getFilename(const std::string &str) {
    std::size_t found = str.find_last_of('/');
    std::size_t dot = str.find_last_of('.');
    return str.substr(found + 1, dot - found - 1);
}

int main(int argc, char **argv) {
    FILE *input;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "ERROR\nError opening input file.\n");
            exit(1);
        }
    } else input = stdin;
    /* The default entry point is used. For other options see Parser.H */
    Program *parse_tree = pProgram(input);
    if (parse_tree) {
        auto *functionHeaders = new FunctionHeaders;
        TopDefAnalyser topDefAnalyser;
        topDefAnalyser.functionHeaders = functionHeaders;
        parse_tree->accept(&topDefAnalyser);
        if (topDefAnalyser.error.isError) {
            std::cerr << "ERROR\n";
            std::cerr << topDefAnalyser.error.getErrorMessage();
            return 1;
        }
        SemanticAnalyser semanticAnalyser;
        auto *error = new Error;
        auto *symbolsTable = new SymbolsTable;
        semanticAnalyser.functionHeaders = functionHeaders;
        semanticAnalyser.symbolsTable = symbolsTable;
        semanticAnalyser.error = error;
        parse_tree->accept(&semanticAnalyser);
        delete symbolsTable;
        if (semanticAnalyser.error->isError) {
            std::cerr << "ERROR\n";
            std::cerr << semanticAnalyser.error->getErrorMessage();
            delete error;
            return 1;
        }
        delete error;
        auto *threeAddressCodeConverter = new ThreeAddressCodeConverter;
        threeAddressCodeConverter->functionHeaders = functionHeaders;
        symbolsTable = new SymbolsTable;
        threeAddressCodeConverter->symbolsTable = symbolsTable;
        parse_tree->accept(threeAddressCodeConverter);

        auto *basicBlockConverter = new BasicBlockConverter;
        basicBlockConverter->controlFlowGraph = new ControlFlowGraph;
        basicBlockConverter->stringValues = threeAddressCodeConverter->stringValues;
        basicBlockConverter->basicBlock = new BasicBlock;
        basicBlockConverter->tacPrinter = new TACPrinter;
        basicBlockConverter->functionHeaders = functionHeaders;
        basicBlockConverter->symbolsTable = symbolsTable;
        parse_tree->accept(basicBlockConverter);

        CodeGenerator codeGenerator;
        codeGenerator.controlFlowGraph = basicBlockConverter->controlFlowGraph;
        codeGenerator.stringValues = basicBlockConverter->stringValues;
        codeGenerator.functionHeaders = functionHeaders;
        codeGenerator.allocator = new Allocator;
        codeGenerator.allocator->symbolsTable = symbolsTable;
        codeGenerator.generateCode();

        String path = getPath(argv[1]);
        String filename = getFilename(argv[1]);

        std::ofstream latcFile;
        latcFile.open(path + '/' + filename + ".s");
        latcFile << codeGenerator.resultCode;
        latcFile.close();

        std::cerr << "OK\n";
        String command1 = "gcc -c " + path + '/' + filename + ".s " + "-o " + path + '/' + filename + ".o";
        String command2 = "gcc -c ./lib/runtime.c -o ./lib/runtime.o";
        String command3 = "gcc " + path + '/' + filename + ".o " + " ./lib/runtime.o -o " + path + '/' + filename;
        String command4 = "rm -f " + path + '/' + filename + ".o";
        system(command1.c_str());
        system(command2.c_str());
        system(command3.c_str());
        system(command4.c_str());

        basicBlockConverter->controlFlowGraph->deleteCFG();
        delete codeGenerator.allocator;
        delete symbolsTable;
        delete threeAddressCodeConverter;
        delete functionHeaders;
        delete basicBlockConverter->controlFlowGraph;
        delete basicBlockConverter->tacPrinter;
        delete basicBlockConverter;
        return 0;
    }
    return 1;
}

