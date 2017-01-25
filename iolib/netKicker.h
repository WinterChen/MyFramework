

#ifndef NETKICKER_H
#define NETKICKER_H


class NetKicker {
public:
    NetKicker();
    ~NetKicker();
    /// The "write" portion of the pipe writes one byte on the fd.
    void Kick();
	int Drain();
    int GetFd() const;
private:
    class Impl;
    Impl& mImpl;
    /// This is the callback from the net-manager to drain out the
    /// bytes written on the pipe
    

};


#endif 
