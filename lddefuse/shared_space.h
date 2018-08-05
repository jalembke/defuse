#ifndef SHARED_SPACE_H
#define SHARED_SPACE_H

class shared_space
{
	public:
		static void* get();
		static void* init(size_t size);
	
	private:
		shared_space() : xFd(-1), xSize(0), xPtr(NULL) {}
		shared_space(const shared_space& other);
		~shared_space();
		shared_space& operator= (const shared_space& other);
		static shared_space& getInstance();

		int xFd;
		ssize_t xSize;
		void* xPtr;
};

#endif // SHARED_SPACE_H
