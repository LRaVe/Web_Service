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

    json::value result;
    http::status status;

    switch (method) {
        case http::verb::get:
            status = service->get(result, id);
            break;
        case http::verb::post:
            throw ServiceException(http::status::not_implemented, "Method not implemented yet");
        case http::verb::patch:
            throw ServiceException(http::status::not_implemented, "Method not implemented yet");
        case http::verb::put:
            throw ServiceException(http::status::not_implemented, "Method not implemented yet");
        default:
            throw ServiceException(http::status::method_not_allowed, "Method not allowed");
    }

    out = json::serialize(result);
    return status;
    
}
