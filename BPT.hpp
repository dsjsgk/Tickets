#ifndef TRAIN_TICKET_BPLUSTREE_HPP
#define TRAIN_TICKET_BPLUSTREE_HPP
#include "DiskExe.hpp"
#include <cstring>
#include <vector>
#include <iostream>

namespace functionn{
    //返回第一个大于目标的key
    template<class T>
    int upper_bound(const T * const area,int size,const T & key){
        if(!size) return 0;
        if(key < area[0]) return 0;
        if(area[size-1] < key || area[size-1] == key) return size;
        int l = 0,r = size - 1;
        while(l < r){
            int mid = (l + r) >> 1;
            if(key < area[mid]) r = mid;
            else l = mid + 1;
        }
        return l;
    }

    //返回第一个大于或等于目标的key
    template<class T>
    int lower_bound(const T * const area,int size,const T & key){
        if(!size) return 0;
        if(key < area[0] || key == area[0]) return 0;
        if(area[size-1] < key) return size - 1;
        int l = 0,r = size - 1;
        while(l < r){
            int mid = (l + r + 1) >> 1;
            if(area[mid] < key) l = mid;
            else r = mid - 1;
        }
        return l;
    }
}
//请实现KEY的" < "，" == "重载函数
using namespace functionn;
using namespace std;

//b+树空间回收池，类栈
class recyclePool{
public:
    int data[200] = {0};
    int top;

    recyclePool():top(0){}

    void push_back(int pos){data[++top] = pos;}

    int pop_back(){return data[top--];}

    void clear(){top = 0;}
};

template <class Key,class Data,int Cap1 = 379,int Cap2 = 15372>
class BPT{
private:
    /**非叶结点最大儿子数M，叶结点最大数据量L**/
    static constexpr int M = 115;
    static constexpr int L = 115;

    /**叶结点定义**/
    class leafNode{
    public:
        int position = -1;
        int father = -1;
        int leftPos = -1;
        int rightPos = -1;
        int dataSize = 0;
        Key KEY[L];
        Data DATA[L];

        void split(BPT *theTree){
            leafNode newNode;
            newNode.dataSize = L - L/2,newNode.position = theTree->leafDisk.tellp();
            newNode.father = father,newNode.leftPos = position,newNode.rightPos = rightPos;
            if(rightPos >= 0){
                leafNode * right = theTree->leafDisk.read(rightPos);
                right->leftPos = newNode.position;
                theTree->leafDisk.write(right,rightPos);
            }
            rightPos = newNode.position;
            for(int i = 0;i < newNode.dataSize;++i){
                newNode.KEY[i] = KEY[L/2 + i];
                newNode.DATA[i] = DATA[L/2 + i];
            }
            dataSize = L/2;
            theTree->leafDisk.write(newNode),theTree->leafDisk.write(this,position);
            //更新父结点
            nonLeafNode * fatherNode = theTree->nodeDisk.read(newNode.father);
            fatherNode->addElement(newNode.KEY[0],newNode.position,theTree);
        }

        void addElement(const Key & _key,const Data & _data,BPT * theTree){
            int addPos = upper_bound(KEY,dataSize,_key);
            for(int i = dataSize;i > addPos;--i){
                KEY[i] = KEY[i - 1];
                DATA[i] = DATA[i - 1];
            }
            KEY[addPos] = _key,DATA[addPos] = _data;
            dataSize++;
            if(dataSize == L){split(theTree);return;}//裂块
            theTree->leafDisk.write(this,position);
        }

        //如果想要存指针地址，请再写一个容器为 vector<Data*>& vec_ans 的findElement函数
        //方向flag：1向左找，2向右找，初始为0
        void findElement(const Key & _key,vector<Data>& vec_ans,BPT *theTree,int flag){
            int frontPos = lower_bound(KEY,dataSize,_key);
            int behindPos = upper_bound(KEY,dataSize,_key);
            for(int i = min(behindPos,dataSize - 1);i >= frontPos;--i){
                if(KEY[i] == _key) vec_ans.push_back(DATA[i]);
            }
            if(KEY[0] == _key && leftPos != -1 && flag != 2){
                leafNode * left = theTree->leafDisk.read(leftPos);
                left->findElement(_key,vec_ans,theTree,1);
            }
            if(KEY[dataSize-1] == _key && rightPos != -1 && flag != 1){
                leafNode * right = theTree->leafDisk.read(rightPos);
                right->findElement(_key,vec_ans,theTree,2);
            }
        }

        void findNextElement(const Key & _key,BPT *theTree,Data &tar,int &flag){
            int Pos = -1;
            for(int i = 0;i < dataSize; ++i){
                if(KEY[i] < _key)Pos = i;
                else if(KEY[i] == _key){Pos = i;flag = 1;}
            }
            if(Pos == dataSize - 1){
                if(rightPos == -1) { flag = 2;return; }
                leafNode * right = theTree->leafDisk.read(rightPos);
                tar = right->DATA[0];
                return;
            }else{
                tar = DATA[Pos + 1];
                return;
            }

        }

        void FindNextElement(const Key & _key,BPT *theTree,int &tarPos,int &flag,leafNode *&tarLeaf){
            int Pos = -1;
            for(int i = 0;i < dataSize; ++i){
                if(KEY[i] < _key)Pos = i;
                else if(KEY[i] == _key){Pos = i;flag = 1;}
            }
            if(Pos == dataSize - 1){
                if(rightPos == -1) { flag = 2;tarPos = Pos;return; }
                tarLeaf = theTree->leafDisk.read(rightPos);
                tarPos = 0;
                return;
            }else{
                tarPos = Pos + 1;
                return;
            }

        }

        //方向flag：1向左找，2向右找，初始为0
        void findDeletePos(const Key & _key,const Data & _data,BPT * theTree,int flag,int & leafPos,int & keyPos){
            int frontPos = lower_bound(KEY,dataSize,_key);
            int behindPos = upper_bound(KEY,dataSize,_key);
            for(int i = frontPos;i <= behindPos && i < dataSize;++i){
                if(KEY[i] == _key && DATA[i] == _data){
                    leafPos = position,keyPos = i;
                    return;
                }
            }
            if(KEY[0] == _key && leftPos != -1 && flag != 2 && leafPos == -1){
                leafNode * left = theTree->leafDisk.read(leftPos);
                left->findDeletePos(_key,_data,theTree,1,leafPos,keyPos);
            }
            if(KEY[dataSize-1] == _key && rightPos != -1 && flag != 1 && leafPos == -1){
                leafNode * right = theTree->leafDisk.read(rightPos);
                right->findDeletePos(_key,_data,theTree,2,leafPos,keyPos);
            }
        }

        bool borrowLeft(BPT * theTree){
            if(leftPos == -1) return false;
            leafNode * leftBro = theTree->leafDisk.read(leftPos);
            if(leftBro->father != father) return false;
            if(leftBro->dataSize > L/2){ // borrow one record from left brother
                for(int i = this->dataSize;i > 0;--i){
                    KEY[i] = KEY[i-1];
                    DATA[i] = DATA[i-1];
                }
                KEY[0] = leftBro->KEY[leftBro->dataSize-1],DATA[0] = leftBro->DATA[leftBro->dataSize-1];
                ++dataSize;--leftBro->dataSize;
                nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
                int pos = fatherNode->findKeyPos(leftBro->position);
                fatherNode->INDEX[pos] = KEY[0];
                theTree->leafDisk.write(this,position);
                theTree->leafDisk.write(leftBro,leftBro->position);
                theTree->nodeDisk.write(fatherNode,fatherNode->position);
                return true;
            }else
                return false;
        }

        bool borrowRight(BPT * theTree){
            if(rightPos == -1) return false;
            leafNode * rightBro = theTree->leafDisk.read(rightPos);
            if(rightBro->father != father) return false;
            if(rightBro->dataSize > L/2){
                KEY[dataSize] = rightBro->KEY[0];
                DATA[dataSize] = rightBro->DATA[0];
                for(int i = 0;i < rightBro->dataSize - 1;++i){
                    rightBro->KEY[i] = rightBro->KEY[i + 1];
                    rightBro->DATA[i] = rightBro->DATA[i + 1];
                }
                ++dataSize,--rightBro->dataSize;
                nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
                int pos = fatherNode->findKeyPos(position);
                fatherNode->INDEX[pos] = rightBro->KEY[0];
                theTree->leafDisk.write(this,position);
                theTree->leafDisk.write(rightBro,rightBro->position);
                theTree->nodeDisk.write(fatherNode,fatherNode->position);
                return true;
            }else
                return false;
        }

        bool mergeLeft(BPT * theTree){
            if(leftPos == -1) return false;
            leafNode * leftBro = theTree->leafDisk.read(leftPos);
            if(leftBro->father != father) return false;
            for(int i = 0;i < dataSize;++i){
                leftBro->KEY[leftBro->dataSize+i] = KEY[i];
                leftBro->DATA[leftBro->dataSize+i] = DATA[i];
            }
            leftBro->dataSize += dataSize;
            leftBro->rightPos = rightPos;
            if(~this->rightPos){
                leafNode * RRNode = theTree->leafDisk.read(rightPos);
                RRNode->leftPos = leftBro->position;
                theTree->leafDisk.write(RRNode,RRNode->position);
            }
            theTree->leafDisk.write(leftBro,leftBro->position);
            nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
            theTree->leafDisk.erase(position);
            int pos = fatherNode->findKeyPos(leftBro->position);
            fatherNode->deleteElement(pos,theTree);
            return true;
        }

        bool mergeRight(BPT *theTree){
            if(rightPos == -1) return false;
            leafNode * rightBro = theTree->leafDisk.read(rightPos);
            if(rightBro->father != father) return false;
            for(int i = 0;i < rightBro->dataSize;++i){
                KEY[dataSize+i] = rightBro->KEY[i];
                DATA[dataSize+i] = rightBro->DATA[i];
            }
            dataSize += rightBro->dataSize;
            rightPos = rightBro->rightPos;
            if(~rightBro->rightPos){
                leafNode * RRNode = theTree->leafDisk.read(rightBro->rightPos);
                RRNode->leftPos = position;
                theTree->leafDisk.write(RRNode,RRNode->position);
            }
            theTree->leafDisk.write(this,position);
            theTree->leafDisk.erase(rightBro->position);
            nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
            int pos = fatherNode->findKeyPos(position);
            fatherNode->deleteElement(pos,theTree);
            return true;
        }

        void deleteElement(int keyPos,BPT *theTree){
            for(int i = keyPos;i < dataSize - 1;++i){
                KEY[i] = KEY[i+1];
                DATA[i] = DATA[i+1];
            }
            --dataSize;
            if(keyPos == 0 && position != theTree->BasicInformation.head){
                nonLeafNode * tmpFather = theTree->nodeDisk.read(father);
                int tmp = tmpFather->findKeyPos(position);
                while(tmp == 0){
                    int _pos = tmpFather->position;
                    tmpFather = theTree->nodeDisk.read(tmpFather->father);
                    tmp = tmpFather->findKeyPos(_pos);
                }
                tmpFather->INDEX[tmp-1] = KEY[0];
                theTree->nodeDisk.write(tmpFather,tmpFather->position);
            }
            if(dataSize > L/2 - 1){
                theTree->leafDisk.write(this,position);
                return;
            }
            if(borrowLeft(theTree) || borrowRight(theTree)) return;
            if(this->mergeLeft(theTree) || mergeRight(theTree)) return;
            theTree->leafDisk.write(this,position);
        }

    };
    /**非叶结点定义**/
    class nonLeafNode{
    public:
        int position = -1;
        int father = -1;
        int leftPos = -1;
        int rightPos = -1;
        int childSize = 0;
        Key INDEX[M];
        int childPosition[M];//记录子结点在文件中位置，若儿子为叶结点，则为在叶节点文件中位置
        bool childIsLeaf = false;

        void split(BPT *theTree){
            if(position == theTree->BasicInformation.root){
                nonLeafNode newRoot;
                newRoot.position = theTree->nodeDisk.tellp(),newRoot.childSize = 1;
                newRoot.childPosition[0] = position;
                theTree->nodeDisk.write(newRoot);
                theTree->BasicInformation.root = newRoot.position;
                father = theTree->BasicInformation.root;
            }
            nonLeafNode tmpNode;
            tmpNode.father = father,tmpNode.position = theTree->nodeDisk.tellp();
            tmpNode.leftPos = position,tmpNode.rightPos = rightPos;
            tmpNode.childSize = M - M/2;
            tmpNode.childIsLeaf = childIsLeaf;
            if(rightPos >= 0){
                nonLeafNode * tmpNode1 = (theTree->nodeDisk.read(rightPos));
                tmpNode1->leftPos = tmpNode.position;
                theTree->nodeDisk.write(tmpNode1,tmpNode1->position);
            }
            rightPos = tmpNode.position;
            for(int i = 0;i < tmpNode.childSize;++i){
                tmpNode.childPosition[i] = childPosition[M/2 + i];
                if(tmpNode.childIsLeaf){
                    leafNode * modifyFather = theTree->leafDisk.read(tmpNode.childPosition[i]);
                    modifyFather->father = tmpNode.position;
                    theTree->leafDisk.write(modifyFather,modifyFather->position);
                }else{
                    nonLeafNode * modifyFather = theTree->nodeDisk.read(tmpNode.childPosition[i]);
                    modifyFather->father = tmpNode.position;
                    theTree->nodeDisk.write(modifyFather,modifyFather->position);
                }
            }
            for(int i = 0; i < tmpNode.childSize-1;++i){
                tmpNode.INDEX[i] = INDEX[M/2 + i];
            }
            childSize = M/2;
            theTree->nodeDisk.write(tmpNode);
            theTree->nodeDisk.write(this,position);
            nonLeafNode * fatherNode = (theTree->nodeDisk.read(tmpNode.father));
            fatherNode->addElement(INDEX[M/2 - 1],tmpNode.position,theTree);
        }

        void addElement(const Key & _key,int _position,BPT * theTree){
            int addPos = theTree->repos.pop_back();
            for(int i = childSize-1;i > addPos;--i) { INDEX[i] = INDEX[i - 1]; }
            for(int i = childSize;i > addPos + 1;--i) { childPosition[i] = childPosition[i - 1]; }
            INDEX[addPos] = _key;
            childPosition[addPos + 1] = _position,++childSize;
            if(childSize == M){split(theTree);return;}
            theTree->nodeDisk.write(this,position);
        }

        bool borrowLeft(BPT * theTree){
            if(leftPos == -1) return false;
            nonLeafNode * leftBro = theTree->nodeDisk.read(leftPos);
            if(leftBro->father != father) return false;
            if(leftBro->childSize > M/2){
                nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
                int keyPos = fatherNode->findKeyPos(leftBro->position);
                Key targetKey = fatherNode->INDEX[keyPos];
                fatherNode->INDEX[keyPos] = leftBro->INDEX[leftBro->childSize-2];
                theTree->nodeDisk.write(fatherNode,fatherNode->position);
                for(int i = childSize-1;i > 0;--i){
                    INDEX[i] = INDEX[i-1];
                }
                for(int i = childSize;i > 0;--i){
                    childPosition[i] = childPosition[i-1];
                }
                INDEX[0] = targetKey;
                childPosition[0] = leftBro->childPosition[leftBro->childSize-1];
                ++childSize;
                theTree->nodeDisk.write(this,position);
                --leftBro->childSize;
                theTree->nodeDisk.write(leftBro,leftBro->position);
                if(childIsLeaf){
                    leafNode * childNode = theTree->leafDisk.read(childPosition[0]);
                    childNode->father = position;
                    theTree->leafDisk.write(childNode,childNode->position);
                }else{
                    nonLeafNode * childNode = theTree->nodeDisk.read(childPosition[0]);
                    childNode->father = position;
                    theTree->nodeDisk.write(childNode,childNode->position);
                }
                return true;
            }else return false;
        }

        bool borrowRight(BPT * theTree){
            if(rightPos == -1) return false;
            nonLeafNode * rightBro = theTree->nodeDisk.read(rightPos);
            if(rightBro->father != father) return false;
            if(rightBro->childSize > M/2){
                nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
                int keyPos = fatherNode->findKeyPos(position);
                INDEX[childSize-1] = fatherNode->INDEX[keyPos];
                fatherNode->INDEX[keyPos] = rightBro->INDEX[0];
                theTree->nodeDisk.write(fatherNode,fatherNode->position);
                childPosition[childSize] = rightBro->childPosition[0];
                ++childSize;
                theTree->nodeDisk.write(this,position);
                for(int i = 0;i < rightBro->childSize - 2;++i){
                    rightBro->INDEX[i] = rightBro->INDEX[i+1];
                }
                for(int i = 0;i < rightBro->childSize - 1;++i){
                    rightBro->childPosition[i] = rightBro->childPosition[i+1];
                }
                --rightBro->childSize;
                theTree->nodeDisk.write(rightBro,rightBro->position);
                if(childIsLeaf){
                    leafNode * childNode = theTree->leafDisk.read(childPosition[childSize-1]);
                    childNode->father = position;
                    theTree->leafDisk.write(childNode,childNode->position);
                }else{
                    nonLeafNode * childNode = theTree->nodeDisk.read(childPosition[childSize-1]);
                    childNode->father = position;
                    theTree->nodeDisk.write(childNode,childNode->position);
                }
                return true;
            }else return false;
        }

        bool mergeLeft(BPT * theTree){
            if(leftPos== -1) return false;
            nonLeafNode * leftBro = theTree->nodeDisk.read(leftPos);
            if(leftBro->father != father) return false;
            nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
            int keyPos = fatherNode->findKeyPos(leftBro->position);
            Key downKey = fatherNode->INDEX[keyPos];
            leftBro->INDEX[leftBro->childSize-1] = downKey;
            for(int i = 0;i < childSize-1;++i){
                leftBro->INDEX[i+leftBro->childSize] = INDEX[i];
            }
            for(int i = 0;i < childSize;++i){
                leftBro->childPosition[i+leftBro->childSize] = childPosition[i];
                if(childIsLeaf){
                    leafNode * tmp = theTree->leafDisk.read(childPosition[i]);
                    tmp->father = leftBro->position;
                    theTree->leafDisk.write(tmp,tmp->position);
                }else{
                    nonLeafNode * tmp = theTree->nodeDisk.read(childPosition[i]);
                    tmp->father = leftBro->position;
                    theTree->nodeDisk.write(tmp,tmp->position);
                }
            }
            leftBro->childSize += childSize;
            leftBro->rightPos = rightPos;
            if(rightPos != -1){
                nonLeafNode * RRNode = theTree->nodeDisk.read(rightPos);
                RRNode->leftPos = leftBro->position;
                theTree->nodeDisk.write(RRNode,RRNode->position);
            }
            theTree->nodeDisk.write(leftBro,leftBro->position);
            theTree->nodeDisk.erase(position);
            fatherNode->deleteElement(keyPos,theTree);
            return true;
        }

        bool mergeRight(BPT *theTree){
            if(rightPos == -1) return false;
            nonLeafNode * rightBro = theTree->nodeDisk.read(rightPos);
            if(rightBro->father != father) return false;
            nonLeafNode * fatherNode = theTree->nodeDisk.read(father);
            int keyPos = fatherNode->findKeyPos(position);
            Key downKey = fatherNode->INDEX[keyPos];
            INDEX[childSize-1] = downKey;
            for(int i = 0;i < rightBro->childSize-1;++i){
                INDEX[i+childSize] = rightBro->INDEX[i];
            }
            for(int i = 0;i < rightBro->childSize;++i){
                this->childPosition[i+childSize] = rightBro->childPosition[i];
                if(childIsLeaf){
                    leafNode * tmp = theTree->leafDisk.read(rightBro->childPosition[i]);
                    tmp->father = position;
                    theTree->leafDisk.write(tmp,tmp->position);
                }else{
                    nonLeafNode * tmp = theTree->nodeDisk.read(rightBro->childPosition[i]);
                    tmp->father = position;
                    theTree->nodeDisk.write(tmp,tmp->position);
                }
            }
            childSize += rightBro->childSize;
            rightPos = rightBro->rightPos;
            if(rightBro->rightPos != -1){
                nonLeafNode * RRNode = theTree->nodeDisk.read(rightBro->rightPos);
                RRNode->leftPos = position;
                theTree->nodeDisk.write(RRNode,RRNode->position);
            }
            theTree->nodeDisk.write(this,position);
            theTree->nodeDisk.erase(rightBro->position);
            fatherNode->deleteElement(keyPos,theTree);
            return true;
        }
        void deleteElement(int keyPos,BPT * theTree){
            for(int i = keyPos;i < childSize - 2;++i){
                INDEX[i] = INDEX[i+1];
            }
            for(int i = keyPos + 1;i < childSize - 1;++i){
                childPosition[i] = childPosition[i+1];
            }
            --childSize;
            if(position == theTree->BasicInformation.root){
                if(childSize == 1 && !childIsLeaf){ // relocate the root
                    theTree->BasicInformation.root = childPosition[0];
                    nonLeafNode * newRoot = theTree->nodeDisk.read(childPosition[0]);
                    newRoot->father = -1;
                    theTree->nodeDisk.write(newRoot,newRoot->position);
                    return;
                }else{
                    theTree->nodeDisk.write(this,position);
                    return;
                }
            }
            if(childSize > M/2 - 1){
                theTree->nodeDisk.write(this,position);
                return;
            }
            if(borrowLeft(theTree) || borrowRight(theTree)) return;
            if(mergeLeft(theTree) || mergeRight(theTree)) return;
            theTree->nodeDisk.write(this,position);
        }

        int findKeyPos(int tarPos){
            //遍历即可
            for(int i = 0; i < childSize;++i)
                if(childPosition[i] == tarPos)return i;
            //并未找到
            cerr << "can not find the position";
        }
    };

    //b+树基本信息，包括根结点位置，b+树大小，叶结点表头
    class basicInfo{
    public:
        int root = -1;
        int size = 0;

        int head = -1;//叶结点表头

        void setInfo(int _root,int _head,int _size){
            root = _root,head = _head,size = _size;
        }
    };

public:
    class iterator{
        friend BPT;
        friend leafNode;
//    private:
    public:
        int pos = 0;
        bool isEnd = false;
        BPT *theTree = nullptr;
        leafNode *node = nullptr;

    public:
        iterator() = default;

        iterator(const iterator &obj) = default;

        iterator &operator=(const iterator &obj) = default;

        iterator(BPT *_tree,leafNode *_node,int _pos,bool _isEnd)
                :pos(_pos),theTree(_tree),node(_node),isEnd(_isEnd){}

        bool end(){return isEnd;}

        bool begin(){return pos == 0 && node->position == theTree->BasicInformation.head;}

        iterator& operator++(){
            if(isEnd)return *this;
            if(pos < node->dataSize - 1)return *this(theTree,node,pos + 1,false);
            else{
                if(node->rightPos == -1)return *this(theTree,node,pos,true);
                leafNode *right = theTree->leafDisk.read(node->rightPos);
                return *this(theTree,right,0,false);
            }
        }

        iterator operator++(int){
            if(isEnd)return *this;
            iterator tmp(*this);
            if(pos < node->dataSize - 1){
                pos++;
                return tmp;
            }else{
                if(node->rightPos == -1){
                    isEnd = true;
                    return tmp;
                }else{
                    node = theTree->leafDisk.read(node->rightPos);
                    pos = 0;
                    return tmp;
                }
            }
        }

        iterator& operator--(){
            if(isEnd)return *this;
            if(pos > 0)return *this(theTree,node,pos - 1,false);
            else{
                if(node->leftPos == -1)return *this;
                leafNode *left = theTree->leafDisk.read(node->leftPos);
                return *this(theTree,left,left->dataSize - 1,false);
            }
        }

        iterator operator--(int){
            if(isEnd)return *this;
            iterator tmp(*this);
            if(pos > 0){
                pos--;
                return tmp;
            }else{
                if(node->leftPos == -1){
                    return tmp;
                }else{
                    leafNode *left = theTree->leafDisk.read(node->leftPos);
                    node = left,pos = left->dataSize - 1;
                    return tmp;
                }
            }
        }

        Key key()const{return node->KEY[pos];}

        Data data()const{return node->DATA[pos];}

        bool operator==(const iterator &o)const{return pos == o.pos && node == o.node && theTree == o.theTree && isEnd == o.isEnd;}

        bool operator!=(const iterator &o)const{return !(*this == o);}

    };



private:
    basicInfo BasicInformation;
    //基本信息储存入非叶结点文件中
    DiskExe<leafNode,basicInfo> leafDisk;
    DiskExe<nonLeafNode,basicInfo> nodeDisk;

    recyclePool repos;


    int findLeaf(const Key & _key){
        repos.clear();
        nonLeafNode * tmpNode = nodeDisk.read(BasicInformation.root);
        while(!tmpNode->childIsLeaf){
            int index = upper_bound(tmpNode->INDEX,tmpNode->childSize-1,_key);
            repos.push_back(index);
            tmpNode = nodeDisk.read(tmpNode->childPosition[index]);
        }
        int index = upper_bound(tmpNode->INDEX,tmpNode->childSize-1,_key);
        repos.push_back(index);
        return tmpNode->childPosition[index];
    }

public:
    BPT() = delete;

    explicit BPT(const string & _name):leafDisk(_name + "_leaf.dat",Cap1),nodeDisk(_name + "_node.dat",Cap2){
        BasicInformation = nodeDisk.tellInfo();
    }
    ~BPT(){
        nodeDisk.setInfo(BasicInformation);
    }
    size_t size() const{
        return BasicInformation.size;
    }
    bool empty() const{
        return BasicInformation.size == 0;
    }

    void insert(const Key & _key,const Data & _data){
        BasicInformation.size++;
        if(BasicInformation.root == -1) {
            nonLeafNode newRoot;
            ++newRoot.childSize;
            newRoot.position = nodeDisk.tellp();
            newRoot.childIsLeaf = true,newRoot.childPosition[0] = leafDisk.tellp();
            leafNode newLeafNode;
            newLeafNode.position = newRoot.childPosition[0];
            newLeafNode.father = newRoot.position;
            newLeafNode.KEY[0] = _key;
            newLeafNode.DATA[0] = _data;
            ++newLeafNode.dataSize;
            BasicInformation.setInfo(newRoot.position,newLeafNode.position,1);
            nodeDisk.write(newRoot);
            leafDisk.write(newLeafNode);
        }
        else{
            int leafPos = findLeaf(_key);
            leafNode * tarLeaf = leafDisk.read(leafPos);
            tarLeaf->addElement(_key,_data,this);
        }
    }

    bool erase(const Key & _key,const Data & _data){
        if(BasicInformation.root == -1) return false;
        int leafPos = -1,keyPos = -1;
        int tarPos = findLeaf(_key);
        leafNode * tarLeaf = leafDisk.read(tarPos);
        tarLeaf->findDeletePos(_key,_data,this,0,leafPos,keyPos);
        if(leafPos == -1) return false;
        if(BasicInformation.size == 1){
            this->clear();
            return true;
        }
        leafNode * targetLeafNode = leafDisk.read(leafPos);
        targetLeafNode->deleteElement(keyPos,this);
        --BasicInformation.size;
        return true;
    }

    void find(const Key & _key,vector<Data> & vec_ans){
        if(BasicInformation.root == -1) return;
        int leafPosition = findLeaf(_key);
        leafNode * tmpLeafNode = leafDisk.read(leafPosition);
        tmpLeafNode->findElement(_key,vec_ans,this,0);
    }

    int count(const Key & _key){
        vector<Data>tmp;
        find(_key,tmp);
        if(tmp.empty())return 0;
        else return 1;
    }

    //second == 0:未找到；second == 1: 已找到；second == 2: end
    pair<Data,int> NextData(const Key & _key){
        Data need = Data();
        if(BasicInformation.root == -1)return make_pair(need,0);
        int flag = 0;
        int leafPosition = findLeaf(_key);
        leafNode * tmpLeafNode = leafDisk.read(leafPosition);
        tmpLeafNode->findNextElement(_key,this,need,flag);
        return make_pair(need,flag);
    }

    iterator nextData(const Key & _key){
        if(BasicInformation.root == -1)return iterator(this, nullptr,0, true);
        int flag = 0,pos = -1;
        int leafPosition = findLeaf(_key);
        leafNode * tmpLeafNode = leafDisk.read(leafPosition);
        leafNode * tar = tmpLeafNode;
        tmpLeafNode->FindNextElement(_key,this,pos,flag,tar);
        if(flag == 2)return iterator(this,tar,pos,true);
        else return  iterator(this,tar,pos,false);
    }

    iterator begin(){
        return iterator(this,leafDisk.read(BasicInformation.head),0,false);
    }

    void clear(){
        leafDisk.clear(),nodeDisk.clear();
        BasicInformation.setInfo(-1,-1,0);
    }
    void findAll(vector<Data> & vec_ans){
        if(BasicInformation.root == -1 || BasicInformation.head == -1) return;
        int ptr = BasicInformation.head;
        while(true){
            leafNode * tmpNode = leafDisk.read(ptr);
            for(int i = 0;i < tmpNode->dataSize;++i){
                vec_ans.push_back(tmpNode->DATA[i]);
            }
            if(tmpNode->rightPos == -1) break;
            ptr = tmpNode->rightPos;
        }
    }

};


#endif //TRAIN_TICKET_BPLUSTREE_HPP
