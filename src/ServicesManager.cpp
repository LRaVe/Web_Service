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
    (void)in;

    auto* service = findService(url);
    if (service == nullptr) {
        throw ServiceException(http::status::not_found, "Service not found");
    }

    json::value result;
    http::status status;

    switch (method) {
        case http::verb::get:
            status = service->get(result, std::nullopt);
            break;
        default:
            throw ServiceException(http::status::method_not_allowed, "Method not allowed");
    }

    out = json::serialize(result);
    return http::status::ok;
    
}
