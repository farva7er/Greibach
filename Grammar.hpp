#ifndef GRAMMAR
#define GRAMMAR

const char EPS = '$';

struct Symbol {
   bool isTerminal;
   char chr;
   Symbol *next;

   Symbol(bool isTerminal, char chr, Symbol *next)
   : isTerminal(isTerminal), chr(chr), next(next) {}
   ~Symbol() { delete next; }
};

struct node;   

class Production {
   friend class Grammar; 
   Symbol *head;
   Symbol *tail;

   void addToTail(Symbol *s);
   void addToTail(Production *p);
   void addToHead(Symbol *s);
   void addToHead(const Symbol *start, const Symbol *end); 
   bool isEps(const bool *eps, const int *pos) const;

public:
   Production();
   Production(const char *str, const bool *term);
   ~Production() { delete head; }

   Production(const Production &other);


   static node* substEps(node **out, const bool *eps,
                  const Symbol *s, int *rec, const int *nt_pos);
   
   bool isFirstTerminal() const { return head->isTerminal; }
   bool isLeftRec(const int *nt_pos, int i) const 
   { return !isFirstTerminal() && nt_pos[(int) head->chr] == i; }
   void removeFirst();
   bool cmp(const Production *p) const;
   void print() const;
};


struct node {
   Production *p;
   node* next;
   node(node *next, Production *p) : p(p), next(next) {}
   ~node() { delete p; delete next; } 
};

class Grammar {
   
   bool epsProd; 
   node **nt_arr;
   bool *terminals;
   int *nt_pos;
   char *nt_chr;
   int nt_size;
   int nt_aux_size;

   void clean();
   void removeProd(node **prev);
   void removeLRec(int i);
   void removeIRec();
   void invSubst();
   int addNT();
public:
   Grammar(bool *terminals, int nt_size, int *nt_pos, char *nt_chr);
   ~Grammar();

   void eliminateEps();
   void addProd(const char* str, int i);
   void addProd(node *start, node *end, int i);
   void addProd(Production *p, int i);
   void Greibach(bool p);
   void print() const;
};



#endif
