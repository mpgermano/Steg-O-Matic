
#ifndef Project_4_HashTable_h
#define Project_4_HashTable_h

#include <iostream>
using namespace std;

template <typename KeyType,	typename ValueType>
class HashTable
{
public:
    HashTable(unsigned int numBuckets,	unsigned int capacity);
    ~HashTable();
    bool isFull()	const;
    bool set(const KeyType&	key, const ValueType& value, bool permanent = false);
    bool get(const KeyType& key,	ValueType& value) const;
    bool touch(const KeyType& key);
    bool discard(KeyType& key,	ValueType& value);

private:
    
    struct Node
    {
    public:
        ValueType m_value;
        KeyType m_key;
        Node* nextInBucket;
        Node* nextInList;       //pointer to the Node one spot more recent than the current node
        Node* prevInList;       //pointer to the Node one spot less recent than the current node
        bool m_Permanent;
    };
				//	We	prevent a	HashTable from	being	copied	or	assigned	by	declaring	the
				//	copy	constructor	and	assignment	operator	private	and	not	implementing	them.
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
    void updateOrder(Node* ptr);
    unsigned int getBucketNumber(const KeyType& key) const;         //used to compute the bucket number
    
    Node** hTable;
    Node* mostRecent;
    Node* leastRecent;
     
    unsigned int nBuckets;
    unsigned int m_cap;
    unsigned int m_nItems;
    unsigned int m_nNonPerm;
    
};

template <typename KeyType>
unsigned int computeHash(const KeyType&	key);   //delete this eventually

template <typename KeyType,	typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
: nBuckets(numBuckets), m_cap(capacity), m_nItems(0), m_nNonPerm(0), mostRecent(nullptr),
        leastRecent(nullptr)
{
    hTable = new Node*[nBuckets];       //dynamically allocate array with given stated size of buckets
    for (int k = 0; k < nBuckets; k++)
    {
        hTable[k] = nullptr;        //fill each spot with the nullptr
    }
}


template <typename KeyType,	typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
    Node* temp;
    Node* ptr;
    for (int k = 0; k < nBuckets; k++)      //cycle to each bucket
    {
        ptr = hTable[k];
        while (ptr != nullptr)      //delete all dynamically allocated nodes in each bucket
        {
            temp = ptr;
            ptr = ptr->nextInBucket;
            delete temp;
        }
    }
  
    delete [] hTable;  //delete the dynamically allocated array of pointers to Nodes

}


template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::isFull() const
{
    return m_nItems >= m_cap;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType&	key,const ValueType& value, bool permanent)
{
    unsigned int bucket = getBucketNumber(key);
    
    Node* loop = hTable[bucket];
    Node* secPtr = hTable[bucket];
    
    for ( ; loop != nullptr; loop = loop->nextInBucket)     //loop needed to check for collisions
    {
        if (loop->m_key == key)     //find the correct node and update its value
        {
            loop->m_value = value;
            if (!loop->m_Permanent)
            {
                updateOrder(loop);      // if the loop isn't permanent update the recently updated list
            }
            return true;
        }
        
        secPtr = loop;      //this pointer is needed to connect a new node to the list of the bucket
    }
    
    //otherwise that key doesn't exist and a new assocation must be made
    
    if (isFull())       //can't add a new association to a full hashTable
        return false;
  
    Node* tempPtr = new Node;
    tempPtr->m_key = key;
    tempPtr->m_value = value;
    tempPtr->m_Permanent = permanent;
    tempPtr->nextInBucket = nullptr;        //newest node doesn't have a node ahead in its bucket
    
   if (permanent)
   {
       tempPtr->nextInList = nullptr;
       tempPtr->prevInList = nullptr;
   }
    
    if (!permanent)
    {
        tempPtr->nextInList = nullptr;          //most recent node doesn't have a more recent node ahead
        tempPtr->prevInList = mostRecent;       //the old mostRecent node is the newest node's prev Node
        
        mostRecent = tempPtr;
        
        if (m_nNonPerm == 0)
            leastRecent = tempPtr;      //if this is the first non-permanent node, then leastRecent is set
        else
            tempPtr->prevInList->nextInList = tempPtr;  //otherwise, the need node is connected to the list
        
        m_nNonPerm++;

    }

    
    //the following code connects the new node to the list in its bucket
    if (secPtr == nullptr)
        hTable[bucket] = tempPtr;
    
    else
        secPtr->nextInBucket = tempPtr;
    
   
    m_nItems++;
    return true;
}


template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key,	ValueType& value) const
{
    unsigned int bucket = getBucketNumber(key);
    
    Node* loop = hTable[bucket];
    for ( ; loop != nullptr; loop = loop->nextInBucket)     //used to check for collisions
    {
        if (loop->m_key == key)
        {
            value = loop->m_value;
            return true;
        }
    }
    
    return false;       //Node doesn't exist in the HashTable
}
template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
    unsigned int bucket = getBucketNumber(key);
    
    Node* loop = hTable[bucket];
    for ( ; loop != nullptr; loop = loop->nextInBucket)
    {
        if (loop->m_key == key && !loop->m_Permanent)
        {
            updateOrder(loop);      //find the node and update the recently updated list's order
            return true;
        }

    }
    return false;       //Node doesn't exist in the HashTable
}
template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
  if (leastRecent == nullptr)       //if there are no non-permenant items, return false
      return false;
  
    
    key = leastRecent->m_key;
    value = leastRecent->m_value;
    
    unsigned int bucket = getBucketNumber(key);
    
    Node* toDelete = hTable[bucket];
    Node* prevPtr = toDelete;
    
    
    for ( ; toDelete != leastRecent; toDelete = toDelete->nextInBucket)     //find the to-be-deleted Node
        prevPtr = toDelete;
    
    if (toDelete == prevPtr)        //the to-be-deleted Node is the first Node in the bucket
        hTable[bucket] = toDelete->nextInBucket;
    
    else
        prevPtr->nextInBucket = toDelete->nextInBucket; //connect bucket's list around to-be-deleted node
    
    
    Node* tempPtr = leastRecent->nextInList;
    
    
    delete toDelete;        //delete dynamically allocated node
    m_nItems--;
    m_nNonPerm--;
    
    leastRecent = tempPtr;
    if (leastRecent != nullptr)     //checks to ensure the deleted node wasn't the last non-permanent node
        leastRecent->prevInList = nullptr;
    else
        mostRecent = nullptr;
    
    return true;
}

template <typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::updateOrder(HashTable::Node* ptr)       //crucial function
{
    if (m_nNonPerm == 1)
        return;
    
    else if (m_nNonPerm == 2 && ptr->nextInList != nullptr)     //code switches the pointers between
    {                                                           //the two nodes as well as updating
        ptr->nextInList->nextInList = ptr;                      //the mostRecent and leastRecent ptrs.
        ptr->prevInList = ptr->nextInList;
        ptr->nextInList = nullptr;
        ptr->prevInList->prevInList = nullptr;
        mostRecent = ptr;
        leastRecent = ptr->prevInList;
    }
    
    else if (m_nNonPerm >= 3 && ptr->nextInList != nullptr)
    {
        if (ptr == leastRecent)     //if the leastRecent node is being moved forward, the pointer must be
        {                           //adjusted accordingly
            leastRecent = ptr->nextInList;
            ptr->nextInList->prevInList = nullptr;          //this code updates the new, least recent node,
            ptr->nextInList = nullptr;                      //moves the updated node to the top
            ptr->prevInList = mostRecent;                   //and connects it to to the previous mostRecent
            mostRecent->nextInList = ptr;
            mostRecent = ptr;
        }
        else
        {
            ptr->nextInList->prevInList = ptr->prevInList;      //this code connects the two surrounding
            ptr->prevInList->nextInList = ptr->nextInList;      //nodes together, then it moves the node
            ptr->nextInList = nullptr;                          //to the top and connects it properly
            ptr->prevInList = mostRecent;
            mostRecent->nextInList = ptr;
            mostRecent = ptr;
        }
    }
    
}

template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::getBucketNumber(const KeyType& key) const
{
    unsigned int computeHash(KeyType key);      // prototype
    unsigned int result = computeHash(key);     //calls the appropriate computeHash function for the KeyType
    unsigned int bucketNum = result % nBuckets; //gets the appropriate bucket number
    
    return bucketNum;
}

#endif

