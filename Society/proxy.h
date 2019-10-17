//
//  proxy.h
//  Society
//
//  Created by Arthur Sun on 10/14/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef proxy_h
#define proxy_h

struct proxy {
    size_t hash;
    obj* obj;
        
    proxy(struct obj* obj) : obj(obj) {}
};

struct proxy_set {
    std::vector<proxy*> proxies;
    
    ~proxy_set() {
        for(proxy* p : proxies)
            delete p;
    }
    
    void sort() {
        std::sort(proxies.begin(), proxies.end(), [] (const proxy* a, const proxy* b) {
            return a->hash < b->hash;
        });
    }
    
    void add(proxy *proxy) {
        proxies.push_back(proxy);
    }
};

#endif /* proxy_h */
