#include <iostream>
#include <thread>
#include "client.h"
int main() {
    LanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
    MessageHandler my;
    std::thread thread([&] {
        client.loop(my);
    });

    auto *file = "file:///C:/Users/Administrator/Desktop/compiler4e/runtime.c";

    std::string text = "int main() {\n return 0; }";
    int res;
    while (scanf("%d", &res)) {
        if (res == 1) {
            client.Exit();
            thread.detach();
            return 0;
        }
        if (res == 2) {
            client.Initialize();
        }
        if (res == 3) {
            client.DidOpen(file, text);
        }
    }
    return 0;
}
