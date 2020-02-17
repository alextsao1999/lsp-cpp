#include <iostream>
#include <thread>
#include "client.h"
int main() {
    LanguageClient client(R"(F:\LLVM\bin\clangd.exe)");
    MapMessageHandler my;

    my.bindNotify<ShowMessageParams>("window/showMessage", [](auto &params) {
        printf("message -> %s\n", params.message.c_str());
    });

    std::thread thread([&] {
        client.loop(my);
    });

    string_ref file = "file:///C:/Users/Administrator/Desktop/test.c";

    std::string text = "int main() { return 0; }\n";
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
            client.Sync();
        }
        if (res == 4) {
            client.Formatting(file);
        }
    }
    return 0;
}
