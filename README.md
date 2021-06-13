# Train Ticket 开发文档

## BPT

### 私有类

储存信息为索引和数据两部分组成

模板：

```c++
template <class Key,class Data>
```

#### 1、叶结点类（leafNode）

###### 数据成员：

其在叶结点文件中位置和父亲、左右兄弟在文件中位置；

```c++
 int position = -1;
 int father = -1;
 int leftPos = -1;
 int rightPos = -1;
```

储存数据数量；

```c++
int dataSize = 0;
```

索引数组（最大容量为L），数据数组（最大容量为L）；

```c++
Key KEY[L];
Data DATA[L];
```

###### 成员函数：

添加数据函数（addElement），若达到L条数据，则进行裂块（split）；

删除数据函数（deleteElement），若小于L/2条数据，则分情况进行：

左借（borrowLeft），右借(borrowRight)，左合并(mergeLeft)，右合并(mergeRight)；

查找数据函数（findElement）:在引用的vector中储存与特定索引（_key）相关的数据；

查找删除位置（findDeletePos）;（服务于可重复索引的bpt）

查找下一条数据（findNextElement）:查找特定索引的下一个索引的数据；（服务于储存不可重复索引的bpt）；

#### 2、非叶结点类（nonLeafNode）

###### 数据成员：

其在非叶结点文件中位置和父亲、左右兄弟在文件中位置；

```c++
int position = -1;
int father = -1;
int leftPos = -1;
int rightPos = -1;
```

子结点数量；

```c++
int childSize = 0;
```

索引数组（最大数量M - 1）；

```c++
Key INDEX[M];
```

子结点在文件中位置，若儿子为叶结点，则为在叶节点文件中位置（最大数量M）；

```c++
int childPosition[M];
```

表示子结点是否为叶结点的布尔值；

```c++
bool childIsLeaf = false;
```

###### 成员函数：

添加数据函数（addElement），若达到M个子结点，则进行裂块（split）；

删除数据函数（deleteElement），若小于M/2个子结点，则分情况进行：

左借（borrowLeft），右借(borrowRight)，左合并(mergeLeft)，右合并(mergeRight)；

查找索引位置函数（findKeyPos）；

#### 3、基本信息类

###### 数据成员：

根结点在非叶结点文件中位置；

储存数据量（叶结点数）；

最左（最小key）叶结点在叶结点文件中位置，即叶结点链表表头；

```c++
int root = -1;
int size = 0;
int head = -1;
```

###### 成员函数：

设置基本信息（setInfo）;

```c++
void setInfo(int _root,int _head,int _size){
            root = _root,head = _head,size = _size;
        }
```

### 4、迭代器类（iterator）

在叶结点链表内的迭代器，实现++、--，可返回当前数据信息的key和data，以及可以知道是否在表头或表尾。

### 私有成员

```c++
basicInfo BasicInformation;
//基本信息储存入非叶结点文件中
DiskExe<leafNode,basicInfo> leafDisk;
DiskExe<nonLeafNode,basicInfo> nodeDisk;
//bonus内容：B+树回收池
recyclePool repos;
```

### 公有函数接口

###### 唯一构造函数:

需给出文件名_name

非叶结点文件名为 "_name_node.dat"

叶结点文件名为 "_name_leaf.dat"

```c++
explicit BPT(const string & _name):leafDisk(_name + "_leaf.dat",379),nodeDisk(_name + "_node.dat",15372)
{
   BasicInformation = nodeDisk.tellInfo();
}
```

###### 析构函数：

重新在非叶结点文件头设置基本信息

```c++
~BPT(){nodeDisk.setInfo(BasicInformation);}
```

###### 功能函数接口：

1. size()返回bpt大小；

2. empty()返回布尔值表示bpt是否为空；

3. ```c++
   //插入函数，给出索引和储存信息，将其作为数据块插入bpt中
   void insert(const Key & _key,const Data & _data);
   ```

4. ```c++
   //删除函数，给出索引和储存信息,若在bpt中存在该数据块则将其删除，返回true，反之返回false
   bool erase(const Key & _key,const Data & _data);
   ```

5. ```c++
   //查找函数，给出索引，将所有该索引连结的数据储存如引用的vector中
   void find(const Key & _key,vector<Data> & vec_ans);
   ```

6. ```c++
   //对于储存不可重复key的bpt，给出一个索引，找到下一索引的数据，返回pair
   //second == 0:未找到；second == 1: 已找到；second == 2: end
   pair<Data,int> NextData(const Key & _key);
   ```

7. begin(): 返回一个指向叶结点表头的iterator；

8. clear(): 清除bpt内文件，重置其信息；

9. findAll(): 将所有数据存入引用容器中（数据量过大时不建议使用）；

## DiskExe(文件处理类)

### 私有成员

文件流对象，文件名；

文件链表当前位置与预处理位置：节省存储空间；

缓存链表，辅助哈希表；

### 私有类

#### 缓存链表（cachelist）:

将文件中内容作为数据块读入内存，以链表形式存储；

链表容量一定，每次插入至表头，并弹出表尾元素；

保证可能多次使用的数据块更新至表头；

析构时将缓存内容全部写入文件。

### 公有函数接口

1.

```c++
//在文件尾或者文件回收位置写入data
int write(const T & data)；
```

2.

```c++
//在指定位置写入*data
void write(const T * data,int position);
```

3.

```c++
//从指定位置读入类型为T的数据
T * read(int index)；
```

4. tellp()：返回文件尾位置；

5. ```c++
   //在表头设置基本信息（bpt）
   void setInfo(const basicInfo & info);
   ```

6. clear(): 清空并重置文件；

## vector

参考STLite中vetor，使用方法类同stl库；

## HashMap

哈希表使用头文件<function>中hash以给索引key分配相应位置。

### 接口：

```c++
void insert(const Key & _key,const Data & _data);
```

```c++
Data & find(const Key & _key);
```

```c++
void erase(const Key & _key);
```

```c++
Data & operator [](const Key & _key);
```

```c++
void clear();
```
# 逻辑部分开发文档

# User 类 
1. _username char数组，储存用户名
2. _password char数组，储存密码
3. _mailAddr char数组，储存邮箱地址
4. _name char数组，储存中文名
5. _privilege(int)，储存优先级
6. _login(int),记录是否已经登录 0：未登录 1：已登录
7. hsh(unsigned int) 记录用户名的哈希值

# Station 类
1. name char数组，记录车站名
2. hsh(unsigned int),记录车站名哈希值

# Train 类
1. _trainID char数组，记录Train的ID
2. int _stationNum 记录经过车站数
3.	int _seatNum 记录座位数
4.	int _prices 记录车票价格
5.	int _travelTimes 记录相邻两个站之间行驶时间
6. int _stopoverTimes 记录在一个站停留的时间
7.	station _stations 记录经过的车站
8.	char _type 记录列车类型
9.	std::pair<int,int> _st,_ed 记录列车运行的时间段
10. std::pair<int,int> _starttime 记录列车每天发车时间
11. int released 记录列车状态 0：未发布 1：已发布 -1：删除
12. int _ticketsSold[96] 被售出的车票数
13. std::pair<int,int> tim[105][2] 记录到达每个站和从某个站出发的时间
14. int g[105][2]; 记录到达每个站和从每个站出发的日期和发车日期间隔的天数

# Order 类
1. data pair<int,int> 记录订单的购买的车票的发车时间
2. user_id train_id unsigned int 记录订单的购买用户和火车车次
3. Num status 购买车票数量和订单状态
4. l,r int 记录出发站和到达站
5. price  long long 记录车票价格

设计整体思想: 使用b+树维护火车信息用户信息订单信息在文档中的位置

BPT:
BPT<uint,int> mp("User_bpt"); 维护用户信息在文件中的位置
BPT<std::pair<uint,int>,int> Order_mp("Order_bpt"),Wait_mp("Wait_bpt"); 维护订单信息和候补订单信息在文件中的位置
BPT<uint,int> Station_mp("Station_bpt"); 车站信息在文件中的位置
BPT<uint,int> mp_Train("Train_bpt"); 维护火车信息在文件中的位置

文件：
User:储存用户信息

Train:储存火车信息

Order:储存订单信息

Seat:储存每天班次车辆卖出的车票数量

Station:记录每个站台经过的火车信息

函数：
int main()：分配指令

void create_file()：建立文件

inline uint gethsh(char *ch) 求ch哈希值

inline bool pd (std::pair<int,int> st,std::pair<int,int> ed,std::pair<int,int> tmp)判断tmp是否在st到ed之间 

void T_read(int pos,T &a) 从文件pos位读取信息存到a中

int T_write(int pos,T &a) 将a储存到文件从pos位开始

int add_user()：添加用户

int login()：登录

int logout()：登出

int query_profile()：查询用户信息

int modify_profile()：修改用户信息

int add_train()：添加车辆

int release_train()：发布车辆

int query_train():查询车辆

int delete_train()：删除车辆

void query_ticket():查询车票 找到同时经过出发站终点站的列车，查看是否满足要求，若满足加入答案，最后将答案排序输出

void query_transfer()：查询换乘 先枚举第一辆车，再枚举中间站，再枚举第二辆车，第二辆车必须同时经过中间站和终点站

long long buy_ticket()：买票

int query_order()：查询订单 通过Order_mp查询

int refund_ticket()：退票 通过Order_mp查询修改，若退票成功，再检查该列车是否可以进行补票

void clean()：清空