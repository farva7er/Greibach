#include<stdio.h>
#include<string.h>
#include"Grammar.hpp"

enum { alphabet_max = 256, prod_max = 512};
void removeNL(char *str)
{
   for(int i = 0; str[i]; i++)
      if(str[i] == '\n') {
         str[i] = 0;
         break;
      }
}

int main()
{

   bool *terminals = new bool[alphabet_max];
   memset(terminals, 0, alphabet_max * sizeof(bool));

   char buff[alphabet_max];

   fgets(buff, alphabet_max, stdin);
   terminals[(int) '$'] = true;
   for(int i = 0; buff[i] != '\n'; i++) {
      terminals[(int)buff[i]] = true;
   }
   
   int *nt_pos = new int[alphabet_max];
   fgets(buff, alphabet_max, stdin); 
   int nt_size = strlen(buff) - 1;
   char *nt_chr = new char[nt_size];
   for(int i = 0; buff[i] != '\n'; i++) {
      nt_pos[(int)buff[i]] = i;
      nt_chr[i] = buff[i];
   }

   Grammar gr(terminals, nt_size, nt_pos, nt_chr);
   
   int pr_size;
   char prod[prod_max];
   for(int i = 0; i < nt_size; i++) {
      printf("%c: ", nt_chr[i]);
      scanf("%d", &pr_size);
      getchar();
      for(int j = 0; j < pr_size; j++) {
         fgets(prod, prod_max, stdin);
         removeNL(prod);
         gr.addProd(prod, i);
      } 
   }
   gr.print();
   gr.Greibach(true);
}
