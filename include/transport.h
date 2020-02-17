//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_TRANSPORT_H
#define LSP_TRANSPORT_H

#include "uri.h"
#include <functional>
using value = json;
using RequestID = int;

class MessageHandler {
public:
    MessageHandler() = default;
    virtual void onNotify(string_ref method, value &params) {}
    virtual void onResponse(value &ID, value &result) {}
    virtual void onError(value &ID, value &error) {}
    virtual void onRequest(string_ref method, value &params, value &ID) {}

};
class MapMessageHandler : public MessageHandler {
public:
    std::map<std::string, std::function<void(value &, RequestID)>> m_calls;
    std::map<std::string, std::function<void(value &)>> m_notify;
    MapMessageHandler() = default;
    template<typename Param>
    void bindRequest(const char *method, std::function<void(Param &, RequestID)> func) {
        m_calls[method] = [=](json &params, json &id) {
            Param param = params.get<Param>();
            func(param, id.get<RequestID>());
        };
    }
    template<typename Param>
    void bindNotify(const char *method, std::function<void(Param &)> func) {
        m_notify[method] = [=](json &params) {
            Param param = params.get<Param>();
            func(param);
        };
    }

    void onNotify(string_ref method, value &params) override {
        std::string str = method.str();
        if (m_notify.count(str)) {
            m_notify[str](params);
        }
    }
    void onResponse(value &ID, value &result) override {

    }
    void onError(value &ID, value &error) override {

    }
    void onRequest(string_ref method, value &params, value &ID) override {
        std::string string = method.str();
        if (m_calls.count(string)) {
            m_calls[string](params, ID);
        }
    }
};

class Transport {
public:
    virtual void notify(string_ref method, value &params) = 0;
    virtual void request(string_ref method, value &params, RequestID id) = 0;
    virtual int loop(MessageHandler &) = 0;
};

class JsonTransport : public Transport {
public:
    const char *jsonrpc = "2.0";
    int loop(MessageHandler &handler) override {
        value value;
        while (readJson(value)) {
            if (value.count("id")) {
                if (value.contains("method")) {
                    handler.onRequest(value["method"].get<std::string>(), value["params"], value["id"]);
                } else if (value.contains("result")) {
                    handler.onResponse(value["id"], value["result"]);
                } else if (value.contains("error")) {
                    handler.onError(value["id"], value["error"]);
                }
            } else if (value.contains("method")) {
                if (value.contains("params")) {
                    handler.onNotify(value["method"].get<std::string>(), value["params"]);
                }
            }
        }
        return 0;
    }
    virtual bool readJson(value &) = 0;
    virtual bool writeJson(value &) = 0;
};

#endif //LSP_TRANSPORT_H
