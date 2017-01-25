#include "netKicker.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include "mutex.h"

class NetKicker::Impl
{
public:
    Impl()
        : mMutex(),
          mWritten(0)
    {
        const int res = pipe(mPipeFds);
        if (res < 0) {
            perror("Pipe: ");
            mPipeFds[0] = -1;
            mPipeFds[1] = -1;
            return;
        }
        fcntl(mPipeFds[0], F_SETFL, O_NONBLOCK);
        fcntl(mPipeFds[1], F_SETFL, O_NONBLOCK);
    }
    void Kick()
    {
        mMutex.Lock();
        if (mWritten <= 0) {
            mWritten++;
            char buf = 'k';
            write(mPipeFds[1], &buf, sizeof(buf));
        }
		mMutex.Unlock();
    }
    int Drain()
    {
        mMutex.Lock();
        while (mWritten > 0) {
            char buf[64];
            const int res = read(mPipeFds[0], buf, sizeof(buf));
            if (res > 0) {
                mWritten -= std::min(mWritten, res);
            } else {
                break;
            }
        }
		mMutex.Unlock();
        return (mWritten);
    }
    int GetFd() const { return mPipeFds[0]; }
private:
    Mutex mMutex;
    int     mWritten;
    int     mPipeFds[2];

private:
   Impl(const Impl&);
   Impl& operator=(const Impl&); 
};

NetKicker::NetKicker()
    : mImpl(*new Impl())
{}

NetKicker::~NetKicker()
{
    delete &mImpl;
}

void
NetKicker::Kick()
{
    mImpl.Kick();
}

int 
NetKicker::Drain()
{
    return mImpl.Drain();
}

int 
NetKicker::GetFd() const
{
    return mImpl.GetFd();
}
