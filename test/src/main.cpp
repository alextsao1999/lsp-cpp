#include <iostream>
#include <thread>
#include <fstream>
#include <lsp/client.h>
using namespace nlohmann;
using namespace LspCore;

bool readFile(std::string& path, std::string* data) 
{
    std::ifstream ifStream(path, std::ios::binary | std::ios::in);
    if (ifStream.is_open()) {
        ifStream.seekg(0, std::ios::end);
        int len = ifStream.tellg();
        ifStream.seekg(0, std::ios::beg);
        char* buff = new char[len];
        ifStream.read(buff, len);
        data->append(buff);
        delete[]buff;
    }
    else {
        std::cout << "file open fail!!!" << std::endl;
        return false;
    }
    return true;
}

void copyCompileCommandsFiles()
{
    std::string source = "D:/c_workstation/projects/Third_Fork_Projects/lsp-cpp/out/build/x64-Debug/compile_commands.json";
    std::string target = "D:/c_workstation/projects/Third_Fork_Projects/lsp-cpp/compile_commands.json";
    std::ifstream  src(source, std::ios::binary);
    std::ofstream  dst(target, std::ios::binary);

    dst << src.rdbuf();
}
auto debugFunc = [](value& j) {
    //value method = j.at("method");
    std::cout << "========================================\n";
    std::cout << j << std::endl;
};


int main() {
    std::string file1_uri = "file:///D:/c_workstation/projects/Third_Fork_Projects/lsp-cpp/test/src/main.cpp";
    std::string file1_path = "D:/c_workstation/projects/Third_Fork_Projects/lsp-cpp/test/src/main.cpp";
    std::string root_uri = "file:///D:/c_workstation/projects/Third_Fork_Projects/lsp-cpp";
    PipJsonIO jsonIO(R"(D:\c_workstation\soft_tool\clangd_17.0.3\bin\clangd.exe)");
    MapMessageHandler msgHandler;
    LanguageClient client(msgHandler, jsonIO);

    copyCompileCommandsFiles();
    {
        MapMessageHandler::Accessor accessor = msgHandler.access();
        accessor.bindNotify(METHOD_PublishDiagnostics.c_str(), debugFunc);
        accessor.bindNotify(METHOD_DidOpen.c_str(), debugFunc);
        accessor.bindNotify(METHOD_DidClose.c_str(), debugFunc);
    }

    std::thread thread([&] {
        client.safeLoop();
    });

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
            MapMessageHandler::Accessor accessor = msgHandler.access();
            accessor.bindResponse(client.Initialize(string_ref(root_uri)), debugFunc);
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
