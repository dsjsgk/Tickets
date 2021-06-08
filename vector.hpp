#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>
#include <cmath>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	 size_t capacity;
	 size_t now_size;
	 T **data;

	 void resize(){
        capacity *= 2;
        T **new_data = data;
        data = new T *[capacity];
        for(int i = 0; i < capacity; ++i){
            if(i < now_size)data[i] = new_data[i];
            else data[i] = nullptr;
        }
        delete [] new_data;
	 }

	class const_iterator;
	class iterator {
	    friend vector<T>;
	    friend const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		 T **pst = nullptr;
		 int pos;
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator() = default;

		iterator(const iterator &obj) = default;

		iterator &operator=(const iterator &obj) = default;

        iterator(T ** _ptr,int _pos):pst(_ptr),pos(_pos){}

		iterator operator+(const int &n) const {
			//TODO
            iterator tmp;
            tmp.pst = pst;
            tmp.pos = pos + n;
            return tmp;
		}
		iterator operator-(const int &n) const {
			//TODO
            iterator tmp;
            tmp.pst = pst;
            tmp.pos = pos - n;
            return tmp;
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			if(rhs.pst != pst)throw("invaild_iterator");
			return fabs(pos - rhs.pos);
		}
		iterator& operator+=(const int &n) {
			//TODO
            pos += n;
            return *this;
		}
		iterator& operator-=(const int &n) {
			//TODO
			pos -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
		    iterator tmp(*this);
		    pos ++;
            return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
		    pos ++;
            return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            iterator tmp(*this);
            pos --;
            return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
		    pos --;
		    return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
            return *(pst[pos]);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
		    return (pst == rhs.pst && pos == rhs.pos);
		}
		bool operator==(const const_iterator &rhs) const {
		    return (pst == rhs.pst && pos == rhs.pos);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
		    if(rhs == *this)return false;
		    else return true;
		}
		bool operator!=(const const_iterator &rhs) const {
            if(rhs == *this)return false;
            else return true;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator {
	    friend vector<T>;
    private:
	    T **pst = nullptr;
	    int pos;

    public:
	    const_iterator() = default;
        const_iterator(const iterator & obj):pst(obj.pst),pos(obj.pos){}
        const_iterator(const const_iterator &obj) = default;
        const_iterator(T **pt,int ps):pst(pt),pos(ps){}
        const_iterator &operator=(const iterator &obj){
            pst = obj.pst;
            pos = obj.pos;
        }
	    const_iterator &operator=(const const_iterator &obj) = default;


        const_iterator operator+(const int &n) const {
            //TODO
            const_iterator tmp;
            tmp.pst = pst;
            tmp.pos = pos + n;
            return tmp;
        }
        const_iterator operator-(const int &n) const {
            //TODO
            const_iterator tmp;
            tmp.pst = pst;
            tmp.pos = pos - n;
            return tmp;
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            //TODO
            if(rhs.pst != pst)throw("invaild_iterator");
            return fabs(pos - rhs.pos);
        }
        const_iterator & operator+=(const int &n) {
            //TODO
            pos += n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            //TODO
            pos -= n;
            return *this;
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            pos ++;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++() {
            pos ++;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            iterator tmp(*this);
            pos --;
            return tmp;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() {
            pos --;
            return *this;
        }
        /**
         * TODO *it
         */
        T& operator*() const{
            return *(pst[pos]);
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return (pst == rhs.pst && pos == rhs.pos);
        }
        bool operator==(const const_iterator &rhs) const {
            return (pst == rhs.pst && pos == rhs.pos);
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            if(rhs == *this)return false;
            else return true;
        }
        bool operator!=(const const_iterator &rhs) const {
            if(rhs == *this)return false;
            else return true;
        }
	};
	/**
	 * TODO Constructs
	 * Atleast two: default constructor, copy constructor
	 */
	vector() {
        capacity = 10;
	    data = new T *[capacity];
	    for(int i = 0;i < capacity; ++i){
	        data[i] = nullptr;
	    }
	    now_size = 0;
	}
	vector(const vector &other) {
        capacity = other.capacity;
        now_size = other.now_size;
        data = new T *[capacity];
        for(int i = 0; i < capacity; ++i){
            if(i < now_size)data[i] = new T (*(other.data[i]));
            else data[i] = nullptr;
        }
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
        if(data){
            for(int i = 0;i < capacity; ++i){
                if(data[i])delete data[i];
            }
            delete [] data;
        }
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other){
	    if(this == &other)return (*this);
	    clear();
        capacity = other.capacity;
        now_size = other.now_size;
        data = new T *[capacity];
        for(int i = 0; i < capacity; ++i){
            if(i < now_size)data[i] = new T (*(other.data[i]));
            else data[i] = nullptr;
        }
	    return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
	    if(pos < 0 || pos >= now_size)throw "index_out_of_bound";
	    return *(data[pos]);
	}
	const T & at(const size_t &pos) const {
        if(pos < 0 || pos >= now_size)throw "index_out_of_bound";
        return *(data[pos]);
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
	    return at(pos);
	}
	const T & operator[](const size_t &pos) const {
        return at(pos);
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
	    if(now_size == 0)throw "container_is_empty";
	    return * data[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
        if(now_size == 0)throw "container_is_empty";
        return * data[now_size - 1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
	    iterator tmp;
	    tmp.pst = data;
	    tmp.pos = 0;
	    return tmp;
	}
	const_iterator cbegin() const {
	    const_iterator tmp;
        tmp.pst = data;
        tmp.pos = 0;
        return tmp;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
        iterator tmp;
        tmp.pst = data;
        tmp.pos = now_size;
        return tmp;
	}
	const_iterator cend() const {
        const_iterator tmp;
        tmp.pst = data;
        tmp.pos = now_size;
        return tmp;
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {
	    return (now_size == 0);
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
        return now_size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
        if(data == nullptr)return;
	    for(int i = 0; i < capacity; ++i){
            if(data[i] != nullptr)delete data[i];
	    }
        delete [] data;
	    now_size = 0;
	    capacity = 10;
	    data = nullptr;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
	    now_size += 1;
	    if(now_size == capacity)resize();
	    iterator it(pos);
	    for(int i = now_size - 1; i > pos.pos; --i){
	        data[i] = data[i - 1];
	    }
	    data[it.pos] = new T (value);
	    return it;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
	    if(ind < 0 || ind > now_size)throw("index_out_of_bound");
	    now_size += 1;
	    for(int i = now_size - 1; i > ind; --i){
	        data[i] = data[i - 1];
	    }
	    data[ind] = new T (value);
	    if(now_size == capacity)resize();
	    iterator it(data,ind);
	    return it;
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
	    iterator it(pos);
	    now_size --;
	    if(now_size == 0)return end();
        delete data[pos.pos];
	    for(int i = pos.pos; i < now_size; ++i){
	        data[i] = data[i + 1];
	    }
	    data[now_size] = nullptr;
	    return it;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
        if(ind < 0 || ind >= now_size)throw("index_out_of_bound");
        now_size --;
        if(now_size == 0)return end();
        delete data[ind];
        for(int i = ind; i < now_size; ++i){
            data[i] = data[i + 1];
        }
        data [now_size] = nullptr;
        iterator it(data,ind);
        return it;
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
	    data[now_size ++] = new T(value);
	    if(now_size == capacity)resize();
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
        if(size() == 0)throw "container_is_empty";
        delete data[now_size - 1];
        data[now_size - 1] = nullptr;
        now_size --;
	}
};


}

#endif
