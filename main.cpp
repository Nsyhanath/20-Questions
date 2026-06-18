#include "twentyquestions.h"

int main(int argc, char** argv) {

  Tree tree;
  std::string file_name;

  if(readCommands(argc, argv, tree, file_name) == -1) //read commands and return if necessary
  {
    return 1;
  }

  play(tree, file_name);
 
  return 0;
}
