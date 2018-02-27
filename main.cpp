/*
Timeline:
Oct 12 @ 20:00 - adjacency list representation of graph is working
Oct 13 @ 01:45 - Dijkstra's using binary heaps is working for normal cases. test for the unusual ones
Oct 14 @ 03:56 - binomial heap is mostly implemented. huge problem of updating 'parent' of each node persists [cuz a node doesn't point to all kids]
                 parents are important in decrease-key operation where value percolates upwards
Oct 15 @ 01:56 - Dijkstra's using binomial heaps is working for normal cases. test for the unusual ones
Oct 15 @ 04:19 - Fibonacci heaps partially implemented
Oct 15 @ 20:12 - Dijkstra's using fibonacci heaps is working for normal cases. test for the unusual ones

Remaining work:
-> read graph input from file DONE
-> generate graph input using random numbers NOT NEEDED
-> display running time of each heap DONE
-> display number of operations in each heap
-> add checks for wrong input =/= error handling - stuff like wrong format, negative weights etc DONE
-> test the program for few more graphs
*/


/*
Notes:
-> Each vertex is labeled as a number - from 0 to V-1 where V is the total number of vertices
-> The graph is stored in adjacency list format
-> Need for heaps: to store the vertices not yet included in Shortest-Path-Tree (or, the vertices for which shortest distance has not been finalized yet).
-> Search operation in both binomial and fibonacci heaps is expensive
-> Link for online simulation for Dijkstra's algorithm on any input graph:  http://graphonline.ru/en/
*/

#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<fstream>
#include<chrono>
using namespace std;

class adjlistnode{
 public:
    int dest;
    int weight;
    class adjlistnode*next;

    adjlistnode(int d, int w)
    {
        dest = d;
        weight = w;
        next = NULL;
    }

    adjlistnode()
    {
        next = NULL;
        dest = weight = 0;
    }

    void modify(int d, int w)
    {
        dest = d;
        weight = w;
    }

}*temp; //this pointer will be used repeatedly for inserting nodes into the adjacency list


class graph{
 public:
    int v;
    adjlistnode *adjlist;

    graph(int numvertices)
    {
        v = numvertices;
        adjlist = new adjlistnode[v]; //access each node now as adjlist[num]

        creategraph();
    }

    void creategraph()
    {
        int i;

        for(i=0;i<v;i++)
            adjlist[i].modify(i, 0);
    }

    void addedge(int s, int d, int w) //source dest, weight
    {
        temp = new adjlistnode(d, w);

        adjlistnode *travel = &adjlist[s];

        while(travel->next!=NULL)
            travel = travel->next;

        travel->next = temp;

        temp=NULL; //if you do 'delete temp', that will eradicate/release/free-up the newly create node
    }

    void printgraph()
    {
         adjlistnode *travel = NULL;
         int i;
         cout<<endl<<endl;
         int who; //the source of the edge
         for(i=0;i<v;i++)
         {
             who = adjlist[i].dest;
             travel = adjlist[i].next;

             while(travel!=NULL)
             {
                 cout<<who<<'\t'<<travel->dest<<'\t'<<travel->weight<<endl;
                 travel=travel->next;
             }
         }
    }

}*g;

//the answers are stored in the following
int *parent; //parent-array
int *dist; //shortest path's cost

int ninsert;
int nextract;
int ndecrease;
int nunion;
int nswap;
int ntrav;

void reset()
{
    ninsert = 0;
    nextract = 0;
    ndecrease = 0;
    nunion = 0;
    nswap = 0;
    ntrav = 0;
}


class binaryheapnode{
 public:
     int v;
     int dist;

     binaryheapnode()
     {
         v = 0;
         dist = INT_MAX;
     }
};

class binaryheap{
 public:
    int total; //total number of nodes in heap
    int size;
    int *pos;

    class binaryheapnode *minheap;

    binaryheap(int num) //num is the max size of heap
    {
        total = num;
        size=0;
        minheap = new binaryheapnode[num];
        pos = new int[num]; //holds pos[i] gives index number of vertex i in minheap-array

        createminheap();
    }

    void createminheap()
    {
        int i;
        for(i=0; i<total; i++)
        {
            minheap[i].dist = INT_MAX;
            minheap[i].v = i;
            pos[i] = i;
        }
    }

    int getmin()
    {
        return minheap[0].dist;
    }

    //heapify ripples values downwards
    void heapify(int index) //if parentvalue > any childvalue
    {
        int left = 2*index + 1;
        int right = 2*index + 2;
        int smallest = index;

        int p, c; //parent and child vertex

        if(left<size && minheap[left].dist < minheap[smallest].dist)
            smallest = left;
        if(right<size && minheap[right].dist < minheap[smallest].dist)
            smallest = right;

        if(smallest!=index)
        {
            p = minheap[index].v;

            if(smallest == left)
            {
                c = minheap[left].v;

                //swap!
                nswap++;
                binaryheapnode swapper;
                swapper = minheap[left];
                minheap[left] = minheap[index];
                minheap[index] = swapper;

                pos[c] = index;
                pos[p] = left;
            }

            else
            {
                c = minheap[right].v;

                //swap!
                nswap++;
                binaryheapnode swapper;
                swapper = minheap[right];
                minheap[right] = minheap[index];
                minheap[index] = swapper;

                pos[c] = index;
                pos[p] = right;
            }

            heapify(smallest);
        }
    }

    void decreasekey(int x, int val)
    {
        int index = pos[x];
        minheap[index].dist = val;

        //ripple value upwards
        int parentindex = (index-1)/2;

        int p, c; //vertex labels

        while(index && minheap[index].dist < minheap[parentindex].dist)
        {
            p = minheap[parentindex].v;
            c = minheap[index].v;

            //swap!
            nswap++;
            binaryheapnode swapper;
            swapper = minheap[parentindex];
            minheap[parentindex] = minheap[index];
            minheap[index] = swapper;

            pos[p] = index;
            pos[c] = parentindex;

            index = (index-1)/2;
            parentindex = (index-1)/2;
        }
    }

    binaryheapnode* extractmin()
    {
        //cout<<size<<endl;
        if(size==0)
            return NULL;

        binaryheapnode* root = new binaryheapnode();
        root->v = minheap[0].v;
        root->dist = minheap[0].dist;

        //replace root with last element in heap
        minheap[0] = minheap[size-1];

        pos[root->v] = INT_MAX;
        pos[minheap[0].v]=0;

        size--;

        heapify(0);

        return root;
    }

    bool search(int x)
    {
        if(pos[x]>=size)
            return false;
        return true;
    }

//decreasekey - maintain pos[] values as you shift values up/down the tree
//extractmin - maintain pos[] values as you shift values up/down the tree
//search -> check if pos[] is greater than current number of nodes in heap
};


class binomialheapnode{
 public:
     int v; //vertex label
     int dist;

     int degree; //number of kids it has

     binomialheapnode *parent;
     binomialheapnode *child;
     binomialheapnode *sibling;

     binomialheapnode(int label, int d)
     {
         v = label;
         dist = d;
         parent = NULL;
         child = NULL;
         sibling = NULL;
         degree = 0;
     }

     binomialheapnode()
     {
         parent = NULL;
         child = NULL;
         sibling = NULL;
         degree = 0;
         v = -1;
         dist = INT_MAX;
     }
}*btemp; //to create nodes for insertion


class binomialheap{
 public:
     binomialheapnode *head;

    binomialheap()
    {
        head = NULL;
    }

    //make tree rooted at y a subtree of tree rooted at z
    void link(binomialheapnode *y, binomialheapnode* z)
    {
       y->parent = z;
       y->sibling = z->child;
       z->child = y;
       z->degree = z->degree + 1;
    }

    void insert(int v, int dist)
    {
        btemp = new binomialheapnode(v, dist);
        heapunion(btemp);
    }

    //we're not explicitly doing union on two different heaps
    //we do delete-min and then do union of remaining stuff, or, we do union while inserting a new node into existing heap
    //so, ultimately, everything is within one heap only
    //thus, we update everything in 'head' pointer itself. no need to return the resultant head from every function
    //doing union and merge in one pass, like in Alan Weiss book
    void heapunion(binomialheapnode *head2)
    {

        binomialheapnode *t1, *t2, *carry, *prev_carry,*siblingholder;

        t1 = head;
        t2 = head2;
        carry = NULL;
        siblingholder = NULL;
        prev_carry = NULL;

        bool flag = false; //head to result hasn't been set yet

        if(t1==NULL && t2==NULL)
        {
            head = NULL; //redundant
            return;
        }
        else if(t1==NULL)
        {
            head = t2;
            return;
        }
        else if(t2==NULL)
        {
            head = t1; //redundant
            return;
        }

        nunion++;
        while(t1!=NULL && t2!=NULL)
        {

            if(t1->degree < t2->degree)
            {

                if(carry==NULL)
                {
                    carry = t1;
                    prev_carry = carry; //initialize

                    ntrav++;

                    t1 = t1->sibling;
                    carry->sibling = NULL;
                }
                else
                {
                    if(carry->degree == t1->degree)
                    {
                        if(carry->dist <= t1->dist) //make t1 a child of carry
                        {
                            siblingholder = t1->sibling;
                            link(t1, carry); //changes t1->sibling. hence need of a siblingholder

                            if(t1==head)
                                head = carry;

                            t1 = siblingholder;
                        }
                        else
                        {
                            //node pointing to carry should point to t1 now
                            prev_carry->sibling = t1;
                            link(carry, t1);

                            if(carry==head)
                                head = t1;

                            ntrav++;
                            carry = t1;

                            ntrav++;
                            t1 = t1->sibling;
                            carry->sibling = NULL;
                        }
                    }

                    else //carry->degree < t1->degree
                    {
                        prev_carry = carry;
                        carry->sibling = t1;

                        ntrav++;
                        carry = carry->sibling;

                        ntrav++;
                        t1 = t1->sibling;
                        carry->sibling = NULL;
                    }
                }
            }

            else if(t2->degree < t1->degree)
            {
                if(carry==NULL)
                {
                    carry = t2;
                    prev_carry = carry; //initialize

                    ntrav++;
                    t2 = t2->sibling;
                    carry->sibling = NULL;
                }
                else
                {
                    if(carry->degree == t2->degree)
                    {
                        if(carry->dist <= t2->dist) //make t2 a child of carry
                        {
                            siblingholder = t2->sibling;
                            link(t2, carry); //changes t2->sibling

                            if(t2==head)
                                head = carry;

                            t2 = siblingholder;
                        }
                        else
                        {
                            //node pointing to carry should point to t2 now
                            prev_carry->sibling = t2;
                            link(carry, t2);

                            if(carry==head)
                                head = t2;

                            ntrav++;
                            carry = t2;

                            ntrav++;
                            t2 = t2->sibling;
                            carry->sibling = NULL;
                        }
                    }

                    else //carry->degree < t2->degree
                    {
                        prev_carry = carry;
                        carry->sibling = t2;

                        ntrav++;
                        carry = carry->sibling;

                        ntrav++;
                        t2 = t2->sibling;
                        carry->sibling = NULL;
                    }
                }
            }

            else //t1->degree = t2->degree
            {
                //can optimise here by making checks on degree of tree. then make smaller tree part of the larger one
                if(carry==NULL)
                {
                    if(t1->dist <= t2->dist)
                    {
                        siblingholder = t2->sibling;
                        link(t2,t1);

                        if(t2==head)
                            head = t1;

                        ntrav++;
                        carry = t1;
                        prev_carry = carry; //initialize

                        ntrav++;
                        t1 = t1->sibling;
                        t2 = siblingholder;
                        carry->sibling = NULL;
                    }

                    else
                    {
                        siblingholder = t1->sibling;
                        link(t1,t2);

                        if(t1==head)
                            head = t2;

                        ntrav++;
                        carry = t2;
                        prev_carry = carry; //initialize

                        ntrav++;
                        t2 = t2->sibling;
                        t1 = siblingholder;
                        carry->sibling = NULL;
                    }
                }
                else //merge t1&t2. doesn't matter what carry->degree is
                {
                    if(t1->dist <= t2->dist)
                    {
                        siblingholder = t2->sibling;
                        link(t2,t1);

                        if(t2==head)
                            head = t1;

                        carry->sibling = t1;
                        prev_carry = carry;

                        ntrav++;
                        carry = carry->sibling;

                        ntrav++;
                        t1 = t1->sibling;
                        t2 = siblingholder;
                        carry->sibling = NULL;
                    }

                    else
                    {
                        siblingholder = t1->sibling;
                        link(t1,t2);

                        if(t1==head)
                            head = t2;

                        carry->sibling = t2;
                        prev_carry = carry;

                        ntrav++;
                        carry = carry->sibling;

                        ntrav++;
                        t2 = t2->sibling;
                        t1 = siblingholder;
                        carry->sibling = NULL;
                    }
                }
            }

            if(!flag)
            {
                head = carry;

                if(carry!=NULL)
                    flag = true;
            }
        }

        while(t2!=NULL) //merging carry-heap and t2-heap
        {
            if(carry->degree < t2->degree)
            {
                prev_carry = carry;
                carry->sibling = t2;

                ntrav++;
                t2 = t2->sibling;

                ntrav++;
                carry = carry->sibling;
                carry->sibling = NULL;
            }

            else //their degrees are equal
            {
                if(carry->dist <= t2->dist) //make t2 a child of carry
                {
                    siblingholder = t2->sibling;
                    link(t2, carry); //changes t2->sibling. hence need of a siblingholder

                    if(t2==head)
                        head = carry;

                    ntrav++;
                    t2 = siblingholder;
                }
                else
                {
                    //node pointing to carry should point to t2 now
                    prev_carry->sibling = t2;
                    link(carry, t2);


                    if(carry==head)
                        head = t2;

                    carry = t2;

                    ntrav++;
                    t2 = t2->sibling;
                    carry->sibling = NULL;
                }
            }
        }

        while(t1!=NULL) //merging carry-heap and t1-heap
        {
            if(carry->degree < t1->degree)
            {
                prev_carry = carry;
                carry->sibling = t1;

                ntrav++;
                t1 = t1->sibling;

                ntrav++;
                carry = carry->sibling;
                carry->sibling = NULL;
            }

            else //their degrees are equal
            {
                if(carry->dist <= t1->dist) //make t1 a child of carry
                {
                    siblingholder = t1->sibling;
                    link(t1, carry); //changes t1->sibling. hence need of a siblingholder

                    if(t1==head)
                        head = carry;

                    t1 = siblingholder;
                }
                else
                {
                    //node pointing to carry should point to t1 now
                    prev_carry->sibling = t1;
                    link(carry, t1);

                    if(carry==head)
                        head = t1;

                    carry = t1;

                    ntrav++;
                    t1 = t1->sibling;
                    carry->sibling = NULL;
                }
            }
        }
    }


    //needed to reverse the kids into a heap in extract-min function
    binomialheapnode* reverselist(binomialheapnode *x)
    {
        binomialheapnode *result = NULL;

        binomialheapnode *prev, *current, *next;
        prev = NULL;
        current = x;
        next = NULL;

        while(current!=NULL)
        {
            //creating a new heap, so all these nodes have no parent now
            current->parent = NULL;

            ntrav++;
            next = current->sibling;
            current->sibling = prev;

            ntrav++;
            prev = current;

            ntrav++;
            current = next;
        }

        result = prev;

        return result;
    }


    binomialheapnode* extractmin()
    {
        if(head==NULL)
            return NULL;

        binomialheapnode *travel = head;

        //first scan the list to find minimum
        int small = head->dist;
        binomialheapnode *smallnode = head;

        binomialheapnode *prev = NULL;

        while(travel->sibling!=NULL)
        {
            if(travel->sibling->dist < small)
            {
                small = travel->sibling->dist;
                smallnode = travel->sibling;
                prev = travel;
            }

            ntrav++;
            travel = travel->sibling;
        }

        if(prev==NULL && smallnode->sibling==NULL) //=> only one tree in heap
        {
            head = NULL; //the kids of smallnode will be the new heap
        }

        else
        {
            if(prev == NULL)
                head = smallnode->sibling;
            else
                prev->sibling = smallnode->sibling;
        }

        if(smallnode->child!=NULL)
        {
            binomialheapnode *kidsheap = reverselist(smallnode->child);
            //in this new heap, write code so that you assign all their parents to NULL
            heapunion(kidsheap);
        }

        smallnode->sibling = NULL;
        return smallnode;
    }

    binomialheapnode* heapsearch(int label) //vertex label
    {
        return heapsearch(head, label);
    }

    binomialheapnode* heapsearch(binomialheapnode *h, int label)
    {
        binomialheapnode *x, *p;
        x = h;
        p = NULL;

        if(x->v == label)
        {
            p = x;
            return p;
        }

        if (x->child != NULL && p == NULL)
        {
            ntrav++;
            p = heapsearch(x->child, label);
        }
        if (x->sibling != NULL && p == NULL)
        {
            ntrav++;
            p = heapsearch(x->sibling, label);
        }

        return p;
    }

//decreasekey
    void decreasekey(int label, int newval)
    {
        binomialheapnode *who = heapsearch(label);
        int swapper;
        //percolate-up
        who->dist = newval;

        while(who->parent!=NULL && who->dist < who->parent->dist)
        {
            nswap++;

            swapper = who->v;
            who->v = who->parent->v;
            who->parent->v = swapper;

            swapper = who->dist;
            who->dist = who->parent->dist;
            who->parent->dist = swapper;

            ntrav++;
            who = who->parent;
        }
    }

    bool isEmpty()
    {
        if(head==NULL)
            return 1;
        return 0;
    }

    void printheap()
    {
        binomialheapnode *travel = head;

        cout<<"\n\nthe heap:\n";

        if(travel==NULL)
        {
            cout<<"heap empty";
            return;
        }

        while(travel!=NULL)
        {
            cout<<travel->v<<"("<<travel->dist<<","<<travel->degree<<")"<<" ";
            travel = travel->sibling;
        }
    }

};


class fibonacciheapnode{
 public:
     int v; //vertex-label
     int dist;

     int degree;

     fibonacciheapnode *parent;
     fibonacciheapnode *child;
     fibonacciheapnode *leftsibling;
     fibonacciheapnode *rightsibling;

     bool mark; //whether it has lost a kid already or not
     bool track; //when traversing a cirular linked list, we don't know when to stop cuz everything points to each other and nothing points to NULL. this 'track' will keep track of which nodes have been traversed so far

     fibonacciheapnode(int label, int d)
     {
         v = label;
         dist = d;

         degree = 0;

         parent = NULL;
         child = NULL;
         leftsibling = this; //if I have no siblings, then these pointers point to myself
         rightsibling = this;

         mark = false;
         track = false;
     }


}*ftemp; //to create nodes for insertion

class fibonacciheap{
 public:
     fibonacciheapnode *head; //points to node with minimum value in the heap
     int num; //number of nodes in the heap

     fibonacciheap()
     {
         head = NULL;
         num = 0;
     }

     void insert(int label, int d) //wrapper function
     {
         ftemp = new fibonacciheapnode(label, d); //ftemp is a global pointer
         num++;
         insert(ftemp);
     }

     void insert(fibonacciheapnode *x)
     {
         //this function is also called from consolidate. thus, num++ is omitted here and put in wrapper function instead
         if(head==NULL)
         {
             head = x;
         }
         else //just add the node to root list
         {
             x->rightsibling = head->rightsibling;
             x->leftsibling = head;

             head->rightsibling->leftsibling = x;
             head->rightsibling = x;

             if(x->dist < head->dist)
                head = x;
         }
     }

     void link(fibonacciheapnode *y, fibonacciheapnode *z) //remove whichever circular list node y is a part of, and make it a child of node z
     {
         //removing y from the circular list it is part of
         y->leftsibling->rightsibling = y->rightsibling;
         y->rightsibling->leftsibling = y->leftsibling;

         //y->parent->mark = true

         y->parent = z;

         y->mark = false; //y is made a kid to someone else, so its mark is cleared

         if(z->child == NULL)
         {
             z->child = y;
             y->leftsibling = y;
             y->rightsibling = y;
         }
         else
         {
             y->rightsibling = z->child->rightsibling;
             y->leftsibling = z->child;

             if(y->rightsibling!=NULL)
                y->rightsibling->leftsibling = y;

             z->child->rightsibling = y;

             if(y->dist < z->child->dist)
                z->child = y;
         }

         z->degree = z->degree + 1;
     }


     void heapunion(fibonacciheap *heap2) //merge heap2 into me
     {

         fibonacciheapnode *head2 = heap2->head;
         num += heap2->num;

         fibonacciheapnode *temp1;
         fibonacciheapnode *temp2;

         temp1 = head->rightsibling;
         temp2 = head2->leftsibling;

         head->rightsibling = temp2;
         temp2->rightsibling = head;

         head2->leftsibling = temp1;
         temp1->leftsibling = head2;


         if(head->dist > head2->dist)
            head = head2;
     }


     fibonacciheapnode* extractmin()
     {
         fibonacciheapnode *ans = head;
         num--;

         if(num==0) //head was the only node in heap
         {
             head = NULL;
         }
         else
         {
            if(ans->child!=NULL) //if it has kids
            {
                fibonacciheapnode *temp = ans->child;
                fibonacciheapnode *smallestchild = ans->child;
                int smallchild = smallestchild->dist;

                while(temp->parent!=NULL) //once 'ans' node is removed, its kids' parent pointers should point to NULL
                {
                    if(smallchild > temp->dist)
                    {
                        smallestchild = temp;
                        smallchild = temp->dist;
                    }

                    temp->parent = NULL;

                    ntrav++;
                    temp = temp->rightsibling;
                }

                if(ans->rightsibling==ans) //if the node to be removed from the rootlist is the only node in the rootlist
                {
                    head = ans->child;
                }
                else
                {
                    temp = ans->child->rightsibling;
                    ans->leftsibling->rightsibling = ans->child;
                    ans->child->rightsibling = ans->leftsibling;
                    ans->rightsibling->leftsibling = temp;
                    temp->leftsibling = ans->rightsibling;
                }

                if(smallchild < head->dist)
                    head = smallestchild;
            }
            else
            {
                ans->leftsibling->rightsibling = ans->rightsibling;
                ans->rightsibling->leftsibling = ans->leftsibling;
            }

            head = head->rightsibling;
            consolidate();
         }

         return ans;
     }


     void consolidate()
     {

        float goldenratio = 1.618034;
        int D; //maximum number of elements in rootlist
        float f = log(num)/log(goldenratio);
        if(num==1) //special case cuz log(1)=0
            return; //no need to consolidate when there's only one node in the whole heap
        D = ceil(f);
        //if a fibonacci heap has 'num' elements in total, then the number of elements in the rootlist is bounded by the value log(num) (the log's base is the goldenratio)

        fibonacciheapnode *A[D]; //creating an array of D number of pointers
        int i;
        for(i=0;i<D;i++)
            A[i] = NULL;


        fibonacciheapnode *x = head;
        fibonacciheapnode *y, *swapper;
        int deg;

        nunion++;

        do
        {
            deg = x->degree;
            while(deg<D && A[deg]!=NULL)
            {
                y = A[deg];

                if(x->dist > y->dist)
                {
                    swapper = x;
                    x = y;
                    y = swapper;
                }//we always want to make y the kid of x. however, if x->dist > y->dist, then x will have to be made y's kid. hence, we swap the pointers, and now we can safely make y the kid of x

                link(y, x);

                if(y==head)
                    head = x->leftsibling;

                A[deg] = NULL;
                deg++;
            }
            A[deg] = x;

            ntrav++;
            x = x->rightsibling;

        }while(x!=head);

        head=NULL;

        //we'll now construct the entire heap again using the subtrees hashed in array A
        for(i=0;i<D;i++)
        {
            if(A[i]!=NULL)
                insert(A[i]);

            A[i] = NULL; //set this to avoid dangling pointer problem
        }
     }


     void decreasekey(int label, int newval)
     {
         fibonacciheapnode *who = heapsearch(label);
         fibonacciheapnode *parent = who->parent;

         who->dist = newval;

         if(parent!=NULL && newval < parent->dist)
         {
             cut(parent, who);
             cascadingcut(parent);
         }

         if(newval < head->dist)
            head = who;
     }

     void cut(fibonacciheapnode *y, fibonacciheapnode *x)
     {
         //y is the parent of x
         //x has to be cut from y and added to the rootlist

         if(x->rightsibling == x) //=> x is the only child of y
         {
             y->child = NULL;
         }

         else
         {
             x->leftsibling->rightsibling = x->rightsibling;
             x->rightsibling->leftsibling = x->leftsibling;

             if(y->child == x)
                y->child = x->leftsibling;
         }

         y->degree = y->degree -1;
         x->parent = NULL;

         //ading x to rootlist
         x->leftsibling = head;
         x->rightsibling = head->rightsibling;
         head->rightsibling->leftsibling = x;
         head->rightsibling = x;

         x->mark = false;
     }


     void cascadingcut(fibonacciheapnode *y)
     {
         fibonacciheapnode *z = y->parent;

         if(z!=NULL)
         {
             if(y->mark == false)
                y->mark = true;
             else
             {
                 cut(z, y);
                 cascadingcut(z);
             }
         }
     }


    fibonacciheapnode* heapsearch(int label) //wrapper function
    {
        return heapsearch(head, label);
    }

    fibonacciheapnode* heapsearch(fibonacciheapnode *x, int label)
    {
        fibonacciheapnode *travel = x;
        travel->track = true;

        fibonacciheapnode *ans = NULL;

        if(travel->v == label)
            ans = travel;

        if(ans==NULL && travel->child!=NULL)
        {
            ntrav++;
            ans = heapsearch(travel->child, label);
        }
        if(ans==NULL && travel->rightsibling->track==false)
        {
            ntrav++;
            ans = heapsearch(travel->rightsibling, label);
        }

        travel->track = false;

        return ans;
    }

    bool isEmpty()
    {
        if(num==0)
            return 1;
        return 0;
    }

    void printheap()
    {
        fibonacciheapnode *travel = head;

        cout<<"\n\nthe heap:\n";

        if(travel==NULL)
        {
            cout<<"heap empty";
            return;
        }

        while(travel->track==false)
        {
            travel->track = true;
            cout<<travel->v<<"("<<travel->dist<<","<<travel->degree<<")"<<" ";
            travel = travel->rightsibling;
        }
        while(travel->track==true)
        {
            travel->track = false;
            travel = travel->rightsibling;
        }
        cout<<"("<<num<<")";
    }
};


void printresult()
{
    int i, j;
    cout<<endl;

    int *minpath = new int[g->v];
    int pathcount;

    for(i=0; i < g->v; i++)
    {
        cout<<"\n\nVertex: "<<i;

        if(dist[i]==INT_MAX)
        {
            cout<<"\nVertex is unreachable";
            continue;
        }

        cout<<"\nPath: ";
        pathcount = 1;
        minpath[0] = i;
        j = parent[i];
        while(parent[j]!=j)
        {
            minpath[pathcount] = j;
            pathcount++;
            j = parent[j];
        }

        minpath[pathcount] = j;

        for(j=pathcount; j>=0; j--)
            cout<<minpath[j]<<" ";

        cout<<"\nPath cost: "<<dist[i];

    }
}

void paramprint(int t)
{
    if(t==1)
    {
        cout<<endl<<endl;
        cout<<"Number of extract-min ops: "<<nextract<<endl;
        cout<<"Number of decrease-key ops: "<<ndecrease<<endl;
        cout<<"Number of swaps: "<<nswap<<endl;
    }
    else if(t==2)
    {
        cout<<endl<<endl;
        cout<<"Number of insert ops: "<<ninsert<<endl;
        cout<<"Number of extract-min ops: "<<nextract<<endl;
        cout<<"Number of decrease-key ops: "<<ndecrease<<endl;
        cout<<"Number of union ops: "<<nunion<<endl;
        cout<<"Number of swaps: "<<nswap<<endl;
        //cout<<"Number of links traversed: "<<ntrav<<endl;
    }
    else if(t==3)
    {
        cout<<endl<<endl;
        cout<<"Number of insert ops: "<<ninsert<<endl;
        cout<<"Number of extract-min ops: "<<nextract<<endl;
        cout<<"Number of decrease-key ops: "<<ndecrease<<endl;
        cout<<"Number of consolidate ops: "<<nunion<<endl;
        //cout<<"Number of links traversed: "<<ntrav<<endl;
    }
}

void binarydijkstra(int src) //dijkstra's using binary heaps
{

    int v = g->v; //number of vertices

    parent = new int[v];
    dist = new int[v];

    //creating the binary heap
    binaryheap *heap = new binaryheap(v);

    int i;
    for(i=0;i<v;i++)
        dist[i] = INT_MAX;

    heap->size=v;

    dist[src] = 0;
    heap->decreasekey(src, 0);
    ndecrease++;
    parent[src]=src;

    binaryheapnode *unode;
    int u;
    adjlistnode *travel;
    int x;

    //cout<<heap->size<<endl;

    while(heap->size>0) //while there is still a reachable node in the heap whose distance hasn't been finalized yet
    {
        unode = heap->extractmin();
        nextract++;
        u = unode->v;
        travel = (g->adjlist[u]).next;

        while(travel!=NULL)
        {
            x = travel->dest;
            if(heap->search(x))
                if((dist[u] + travel->weight)<dist[x])
                {
                    dist[x] = dist[u] + travel->weight;
                    heap->decreasekey(x, dist[x]);
                    ndecrease++;
                    parent[x] = u;
                }
            travel = travel->next;
        }
    }

    printresult();
    paramprint(1);

    delete [] parent;
    delete [] dist;

}


void binomialdijkstra(int src) //dijkstra's using binomial
{

    //auto startT = chrono::steady_clock::now();

    int v = g->v; //number of vertices

    //parent[] and dist[] are global arrays that will store the final answers; dist will hold minimum path cost, while parent will store the parent of the vertex in the shortest-path-tree
    parent = new int[v];
    dist = new int[v];

    binomialheap *heap = new binomialheap();

    int i;
    for(i=0;i<v;i++)
        dist[i] = INT_MAX;

    dist[src] = 0;
    heap->insert(src, 0);
    ninsert++;
    parent[src] = src;

    binomialheapnode *unode;
    int u;
    adjlistnode *travel;
    int x;

    while(!(heap->isEmpty()))
    {
        unode = heap->extractmin();
        nextract++;

        u = unode->v;

        travel = (g->adjlist[u]).next;

        while(travel!=NULL)
        {
            x = travel->dest; //vertex-label of dest of edge

            if(dist[x]==INT_MAX) //=>insert it into heap for first time
            {
                dist[x] = dist[u] + travel->weight;

                heap->insert(x, dist[x]);

                ninsert++;
                parent[x] = u;


            }
            else if((dist[u] + travel->weight)<dist[x])
            {
                dist[x] = dist[u] + travel->weight;
                parent[x] = u;

                heap->decreasekey(x, dist[x]);
                ndecrease++;
            }
            travel = travel->next;
        }
    }

    //auto endT = chrono::steady_clock::now();

    printresult();
    paramprint(2);

    delete [] parent;
    delete [] dist;


}


void fibonaccidijkstra(int src) //dijkstra's using fibonacci
{

    //auto startT = chrono::steady_clock::now();


    int v = g->v; //number of vertices

    //parent[] and dist[] are global arrays that will store the final answers; dist will hold minimum path cost, while parent will store the parent of the vertex in the shortest-path-tree
    parent = new int[v];
    dist = new int[v];

    fibonacciheap *heap = new fibonacciheap();

    int i;
    for(i=0;i<v;i++)
        dist[i] = INT_MAX; //initially, every vertex is unreachable

    dist[src] = 0;
    heap->insert(src, 0);
    ninsert++;
    parent[src] = src;

    fibonacciheapnode *unode;
    int u;
    adjlistnode *travel;
    int x;

    while(!(heap->isEmpty()))
    {
        unode = heap->extractmin();
        nextract++;
        u = unode->v; //vertex-label of this extracted-node

        //graph is stored as as adjacency list
        travel = (g->adjlist[u]).next;

        while(travel!=NULL)
        {
            x = travel->dest; //vertex-label of destination of edge

            if(dist[x]==INT_MAX) //=> insert this vertex into the heap for the first time
            {
                dist[x] = dist[u] + travel->weight;

                heap->insert(x, dist[x]);
                ninsert++;
                parent[x] = u;
            }
            else if((dist[u] + travel->weight)<dist[x])
            {
                dist[x] = dist[u] + travel->weight;
                parent[x] = u;

                heap->decreasekey(x, dist[x]);
                ndecrease++;
            }
            travel = travel->next;
        }

    }

    //auto endT = chrono::steady_clock::now();

    printresult();
    paramprint(3);

    delete [] parent;
    delete [] dist;


}



int main()
{
    /*
    int num = 9; //number of vertices in the graph
    int source = 0; //source-vertex

    g = new graph(num);
    g->addedge(0, 1, 4);
    g->addedge(0, 7, 8);
    g->addedge(1, 2, 8);
    g->addedge(1, 7, 11);
    g->addedge(2, 3, 7);
    g->addedge(2, 5, 4);
    g->addedge(2, 8, 2);
    g->addedge(3, 4, 9);
    g->addedge(3, 5, 14);
    g->addedge(4, 5, 10);
    g->addedge(5, 6, 2);
    g->addedge(6, 7, 1);
    g->addedge(6, 8, 6);
    g->addedge(7, 8, 7);
    */



    ifstream myinput;
    myinput.open("input.txt");

    int num;
    int source;
    int s, d, w;

    if(!myinput.is_open())
    {
        cout<<"\n\nFailed to open file";
        exit(0);
    }

    myinput>>num;
    myinput>>source;

    g = new graph(num);

    while(!myinput.eof())
    {
        myinput>>s;
        myinput>>d;
        myinput>>w;

        if(w<0)
        {
            cout<<"\n\nNegative weights not allowed!\n";
            exit(0);
        }

        g->addedge(s, d, w);
    }

    myinput.close();



    cout<<"Input graph:\n";
    g->printgraph();

    cout<<"\n\nSource vertex: "<<source;

    std::chrono::high_resolution_clock::time_point startT = chrono::high_resolution_clock::now();


    cout<<"\n\n---------------------------- Using binary heaps ---------------------------- ";
    reset();
    binarydijkstra(source);

    std::chrono::high_resolution_clock::time_point endT = chrono::high_resolution_clock::now();

    cout<<"\nElapsed time in seconds: ";
    cout<<(chrono::duration_cast<chrono::nanoseconds>(endT-startT).count())/(1000000000.0);
    cout<<endl;

    startT = chrono::high_resolution_clock::now();

    cout<<"\n\n\n---------------------------- Using binomial heaps ---------------------------- ";
    reset();
    binomialdijkstra(source);

    endT = chrono::high_resolution_clock::now();

    cout<<"\nElapsed time in seconds: ";
    cout<<(chrono::duration_cast<chrono::nanoseconds>(endT-startT).count())/(1000000000.0);
    cout<<endl;

    startT = chrono::high_resolution_clock::now();

    cout<<"\n\n\n---------------------------- Using fibonacci heaps ---------------------------- ";
    reset();
    fibonaccidijkstra(source);

    endT = chrono::high_resolution_clock::now();

    cout<<"\nElapsed time in seconds: ";
    cout<<(chrono::duration_cast<chrono::nanoseconds>(endT-startT).count())/(1000000000.0);
    cout<<endl;

    return 0;
}

