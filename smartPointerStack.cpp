#include <iostream>
#include <memory>
#include <string>

class Node{
public:
    Node() = default;
    Node(std::string param): val(param){};

    std::string val;
    std::shared_ptr<Node> p_prev_node;
};

class Stack{
public:
    Stack():back(nullptr), count(0){};

    void push(std::string value);
    void pop();

    bool empty() { return back == nullptr; }
    size_t size() const { return count; }
    std::string& top() { return back->val; }

private:
    size_t count;
    std::shared_ptr<Node> back;
};

void Stack::push(std::string value){
    auto new_element = std::make_shared<Node> (Node(value));
    new_element->p_prev_node = back;
    back = new_element;
    
    count++;
}
void Stack::pop(){
    if(!empty()){
        auto temp = back->p_prev_node;
        back.reset();
        back = temp;
        count--;
    }
}
