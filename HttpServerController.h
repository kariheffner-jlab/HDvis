//
// Created by romanov on 7/5/17.
//

#ifndef EVESTANDALONE_SERVERCONTROLLER_H
#define EVESTANDALONE_SERVERCONTROLLER_H

#include <httplib.h>
#include <cstdio>

using namespace httplib;

class HttpServerController {
public:

    ~HttpServerController(){
        svr.stop();
    }


    std::string dump_headers(const MultiMap &headers) {
        std::string s;
        char buf[BUFSIZ];

        for (auto it = headers.begin(); it != headers.end(); ++it) {
            const auto &x = *it;
            snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
            s += buf;
        }

        return s;
    }



    std::string log(const Request &req, const Response &res) {
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

        s += dump_headers(req.headers);

        s += "--------------------------------\n";

        snprintf(buf, sizeof(buf), "%d\n", res.status);
        s += buf;
        s += dump_headers(res.headers);

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



        svr.set_base_dir(baseDir.c_str());


        svr.get("/", [=](const Request &req, Response &res) {
            res.set_redirect("/hi");
        });

        svr.get("/hi", [](const Request &req, Response &res) {
            std::cout<<"I saw /hi"<<endl;
            res.set_content("Hello World!", "text/plain");
        });

        svr.get("/dump", [&](const Request &req, Response &res) {
            res.set_content(dump_headers(req.headers), "text/plain");
        });

        svr.get("/stop", [&](const Request &req, Response &res) {
            svr.stop();
        });



        //std::ifstream t("file.txt");
        //std::string str((std::istreambuf_iterator<char>(t)),
        //              std::istreambuf_iterator<char>());


        svr.set_error_handler([](const Request &req, Response &res) {
            const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, res.status);
            res.set_content(buf, "text/html");
        });

        svr.set_logger([&](const Request &req, const Response &res) {
            printf("%s", log(req, res).c_str());
        });

        cout << "The server started at port " << port << "...";

        svr.listen("localhost", 5000);
    }

private:
    std::thread _runThread;
    Server svr;
    int port = 5000;
    std::string baseDir = "./www";

};


#endif //EVESTANDALONE_SERVERCONTROLLER_H
