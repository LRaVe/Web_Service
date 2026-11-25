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
    (void)out; (void)in;
    throw ServiceException(http::status::not_implemented, "Not implemented yet");
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
