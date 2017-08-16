//
// Created by romanov on 7/5/17.
//

#ifndef EVESTANDALONE_SERVERCONTROLLER_H
#define EVESTANDALONE_SERVERCONTROLLER_H

#include <httplib.h>
#include <cstdio>
#include <utility>
#include <utility>


namespace hdvis{

class HttpController {
public:

    typedef std::function<std::string (const httplib::Request&)> Handler;

    ~HttpController()
    {
        _server.stop();
    }

    void AddApiGetRoute(const char* pattern, HttpController::Handler handler)
    {

        _server.get(pattern, [handler](const httplib::Request &req, httplib::Response &res) {

            string jsonResult = handler(req);
            res.set_content(jsonResult, "application/json");
        });
    }


    std::string DumpHeaders(const httplib::MultiMap &headers)
    {
        std::string s;
        char buf[BUFSIZ];

        for (auto it = headers.begin(); it != headers.end(); ++it) {
            const auto &x = *it;
            snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
            s += buf;
        }

        return s;
    }



    std::string Log(const httplib::Request &req, const httplib::Response &res)
    {
        std::string s;
        char buf[BUFSIZ];

        s += "================================\n";

        snprintf(buf, sizeof(buf), "%s %s", req.method.c_str(), req.path.c_str());
        s += buf;

        std::string query;
        for (auto it = req.params.begin(); it != req.params.end(); ++it) {
            const auto &x = *it;
            snprintf(buf, sizeof(buf), "%c%s=%s",
                     (it == req.params.begin()) ? '?' : '&', x.first.c_str(), x.second.c_str());
            query += buf;
        }
        snprintf(buf, sizeof(buf), "%s\n", query.c_str());
        s += buf;

        // s += DumpHeaders(req.headers);

        s += "--------------------------------\n";

        snprintf(buf, sizeof(buf), "%d\n", res.status);
        s += buf;
        // s += DumpHeaders(res.headers);

        if (!res.body.empty()) {
            s += res.body;
        }

        s += "\n";


        return s;
    }

    void StartListening()
    {
        _runThread = std::thread([&](){this->StartServer();});

    }

protected:
    void StartServer() {
        using namespace httplib;

        _server.set_base_dir(baseDir.c_str());


        _server.get("/", [=](const Request &req, Response &res) {
            res.set_redirect("/event.html");
        });

        _server.get("/hi", [](const Request &req, Response &res) {
            std::cout<<"I saw /hi"<<endl;
            res.set_content("Hello World!", "text/plain");
        });

        _server.get("/dump", [&](const Request &req, Response &res) {
            res.set_content(DumpHeaders(req.headers), "text/plain");
        });

        _server.get("/stop", [&](const Request &req, Response &res) {
            _server.stop();
        });

        _server.set_error_handler([](const Request &req, Response &res) {
            const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, res.status);
            res.set_content(buf, "text/html");
        });

        /*_server.set_logger([&](const Request &req, const Response &res) {
            printf("%s", Log(req, res).c_str());
        });*/

        cout << "The server started at port " << port << "..." << endl;

        _server.listen("localhost", 5000);
    }

private:
    std::thread _runThread;
    httplib::Server _server;
    int port = 5000;
    std::string baseDir = "./www";

};
}

#endif //EVESTANDALONE_SERVERCONTROLLER_H
