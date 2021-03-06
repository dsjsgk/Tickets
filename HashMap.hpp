#ifndef TRAIN_TICKET_HASHMAP_HPP
#define TRAIN_TICKET_HASHMAP_HPP

#include <functional>
template <class Key,class Data,class Hash = std::hash<Key>>
class HashMap{
private:
    class LinkList{
    private:
        struct Node{
            Node * next;
            Key key;
            Data data;

            Node() = delete;
            Node(Node * _next,const Key & _key,const Data & _data):next(_next),key(_key),data(_data){}
        };
        int dataSize;
        Node *head;
    public:
        LinkList():dataSize(0),head(nullptr){}

        ~LinkList(){
            while(head != nullptr){
                Node * tmp = head;
                head = head->next;
                delete tmp;
            }
        }

        void insert(const Key & _key,const Data & _data){
            head = new Node(head,_key,_data);
            ++dataSize;
        }

        Node * find(const Key & _key){
            if(dataSize == 0) return nullptr;
            Node * q = head;
            while(q != nullptr){
                if(q->key == _key) return q;
                q = q->next;
            }
            return nullptr;
        }

        void erase(const Key & _key){
            if(dataSize == 0) return;
            if(head->key == _key){
                Node * tmp = head;
                head = head->next;
                delete tmp;
                --dataSize;
                return;
            }
            Node * p = nullptr; Node * q = head;
            while(q != nullptr){
                if(q->key == _key) break;
                p = q; q = q->next;
            }
            p->next = q->next;
            delete q;
            --dataSize;
        }
    };

    int capacity;
    LinkList * dataSet;
    Hash hash;

    int getIndex(const Key & _key){
        return hash(_key) % capacity;
    }
public:
    HashMap() = delete;

    explicit HashMap(int _capacity):capacity(_capacity){
        dataSet = new LinkList[_capacity];
    }

    ~HashMap(){
        delete [] dataSet;
    }

    bool exist(const Key & _key){
        int index = getIndex(_key);
        return (dataSet[index].find(_key) != nullptr);
    }

    // ????????????????????????key
    void insert(const Key & _key,const Data & _data){
        int index = getIndex(_key);
        dataSet[index].insert(_key,_data);
    }

    void erase(const Key & _key){
        int index = getIndex(_key);
        dataSet[index].erase(_key);
    }

    // ????????????key
    Data & find(const Key & _key){
        int index = getIndex(_key);
        return dataSet[index].find(_key)->data;
    }

    Data & operator [](const Key & _key){
        return this->find(_key);
    }

    void clear(){
        delete [] dataSet;
        dataSet = new LinkList[capacity];
    }
};
#endif
