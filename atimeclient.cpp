#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "./httplib.h"
#include <iostream>

using namespace std;

int main() {
    // HTTP
    printf("starting\n");


    // HTTPS
    httplib::Client cli("https://worker-plain-mouse-fcd5.bxmedia.workers.dev");
    
    cli.set_default_headers({
        {"User-Agent", "Mozilla Bx/Chrome-2.2"}
    });

    auto res = cli.Get("/");
    if (res) {
        // cout << "Status code: " << res->status << endl;
        cout << "Response body: " << res->body << endl;
    } else {
        cerr << "Failed to make request." << endl;
    }

    return 0;
}
