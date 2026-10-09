#include "cache/cache.h"

using namespace std;


Cache::Cache(
    int ttl_seconds
)
    : ttl_(ttl_seconds) {
}


bool Cache::get(
    const string& key,
    string& value
) {

    lock_guard<mutex> lock(
        mutex_
    );

    auto iterator =
        data_.find(key);

    if (
        iterator == data_.end()
    ) {

        return false;
    }

    if (
        chrono::steady_clock::now()
        >= iterator->second.expiry
    ) {

        data_.erase(iterator);

        return false;
    }

    value =
        iterator->second.response;

    return true;
}


void Cache::put(
    const string& key,
    const string& value
) {

    lock_guard<mutex> lock(
        mutex_
    );

    data_[key] = {

        value,

        chrono::steady_clock::now()
        + chrono::seconds(ttl_)
    };
}