#include <iostream>
#include <string>
#include <cstdint>
#include <cstring>
#include "haac.h"

using namespace std;

int main() {
    int *x = new int;
    char name[] = "../1.aac";
    int cap = haac_get_capacity(name, x);
    cout << "cap: " << cap << " logic_bits: " << *x << endl;

    auto *msg = new uint8_t[cap];
    auto *cost = new double[cap * 8];

    memset(msg, 0,cap);
    int cost_cap = haac_get_cost(name, msg, cost, cap);

    cout << "cost_cap: " << cost_cap << endl;

    for (int i = 0; i < 10; i++)
        cout << int(msg[i]) << ' ';
    cout << endl;

    for (int i = 0; i < 10; i++)
        cout << cost[i] << ' ';
    cout << endl;

    memset(msg, 0xff, cap);
    char out_name[] = "../2.aac";
    int emb_cap = haac_embed(name, out_name, msg, cap);
    cout << "emb_cap: " << emb_cap << endl;

    memset(msg, 0x00, cap);
    int ext_cap = haac_extract(out_name, msg, cap);
    cout << "ext_cap: " << ext_cap << endl;
    for (int i = 0; i < 100; i++)
        if (msg[i] != msg[0])
            cout << i << ":"  << int(msg[i]) << '\n';
    cout << endl;
//
//    memset(msg, 0, cap);
//    int ext_cap_ori = haac_extract(name, msg, cap);
//    cout << "ext_cap_old: " << ext_cap_ori << endl;
//    for (int i = 0; i < 100; i++)
//        cout << int(msg[i]) << ' ';
//    cout << endl;

    delete[]msg;
    delete[]cost;
    return 0;
}
