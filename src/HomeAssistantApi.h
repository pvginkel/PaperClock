#pragma once

class HomeAssistantApi {
    string _address;
    string _client_id;

public:
    HomeAssistantApi(string address, string client_id) : _address(std::move(address)), _client_id(std::move(client_id)) {}
};
