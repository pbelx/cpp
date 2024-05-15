#include <iostream>
#include "./httplib.h"
#include "./json.hpp"

using namespace std;
using json = nlohmann::json;

struct Reply {
    std::string planType;
    std::string sessionKey;
    // Add other fields as needed
};

const std::string serverAddress = "http://10.10.10.10";
const std::string username = "2221314";
const std::string password = "3d7f4f7dd21d2293bd2d8500c507ad2c0414a156c6ea6f1ds44bdc50cf2fd44b82";

bool performLogin(const std::string& username, const std::string& password, Reply& reply) {
    httplib::Client cli(serverAddress.c_str());

    json loginBody = {
        {"servedMSISDN", username},
        {"password", password}
    };

    auto loginRes = cli.Post("/billing/user_signin/", loginBody.dump(), "application/json");
    cout << "login\n";
    if (loginRes && loginRes->status == 200) {
        json loginResult = json::parse(loginRes->body);
        reply.sessionKey = loginResult["sessionKey"];
          cout << "session\n";
        return true;
    } else {
        cerr << "Login request failed, error code: " << (loginRes ? loginRes->status : -1) << endl;
        return false;
    }
}

bool getUserBillingUsage(const std::string& sessionKey, const std::string& userId, json& usageResult) {
    httplib::Client cli(serverAddress.c_str());
    httplib::Headers headers = {
        {"SESSIONID", sessionKey}
    };

    auto usageRes = cli.Get("/billing/user_billing_usage/" + userId + "/3", headers);
      cout << "usage\n";

    if (usageRes && usageRes->status == 200) {
        usageResult = json::parse(usageRes->body);
        return true;
    } else {
        cerr << "User billing usage request failed, error code: " << (usageRes ? usageRes->status : -1) << endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <userId>" << endl;
        return 1;
    }

    const std::string userId = argv[1];

    Reply reply;
    if (performLogin(username, password, reply)) {
        json usageResult;
        if (getUserBillingUsage(reply.sessionKey, userId, usageResult)) {
            if (usageResult["status"] == "success") {
                json userBillingUsage = usageResult["userBillingUsage"];
                double dataCurrentUsage = userBillingUsage["dataUsage"]["currentUsage"];
                std::string userName = userBillingUsage["userName"];

                cout << "Data Current Usage: " << dataCurrentUsage << endl;
                cout << "User Name: " << userName << endl;

                if (userBillingUsage["dataUsage"].find("perRecordUsage") != userBillingUsage["dataUsage"].end()) {
                    auto& perRecordUsage = userBillingUsage["dataUsage"]["perRecordUsage"];
                    if (!perRecordUsage.empty()) {
                        auto& perRecord = perRecordUsage[0];
                        if (!perRecord["nLeftUnits"].is_null()) {
                            double nLeftUnits = perRecord["nLeftUnits"];
                            double nLeftUnitsGB = nLeftUnits / (1024 * 1024 * 1024);
                            cout << "Balance: " << nLeftUnitsGB << "GB"<< endl;
                        } else {
                            cerr << "nLeftUnits is null in the JSON response" << endl;
                        }
                    } else {
                        cerr << "perRecordUsage array is empty in the JSON response" << endl;
                    }
                } else {
                    cerr << "perRecordUsage key not found in the JSON response" << endl;
                }
            } else {
                cerr << "User billing usage request failed, status: " << usageResult["status"] << endl;
            }
        }
    }

    return 0;
}
