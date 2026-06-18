#ifndef TWENTYQUESTIONS_H
#define TWENTYQUESTIONS_H

#include <queue>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
//tree constructor
struct Node{
  std::string data;
  Node* no = nullptr;
  Node* yes = nullptr;
  bool type = true; //useless
  //default constructor
  Node(const std::string q = "Is it bigger than a breadbox?") : data(q), yes(nullptr), no(nullptr) {}

  void insert_left(std::string test);
  void insert_right(std::string test);
  int has_children();

  //deconstructor
  ~Node(){
    //delete left child
    if(no) delete no;
    //delete right child
    if(yes) delete yes;
  }
};

struct Tree{
  Node* root = nullptr;
  Tree(std::string r = "Is it bigger than a breadbox?") : root(new Node{r}) {}

  //deconstructor
  ~Tree(){
    delete root;
  }
};

//ready commands from argv
int file_type(std::string); //0 = .tree, 1 = .txt

int readCommands(int, char**, Tree&, std::string&);

int load_tree(std::string, Node*);

int save_tree(std::string, Node*);

int tree_from_file(std::fstream&, Node*);

int load_tree_txt_entry(std::fstream&, Node*);

int tree_from_file_txt(std::fstream&, Node*);

int file_from_tree(std::fstream&, Node*);

int file_from_tree_txt(std::fstream&, Node*);

void default_tree(Tree&);

bool readInput();

void play(Tree&, std::string&);


#endif
