#include <assert.h>

#include <stdbool.h>

#include <stddef.h>

#include <stdio.h>

#include <stdlib.h>

#include <time.h>



// Node definition

struct SkipListNode

{

    struct SkipListNode** next;

    int height;

    float value;

    int* info;

};



// Skip List definition

struct SkipList

{

    struct SkipListNode* head;

    int len;

    int maxHeight;

    int swtch;

};



// Node initializer

struct SkipListNode* node_init(int height, float value, int* info)

{

    struct SkipListNode* self = malloc(sizeof(struct SkipListNode));

    self->next = (struct SkipListNode**)calloc(height, sizeof(struct SkipListNode*));

    self->height = height;

    self->value = value;

    self->info = info;

    return self;

}



// Skip List Initializer

struct SkipList* list_init(bool min)

{

    struct SkipList* self = malloc(sizeof(struct SkipList));

    self->head = node_init(0, -1, NULL);

    self->len = 0;

    self->maxHeight = 0;

    self->swtch = min ? -1 : 1;

    return self;

}



// coin flip to determine height of new node 

// return height of new node

// TODO: better random generator (?)

int flip_coin()

{

    int height = 1;

    srand(time(NULL));

    while(rand()%2 != 0)

    {

        height++;

    }

    return height;

}



// find route to node, by value, through all levels 

// return array of last nodes at each level such that node.value < value at each level

struct SkipListNode** updateList(struct SkipList* self, float value)

{

    struct SkipListNode** path = (struct SkipListNode**)calloc(self->maxHeight, sizeof(struct SkipListNode*));

    struct SkipListNode* x = self->head;

    for(int i = self->maxHeight-1; i>-1; i--)

    {

        while(x->next[i] != NULL && x->next[i]->value < value)

        {

            x = x->next[i];

        }

        path[i] = x;

    }

    return path;

}



// find a node by value 

// uses updateList (<) to find path

// return pointer to node

struct SkipListNode* find(struct SkipList* self, float value, struct SkipListNode** update)

{

    if(update == NULL)

    {

        update = updateList(self, value);

    }

    struct SkipListNode* candidate = update[0]->next[0];

    if(candidate != NULL)

    {

        if(candidate->value == value)

        {

            return candidate;

        }

    }

    return NULL;

}



// check if value is in list

// use find()

// return true if found, false otherwise

bool __contains__(struct SkipList* self, float value)

{

    return (bool)(find(self, value, NULL) != NULL);

}



// find route to node, by value, through all levels 

// return array with last node s.t. node.value<value and node.info<info at each level

struct SkipListNode** updateListInfo(struct SkipList* self, float value, int* info)

{

    struct SkipListNode** path = (struct SkipListNode**)calloc(self->maxHeight, sizeof(struct SkipListNode*));

    struct SkipListNode* x = self->head;

    for(int i = self->maxHeight-1; i>-1; i--)

    {

        while(x->next[i] != NULL && x->next[i]->value < value)

        {

            x = x->next[i];

        }

        while(x->next[i] != NULL && x->next[i]->value == value && x->next[i]->info<info)

        {

            x = x->next[i];

        }

        path[i] = x;

    }

    return path;

}



// insert a node with the given value and info

// uses updateListInfo

// return 0 on success, -1 on failure

int add(struct SkipList* self, float value, int* info)

{

    struct SkipListNode* node = node_init(flip_coin(), value, info);

    self->maxHeight = (self->maxHeight >= node->height) ? self->maxHeight : node->height;

    // if self->head->height < self->maxHeight, extend the head

    struct SkipListNode** update = updateListInfo(self, value, info);

    for(int i = 0; i<node->height; i++)

    {

        node->next[i] = update[i]->next[i];

        update[i]->next[i] = node;

    }

    self->len++;

    return 0;

}



// remove node with value from the list

// uses updateList

// return 0 on failure, info on success

int* delete(struct SkipList* self, float value)

{

	  struct SkipListNode** update = updateList(self, value);

    struct SkipListNode* x = find(self, value, update);

    if(x != NULL)

    {

        for(int i = x->height-1; i > -1; i--)

        {

            update[i]->next[i] = x->next[i];

            if(self->head->next[i] == NULL)

            {

                self->maxHeight--;

            }

        }

        self->len--;

        return x->info;

    }

    return NULL;

}





// find route to node, by value, through all levels

// return array with last nodes at each level such that node.value <= value at each level

struct SkipListNode** updateListEnd(struct SkipList* self, float value)

{

    struct SkipListNode** path = (struct SkipListNode**)calloc(self->maxHeight, sizeof(struct SkipListNode*));

    struct SkipListNode* x = self->head;

    for(int i = self->maxHeight-1; i>-1; i--)

    {

        while(x->next[i] != NULL && x->next[i]->value <= value)

        {

            x = x->next[i];

        }

        path[i] = x;

    }

    return path;

}



// find a node by value 

// uses updateListEnd (<=) to find path

// return pointer to node

struct SkipListNode* findEnd(struct SkipList* self, float value, struct SkipListNode** update)

{

    if(update == NULL)

    {

        update = updateListEnd(self, value);

    }

    struct SkipListNode* candidate = update[0]->next[0];

    if(candidate != NULL)

    {

        if(candidate->value == value)

        {

            return candidate;

        }

    }

    return NULL;

}



// find a node by info 

// (return pointer to node)

struct SkipListNode* find_by_name(struct SkipList* self, float value, int* node_info, struct SkipListNode** update)

{

    if(update == NULL)

    {

        update = updateListInfo(self, value, node_info);

    }

    struct SkipListNode* candidate = update[0]->next[0];

    if(candidate != NULL && candidate->value == value && candidate->info == node_info)

    {

        return candidate;

    }

    return NULL;

}



// delete node with value and info from the list

// return 1 on success, 0 if node not found

int delete_by_name(struct SkipList* self, float value, int* node_info, struct SkipListNode** update)

{

    value = value*(self->swtch);

    if(update == NULL)

    {

        update = updateListInfo(self, value, node_info);

    }

    struct SkipListNode* x = find_by_name(self, value, node_info, update);

    if(x != NULL)

    {

        for(int i = x->height-1; i > -1; i--)

        {

            update[i]->next[i] = x->next[i];

            if(self->head->next[i] == NULL)

            {

                self->maxHeight--;

            }

        }

        self->len--;

        return 1;

    }

    return 0;

}



// print out full list

// return nothing

void print_list(struct SkipList* self)

{

    for(int i = self->maxHeight-1; i>-1; i--)

    {

        printf("level: %d", i);

        struct SkipListNode* curr = self->head->next[i];

        while(curr != NULL)

        {

            printf("%d -> ", curr->value);

            // printf(curr->info);

            curr = curr->next[i];

        }

        printf("\n");

    }

		return;

}



// return node at index in list

// NULL if not found

struct SkipListNode* __getitem__(struct SkipList* self, int index)

{

    assert(index>=0);

    assert(index<self->len);

    struct SkipListNode* x = self->head;

    int i = -1;

    while(true)

    {

        if(i==index)

        {

            return x;

        }

        if(x->next[0] == NULL)

        {

            break;

        }

        i++;

        x = x->next[0];

    }

    return NULL;

}



// delete random node with given value

// uses updateList and updateListEnd to find upper and lower bounds

// return value and info of deleted node 

// (just info for now until I figure out a good way to do this)

int* deleteRand(struct SkipList* self, float value, struct SkipListNode* start, struct SkipListNode* end)

{

    if(start == NULL)

    {

        start = updateList(self, value)[0]->next[0];

    }

    if(end == NULL)

    {

        end = updateListEnd(self, value)[0];

    }

    srand(time(NULL));

    int max0 = (start->info[0] >= end->info[0])? start->info[0] : end->info[0];

    int min0 = (start->info[0] <= end->info[0])? start->info[0] : end->info[0];

    int diff0 = max0-min0;

    int max1 = (start->info[1] >= end->info[1])? start->info[1] : end->info[1];

    int min1 = (start->info[1] <= end->info[1])? start->info[1] : end->info[1];

    int diff1 = max1 - min1;

    int g1_info = min0 + rand()%(diff0);

    int g2_info = min1 + rand()%(diff1);

    int g_info[2] = {g1_info, g2_info};



    struct SkipListNode** update = updateListInfo(self, value, g_info);



    struct SkipListNode* cand1 = (update[0] && update[0]->info != NULL) ? update[0] : update[0]->next[0];



    delete_by_name(self, (self->swtch)*(cand1->value), cand1->info, NULL);

    return cand1->info;

}



// delete a random node in the range (node[0].value, node[0].value+domain)

// use deleteRand once the random value has been selected

// return the same as deleteRand

int* popUniform(struct SkipList* self, float domain)

{

    assert(self->len > 0);

    srand(time(NULL));

    float rand_value = self->head->next[0]->value+ domain*(float)rand()/(float)(RAND_MAX);

    struct SkipListNode* cand1 = updateList(self,rand_value)[0];

    struct SkipListNode* cand2 = updateListEnd(self,rand_value)[0];



		int* ret;



    if (cand1 && cand2 && cand1->next[0] && cand1->next[0]->value == cand2->value)

    {

        ret = deleteRand(self, cand1->value, cand1, cand2);

    }

    else

    {

        if(cand2 && abs(cand2->value-rand_value) < abs(cand1->value-rand_value))

        {

            ret = deleteRand(self, cand2->value, cand2, NULL);

        }

        else

        {

            ret = deleteRand(self, cand1->value, NULL, cand1);

        }

    }

    return ret;

}



// pop first one that works for a randomly generated value

int* popfast(struct SkipList* self, float domain)

{

    assert(self->len > 0);

    srand(time(NULL));

    float rand_value = self->head->next[0]->value+ domain*(float)rand()/(float)(RAND_MAX);

    if(__contains__(self, rand_value))

    {

        int* info = delete(self, rand_value);

        return info;

    }

    else

    {

        struct SkipListNode** update = updateList(self, rand_value);

        struct SkipListNode* candidate1 = update[0];

        struct SkipListNode* candidate2 = update[0]->next[0];



        if(candidate2 == NULL)

        {

            int* info = delete(self, candidate1->value);

            return info;

        }

        if(abs(candidate1->value-rand_value)<=abs(candidate2->value-rand_value))

        {

            int* info = delete(self, candidate1->value);

            return info;

        }

        if(abs(candidate2->value-rand_value)<abs(candidate1->value-rand_value))

        {

            int* info = delete(self, candidate2->value);

            return info;

        }

        return NULL;

    }

}

// if fast, popfast(domain)

// else, popUniform(domain)

// return whatever the relevant function returns

// (going to make other pop functions return info for now)

int* pop(struct SkipList* self, float domain, bool fast)

{

    if(fast)

    {

        return popfast(self, domain);

    }

    else

    {

        return popUniform(self, domain);

    }

}





// another removal algorithm

int* opop(struct SkipList* self, float domain)

{

    assert(self->len > 0);

    float tail = self->head->next[0]->value + domain;

    struct SkipListNode* temp = self->head->next[0];

    float choice_value = temp->value;

    int* choice_info = temp->info;



    int n = 0;

    

    srand(time(NULL));

    while(temp && temp->value < tail)

    {

        if(temp->next[0] == NULL)

        {

            break;

        }

        temp = temp->next[0];

        n++;



        int j = rand()%(n+2);

        if(j==0)

        {

            choice_value = temp->value;

            choice_info = temp->info;

        }

    }

    delete(self, choice_value);

    return choice_info;

}



// TODO: figure out a good way to do this in C

// return list as a string

// treat as regular linked list:

// only worry about bottom level

char* __str__(struct SkipList* self)

{

    return NULL;

}





int main()

{

    return 0;

}

