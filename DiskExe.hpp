#ifndef TRAIN_TICKET_DISKMANAGER_HPP
#define TRAIN_TICKET_DISKMANAGER_HPP
#include <cstring>
#include <fstream>
#include "HashMap.hpp"

using namespace std;

template <class T,class basicInfo>
class DiskExe{
private:
    const string fileName;
    int nowPtr,nextPtr; //文件链表位置
    fstream file;
private:
    class cacheList{
    public:
        struct Node{
            Node *pre,*nxt;
            T *data;
            int position;
            bool isWriten = false;

            Node() = delete;

            Node(Node * _bef,Node * _aft,const T & _data,int _pos):pre(_bef),nxt(_aft),data(new T(_data)),position(_pos),isWriten(false){}

            Node(Node * _bef,Node * _aft):pre(_bef),nxt(_aft),data(nullptr),position(-1),isWriten(false){}

            ~Node(){delete data;}
        };

        int capacity;
        Node * head;Node * tail;
        int dataSize;
        DiskExe * theDisk;
    private:
        void write_back(int _pos,T * _data){
            theDisk->file.seekp(_pos,ios::beg);
            theDisk->file.write(reinterpret_cast<const char *>(_data),sizeof(*_data));
        }
        void pop_back(){
            Node * tmp = tail->pre;
            if(tmp->isWriten) this->write_back(tmp->position,tmp->data);
            tmp->pre->nxt = tmp->nxt;
            tmp->nxt->pre = tmp->pre;
            theDisk->assistantMap.erase(tmp->position);
            --dataSize;
            delete tmp;
        }
        Node * push_front(int _pos,const T & _data){
            Node * tmp = new Node(head,head->nxt,_data,_pos);
            tmp->nxt->pre = tmp;
            head->nxt = tmp;
            ++dataSize;
            if(dataSize == capacity) pop_back();
            return tmp;
        }
    public:
        cacheList() = delete;
        explicit cacheList(int _capacity,DiskExe * _ptr):theDisk(_ptr),capacity(_capacity),dataSize(0),head(new Node(nullptr, nullptr)),tail(new Node(nullptr, nullptr)){
            head->nxt = tail,tail->pre = head;
        }
        ~cacheList(){
            Node *tmp = head;
            theDisk->file.open(theDisk->fileName,ios::in | ios::out | ios::binary);
            while(tmp != nullptr){
                if(tmp->position != -1){
                    if(tmp->isWriten) this->write_back(tmp->position,tmp->data);
                }
                Node * tmpp = tmp;
                tmp = tmp->nxt;
                delete tmpp;
            }
        }
        Node * insert(int _pos,const T & _data){
            return this->push_front(_pos,_data);
        }
        void update(Node *tarNode){
            if(head->nxt == tarNode) return;
            tarNode->pre->nxt = tarNode->nxt;
            tarNode->nxt->pre = tarNode->pre;
            tarNode->pre = head;
            tarNode->nxt = head->nxt;
            head->nxt->pre = tarNode;
            head->nxt = tarNode;
        }
        void erase(Node * tarNode){
            --dataSize;
            tarNode->pre->nxt = tarNode->nxt;
            tarNode->nxt->pre = tarNode->pre;
            delete tarNode;
        }
        void clear(){
            Node *p = head;
            while(p){
                Node * q = p;
                p = p->nxt;
                delete q;
            }
            head = new Node(nullptr, nullptr);
            tail = new Node(nullptr, nullptr);
            head->nxt = tail,tail->pre = head;
            dataSize = 0;
        }
    };
    cacheList cache;
    HashMap<int,typename cacheList::Node *> assistantMap;
public:
    DiskExe() = delete;
    explicit DiskExe(const string & _name,int _capacity):fileName(_name),cache(_capacity,this),assistantMap(_capacity){
        file.open(fileName,ios::in);
        if(file.fail()){
            file.clear();
            file.open(fileName,ios::out);
            file.close();
            file.open(fileName,ios::out | ios::in | ios::binary);
            nowPtr = -1,nextPtr = -1;
            basicInfo temp;
            file.seekp(0,ios::beg);
            file.write(reinterpret_cast<const char *>(&temp),sizeof(temp));
            file.seekp(sizeof(temp),ios::beg);
            file.write(reinterpret_cast<const char *>(&nowPtr),sizeof(nowPtr));
        }else{
            file.close();
            file.open(fileName,ios::in | ios::out | ios::binary);
            file.seekp(sizeof(basicInfo),ios::beg);
            file.read(reinterpret_cast<char *>(&nowPtr),sizeof(nowPtr));
            if(nowPtr > 0){
                file.seekp(nowPtr,ios::beg);
                file.read(reinterpret_cast<char *>(&nextPtr),sizeof(nextPtr));
            }else nextPtr = -1;
        }
    }
    ~DiskExe(){
        file.seekp(sizeof(basicInfo),ios::beg);
        file.write(reinterpret_cast<const char *>(&nowPtr),sizeof(nowPtr));
        file.close();
    }

public:
    int write(const T & data){
        int temp;
        if(nowPtr < 0){
            file.seekp(0,ios::end);
            temp = file.tellp();
            file.write(reinterpret_cast<const char *>(&data),sizeof(data));
            return temp;
        }else{
            temp = nowPtr;
            file.seekp(temp,ios::beg);
            file.write(reinterpret_cast<const char *>(&data),sizeof(data));
            if(nowPtr < 0) return temp;
            nowPtr = nextPtr;
            if(nextPtr > 0){
                file.seekp(nextPtr,ios::beg);
                file.read(reinterpret_cast<char * >(&nextPtr),sizeof(nextPtr));
            }
            return temp;
        }
    }
    void write(const T * data,int position){
        if(assistantMap.exist(position)){
            assistantMap.find(position)->isWriten = true;
        }else {
            if (file.fail()) {
                file.clear();
                file.close();
                file.open(fileName, ios::in | ios::out | ios::binary);
            }
            file.seekp(position, ios::beg);
            file.write(reinterpret_cast<const char *>(data), sizeof(*data));
        }
    }
    T * read(int index,bool mode = false) {
        if(assistantMap.exist(index)){
            typename cacheList::Node * tmp = assistantMap.find(index);
            cache.update(tmp);
            if(mode) tmp->isWriten = true;
            return tmp->data;
        }else {
            T temp;
            file.seekp(index, ios::beg);
            file.read(reinterpret_cast< char *>(&temp), sizeof(temp));
            typename cacheList::Node * tmp = cache.insert(index,temp);
            assistantMap.insert(index,tmp);
            if(mode) tmp->isWriten = true;
            return tmp->data;
        }
    }

    void erase(int index){
        if(assistantMap.exist(index)){
            cache.erase(assistantMap.find(index));
            assistantMap.erase(index);
        }
        if(nowPtr < 0){
            file.seekp(index,ios::beg);
            file.write(reinterpret_cast<const char *>(&nowPtr),sizeof(nowPtr));
            nowPtr = index;
            nextPtr = -1;
            return;
        }else{
            nextPtr = nowPtr;
            file.seekp(index,ios::beg);
            file.write(reinterpret_cast<const char *>(&nowPtr),sizeof(nowPtr));
            nowPtr = index;
            return;
        }
    }

    basicInfo tellInfo(){
        basicInfo temp;
        file.seekp(0,ios::beg);
        file.read(reinterpret_cast< char *>(&temp),sizeof(temp));
        return temp;
    }
    int tellp(){
        if(file.fail()){
            file.clear();
            file.close();
            file.open(fileName,ios::out | ios::in | ios::binary);
        }
        if(nowPtr >= 0) return nowPtr;
        else{
            file.seekp(0,ios::end);
            return file.tellp();
        }
    }
    void setInfo(const basicInfo & info){
        file.seekp(0,ios::beg);
        file.write(reinterpret_cast<const char *>(&info),sizeof(info));
    }
    void clear(){
        file.close();
        file.open(fileName,ios::out);
        file.close();
        file.open(fileName,ios::in | ios::out);
        nowPtr = -1;nextPtr = -1;
        basicInfo temp;
        file.seekp(0,ios::beg);
        file.write(reinterpret_cast<const char *>(&temp),sizeof(temp));
        file.seekp(sizeof(temp),ios::beg);
        file.write(reinterpret_cast<const char *>(&nowPtr),sizeof(nowPtr));
        assistantMap.clear();
        cache.clear();
    }

};
#endif //TRAIN_TICKET_DISKMANAGER_HPP
