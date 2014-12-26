#include <vector>
#include <iostream>

#define NOACTION ((Action*)0)

class Action {
public:
    Action(std::string aName) {
        name = aName;
    }
    virtual ~Action() {}
    virtual int Trigger() = 0;

    inline void GetName(std::string &actionName) {
        actionName = name;
    }
private:
    std::string name;
};

class doSomething : public Action {
public:
doSomething(std::string name) : Action(name) {}
    int Trigger() {
        std::cout<<"I did something!"<<std::endl;
        return 0;
    }
};

class ActionManager {
public:
    ActionManager() {
        aList.resize(32);
    }
    ~ActionManager() {
        aList.clear();
    }
    void GetName(int cmdID, std::string &actionName) {
        Action* action = aList.at(cmdID);
        if (action != NOACTION) {
            action->GetName(actionName);
        } else {
            actionName = "NULL";
        }
    }

    int Bind(int cmdID, Action &action) {
        it = aList.begin();
        for (int i=0;i<cmdID;i++) {
            ++it;
        }
        aList.insert(it,&action);
        return cmdID;
    }

    int Unbind(int cmdID) {
        aList.at(cmdID) = NOACTION;
        return cmdID;
    }

    int Trigger(int cmdID) {
        Action* action = aList.at(cmdID);
        if (action != NOACTION) {
            return action->Trigger();
        }
        return -1;
    }
private:
    std::vector<Action*> aList;
    std::vector<Action*>::iterator it;
};

int main() {
    ActionManager *aHandler = new ActionManager;

    int i=0;
    static std::string test = "Test Case #";
    for (i=0;i<100;i++) {
        aHandler->Bind(i, *(new doSomething(test)) );
    }

    for (i=0;i<100;i++) {
        std::string name;
        aHandler->GetName(i,name);
        std::cout<<name<<std::endl;
        aHandler->Trigger(i);
        aHandler->Unbind(i);
        aHandler->Trigger(i);
    }

    delete aHandler;
}
