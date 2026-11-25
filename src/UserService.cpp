#include "UserService.h"

UserService::UserService(UserDB& userDB) : AbstractService("/user"), userDB_(userDB) {}

http::status UserService::get(json::value& out, std::optional<int> id) const
{
    // TODO Question 6 (et Question 13 pour GET /user)
    if (id.has_value()) {
        const User* user = userDB_.getUser(id.value());
        if (user == nullptr) {
            throw ServiceException(http::status::not_found, "User not found");
        }
        out = json::value_from(*user);
    } else {
        out = json::value_from(userDB_.getUsers());
    }
    return http::status::ok;
}

http::status UserService::post(json::value& out, const json::value& in)
{
    // TODO Question 8
    if(!in.is_object()) {
        throw ServiceException(http::status::bad_request, "Invalid input");
    }
    const auto& obj = in.as_object();
    if(!obj.contains("name") || !obj.contains("age")) {
        throw ServiceException(http::status::bad_request, "Missing required fields");
    }
    const auto* name = obj.if_contains("name");
    const auto* age = obj.if_contains("age");

    if(name == nullptr || age == nullptr || !name->is_string() || !age->is_int64()|| age->as_int64() < 0 || age->as_int64() > 150) {
        throw ServiceException(http::status::bad_request, "Invalid field types");
    }

    const auto nameValue = name->as_string();
    const auto ageValue = static_cast<int>(age->as_int64());

    User user{std::string(nameValue.data(), nameValue.size()), ageValue};
    const int id = userDB_.addUser(std::move(user));
    out = json::object({{"id", id}});
    return http::status::created;
}

http::status UserService::patch(std::optional<int> id, const json::value& in)
{
    // TODO Question 10
    (void)id; (void)in;
    throw ServiceException(http::status::not_implemented, "Not implemented yet");
}

http::status UserService::remove(std::optional<int> id)
{
    // TODO Question 12
    (void)id;
    throw ServiceException(http::status::not_implemented, "Not implemented yet");
}
