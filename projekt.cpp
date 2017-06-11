#include <iostream>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>
#include <chrono>
#include <ncurses.h>

using namespace std;




//resources
bool **matrix;
vector <int> queue1;
vector <int> queue2;

//otherGlobals
int sizeM=0;
int sizeN=0;

//cv and mtx for resources
mutex mx_queue1;
mutex mx_queue2;
mutex mx_matrix;

//threads
void producent(){
 
 int randNum;
 int maxSize = 10;
while(1){
 randNum = rand() % sizeM;

 //cout<<"added to queues:\n x: "<<randNum<<endl;
 mx_queue1.lock();
 if(queue1.size()<=maxSize)queue1.push_back(randNum);
 mx_queue1.unlock();

 randNum= rand() % sizeN;
 //cout<<"y: "<<randNum<<endl;
 mx_queue2.lock();
 if(queue2.size()<=maxSize)queue2.push_back(randNum);
 mx_queue2.unlock(); 
  
 this_thread::sleep_for(chrono::seconds(1));
 }
};

void przetwarzacz(){
 int x,y;
while(1){
  x=-1;
  y=-1;

 //get values from queues if they exist
 mx_queue1.lock();
 if(queue1.size()>0) { x = queue1.front();queue1.erase(queue1.begin());}
 mx_queue1.unlock();

 mx_queue2.lock();
 if(queue2.size()>0) { y = queue2.front();queue2.erase(queue2.begin());}
 mx_queue2.unlock();

 //move pawns
 for(int i=0;i<sizeM;i++){
  for(int j=0;j<sizeN;j++){
   if(j==0){
    mx_matrix.lock();
    matrix[i][j] = false;
    mx_matrix.unlock();
    }else{
      mx_matrix.lock();
      if(matrix[i][j]){
      matrix[i][j] = false;
      matrix[i][j-1] = true;
     }
      mx_matrix.unlock();
     
    }//end if
   }//end for
  }//end for

  //try to put pawn on table if field is empty,if no return pawn to queues
  if((x>-1)&&(y>-1)){
   mx_matrix.lock();
   if(!matrix[x][y]){
    matrix[x][y] = true;
   }else{
    mx_queue1.lock();
    queue1.push_back(x);
    mx_queue1.unlock();
   
    mx_queue2.lock();
    queue2.push_back(y);
    mx_queue2.unlock();
   }
   mx_matrix.unlock();
  }
 this_thread::sleep_for(chrono::seconds(1));
}
 
};
void displayMatrix(){

   
    
    keypad( stdscr, FALSE ); //wylaczenie strzalek, F1 itp...
    char znak; //utworzenie zmiennej typu char
    do //poczatek petli
    {
    initscr(); //start
    start_color();	
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
  //  printw( "Hello World !!!\n" ); //2
    for(int i=0;i<sizeM;i++){
     for(int j=0;j<sizeN;j++){
      if(matrix[i][j]){
       attron(COLOR_PAIR(1));
       printw("%d ",(int)matrix[i][j]);
       attroff(COLOR_PAIR(1));
      }else{
       printw("%d ",(int)matrix[i][j]);
      }
     }
     printw("\n");
    }
    printw("sizeOfQ1: %d \n",queue1.size());
    printw("sizeOfQ2: %d ",queue2.size());
   // getch(); //3
    refresh();
    this_thread::sleep_for(chrono::seconds(1));
    clear();
    endwin(); //konczenie
     
    } while(1);  
}

int main(int argc,char *argv[])
{
       
       vector<thread> threads;
       srand(time(NULL));
         if(argc != 3){
        cout<<"wrong number of parameters!"<<endl;
       }else{
        //read from argv
        if(atoi(argv[1])>1 && atoi(argv[2])>1){
         sizeM = atoi(argv[1])-1;
         cout<<"readed M:"<<sizeM<<endl;
         sizeN = atoi(argv[2])-1;
         cout<<"readed N:"<<sizeN<<endl;

         //create new array   
         matrix = new bool *[sizeM];
         for (int i =0;i<sizeM;i++){
         matrix[i] = new bool [sizeN];
         }
         
         //no pawns on table - each fields should be false
         for(int i=0;i<sizeM;i++){
           for(int j=0;j<sizeN;j++){
            matrix[i][j] = false;         
          }
         }
        }else{
         cout<<"sizes of arrays have to be greater than 1!"<<endl;
         return 0;
        }        
       }
threads.push_back(thread(przetwarzacz));

threads.push_back(thread(producent));
threads.push_back(thread(displayMatrix));
for (auto& thread : threads) thread.join();// for (int i = 0; i < 10; i++)

  return 0;
}





















