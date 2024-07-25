//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_TRANSPORT_H
#define LSP_TRANSPORT_H

#include <lsp/uri.h>
#include <lsp/iolayer.h>
#include <iostream>
#include <functional>
#include <utility>
#include <mutex>

namespace LspCore {

using value = json;
using RequestID = std::string;

class MessageHandler {
public:
    enum class MsgType{
        Request,
        Response,
        Error,
        Notify
    };
public:
    MessageHandler() = default;

    virtual void onAnyJsonRPC(MsgType type, value& dataPart, string_ref method = nullptr) = 0;
    virtual void onNotify(string_ref method, value &params) = 0;
    virtual void onResponse(value &ID, value &result) = 0;
    virtual void onError(value &ID, value &error) = 0;
    virtual void onRequest(string_ref method, value &params, value &ID) = 0;

};


class MapMessageHandler : public MessageHandler {
public:
    std::map<std::string, std::function<void(value &, RequestID)>> m_calls;
    std::map<std::string, std::function<void(value &)>> m_notify;
    std::vector<std::pair<RequestID, std::function<void(value &)>>> m_requests;
    std::function<void(MsgType, value&, string_ref)> m_anyJsonCallback = nullptr;
    std::mutex m_lock;

    // 用于线程安全的访问类
    class Accessor{
        MapMessageHandler& m_other;
        std::unique_lock<std::mutex> m_guard;
    public:
        explicit Accessor(MapMessageHandler& other) : m_other(other), m_guard(other.m_lock){}

        template<typename Param>
        void bindRequest(const char *method, std::function<void(Param &, RequestID)> func) {
            m_other.bindRequest(method, func);
        }
        void bindRequest(const char *method, std::function<void(value &, RequestID)> func) {
            m_other.bindRequest(method, func);
        }
        template<typename Param>
        void bindNotify(const char *method, std::function<void(Param &)> func) {
            m_other.bindNotify(method, func);
        }
        void bindNotify(const char *method, std::function<void(value &)> func) {
            m_other.bindNotify(method, func);
        }
        void bindResponse(RequestID id, std::function<void(value &)>func) {
            m_other.bindResponse(id, func);
        }
        void bindAnyJsonRPC(std::function<void(MsgType, value&, string_ref)> func){
            m_other.bindAnyJsonRPC(func);
        }

        void onAnyJsonRPC(MsgType type, value& dataPart, string_ref method = nullptr){m_other.onAnyJsonRPC(type, dataPart, method);}
        void onNotify(string_ref method, value &params){m_other.onNotify(method, params);}
        void onResponse(value &ID, value &result){m_other.onResponse(ID, result);}
        void onError(value &ID, value &error){m_other.onError(ID, error);}
        void onRequest(string_ref method, value &params, value &ID){m_other.onRequest(method,params,ID);}
    };

public:
    MapMessageHandler() = default;

    Accessor access(){
        return Accessor(*this);
    }

    template<typename Param>
    void bindRequest(const char *method, std::function<void(Param &, RequestID)> func) {
        m_calls[method] = [=](json &params, json &id) {
            Param param = params.get<Param>();
            func(param, id.get<RequestID>());
        };
    }
    void bindRequest(const char *method, std::function<void(value &, RequestID)> func) {
        m_calls[method] = std::move(func);
    }
    template<typename Param>
    void bindNotify(const char *method, std::function<void(Param &)> func) {
        m_notify[method] = [=](json &params) {
            Param param = params.get<Param>();
            func(param);
        };
    }
    void bindNotify(const char *method, std::function<void(value &)> func) {
        m_notify[method] = std::move(func);
    }
    void bindResponse(RequestID id, std::function<void(value &)>func) {
        m_requests.emplace_back(id, std::move(func));
    }
    void bindAnyJsonRPC(std::function<void(MsgType, value&, string_ref)> func){
        m_anyJsonCallback = func;
    }

    void onNotify(string_ref method, value &params) override {
        std::string str = method.str();
        if (m_notify.count(str)) {
            m_notify[str](params);
        }
    }
    void onResponse(value &ID, value &result) override {
        for (int i = 0; i < m_requests.size(); ++i) {
            if (ID == m_requests[i].first) {
                m_requests[i].second(result);
                m_requests.erase(m_requests.begin() + i);
                return;
            }
        }
    }
    void onError(value &ID, value &error) override {

    }
    void onRequest(string_ref method, value &params, value &ID) override {
        std::string string = method.str();
        if (m_calls.count(string)) {
            m_calls[string](params, ID);
        }
    }
    void onAnyJsonRPC(MsgType type, value& dataPart, string_ref method = nullptr) override{
        if(m_anyJsonCallback){
            m_anyJsonCallback(type, dataPart, method);
        }
    }

};

class Transport {
public:
    virtual ~Transport(){}

    virtual void notify(string_ref method, value &params) = 0;
    virtual void request(string_ref method, value &params, RequestID &id) = 0;
    virtual int loop(MessageHandler &){return 0;}
    virtual int safeLoop() = 0;
    virtual void requestStopLoop() = 0;
};

class JsonTransport : public Transport {
private:
    MapMessageHandler& m_msgHandler;
    JsonIOLayer& m_jsonIO;

    const char *jsonrpc = "2.0";
    std::atomic<bool> havingStopRequest = false;
    std::chrono::milliseconds runningFrequency = std::chrono::milliseconds(20);
public:
    explicit JsonTransport(MapMessageHandler& msgHandler, JsonIOLayer& jsonIO) : m_msgHandler(msgHandler), m_jsonIO(jsonIO){}
    virtual ~JsonTransport(){}

    int safeLoop() override{
        while (!havingStopRequest){
            try {
                value value;
                if(m_jsonIO.isClosed()){throw "closed";}
                if (m_jsonIO.readJson(value)){
                    MapMessageHandler::Accessor accessor = m_msgHandler.access();
                    if (value.count("id")) {
                        if (value.contains("method")) {
                            accessor.onAnyJsonRPC(MessageHandler::MsgType::Request, value["params"], value["method"].get<std::string>());
                            accessor.onRequest(value["method"].get<std::string>(), value["params"], value["id"]);
                        } else if (value.contains("result")) {
                            accessor.onAnyJsonRPC(MessageHandler::MsgType::Response, value["result"], value["id"].get<std::string>());
                            accessor.onResponse(value["id"], value["result"]);
                        } else if (value.contains("error")) {
                            accessor.onAnyJsonRPC(MessageHandler::MsgType::Error, value["error"]);
                            accessor.onError(value["id"], value["error"]);
                        }
                    } else if (value.contains("method")) {
                        if (value.contains("params")) {
                            accessor.onAnyJsonRPC(MessageHandler::MsgType::Response, value["params"], value["method"].get<std::string>());
                            accessor.onNotify(value["method"].get<std::string>(), value["params"]);
                        }
                    }
                }
            } catch (nlohmann::detail::exception& e) {
                std::cout<<e.what()<<std::endl;
            }
            std::this_thread::sleep_for(runningFrequency);
        }
        return 0;
    }

    void requestStopLoop() override{
        havingStopRequest.store(true);
        m_jsonIO.close();
    }

    void notify(string_ref method, value &params) override {
        json value = {{"jsonrpc", jsonrpc},
                      {"method",  method},
                      {"params",  params}};
        m_jsonIO.writeJson(value);
    }
    void request(string_ref method, value &params, RequestID &id) override {
        json rpc = {{"jsonrpc", jsonrpc},
                    {"id",      id},
                    {"method",  method},
                    {"params",  params}};
        m_jsonIO.writeJson(rpc);
    }
};


}

#endif //LSP_TRANSPORT_H
