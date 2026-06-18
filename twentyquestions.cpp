#include "twentyquestions.h"

//command line arguments
int readCommands(int argc, char** argv, Tree& tree, std::string& file_name)
{
  bool found_file = false;

  //read args
  for(int i = 0; i < argc; i++)
  {
    //-h help
    if(strcmp(argv[i], "-h") == 0)
    {
      std::cout << "Options:" << "\n\t-h Show this text." << "\n\t-f Load a game tree from a file\n";
      return -1;
    }
    //-f Load game from a file
    else if(strcmp(argv[i], "-f") == 0)
    {
      if( i + 1 < argc)
      {
        //checks to see if it's a valid file
        std::fstream file;
        bool new_file = false;
        file.open(argv[i+1]);
        if (!file.is_open())
        {
          std::ofstream create_file{ argv[i+1] };
          new_file = true;
        }
        found_file = true;
        file_name = argv[i+1];
        file.close();

        //read existing file into tree
        if(!new_file){
          if(load_tree(argv[i+1], tree.root) == 0){    
              std::cout << "Successfully loaded file: " << file_name << '\n';
          }
          else{
            std::cout << "Could not load: " << file_name << ", using game.tree instead.\n";
            found_file = false;
          }
        }
      }
      else{
        std::cerr << "Error: -f needs a file name.\n";
        return -1;
      }
    }
  }

  //default file name
  if(!found_file)
  {
    file_name = "game.tree";
    std::fstream check_file{ file_name };

    if(!check_file.is_open()){
      std::ofstream create_file{ file_name };
      create_file.close();
      save_tree(file_name,tree.root);
    }
    else{
      if(load_tree(file_name, tree.root) == 0){
        std::cout << "Successfully loaded file: " << file_name << '\n';
      }
      else{
        std::cout << "Failed to load file: " << file_name << '\n';
      }
    }
  }
  return 0;
}

int tree_from_file(std::fstream& file_stream, Node* current_node){
  //initialize
  char character;
  std::string data;

  //read string
  file_stream.read(&character,1);
  while(character != '\0'){
    data.push_back(character);
    file_stream.read(&character, 1);
  }
  current_node->data = data;

  //read node type
  file_stream.read(&character, 1);
  current_node->type = (character ? true : false);

  //read number of children
  file_stream.read(&character, 1);

  //if left child exists
  if(character & 1){
    current_node->no = new Node{};
    tree_from_file(file_stream, current_node->no);
  }

  //if right child exists
  if(character & 2) {
    current_node->yes = new Node{};
    tree_from_file(file_stream, current_node->yes);
  }
  return 0;
}

int file_type(std::string str){
  if(str.find(".tree") != std::string::npos){
    return 0;
  }
  else if(str.find(".txt") != std::string::npos){
    return 1;
  }
  return -1;
}

int load_tree_txt_entry(std::fstream& file_stream, Node* current_node){
  //initialize
  char character, temp;
  std::string data;

  current_node->type = true;
  

  //read question marker
  file_stream.read(&character, 1);
  while(character != 'Q'){
      file_stream.read(&character, 1);
  }
  file_stream.read(&temp, 1);

  return ((character == 'Q') ? tree_from_file_txt(file_stream, current_node) : -1);
}

int load_tree(std::string file_name, Node* current_node){
  std::fstream file {file_name};

  if(file.is_open()){
    switch ( file_type ( file_name ) ){
      case 0:
        return tree_from_file(file, current_node);
      case 1:
        return load_tree_txt_entry(file, current_node);
      default:
        std::cout << "Invalid file extension.\n";
        return -1;
    }
  }
  return -1;
}

int save_tree(std::string file_name, Node* current_node){
  std::fstream file{file_name};

  if(file.is_open()){
    switch (file_type(file_name)) {
      case 0:
        return file_from_tree(file, current_node);
      case 1:
        return file_from_tree_txt(file, current_node);
      default:
        std::cout << "Invalid file extension.\n";
        return -1;
    }
  }
  return -1;
}

int tree_from_file_txt(std::fstream& file_stream, Node* current_node){
  //initialize
  char character, temp;
  std::string data;

  current_node->type = true;

  //read question
  file_stream.read(&character, 1);
  while(character != '\n'){
    data.push_back(character);
    file_stream.read(&character, 1);
  }

  //write question to node
  current_node->data = data;

  //left node
  file_stream.read(&character, 1);
  if(file_stream.eof()) return 0;
  while(character != 'A' && character != 'Q'){
    file_stream.read(&character, 1);
  }
  file_stream.read(&temp, 1);

  data.clear();

  if(character == 'A'){
    //read answer
    file_stream.read(&character, 1);
    while(character != '\n'){
      data.push_back(character);
      file_stream.read(&character, 1);
    }

    current_node->yes = new Node{};
    current_node->yes->type = false;
    current_node->yes->data = data;
  }
  else if(character == 'Q'){
    current_node->yes = new Node{};
    tree_from_file_txt(file_stream, current_node->yes);
  }
  else{
    return -1;
  }

  //right node
  file_stream.read(&character, 1);
  if(file_stream.eof()) return 0;
  while(character != 'A' && character != 'Q'){
    file_stream.read(&character, 1);
  }
  file_stream.read(&temp, 1);
  
  data.clear();

  if(character == 'A'){
    //read answer
    file_stream.read(&character, 1);
    while(character != '\n'){
      data.push_back(character);
      file_stream.read(&character, 1);
    }
    current_node->no = new Node{};
    current_node->no->type = false;
    current_node->no->data = data;
  }
  else if(character == 'Q'){
    current_node->no = new Node{};
    tree_from_file_txt(file_stream, current_node->no);
  }
  else{
    return -1;
  }
  return 0;
}

int file_from_tree_txt(std::fstream& file, Node* tree){
  if(tree){
    file << ((tree->type) ? "Q:" : "A:");
    file << tree->data << '\n';

    if(tree->type == true){
      file_from_tree_txt(file, tree->yes);
      file_from_tree_txt(file, tree->no);
    }
  }
  return 0;
}

int file_from_tree(std::fstream& file, Node* tree){
  //recursively call file from tree for each yes and no branch
  int num_of_children = 0;
  if(tree->no) num_of_children |= 1;
  if(tree->yes) num_of_children |= 2;

  //write string
  file.write(tree->data.c_str(), strlen(tree->data.c_str()) + 1);

  //write node type
  file << (char)tree->type;

  //write number of children
  file << num_of_children;

  //recursively call file from tree for each yes and no branch
  if (tree->no){
    file_from_tree(file, tree->no);
  }
  if (tree->yes){
    file_from_tree(file,tree->yes);
  }

  return 0;
}

void default_tree(Tree& root){
  root.root = new Node{ "Is it bigger than a breadbox?" };
  root.root->type = true;
}

bool readInput() {
  while(1){
  std::string input_string;
  std::getline(std::cin, input_string);

  if(strcmp(input_string.c_str(), "yes") == 0) return true;
  if(strcmp(input_string.c_str(), "no") == 0) return false;

  std::cout << "Invalid option\n";
  }
}

Node* build_tree_file(std::istream& input){
  std::string value;
  //check if a node is null
  if(!input) return nullptr;

  //build tree
  Node* node = new Node(value);
  node->no = build_tree_file(input);
  node->yes = build_tree_file(input);
  return node;
}

//play 20 questions
void play(Tree& defaultTree, std::string& file_name)
{
  //game reset loop
  while(1)
  {
    //tree positions
    Node* position = defaultTree.root;
    Node* last_position = defaultTree.root;
    bool last_answer = true;
    //game loop
    while(1)
    {
      //transverse through the binary tree
      //continue asking questions until the computer reaches a leaf node
      if(position && position->type)
      {
        //asks a question
        std::cout << position->data << '\n';
        last_answer = readInput();
        if(last_answer){
          last_position = position;
          position = position->yes;
        }else{
          last_position = position;
          position = position->no;
        }
      }
      else
      { //answer type or nullptr node reached
        if(position) // answer
        {
          //test all answers
          std::cout << "Is it " << position->data << "?\n";
          //check is position is not a nullptr
          if(readInput()){ 
            break;
          }
        }

        //insert new node
        std::string question;
        std::string object;
        bool answer;
        
        //asks what was it
        std::cout << "OK, what was it?\n";
        std::getline(std::cin, object);
        
        //if answer node
        if(position){

          //distinguish the question
          std::cout << "Give me a question that would distinguish a " << position->data << "and " << object;
          std::getline(std::cin, question);

          //asks if the question would be a yes or no
          std::cout << "And would the answer to the question for a " << object << " be a yes or no? \n";
          answer = readInput();
        }
        //construct child node
        if(position){

          Node* old_answer = new Node{ position->data };
          old_answer->type = false;

          position->data = question;
          position->type = true;

          if(answer){
            position->yes = new Node{ object };
            position->yes->type = false;
            position->no = old_answer;
          }
          else{
           position->no = new Node { object };
           position->no->type = false;
           position->yes = old_answer;
          }
        }
        //construct new node
        else{
          if(last_answer){
            last_position->yes = new Node{object};
            last_position->yes->type = false;
          }else{
            last_position->no = new Node{object};
            last_position->no->type = false; 
          }
      }
      break;
      }
    }
      //ask if they wanna save the game
      std::cout << "Do you want to save the game?\n";
      if(readInput())
      {
        std::fstream file;
        file.open(file_name);
        if(file.is_open())
        {
          //write all the binary tree data to a file;
          file_from_tree(file, defaultTree.root);
          std::cout << "Game saved. \nGoodbye!";
        }
        else{
          std::cout << "Failed to write to file: " << file_name << '\n';
        }
      }
      std::cout << "Do you want to play again?\n";
      if(!readInput())
      {
        return;
      }
  }
}
