#include "ServicesManager.h"

#include <charconv>
#include <optional>
#include <utility>

void ServicesManager::registerService(std::unique_ptr<AbstractService> service)
{
    // TODO Question 3
    services_.push_back(std::move(service));
}

AbstractService* ServicesManager::findService(std::string_view url) const
{
    // TODO Question 3
    for (const auto& service : services_) {
        const std::string_view pattern = service->getPattern();

        if (url == pattern ||
            (url.size() > pattern.size() &&
             url.starts_with(pattern) &&
             url[pattern.size()] == '/')) {
            return service.get();
        }
    }

    return nullptr;
}

http::status ServicesManager::queryService(std::string& out, const std::string& in,
                                           std::string_view url, http::verb method)
{
    // TODO Questions 1, 3, 4, 5, 7, 9, 11

    auto* service = findService(url);
    if (service == nullptr) {
        throw ServiceException(http::status::not_found, "Service not found");
    }

    std::optional<int> id;
    std::string_view idText = url.substr(service->getPattern().size());

    if (!idText.empty()) {
        idText.remove_prefix(1);

        if (!idText.empty()) { 
            int value{};
            const char* end = idText.data() + idText.size();
            const auto [ptr, error] = std::from_chars(idText.data(), end, value);

            if (error != std::errc{} || ptr != end) {
                throw ServiceException(http::status::bad_request, "Invalid resource id");
            }
            id = value;
        }
    }

    if (!in.empty()) {
        try {
            json::value parsedIn = json::parse(in);
            if (!parsedIn.is_object()) {
                throw ServiceException(http::status::bad_request, "Input JSON must be an object");
            }
        } catch (const std::exception& e) {
            throw ServiceException(http::status::bad_request, "Invalid JSON input");
        }
    }

    json::value result;
    http::status status;

    switch (method) {
        case http::verb::get:
            status = service->get(result, id);
            break;
        case http::verb::post:
        {
            if (id.has_value()) {
                throw ServiceException(http::status::method_not_allowed, "POST method does not accept an id");
            }
            if (in.empty()) {
                throw ServiceException(http::status::bad_request, "POST method requires a request body");
            }
            boost::system::error_code code;
            json::value parsedIn = json::parse(in, code);
            if (code) {
                throw ServiceException(http::status::bad_request, "Invalid JSON input");
            }
            status = service->post(result, parsedIn);
            break;
        }
        case http::verb::patch:{
            if (!id.has_value()) {
                throw ServiceException(http::status::method_not_allowed, "PATCH method requires an id");
            }
            if (in.empty()) {
                throw ServiceException(http::status::bad_request, "PATCH method requires a request body");
            }
            boost::system::error_code code;
            json::value parsedIn = json::parse(in, code);
            if (code) {
                throw ServiceException(http::status::bad_request, "Invalid JSON input");
            }
            status = service->patch(id, parsedIn);
            break;
        }
        case http::verb::delete_:
            if (!id.has_value()) {
                throw ServiceException(http::status::method_not_allowed, "DELETE method requires an id");
            }
            status = service->remove(id);
            break;
        case http::verb::put:
            throw ServiceException(http::status::not_implemented, "PUT method is not supported");
            
        default:
            throw ServiceException(http::status::method_not_allowed, "Method not allowed");
    }

    out = json::serialize(result);
    return status;
    
}
