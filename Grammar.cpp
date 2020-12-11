#include "Grammar.hpp"
#include <stdio.h>
#include <string.h>

Grammar::Grammar(bool *terminals, int nt_size, int *nt_pos, char *nt_chr)
: epsProd(false), terminals(terminals), nt_pos(nt_pos),
   nt_chr(nt_chr), nt_size(nt_size),
   nt_aux_size(0)

{
   nt_arr = new node*[nt_size];
   memset(nt_arr, 0, nt_size * sizeof(node*));
}

Grammar::~Grammar()
{
   for(int i = 0; i < nt_size + nt_aux_size; i++)
      delete nt_arr[i];
   delete [] nt_arr;
   delete [] terminals;
   delete [] nt_pos;
   delete [] nt_chr; 
}

void Grammar::addProd(const char* str, int i)
{
   Production *p = new Production(str, terminals);
   node *newNode = new node(nt_arr[i], p);
   nt_arr[i] = newNode;
}

void Grammar::addProd(Production *p, int i)
{
   node *n = new node(0, p);
   addProd(n, 0, i); 
}

int Grammar::addNT()
{
   node **arr = new node*[nt_size + nt_aux_size + 1];
   for(int i = 0; i < nt_size + nt_aux_size; i++)
      arr[i] = nt_arr[i];
   arr[nt_size + nt_aux_size] = 0;
   delete [] nt_arr;
   nt_arr = arr;
   nt_aux_size++;
   return nt_size + nt_aux_size - 1;
}

void Grammar::removeLRec(int i)
{
   bool f_lrec = false;
   
   node *current = nt_arr[i];
   while(current) {
      if(current->p->isLeftRec(nt_pos, i)) {
         f_lrec = true;
         break;
      }
      current = current->next;
   }
   if(!f_lrec)
      return;
   int j = addNT();
   node **curr = &nt_arr[i];
   while(*curr) {
      Production *p = (*curr)->p;
      if(p->isLeftRec(nt_pos, i)) {
         p->removeFirst();
         node *save = *curr;
         *curr = (*curr)->next;
         addProd(save, save->next, j);
         Production *recProd = new Production(*p);
         recProd->addToTail(new Symbol(true, 'X' + nt_aux_size - 1, 0));
         addProd(recProd, j);
         continue;
      }

      Production *recProd = new Production(*p);
      recProd->addToTail(new Symbol(true, 'X' + nt_aux_size - 1, 0));
      curr = &((*curr)->next);
      addProd(recProd, i);
   }
}

void Grammar::removeIRec()
{
   for(int i = 0; i < nt_size; i++) {
      node **curr = &nt_arr[i];
      while(*curr) {
         Production *p = (*curr)->p;
         if(!p->isFirstTerminal() && nt_pos[(int)p->head->chr] < i) {
            int j = nt_pos[(int)p->head->chr];
            
            p->removeFirst();
            node *jcurr = nt_arr[j];
            while(jcurr) {
               Production *jp = jcurr->p;
               Production *add = new Production(*jp);
               add->addToTail(new Production(*p));
               addProd(add, i);
               if(curr == &nt_arr[i])
                  curr = &((*curr)->next);
               jcurr = jcurr->next;
            }
            removeProd(curr);
            continue; 
         }
         curr = &((*curr)->next);
      }
      removeLRec(i);
   }
}

void Grammar::invSubst()
{
   for(int i = nt_size - 1; i >= 0; i--) {
      node **curr = &nt_arr[i];
      while(*curr) {
         Production *p = (*curr)->p;
         if(!p->isFirstTerminal()) {
            int j = nt_pos[(int) p->head->chr];
            p->removeFirst();
            node *jcurr = nt_arr[j];
            while(jcurr) {
               Production *jp = jcurr->p;
               Production *add = new Production(*jp);
               add->addToTail(new Production(*p));
               addProd(add, i);
               if(curr == &nt_arr[i])
                  curr = &((*curr)->next);
               jcurr = jcurr->next; 
            }
            removeProd(curr);
            continue;  
         }
         curr = &((*curr)->next);
      }
   }
   for(int i = nt_size; i < nt_size + nt_aux_size; i++) {
      node **curr = &nt_arr[i];
      while(*curr) {
         Production *p = (*curr)->p;
         if(!p->isFirstTerminal()) {
            int j = nt_pos[(int) p->head->chr];
            p->removeFirst();
            node *jcurr = nt_arr[j];
            while(jcurr) {
               Production *jp = jcurr->p;
               Production *add = new Production(*jp);
               add->addToTail(new Production(*p));
               addProd(add, i);
               if(curr == &nt_arr[i])
                  curr = &((*curr)->next);
               jcurr = jcurr->next; 
            }
            removeProd(curr);
            continue;  
         }
         curr = &((*curr)->next);
      }
   }

}

void Grammar::Greibach(bool p = false)
{
   eliminateEps();
   if(p) {
      puts("\nEpsilon Elimination:");
      if(epsProd)
         puts("The Grammar is epsilon productive.");
      print();
   }
   for(int i = 0; i < nt_size; i++) {
      removeLRec(i);
   }
   if(p) {
      puts("\nRemove LRec:");
      print();
   }
   removeIRec(); 
   if(p) {
      puts("\nRemove IRec:");
      print();
   }
   invSubst();
   if(p) {
      puts("\nInverse Substitution:");
      print();
   }
}

void Grammar::print() const
{  

   int k = 1;
   node *curr;
   for(int i = 0; i < nt_size; i++) {
      curr = nt_arr[i];
      while(curr) {
         printf("%d. \t%c -> ", k, nt_chr[i]);
         curr->p->print();
         curr = curr->next;
         k++;
      } 
   }
   for(int i = nt_size; i < nt_size + nt_aux_size; i++) {
      curr = nt_arr[i];
      while(curr) {
         printf("%d. \t%c -> ", k, 'X' + i - nt_size);
         curr->p->print();
         curr = curr->next;
         k++;
      } 
   }

}

Production::Production()
{
   head = 0; tail = 0;
}
Production::Production(const char* str, const bool *term)
{
   head = 0; tail = 0;
   for(int i = 0; str[i]; i++) {
      Symbol *s = new Symbol(term[(int)str[i]], str[i], 0);
      addToTail(s);
   }
   
}

Production::Production(const Production &other)
{
   head = 0; tail = 0;
   Symbol *curr = other.head;
   while(curr) {
      Symbol *copy = new Symbol(curr->isTerminal, curr->chr, 0);  
      addToTail(copy);
      curr = curr->next;
   }
}

void Production::addToTail(Symbol *s)
{
   if(!tail) {
      head = s;
      tail = s;
      return;
   }  
   tail->next = s;
   tail = s;
}

void Production::addToTail(Production *p)
{
   if(!p->tail) {
      delete p;
      return;
   }
   tail->next = p->head;
   tail = p->tail;
   p->head = 0;
   p->tail = 0;
   delete p;
}

void Production::addToHead(Symbol *s)
{
   if(!head) {
      head = s;
      tail = s;
      return;
   }
   s->next = head;
   head = s;
}

void Production::removeFirst()
{
   if(head == tail) {
      delete head;
      head = 0;
      tail = 0;
      return;
   }
   Symbol *save = head;
   head = head->next;
   save->next = 0;
   delete save;
}

void Production::addToHead(const Symbol *start, const Symbol *end)
{
  
   if(!start || start == end)
      return; 
   Symbol *h = new Symbol(start->isTerminal, start->chr, 0);
   Symbol *t = h;
   start = start->next;
   while(start != end) {
      t->next = new Symbol(start->isTerminal, start->chr, 0);
      t = t->next;
      start = start->next;
   }
   t->next = head;
   head = h;
}


node* Production::
substEps(node **out, const bool *eps, const Symbol *s, int* rec, 
                                                   const int *nt_pos)
{
   const Symbol *save = s;
   while(s && (s->isTerminal || !eps[nt_pos[(int)s->chr]]))
      s = s->next;
   if(!s) {
      Production *p = new Production();
      node *newNode = new node(*out, p);
      *out = newNode;
      *rec = 1;
      p->addToHead(save, s);
      return *out; 
   }
   int my_rec;
   substEps(out, eps, s->next, &my_rec, nt_pos);
   node *start = substEps(out, eps, s->next, &my_rec, nt_pos); 
   node *curr = start;
   for(int i = 0; i < 2 * my_rec; i++) {
      if(i < my_rec) {
         curr->p->addToHead(new Symbol(s->isTerminal, s->chr, 0));
      }
      curr->p->addToHead(save, s);
      curr = curr->next;
   } 
   *rec = my_rec * 2;
   return start;
}

void Grammar::eliminateEps()
{
 
   bool *eps = new bool[nt_size];
   memset(eps, 0, nt_size * sizeof(bool));
   
   bool f;  
   do {
      f = false;
      for(int i = 0; i < nt_size; i++) {
         if(eps[i])
            continue;
         node* curr = nt_arr[i];
         while(curr) {
            if(curr->p->isEps(eps, nt_pos)) {
               eps[i] = true;
               f = true;
               break;
            } 
            curr = curr->next;
         } 
      }
   } while (f);
   if(eps[0]) {
      epsProd = true;
   }
   for(int i = 0; i < nt_size; i++) {
      node* curr = nt_arr[i];
      while(curr) {
         node *add = 0;
         int rec;
         Production::substEps(&add, eps, curr->p->head, &rec, nt_pos);
         addProd(add, 0, i); 
         curr = curr->next;
      }
   }
   delete [] eps;
   clean();
}

void Grammar::addProd(node *start, node *end, int i)
{
   if(!start || start == end)
      return;
   node *tail = start;
   while(tail->next != end)
      tail = tail->next;
   tail->next = nt_arr[i];
   nt_arr[i] = start; 
}

void Grammar::clean()
{
   for(int i = 0; i < nt_size; i++) {
      node** curr = &nt_arr[i];
      while(*curr) {
         if((*curr)->p->head == 0 || (*curr)->p->head->chr == EPS){
            removeProd(curr);
            continue;
         }
         node* follow = (*curr)->next;
         bool f = false;
         while(follow) {
            if((*curr)->p->cmp(follow->p)) {
               f = true;
               removeProd(curr);
               break; 
            }
            follow = follow->next;
         }
         if(!f)
            curr = &((*curr)->next);
      }
   }
}

void Grammar::removeProd(node **prev)
{
   node *tmp = (*prev);
   *prev = (*prev)->next;
   tmp->next = 0;
   delete tmp;
}
bool Production::isEps(const bool *eps, const int *pos) const
{
   bool f = true;
   Symbol *curr = head;
   while(curr) {
      if(curr->chr == EPS){
         curr = curr->next;
         continue;
      }
      if(curr->isTerminal || !eps[pos[(int) curr->chr]]) {
         f = false;
         break;
      }
      curr = curr->next;
   }
   return f;
}

bool Production::cmp(const Production *p) const
{
   Symbol *a = head, *b = p->head;
   while(a && b) {
      if(a->chr != b->chr)
         return false;
      a = a->next;
      b = b->next;
   }
   if(a || b)
      return false;
   return true;
}

void Production::print() const
{
   Symbol* curr = head;
   while(curr) {
      putchar(curr->chr);
      curr = curr->next;
   }
   putchar('\n');   
}



