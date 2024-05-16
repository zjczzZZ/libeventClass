/*
这是一个测试用的服务器，只有两个功能：
1：对于每个已连接客户端，每10秒向其发送一句hello, world
2：若客户端向服务器发送数据，服务器收到后，再将数据回发给客户端
*/
// test.cpp
#include <atomic>
#include <iostream>
#include <set>
#include <thread>
#include <vector>

#include "MultiServer.h"
using namespace std;

// 测试示例
class TestServer : public MultiServer {
private:
    vector<Conn *> vec;
    std::atomic<int> _connCount;

protected:
    // 重载各个处理业务的虚函数
    void ReadEvent(Conn *conn);
    void WriteEvent(Conn *conn);
    void ConnectionEvent(Conn *conn);
    void CloseEvent(Conn *conn, short events);

public:
    TestServer(int count) : MultiServer(count) {}
    ~TestServer() {}

    // 退出事件，响应Ctrl+C
    static void QuitCb(int sig, short events, void *data);
    // 定时器事件，每10秒向所有客户端发一句hello, world
    static void TimeOutCb(int id, int short events, void *data);
    void printConnCount();
};

void TestServer::ReadEvent(Conn *conn) {
    conn->MoveBufferData();
}

void TestServer::WriteEvent(Conn *conn) {
}

void TestServer::ConnectionEvent(Conn *conn) {
    TestServer *me = (TestServer *)conn->GetThread()->tcpConnect;
    std::cout << "new connection: " << conn->GetFd() << " threadnum: " << std::this_thread::get_id() << std::endl;
    me->vec.push_back(conn);
    ++_connCount;
}

void TestServer::CloseEvent(Conn *conn, short events) {
    printf("connection closed: %d\n", conn->GetFd());
    --_connCount;
}

void TestServer::QuitCb(int sig, short events, void *data) {
    printf("Catch the SIGINT signal, quit in one second\n");
    TestServer *me = (TestServer *)data;
    timeval tv = {1, 0};
    me->StopRun(&tv);
}

void TestServer::TimeOutCb(int id, short events, void *data) {
    //    TestServer *me = (TestServer *)data;
    //    char temp[33] = "hello, world\n";
    //    for (int i = 0; i < me->vec.size(); i++)
    //        me->vec[i]->AddToWriteBuffer(temp, strlen(temp));
    TestServer *me = (TestServer *)data;
    me->printConnCount();
}

void TestServer::printConnCount() {
    std::cout << " =======connNum=======" << _connCount << std::endl;
}

int main() {
    printf("pid: %d\n", getpid());
    TestServer server(4);
    server.AddSignalEvent(SIGINT, TestServer::QuitCb);
    timeval tv = {2, 0};
    server.AddTimerEvent(TestServer::TimeOutCb, tv, false);
    // std::thread t(&TestServer::printConnCount, &server);
    server.SetPort(9099);
    server.StartRun();
    printf("done\n");
    return 0;
}