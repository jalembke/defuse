#ifndef _SPTR_H
#define _SPTR_H

template < typename T > class sptr 
{
	private:
		T* ptr;
		int* count;

		inline void check_and_delete() {
			if(__sync_sub_and_fetch(count, 1) == 0) {
				delete ptr;
				delete count;
			}
		}

	public:

		sptr() : ptr(0), count(new int) { (*count) = 0; }
		sptr(T* p) : ptr(p), count(new int) { (*count) = 1; }
		sptr(const sptr<T>& other) : ptr(other.ptr), count(other.count) { __sync_add_and_fetch(count, 1); }
		~sptr() { check_and_delete(); }

		T& operator* () const { return *ptr; }
		T* operator-> () const { return ptr; }
		operator bool() const { return (ptr != 0); }

		int use_count() const { return (*count); }
		T* get() const { return ptr; }
    
		sptr<T>& operator = (const sptr<T>& other)
		{
		 	if (this != &other) {
				check_and_delete();
				ptr = other.ptr;
				count = other.count;
				__sync_add_and_fetch(count, 1);
			}
			return *this;
		}

		bool operator== (const sptr<T>& other) const { return (ptr == other.ptr); }
		bool operator!= (const sptr<T>& other) const { return (ptr != other.ptr); }
};

#endif // _SPTR_H
