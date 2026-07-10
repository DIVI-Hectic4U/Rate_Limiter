#include "../../include/http/HttpServer.h"

#include <crow.h>

HttpServer::HttpServer(IRateLimiter& limiter)
        :limiter_(limiter)
{
}

void HttpServer::setupRoutes(crow::SimpleApp& app)
{
    // 1.Health Check Endpoint (For Docker/Kubernetes)
    CROW_ROUTE(app,"/health")
    ([]()
    {
        crow::json::wvalue response;
        response["status"] = "UP";
        return crow::response(200,response);
    });

    // 2.Versioned API EndPoint
    CROW_ROUTE(app, "/api/v1/rate-limit")
        .methods(crow::HTTPMethod::Post)
    ([this](const crow::request& req)
    {
        auto body = crow::json::load(req.body);

        if(!body)
        {
            crow::json::wvalue error;
            error["error"] = "Invalid JSON payload";
            return crow::response(400, error);
        }

        if(!body.has("clientId"))
        {
            crow::json::wvalue error;
            error["error"] = "Missing 'clientId' key in JSON.";
            return crow::response(400,error);
        }

        std::string clientId = body["clientId"].s();

        bool allowed = limiter_.allowRequest(clientId);

        crow::json::wvalue body_response;
        body_response["clientId"] = clientId;
        body_response["allowed"] = allowed;

        // 3. Construct the response with proper status codes
        auto res = crow::response(allowed ? 200 : 429, body_response);

        // 4. Inject Standard Rate Limit HTTP headers
        res.add_header("X-RateLimit-Allowed", allowed ? "true" : "false");

        return res;
    });
}

void HttpServer::start()
{
    crow::SimpleApp app;
    
    setupRoutes(app);
    
    app.bindaddr("127.0.0.1")
        .port(8080)
        .multithreaded()
        .run();
    
}