-- wrk Lua script: POST with rotating client IDs (simulates multi-tenant traffic)
counter = 0
wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"
request = function()
counter = counter + 1
local client_id = "client_" .. (counter % 1000)
wrk.body = '{"clientId":"' .. client_id .. '"}'
return wrk.format(nil, "/api/v1/rate-limit")
end
