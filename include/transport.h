//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_TRANSPORT_H
#define LSP_TRANSPORT_H

#include "uri.h"
#include "common.h"
using value = json;
class MessageHandler {
public:
    MessageHandler() = default;
    virtual void onNotify(string_ref method, value &params) {}
    virtual void onResponse(value &ID, value &result) {}
    virtual void onError(value &ID, value &error) {}
    virtual void onRequest(string_ref method, value &params, value &ID) {}

};

class Transport {
public:
    using RequestID = int;
    virtual void notify(string_ref method, value &params) = 0;
    virtual void request(string_ref method, value &params, RequestID id) = 0;
    virtual int loop(MessageHandler &) = 0;
};

class JsonTransport : public Transport {
public:
    const char *jsonrpc = "2.0";
    int loop(MessageHandler &handler) override {
        json value;
        try {
            while (readJson(value)) {
                if (value.count("id")) {
                    if (value.contains("method")) {
                        handler.onRequest(value["method"].get<std::string>(), value["params"], value["id"]);
                    } else if (value.contains("result")) {
                        //handler.onResponse(value["id"], value["result"]);
                    } else if (value.contains("error")) {
                        handler.onError(value["id"], value["error"]);
                    }
                } else if (value.contains("method")) {
                    if (value.contains("params")) {
                        handler.onNotify(value["method"].get<std::string>(), value["params"]);
                    }
                }
            }
        } catch (std::exception &e) {
            printf("exception:%s\n", e.what());
        }
        return 0;
    }
    virtual bool readJson(value &) = 0;
    virtual bool writeJson(value &) = 0;
};

#endif //LSP_TRANSPORT_H
