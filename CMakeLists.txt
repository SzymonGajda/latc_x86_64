cmake_minimum_required(VERSION 3.15)
project(latc_x86_64)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES Test.C BaseVisitor.H BaseVisitor.C Printer.H Printer.C Parser.H Parser.C Lexer.C Absyn.H Absyn.C TopDefAnalyser.C TopDefAnalyser.H FunctionHeader.cpp FunctionHeader.h Error.cpp Error.h SemanticAnalyser.cpp SemanticAnalyser.h SymbolsTable.cpp SymbolsTable.h ThreeAddressCodeConverter.cpp ThreeAddressCodeConverter.h TACPrinter.cpp TACPrinter.h BasicBlock.cpp BasicBlock.h ControlFlowGraph.cpp ControlFlowGraph.h BasicBlockConverter.cpp BasicBlockConverter.h LivenessAnalyser.cpp LivenessAnalyser.h Allocator.cpp Allocator.h CodeGenerator.cpp CodeGenerator.h)
add_executable(latc_x86_64 ${SOURCE_FILES})