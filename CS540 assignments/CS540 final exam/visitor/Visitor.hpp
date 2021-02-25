class Visitor;
class IntNode;
class TextNode;
class Node {
public:
    Node(Node *l, Node *r) : m_left_child(l), m_right_child(r) {}
    virtual void accept(const Visitor* v) const{
        recurse(m_left_child, v);
        recurse(m_right_child, v);
    }
private:
    const Node *m_left_child; const Node *m_right_child;
    static void recurse(const Node *n, const Visitor* v) { if (n != nullptr) n->accept(v);}
};
class Visitor {
public:
    virtual void visit(const IntNode &) const = 0;
    virtual void visit(const TextNode &) const = 0;
};

class IntNode : public Node {
public:
    IntNode(const int i, Node* n1, Node* n2) : m_int(i), m_left_child(n1), m_right_child(n2), Node(n1, n2) {}
    int integer() const { return m_int; }
    void accept(const Visitor* v) const override{
        recurse(m_left_child, v);
        recurse(m_right_child, v);
        v->visit(*this);
    }
private:
    const int m_int; const Node* m_left_child; const Node* m_right_child;
    static void recurse(const Node* n,const Visitor* v){ if(n!=nullptr) n->accept(v); }
};
class TextNode : public Node {
public:
    TextNode(const char *s, Node* n1, Node* n2) : m_str(s),m_left_child(n1), m_right_child(n2), Node(n1, n2){}
    const std::string &str() const { return m_str; }
    void accept(const Visitor* v) const override{
        recurse(m_left_child,v);
        recurse(m_right_child, v);
        v->visit(*this);
    }
private:
    const std::string m_str; const Node* m_left_child; const Node* m_right_child;
    static void recurse(const Node* n,const Visitor* v){ if(n!=nullptr) n->accept(v); }
};
class MyVisitor1 : public Visitor {
public:
    void visit(const IntNode &n) const override{std::cout << "MyVisitor1: " << n.integer() << std::endl;}
    void visit(const TextNode &n) const override{std::cout << "MyVisitor1: " << n.str() << std::endl;}
};

class MyVisitor2 : public Visitor {
public:
    void visit(const IntNode &n) const override{std::cout << "MyVisitor2: " << n.integer() << std::endl;}
    void visit(const TextNode &n) const override{std::cout << "MyVisitor2: " << n.str() << std::endl;}
};

