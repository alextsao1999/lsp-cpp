# lsp-cpp

该项目从 "lsp-cpp项目(An easy language-server-protocol client)"Fork而来。源项目：[https://github.com/alextsao1999/lsp-cpp]
由于源项目已经不维护了，所以本fork项目独立了出来，新的项目地址：[https://github.com/firemeatman/LspClient]

源项目是一个十分简单的Lsp语言客户端，通俗易懂。官方的LspCpp有点庞大不太用的来，而此项目足够简单，适合简单使用。

本项目的目的是基于lsp-client进行完善和扩展，包括跨平台支持、模块化改造、协议扩充等。


# 1.平台支持

> 可以运行的平台
- windows
- linux(未完成)
> 与其他框架的集成使用
- Qt6.x，编译通过，正常使用。示例项目： 代码编辑器[https://github.com/firemeatman/CodeEditor]

# 2. 如何引入第三方项目
cmake

# 3. 代码示例

```c++
#include <iostream>
#include <thread>
#include <fstream>
#include <lsp/client.h>
using namespace nlohmann;
using namespace LspCore;

bool readFile(std::string& path, std::string* data) 
{
    // 这里就不实现了
    return true;
}

void copyCompileCommandsFiles()
{
    // 这里就不实现了
}
auto debugFunc = [](value& j) {
    //value method = j.at("method");
    std::cout << "========================================\n";
    std::cout << j << std::endl;
};

int main() {
    std::string file1_uri = "file:///yourTestCodeFilePath";
    std::string file1_path = "yourTestCodeFilePath";
    std::string root_uri = "file:///yourTestCodeFileRootPath";
    // 1. 创建Lsp客户端
    PipJsonIO jsonIO(R"(clangd.exe)"); // Lsp服务器路径, 这里我用的是Clangd.
    MapMessageHandler msgHandler;
    LanguageClient client(msgHandler, jsonIO);
    // 2. 复制compile_commands.json到工作区, clangd需要这个文件；通过命令应该也能指定该文件路径和clangd工作文件夹
    copyCompileCommandsFiles();

    // 3. 绑定一些通知消息的回调函数
    {
        MapMessageHandler::Accessor accessor = msgHandler.access(); // 访问者模式访问MapMessageHandler的接口，线程安全
        accessor.bindNotify(METHOD_PublishDiagnostics.c_str(), debugFunc);
        accessor.bindNotify(METHOD_DidOpen.c_str(), debugFunc);
        accessor.bindNotify(METHOD_DidClose.c_str(), debugFunc);
    }
    // 4. 线程中运行Lsp客户端
    std::thread thread([&] {
        client.safeLoop();
    });

    // 5. 测试与本地Lsp服务器的通信
    int res;
    while (scanf("%d", &res)) {
        
        switch (res)
        {
        case 0: // 退出
        {
            client.Exit();
            client.requestStopLoop();
            thread.join();
            return 0;
            break;
        }
        case 1: // 初始化
        {
            MapMessageHandler::Accessor accessor = msgHandler.access();// 访问者模式访问MapMessageHandler的接口，线程安全
            accessor.bindResponse(client.Initialize(string_ref(root_uri)), debugFunc);// 发出初始化请求消息，并绑定回调函数
            break;
        }
        case 2: // 打开文件
        {
            std::string text;
            readFile(file1_path, &text);
            client.DidOpen(file1_uri, text);
            break;
        }
        case 3: // 关闭文件
        {
            client.DidClose(string_ref(file1_uri));
            break;
        }
        case 4: // 获取词语列表
        {
            MapMessageHandler::Accessor accessor = msgHandler.access();
            accessor.bindResponse(client.SemanticTokensALL(string_ref(file1_uri)), debugFunc);
            break;
        }
        case 5: // 悬停信息
        {
            MapMessageHandler::Accessor accessor = msgHandler.access();
            Position pos{8, 13};
            accessor.bindResponse(client.Hover(string_ref(file1_uri), pos), debugFunc);
            break;
        }
        default:
            break;
        }
    }
    return 0;
}

```
