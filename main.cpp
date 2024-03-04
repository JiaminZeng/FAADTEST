#include <iostream>
#include <string>
#include <cstdint>
#include <cstring>
#include "haac.h"

using namespace std;

int main() {
    int *x = new int;
    char name[] = "../1.aac";
    char out_name[] = "../2.aac";

    int cap = haac_get_capacity(name, x);
    cout << "cap: " << cap << " logic_bits: " << *x << endl;

    auto *msg = new uint8_t[cap];
    auto *cost = new double[cap * 8];

    memset(msg, 0, cap);
    int cost_cap = haac_get_cost(name, msg, cost, cap);

    cout << "cost_cap: " << cost_cap << endl;

    for (int i = 0; i < 10; i++)
        cout << int(msg[i]) << ' ';
    cout << endl;

    for (int i = 0; i < 10; i++)
        cout << cost[i] << ' ';
    cout << endl;

    for (int i = 0; i < cap; i++)
        msg[i] = i % 123;
    int emb_cap = haac_embed(name, out_name, msg, cap);
    cout << "emb_cap: " << emb_cap << endl;

    // cap: 16119 logic_bits: 184219

    memset(msg, 0x00, cap);
    int ext_cap = haac_extract(out_name, msg, cap);
    cout << "ext_cap: " << ext_cap << endl;

    int num = 0;
    for (int i = 0; i < cap; i++)
        if (msg[i] != i % 123) {
            num += 1;
        }
    cout << num << endl;

    cap = haac_get_capacity(out_name, x);
    cout << "cap: " << cap << " logic_bits: " << *x << endl;

    delete[]msg;
    delete[]cost;
    return 0;
}
