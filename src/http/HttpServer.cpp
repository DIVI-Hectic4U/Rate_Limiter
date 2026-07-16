#include "../../include/http/HttpServer.h"
#include <iostream>

HttpServer::HttpServer(IRateLimiter& limiter)
    : limiter_(limiter), app_(std::make_unique<crow::SimpleApp>())
{
    setupRoutes();
}

HttpServer::~HttpServer() = default;

void HttpServer::setupRoutes()
{
    // We use -> because app_ is now a secure std::unique_ptr
    CROW_ROUTE((*app_), "/api/v1/rate-limit").methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) { 
        auto body = crow::json::load(req.body);
        
        if (!body || !body.has("clientId"))
        {
            return crow::response(400, "Invalid JSON: Missing 'clientId' field\n");
        }

        std::string clientId = body["clientId"].s();
        bool allowed = limiter_.allowRequest(clientId);

        crow::response res;
        res.add_header("X-RateLimit-Allowed", allowed ? "true" : "false");

        if (allowed)
        {
            res.code = 200;
            res.body = "Request Allowed for client: " + clientId + "\n";
        }
        else
        {
            res.code = 429; // HTTP 429: Too Many Requests
            res.body = "Rate Limit Exceeded for client: " + clientId + "\n";
        }

        return res;
    });
}

void HttpServer::start(int port)
{
    std::cout << "[HttpServer] Starting server on port " << port << "...\n";
    app_->port(port).multithreaded().run();
}