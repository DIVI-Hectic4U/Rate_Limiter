#include "../../include/http/HttpServer.h"

#include <crow.h>

HttpServer::HttpServer(IRateLimiter& limiter)
        :limiter_(limiter)
{
}

void HttpServer::start()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([]()
    {
        return "Rate Limiter Service";
    });

    CROW_ROUTE(app, "/rate-limit")
        .methods(crow::HTTPMethod::Post)
    ([this](const crow::request& req)
    {
        auto body = crow::json::load(req.body);

        if(!body)
        {
            return crow::response(400, "Invalid JSON payload");
        }

        if(!body.has("clientId"))
        {
            return crow::response(400, "Missing 'clientId' key in JSON");
        }

        std::string clientId = body["clientId"].s();

        bool allowed = limiter_.allowRequest(clientId);

        crow::json::wvalue response;
        response["allowed"] = allowed;

        return crow::response(
            allowed ? 200 : 429,
            response
        );
    });

    app.bindaddr("127.0.0.1")
        .port(8080)
        .multithreaded()
        .run();
    
}