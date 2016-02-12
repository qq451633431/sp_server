#ifndef __UDPLIST_H__
#define __UDPLIST_H__

class udpUser
{
public:
    string name;
    int State;
    udpUser *ptr;
    udpUser(string s,udpUser *p)
    {
        name = s;
        ptr = p;
    }
    int updateState()
    {
        return State = abs(((State = (~State + 0x14fb) * 0x1f) >> 16) ^ State);
    }
};

class udpList{
    private:
    udpUser *head;
    public:
    udpList()
    {
        head = 0;
    }
    void insert(string s)
    {
        udpUser *currPtr = head, *prevPtr = 0;
		while(currPtr != NULL && s.compare(currPtr->name) < 0)
        {
            prevPtr = currPtr;
            currPtr = currPtr->ptr;
        }
        if(prevPtr == NULL)
            head = new udpUser(s,currPtr);
        else
         prevPtr->ptr = new udpUser(s,currPtr);
    }
    int updateState(string s)
    {
        udpUser *currPtr = head;
        while(currPtr != NULL && s.compare(currPtr->name) != 0)
            currPtr = currPtr->ptr;
        if(currPtr)
            return currPtr->updateState();
        else {
			cout << "Unable to update " << s.c_str() << "state : UDPLIST" << endl;
        return 0;
        }
    }
	int updateState(int room, int slot)
	{	

	}
};

#endif
