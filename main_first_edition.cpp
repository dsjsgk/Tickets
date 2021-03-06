#include<cstdio>
#include<iostream>
#include<cstring>
//#include<bits/stdc++.h>
#include "BPT.hpp"
#include "vector.hpp" 

#define debug(x) std::cerr<<#x<<"="<<x<<"\n"
const char instructions[16][15]={"add_user","login","logout","query_profile","modify_profile","add_train","release_train","query_train","delete_train","query_ticket","query_transfer","buy_ticket","query_order","refund_ticket","clean","exit"};
const int Mod1 = 1e9+7 , Mod2 = 998244353,base2 = 653;int Month[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
typedef unsigned int uint;
uint base1 = 401;
long long _counter=0;
std::fstream fileuser("User",std::ios::in|std::ios::out|std::ios::binary);

std::fstream filetrain("Train",std::ios::in|std::ios::out|std::ios::binary);

std::fstream fileorder("Order",std::ios::in|std::ios::out|std::ios::binary); 

std::fstream fileseat("Seat",std::ios::in|std::ios::out|std::ios::binary);

std::fstream filestation("Station",std::ios::in|std::ios::out|std::ios::binary);
void create_file(){
	if(!fileuser.is_open()){
		fileuser.open("User",std::ios::out|std::ios::binary);
		fileuser.close();
		fileuser.open("User",std::ios::in|std::ios::out|std::ios::binary);
	}
	if(!filetrain.is_open()){
		filetrain.open("Train",std::ios::out|std::ios::binary);
		filetrain.close();
		filetrain.open("Train",std::ios::in|std::ios::out|std::ios::binary);
	}
	if(!fileorder.is_open()){
		fileorder.open("Order",std::ios::out|std::ios::binary);
		fileorder.close();
		fileorder.open("Order",std::ios::in|std::ios::out|std::ios::binary);
	}
	if(!fileseat.is_open()){
		fileseat.open("Seat",std::ios::out|std::ios::binary);
		fileseat.close();
		fileseat.open("Seat",std::ios::in|std::ios::out|std::ios::binary);
	}
	if(!filestation.is_open()){
		filestation.open("Station",std::ios::out|std::ios::binary);
		filestation.close();
		filestation.open("Station",std::ios::in|std::ios::out|std::ios::binary);
	}
}
inline uint gethsh(char *ch) {
	int len=strlen(ch);
	uint res1=0;
	for(int i=0;i<len;++i) {
//		assert(ch[i]+257>0);
		res1=(uint)res1*base1+(uint)(ch[i]+257);
	}
	return res1;
} 
class station{
	public:
	char name[33];
	uint hsh;
	inline void init() {hsh=gethsh(name);}
};
class Station {
	public: 
	uint Train_hsh[10001];
//	short pos[10001];
	int Num;
	Station() {Num=0;}
};
inline bool pd (std::pair<int,int> st,std::pair<int,int> ed,std::pair<int,int> tmp) {
	if(tmp<st) return 0;
	if(tmp>ed) return 0;
	return 1;
}
class User{
	public:
		char _username[25];
		char _password[35];
		char _mailAddr[35];
		char _name[25]; 
		int _privilege;
		bool _login;
		uint hsh;
	public:	
	User() {
		_login=0;
	}
	friend void add_user();
	friend void login(); 
	friend void exit();
	friend void clean(); 
	friend void query_profile();
	friend void logout();
	friend void modify_profile();
};
class Seat {
	public:
		int _Sold[101];
	Seat() {memset(_Sold,0,sizeof(_Sold));}
};
class Train{
	public:
		char _trainID[25];
		int _stationNum;
		int _seatNum;
		int _prices[101];
		int _travelTimes[101],_stopoverTimes[101];
		station _stations[101];
		char _type;
		std::pair<int,int> _st,_ed;
		std::pair<int,int> _starttime;
		int released;
		int _ticketsSold[96];
		std::pair<int,int> tim[101][2];
		short g[101][2];
	public:
	Train() {
		released=0;
//		memset(_ticketsSold,0,sizeof(_ticketsSold));
		memset(_trainID,0,sizeof(_trainID));
		_stationNum=_seatNum=0;
//		memset(_prices,0,sizeof(_prices));
//		memset(_travelTimes,0,sizeof(_travelTimes));
//		memset(_stopoverTimes,0,sizeof(_stopoverTimes));
		_st=_ed=_starttime=std::make_pair(0,0);
		memset(g,0,sizeof(g));
//		for(int i=0;i<101;++i) tim[i][0]=tim[i][1]=std::make_pair(0,0);
	}
	friend void add_train();
	friend void release_train();
	friend void query_train();
	friend void delete_train(); 
	friend void but_ticket();
};
inline bool operator == (Train a,Train b){
	int len1=strlen(a._trainID);
	int len2=strlen(b._trainID);
	if(len1!=len2) return 0;
	for(int i=0;i<len1;++i) if(a._trainID[i]!=b._trainID[i]) return 0;
	return 1;
}
class order{
	private:
	public:
		std::pair<int,int> date;uint user_id,train_id;
		int Num,status,l,r;
		long long price;
		station s,t;
};
BPT<uint,int> mp("User_bpt");
inline void User_read(int pos,User &a) {
//	User a;
	fileuser.seekg(pos,std::ios::beg);
	fileuser.read(reinterpret_cast<char*>(&a),sizeof(a));
//	return a;
}
inline void Train_read(int pos,Train &a) {
//	Train a;
	filetrain.seekg(pos,std::ios::beg);
	filetrain.read(reinterpret_cast<char*>(&a),sizeof(a));
//	return a; 
}
inline void order_read(int pos,order &a) {
//	order a;
	fileorder.seekg(pos,std::ios::beg);
	fileorder.read(reinterpret_cast<char*>(&a),sizeof(a));
//	return a; 
}
inline void Seat_read(int pos,Seat &a) {
//	Seat a;
	fileseat.seekg(pos,std::ios::beg);
	fileseat.read(reinterpret_cast<char*>(&a),sizeof(a));
//	return a; 
}
inline void Station_read(int pos,Station &a) {
//	Station a;
	filestation.seekg(pos,std::ios::beg);
	filestation.read(reinterpret_cast<char*>(&a),sizeof(a));
//	return a; 
}
inline int User_write(int pos,User &a) {
	if(pos==-1) fileuser.seekg(0,std::ios::end);
	else fileuser.seekg(pos,std::ios::beg);
	int cur=fileuser.tellg();
	fileuser.write(reinterpret_cast<char*>(&a),sizeof(a));
	return cur;
}
inline int Train_write(int pos,Train &a) {
	if(pos==-1) filetrain.seekg(0,std::ios::end);
	else filetrain.seekg(pos,std::ios::beg);
	int cur=filetrain.tellg();
	filetrain.write(reinterpret_cast<char*>(&a),sizeof(a));
	return cur;
}
inline int order_write(int pos,order &a) {
	if(pos==-1) fileorder.seekg(0,std::ios::end);
	else fileorder.seekg(pos,std::ios::beg);
	int cur=fileorder.tellg();
	fileorder.write(reinterpret_cast<char*>(&a),sizeof(a));
	return cur;
}
inline int Seat_write(int pos,Seat &a) {
	if(pos==-1) fileseat.seekg(0,std::ios::end);
	else fileseat.seekg(pos,std::ios::beg);
	int cur=fileseat.tellg();
	fileseat.write(reinterpret_cast<char*>(&a),sizeof(a));
	return cur;
}
inline int Station_write(int pos,Station &a) {
	if(pos==-1) filestation.seekg(0,std::ios::end);
	else filestation.seekg(pos,std::ios::beg);
	int cur=filestation.tellg();
	filestation.write(reinterpret_cast<char*>(&a),sizeof(a));
	return cur;
}
BPT<std::pair<uint,int>,int> Order_mp("Order_bpt"),Wait_mp("Wait_bpt");
BPT<uint,int> Station_mp("Station_bpt");
bool operator < (station &p,station &q) {
	int len1=strlen(p.name);
	int len2=strlen(q.name);
	for(int i=0;i<std::min(len1,len2);++i) if(p.name[i]!=q.name[i]) return p.name[i]<q.name[i];
	return len1<len2; 
}
bool operator == (station &p,station &q) {
	int len1=strlen(p.name);
	int len2=strlen(q.name);
	if(len1!=len2) return 0;
	for(int i=0;i<std::min(len1,len2);++i) if(p.name[i]!=q.name[i]) return 0;
	return 1; 
} 
BPT<uint,int> mp_Train("Train_bpt");
inline void put_date(std::pair<int,int> a) {
	if(a.first<10) std::cout<<0<<a.first;
	else std::cout<<a.first;
	std::cout<<"-";
	if(a.second<10) std::cout<<0<<a.second;
	else std::cout<<a.second;
} 
inline void put_time(std::pair<int,int> a) {
	if(a.first<10) std::cout<<0<<a.first;
	else std::cout<<a.first;
	std::cout<<":";
	if(a.second<10) std::cout<<0<<a.second;
	else std::cout<<a.second;
}
int get_date(std::pair<int,int> a,std::pair<int,int> b) {
//	if(a>b) assert(0);
	int Sum=0;
	for(int i=a.first;i<b.first;++i) {
		Sum+=Month[i];
	}
	Sum-=a.second;
	Sum+=b.second;
	return Sum; 
}
int get_time(std::pair<int,int> a,std::pair<int,int> b,std::pair<int,int> c,std::pair<int,int> d) {
	return get_date(a,c)*24*60+d.first*60+d.second-b.first*60-b.second;
}
inline std::pair<int,int> plusdate_2(std::pair<int,int> a,int o) {
	a.second+=o;
	while(a.second>Month[a.first]) {
		a.second-=Month[a.first];
		a.first++;
	}
	return a;
}
inline void plus_date(std::pair<int,int> &a,std::pair<int,int> &b,int O) {
	int _hour=O/60,_minute=O%60;
	b.second+=_minute;
	if(b.second>=60) b.second-=60,_hour++;
	b.first+=_hour;
	int _day=b.first/24;
	b.first%=24;
	a=plusdate_2(a,_day);
}
inline std::pair<int,int> mns(std::pair<int,int> a,int o) {
	a.second-=o;
	while(a.second<=0) {
		a.first--;
		a.second+=Month[a.first];	 
	}
	return a;
}
void add_train(){
	char ch=getchar();
	Train tmp;
	uint hsh;
	int ok=1;
	for(;;ch=getchar()) {
		if(ch=='\n') break;
		if(ch<'a'||ch>'z') continue;
		if(ch=='i') {
			char s[25];
			scanf("%s",s);
			memcpy(tmp._trainID,s,sizeof(s));
			hsh=gethsh(s);
			continue;
		}
		if(ch=='n') {
			scanf("%d",&tmp._stationNum);
			continue;
		}
		if(ch=='m') {
			scanf("%d",&tmp._seatNum);
			continue;
		}
		if(ch=='s') {
			ch=getchar();
			for(int i=0;i<tmp._stationNum;++i) {
				if(i!=tmp._stationNum-1)
				std::cin.getline(tmp._stations[i].name,100000,'|');
				else scanf("%s",tmp._stations[i].name);
			}
			for(int i=0;i<tmp._stationNum;++i) tmp._stations[i].init();
			continue;
		}
		if(ch=='p') {
			ch=getchar();
			for(int i=0;i<tmp._stationNum-1;++i) {
				if(i!=tmp._stationNum-2) {
					char s[25];
					std::cin.getline(s,100000,'|');
					int len=strlen(s);
					tmp._prices[i]=0;
					for(int j=0;j<len;++j) {
						tmp._prices[i]*=10;
						tmp._prices[i]+=s[j]-'0';
					}
				}
				else  scanf("%d",&tmp._prices[i]);
			}
			continue;
		}
		if(ch=='x') {
			char s[25];
			scanf("%s",s);
			int len=strlen(s);
			int cur=0;
			tmp._starttime=std::make_pair(0,0);
			for(int i=0;i<len;++i) {
				if(s[i]==':') {cur++;continue;} 
				if(!cur) {
					tmp._starttime.first*=10;
					tmp._starttime.first+=s[i]-'0';
				}
				else {
					tmp._starttime.second*=10;
					tmp._starttime.second+=s[i]-'0';
				}
			}
			continue;
		}
		if(ch=='t') {
			ch=getchar();
			for(int i=0;i<tmp._stationNum-1;++i) {
				if(i!=tmp._stationNum-2) {
					char s[21];
					std::cin.getline(s,100000,'|');
					int len=strlen(s);
					tmp._travelTimes[i]=0;
					for(int j=0;j<len;++j) {
						tmp._travelTimes[i]*=10;
						tmp._travelTimes[i]+=s[j]-'0';
					}
				}
				else  scanf("%d",&tmp._travelTimes[i]);
			}
			continue;
		}
		if(ch=='o') {
			ch=getchar();
			for(int i=0;i<tmp._stationNum-2;++i) {
				if(i!=tmp._stationNum-3) {
					char s[21];
					std::cin.getline(s,100000,'|');
					int len=strlen(s);
					tmp._stopoverTimes[i]=0;
					for(int j=0;j<len;++j) {
						tmp._stopoverTimes[i]*=10;
						tmp._stopoverTimes[i]+=s[j]-'0';
					}
				}
				else  scanf("%d",&tmp._stopoverTimes[i]);
			}
			continue;
		}
		if(ch=='d') {
			char s[21];
			scanf("%s",s);
			tmp._st.first=(s[0]-'0')*10+(s[1]-'0');
			tmp._st.second=(s[3]-'0')*10+(s[4]-'0');
			tmp._ed.first=(s[6]-'0')*10+(s[7]-'0');
			tmp._ed.second=(s[9]-'0')*10+(s[10]-'0');
			continue;
		}
		if(ch=='y') {
			tmp._type=getchar();
			tmp._type=getchar();
			continue;
		}
	}
	if(!ok) {
		{printf("-1\n");return ;} 
	}
	if(tmp._st>tmp._ed) {
		{printf("-1\n");return ;} 
	}
	if(mp_Train.count(hsh)) {
		{printf("-1\n");return ;}
	}
	std::pair<int,int> tim1,tim2;
	tim1=tmp._starttime,tim2.first=1,tim2.second=1;
	tmp.tim[0][1]=tim1;
	tmp.g[0][1]=0;
	// 1: chufa 0:daoda
	for(int i=0;i<tmp._stationNum-1;++i) {
		plus_date(tim2,tim1,tmp._travelTimes[i]);
		tmp.tim[i+1][0]=tim1;
		tmp.g[i+1][0]=get_date(std::make_pair(1,1),tim2);
		if(i!=tmp._stationNum-2) {
			plus_date(tim2,tim1,tmp._stopoverTimes[i]);
			tmp.tim[i+1][1]=tim1;
			tmp.g[i+1][1]=get_date(std::make_pair(1,1),tim2);
		} 
	}
	mp_Train.insert(hsh,Train_write(-1,tmp));
	printf("0\n");return ;
}
void release_train() {
	char ch=getchar();
	int ok=0;
	for(;;ch=getchar()) {
		if(ch=='\n') break;
		if(ch<'a'||ch>'z') continue;
		if(ch=='i') {
			char s[25];
			scanf("%s",s); 
			uint tmp=gethsh(s);
			if(!mp_Train.count(tmp)) {ok=-1;continue;}
			vector<int> __v;
			mp_Train.find(tmp,__v);
			int cur=__v[0];
			Train _train_;
			Train_read(cur,_train_);
			if(_train_.released!=0) {ok=-1;continue;}
			_train_.released=1;
			
			for(int i=0;i<_train_._stationNum;++i) {
				uint Stationhsh=_train_._stations[i].hsh;
				if(Station_mp.count(Stationhsh)) {
					vector<int> _v_;
					Station_mp.find(Stationhsh,_v_);
					int Stationcur=_v_[0];
					Station _temp_;
					Station_read(Stationcur,_temp_);
					_temp_.Num++;
					_temp_.Train_hsh[_temp_.Num]=tmp;
//					_temp_.pos[_temp_.Num]=i; 
					Station_write(Stationcur,_temp_);
				}
				else {
					Station _temp_;
					_temp_.Num=0;
					_temp_.Num++;
					_temp_.Train_hsh[_temp_.Num]=tmp;
//					_temp_.pos[_temp_.Num]=i; 
					Station_mp.insert(Stationhsh,Station_write(-1,_temp_));
				}
			}
			int totdays=get_date(_train_._st,_train_._ed);
			Seat temp;
			fileseat.seekg(0,std::ios::end);
			for(int i=0;i<=totdays;++i) {
				_train_._ticketsSold[i]=fileseat.tellg();
				fileseat.write(reinterpret_cast<char*>(&temp),sizeof(temp));
			}
			Train_write(cur,_train_);
			continue;
		} 
	}
	printf("%d\n",ok);
}
void query_train() {
	char ch=getchar();
	int ok=0;
	uint tmp;
	std::pair<int,int> o;
	for(;;ch=getchar()) {
		if(ch=='\n') break;
		if(ch<'a'||ch>'z') continue;
		if(ch=='i') {
			char s[25];
			scanf("%s",s);
			tmp=gethsh(s);
			if(!mp_Train.count(tmp)) {
				ok=-1;
				continue;
			}
			vector<int> __v;
			mp_Train.find(tmp,__v);
			int traincur=__v[0];
			Train R;
			Train_read(traincur,R);
			if(!pd(R._st,R._ed,o)) {ok=-1;continue;}
			if(R.released==-1) {ok=-1;continue;} 
			printf("%s %c\n",R._trainID,R._type);
			std::pair<int,int> cur1=o,cur2=R._starttime;
			long long Sum=0;
			int dddd=get_date(R._st,o);
			
			Seat _temp;
			if(R.released==1){ Seat_read(R._ticketsSold[dddd],_temp);}
			for(int i=0;i<R._stationNum;++i) {
				printf("%s ",R._stations[i].name);
				if(i==0) {
					std::cout<<"xx-xx xx:xx -> ";
					put_date(cur1);
					std::cout<<" ";
					put_time(cur2);
					std::cout<<" "<<Sum<<" "<<R._seatNum-_temp._Sold[i]<<"\n";
					plus_date(cur1,cur2,R._travelTimes[i]);
					Sum+=R._prices[i];
					continue;
				}
				if(i==R._stationNum-1){
					put_date(cur1);
					std::cout<<" ";
					put_time(cur2);
					std::cout<<" -> xx-xx xx:xx";
					std::cout<<" "<<Sum<<" "<<"x\n";
					continue;
				}
				put_date(cur1);
				std::cout<<" ";
				put_time(cur2);
				std::cout<<" -> ";
				plus_date(cur1,cur2,R._stopoverTimes[i-1]);
				put_date(cur1);
				std::cout<<" ";
				put_time(cur2);
				std::cout<<" "<<Sum<<" "<<R._seatNum-_temp._Sold[i]<<"\n";
				plus_date(cur1,cur2,R._travelTimes[i]);
				Sum+=R._prices[i];
				continue;
			}
//			return ;
			continue;
		}
		if(ch=='d') {
			char s[21];
			scanf("%s",s);
			o.first=(s[0]-'0')*10+s[1]-'0';
			o.second= (s[3]-'0')*10+s[4]-'0';
		}
	}
	if(ok==-1)
	printf("%d\n",ok);
}
void delete_train(){
	char ch=getchar();
	int ok=0;
	uint tmp;
	for(;;ch=getchar()) {
		if(ch=='\n') break;
		if(ch<'a'||ch>'z') continue;
		if(ch=='i') {
			char s[25];
			scanf("%s",s);
			tmp=gethsh(s);
			if(!mp_Train.count(tmp)) {
				ok=-1;
				continue;
			}
			vector<int> __v;
			mp_Train.find(tmp,__v);
			int curtrain=__v[0];
			Train O;
			Train_read(curtrain,O);
			if(O.released!=0) {
				ok=-1;
				continue;
			}
			mp_Train.erase(tmp,__v[0]);
			continue;
		}
	}
	std::cout<<ok<<"\n";
} 
void add_user() {
	char ch=getchar(); 
	int ok=0;
	User tmp; 
	uint h;
	int pri=-1;
	char nwname[25],ps[35],nm[25],ml[35];
	for(;;ch=getchar()) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='c') {
			char s[35];
			scanf("%s",s);
			h=gethsh(s); 
			continue;
		}
		if(ch=='u') {
			scanf("%s",nwname);
			continue;
		}
		if(ch=='p') {
			scanf("%s",ps);
			continue;
		}
		if(ch=='n') {
			scanf("%s",nm);
			continue;
		}
		if(ch=='m') {
			scanf("%s",ml);
			continue;
		}
		if(ch=='g') {
			scanf("%d",&pri);
			if(!mp.count(h)&&(!mp.empty())) {ok=-1;continue;}
			else {
				if(mp.empty()) {
					memcpy(tmp._username,nwname,sizeof(nwname));
					memcpy(tmp._password,ps,sizeof(ps));
					memcpy(tmp._name,nm,sizeof(nm));
					memcpy(tmp._mailAddr,ml,sizeof(ml));
					tmp.hsh=gethsh(nwname);
					tmp._privilege=10;		
					mp.insert(tmp.hsh,User_write(-1,tmp));
					continue;
				}
				vector<int> _v;
				mp.find(h,_v);
				User root;
				User_read(_v[0],root);
				if(root._privilege>pri&&root._login) {
					memcpy(tmp._username,nwname,sizeof(nwname));
					memcpy(tmp._password,ps,sizeof(ps));
					memcpy(tmp._name,nm,sizeof(nm));
					memcpy(tmp._mailAddr,ml,sizeof(ml));
					tmp.hsh=gethsh(nwname);
					if(mp.count(tmp.hsh)) {
						ok=-1;continue;
					}
					tmp._privilege=pri;
					mp.insert(tmp.hsh,User_write(-1,tmp));
				}
				else ok=-1;
			}
			continue;
		}
	}
	printf("%d\n",ok);
//	fflush(stdout);	
}
void login() {
//	debug(1);
	uint h;
	int ok=0;
	char ch=getchar();
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='u') {
			char s[35];
			scanf("%s",s);
			h=gethsh(s);
		}
		else {
			char s[35];
			scanf("%s",s);
			if(!mp.count(h)) {
				ok=-1;
				continue;
			}
			vector<int> _v;
			mp.find(h,_v);
			int cur=_v[0];
			User tmp;
			User_read(cur,tmp);
			int len1=strlen(tmp._password);
			int len2=strlen(s);
			if(len1!=len2) {
				ok=-1;
				continue;
			}
			for(int i=0;i<len1;++i) if(s[i]!=tmp._password[i]) {ok=-1;continue;}
			if(tmp._login==1) {ok=-1;continue;}
			if(ok!=-1)
			{
				tmp._login=1;
				User_write(cur,tmp);
			}
		}
	}
	printf("%d\n",ok);
//	fflush(stdout);
}
void logout() {
	uint h;
	int ok=0;
	char ch=getchar();
	for(;;ch=getchar()) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='u') {
			char s[31];
			scanf("%s",s);
			h=gethsh(s);
			if(!mp.count(h)) {
				ok=-1;
				continue;
			}
			vector<int> _v;
			mp.find(h,_v);
			int cur=_v[0];
			User tmp;
			User_read(cur,tmp);
			if(!tmp._login) {
				ok=-1;
				continue;
			}
			tmp._login=0;
			User_write(cur,tmp);
			continue;
		}
	}
	printf("%d\n",ok);
//	fflush(stdout);
}
void query_profile() {
	uint h1,h2;
	int pri1=-1,pri2=-1;
	int ok=0;
	char ch=getchar();
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='c') {
			char s[35];
			scanf("%s",s);
			h1=gethsh(s);
			if(!mp.count(h1)) {
				ok=-1;
				continue;
			}
			vector<int> _v;
			mp.find(h1,_v);
			int cur=_v[0];
			User tmp;
			User_read(cur,tmp);
			if(!tmp._login) {
				ok=-1;
				continue;
			}
			pri1=tmp._privilege;
			continue;
		}
		else {
			char s[35];
			scanf("%s",s);
			h2=gethsh(s);
			if(!mp.count(h2)) {
				ok=-1;
				continue;
			}
			vector<int> _v;
			mp.find(h2,_v);
			int cur=_v[0];
			User tmp;
			User_read(cur,tmp);
			pri2=tmp._privilege;
			if(pri1<pri2||(pri1==pri2&&h1!=h2)) {
				ok=-1;
				continue;	
			}  
			if(ok!=-1)
			printf("%s %s %s %d\n",tmp._username,tmp._name,tmp._mailAddr,tmp._privilege);
			continue;
		}
	}
	if(ok==-1)
	printf("%d\n",ok);
}
void modify_profile() {
	uint h1,h2;
	int pri1=-1,pri2=-1,newpri=-1;
	int ok=0;
	char ch=getchar();
	char p[35],nm[25],ml[35];
	int _p=0,_nm=0,_ml=0;
	User temp;
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='c') {
			char s[35];
			scanf("%s",s);
			h1=gethsh(s);
			if(!mp.count(h1)) {
				ok=-1;
				continue;
			}
			vector<int> _v;
			mp.find(h1,_v);
			int cur=_v[0];
			User tmp;
			User_read(cur,tmp);
			if(!tmp._login) {
				ok=-1;
				continue;
			}
			pri1=tmp._privilege;
			continue;
		}
		if(ch=='u')
		{
			char s[35];
			scanf("%s",s);
			h2=gethsh(s);
			if(!mp.count(h2)) {
				ok=-1;
				continue;
			}
			vector<int> _v;
			mp.find(h2,_v);
			int cur=_v[0];
			User tmp;
			User_read(cur,tmp);
			temp=tmp;
			pri2=tmp._privilege;
			if(pri1<=pri2&&h1!=h2) {
				ok=-1;
				continue;
			} 
			continue;
		}
		if(ch=='p') {
			scanf("%s",p);
			_p=1;		
			continue;
		}
		if(ch=='n') {
			scanf("%s",nm);
			_nm=1;
			continue;
		}
		if(ch=='m') {
			scanf("%s",ml);
			_ml=1;
			continue;
		}
		if(ch=='g') {
			scanf("%d",&newpri);
			if(newpri>=pri1) {
				ok=-1;
				continue;
			}
			continue;
		} 
	}
	if(ok==-1) {
		std::cout<<-1<<"\n";
		return ;
	}
	if(_p) {
		memcpy(temp._password,p,sizeof(p));
	}
	if(_nm) {
		memcpy(temp._name,nm,sizeof(nm));
	}
	if(_ml) {
		memcpy(temp._mailAddr,ml,sizeof(ml));
	}
	if(newpri>=0) temp._privilege=newpri;
	vector<int> _v;
	mp.find(h2,_v);
	int cur=_v[0];
	User_write(cur,temp);
	printf("%s %s %s %d\n",temp._username,temp._name,temp._mailAddr,temp._privilege);
}
class answer_to_transfer{
	public:
		std::pair<int,int> lev1,lev2,arr1,arr2,leavemid1,leavemid2,arrmid1,arrmid2;
		int price,seat,time,time1,time2,price1,price2,seat1,seat2;
		station mid;
		char s1[31],s2[31];
};
inline bool Compare (std::pair<answer_to_transfer,int> &a,std::pair<answer_to_transfer,int> &b) {
	if(a.second!=b.second) return a.second<b.second;
	else return a.first.time1<b.first.time1;
}
inline bool operator < (std::pair<Train,int> &a,std::pair<Train,int> &b) {
	if(a.second!=b.second) return a.second<b.second;
	else {
		int len1=strlen(a.first._trainID);
		int len2=strlen(b.first._trainID);
		for(int i=0;i<std::min(len1,len2);++i) {
			if(a.first._trainID[i]!=b.first._trainID[i]) return a.first._trainID[i]<b.first._trainID[i];
		}
		return len1<len2;
	}
}
inline void sort2(int a,int b,std::vector<std::pair<Train,int> > &X,std::vector<int> &Y) {
	if(b-a<2) return ;
	int mid=(b-a-1)/2;
	sort2(a,a+mid+1,X,Y);
	sort2(a+mid+1,b,X,Y);
	int *tmp;
	tmp=new int [b-a];
	int pos1=0,pos2=mid+1,cur=0;
	while(pos1<=mid&&pos2<b-a) {
		if(X[Y[pos1+a]]<X[Y[pos2+a]]) pos1++,tmp[cur++]=Y[pos1-1+a];
		else pos2++,tmp[cur++]=Y[pos2-1+a];
	}
	while(pos1<=mid) pos1++,tmp[cur++]=Y[pos1-1+a];
	while(pos2<b-a) pos2++,tmp[cur++]=Y[pos2-1+a];
	for(int i=a;i<b;++i) Y[i]=tmp[i-a];
	delete [] tmp;
}
inline void mergesort(uint *a,uint *b) {
	if(b-a<2) return ;
	uint *tmp;
	int mid=(b-a-1)/2;
//	debug(mid);
	mergesort(a,a+mid+1);
	mergesort(a+mid+1,b);
	tmp=new uint [b-a];
	int pos1=0,pos2=mid+1,cur=0;
	while(pos1<=mid&&pos2<b-a) {
		if(a[pos1]<a[pos2]) pos1++,tmp[cur++]=a[pos1-1];
		else pos2++,tmp[cur++]=a[pos2-1];
	}
	while(pos1<=mid) pos1++,tmp[cur++]=a[pos1-1];
	while(pos2<b-a) pos2++,tmp[cur++]=a[pos2-1];
	for(int i=0;i<b-a;++i) a[i]=tmp[i];
	delete [] tmp;
}
void query_ticket() {
	char ch=getchar();
	station S,T;
	std::pair<int,int> d;
	int tp=0;
	//0:time 1:cost
	//debug(1);
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='s') {
			scanf("%s",S.name);
			S.init();
			continue;
		}
		if(ch=='t') {
			scanf("%s",T.name);
			T.init(); 
			continue;
		}
		if(ch=='d') {
			char s[41];
			scanf("%s",s);
			d.first=(s[0]-'0')*10+(s[1]-'0');
			d.second=(s[3]-'0')*10+(s[4]-'0');
			continue;
		}
		if(ch=='p') {
			char s[21];
			scanf("%s",s);
			if(s[0]=='c') {
				tp=1;
			} 
			continue;
		}
	}
	uint tmp1=gethsh(S.name);
	uint tmp2=gethsh(T.name);
	vector<int> _V_1;
	Station_mp.find(tmp1,_V_1);
	if(_V_1.size()==0) {
		printf("0\n");
		return ;
	} 
	vector<uint> vec;
	int Stationcur1=_V_1[0]; 
	Station Station_S;
	Station_read(Stationcur1,Station_S);
	vector<int> _V_2;
	Station_mp.find(tmp2,_V_2);
	if(_V_2.size()==0) {
		printf("0\n");
		return ;
	} 
	int Stationcur2=_V_2[0]; 
	Station Station_T;
	Station_read(Stationcur2,Station_T);
	mergesort(Station_S.Train_hsh+1,Station_S.Train_hsh+Station_S.Num+1);
	mergesort(Station_T.Train_hsh+1,Station_T.Train_hsh+Station_T.Num+1);
	int cur1=1,cur2=1;
	while(cur1<=Station_S.Num&&cur2<=Station_T.Num) {
		if(Station_S.Train_hsh[cur1]==Station_T.Train_hsh[cur2]) {
			vec.push_back(Station_S.Train_hsh[cur1]);
			cur1++;cur2++;
		}
		else {if(Station_S.Train_hsh[cur1]<Station_T.Train_hsh[cur2]) cur1++;else cur2++;}
	}
	vector<std::pair<Train,int> > ans;
	vector<int> Queue;
	int SZ_vec=vec.size();
	for(int i=0;i<SZ_vec;++i) {
		uint tmp=vec[i];
		if(!mp_Train.count(tmp)) continue;
		vector<int> __v;
		mp_Train.find(tmp,__v);
		int traincur=__v[0];
		Train tt;
		Train_read(traincur,tt);
		if(tt.released!=1) continue;
		int l=-1,r=-1;
		for(int j=0;j<tt._stationNum;++j) {
			if(tt._stations[j].hsh==tmp1) l=j;
			if(tt._stations[j].hsh==tmp2) r=j;
		}
		if(l==-1||r==-1) continue;
		std::pair<int,int> nwd;
		if(l>=r) continue;
		nwd=mns(d,tt.g[l][1]);
		if(!pd(tt._st,tt._ed,nwd)) continue;
		int Sum=0;
		for(int j=l;j<r;++j) {
			if(tp==0) {
				Sum+=tt._travelTimes[j];
				if(j!=r-1)
				Sum+=tt._stopoverTimes[j];
			}
			else {
				Sum+=tt._prices[j];
			}
		}
		ans.push_back(std::make_pair(tt,Sum));
		Queue.push_back(Queue.size());
	}
		sort2(0,Queue.size(),ans,Queue); //to do now
		int SZ=ans.size();
		int ANS=0;
		for(int i=0;i<SZ;++i){
			Train tmp=ans[Queue[i]].first;
			if(i>0) if(tmp==ans[Queue[i-1]].first) continue;
			ANS++;
		}
		std::cout<<ANS<<"\n";
		for(int i=0;i<SZ;++i){
			Train tmp=ans[Queue[i]].first;
			if(i>0) if(tmp==ans[Queue[i-1]].first) continue;
			int l,r;
			for(int j=0;j<tmp._stationNum;++j) {
				if(tmp._stations[j].hsh==tmp1) l=j;
				if(tmp._stations[j].hsh==tmp2) r=j;
			}
			int Sum1=0,Sum2=0,Min=1e9;
			std::pair<int,int> nwd;
			nwd=mns(d,tmp.g[l][1]);
			int dd=get_date(tmp._st,nwd);
//			dd-=tmp.g[l][1];
			Seat _temp;
			Seat_read(tmp._ticketsSold[dd],_temp); 
			for(int j=l;j<r;++j) {
				Sum1+=tmp._travelTimes[j];
				if(j!=r-1)
				Sum1+=tmp._stopoverTimes[j];
				Sum2+=tmp._prices[j];
				Min=std::min(Min,tmp._seatNum-_temp._Sold[j]);
			}
			printf("%s %s ",tmp._trainID,S.name);
			
			std::pair<int,int> t1,t2,t3,t4;
			t1=plusdate_2(nwd,tmp.g[l][1]);
			t3=plusdate_2(nwd,tmp.g[r][0]);
			t2=tmp.tim[l][1];
			t4=tmp.tim[r][0];
			put_date(t1);
			std::cout<<" ";
			put_time(t2);
			std::cout<<" -> ";
			printf("%s ",T.name);
			put_date(t3);
			std::cout<<" ";
			put_time(t4);
			std::cout<<" "<<Sum2<<" "<<Min<<"\n"; 
		}
}
void query_transfer() {
	char ch=getchar();
	station S,T;
	std::pair<int,int> d;
	int tp=0;
	//0:time 1:cost
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='s') {
			scanf("%s",S.name);
			S.init();
			continue;
		}
		if(ch=='t') {
			scanf("%s",T.name);
			T.init(); 
			continue;
		}
		if(ch=='d') {
			char s[41];
			scanf("%s",s);
			d.first=(s[0]-'0')*10+(s[1]-'0');
			d.second=(s[3]-'0')*10+(s[4]-'0');
			continue;
		}
		if(ch=='p') {
			char s[21];
			scanf("%s",s);
			if(s[0]=='c') {
				tp=1;
			} 
			continue;
		}
	}
	vector<int> _V_1;
	Station_mp.find(S.hsh,_V_1);
	if(_V_1.size()==0) {
		printf("0\n");
		return ;
	}
	int Stationcur1=_V_1[0]; 
	Station Station_S;
	Station_read(Stationcur1,Station_S);
	
	vector<int> _V_3;
	Station_mp.find(T.hsh,_V_3);
	if(_V_3.size()==0) {
		printf("0\n");
		return ;
	}
	int Stationcur3=_V_3[0]; 
	Station Station_T;
	Station_read(Stationcur3,Station_T);
	mergesort(Station_T.Train_hsh+1,Station_T.Train_hsh+Station_T.Num+1);
	
	answer_to_transfer _temp;
	std::pair<answer_to_transfer,int> ans=std::make_pair(_temp,2e9);
	int exist=0;
	
	for(int T1=1;T1<=Station_S.Num;++T1) {
//		if(it1.end()) break;
//		if(it1.key().first!=S.hsh) break;
		
		vector<int> __v;
		mp_Train.find(Station_S.Train_hsh[T1],__v);
		int traincur=__v[0];
		Train tmp;
		Train_read(traincur,tmp);
		std::pair<int,int> arrmid1,arrmid2;
		int ok=-1;
		vector<station> vec;
		for(int i=0;i<tmp._stationNum;++i) {
			if(ok!=-1) {
				vec.push_back(tmp._stations[i]);
			}
			else if(tmp._stations[i].hsh==S.hsh) ok=i;
		}
		if(ok==-1) {continue;}
		int Sum1=0;
		int Min1=1e9;
		std::pair<int,int> nwd=mns(d,tmp.g[ok][1]);
		if(!pd(tmp._st,tmp._ed,nwd)) {continue;}
		int dddd=get_date(tmp._st,nwd);
		Seat _temp1;
		Seat_read(tmp._ticketsSold[dddd],_temp1); 
		for(int i=0;i<vec.size();++i) {
			Sum1+=tmp._prices[i+ok];
			Min1=std::min(Min1,tmp._seatNum-_temp1._Sold[i+ok]);
			station mid = vec[i];
			arrmid1=plusdate_2(nwd,tmp.g[ok+i+1][0]);
			arrmid2=tmp.tim[ok+i+1][0];
			vector<int> _V_2;
			Station_mp.find(mid.hsh,_V_2);
			if(_V_2.size()==0) continue;
			int Stationcur2=_V_2[0]; 
			Station Station_mid;
			Station_read(Stationcur2,Station_mid);
			mergesort(Station_mid.Train_hsh+1,Station_mid.Train_hsh+Station_mid.Num+1);
			vector<uint> vec;
			
			int cur1=1,cur2=1;
			
			while(cur1<=Station_mid.Num&&cur2<=Station_T.Num) {
				if(Station_mid.Train_hsh[cur1]==Station_T.Train_hsh[cur2]) {
					vec.push_back(Station_mid.Train_hsh[cur1]);
					cur1++;cur2++;
				}
				else {if(Station_mid.Train_hsh[cur1]<Station_T.Train_hsh[cur2]) cur1++;else cur2++;}
			}
			int _SZ=vec.size();
			for(int T2=0;T2<_SZ;++T2) {
//				if(it2.end()) break;
//				if(it2.key().first!=mid.hsh) break;
				if(Station_S.Train_hsh[T1]==vec[T2]) {continue;} 
				vector<int> ___v;
				mp_Train.find(vec[T2],___v);
				int traincur2=___v[0];
				Train tmp2;
				Train_read(traincur2,tmp2);
				
//				if(tmp2.released!=1) {it2++;continue;}
				
				int l=-1,r=-1;
				for(int j=0;j<tmp2._stationNum;++j) {
					if(tmp2._stations[j].hsh==mid.hsh) l=j;
					if(tmp2._stations[j].hsh==T.hsh) r=j; 
				}
				if(l==-1||r==-1||l>=r) {
					continue;
				}
				int Sum2=0;
				int Min2=1e9;
				for(int j=l;j<r;++j){
					Sum2+=tmp2._prices[j];
					//Min2=min(Min2,tmp2._seatNum-tmp2._ticketsSold);
				}
//				int totdays=get_date(tmp2._st,tmp2._ed);
				std::pair<int,int> leavemid1,leavemid2,nwd2;
				leavemid1=plusdate_2(tmp2._st,tmp2.g[l][1]);
				leavemid2=tmp2.tim[l][1];
				if(std::make_pair(leavemid1,leavemid2)<std::make_pair(arrmid1,arrmid2)) {
//					int tot=0;
					leavemid1=plusdate_2(leavemid1,get_date(leavemid1,arrmid1));
					if(std::make_pair(leavemid1,leavemid2)<std::make_pair(arrmid1,arrmid2)) leavemid1=plusdate_2(leavemid1,1);
					
				}
				//////////////////////////////////////////////
					nwd2=mns(leavemid1,tmp2.g[l][1]);
					if(!pd(tmp2._st,tmp2._ed,nwd2)) {continue;}
					answer_to_transfer o;
					int ___d=get_date(tmp2._st,nwd2);
					Seat _temp2;
					Seat_read(tmp2._ticketsSold[___d],_temp2); 
					for(int k=l;k<r;++k) {
						Min2=min(Min2,tmp2._seatNum-_temp2._Sold[k]);
					}
					o.arr1=plusdate_2(nwd2,tmp2.g[r][0]);
					o.arr2=tmp2.tim[r][0];
					o.lev1=plusdate_2(nwd,tmp.g[ok][1]);
					o.lev2=tmp.tim[ok][1];
					o.price=Sum1+Sum2;
					o.seat1=Min1;
					o.seat2=Min2;
					o.time1=get_time(o.lev1,o.lev2,arrmid1,arrmid2);
					o.time2=get_time(leavemid1,leavemid2,o.arr1,o.arr2);
					o.time=get_time(o.lev1,o.lev2,o.arr1,o.arr2);
					o.arrmid1=arrmid1;
					o.arrmid2=arrmid2;
					o.leavemid1=leavemid1;
					o.leavemid2=leavemid2;
					o.mid=mid;
					o.price1=Sum1;
					o.price2=Sum2;
					memcpy(o.s1,tmp._trainID,sizeof(tmp._trainID));
					memcpy(o.s2,tmp2._trainID,sizeof(tmp2._trainID));
					std::pair<answer_to_transfer,int> res;
//					assert(o.time>0);
					if(tp==0)res=std::make_pair(o,o.time);
					else res=std::make_pair(o,o.price); 
					if(Compare(res,ans)) ans=res; 
					exist=1;	
				continue;
			}
		}
	}
	if(exist==0) {
		std::cout<<0<<"\n";
		return ; 
	}
		answer_to_transfer anss=ans.first;
		printf("%s %s ",anss.s1,S.name);
		put_date(anss.lev1);
		std::cout<<" ";
		put_time(anss.lev2);
		std::cout<<" -> ";
		printf("%s ",anss.mid.name);
		put_date(anss.arrmid1);
		std::cout<<" ";
		put_time(anss.arrmid2);
		std::cout<<" "<<anss.price1<<" "<<anss.seat1<<"\n";
		printf("%s %s ",anss.s2,anss.mid.name);
		put_date(anss.leavemid1);
		std::cout<<" ";
		put_time(anss.leavemid2);
		std::cout<<" -> ";
		printf("%s ",T.name);
		put_date(anss.arr1);
		std::cout<<" ";
		put_time(anss.arr2);
		std::cout<<" "<<anss.price2<<" "<<anss.seat2<<"\n";
}
void buy_ticket() {
	char ch=getchar();
	std::pair<int,int> date;uint user_id,train_id;
	station S,T; 
	int num=0;
	int wishtowait=0; 
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='u') {
			char s[25];
			scanf("%s",s);
			user_id=gethsh(s);
			continue;
		}
		if(ch=='i') {
			char s[25];
			scanf("%s",s);
			train_id=gethsh(s);
			continue;
		} 
		if(ch=='d') {
			char s[21];
			scanf("%s",s);
			date.first=(s[0]-'0')*10+s[1]-'0';
			date.second=(s[3]-'0')*10+s[4]-'0';
			continue;
		}
		if(ch=='n') {
			scanf("%d",&num); 
			continue;
		}
		if(ch=='f') {
			scanf("%s",S.name);
			S.init();
			continue;
		}
		if(ch=='t') {
			scanf("%s",T.name);
			T.init();
			continue;
		}
		if(ch=='q') {
			char s[21];
			scanf("%s",s+1);
			if(s[1]=='t') wishtowait=1;
			continue; 
		}
	}
	if(!mp_Train.count(train_id)) {
		std::cout<<-1<<"\n";
		return ;
	}
	if(!mp.count(user_id)) {
		std::cout<<-1<<"\n";
		return ;
	} 
	vector<int> _v;
	mp.find(user_id,_v);
	int curuser=_v[0];
	User U;
	User_read(curuser,U);
	if(!U._login) {
		std::cout<<-1<<"\n";
		return ;
	}
	vector<int> __v;
	mp_Train.find(train_id,__v);
	int traincur=__v[0];
	Train tmp;
	Train_read(traincur,tmp);
	if(tmp.released!=1) {
		std::cout<<-1<<'\n';
		return ;
	} 
	int l=-1,r=-1;
	for(int i=0;i<tmp._stationNum;++i) {
		if(tmp._stations[i].hsh==S.hsh) l=i;
		if(tmp._stations[i].hsh==T.hsh) r=i;
	} 
	if(l==-1||r==-1) {
		std::cout<<-1<<"\n";
		return ;
	} 
	if(l>=r) {
		std::cout<<-1<<"\n";
		return ;
	}
	date=mns(date,tmp.g[l][1]);
	if(date<tmp._st||date>tmp._ed) {std::cout<<-1<<"\n";return ;}
	int tt=get_date(tmp._st,date);//day number
	long long  totNum=0;
	int ok=1;
	Seat _temp;
	Seat_read(tmp._ticketsSold[tt],_temp); 
	for(int i=l;i<r;++i) {
		totNum+=tmp._prices[i];
		if(tmp._seatNum-_temp._Sold[i]<num) ok=0;  
	}
	if(ok) {
		std::cout<<totNum*num<<"\n"; 
		for(int i=l;i<r;++i) {
			_temp._Sold[i]+=num;  
		}
		Seat_write(tmp._ticketsSold[tt],_temp);
		order nw;
		nw.user_id=user_id;
		nw.train_id=train_id;
		nw.date=date;
		nw.s=S;
		nw.t=T;
		nw.Num=num;
		nw.status=1;
		nw.price=totNum;
		nw.l=l;
		nw.r=r;
		int sz=Order_mp.size()+1;
		Order_mp.insert(std::make_pair(user_id,Mod1-sz),order_write(-1,nw));
	}
	else if(!wishtowait||num>tmp._seatNum) {
		std::cout<<-1<<"\n";
		return ; 
	}
	else {
		std::cout<<"queue\n";
		order nw;
		nw.user_id=user_id;
		nw.train_id=train_id;
		nw.date=date;
		nw.s=S;
		nw.t=T;
		nw.Num=num;
		nw.status=0;
		nw.price=totNum;
		nw.l=l;
		nw.r=r;
		int sz=Order_mp.size()+1;
		int _pos=order_write(-1,nw);
		Wait_mp.insert(std::make_pair(train_id,sz),_pos);
		Order_mp.insert(std::make_pair(user_id,Mod1-sz),_pos);
	}
}
void query_order() {
	char ch=getchar();
	uint user_id;
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='u') {
			char s[50];
			scanf("%s",s);
			user_id=gethsh(s);
			continue;
		}
	}
	if(!mp.count(user_id)) {
		std::cout<<-1<<'\n';
		return;
	}
	vector<int> _v;
	mp.find(user_id,_v);
	int curuser=_v[0];
	User tmp;
	User_read(curuser,tmp);
	if(!tmp._login) {
		std::cout<<-1<<"\n";
		return ;
	}
	BPT<std::pair<uint,int>,int>::iterator it;
	it=Order_mp.nextData(std::make_pair(user_id,-1));
	int Sum=0;
	vector<int> _pos;
	while(1) {
		if(it.end()) break;
		if(it.key().first!=user_id) break;
		_pos.push_back(it.data());
		it++;
		Sum++;
		
	}
	std::cout<<Sum<<"\n";
	for(int i=0;i<Sum;++i)  {
		order tmp;
		order_read(_pos[i],tmp);
		if(tmp.status==1) std::cout<<"[success] ";
		if(tmp.status==0) std::cout<<"[pending] ";
		if(tmp.status==-1) std::cout<<"[refunded] ";
		vector<int> __v;
		mp_Train.find(tmp.train_id,__v);
		int curtrain=__v[0];
		Train t;
		Train_read(curtrain,t);
		printf("%s %s ",t._trainID,tmp.s.name);
		std::pair<int,int> t1,t2,t3,t4;
		t1=plusdate_2(tmp.date,t.g[tmp.l][1]);
		t3=plusdate_2(tmp.date,t.g[tmp.r][0]);
		t2=t.tim[tmp.l][1];
		t4=t.tim[tmp.r][0];
		put_date(t1);
		std::cout<<" ";
		put_time(t2);
		std::cout<<" -> ";
		printf("%s ",tmp.t.name);
		put_date(t3);
		std::cout<<" ";
		put_time(t4);
		std::cout<<" "<<tmp.price<<" "<<tmp.Num<<"\n"; 
	}
}
inline void Check_waitlist(uint tmp) {
	BPT<std::pair<uint,int>,int>::iterator it; 
	it=Wait_mp.nextData(std::make_pair(tmp,-1));
	vector<int> __v;
	mp_Train.find(tmp,__v);
	int traincur=__v[0];
	Train o;
	Train_read(traincur,o); 
	while(1) {
		if(it.end()) break;
		if(it.key().first!=tmp) break;
		order nw;
		order_read(it.data(),nw);
		if(nw.status!=0) {it++;continue;}
		int ok=1;
		int dd=get_date(o._st,nw.date);
		Seat _temp;
		Seat_read(o._ticketsSold[dd],_temp); 
		for(int i=nw.l;i<nw.r;++i) {
			if(o._seatNum-_temp._Sold[i]<nw.Num) ok=0;
		}
		if(!ok) {it++;continue;}
		for(int i=nw.l;i<nw.r;++i) {
			_temp._Sold[i]+=nw.Num;
		}
		Seat_write(o._ticketsSold[dd],_temp);
		nw.status=1;
		order_write(it.data(),nw);
		it++;
	}
}
void refund_ticket() {
	char ch=getchar();
	uint user_id;
	int No=1;
	for(;;ch=getchar() ) {
		if(ch=='\n') {break;}
		if(ch<'a'||ch>'z') continue;
		if(ch=='u') {
			char s[50];
			scanf("%s",s);
			user_id=gethsh(s);
			continue;
		}
		if(ch=='n') {
			scanf("%d",&No);
			continue;
		}
	}
	if(!mp.count(user_id)) {
		printf("-1\n");
		return ; 
	}
	vector<int> _v;
	mp.find(user_id,_v);
	int curuser=_v[0];
	User _user_;
	User_read(curuser,_user_);
	if(!_user_._login||No<=0) {
		printf("-1\n");
		return ; 
	}
	BPT<std::pair<uint,int>,int>::iterator it,it2;
	it=Order_mp.nextData(std::make_pair(user_id,-1));
	while(1) {
		if(it.end()) break ;
		if(it.key().first!=user_id) break ;
		No--;
		if(!No) {
			order u;
			order_read(it.data(),u);
			if(u.status==-1) {
				printf("-1\n");
				return ;
			}
			vector<int> ___v;
			mp_Train.find(u.train_id,___v);
			int traincur=___v[0];
			Train tmp;
			Train_read(traincur,tmp);
			int dd=get_date(tmp._st,u.date);
			Seat _temp;
			Seat_read(tmp._ticketsSold[dd],_temp);
			for(int i=u.l;i<u.r;++i) {
				_temp._Sold[i]-=u.Num*u.status;
			} u.status=-1;
			order_write(it.data(),u);
			Seat_write(tmp._ticketsSold[dd],_temp);
			printf("0\n");
			Check_waitlist(u.train_id); 
			return ; 
		}
		it++;
	}
	printf("-1\n");
}
void exit() {
	BPT<uint,int>::iterator it = mp.begin();
	for(;!it.end();it++) {
		int cur=it.data();
		User tmp;
		User_read(cur,tmp);
		tmp._login=0;
		User_write(cur,tmp);
	}
	printf("bye\n");
}
void clean() {
	mp.clear();
	mp_Train.clear();
	Order_mp.clear();
	Wait_mp.clear();
	Station_mp.clear();
	fileuser.close();
	fileuser.open("User",std::fstream::out|std::ios::trunc);
	fileuser.close();
	fileuser.open("User",std::ios::in|std::ios::out|std::ios::binary);
	filetrain.close();
	filetrain.open("Train",std::fstream::out|std::ios::trunc);
	filetrain.close();
	filetrain.open("Train",std::ios::in|std::ios::out|std::ios::binary);
	fileorder.close();
	fileorder.open("Order",std::fstream::out|std::ios::trunc);
	fileorder.close();
	fileorder.open("Order",std::ios::in|std::ios::out|std::ios::binary);
	fileseat.close();
	fileseat.open("Seat",std::fstream::out|std::ios::trunc);
	fileseat.close();
	fileseat.open("Seat",std::ios::in|std::ios::out|std::ios::binary);
	filestation.close();
	filestation.open("Station",std::fstream::out|std::ios::trunc);
	filestation.close();
	filestation.open("Station",std::ios::in|std::ios::out|std::ios::binary);
	printf("0\n");
}
int main() {
//	freopen("3.in","r",stdin);
//	freopen("1.ans","w",stdout);
	create_file();
	while(1) {
		_counter++;
		char s[20];
		if(scanf("%s",s)==EOF) return 0;
		int len1=strlen(s);
		int tp=-1;
		for(int i=0;i<=15;++i){
			if(len1==strlen(instructions[i])) {
				int ok=1;
				for(int j=0;j<len1;++j) if(s[j]!=instructions[i][j]) ok=0;
				if(ok) {tp=i;break;}
			}
		}
		switch(tp) {
			case 0:add_user();break;
            case 1:login();break;
            case 2:logout();break;
            case 3:query_profile();break;
            case 4:modify_profile();break;
            case 5:add_train();break;
            case 6:release_train();break;
            case 7:query_train();break;
            case 8:delete_train();break;
            case 9:query_ticket();break;
            case 10:query_transfer();break;
            case 11:buy_ticket();break;
            case 12:query_order();break;
            case 13:refund_ticket();break;
            case 14:clean();break;
            case 15:
            {
                exit();
				fileuser.close();
				filetrain.close();
				fileorder.close();
				fileseat.close();
				filestation.close();
              	return 0;
            }
		}
	}
	
	return 0;
}

